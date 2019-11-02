#-*- coding: UTF-8 -*-
import sys
sys.path.append('../scintilla/scripts')
import re
from enum import IntFlag
from FileGenerator import Regenerate

# see EditLexer.h
class KeywordAttr(IntFlag):
	Default = 0
	NoLexer = 1
	MakeLower = 2
	NoAutoComp = 4

def MakeKeywordGroups(items, maxLineLength=120, prefixLen=1):
	items = list(sorted(items))
	groups = {}
	for item in items:
		if item.endswith('()'):
			# ')' is not used by lexer or auto-completion:
			# 1. use InListPrefixed(s, '(') in lexer to match the word
			# 2. ')' is auto added in WordList_AddListEx()
			item = item[:-1]
		key = item[:prefixLen]
		if key in groups:
			groups[key]['items'].append(item)
			groups[key]['len'] += len(item) + 1
		else:
			groups[key] = {'items': [item], 'len': len(item) + 1}

	if prefixLen > 1:
		removed = []
		subs = []
		for key, group in groups.items():
			if group['len'] > maxLineLength:
				removed.append(key)
				sub = MakeKeywordGroups(group['items'], maxLineLength, prefixLen + 1)
				subs.append(sub)
		for key in removed:
			del groups[key]
		for sub in subs:
			groups.update(sub)
		groups = dict(sorted(groups.items()))
	return groups

def MakeKeywordLines(items, maxLineLength=120, prefixLen=1):
	if not items:
		return []
	groups = MakeKeywordGroups(items, maxLineLength, prefixLen)
	groups = list(groups.values())
	count = len(groups)
	lines = []
	index = 0
	while index < count:
		group = groups[index]
		index += 1
		lineLen = group['len']
		items = group['items']
		if lineLen > maxLineLength and prefixLen == 1:
			sub = MakeKeywordLines(items, maxLineLength, 2)
			lines.extend(sub)
		else:
			while index < count:
				group = groups[index]
				lineLen += group['len']
				if lineLen > maxLineLength:
					break
				index += 1
				items.extend(group['items'])
			lines.append(' '.join(items))

	return lines

def RemoveDuplicateKeyword(keywordMap, orderedKeys):
	unique = set()
	for key in orderedKeys:
		items = set(keywordMap[key])
		if not unique:
			unique |= items
		else:
			items -= unique
			unique |= items
		keywordMap[key] = items

def BuildKeywordContent(keywordList, keywordCount=16):
	output = []
	for index, item in enumerate(keywordList):
		comment, items, attr = item
		lines = MakeKeywordLines(items)
		if index != 0:
			output.append(", // %d %s" % (index, comment))
		if lines:
			output.extend('"' + line + ' "' for line in lines)
		else:
			output.append('NULL')
		if index + 1 < keywordCount:
			output.append("")

	count = keywordCount - len(keywordList)
	if count:
		output.append(", NULL" * count)
	return output

def UpdateKeywordFile(path, keywordList):
	output = BuildKeywordContent(keywordList)
	Regenerate(path, '//', output)

def read_api_file(path, comment):
	doc = open(path, encoding='utf-8').read()
	doc = re.sub(comment + r'[^!].+', '', doc) # normal comment
	sections = []
	items = doc.split(comment + "!") #! section name
	for section in items:
		lines = section.strip().splitlines()
		if not lines:
			continue

		key = lines[0].replace('=', '').strip().lower()
		doc = '\n'.join(lines[1:])
		sections.append((key, doc))
	return sections


