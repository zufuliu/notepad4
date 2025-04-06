import sys
import os.path
import unicodedata

# Common References for Unicode Standard Annexes
# https://www.unicode.org/reports/tr41/
# Unicode Character Database
# https://www.unicode.org/reports/tr44/
# Unicode Locale Data Markup Language (LDML)
# https://www.unicode.org/reports/tr35/
# Unicode Character Database in XML
# https://www.unicode.org/reports/tr42/

MaxCharacter = sys.maxunicode
UnicodeCharacterCount = sys.maxunicode + 1
BMPCharacterCharacterCount = 0xffff + 1
DBCSCharacterCount = 0xffff + 1

def getCharacterName(ch):
	try:
		return unicodedata.name(ch).title()
	except ValueError:
		return ''

def readUnicodePropertyFile(path, propertyIndex=1, firstLast=None, withMissing=False, missingMap=None):
	filename, ext = os.path.splitext(os.path.basename(path))
	version = ''
	propertyList = {}
	missingList = {}
	prevLine = []
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
				if withMissing:
					propertyName = items[1].strip()
					if missingMap and propertyName in missingMap:
						propertyName = missingMap[propertyName]
					if propertyName in missingList:
						missingList[propertyName].append(code)
					else:
						missingList[propertyName] = [code]
			else:
				propertyName = items[propertyIndex].strip()
				if firstLast:
					last = items[firstLast].strip()
					if last.endswith('Last>'):
						first = prevLine[firstLast].strip()
						prop = prevLine[propertyIndex].strip()
						assert first.endswith('First>')
						assert propertyName == prop and len(code) == 1
						propertyList[propertyName][-1].append(code[0])
						propertyName = None
				if propertyName is not None:
					if propertyName in propertyList:
						propertyList[propertyName].append(code)
					else:
						propertyList[propertyName] = [code]
			prevLine = items

	if withMissing and missingList:
		for name, codeList in propertyList.items():
			if name in missingList:
				missingList[name].extend(codeList)
			else:
				missingList[name] = codeList
		propertyList = missingList
	print(path, version, 'property:', ', '.join(sorted(propertyList.keys())))
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

def flattenPropertyMap(propertyMap):
	result = {}
	for key, items in propertyMap.items():
		for value in items:
			result[value] = key
	return result

def parseSegmentationChart(path, opportunity=0):
	from bs4 import BeautifulSoup

	print('parse:', path)
	with open(path, encoding='utf-8', newline='\n') as fd:
		doc = fd.read()
	soup = BeautifulSoup(doc, 'html5lib')
	title = soup.find('title').get_text()
	node = soup.find('a', {'name': 'table'})
	node = node.find_next('table').find('tbody')
	table = []
	row_map = {}
	same_row = {}
	column_header = []
	for row in node.find_all('tr'):
		items = []
		for column in row.find_all('th'):
			items.append(column.get_text().strip())
		if not items:
			continue
		if not column_header:
			assert len(items) > 1 and len(items[0]) == 0
			column_header = items[1:]
		else:
			assert len(items) == len(column_header) + 1
			header = items[0]
			items = items[1:]
			if header in row_map:
				assert items == row_map[header]
			else:
				for name, value in row_map.items():
					if items == value:
						same_row[header] = name
						# print('same row:', name, header)
						break
				table.append(items)
				row_map[header] = items

	row_header = list(row_map.keys())
	row_count = len(row_header)
	print(f'{title} table row: {row_count}, column: {len(column_header)}')
	column_map = {}
	same_column = {}
	for index, header in enumerate(column_header):
		items = [None] * row_count
		for i in range(row_count):
			items[i] = table[i][index]
		if header in column_map:
			assert items == column_map[header]
		else:
			for name, value in column_map.items():
				if items == value:
					same_column[header] = name
					# print('same column:', name, header)
					break
			column_map[header] = items

	# find property with same row and same column
	same_map = {}
	both = (same_row.keys() | same_row.values()) & (same_column.keys() | same_column.values())
	if len(both) > 1:
		count = len(both)
		both = sorted(both, key=row_header.index)
		index = 0
		while index < count:
			header = both[index]
			index += 1
			row = same_row.get(header, header)
			column = same_column.get(header, header)
			j = index
			while j < count:
				name = both[j]
				j += 1
				r = same_row.get(name, name)
				c = same_column.get(name, name)
				if row == r and column == c:
					print('same row & column:', header, name)
					same_map[name] = header
					j -= 1
					count -= 1
					del both[j]

	opportunity = '×÷'[opportunity]
	for header, items in row_map.items():
		if header in same_map:
			continue
		row = []
		for index, symbol in enumerate(items):
			if symbol == opportunity:
				value = row_header[index]
				value = same_map.get(value, value)
				if value not in row:
					row.append(value)
		if row:
			print(f'{opportunity} {header}:', ', '.join(row))
