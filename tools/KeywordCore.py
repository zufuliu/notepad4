import sys
import os.path
import re
#from collections import Counter
from enum import IntFlag
import string

sys.path.append('../scintilla/scripts')
from FileGenerator import Regenerate
from LexerConfig import *

SinglyWordMap = {
	'class': 'class',
	'classes': 'class',
	'properties': 'property',
}

AllKeywordAttrList = {}
# for keyword list used in AutoC_AddSpecWord()
SpecialKeywordIndexList = {}
LexerKeywordIndexList = {}
# X11 and SVG color names
ColorNameList = set()
JavaKeywordMap = {}
JavaScriptKeywordMap = {}
GroovyKeyword = []

def MakeKeywordGroups(items, maxLineLength=120, prefixLen=1):
	items = sorted(items)
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

def to_lower(items):
	return [item.lower() for item in items]

def build_enum_name(comment):
	items = [item.replace('-', '') for item in comment.split()]
	item = items[-1]
	if item[-1] == 's':
		if item in SinglyWordMap:
			singly = SinglyWordMap[item]
		else:
			singly = item[:-1]
			SinglyWordMap[item] = singly
		items[-1] = singly
	return ''.join(item.title() for item in items)

def BuildKeywordContent(rid, lexer, keywordList, keywordCount=16):
	output = []
	attrList = []
	indexList = LexerKeywordIndexList.setdefault(lexer, {})
	prefix = lexer[3:-4] + 'KeywordIndex_'
	for index, item in enumerate(keywordList):
		comment, items, attr = item
		lines = MakeKeywordLines(set(items))
		if index != 0:
			output.append(f", // {index} {comment}")
		if lines:
			output.extend('"' + line + ' "' for line in lines)
		else:
			output.append('NULL')
		if index + 1 < keywordCount:
			output.append("")

		indexName = build_enum_name(comment)
		# keyword index for lexer
		if (attr & KeywordAttr.NoLexer) == 0 and comment != 'unused':
			if indexName in indexList:
				assert index == indexList[indexName][0], (rid, lexer, comment)
			else:
				indexList[indexName] = (index, rid)
		# keyword index for smart auto-completion
		if attr & KeywordAttr.Special:
			attr &= ~KeywordAttr.Special
			group = SpecialKeywordIndexList.setdefault(prefix, {})
			if indexName in group:
				assert index == group[indexName], (rid, lexer, comment)
			else:
				group[indexName] = index
		# keyword attribute for lexer
		if lines and (attr & KeywordAttr.NoLexer) == 0:
			attr |= KeywordAttr.PreSorted
			if attr & KeywordAttr.MakeLower:
				result = sorted(to_lower(items))
				if set(result) == set(items):
					attr &= ~KeywordAttr.MakeLower
				second = to_lower(sorted(items))
				if second != result:
					attr &= ~KeywordAttr.PreSorted
		if attr != KeywordAttr.Default:
			attrList.append((index, attr, comment))

	count = keywordCount - len(keywordList)
	if count:
		output.append(", NULL" * count)
	if attrList:
		AllKeywordAttrList[rid] = attrList
	return output, attrList

def UpdateKeywordFile(rid, path, lexer, keywordList, keywordCount=16, suffix=''):
	attrList = []
	if keywordList:
		output, attrList = BuildKeywordContent(rid, lexer, keywordList, keywordCount=keywordCount)
		if len(output) > 1:
			Regenerate(path, '//' + suffix, output)

	output = BuildLexerConfigContent(rid, attrList)
	if output:
		suffix = (suffix + ' Settings').strip()
		Regenerate(path, '//' + suffix, output)

def UpdateLexerEnumFile(path, lexerPath):
	output = dump_enum_flag(LexerAttr, as_shift=True)
	output.append('')
	output.extend(dump_enum_flag(KeywordAttr, max_value=KeywordAttr.Special))
	Regenerate(path, '//Lexer Enum', output)

	output = dump_enum_flag(KeywordAttr, indent='\t', anonymous=False, max_value=KeywordAttr.NoLexer)
	Regenerate(lexerPath, '//', output)

def UpdateAutoCompletionCache(path):
	commentLine, commentBlock, scriptShebang = BuildLexerCommentString()
	Regenerate(path, '//CommentLine', commentLine)
	Regenerate(path, '//CommentBlock', commentBlock)
	Regenerate(path, '//ScriptShebang', scriptShebang)

	cache = BuildAutoCompletionCache()
	Regenerate(path, '//Cache', cache)


def split_api_section(doc, comment, commentKind=0):
	if commentKind == 0:
		doc = re.sub(comment + r'[^!].+', '', doc) # normal comment
	sections = []
	items = doc.split(comment + "!") #! section name
	for section in items:
		lines = section.strip().splitlines()
		if commentKind == 1:
			result = []
			for line in lines:
				index = line.find(comment)
				if index < 0:
					result.append(line)
					continue
				if index > 0:
					line = line[:index].strip()
					if line:
						result.append(line)
			lines = result
		if not lines:
			continue

		key = lines[0].replace('=', '').strip().lower()
		doc = '\n'.join(lines[1:])
		sections.append((key, doc))
	return sections

def read_file(path):
	with open(path, encoding='utf-8') as fd:
		return fd.read()

def read_api_file(path, comment, commentKind=0):
	doc = read_file(path)
	return split_api_section(doc, comment, commentKind=commentKind)

def has_upper_char(s):
	return any(ch.isupper() for ch in s)

def to_upper_conditional(items):
	result = []
	for item in items:
		if has_upper_char(item):
			result.append(item)
		else:
			result.append(item.upper())
	return result

def to_lower_conditional(items):
	result = []
	for item in items:
		if sum(ch.isupper() for ch in item) > 1:
			result.append(item)
		else:
			result.append(item.lower())
	return result


def parse_actionscript_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key in ('keywords', 'types', 'directive'):
			keywordMap[key] = doc.split()
		if key == 'class':
			items = re.findall(r'class\s+(\w+)', doc)
			keywordMap[key] = items
		elif key == 'functions':
			items = re.findall(r'function\s+(\w+\()', doc)
			keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'directive',
		'keywords',
		'types',
		'class',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('types', keywordMap['types'], KeywordAttr.Default),
		('directive', keywordMap['directive'], KeywordAttr.Default),
		('class', keywordMap['class'], KeywordAttr.Default),
		('interface', [], KeywordAttr.Default),
		('enumeration', [], KeywordAttr.Default),
		('constant', [], KeywordAttr.Default),
		('metadata', [], KeywordAttr.Default),
		('function', keywordMap['functions'], KeywordAttr.NoLexer),
		('properties', [], KeywordAttr.NoLexer),
		('doc tag', [], KeywordAttr.NoLexer),
	]