# CMake
def parse_cmake_api_file(path):
	# languages from https://gitlab.kitware.com/cmake/cmake/blob/master/Auxiliary/vim/extract-upper-case.pl
	cmakeLang = "C CSharp CUDA CXX Fortran Java RC Swift".split()

	def is_long_name(name):
		return '_' in name or '-' in name

	def expand_name(name, expanded):
		# TODO: expand <CONFIG> and others
		if '<LANG>' in name:
			items = []
			for lang in cmakeLang:
				item = name.replace('<LANG>', lang)
				if item in expanded:
					# only valid for specific languages
					items = []
					break
				items.append(item)
			return items
		else:
			return [name]

	def get_prefix(name):
		if '<' in name:
			# not full expanded
			if name.count('<') == 1 and name[-1] == '>':
				# ignore suffix
				name = name[:name.index('<')].strip('_')
				return name, True
			else:
				return '', False
		return name, is_long_name(name)

	sections = read_api_file(path, '#')
	keywordMap = {}
	parameters = set()
	needExpand = {}
	for key, doc in sections:
		if key in ('keywords', 'commands'):
			items = re.findall(r'(?P<name>\w+)\s*\((?P<params>.*?)\)', doc, re.MULTILINE | re.DOTALL)
			commands = set()
			for item in items:
				commands.add(item[0] + '()')
				parameters.update(item[1].split())
			keywordMap[key] = commands
		elif key == 'generator expressions':
			items = re.findall(r'\$\<(?P<name>[\w\-]+)[:>]', doc, re.MULTILINE)
			parameters.update(items)
		elif key == 'values':
			keywordMap[key] = doc.split()
		else:
			items = doc.split()
			result = set()
			ignores = set()
			pending = set()
			for item in items:
				if '<' in item:
					pending.add(item)
				elif is_long_name(item):
					ignores.add(item)
				else:
					result.add(item)
			keywordMap[key] = result
			keywordMap['long ' + key] = ignores
			needExpand[key] = (pending, result | ignores)

	for key, value in needExpand.items():
		pending, expanded = value
		result = set()
		ignores = set()
		items = set()
		for item in pending:
			items.update(expand_name(item, expanded))
		for item in items:
			name, ignore = get_prefix(item)
			if name and name not in expanded:
				if ignore:
					ignores.add(name)
				else:
					result.add(name)

		keywordMap[key] |= result
		keywordMap['long ' + key] |= ignores

	keywordMap['parameters'] = parameters
	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'commands',
		'variables',
		'parameters',
		'properties',
		'values',
		'long properties',
		'long variables',
	])
	keywordList = [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('commands', keywordMap['commands'], KeywordAttr.Default),
		('parameters', keywordMap['parameters'], KeywordAttr.Default),
		('properties', keywordMap['properties'], KeywordAttr.Default),
		('variables', keywordMap['variables'], KeywordAttr.Default),
		('values', keywordMap['values'], KeywordAttr.Default),
		#('long properties', keywordMap['long properties'], KeywordAttr.NoLexer),
		#('long variables', keywordMap['long variables'], KeywordAttr.NoLexer),
		('long properties', [], KeywordAttr.NoLexer),
		('long variables', [], KeywordAttr.NoLexer),
	]
	return keywordList

def update_cmake_keyword():
	keywordList = parse_cmake_api_file('lang/CMake.cmake')
	UpdateKeywordFile('../src/EditLexers/stlCMake.c', keywordList)

# Kotlin
def parse_kotlin_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key == 'kdoc':
			items = re.findall(r'@(\w+)', doc)
		elif key == 'library':
			items = re.findall(r'annotation\s+class\s+(\w+)', doc)
			annotations = set(items)

			items = re.findall(r'enum\s+class\s+(\w+)', doc)
			enums = set(items)

			items = re.findall(r'class\s+(\w+)', doc)
			classes = set(items)
			classes -= annotations
			classes -= enums

			items = re.findall(r'object\s+(\w+)', doc)
			classes.update(items)

			items = re.findall(r'interface\s+(\w+)', doc)
			interfaces = set(items)

			keywordMap['class'] = classes
			keywordMap['interface'] = interfaces
			keywordMap['enum'] = enums
			keywordMap['annotation'] = annotations

			items = re.findall(r'fun\s+.*?(\w+\()', doc, re.DOTALL)
			keywordMap['function'] = items
		else:
			items = doc.split()
		keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'class',
		'interface',
		'enum',
		'annotation',
	])
	keywordList = [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('class', keywordMap['class'], KeywordAttr.Default),
		('interface', keywordMap['interface'], KeywordAttr.Default),
		('enum', keywordMap['enum'], KeywordAttr.Default),
		('annotation', keywordMap['annotation'], KeywordAttr.NoLexer),
		('function', keywordMap['function'], KeywordAttr.NoLexer),
		#('KDoc', keywordMap['kdoc'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp),
		('KDoc', [], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp),
	]
	return keywordList

