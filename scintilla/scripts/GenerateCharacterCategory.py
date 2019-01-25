#!/usr/bin/env python3
# Script to generate CharacterCategory.cxx from Python's Unicode data
# Should be run rarely when a Python with a new version of Unicode data is available.
# Requires Python 3.3 or later
# Should not be run with old versions of Python.

import codecs, os, platform, sys, unicodedata
from collections import OrderedDict
from enum import IntEnum
import math
from FileGenerator import Regenerate

class CharClassify(IntEnum):
	ccSpace = 0
	ccNewLine = 1
	ccWord = 2
	ccPunctuation = 3
	ccCJKWord = 4

	OnlyWord = 1 << ccWord
	OnlyWordPunctuation = (1 << ccWord) | (1 << ccPunctuation)

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


# https://en.wikipedia.org/wiki/Unicode_block
# https://en.wikipedia.org/wiki/Plane_(Unicode)#Basic_Multilingual_Plane
# https://en.wikipedia.org/wiki/CJK_Unified_Ideographs
# https://en.wikipedia.org/wiki/Katakana
# https://en.wikipedia.org/wiki/Hangul
CJKBlockList = [
	(0x4E00, 0x9FFF), # U+4E00..U+9FFF CJK Unified Ideographs
	(0x3400, 0x4DBF), # U+3400..U+4DBF CJK Unified Ideographs Extension A
	(0xF900, 0xFAFF), # U+F900..U+FAFF CJK Compatibility Ideographs
	# surrogate pair
	(0x20000, 0x2A6DF), # U+20000..U+2A6DF CJK Unified Ideographs Extension B
	(0x2A700, 0x2B73F), # U+2A700..U+2B73F CJK Unified Ideographs Extension C
	(0x2B740, 0x2B81F), # U+2B740..U+2B81F CJK Unified Ideographs Extension D
	(0x2B820, 0x2CEAF), # U+2B820..U+2CEAF CJK Unified Ideographs Extension E
	(0x2CEB0, 0x2EBEF), # U+2CEB0..U+2EBEF CJK Unified Ideographs Extension F
	(0x2F800, 0x2FA1F), # U+2F800..U+2FA1F CJK Compatibility Ideographs Supplement

	(0x2E80, 0x2EFF), # U+2E80..U+2EFF CJK Radicals Supplement
	(0x2F00, 0x2FDF), # U+2F00..U+2FDF Kangxi Radicals
	(0x3000, 0x303F), # U+3000..U+303F CJK Symbols and Punctuation
	#(0x3100, 0x312F), # U+3100..U+312F Bopomofo
	#(0x31A0, 0x31BF), # U+31A0..U+31BF Bopomofo Extended
	(0x31C0, 0x31EF), # U+31C0..U+31EF CJK Strokes
	(0x3200, 0x32FF), # U+3200..U+32FF Enclosed CJK Letters and Months
	(0x3300, 0x33FF), # U+3300..U+33FF CJK Compatibility
	(0xFE30, 0xFE4F), # U+FE30..U+FE4F CJK Compatibility Forms
	#(0xFF5F, 0xFFEF), # U+FF5F..U+FFEF Halfwidth and Fullwidth Forms (none ASCII)
	# Japanese
	(0x3040, 0x309F), # U+3040..U+309F Hiragana
	(0x30A0, 0x30FF), # U+30A0..U+30FF Katakana
	(0x3190, 0x319F), # U+3190..U+319F Kanbun
	(0x31F0, 0x31FF), # U+31F0..U+31FF Katakana Phonetic Extensions
	(0xFF65, 0xFF9F), # U+FF65..U+FF9F Halfwidth Katakana
	(0x1B000, 0x1B0FF), # U+1B000..U+1B0FF Kana Supplement
	(0x1B100, 0x1B12F), # U+1B100..U+1B12F Kana Extended-A
	# Korean
	(0xAC00, 0xD7AF), # U+AC00..U+D7AF Hangul Syllables
	(0x1100, 0x11FF), # U+1100..U+11FF Hangul Jamo
	(0x3130, 0x318F), # U+3130..U+318F Hangul Compatibility Jamo
	(0xA960, 0xA97F), # U+A960..U+A97F Hangul Jamo Extended-A
	(0xD7B0, 0xD7FF), # U+D7B0..U+D7FF Hangul Jamo Extended-B
	(0xFFA0, 0xFFDC), # U+FFA0..U+FFDC Halfwidth Compatibility Jamo
	# Other Chinese characters
	(0xA000, 0xA48F), # U+A000..U+A48F Yi Syllables
	(0xA490, 0xA4CF), # U+A490..U+A4CF Yi Radicals
	(0x17000, 0x187FF), # U+17000..U+187FF Tangut
	(0x18800, 0x18AFF), # U+18800..U+18AFF Tangut Components
	(0x1B170, 0x1B2FF), # U+1B170..U+1B2FF Nushu
]

