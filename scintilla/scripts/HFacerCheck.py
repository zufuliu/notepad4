import os.path
import glob
import re

def findHoles(asc):
	holes = []
	prev = asc[0]
	for val in asc:
		diff = val - prev
		if diff > 1:
			holes.extend(range(prev+1, val))
		prev = val
	return holes

def readIFace(path):
	with open(path, encoding='utf-8') as fd:
		doc = fd.read()
	# remove comment
	ifaceDoc = re.sub(r'\s+#.+', '', doc)
	# ignore deprecated category
	index = ifaceDoc.find('cat Deprecated')
	if index > 0:
		ifaceDoc = ifaceDoc[:index]
	return ifaceDoc, doc

def findAPIHoles():
	ifaceDoc, backup = readIFace('../include/Scintilla.iface')

	# find unused or duplicate API message number
	pattern = r'(fun|get|set)\s+(?P<type>\w+)\s+(?P<name>\w+)\s*=\s*(?P<value>\d+)'
	valList = {} # {value: [name]}
	result = re.findall(pattern, ifaceDoc)
	for item in result:
		name = item[2]
		value = int(item[3])
		values = valList.setdefault(value, [])
		if values:
			print(f'duplicate value: {value} {name} {" ".join(values)}')
		values.append(name)

	allVals = sorted(valList.keys())
	print('all values:', allVals)
	allVals = [item for item in allVals if item < 3000]
	holes = findHoles(allVals)
	print('min, max and holes:', allVals[0], allVals[-1], holes)

	if holes:
		values = []
		def print_holes(tag, regex, doc):
			result = re.findall(regex, doc)
			output = []
			for item in result:
				value = int(item[3])
				if value in holes:
					name = item[2]
					values.append(value)
					output.append(f'{value} {name}')
			print(tag, ', '.join(sorted(output)))

		ifaceDoc = backup
		print_holes('used:', r'#\s*' + pattern, ifaceDoc)
		index = ifaceDoc.find('cat Deprecated')
		if index > 0:
			ifaceDoc = ifaceDoc[index:]
			print_holes('deprecated:', pattern, ifaceDoc)
		print('unused:', sorted(set(holes) - set(values)))

def checkLexerDefinition():
	ifaceDoc, _ = readIFace('../include/SciLexer.iface')

	# ensure SCLEX_ is unique
	valList = {} # {value: [name]}
	result = re.findall(r'val\s+(?P<name>SCLEX_\w+)\s*=\s*(?P<value>\d+)', ifaceDoc)
	for name, value in result:
		value = int(value)
		values = valList.setdefault(value, [])
		if values:
			print(f'duplicate value: {value} {name} {" ".join(values)}')
		values.append(name)

	# StylesCommon in Scintilla.iface
	STYLE_FIRSTPREDEFINED = 32
	STYLE_LASTPREDEFINED = 39
	# ensure style number is unique within same lexer and not used by StylesCommon
	prefixMap = {} # {prefix: lexer}
	result = re.findall(r'lex\s+(?P<name>\w+)\s*=(.+)+', ifaceDoc)
	stylePrefix = {} # {lexer: [prefix]}
	for name, value in result:
		if name == 'XML':
			name = 'HTML'
		items = value.split()
		for item in items:
			prefixMap[item] = name
		lexer = items[0]
		assert lexer not in stylePrefix
		stylePrefix[lexer] = items[1:]

	lexrList = {} # {lexer: {value: [name]}}
	result = re.findall(r'val\s+(?P<name>SCE_\w+)\s*=\s*(?P<value>\d+)', ifaceDoc)
	for name, value in result:
		prefix = name[:name.index('_', 4) + 1] # underscore after SCE_
		lexer = prefixMap[prefix]
		valList = lexrList.setdefault(lexer, {})
		value = int(value)
		if value >= STYLE_FIRSTPREDEFINED and value <= STYLE_LASTPREDEFINED:
			print(f'error value: {value} {name}')
		values = valList.setdefault(value, [])
		if values:
			print(f'duplicate value: {value} {name} {" ".join(values)}')
		values.append(name)

	for path in glob.glob('../lexers/Lex*.cxx'):
		path = os.path.normpath(path)
		name = os.path.basename(path)

		with open(path, encoding='utf-8') as fd:
			doc = fd.read()
		prefix = set()
		lexers = re.findall(r'SCLEX_\w+', doc)
		for lexer in lexers:
			if lexer in stylePrefix:
				prefix |= set(stylePrefix[lexer])
		items = set(re.findall(r'SCE_\w+?_', doc))
		if unknown := items - prefix:
			print(name, 'unknown style:', ', '.join(sorted(unknown)))

findAPIHoles()
checkLexerDefinition()
