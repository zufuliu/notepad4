#!/usr/bin/env python3
# Script to generate CharacterCategory.cxx from Python's Unicode data
# Should be run rarely when a Python with a new version of Unicode data is available.

import sys
import codecs
import platform
import unicodedata
from enum import IntEnum
import math

from FileGenerator import Regenerate
from splitbins import *

UnicodeCharacterCount = sys.maxunicode + 1
BMPCharacterCharacterCount = 0xffff + 1
DBCSCharacterCount = 0xffff + 1

class CharClassify(IntEnum):
	ccSpace = 0
	ccNewLine = 1
	ccWord = 2
	ccPunctuation = 3
	ccCJKWord = 4

	OnlyWord = 1 << ccWord
	OnlyWordPunctuation = (1 << ccWord) | (1 << ccPunctuation)
	RLEValueBit = 3

CharClassifyOrder = [CharClassify.ccCJKWord, CharClassify.ccWord, CharClassify.ccPunctuation, CharClassify.ccNewLine]
def prefCharClassify(values):
	for value in CharClassifyOrder:
		if value in values:
			return value
	# Cn
	return CharClassify.ccSpace

# https://en.wikipedia.org/wiki/Unicode_character_property
# Document::WordCharacterClass()
CharClassifyMap = {
	CharClassify.ccSpace: [
		'Zs',
		# Other
		'Cc',
		'Cf',
		'Cs',
		'Co',
		'Cn',
	],
	CharClassify.ccNewLine: [
		'Zl',
		'Zp',
	],
	CharClassify.ccWord: [
		# Letter
		'Lu',
		'Ll',
		'Lt',
		'Lm',
		'Lo',
		# Number
		'Nd',
		'Nl',
		'No',
		# Mark
		'Mn',
		'Mc',
		'Me',
	],
	CharClassify.ccPunctuation: [
		# Punctuation
		'Pc',
		'Pd',
		'Ps',
		'Pe',
		'Pi',
		'Pf',
		'Po',
		# Symbol
		'Sm',
		'Sc',
		'Sk',
		'So',
	],
}

ClassifyMap = {}
for key, items in CharClassifyMap.items():
	for category in items:
		ClassifyMap[category] = key


# https://en.wikipedia.org/wiki/Private_Use_Areas
# Category: Other, private use (Co)
def isPrivateChar(c):
	return (c >= 0xE000 and c <= 0xF8FF) \
		or (c >= 0xF0000 and c <= 0xFFFFD) \
		or (c >= 0x100000 and c <= 0x10FFFD)

