import sys
import os.path
import unicodedata

MaxCharacter = sys.maxunicode
UnicodeCharacterCount = sys.maxunicode + 1
BMPCharacterCharacterCount = 0xffff + 1
DBCSCharacterCount = 0xffff + 1

def getCharacterName(ch):
	try:
		return unicodedata.name(ch).title()
	except ValueError:
		return ''

def readUnicodePropertyFile(path, propertyIndex=1):
	filename, ext = os.path.splitext(os.path.basename(path))
	version = ''
	propertyList = {}
	missingList = {}
	with open(path, encoding='utf-8') as fd:
		for line in fd.readlines():
			line = line.strip()
			missing = False
			if not line:
				continue
			if line[0] == '#':
				line = line[1:].strip()
				if line.startswith(filename) and line.endswith(ext):
					version = line[len(filename) + 1:-len(ext)]
					continue
				if not line.startswith('@missing:'):
					continue
				missing = True
				print(path, line)
				line = line[len('@missing:'):]

			index = line.find('#') # ignore comment
			if index > 0:
				line = line[:index]
			items = line.split(';')
			code = [int(item, 16) for item in items[0].replace('.', ' ').split() if item]
			if missing:
				propertyName = items[1].strip()
				value = items[2] if len(items) > 2 else None
				assert propertyName not in missingList
				missingList[propertyName] = (code, value)
			else:
				propertyName = items[propertyIndex].strip()
				if propertyName in propertyList:
					propertyList[propertyName].append(code)
				else:
					propertyList[propertyName] = [code]

	print(path, version, ', '.join(propertyList.keys()))
	return version, propertyList

def updateUnicodePropertyTable(table, propertyMap, propertyList):
	for name, codeList in propertyList.items():
		if name not in propertyMap:
			continue
		value = int(propertyMap[name])
		for code in codeList:
			if len(code) == 1:
				table[code[0]] = value
			else:
				assert len(code) == 2
				start, end = code
				while start <= end:
					table[start] = value
					start += 1

def flattenUnicodePropertyTable(table, propertyList):
	for name, codeList in propertyList.items():
		for code in codeList:
			if len(code) == 1:
				table[code[0]] = name
			else:
				assert len(code) == 2
				start, end = code
				while start <= end:
					table[start] = name
					start += 1