def findCategories(filename):
	with codecs.open(filename, "r", "UTF-8") as infile:
		lines = [x.strip() for x in infile.readlines() if "\tcc" in x]
	values = "".join(lines).replace(" ","").split(",")
	print(values)
	return [v[2:] for v in values]

def isCJKLetter(category, ch):
	if category not in CharClassifyMap[CharClassify.ccWord]:
		return False

	for block in CJKBlockList:
		if ch >= block[0] and ch <= block[1]:
			return True

	return False

def updateCharacterCategory(filename):
	categories = findCategories("../lexlib/CharacterCategory.h")
	values = ["// Created with Python %s,  Unicode %s" % (
		platform.python_version(), unicodedata.unidata_version)]
	category = unicodedata.category(chr(0))
	startRange = 0
	for ch in range(sys.maxunicode):
		uch = chr(ch)
		current = unicodedata.category(uch)
		if current != category:
			value = startRange * 32 + categories.index(category)
			values.append("%d," % value)
			category = current
			startRange = ch
	value = startRange * 32 + categories.index(category)
	values.append("%d," % value)
	print('catRanges:', len(values), 4*len(values)/1024, math.ceil(math.log2(len(values))))

	Regenerate(filename, "//", values)

def getCharClassify(ch):
	if not ch:
		# undefined, treat as Cn, Not assigned
		category = 'Cn'
	else:
		category = unicodedata.category(ch)
	cc = ClassifyMap[category]
	return cc

def buildCharClassify(cp):
	decode = codecs.getdecoder(cp)
	result = {}
	mask = 0
	for ch in range(128, 256):
		uch = decode(bytes([ch]), 'ignore')
		cc = int(getCharClassify(uch[0]))
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

	result = OrderedDict()
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

	output = ["// Created with Python %s,  Unicode %s" % (
		platform.python_version(), unicodedata.unidata_version)]

	output.append("static const UINT8 ANSICharClassifyTable[] = {")
	for item in result.values():
		for page in item['codepage']:
			output.append('// ' + page[1])
		data = item['data']
		output.append(', '.join('0x%02X' % ch for ch in data[:16]) + ',')
		if len(data) > 16:
			output.append(', '.join('0x%02X' % ch for ch in data[16:]) + ',')
	output.append("};\n")

	output.append("static const UINT8* GetANSICharClassifyTable(UINT cp, int *length) {")
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

	print('ANSICharClassifyTable:', len(result), len(encodingList))
	Regenerate(filename, "//", output)

# splitbins() is based on Python source
# https://github.com/python/cpython/blob/master/Tools/unicode/makeunicodedata.py

def getsize(data):
	# return smallest possible integer size for the given array
	maxdata = max(data)
	if maxdata < 256:
		return 1
	elif maxdata < 65536:
		return 2
	else:
		return 4

def splitbins(t, second=False):
	"""t -> (t1, t2, shift).  Split a table to save space.

	t is a sequence of ints.  This function can be useful to save space if
	many of the ints are the same.	t1 and t2 are lists of ints, and shift
	is an int, chosen to minimize the combined size of t1 and t2 (in C
	code), and where for each i in range(len(t)),
		t[i] == t2[(t1[i >> shift] << shift) + (i & mask)]
	where mask is a bitmask isolating the last "shift" bits.
	"""

	# the most we can shift n and still have something left
	maxshift = math.floor(math.log2(len(t)))

	total = sys.maxsize	 # smallest total size so far
	t = tuple(t)	# so slices can be dict keys
	for shift in range(maxshift + 1):
		t1 = []
		t2 = []
		size = 2**shift
		bincache = {}
		for i in range(0, len(t), size):
			part = t[i:i+size]
			index = bincache.get(part)
			if index is None:
				index = len(t2)
				bincache[part] = index
				t2.extend(part)
			t1.append(index >> shift)
		# determine memory size
		b = len(t1)*getsize(t1)
		if second:
			t3, t4, shift2 = splitbins(t2, False)
			b += len(t3)*getsize(t3) + len(t4)*getsize(t4)
		else:
			b += len(t2)*getsize(t2)
		if b < total:
			if second:
				best = t1, t3, t4, shift, shift2
			else:
				best = t1, t2, shift
			total = b
	return best