# https://en.wikipedia.org/wiki/Unicode_block
# https://en.wikipedia.org/wiki/Plane_(Unicode)#Basic_Multilingual_Plane
# https://en.wikipedia.org/wiki/CJK_Unified_Ideographs
# https://en.wikipedia.org/wiki/Katakana
# https://en.wikipedia.org/wiki/Hangul
# Chapter 18 East Asia http://www.unicode.org/versions/Unicode13.0.0/UnicodeStandard-13.0.pdf
# Unicode Han Database (Unihan) https://unicode.org/reports/tr38/#BlockListing
CJKBlockList = [
	# Table 18-1. Blocks Containing Han Ideographs
	(0x3400, 0x4DBF), # U+3400..U+4DBF CJK Unified Ideographs Extension A
	(0x4E00, 0x9FFF), # U+4E00..U+9FFF CJK Unified Ideographs
	(0xF900, 0xFAFF), # U+F900..U+FAFF CJK Compatibility Ideographs
	# surrogate pair
	(0x20000, 0x2A6DF), # U+20000..U+2A6DF CJK Unified Ideographs Extension B
	(0x2A700, 0x2B73F), # U+2A700..U+2B73F CJK Unified Ideographs Extension C
	(0x2B740, 0x2B81F), # U+2B740..U+2B81F CJK Unified Ideographs Extension D
	(0x2B820, 0x2CEAF), # U+2B820..U+2CEAF CJK Unified Ideographs Extension E
	(0x2CEB0, 0x2EBEF), # U+2CEB0..U+2EBEF CJK Unified Ideographs Extension F
	(0x2F800, 0x2FA1F), # U+2F800..U+2FA1F CJK Compatibility Ideographs Supplement
	(0x30000, 0x3134F), # U+30000..U+3134F CJK Unified Ideographs Extension G

	(0x2E80, 0x2EFF), # U+2E80..U+2EFF CJK Radicals Supplement
	(0x2F00, 0x2FDF), # U+2F00..U+2FDF Kangxi Radicals
	(0x2FF0, 0x2FFF), # U+2FF0..U+2FFF Ideographic Description Characters
	(0x3000, 0x303F), # U+3000..U+303F CJK Symbols and Punctuation
	(0x3100, 0x312F), # U+3100..U+312F Bopomofo
	(0x31A0, 0x31BF), # U+31A0..U+31BF Bopomofo Extended
	(0x31C0, 0x31EF), # U+31C0..U+31EF CJK Strokes
	(0x3200, 0x32FF), # U+3200..U+32FF Enclosed CJK Letters and Months
	(0x3300, 0x33FF), # U+3300..U+33FF CJK Compatibility
	(0xFE30, 0xFE4F), # U+FE30..U+FE4F CJK Compatibility Forms
	(0xFF5F, 0xFFEF), # U+FF5F..U+FFEF Halfwidth and Fullwidth Forms (without fullwidth ASCII)
	(0x16FE0, 0x16FFF), # U+16FE0..U+16FFF Ideographic Symbols and Punctuation
	(0x1F200, 0x1F2FF), # U+1F200..U+1F2FF Enclosed Ideographic Supplement
	# Japanese
	(0x3040, 0x309F), # U+3040..U+309F Hiragana
	(0x30A0, 0x30FF), # U+30A0..U+30FF Katakana
	(0x3190, 0x319F), # U+3190..U+319F Kanbun
	(0x31F0, 0x31FF), # U+31F0..U+31FF Katakana Phonetic Extensions
	(0xFF65, 0xFF9F), # U+FF65..U+FF9F Halfwidth Katakana
	(0x1B000, 0x1B0FF), # U+1B000..U+1B0FF Kana Supplement
	(0x1B100, 0x1B12F), # U+1B100..U+1B12F Kana Extended-A
	(0x1B130, 0x1B16F), # U+1B130..U+1B16F Small Kana Extension
	# Korean
	(0x1100, 0x11FF), # U+1100..U+11FF Hangul Jamo
	(0x3130, 0x318F), # U+3130..U+318F Hangul Compatibility Jamo
	(0xA960, 0xA97F), # U+A960..U+A97F Hangul Jamo Extended-A
	(0xAC00, 0xD7AF), # U+AC00..U+D7AF Hangul Syllables
	(0xD7B0, 0xD7FF), # U+D7B0..U+D7FF Hangul Jamo Extended-B
	(0xFFA0, 0xFFDC), # U+FFA0..U+FFDC Halfwidth Compatibility Jamo
	# Other
	(0xA000, 0xA48F), # U+A000..U+A48F Yi Syllables
	(0xA490, 0xA4CF), # U+A490..U+A4CF Yi Radicals
	(0x17000, 0x187FF), # U+17000..U+187FF Tangut
	(0x18800, 0x18AFF), # U+18800..U+18AFF Tangut Components
	(0x18D00, 0x18D8F), # U+18D00..U+18D8F Tangut Supplement
	(0x18B00, 0x18CFF), # U+18B00..U+18CFF Khitan Small Script
	(0x1B170, 0x1B2FF), # U+1B170..U+1B2FF Nüshu
]

def findCategories(filename):
	with open(filename, "r", encoding="utf-8") as infile:
		lines = [x.strip() for x in infile.readlines() if "\tcc" in x]
	values = "".join(lines).replace(" ","").split(",")
	print(values)
	return [v[2:] for v in values]

def isCJKCharacter(category, ch):
	if category not in CharClassifyMap[CharClassify.ccWord]:
		return False

	for block in CJKBlockList:
		if ch >= block[0] and ch <= block[1]:
			return True

	return False

def dumpArray(items, step, fmt='%d'):
	lines = []
	if step:
		for i in range(0, len(items), step):
			line = ", ".join(fmt % value for value in items[i:i+step]) + ","
			lines.append(line)
	else:
		line = ", ".join(fmt % value for value in items)
		lines.append(line)
	return lines