def parse_autohotkey_api_file(pathList):
	keywordMap = {}
	for path in pathList:
		sections = read_api_file(path, ';')
		for key, doc in sections:
			if key in ('keywords', 'built-in variables', 'keys', 'misc'):
				items = doc.split()
			elif key == 'flow of control':
				key = 'keywords'
				items = re.findall(r'^\s*(\w+)', doc, re.MULTILINE)
			elif key == 'directives':
				items = re.findall(r'#(\w+)', doc)
			elif key == 'script compiler directives':
				items = re.findall(r'@([\w\-]+)', doc)
			elif key == 'functions':
				items = re.findall(r'^\s*(\w+\(?)', doc, re.MULTILINE)
				functions = []
				for item in items:
					if item.endswith('('):
						functions.append(item + ')')
					else:
						functions.append(item)
				items = functions
			elif key == 'objects':
				items = doc.replace('.', ' ').split()
			keywordMap.setdefault(key, []).extend(items)

	keywordMap['keywords'].extend(to_lower_conditional(keywordMap['keywords']))
	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'objects',
		'built-in variables',
		'keys',
		'misc',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.MakeLower),
		('directives', keywordMap['directives'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('compiler directives', keywordMap['script compiler directives'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('objects', keywordMap['objects'], KeywordAttr.MakeLower),
		('built-in variables', keywordMap['built-in variables'], KeywordAttr.MakeLower),
		('keys', keywordMap['keys'], KeywordAttr.MakeLower),
		('functions', keywordMap['functions'], KeywordAttr.MakeLower),
		('misc', keywordMap['misc'], KeywordAttr.NoLexer),
	]

def parse_apdl_api_file(path):
	ext = os.path.splitext(path)[1].lower()
	comment = '!' if ext == '.cdb' else '**'
	sections = read_api_file(path, comment, 1)
	keywordMap = {}
	for key, doc in sections:
		key = key.strip('*! ')
		if key == 'function':
			items = re.findall(r'(\w+\()', doc)
		else:
			items = doc.split()
			if key in ('code folding', 'slash command', 'star command'):
				items = [item.lstrip('*/') for item in items]
		keywordMap[key] = items

	keywordMap['star command'].extend(keywordMap['code folding']) # for auto-completion
	return [
		('code folding', keywordMap['code folding'], KeywordAttr.Default),
		('command', keywordMap['command'], KeywordAttr.Default),
		('slash command', keywordMap['slash command'], KeywordAttr.Special),
		('star command', keywordMap['star command'], KeywordAttr.Special),
		('argument', keywordMap['argument'], KeywordAttr.Default),
		('function', keywordMap['function'], KeywordAttr.Default),
	]

def parse_asymptote_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key == 'functions':
			items = re.findall(r'(\w+\()', doc)
			items = [item for item in items if len(item) > 2]
		else:
			items = doc.split()
		keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'types',
		'structs',
		'constants',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('types', keywordMap['types'], KeywordAttr.Default),
		('structs', keywordMap['structs'], KeywordAttr.Default),
		('constants', keywordMap['constants'], KeywordAttr.Default),
		('functions', keywordMap['functions'], KeywordAttr.NoLexer),
	]

def parse_avisynth_api_file(path):
	sections = read_api_file(path, '#')
	keywordMap = {}
	for key, doc in sections:
		if key == 'keywords':
			items = doc.split()
			items.extend([item.lower() for item in items])
			keywordMap[key] = items
		elif key == 'functions':
			items = re.findall(r'^(\w+\s+)?(\w+\()', doc, re.MULTILINE)
			functions = [item[1] for item in items if 'global' not in item[0]]
			keywordMap['functions'] = functions
			items = re.findall(r'^global\s+(\w+\(?)', doc, re.MULTILINE)
			keywordMap['options'] = items
		elif key == 'properties':
			items = re.findall(r'clip\.(\w+\(?)', doc)
			properties = []
			functions = []
			for item in items:
				if item[-1] == '(':
					functions.append(item)
				else:
					properties.append(item)
			items = re.findall(r'^\w+\s+(\w+\(?)', doc, re.MULTILINE)
			for item in items:
				if item != 'clip':
					if item[-1] == '(':
						functions.append(item)
					else:
						properties.append(item)
			keywordMap[key] = properties
			keywordMap['functions'].extend(functions)
		elif key in ('filters', 'plugins'):
			items = re.findall(r'^(\w+\()', doc, re.MULTILINE)
			keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'functions',
		'filters',
		'plugins',
		'properties',
		'options',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('internal functions', keywordMap['functions'], KeywordAttr.MakeLower),
		('internal filters', keywordMap['filters'], KeywordAttr.MakeLower),
		('external filters', keywordMap['plugins'], KeywordAttr.MakeLower),
		('properties', keywordMap['properties'], KeywordAttr.MakeLower),
		('options', keywordMap['options'], KeywordAttr.NoLexer),
	]

def parse_awk_api_file(path):
	sections = read_api_file(path, '#')
	keywordMap = {}
	for key, doc in sections:
		if key in ('keywords', 'predefined variables', 'misc'):
			keywordMap[key] = doc.split()
		elif key in ('built-in functions', 'library functions'):
			items = re.findall(r'(\w+)\s*\(', doc)
			keywordMap[key] = [item + '(' for item in items]

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'predefined variables',
		'built-in functions',
		'library functions',
		'misc',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('predefined variable', keywordMap['predefined variables'], KeywordAttr.Default),
		('built-in function', keywordMap['built-in functions'], KeywordAttr.Default),
		('library function', keywordMap['library functions'], KeywordAttr.NoLexer),
		('misc', keywordMap['misc'], KeywordAttr.NoLexer),
	]

def parse_batch_api_file(path):
	sections = read_api_file(path, '::')
	keywordMap = {
		'upper case keywords': [],
		'options': [],
	}
	for key, doc in sections:
		if key in ('keywords', 'internal command'):
			items = doc.split()
			keywordMap['upper case keywords'].extend(items)
			keywordMap[key] = [item.lower() for item in items]
		elif key == 'external command':
			uppercase = keywordMap['upper case keywords']
			options = keywordMap['options']
			commands = []
			for line in doc.splitlines():
				items = line.strip().split()
				if not items:
					continue
				command = items[0]
				if command[0].isupper():
					uppercase.append(command)
				elif command[0].islower():
					commands.append(command)
				if len(items) > 1:
					options.extend(item for item in items[1:] if item[0].isalpha())
			keywordMap['system commands'] = commands
		elif key == 'environment variables':
			keywordMap[key] = doc.split()
		elif key == 'options':
			keywordMap[key].extend(doc.split())

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'internal command',
		'system commands',
		'upper case keywords',
		'environment variables',
		'options',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('internal commands', keywordMap['internal command'], KeywordAttr.Default),
		('system commands', keywordMap['system commands'], KeywordAttr.NoLexer),
		('upper case keywords / commands', keywordMap['upper case keywords'], KeywordAttr.NoLexer),
		('environment variables', keywordMap['environment variables'], KeywordAttr.NoLexer),
		('command options', keywordMap['options'], KeywordAttr.NoLexer),
	]

def parse_coffeescript_api_file(path):
	sections = read_api_file(path, '#')
	keywordMap = {}
	for key, doc in sections:
		keywordMap[key] = doc.split()

	keywordMap['directive'] = JavaScriptKeywordMap['directive']
	keywordMap['class'] = JavaScriptKeywordMap['class']
	RemoveDuplicateKeyword(keywordMap, [
		'directive',
		'keywords',
		'reserved words',
		'class',
	])

	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('reserved words', keywordMap['reserved words'], KeywordAttr.Default),
		('directive', keywordMap['directive'], KeywordAttr.Default),
		('class', keywordMap['class'], KeywordAttr.Default),
	]