def update_kotlin_keyword():
	keywordList = parse_kotlin_api_file('lang/Kotlin.kt')
	UpdateKeywordFile('../src/EditLexers/stlKotlin.c', keywordList)

# Rust
def parse_rust_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	used = set()
	for key, doc in sections:
		if key in ('keywords', 'reserved keywords', 'primitive types', 'macros'):
			items = set(doc.split())
			keywordMap[key] = items
			used |= items
		elif key == 'attribute':
			items = re.findall(r"(\w+\(?)", doc)
			attributes = set()
			for item in items:
				if len(item) == 1:
					continue
				if item[-1] == '(':
					attributes.add(item + ')')
				else:
					attributes.add(item)
			keywordMap[key] = list(attributes)
		elif key == 'modules':
			items = re.findall(r'mod\s+([\w:]+)', doc)
			modules = set()
			for item in items:
				modules.update(item.replace(':', ' ').split())
			modules -= used
			keywordMap[key] = modules

			items = re.findall(r'struct\s+(\w+)', doc)
			structs = set(items)

			items = re.findall(r'trait\s+(\w+)', doc)
			traits = set(items)

			items = re.findall(r'union\s+(\w+)', doc)
			unions = set(items)

			enums = set()
			items = re.findall(r"enum\s+(?P<name>\w+)\s*(([^\r\n]*?\{(?P<def>.+?)\})|;)", doc, re.MULTILINE | re.DOTALL)
			for item in items:
				definition = re.findall(r'^\s+(\w+)[,\(]', item[1], re.MULTILINE)
				enums.add(item[0])
				enums.update(definition)

			types = keywordMap['primitive types']
			items = re.findall(r'type\s+(\w+).*?=\s*(\w+)', doc)
			for alias, name in items:
				if name in structs:
					structs.add(alias)
				elif name in traits:
					traits.add(alias)
				elif name in enums:
					enums.add(alias)
				elif name in unions:
					unions.add(alias)
				elif name in types:
					types.add(alias)

			keywordMap['struct'] = structs
			keywordMap['trait'] = traits
			keywordMap['enumeration'] = enums
			keywordMap['union'] = unions

			items = re.findall(r'^\s*const\s+(\w+)\s*:', doc, re.MULTILINE)
			keywordMap['constant'] = set(items)

			items = re.findall(r'fn\s+(\w+)', doc)
			items = set(items)
			items = [item + '()' for item in items if item not in used]
			keywordMap['function'] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'reserved keywords',
		'primitive types',
		'struct',
		'trait',
		'enumeration',
		'union',
		'constant',
	])
	keywordList = [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('reserved keywords', keywordMap['reserved keywords'], KeywordAttr.NoAutoComp),
		('primitive types', keywordMap['primitive types'], KeywordAttr.Default),
		('struct', keywordMap['struct'], KeywordAttr.Default),
		('trait', keywordMap['trait'], KeywordAttr.Default),
		('enumeration', keywordMap['enumeration'], KeywordAttr.Default),
		('union', keywordMap['union'], KeywordAttr.Default),
		('constant', keywordMap['constant'], KeywordAttr.Default),
		('attribute', keywordMap['attribute'], KeywordAttr.NoLexer),
		('macro', keywordMap['macros'], KeywordAttr.NoLexer),
		('module', keywordMap['modules'], KeywordAttr.NoLexer),
		('function', keywordMap['function'], KeywordAttr.NoLexer),
	]
	return keywordList

def update_rust_keyword():
	keywordList = parse_rust_api_file('lang/Rust.rs')
	UpdateKeywordFile('../src/EditLexers/stlRust.c', keywordList)

# Vim
def parse_vim_api_file(path):
	sections = read_api_file(path, '"')
	keywordMap = {}
	for key, doc in sections:
		items = doc.split()
		keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'commands',
	])
	keywordList = [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('commands', keywordMap['commands'], KeywordAttr.Default),
	]
	return keywordList

def update_vim_keyword():
	keywordList = parse_vim_api_file('lang/Vim.vim')
	UpdateKeywordFile('../src/EditLexers/stlVim.c', keywordList)

# update all keywords in order
def update_all_keyword():
	update_cmake_keyword()
	update_kotlin_keyword()
	update_rust_keyword()
	update_vim_keyword()