def updateCharacterCategory(categories):
	values = ["// Created with Python %s, Unicode %s" % (
		platform.python_version(), unicodedata.unidata_version)]

	# catLatin
	values.append("#if CHARACTERCATEGORY_OPTIMIZE_LATIN1")
	values.append("const unsigned char catLatin[] = {")
	table = [categories.index(unicodedata.category(chr(ch))) for ch in range(256)]
	for i in range(0, len(table), 16):
		line = ', '.join(str(value) for value in table[i:i+16]) + ','
		values.append(line)
	values.append("};")
	values.append("#endif")
	values.append("")

	# catRanges
	startRange = 0
	category = unicodedata.category(chr(startRange))
	table = []
	for ch in range(sys.maxunicode):
		uch = chr(ch)
		current = unicodedata.category(uch)
		if current != category:
			value = startRange * 32 + categories.index(category)
			table.append(value)
			category = current
			startRange = ch
	value = startRange * 32 + categories.index(category)
	table.append(value)

	# the sentinel value is used to simplify CharacterMap::Optimize()
	category = 'Cn'
	value = (sys.maxunicode + 1)*32 + categories.index(category)
	table.append(value)

	print('catRanges:', len(table), 4*len(table)/1024, math.ceil(math.log2(len(table))))
	values.append("#if CHARACTERCATEGORY_USE_BINARY_SEARCH")
	values.append("const int catRanges[] = {")
	values.extend(["%d," % value for value in table])
	values.append("};")
	values.append("")
	values.append("#else")

	return values

def bytesToHex(b):
	return ''.join('\\x%02X' % ch for ch in b)

def buildFoldDisplayEllipsis():
	# Interpunct https://en.wikipedia.org/wiki/Interpunct
	defaultText = '\u00B7' * 3	# U+00B7 Middle Dot
	fallbackText = '.' * 3		# U+002E Full Stop
	fallbackJIS = '\uFF65' * 3	# U+FF65 Halfwidth Katakana Middle Dot

	# DBCS
	encodingList = [
		('cp932', 932, 'Shift_JIS'),
		('cp936', 936, 'GBK'),
		('cp949', 949, 'UHC'),
		('cp950', 950, 'Big5'),
		('cp1361', 1361, 'Johab'),
	]

	result = {}
	for encoding, codepage, comment in encodingList:
		try:
			value = defaultText.encode(encoding)
			value = bytesToHex(value)
		except UnicodeEncodeError:
			if codepage == 932:
				value = fallbackJIS.encode(encoding)
				value = bytesToHex(value)
			else:
				value = fallbackText

		values = result.setdefault(value, [])
		values.append((codepage, comment))

	utf8Text = defaultText.encode('utf-8');
	utf8Text = bytesToHex(utf8Text)

	output = []
	output.append("const char* GetFoldDisplayEllipsis(UINT cpEdit, UINT acp) {")
	output.append("\tswitch (cpEdit) {")
	output.append("\tcase SC_CP_UTF8:")
	output.append('\t\treturn "%s";' % utf8Text)
	for key, values in result.items():
		for codepage, comment in values:
			output.append("\tcase %d: // %s" % (codepage, comment))
		output.append('\t\treturn "%s";' % key)
	output.append("\t}")

	encodingList = [
		('cp1250', 1250, 'Central European (Windows-1250)'),
		('cp1251', 1251, 'Cyrillic (Windows-1251)'),
		('cp1252', 1252, 'Western European (Windows-1252)'),
		('cp1253', 1253, 'Greek (Windows-1253)'),
		('cp1254', 1254, 'Turkish (Windows-1254)'),
		('cp1255', 1255, 'Hebrew (Windows-1255)'),
		('cp1256', 1256, 'Arabic (Windows-1256)'),
		('cp1257', 1257, 'Baltic (Windows-1257)'),
		('cp1258', 1258, 'Vietnamese (Windows-1258)'),
		('cp874', 874, 'Thai (Windows-874)'),
	]

	result = {}
	fallback = []
	for encoding, codepage, comment in encodingList:
		try:
			value = defaultText.encode(encoding)
			value = bytesToHex(value)
		except UnicodeEncodeError:
			fallback.append((codepage, comment))
			continue

		values = result.setdefault(value, [])
		values.append((codepage, comment))

	fallback.append(('default', ''))
	result[fallbackText] = fallback

	output.append("\t// SBCS")
	output.append("\tswitch (acp) {")
	for key, values in result.items():
		for codepage, comment in values:
			if codepage == 'default':
				output.append("\tdefault:")
			else:
				output.append("\tcase %d: // %s" % (codepage, comment))
		output.append('\t\treturn "%s";' % key)
	output.append("\t}")
	output.append("}")

	return output

