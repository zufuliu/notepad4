#-*- coding: UTF-8 -*-
import sys
sys.path.append('../scintilla/scripts')
import re
from FileGenerator import Regenerate

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
		comment, items = item
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

# Rust
def parse_rust_api_file(path):
	doc = open(path, encoding='utf-8').read()
	doc = re.sub(r'//[^!].+', '', doc)
	sections = doc.split("//!")
	keywordMap = {}
	used = set()
	for section in sections:
		lines = section.strip().splitlines()
		if not lines:
			continue

		key = lines[0].strip().lower()
		doc = '\n'.join(lines[1:])
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
		('keywords', keywordMap['keywords']),
		('reserved keywords', keywordMap['reserved keywords']),
		('primitive types', keywordMap['primitive types']),
		('struct', keywordMap['struct']),
		('trait', keywordMap['trait']),
		('enumeration', keywordMap['enumeration']),
		('union', keywordMap['union']),
		('constant', keywordMap['constant']),
		('attribute', keywordMap['attribute']),
		('macro', keywordMap['macros']),
		('module', keywordMap['modules']),
		('function', keywordMap['function']),
	]
	return keywordList

def update_rust_keyword():
	keywordList = parse_rust_api_file('lang/Rust.rs')
	UpdateKeywordFile('../src/EditLexers/stlRust.c', keywordList)

