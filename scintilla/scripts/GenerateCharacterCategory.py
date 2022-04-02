# Script to generate CharacterCategory.cxx from Python's Unicode data
# Should be run rarely when a Python with a new version of Unicode data is available.

import codecs
import platform
import unicodedata
from enum import IntEnum

from FileGenerator import Regenerate
from MultiStageTable import *
from UnicodeData import *

class CharacterClass(IntEnum):
	Space = 0
	NewLine = 1
	Punctuation = 2
	Word = 3
	CJKWord = 4

CharacterClassOrder = [CharacterClass.CJKWord, CharacterClass.Word, CharacterClass.Punctuation, CharacterClass.NewLine]
def GetPreferredCharacterClass(values):
	for value in CharacterClassOrder:
		if value in values:
			return value
	# Cn
	return CharacterClass.Space

# https://en.wikipedia.org/wiki/Unicode_character_property
# Document::WordCharacterClass()
CharClassifyMap = {
	CharacterClass.Space: [
		'Zs',
		# Other
		'Cc',
		'Cf',
		'Cs',
		'Co',
		'Cn',
	],
	CharacterClass.NewLine: [
		'Zl',
		'Zp',
	],
	CharacterClass.Word: [
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
	CharacterClass.Punctuation: [
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

CategoryClassifyMap = {}
for key, items in CharClassifyMap.items():
	for category in items:
		CategoryClassifyMap[category] = key


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
	if category not in CharClassifyMap[CharacterClass.Word]:
		return False

	for block in CJKBlockList:
		if ch >= block[0] and ch <= block[1]:
			return True

	return False

def bytesToHex(b):
	return ''.join(f'\\x{ch:02X}' for ch in b)

SBCSCodePageList = [
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

def buildFoldDisplayEllipsis():
	# Interpunct https://en.wikipedia.org/wiki/Interpunct
	defaultText = '\u00B7' * 3	# U+00B7 Middle Dot
	fallbackText = '.' * 3		# U+002E Full Stop
	fallbackJIS = '\uFF65' * 3	# U+FF65 Halfwidth Katakana Middle Dot

	# DBCS
	encodingList = [
		('cp932', 932, 'Shift-JIS'),
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

	utf8Text = defaultText.encode('utf-8')
	utf8Text = bytesToHex(utf8Text)

	output = []
	output.append("const char* GetFoldDisplayEllipsis(UINT cpEdit, UINT acp) {")
	output.append("\tswitch (cpEdit) {")
	output.append("\tcase SC_CP_UTF8:")
	output.append(f'\t\treturn "{utf8Text}";')
	for key, values in result.items():
		for codepage, comment in values:
			output.append(f"\tcase {codepage}: // {comment}")
		output.append(f'\t\treturn "{key}";')
	output.append("\t}")

	result = {}
	for encoding, codepage, comment in SBCSCodePageList:
		try:
			value = defaultText.encode(encoding)
			value = bytesToHex(value)
			result[codepage] = value
		except UnicodeEncodeError:
			pass

	assert len(set(result.values())) == 1
	assert len(result) == len(SBCSCodePageList) - 1
	assert 874 not in result

	output.append("\t// SBCS")
	lines = f'''cpEdit = acp - 1250;
if (cpEdit <= 1258 - 1250) {{
	return "{result[1250]}";
}}
return "{fallbackText}";'''.splitlines()
	output.extend('\t' + line for line in lines)
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

	cc = CategoryClassifyMap[category]
	return int(cc)

def buildCharClassify(cp):
	decode = codecs.getdecoder(cp)
	result = {}
	for ch in range(128, 256):
		cc = getCharClassify(decode, ch)
		result[ch] = cc

	output = [0] * 32
	for ch, value in result.items():
		ch -= 128
		output[ch >> 2] |= value << (2 * (ch & 3))

	return output

def buildANSICharClassifyTable(filename):
	result = {}
	offset = 0
	for encoding, codepage, comment in SBCSCodePageList:
		data = buildCharClassify(encoding)
		result[codepage] = { 'data': data, 'offset': offset, 'codepage': [(codepage, comment)]}
		offset += len(data)

	output = [f"// Created with Python {platform.python_version()}, Unicode {unicodedata.unidata_version}"]
	output.append("static const uint8_t ANSICharClassifyTable[] = {")
	for item in result.values():
		for page in item['codepage']:
			output.append('// ' + page[1])
		data = item['data']
		output.extend(dumpArray(data, 16, '0x%02X'))
	output.append("};")
	output.append("")

	output.append("static inline const uint8_t* GetANSICharClassifyTable(UINT acp, int *length) {")
	lines = f"""const UINT diff = acp - 1250;
if (diff <= 1258 - 1250) {{
	return ANSICharClassifyTable + diff*32;
}}
if (acp == 874) {{
	return ANSICharClassifyTable + {result[874]['offset']};
}}
*length = 0;
return NULL;
""".splitlines()
	output.extend('\t' + line for line in lines)
	output.append("}")

	output.append("")
	ellipsis = buildFoldDisplayEllipsis()
	output.extend(ellipsis)

	print('ANSICharClassifyTable:', len(result), len(SBCSCodePageList))
	Regenerate(filename, "//", output)

def updateCharClassifyTable(filename, headfile):
	indexTable = [0] * UnicodeCharacterCount
	for ch in range(UnicodeCharacterCount):
		uch = chr(ch)
		category = unicodedata.category(uch)
		value = CategoryClassifyMap[category]
		if isCJKCharacter(category, ch):
			value = CharacterClass.CJKWord
		indexTable[ch] = int(value)

	output = [f"// Created with Python {platform.python_version()}, Unicode {unicodedata.unidata_version}"]
	head_output = output[:]

	valueBit, totalBit, data = runLengthEncode('CharClassify Unicode BMP', indexTable[:BMPCharacterCharacterCount])
	assert valueBit == 3
	assert totalBit == 16
	output.append('const uint16_t CharClassifyRLE_BMP[] = {')
	output.extend(dumpArray(data, 20))
	output.append("};")
	output.append("")
	output.append("}") # namespace
	output.append("")

	config = {
		'tableVarName': 'CharClassify::CharClassifyTable',
		'tableName': 'CharClassifyTable',
		'function': """static CharacterClass ClassifyCharacter(uint32_t ch) noexcept {
	if (ch < sizeof(classifyMap)) {
		return static_cast<CharacterClass>(classifyMap[ch]);
	}
	if (ch > maxUnicode) {
		// Cn
		return CharacterClass::space;
	}

	ch -= sizeof(classifyMap);""",
		'returnType': 'CharacterClass'
	}

	table = indexTable[BMPCharacterCharacterCount:]
	data, function = buildMultiStageTable('CharClassify Unicode', table, config=config, level=3)
	output.extend(data)
	head_output.extend('\t' + line for line in function)

	Regenerate(filename, "//", output)
	Regenerate(headfile, "//", head_output)

def updateCharacterCategoryTable(filename):
	categories = findCategories("../lexlib/CharacterCategory.h")
	output = [f"// Created with Python {platform.python_version()}, Unicode {unicodedata.unidata_version}"]

	indexTable = [0] * UnicodeCharacterCount
	for ch in range(UnicodeCharacterCount):
		uch = chr(ch)
		category = unicodedata.category(uch)
		value = categories.index(category)
		indexTable[ch] = value

	# the sentinel value is used to simplify CharacterMap::Optimize()
	sentinel = UnicodeCharacterCount*32 + categories.index('Cn')
	valueBit, rangeList = rangeEncode('catRanges', indexTable, sentinel=sentinel)
	assert valueBit == 5
	output.append("#if CharacterCategoryUseRangeList")
	output.append("const int catRanges[] = {")
	#output.extend(f"{value}," for value in rangeList)
	output.append("};")
	output.append("")
	output.append("#else")

	config = {
		'tableName': 'catTable',
		'function': """CharacterCategory CategoriseCharacter(int character) noexcept {
	if (character < 0 || character > maxUnicode) {
		return ccCn;
	}""",
		'returnType': 'CharacterCategory',
	}
	table, function = buildMultiStageTable('CharacterCategory', indexTable, config=config, level=3)
	output.append("")
	output.extend(table)

	valueBit, totalBit, data = runLengthEncode('CharacterCategory BMP', indexTable[:BMPCharacterCharacterCount])
	assert valueBit == 5
	assert totalBit == 16
	output.append("")
	output.append('const uint16_t CatTableRLE_BMP[] = {')
	output.extend(dumpArray(data, 20))
	output.append("};")

	output.append("")
	output.append("#endif")

	Regenerate(filename, "//", output)
	Regenerate(filename, "//function", function)

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
			return int(CharacterClass.CJKWord)
	else:
		# treat reserved or user-defined characters as word
		if isReservedOrUDC and isReservedOrUDC(ch, buf):
			return int(CharacterClass.CJKWord)
		# undefined, treat as Cn, Not assigned
		category = 'Cn'

	cc = CategoryClassifyMap[category]
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
		indexTable[ch] = int(GetPreferredCharacterClass(result[ch]))

	suffix = '_' + encodingList[0].upper()
	head = 'CharClassify' + suffix

	valueBit, totalBit, data = runLengthEncode(head, indexTable)
	assert valueBit == 3
	if encodingList[0] == 'cp1361':
		#skipBlockEncode(head, indexTable, tableName=head)
		data = runBlockEncode(head, indexTable, tableName=head)
		output.extend(data)
	else:
		assert totalBit == 16
		output.append(f'const uint16_t CharClassifyRLE{suffix}[] = {{')
		output.extend(dumpArray(data, 20))
		output.append("};")
	output.append("")

	if False:
		config = {
			'tableName': 'CharClassifyTable' + suffix,
			'function': f"""CharacterClass ClassifyCharacter{suffix}(uint32_t ch) noexcept {{
	if (ch > maxDBCSCharacter) {{
		// Cn
		return CharacterClass::space;
	}}
	""",
			'returnType': 'CharacterClass'
		}

		table, function = buildMultiStageTable(head, indexTable, config)
		output.extend(table)
		output.append('')
		output.extend(function)
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
		if ch >= block[0] and ch <= block[1]:
			return True
	return False

def updateDBCSCharClassifyTable(filename):
	output = [f"// Created with Python {platform.python_version()}, Unicode {unicodedata.unidata_version}"]

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