def getCharClassify(decode, ch):
	try:
		uch = decode(bytes([ch]))
	except UnicodeDecodeError:
		uch = ('', 0)

	if uch[1] == 1 and len(uch[0]) == 1:
		category = unicodedata.category(uch[0])
	else:
		# undefined, treat as Cn, Not assigned
		category = 'Cn'

	cc = ClassifyMap[category]
	return int(cc)

def buildCharClassify(cp):
	decode = codecs.getdecoder(cp)
	result = {}
	mask = 0
	for ch in range(128, 256):
		cc = getCharClassify(decode, ch)
		mask |= 1 << cc
		result[ch] = cc

	if mask == CharClassify.OnlyWord:
		output = None
	elif mask == CharClassify.OnlyWordPunctuation:
		output = [0] * 16
		for ch, value in result.items():
			ch -= 128
			value -= CharClassify.ccWord
			output[ch >> 3] |= value << (ch & 7)
	else:
		output = [0] * 32
		for ch, value in result.items():
			ch -= 128
			output[ch >> 2] |= value << (2 * (ch & 3))

	s = ''.join('%02X' % ch for ch in output) if output else ''
	return s, output

def buildANSICharClassifyTable(filename):
	encodingList = [
		('cp1250', 1250, 'Central European (Windows-1250)'),
		('cp1251', 1251, 'Cyrillic (Windows-1251)'),
		('cp1252', 1252, 'Western European (Windows-1252)'),
		('cp1253', 1253, 'Greek (Windows-1253)'),
		('cp1254', 1254, 'Turkish (Windows-1254)'),
		('cp1255', 1255, 'Hebrew (Windows-1255)'),
		('cp1256', 1256, 'Arabic (Windows-1256)'),
		('cp1257', 1257, 'Baltic (Windows-1257)'),
		('cp1258', 1258, 'Vietnamese (Windows-1258)'),
		('cp874', 874, 'Thai (Windows-874)'),
	]

	result = {}
	offset = 0
	for encoding, codepage, comment in encodingList:
		s, m = buildCharClassify(encoding)
		if not s:
			continue

		if s not in result:
			result[s] = { 'data': m, 'offset': offset, 'codepage': [(codepage, comment)]}
			offset += len(m)
		else:
			result[s]['codepage'].append((codepage, comment))

	output = ["// Created with Python %s, Unicode %s" % (
		platform.python_version(), unicodedata.unidata_version)]

	output.append("static const uint8_t ANSICharClassifyTable[] = {")
	for item in result.values():
		for page in item['codepage']:
			output.append('// ' + page[1])
		data = item['data']
		output.extend(dumpArray(data, 16, '0x%02X'))
	output.append("};")
	output.append("")

	output.append("static const uint8_t* GetANSICharClassifyTable(UINT cp, int *length) {")
	output.append("\tswitch (cp) {")
	for item in result.values():
		for page in item['codepage']:
			output.append("\tcase %d: // %s" % (page[0], page[1]))
		output.append("\t\t*length = %d;" % len(item['data']))
		output.append("\t\treturn ANSICharClassifyTable + %d;" % item['offset'])
	output.append("\tdefault:")
	output.append("\t\t*length = 0;")
	output.append("\t\treturn NULL;")
	output.append("\t}")
	output.append("}")

	output.append("")
	ellipsis = buildFoldDisplayEllipsis()
	output.extend(ellipsis)

	print('ANSICharClassifyTable:', len(result), len(encodingList))
	Regenerate(filename, "//", output)