def compressIndexTable(head, indexTable, args):
	indexA, indexC, indexD, shiftA, shiftC = splitbins(indexTable, True)
	print(f'{head}:', len(indexA), max(indexA), len(indexC), max(indexC), len(indexD), shiftA, shiftC)

	sizeA = getsize(indexA)
	sizeC = getsize(indexC);
	total = len(indexA)*sizeA + len(indexC)*sizeC + len(indexD)
	print(f'{head} total size:', total/1024)

	maskA = (1 << shiftA) - 1
	maskC = (1 << shiftC) - 1
	table, function = '', ''
	for ch in range(len(indexTable)):
		i = (indexA[ch >> shiftA] << shiftA) + (ch & maskA)
		i = (indexC[i >> shiftC] << shiftC) + (i & maskC)
		value = indexD[i]
		expect = indexTable[ch]
		if value != expect:
			print(f'{head} verify fail:', '%04X, expect: %d, got: %d' % (head, ch, expect, value))
			return table, function

	# one table
	if sizeA == sizeC == 1:
		output = []
		output.append("const unsigned char %s[] = {" % args['table'])
		output.append(', '.join(str(i) for i in indexA) + ',')
		output.append(', '.join(str(i) for i in indexC) + ',')
		output.append(', '.join(str(i) for i in indexD) + ',')
		output.append("};")
		table = '\n'.join(output)

		args.update({
			'shiftA': shiftA,
			'maskA': maskA,
			'offsetC': len(indexA),
			'shiftC': shiftC,
			'maskC': maskC,
			'offsetD': len(indexA) + len(indexC)
		})
		function = """{function}

	ch = ({table}[ch >> {shiftA}] << {shiftA}) + (ch & {maskA});
	ch = ({table}[{offsetC} + (ch >> {shiftC})] << {shiftC}) + (ch & {maskC});
	return static_cast<{returnType}>({table}[{offsetD} + ch]);
}}""".format(**args)
	# two tables
	elif sizeA == 1:
		assert sizeC == 2
		output = []
		output.append("const unsigned char %s1[] = {" % args['table'])
		output.append(', '.join(str(i) for i in indexA) + ',')
		output.append(', '.join(str(i) for i in indexD) + ',')
		output.append("};")
		output.append("const unsigned short %s2[] = {" % args['table'])
		output.append(', '.join(str(i) for i in indexC) + ',')
		output.append("};")
		table = '\n'.join(output)

		args.update({
			'shiftA': shiftA,
			'maskA': maskA,
			'shiftC': shiftC,
			'maskC': maskC,
			'offsetD': len(indexA)
		})
		function = """{function}

	ch = ({table}1[ch >> {shiftA}] << {shiftA}) + (ch & {maskA});
	ch = ({table}2[(ch >> {shiftC})] << {shiftC}) + (ch & {maskC});
	return static_cast<{returnType}>({table}1[{offsetD} + ch]);
}}""".format(**args)
	else:
		print(f'{head} unknown bin size')
	return table, function

def updateCharClassifyTable(filename):
	maxUnicode = sys.maxunicode + 1
	indexTable = [0] * maxUnicode
	for ch in range(maxUnicode):
		uch = chr(ch)
		category = unicodedata.category(uch)
		value = ClassifyMap[category]
		if isCJKLetter(category, ch):
			value = CharClassify.ccCJKWord
		indexTable[ch] = int(value)

	output = ["""// Created with Python %s,  Unicode %s
namespace {
constexpr unsigned int maxUnicode = 0x10ffff;
""" % (platform.python_version(), unicodedata.unidata_version)]

	args = {
		'table': 'CharClassifyTable',
		'function': """CharClassify::cc CharClassify::ClassifyCharacter(unsigned int ch) noexcept {
	if (ch > maxUnicode) {
		// Cn
		return ccSpace;
	}""",
		'returnType': 'cc'
	}

	table, function = compressIndexTable('CharClassify Unicode', indexTable, args)
	output.append(table)
	output.append('')
	output.append("}\n") # namespace
	output.append(function)

	Regenerate(filename, "//", output)

def updateCharacterCategoryTable(filename):
	categories = findCategories("../lexlib/CharacterCategory.h")

	maxUnicode = sys.maxunicode + 1
	indexTable = [0] * maxUnicode
	for ch in range(maxUnicode):
		uch = chr(ch)
		category = unicodedata.category(uch)
		value = categories.index(category)
		indexTable[ch] = value

	output = ["// Created with Python %s,  Unicode %s" % (
		platform.python_version(), unicodedata.unidata_version)]

	args = {
		'table': 'CharacterCategoryTable',
		'function': """CharacterCategory CategoriseCharacter(unsigned int ch) noexcept {
	if (ch > maxUnicode) {
		// Cn
		return ccCn;
	}""",
		'returnType': 'CharacterCategory'
	}

	table, function = compressIndexTable('CharacterCategoryTable', indexTable, args)
	output.append(table)
	output.append('')
	output.append("}\n") # namespace
	output.append(function)

	Regenerate(filename, "//", output)

if __name__ == '__main__':
	buildANSICharClassifyTable('../../src/EditEncoding.c')
	updateCharClassifyTable("../src/CharClassify.cxx")
	#updateCharacterCategoryTable("../lexlib/CharacterCategory.cxx")
	#updateCharacterCategory("../lexlib/CharacterCategory.cxx")