def parse_cmake_api_file(path):
	# languages from https://gitlab.kitware.com/cmake/cmake/blob/master/Auxiliary/vim/extract-upper-case.pl
	cmakeLang = "ASM C CSharp CUDA CXX Fortran Java RC Swift".split()

	def is_long_name(name):
		count = name.count('_') + name.count('-')
		return count != 0

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
		return [name]

	def get_prefix(name):
		if '<' in name:
			# not full expanded
			if name.count('<') == 1 and name[-1] == '>':
				# ignore suffix
				name = name[:name.index('<')].strip('_')
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

	#counter = Counter(item.split('_')[0] for item in keywordMap['long properties'])
	#print('CMake long properties prefix:', counter)
	#counter = Counter(item.split('_')[0] for item in keywordMap['long variables'])
	#print('CMake long variables prefix:', counter)

	return [
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

def parse_cpp_api_file(pathList):
	return [
		('keywords', [], KeywordAttr.Default),
		('type keyword', [], KeywordAttr.Default),
		('preprocessor', [], KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('directive', [], KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('attribute', [], KeywordAttr.Default),
		('class', [], KeywordAttr.Default),
		('interface', [], KeywordAttr.Default),
		('enumeration', [], KeywordAttr.Default),
		('constant', [], KeywordAttr.Default),
		('2nd keyword', [], KeywordAttr.Default),
		('2nd type keyword', [], KeywordAttr.Default),
		('asm intruction', [], KeywordAttr.NoAutoComp),
		('asm register', [], KeywordAttr.NoAutoComp),
		('function', [], KeywordAttr.NoLexer),
		('C++ method', [], KeywordAttr.NoLexer),
	]

def parse_csharp_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key in ('keywords', 'types', 'vala types'):
			keywordMap[key] = doc.split()
		elif key == 'preprocessor':
			items = re.findall(r'#(\w+)', doc)
			keywordMap[key] = items
		elif key == 'api':
			items = re.findall(r'class\s+(\w+)', doc)
			keywordMap['class'] = items
			items = re.findall(r'struct\s+(\w+)', doc)
			keywordMap['struct'] = items
			items = re.findall(r'interface\s+(\w+)', doc)
			keywordMap['interface'] = items
			items = re.findall(r'enum\s+(\w+)', doc)
			keywordMap['enumeration'] = items
			items = re.findall(r'\[(\w+)', doc)
			keywordMap['attributes'] = items
			items = re.findall(r'delegate\s+\w+\s+(\w+)', doc)
			keywordMap['class'].extend(items)
		elif key == 'comment':
			items = re.findall(r'<(\w+)', doc)
			keywordMap['comment tag'] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'types',
		'class',
		'struct',
		'interface',
		'enumeration',
		'vala types',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('types', keywordMap['types'], KeywordAttr.Default),
		('vala types', keywordMap['vala types'], KeywordAttr.NoAutoComp),
		('preprocessor', keywordMap['preprocessor'], KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('attributes', keywordMap['attributes'], KeywordAttr.Default),
		('class', keywordMap['class'], KeywordAttr.Default),
		('struct', keywordMap['struct'], KeywordAttr.Default),
		('interface', keywordMap['interface'], KeywordAttr.Default),
		('enumeration', keywordMap['enumeration'], KeywordAttr.Default),
		('constant', [], KeywordAttr.Default),
		('comment tag', keywordMap['comment tag'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
	]

def parse_css_api_file(pathList):
	# https://developer.mozilla.org/en-US/docs/Glossary/Vendor_Prefix
	# custom property https://www.w3.org/TR/css-variables-1/
	vendor = '^-moz- ^-ms- ^-o- ^-webkit-'.split()
	keywordMap = {
		'properties': vendor + ['^--'],
		'at rules': vendor[:],
		'pseudo classes': vendor[:],
		'pseudo elements': vendor,
	}

	values = []
	for path in pathList:
		for line in read_file(path).splitlines():
			line = line.strip()
			if not line or line.startswith('//'):
				continue
			if line[0] == '@':
				rule = line.split()[0][1:]
				keywordMap['at rules'].append(rule)
			elif line[0] == '!':
				values.append(line[1:])
			elif line[0] == ':':
				line = line.rstrip(')')
				if line[1] == ':':
					keywordMap['pseudo elements'].append(line[2:])
				else:
					keywordMap['pseudo classes'].append(line[1:])
			elif line[0].isalpha():
				line = re.sub(r'\(.*?\)', '(', line)
				index = line.find(':')
				if index > 0:
					name = line[:index].strip()
					keywordMap['properties'].append(name)
				line = re.sub(r'[!:;,.|\[\])]', ' ', line)
				items = line.split()
				values.extend(items)

	items = []
	for value in keywordMap.values():
		items.extend(value)
	keywordMap['values'] = set(values) - ColorNameList - set(items)
	return [
		('properties', keywordMap['properties'], KeywordAttr.Default),
		('at rules', keywordMap['at rules'], KeywordAttr.Special),
		('pseudo classes', keywordMap['pseudo classes'], KeywordAttr.Special),
		('pseudo elements', keywordMap['pseudo elements'], KeywordAttr.Special),
		('color names', ColorNameList, KeywordAttr.NoLexer),
		('values', keywordMap['values'], KeywordAttr.NoLexer),
	]

def parse_dlang_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key == 'library':
			keywordMap['class'] = re.findall(r'class\s+(\w+)', doc)
			keywordMap['struct'] = re.findall(r'struct\s+(\w+)', doc)
			keywordMap['union'] = re.findall(r'union\s+(\w+)', doc)
			keywordMap['interface'] = re.findall(r'interface\s+(\w+)', doc)
			keywordMap['enumeration'] = re.findall(r'enum\s+(\w+)', doc)
		else:
			items = doc.split()
			if key in ('preprocessor', 'attribute'):
				items = [item[1:] for item in items]
			keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'types',
		'class',
		'struct',
		'union',
		'interface',
		'trait',
		'enumeration',
		'constant',
	])

	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('types', keywordMap['types'], KeywordAttr.Default),
		('preprocessor', keywordMap['preprocessor'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('attribute', keywordMap['attribute'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('class', keywordMap['class'], KeywordAttr.Default),
		('struct', keywordMap['struct'], KeywordAttr.Default),
		('union', keywordMap['union'], KeywordAttr.Default),
		('interface', keywordMap['interface'], KeywordAttr.Default),
		('trait', keywordMap['trait'], KeywordAttr.Default),
		('enumeration', keywordMap['enumeration'], KeywordAttr.Default),
		('constant', keywordMap['constant'], KeywordAttr.Default),
		('asm keywords', keywordMap['asm keywords'], KeywordAttr.NoAutoComp),
		('asm register', keywordMap['asm register'], KeywordAttr.NoAutoComp),
		('asm instruction', keywordMap['asm instruction'], KeywordAttr.NoAutoComp),
		('function', [], KeywordAttr.NoLexer),
	]

def parse_dart_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key in ('keywords', 'types'):
			items = doc.split()
			keywordMap[key] = items
		elif key == 'libraries':
			items = re.findall(r'(class|typedef)\s+(\w+)', doc)
			keywordMap['class'] = [item[1] for item in items]

			items = re.findall(r'(enum)\s+(\w+)', doc)
			keywordMap['enumeration'] = [item[1] for item in items]

			items = re.findall(r'@(\w+\(?)', doc)
			keywordMap['metadata'] = items

			items = re.findall(r'\s\w+\(', doc)
			keywordMap['function'] = [item.strip() for item in items]

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'types',
		'class',
		'enumeration',
		'metadata',
		'function',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('types', keywordMap['types'], KeywordAttr.Default),
		('class', keywordMap['class'], KeywordAttr.Default),
		('enumeration', keywordMap['enumeration'], KeywordAttr.Default),
		('metadata', keywordMap['metadata'], KeywordAttr.NoLexer | KeywordAttr.Special),
		('function', keywordMap['function'], KeywordAttr.NoLexer),
	]

def parse_gn_api_file(path):
	sections = read_api_file(path, '#')
	keywordMap = {}
	for key, doc in sections:
		if key == 'function':
			items = re.findall(r'^(\w+\()', doc, re.MULTILINE)
		else:
			items = doc.split()
		keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'predefined variables',
		'target variables',
		'placeholders',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('function', keywordMap['function'], KeywordAttr.Default),
		('predefined variables', keywordMap['predefined variables'], KeywordAttr.Default),
		('target variables', keywordMap['target variables'], KeywordAttr.NoLexer),
		('placeholders', keywordMap['placeholders'], KeywordAttr.NoLexer),
	]

def parse_go_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key in ('keywords', 'types'):
			items = doc.split()
			if key == 'types':
				keywordMap['primitive types'] = items
			else:
				keywordMap[key] = items
		else:
			items = re.findall(r'^\s*(func\s+)?(?P<name>\w+\()', doc, re.MULTILINE)
			items = set(item[1] for item in items)
			if key == 'builtin':
				keywordMap['builtin functions'] = items
			else:
				keywordMap['function'] = items

			types = set()
			structs = set()
			interfaces = set()
			items = re.findall(r'type\s+(?P<name>\w+)(\s+(?P<kind>\w+))?', doc)
			for name, _, kind in items:
				if kind == 'struct':
					structs.add(name)
				elif kind == 'interface':
					interfaces.add(name)
				else:
					types.add(name)

			keywordMap['type'] = types
			keywordMap['struct'] = structs
			keywordMap['interface'] = interfaces

			items = re.findall(r'const\s+(\w+)', doc)
			constant = set(items)
			items = re.findall(r'const\s+\((?P<def>[^()]+)\)', doc, re.MULTILINE)
			for item in items:
				items = re.findall(r'^\s+(\w+)', item, re.MULTILINE)
				constant.update(items)
			keywordMap['constant'] = constant

			items = re.findall(r'var\s+(\w+)', doc)
			variables = set(items)
			items = re.findall(r'var\s+\((?P<def>[^()]+)\)', doc, re.MULTILINE)
			for item in items:
				items = re.findall(r'^\s+(\w+)', item, re.MULTILINE)
				variables.update(items)
			keywordMap['variables'] = variables

			items = re.findall(r'package\s+([/\w]+)', doc)
			packages = set()
			for item in items:
				packages.update(item.split('/'))
			keywordMap['package'] = set(packages)

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'primitive types',
		'builtin functions',
		'type',
		'struct',
		'interface',
		'constant',
		'variables',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('primitive types', keywordMap['primitive types'], KeywordAttr.Default),
		('builtin functions', keywordMap['builtin functions'], KeywordAttr.Default),
		('type', keywordMap['type'], KeywordAttr.Default),
		('struct', keywordMap['struct'], KeywordAttr.Default),
		('interface', keywordMap['interface'], KeywordAttr.Default),
		('constant', keywordMap['constant'], KeywordAttr.Default),
		('variables', keywordMap['variables'], KeywordAttr.NoLexer),
		('function', keywordMap['function'], KeywordAttr.NoLexer),
		('package', keywordMap['package'], KeywordAttr.NoLexer),
	]

def parse_gradle_api_file(path):
	sections = read_api_file(path, '//')
	functions = []
	for key, doc in sections:
		if key == 'api':
			# properties
			items = re.findall(r'([a-z]\w+)$', doc, re.MULTILINE)
			functions.extend(items)
			# methods
			items = re.findall(r'([a-z]\w+)\s*\(', doc)
			functions.extend(items)
			# script block
			items = re.findall(r'^\s*([a-z]\w+)\s*\{', doc, re.MULTILINE)
			functions.extend([item + '^{}' for item in items])

	return [
		('keywords', GroovyKeyword, KeywordAttr.Default),
		('types', JavaKeywordMap['types'], KeywordAttr.Default),
		('unused', [], KeywordAttr.Default),
		('class', JavaKeywordMap['class'], KeywordAttr.Default),
		('interface', JavaKeywordMap['interface'], KeywordAttr.Default),
		('enumeration', JavaKeywordMap['enumeration'], KeywordAttr.Default),
		('constant', [], KeywordAttr.Default),
		('annotation', JavaKeywordMap['annotation'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp),
		('function', functions, KeywordAttr.NoLexer),
		('GroovyDoc', JavaKeywordMap['javadoc'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp),
	]

def parse_graphviz_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {
		'attributes': [],
		'values': [],
		'labels': [],
	}
	for key, doc in sections:
		if key in ('keywords', 'node shapes'):
			keywordMap[key] = doc.split()
		elif key == 'attributes':
			attributes = []
			values = []
			doc = doc.replace('[', '').replace(']', '')
			for line in doc.splitlines():
				line = line.strip()
				if not line:
					continue
				items = line.split('=', 2)
				attributes.append(items[0].strip())
				if len(items) > 1:
					items = [item for item in items[1].replace(',', ' ').split() if item]
					values.extend(items)
			keywordMap['attributes'].extend(attributes)
			keywordMap['values'].extend(values)
		elif key == 'color names':
			items = [item for item in doc.split() if not item[-1] in string.digits]
			ColorNameList.update(items)
		elif key == 'labels':
			labels = re.findall(r'<(\w+)', doc)
			keywordMap[key].extend(labels)
			attributes = re.findall(r'([\w\-]+)=', doc)
			keywordMap['attributes'].extend(attributes)
			values = re.findall(r'\W([A-Z]+)', doc)
			values = set(values) - set(labels) - set(attributes)
			keywordMap['values'].extend(values)

	keywordMap['color names'] = ColorNameList
	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'color names',
		'attributes',
		'node shapes',
		'values',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('html labels', keywordMap['labels'], KeywordAttr.NoLexer | KeywordAttr.Special),
		('attributes', keywordMap['attributes'], KeywordAttr.NoLexer),
		('node shapes', keywordMap['node shapes'], KeywordAttr.NoLexer),
		('color names', ColorNameList, KeywordAttr.NoLexer),
		('values', keywordMap['values'], KeywordAttr.NoLexer),
	]

def parse_groovy_api_file(path):
	sections = read_api_file(path, '//')
	keywords = []
	for key, doc in sections:
		if key == 'keywords':
			keywords = doc.split()

	keywords.extend(JavaKeywordMap['keywords'])
	GroovyKeyword.extend(keywords)
	return [
		('keywords', keywords, KeywordAttr.Default),
		('types', JavaKeywordMap['types'], KeywordAttr.Default),
		('unused', [], KeywordAttr.Default),
		('class', JavaKeywordMap['class'], KeywordAttr.Default),
		('interface', JavaKeywordMap['interface'], KeywordAttr.Default),
		('enumeration', JavaKeywordMap['enumeration'], KeywordAttr.Default),
		('constant', [], KeywordAttr.Default),
		('annotation', JavaKeywordMap['annotation'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('function', [], KeywordAttr.NoLexer),
		('GroovyDoc', JavaKeywordMap['javadoc'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
	]

def parse_haxe_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key in ('keywords', 'preprocessor'):
			items = [item.strip('#') for item in doc.split()]
			keywordMap[key] = items
		elif key == 'library':
			items = re.findall(r'(abstract|class|typedef)\s+(\w+)', doc)
			classes = [item[1] for item in items]
			interfaces = re.findall(r'interface\s+(\w+)', doc)
			items = re.findall(r'enum\s+(abstract\s+)?(\w+)', doc)
			enums = [item[1] for item in items]

			keywordMap['class'] = classes
			keywordMap['interface'] = interfaces
			keywordMap['enumeration'] = enums
		elif key == 'comment':
			items = re.findall(r'@(\w+)', doc)
			keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'enumeration',
		'class',
		'interface',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('preprocessor', keywordMap['preprocessor'], KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('class', keywordMap['class'], KeywordAttr.Default),
		('interface', keywordMap['interface'], KeywordAttr.Default),
		('enumeration', keywordMap['enumeration'], KeywordAttr.Default),
		('constant', [], KeywordAttr.Default),
		('metadata', [], KeywordAttr.Default),
		('function', [], KeywordAttr.Default),
		('comment tag', keywordMap['comment'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
	]

def parse_html_api_file(path):
	return [
		('tag', [], KeywordAttr.Special),
		('JavaScript', [], KeywordAttr.NoAutoComp),
		('VBScript', [], KeywordAttr.MakeLower | KeywordAttr.NoAutoComp),
		('Python', [], KeywordAttr.Default),
		('PHP', [], KeywordAttr.Default),
		('SGML', [], KeywordAttr.Default),
		('attribute', [], KeywordAttr.Special),
		('event handler', [], KeywordAttr.Special),
		('value', [], KeywordAttr.NoLexer | KeywordAttr.Special),
	]

def parse_inno_setup_api_file(path):
	section = ''
	lines = []
	sections = []
	doc = read_file(path)
	for line in doc.splitlines():
		line = line.strip()
		if not line:
			continue
		if line[0] == '[':
			if section and lines:
				doc = '\n'.join(lines)
				sections.append((section, doc))
			section = line.strip('[]').strip()
			lines = []
		else:
			lines.append(line)
	if section and lines:
		doc = '\n'.join(lines)
		sections.append((section, doc))

	keywordMap = {
		'section': [],
		'parameters': [],
		'constants': [],

		'keywords': [],
		'directives': [],
		'types': [],
		'predefined variables': [],
		'functions': [],

		'pascal keywords': [],
		'pascal types': ['IDispatch', 'IUnknown'],
		'pascal functions': [],
		'pascal constants': [],
		'misc': ['Local'],
	}
	for section, content in sections:
		if section == 'ISPP':
			for key, doc in split_api_section(content, '//'):
				if key == 'directives':
					items = re.findall(r'^#(\w+)', doc, re.MULTILINE)
				elif key in ('keywords', 'types', 'predefined variables'):
					items = doc.split()
				elif key == 'functions':
					items = re.findall(r'(\w+)\(', doc)
					items = [item + '()' for item in items]
				else:
					items = []
				keywordMap[key].extend(items)
		elif section in ('Constants', 'Misc'):
			items = re.findall(r'^\{?(\w+)', content, re.MULTILINE)
			keywordMap[section.lower()].extend(items)
		elif section == 'Code':
			keywordMap['section'].append(section)
			for key, doc in split_api_section(content, '//'):
				items = []
				if key == 'keywords':
					items = doc.split()
				elif key == 'constants':
					items = doc.replace(',', ' ').split()
				elif key in ('event', 'functions', 'classes'):
					items = re.findall(r'^\s*(function|procedure|constructor|property)\s+(\w+\(?)', doc, re.MULTILINE | re.IGNORECASE)
					functions = []
					properties = []
					for kind, name in items:
						if key == 'event' or kind.lower() == 'property':
							properties.append(name)
						elif not name.endswith('('):
							functions.append(name)
						else:
							functions.append(name + ')')

					types = re.findall(r':\s*(\w+)', doc)
					if 'array' in types:
						types.remove('array')
					if key == 'classes':
						items = re.findall(r'^(T\w+)\s*=', doc, re.MULTILINE)
						types.extend(items)
						items = re.findall(r'^T\w+\s*=\s*\(([\w\s,]+)\)', doc, re.MULTILINE)
						for item in items:
							keywordMap['pascal constants'].extend(item.replace(',', ' ').split())
					keywordMap['misc'].extend(properties)
					if key == 'functions':
						items = functions
					else:
						key = 'types'
						items = types
				keywordMap['pascal ' + key].extend(items)
		else:
			keywordMap['section'].append(section)
			items = re.findall(r'^(\w+)=?:?', content, re.MULTILINE)
			keywordMap['parameters'].extend(items)

	RemoveDuplicateKeyword(keywordMap, [
		'pascal keywords',
		'pascal types',
		'functions',
		'pascal functions',
		'predefined variables',
		'pascal constants',
		'misc',
	])
	return [
		('section', keywordMap['section'], KeywordAttr.NoLexer),
		('parameters', keywordMap['parameters'], KeywordAttr.NoLexer),
		('constants', keywordMap['constants'], KeywordAttr.NoLexer),

		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('directives', keywordMap['directives'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('types', keywordMap['types'], KeywordAttr.Default),
		('predefined variables', keywordMap['predefined variables'], KeywordAttr.MakeLower),
		('functions', keywordMap['functions'], KeywordAttr.NoLexer),

		('pascal keywords', keywordMap['pascal keywords'], KeywordAttr.MakeLower),
		('pascal types', keywordMap['pascal types'], KeywordAttr.MakeLower),
		('pascal functions', keywordMap['pascal functions'], KeywordAttr.NoLexer),
		('pascal constants', keywordMap['pascal constants'], KeywordAttr.MakeLower),
		('misc', keywordMap['misc'], KeywordAttr.NoLexer),
	]

def parse_jam_api_file(path):
	sections = read_api_file(path, '#')
	keywordMap = {}
	for key, doc in sections:
		if key in ('keywords', 'constants', 'features'):
			keywordMap[key] = doc.split()
		elif key in ('rules', 'modules', 'classes'):
			rules = re.findall(r'rule\s+([\w\-]+)', doc)
			if key == 'rules':
				keywordMap['builtin rules'] = rules
			else:
				keywordMap.setdefault('rules', []).extend(rules)
				modules = re.findall(r'module\s+([\w\-]+)', doc)
				if modules:
					keywordMap['modules'] = modules
				classes = re.findall(r'class\s+([\w-]+)', doc)
				if classes:
					keywordMap['classes'] = classes

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'modules',
		'classes',
		'builtin rules',
		'rules',
		'features',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('module', keywordMap['modules'], KeywordAttr.Default),
		('class', keywordMap['classes'], KeywordAttr.Default),
		('builtin rule', keywordMap['builtin rules'], KeywordAttr.Default),
		('constant', keywordMap['constants'], KeywordAttr.Default),
		('rule', keywordMap['rules'], KeywordAttr.NoLexer),
		('feature', keywordMap['features'], KeywordAttr.NoLexer),
	]

def parse_java_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key in ('keywords', 'types', 'directive'):
			keywordMap[key] = doc.split()
		elif key == 'api':
			classes = re.findall(r'class\s+(\w+)', doc)
			interfaces = re.findall(r'interface\s+(\w+)', doc)
			enumeration = re.findall(r'enum\s+(\w+)', doc)
			annotations = re.findall(r'@(\w+)', doc)

			keywordMap['class'] = classes
			keywordMap['interface'] = interfaces
			keywordMap['enumeration'] = enumeration
			keywordMap['annotation'] = annotations
		elif key == 'javadoc':
			items = re.findall(r'@(\w+)', doc)
			keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'types',
		'directive',
		'class',
		'interface',
		'enumeration',
	])

	JavaKeywordMap.update(keywordMap)
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('types', keywordMap['types'], KeywordAttr.Default),
		('directive', keywordMap['directive'], KeywordAttr.Default),
		('class', keywordMap['class'], KeywordAttr.Default),
		('interface', keywordMap['interface'], KeywordAttr.Default),
		('enumeration', keywordMap['enumeration'], KeywordAttr.Default),
		('constant', [], KeywordAttr.Default),
		('annotation', keywordMap['annotation'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('function', [], KeywordAttr.Default),
		('Javadoc', keywordMap['javadoc'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
	]

def parse_javascript_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key in ('keywords', 'future reserved words'):
			keywordMap[key] = set(doc.split())
		elif key == 'directive':
			items = set()
			for item in doc.split():
				index = item.find('(')
				if index > 0:
					item = item[:index]
				items.add(item)
			keywordMap[key] = items
		elif key == 'jsdoc':
			items = re.findall(r'@(\w+)', doc)
			keywordMap[key] = items
		elif key == 'api':
			classes = set(['JSON', 'jQuery'])
			constant = set()
			functions = set()
			properties = set(['URL'])

			items = re.findall(r'^\s*(\w+)(.?)', doc, re.MULTILINE)
			for item, kind in items:
				if item[0].isupper():
					if any(ch.islower() for ch in item):
						classes.add(item)
					elif kind == '(':
						functions.add(item + '(')
					else:
						constant.add(item)
				else:
					if kind == '(':
						functions.add(item + '(')
					else:
						properties.add(item)

			items = ['function', 'require', 'import']
			for item in items:
				functions.add(item + '(')

			keywordMap['class'] = classes
			keywordMap['constant'] = constant
			keywordMap['function'] = functions
			keywordMap['properties'] = properties

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'future reserved words',
		'class',
		'properties',
		'constant',
	])

	JavaScriptKeywordMap.update(keywordMap)
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('future reserved words', keywordMap['future reserved words'], KeywordAttr.Default),
		('directive', keywordMap['directive'], KeywordAttr.Default),
		('class', keywordMap['class'], KeywordAttr.Default),
		('interface', [], KeywordAttr.Default),
		('enumeration', [], KeywordAttr.Default),
		('constant', keywordMap['constant'], KeywordAttr.Default),
		('decorator', [], KeywordAttr.Special),
		('function', keywordMap['function'], KeywordAttr.NoLexer),
		('properties', keywordMap['properties'], KeywordAttr.NoLexer),
		('JSDoc', keywordMap['jsdoc'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
	]

def parse_json_api_file(path):
	keywords = 'Infinity NaN false null true'.split()
	return [
		('keywords', keywords, KeywordAttr.Default),
	]

def parse_julia_api_file(path):
	sections = read_api_file(path, '#')
	keywordMap = {}
	for key, doc in sections:
		if key in ('core', 'modules'):
			items = re.findall(r'^\s*module\s+([\w.]+)', doc, re.MULTILINE)
			modules = []
			for item in items:
				modules.extend(item.split('.'))
			keywordMap.setdefault('module', set()).update(modules)

			items = re.findall(r'^\s+const\s+(\w+)', doc, re.MULTILINE)
			keywordMap.setdefault('constant', set()).update(items)

			items = re.findall(r'^\s+([A-Z]\w+)', doc, re.MULTILINE)
			keywordMap.setdefault('type', set()).update(items)

			items = re.findall(r'^\s+@([a-z]\w+)', doc, re.MULTILINE)
			keywordMap.setdefault('macro', set()).update(items)

			items = re.findall(r'^\s+([a-z]\w+!?\()', doc, re.MULTILINE)
			if key == 'core':
				keywordMap['basic function'] = set(items)
			else:
				keywordMap['function'] = set(items)
		else:
			keywordMap[key] = set(doc.split())

	folding = keywordMap['code folding']
	keywordMap['keywords'] |= folding
	folding.remove('end')
	folding.add('type')

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'constant',
		'type',
		'module',
		'function',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('code folding', folding, KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('type', keywordMap['type'], KeywordAttr.Default),
		('constant', keywordMap['constant'], KeywordAttr.Default),
		('basic function', keywordMap['basic function'], KeywordAttr.Default),
		('module', keywordMap['module'], KeywordAttr.NoLexer),
		('macro', keywordMap['macro'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('function', keywordMap['function'], KeywordAttr.NoLexer),
	]

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

			interfaces = re.findall(r'interface\s+(\w+)', doc)

			keywordMap['class'] = classes
			keywordMap['interface'] = interfaces
			keywordMap['enumeration'] = enums
			keywordMap['annotation'] = annotations

			items = re.findall(r'fun\s+.*?(\w+\()', doc, re.DOTALL)
			keywordMap['function'] = items
		else:
			items = doc.split()
		keywordMap[key] = items

	if True:
		# for JVM target
		keywordMap['class'].update(JavaKeywordMap['class'])
		keywordMap['interface'].extend(JavaKeywordMap['interface'])
		keywordMap['enumeration'].update(JavaKeywordMap['enumeration'])
		keywordMap['annotation'].update(JavaKeywordMap['annotation'])

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'class',
		'interface',
		'enumeration',
		'annotation',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('class', keywordMap['class'], KeywordAttr.Default),
		('interface', keywordMap['interface'], KeywordAttr.Default),
		('enumeration', keywordMap['enumeration'], KeywordAttr.Default),
		('annotation', keywordMap['annotation'], KeywordAttr.NoLexer | KeywordAttr.Special),
		('function', keywordMap['function'], KeywordAttr.NoLexer),
		('KDoc', keywordMap['kdoc'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
	]

def parse_lua_api_file(path):
	sections = read_api_file(path, '--')
	keywordMap = {}
	constant = []
	for key, doc in sections:
		if key in ('keywords', 'metamethod'):
			keywordMap[key] = doc.split()
		else:
			items = re.findall(r'^\s*(\w+\.?:?\w*\(?)', doc, re.MULTILINE)
			modules = []
			functions = []
			for item in items:
				index = item.find(':')
				if index > 0:
					# functions for file handle
					functions.append(item[index + 1:])
					continue

				functions.append(item)
				index = item.find('.')
				if index >= 0:
					module = item[:index]
					modules.append(module)
					if module == 'string':
						# string functions in object-oriented style
						functions.append(item[index + 1:])

			keywordMap[key] = functions
			keywordMap.setdefault('basic function', []).extend(modules)

			items = re.findall(r'^"([\w\s]+)"', doc, re.MULTILINE)
			for item in items:
				constant.extend(item.split())

	keywordMap.setdefault('library', []).extend(constant)
	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'basic function',
		'metamethod',
		'library',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('basic function', keywordMap['basic function'], KeywordAttr.Default),
		('metamethod', keywordMap['metamethod'], KeywordAttr.Default),
		('standard library', keywordMap['library'], KeywordAttr.NoLexer),
	]

def parse_llvm_api_file(path):
	sections = read_api_file(path, ';')
	keywordMap = {}
	for key, doc in sections:
		if key in ('keywords', 'type'):
			keywordMap[key] = doc.split()
		elif key == 'attribute':
			items = re.findall(r'^\s*([\w\-]+\(?)', doc, re.MULTILINE)
			keywordMap[key] = items
		elif key == 'instruction':
			items = re.findall(r'^\s*(\w+)', doc, re.MULTILINE)
			keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'type',
		'attribute',
		'instruction',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('type', keywordMap['type'], KeywordAttr.Default),
		('attribute', keywordMap['attribute'], KeywordAttr.Default),
		('instruction', keywordMap['instruction'], KeywordAttr.Default),
	]

def parse_markdown_api_file(path):
	blockTag = ['pre', 'script', 'style', 'textarea'] # type 1
	type6 = "address, article, aside, base, basefont, blockquote, body, caption, center, col, colgroup, dd, details, dialog, dir, div, dl, dt, fieldset, figcaption, figure, footer, form, frame, frameset, h1, h2, h3, h4, h5, h6, head, header, hr, html, iframe, legend, li, link, main, menu, menuitem, nav, noframes, ol, optgroup, option, p, param, section, source, summary, table, tbody, td, tfoot, th, thead, title, tr, track, ul"
	blockTag.extend(type6.replace(',', ' ').split())
	return [
		('html block tag', blockTag, KeywordAttr.NoAutoComp),
	]

def parse_nsis_api_file(path):
	keywordMap = {}
	sections = read_api_file(path, ';')
	for key, doc in sections:
		items = doc.split()
		if key == 'preprocessor':
			items = [item.strip('!') for item in items]
		elif key == 'functions':
			functions = []
			for item in items:
				item = item.strip('.')
				if item.startswith('un.'):
					item = item[3:]
				functions.append(item)
			items = functions
		keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'instructions',
		'attributes',
		'functions',
		'predefined variables',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.MakeLower),
		('preprocessor', keywordMap['preprocessor'], KeywordAttr.NoLexer),
		('instruction', keywordMap['instructions'], KeywordAttr.NoLexer),
		('attribute', keywordMap['attributes'], KeywordAttr.NoLexer),
		('function', keywordMap['functions'], KeywordAttr.NoLexer),
		('predefined variables', keywordMap['predefined variables'], KeywordAttr.NoLexer),
	]

def parse_php_api_file(path):
	keywordMap = {}
	sections = read_api_file(path, '//')
	for key, doc in sections:
		if key == 'keywords':
			keywordMap[key] = [item.replace('()', '(') for item in doc.split()]
		elif key == 'type':
			keywordMap[key] = doc.split()
		elif key == 'predefined variable':
			items = doc.split()
			keywordMap[key] = [item for item in items if item[0] == '$']
			keywordMap['misc'] = [item for item in items if item[0].isalpha()]
		elif key == 'api':
			items = re.findall(r'\w+\(', doc)
			keywordMap['magic method'] = [item for item in items if item.startswith('__')]
			keywordMap['function'] = items
			keywordMap['class'] = re.findall(r'\Wclass\s+(\w+)', doc)
			keywordMap['interface'] = re.findall(r'\Winterface\s+(\w+)', doc)
			items = re.findall(r'^\s*([_A-Z0-9]+)\s*$', doc, re.MULTILINE)
			keywordMap['magic constant'] = [item for item in items if item.startswith('__')]
			keywordMap['constant'] = items
			# field
			items = re.findall(r'\Wconst\s+\w+\s+([_A-Z0-9]+)', doc)
			keywordMap['constant'].extend(items)
			items = re.findall(r'(public|protected)\s+[\w\?\|]+\s+\$(\w+)', doc)
			keywordMap['misc'].extend(item[1] for item in items)
		elif key == 'phpdoc':
			keywordMap[key] = re.findall(r'@(\w+)', doc)

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'type',
		'class',
		'interface',
		'magic constant',
		'magic method',
		'constant',
		'function',
		'misc',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('type', keywordMap['type'], KeywordAttr.Default),
		('class', keywordMap['class'], KeywordAttr.MakeLower),
		('interface', keywordMap['interface'], KeywordAttr.MakeLower),
		('predefined variable', keywordMap['predefined variable'], KeywordAttr.Default),
		('magic constant', keywordMap['magic constant'], KeywordAttr.Default),
		('magic method', keywordMap['magic method'], KeywordAttr.MakeLower),
		('constant', keywordMap['constant'], KeywordAttr.NoLexer),
		('function', keywordMap['function'], KeywordAttr.NoLexer),
		('misc', keywordMap['misc'], KeywordAttr.NoLexer),
		('JavaScript', JavaScriptKeywordMap['keywords'], KeywordAttr.NoAutoComp),
		('phpdoc', keywordMap['phpdoc'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp | KeywordAttr.Special),
	]

def parse_python_api_file(path):
	keywordMap = {
		'modules': [],
		'attributes': [],
		'classes': [],
		'decorators': [],
		'special method': [],
		'functions': [],
		'fields': [],
		'misc': ['utf-8'],
	}
	sections = read_api_file(path, '#')
	for key, doc in sections:
		if key in ('keywords', 'built-in constants', 'exceptions'):
			items = doc.split()
			if key == 'exceptions':
				keywordMap['classes'].extend(items)
			else:
				keywordMap[key] = items
		elif key in ('built-in functions', 'api'):
			items = re.findall(r'@(\w+)', doc)
			keywordMap['decorators'].extend(items)
			items = re.findall(r'class\s+(\w+)', doc)
			if key == 'api':
				keywordMap['classes'].extend(items)
			else:
				keywordMap['types'] = items
			items = re.findall(r'exception\s+(\w+)', doc)
			keywordMap['classes'].extend(items)
			items = re.findall(r'(\w+)\(', doc)
			items = set(items) - set(keywordMap['classes'])
			if key == 'api':
				for item in items:
					if item.startswith('__') and item.endswith('__'):
						keywordMap['special method'].append(item + '()')
					else:
						keywordMap['functions'].append(item + '()')
			else:
				keywordMap[key] = [item + '()' for item in items]
			items = re.findall(r'(\w+)=', doc)
			keywordMap['misc'].extend(item for item in items if len(item) > 2)
			items = re.findall(r'^([\w\.]+)', doc, re.MULTILINE)
			keywordMap['modules'].extend(items)
			items = re.findall(r'^[ \t]+(\w+\(?)', doc, re.MULTILINE)
			for item in items:
				if not item.endswith('('):
					if item.startswith('__') and item.endswith('__'):
						keywordMap['attributes'].append(item)
					else:
						keywordMap['fields'].append(item)
			items = re.findall(r"""['"](.*?)['"]""", doc)
			items = ' '.join(items).replace('|', ' ').split()
			items = [item for item in items if len(item) > 2 and all(ch.isalpha() for ch in item)]
			keywordMap['misc'].extend(items)
		elif key == 'attributes':
			items = set(doc.split())
			for item in items:
				if item.endswith(')'):
					keywordMap['built-in functions'].append(item)
				else:
					keywordMap['attributes'].append(item)
		elif key == 'special method':
			items = re.findall(r'(__\w+__\(?)', doc)
			for item in items:
				if item.endswith('('):
					keywordMap[key].append(item + ')')
				else:
					keywordMap['attributes'].append(item)
			items = re.findall(r'(\w+)=', doc)
			keywordMap['misc'].extend(items)
		elif key == 'comment':
			items = re.findall(r':(\w+)', doc)
			keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'types',
		'built-in constants',
		'built-in functions',
		'attributes',
		'special method',
		'classes',
		'modules',
		'functions',
		'fields',
		'misc',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('type', keywordMap['types'], KeywordAttr.Default),
		('built-in constant', keywordMap['built-in constants'], KeywordAttr.Default),
		('built-in function', keywordMap['built-in functions'], KeywordAttr.Default),
		('attribute', keywordMap['attributes'], KeywordAttr.Default),
		('special method', keywordMap['special method'], KeywordAttr.Default),
		('class', keywordMap['classes'], KeywordAttr.Default),
		('decorator', keywordMap['decorators'], KeywordAttr.NoLexer | KeywordAttr.Special),
		('module', keywordMap['modules'], KeywordAttr.NoLexer),
		('function', keywordMap['functions'], KeywordAttr.NoLexer),
		('field', keywordMap['fields'], KeywordAttr.NoLexer),
		('misc', keywordMap['misc'], KeywordAttr.NoLexer),
		('comment tag', keywordMap['comment'], KeywordAttr.NoLexer),
	]

def parse_rlang_api_file(path):
	sections = read_api_file(path, '#')
	keywordMap = {}
	for key, doc in sections:
		if key == 'keywords':
			items = doc.split()
		else:
			result = re.findall(r'^[\w\.]+\(?', doc, re.MULTILINE)
			items = []
			for item in result:
				item = item.strip('.')
				if len(item.rstrip('(')) > 2:
					items.append(item)
		keywordMap[key] = items

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'package',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('package', keywordMap['package'], KeywordAttr.NoLexer),
	]

def parse_rebol_api_file(pathList):
	keywordMap = {}
	for path in pathList:
		sections = read_api_file(path, ';')
		for key, doc in sections:
			items = doc.split()
			if key == 'directive':
				items = [item[1:] for item in items]
			elif key == 'functions':
				items = [item for item in items if len(item.strip('?')) >= 2]
			keywordMap.setdefault(key, []).extend(items)

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'functions',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('directive', keywordMap['directive'], KeywordAttr.Special),
		('datatype', keywordMap['datatypes'], KeywordAttr.NoLexer),
		('function', keywordMap['functions'], KeywordAttr.NoLexer),
	]

def parse_resource_script_api_file(path):
	return [
		('keywords', [], KeywordAttr.Default),
		('type keyword', [], KeywordAttr.Default),
		('preprocessor', [], KeywordAttr.NoAutoComp | KeywordAttr.Special),
	]

def parse_ruby_api_file(path):
	sections = read_api_file(path, '#')
	keywordMap = {
		'module': [],
		'class': [],
		'function': [],
	}
	for key, doc in sections:
		if key in ('built-in', 'core', 'library'):
			items = re.findall(r'module\s+(\w+)', doc)
			keywordMap['module'].extend(items)
			items = re.findall(r'class\s+(\w+)', doc)
			keywordMap['class'].extend(items)
			items = re.findall(r'\sdef\s+(\w+[!?]?)', doc)
			if key == 'built-in':
				keywordMap['built-in function'] = items
			else:
				keywordMap['function'].extend(items)
		else:
			items = set(doc.split())
			keywordMap[key] = items

	folding = keywordMap['code folding']
	keywordMap['keywords'] |= folding
	folding.remove('end')
	keywordMap['regex'].remove('end')
	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'pre-defined constants',
		'module',
		'class',
		'built-in function',
		'function',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('code folding', folding, KeywordAttr.NoAutoComp),
		('regex', keywordMap['regex'], KeywordAttr.NoAutoComp),
		('pre-defined constants', keywordMap['pre-defined constants'], KeywordAttr.Default),
		('pre-defined variables', keywordMap['pre-defined variables'], KeywordAttr.NoLexer),
		('module', keywordMap['module'], KeywordAttr.Default),
		('class', keywordMap['class'], KeywordAttr.Default),
		('built-in function', keywordMap['built-in function'], KeywordAttr.Default),
		('function', keywordMap['function'], KeywordAttr.NoLexer),
	]

def parse_rust_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	used = set()
	for key, doc in sections:
		if key in ('keywords', 'reserved keywords', 'primitive types', 'macros'):
			items = doc.split()
			keywordMap[key] = items
			used.update(items)
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
			keywordMap[key] = attributes
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
					types.append(alias)

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
	return [
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

def parse_smali_api_file(path):
	return [
		('keywords', [], KeywordAttr.Default),
		('type keyword', [], KeywordAttr.Default),
		('unused', [], KeywordAttr.Default),
		('unused', [], KeywordAttr.Default),
		('unused', [], KeywordAttr.Default),
		('unused', [], KeywordAttr.Default),
		('unused', [], KeywordAttr.Default),
		('unused', [], KeywordAttr.Default),
		('constant', [], KeywordAttr.Default),
		('directive', [], KeywordAttr.Special),
		('instruction', [], KeywordAttr.Default),
	]

def parse_sql_api_file(pathList):
	keywordMap = {}
	for path in pathList:
		sections = read_api_file(path, '--')
		for key, doc in sections:
			items = []
			min_len = 2
			if key in ('keywords', 'data types'):
				items = re.findall(r'\w+', doc)
			elif key == 'functions':
				items = re.findall(r'\w+\(', doc)
				min_len = 3
			else:
				raise ValueError('unknown key:', key)
			items = [item for item in items if len(item) >= min_len]
			items = to_upper_conditional(items)
			keywordMap.setdefault(key, set()).update(items)

	upper_keywords = keywordMap['keywords']
	upper_types = keywordMap['data types']
	upper_functions = keywordMap['functions']
	upper_types -= set(['CURSOR', 'SET', 'TABLE'])
	upper_keywords -= upper_types
	upper_keywords.add('END-EXEC')

	keywords = set(to_lower(upper_keywords))
	types = set(to_lower(upper_types))
	functions = set(to_lower(upper_functions))
	keywords -= types

	ignores = set(item for item in functions if item[:-1] in keywords or item[:-1] in types)
	functions -= ignores
	upper_functions |= ignores

	return [
		('keywords', keywords, KeywordAttr.Default),
		('data types', types, KeywordAttr.Default),
		('functions', functions, KeywordAttr.Default),

		('upper case keywords', upper_keywords, KeywordAttr.NoLexer),
		('upper case data types', upper_types, KeywordAttr.NoLexer),
		('upper case functions', upper_functions, KeywordAttr.NoLexer),
	]

def parse_swift_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key == 'keywords':
			keywordMap[key] = doc.split()
		elif key == 'directive':
			items = re.findall(r'#(\w+\(?)', doc)
			keywordMap[key] = items
		elif key == 'attribute':
			items = re.findall(r'@(\w+\(?)', doc)
			keywordMap[key] = items
		elif key == 'library':
			classes = re.findall(r'class\s+(\w+)', doc)
			structs = re.findall(r'struct\s+(\w+)', doc)
			protocols = re.findall(r'protocol\s+(\w+)', doc)
			enums = re.findall(r'enum\s+(\w+)', doc)
			items = re.findall(r'func\s+(\w+)', doc)
			functions = [item + '(' for item in items]

			classes.extend(['AnyObject', 'AnyClass'])
			items = re.findall(r'typealias\s+(\w+)(\s*=\s*(\w+))?', doc)
			for alias, _, name in items:
				if name in classes:
					classes.append(alias)
				elif name in structs or name == '':
					structs.append(alias)

			keywordMap['class'] = classes
			keywordMap['struct'] = structs
			keywordMap['protocol'] = protocols
			keywordMap['enumeration'] = enums
			keywordMap['function'] = functions

	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'class',
		'struct',
		'protocol',
		'enumeration',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('directive', keywordMap['directive'], KeywordAttr.Special),
		('attribute', keywordMap['attribute'], KeywordAttr.Special),
		('class', keywordMap['class'], KeywordAttr.Default),
		('struct', keywordMap['struct'], KeywordAttr.Default),
		('protocol', keywordMap['protocol'], KeywordAttr.Default),
		('enumeration', keywordMap['enumeration'], KeywordAttr.Default),
		('function', keywordMap['function'], KeywordAttr.NoLexer),
	]

def parse_toml_api_file(path):
	keywords = 'false inf nan true'.split()
	return [
		('keywords', keywords, KeywordAttr.Default),
	]

def parse_typescript_api_file(path):
	sections = read_api_file(path, '//')
	keywordMap = {}
	for key, doc in sections:
		if key == 'tsdoc':
			items = re.findall(r'@(\w+)', doc)
			items.extend(['reference', 'amd-module', 'amd-dependency'])
			keywordMap[key] = items
		else:
			keywordMap[key] = doc.split()

	keywordMap['interface'] = JavaScriptKeywordMap['class']
	keywordMap['constant'] = JavaScriptKeywordMap['constant']
	RemoveDuplicateKeyword(keywordMap, [
		'directive',
		'keywords',
		'types',
		'interface',
		'constant',
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('types', keywordMap['types'], KeywordAttr.Default),
		('directive', keywordMap['directive'], KeywordAttr.Default),
		('class', [], KeywordAttr.Default),
		('interface', keywordMap['interface'], KeywordAttr.Default),
		('enumeration', [], KeywordAttr.Default),
		('constant', keywordMap['constant'], KeywordAttr.Default),
		('decorator', [], KeywordAttr.Default),
		('function', [], KeywordAttr.NoLexer),
		('properties', [], KeywordAttr.NoLexer),
		('TSDoc', keywordMap['tsdoc'], KeywordAttr.NoLexer | KeywordAttr.NoAutoComp),
	]

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
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('commands', keywordMap['commands'], KeywordAttr.Default),
	]

def parse_visual_basic_api_file(path):
	return [
		('keywords', [], KeywordAttr.MakeLower),
		('type keyword', [], KeywordAttr.MakeLower),
		('demoted keyword', [], KeywordAttr.MakeLower),
		('preprocessor', [], KeywordAttr.MakeLower | KeywordAttr.NoAutoComp | KeywordAttr.Special),
		('attribute', [], KeywordAttr.MakeLower),
		('constant', [], KeywordAttr.MakeLower),
	]

def parse_wasm_lexer_keywords(path):
	if not os.path.isfile(path):
		print('missing file:', path)
		return [
			('keywords', [], KeywordAttr.Default),
			('type', [], KeywordAttr.Default),
			('instruction', [], KeywordAttr.Default),
			('full instruction',[], KeywordAttr.NoLexer),
		]

	keywordMap = {
		'keywords': [],
		'type': [],
		'instruction': [],
		'full instruction': [],
	}

	types = ['i32', 'i64', 'f32', 'f64', 'v128',
		'f32x4', 'f64x2',
		'i8x16', 'i16x8', 'i32x4', 'i64x2',
		'v8x16', 'v16x8', 'v32x4', 'v64x2',
	]

	def has_type_prefix(word):
		return any(word.startswith(prefix + '.') for prefix in types)

	for line in read_file(path).splitlines():
		if not line or not (line[0].islower() and ',' in line and '::' in line):
			continue

		items = ''.join(line.split()).split(',')
		word = items[0]
		token_type = [item[:item.index('::')] for item in items[1:]]

		if 'Opcode' in token_type:
			if '/' in word:
				# Deprecated names
				continue
			if has_type_prefix(word):
				keywordMap['full instruction'].append(word)
			else:
				keywordMap['instruction'].append(word)
		elif 'TokenType' in token_type:
			if word not in types:
				keywordMap['keywords'].append(word)
		elif 'Type' in token_type:
			keywordMap['type'].append(word)
		else:
			print('Unknown TokenType:', token_type, line)
		assert '/' not in word

	keywordMap['type'].extend(types)
	RemoveDuplicateKeyword(keywordMap, [
		'keywords',
		'type',
		'instruction'
	])
	return [
		('keywords', keywordMap['keywords'], KeywordAttr.Default),
		('type', keywordMap['type'], KeywordAttr.Default),
		('instruction', keywordMap['instruction'], KeywordAttr.Default),
		('full instruction', keywordMap['full instruction'], KeywordAttr.NoLexer),
	]

def parse_xml_api_file(path):
	return [
		('tag', [], KeywordAttr.Default),
		('JavaScript', [], KeywordAttr.Default),
		('VBScript', [], KeywordAttr.Default),
		('Python', [], KeywordAttr.Default),
		('PHP', [], KeywordAttr.Default),
		('SGML', [], KeywordAttr.Default),
		('attribute', [], KeywordAttr.NoLexer),
		('event handler', [], KeywordAttr.Default),
		('value', [], KeywordAttr.NoLexer),
	]

def parse_yaml_api_file(path):
	keywords = '.inf .nan Inf NaN None false inf nan no none null off on true yes'.split()
	return [
		('keywords', keywords, KeywordAttr.Default),
	]

def UpdateLexerKeywordAttr(indexPath, lexerPath):
	#print(SinglyWordMap)
	output = []
	if AllKeywordAttrList:
		output.append('enum {')
		for prefix, group in sorted(SpecialKeywordIndexList.items()):
			items = sorted(group.items(), key=lambda m: m[1])
			output.extend(f'\t{prefix}{key} = {value},' for key, value in items)
		output.append('};')
	Regenerate(indexPath, '//KeywordIndex', output)

	for lexer, indexList in LexerKeywordIndexList.items():
		output = []
		if indexList:
			items = [(value[0], key, value[1]) for key, value in indexList.items()]
			items.sort()
			prev = (-1, '')
			output.append('enum {')
			for item in items:
				value, key, rid = item
				if value == prev[0]:
					print(f'{lexer} same keyword index {value}: ({key} {rid[7:]}), ({prev[1]} {prev[2][7:]})')
				output.append(f'\tKeywordIndex_{key} = {value},')
				prev = item
			output.append('};')

		path = lexerPath + lexer
		Regenerate(path, '//KeywordIndex', output)