def compressIndexTable(head, indexTable, args):
	indexA, indexC, indexD, shiftA, shiftC = splitbins(indexTable, True)
	print(f'{head}:', (len(indexA), max(indexA)), (len(indexC), max(indexC)), len(indexD), (shiftA, shiftC))

	sizeA = getsize(indexA)
	sizeC = getsize(indexC)
	sizeD = getsize(indexD)
	total = len(indexA)*sizeA + len(indexC)*sizeC + len(indexD)*sizeD
	print(f'{head} total size:', total/1024)

	maskA = (1 << shiftA) - 1
	maskC = (1 << shiftC) - 1
	shiftA2 = shiftA
	shiftC2 = shiftC

	if True:
		shiftA2 = preshift(indexA, shiftA)
		shiftC2 = preshift(indexC, shiftC)

	for ch in range(len(indexTable)):
		i = (indexA[ch >> shiftA] << shiftA2) | (ch & maskA)
		i = (indexC[i >> shiftC] << shiftC2) | (i & maskC)
		value = indexD[i]
		expect = indexTable[ch]
		if value != expect:
			print(f'{head} verify fail:', '%04X, expect: %d, got: %d' % (ch, expect, value))
			return None

	typemap = {
		1: 'unsigned char',
		2: 'unsigned short',
	}

	prefix = args['table']
	with_function = args.get('with_function', True)

	# one table
	if sizeA == sizeC == sizeD:
		output = []
		name = args.get('table_var', prefix)
		output.append("const %s %s[] = {" % (typemap[sizeA], name))
		output.append("// %s1" % prefix)
		output.extend(dumpArray(indexA, 20))
		output.append("// %s2" % prefix)
		output.extend(dumpArray(indexC, 20))
		output.append("// %s" % prefix)
		output.extend(dumpArray(indexD, 20))
		output.append("};")
		table = '\n'.join(output)

		args.update({
			'shiftA': shiftA,
			'shiftA2': shiftA2,
			'maskA': maskA,
			'offsetC': len(indexA),
			'shiftC': shiftC,
			'shiftC2': shiftC2,
			'maskC': maskC,
			'offsetD': len(indexA) + len(indexC)
		})
		if with_function:
			function = """{function}
	ch = ({table}[ch >> {shiftA}] << {shiftA2}) | (ch & {maskA});
	ch = ({table}[{offsetC} + (ch >> {shiftC})] << {shiftC2}) | (ch & {maskC});
	return static_cast<{returnType}>({table}[{offsetD} + ch]);
}}""".format(**args)
		else:
			function = """
constexpr int {table}Shift11 = {shiftA};
constexpr int {table}Shift12 = {shiftA2};
constexpr int {table}Mask1 = {maskA};
constexpr int {table}Offset1 = {offsetC};
constexpr int {table}Shift21 = {shiftC};
constexpr int {table}Shift22 = {shiftC2};
constexpr int {table}Mask2 = {maskC};
constexpr int {table}Offset2 = {offsetD};
""".format(**args)
	# three tables
	else:
		output = []
		output.append("const %s %s1[] = {" % (typemap[sizeA], prefix))
		output.extend(dumpArray(indexA, 20))
		output.append("};")
		output.append("")
		output.append("const %s %s2[] = {" % (typemap[sizeC], prefix))
		output.extend(dumpArray(indexC, 20))
		output.append("};")
		output.append("")
		output.append("const %s %s[] = {" % (typemap[sizeD], prefix))
		output.extend(dumpArray(indexD, 20))
		output.append("};")
		table = '\n'.join(output)

		args.update({
			'shiftA': shiftA,
			'shiftA2': shiftA2,
			'maskA': maskA,
			'shiftC': shiftC,
			'shiftC2': shiftC2,
			'maskC': maskC,
		})
		if with_function:
			function = """{function}
	ch = ({table}1[ch >> {shiftA}] << {shiftA2}) | (ch & {maskA});
	ch = ({table}2[(ch >> {shiftC})] << {shiftC2}) | (ch & {maskC});
	return static_cast<{returnType}>({table}[ch]);
}}""".format(**args)
		else:
			function = """
constexpr int {table}Shift11 = {shiftA};
constexpr int {table}Shift12 = {shiftA2};
constexpr int {table}Mask1 = {maskA};
constexpr int {table}Shift21 = {shiftC};
constexpr int {table}Shift22 = {shiftC2};
constexpr int {table}Mask2 = {maskC};
""".format(**args)
	return table, function

def runLengthEncode(head, indexTable, valueBit, totalBit=16):
	assert max(indexTable) < 2**valueBit
	maxLength = 2**(totalBit - valueBit) - 1

	values = []
	prevValue = indexTable[0]
	prevIndex = 0;
	for index, value in enumerate(indexTable):
		if value != prevValue:
			values.append((prevValue, index - prevIndex))
			prevValue = value
			prevIndex = index
	values.append((prevValue, len(indexTable) - prevIndex))

	output = []
	for value, count in values:
		if count > maxLength:
			output.extend([(maxLength << valueBit) | value] * (count // maxLength))
			count = count % maxLength
			if count == 0:
				continue
		output.append((count << valueBit) | value)

	total = getsize(output)*len(output)/1024
	print(f'{head} RLE size: {len(output)} {total}')

	result = []
	mask = 2**valueBit - 1
	for ch in output:
		value = ch & mask
		count = ch >> valueBit
		result.extend([value] * count)

	assert result == indexTable
	return output

def updateCharClassifyTable(filename, headfile):
	indexTable = [0] * UnicodeCharacterCount
	for ch in range(UnicodeCharacterCount):
		uch = chr(ch)
		category = unicodedata.category(uch)
		value = ClassifyMap[category]
		if isCJKCharacter(category, ch):
			value = CharClassify.ccCJKWord
		indexTable[ch] = int(value)

	output = ["// Created with Python %s, Unicode %s" % (
		platform.python_version(), unicodedata.unidata_version)]
	head_output = output[:]

	data = runLengthEncode('CharClassify Unicode BMP', indexTable[:BMPCharacterCharacterCount], int(CharClassify.RLEValueBit))
	output.append(f'const unsigned short CharClassifyRLE_BMP[] = {{')
	output.extend(dumpArray(data, 20))
	output.append("};")
	output.append("")
	output.append("}") # namespace
	output.append("")

	args = {
		'table_var': 'CharClassify::CharClassifyTable',
		'table': 'CharClassifyTable',
		'function': """static cc ClassifyCharacter(unsigned int ch) noexcept {
	if (ch < sizeof(classifyMap)) {
		return static_cast<cc>(classifyMap[ch]);
	}
	if (ch > maxUnicode) {
		// Cn
		return ccSpace;
	}

	ch -= sizeof(classifyMap);""",
		'returnType': 'cc'
	}

	table, function = compressIndexTable('CharClassify Unicode', indexTable[BMPCharacterCharacterCount:], args)
	output.extend(table.splitlines())

	for line in function.splitlines():
		head_output.append('\t' + line)

	Regenerate(filename, "//", output)
	Regenerate(headfile, "//", head_output)

def updateCharacterCategoryTable(filename):
	categories = findCategories("../lexlib/CharacterCategory.h")
	output = updateCharacterCategory(categories)

	indexTable = [0] * UnicodeCharacterCount
	for ch in range(UnicodeCharacterCount):
		uch = chr(ch)
		category = unicodedata.category(uch)
		value = categories.index(category)
		indexTable[ch] = value

	args = {
		'table': 'catTable',
		'with_function': False,
	}

	table, function = compressIndexTable('CharacterCategoryTable', indexTable, args)
	output.append("")
	output.extend(table.splitlines())
	output.extend(function.splitlines())

	data = runLengthEncode('CharacterCategoryTable', indexTable[:BMPCharacterCharacterCount], 5)
	output.append("")
	output.append(f'const unsigned short CatTableRLE_BMP[] = {{')
	output.extend(dumpArray(data, 20))
	output.append("};")

	output.append("")
	output.append("#endif")

	Regenerate(filename, "//", output)

def getDBCSCharClassify(decode, ch, isReservedOrUDC=None):
	buf = bytes([ch]) if ch < 256 else bytes([ch >> 8, ch & 0xff])
	try:
		uch = decode(buf)
	except UnicodeDecodeError:
		uch = ('', 0)

	if uch[1] == len(buf) and len(uch[0]) == 1:
		category = unicodedata.category(uch[0])
		ch = ord(uch[0])
		# treat PUA in DBCS as word instead of punctuation or space
		if isCJKCharacter(category, ch) or (category == 'Co' and isPrivateChar(ch)):
			return int(CharClassify.ccCJKWord)
	else:
		# treat reserved or user-defined characters as word
		if isReservedOrUDC and isReservedOrUDC(ch, buf):
			return int(CharClassify.ccCJKWord)
		# undefined, treat as Cn, Not assigned
		category = 'Cn'

	cc = ClassifyMap[category]
	return int(cc)

def makeDBCSCharClassifyTable(output, encodingList, isReservedOrUDC=None):
	result = {}

	for cp in encodingList:
		decode = codecs.getdecoder(cp)
		for ch in range(DBCSCharacterCount):
			cc = getDBCSCharClassify(decode, ch, isReservedOrUDC)
			if ch in result:
				result[ch].append(cc)
			else:
				result[ch] = [cc]

	indexTable = [0] * DBCSCharacterCount
	for ch in range(DBCSCharacterCount):
		indexTable[ch] = int(prefCharClassify(result[ch]))

	suffix = '_' + encodingList[0].upper()
	head = 'CharClassify' + suffix

	if True:
		data = runLengthEncode(head, indexTable, int(CharClassify.RLEValueBit))
		output.append(f'const unsigned short CharClassifyRLE{suffix}[] = {{')
		output.extend(dumpArray(data, 20))
		output.append("};")
		output.append("")

	if False:
		args = {
			'table': 'CharClassifyTable' + suffix,
			'function': """CharClassify::cc ClassifyCharacter%s(unsigned int ch) noexcept {
	if (ch > maxDBCSCharacter) {
		// Cn
		return CharClassify::ccSpace;
	}
	""" % suffix,
			'returnType': 'CharClassify::cc'
		}

		table, function = compressIndexTable(head, indexTable, args)
		output.extend(table.splitlines())
		output.append('')
		output.extend(function.splitlines())
		output.append('')

# https://en.wikipedia.org/wiki/GBK_(character_encoding)
def isReservedOrUDC_GBK(ch, buf):
	if len(buf) != 2:
		return False

	ch1 = buf[0]
	ch2 = buf[1]
	# user-defined 1 and 2
	if ((ch1 >= 0xAA and ch1 <= 0xAF) or (ch1 >= 0xF8 and ch1 <= 0xFE)) \
		and (ch2 >= 0xA1 and ch2 <= 0xFE):
		return True
	# user-defined 3
	if (ch1 >= 0xA1 and ch1 <= 0xA7) and (ch2 >= 0x40 and ch2 <= 0xA0 and ch2 != 0x7F):
		return True
	return False

# https://en.wikipedia.org/wiki/Big5
def isReservedOrUDC_Big5(ch, buf):
	for block in [(0x8140, 0xA0FE), (0xA3C0, 0xA3FE), (0xC6A1, 0xC8FE), (0xF9D6, 0xFEFE)]:
		if ch >= block[0] or ch <= block[1]:
			return True
	return False

def updateDBCSCharClassifyTable(filename):
	output = ["// Created with Python %s, Unicode %s" % (
		platform.python_version(), unicodedata.unidata_version)]

	makeDBCSCharClassifyTable(output, ['cp932', 'shift_jis', 'shift_jis_2004', 'shift_jisx0213'])
	makeDBCSCharClassifyTable(output, ['cp936', 'gbk'], isReservedOrUDC_GBK)
	makeDBCSCharClassifyTable(output, ['cp949']) # UHC
	makeDBCSCharClassifyTable(output, ['cp950', 'big5', 'big5hkscs'], isReservedOrUDC_Big5)
	makeDBCSCharClassifyTable(output, ['cp1361']) # Johab

	output.pop()
	Regenerate(filename, "//dbcs", output)

if __name__ == '__main__':
	buildANSICharClassifyTable('../../src/EditEncoding.c')
	updateCharClassifyTable("../src/CharClassify.cxx", "../src/CharClassify.h")
	updateDBCSCharClassifyTable("../src/CharClassify.cxx")
	updateCharacterCategoryTable("../lexlib/CharacterCategory.cxx")
