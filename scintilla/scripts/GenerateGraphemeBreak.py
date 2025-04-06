# script to generate grapheme cluster boundary data.
from enum import IntEnum

from FileGenerator import Regenerate
from MultiStageTable import *
from UnicodeData import *

# Unicode Text Segmentation
# https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Break_Property_Values
# https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakTest.html
class GraphemeBreakProperty(IntEnum):
	Other = 0
	Control = 1
	Extend = 2
	ZeroWidthJoiner = 3
	Prepend = 4
	HangulL = 5
	HangulV = 6
	HangulT = 7
	HangulLV = 8
	HangulLVT = 9
	ExtendedPictographic = 10
	RegionalIndicator = 11
	# Indic_Conjunct_Break
	ConjunctLinker = 12
	LinkingConsonant = 13
	# merged property
	SpacingMark = 14
	CR = 15
	LF = 16

# https://www.unicode.org/reports/tr35/tr35-general.html#segmentations
# https://github.com/unicode-org/cldr/blob/main/common/segments/root.xml
# https://www.unicode.org/reports/tr51/#Emoji_Properties
GraphemeBreakPropertyMap = GraphemeBreakProperty.__members__ | {
	'ZWJ': GraphemeBreakProperty.ZeroWidthJoiner,
	'L': GraphemeBreakProperty.HangulL,
	'V': GraphemeBreakProperty.HangulV,
	'T': GraphemeBreakProperty.HangulT,
	'LV': GraphemeBreakProperty.HangulLV,
	'LVT': GraphemeBreakProperty.HangulLVT,
	'Extended_Pictographic': GraphemeBreakProperty.ExtendedPictographic,
	'ExtPict': GraphemeBreakProperty.ExtendedPictographic,
	'Regional_Indicator': GraphemeBreakProperty.RegionalIndicator,
	'RI': GraphemeBreakProperty.RegionalIndicator,
	'Consonant': GraphemeBreakProperty.LinkingConsonant,
	'Virama': GraphemeBreakProperty.ConjunctLinker,
}

# https://www.unicode.org/reports/tr44/#Indic_Conjunct_Break
def updateIndicConjunctBreak(graphemeBreakTable):
	# https://www.unicode.org/Public/UCD/latest/ucd/IndicSyllabicCategory.txt
	indicConjunctBreak = ['Other'] * UnicodeCharacterCount
	version, propertyList = readUnicodePropertyFile('IndicSyllabicCategory.txt')
	flattenUnicodePropertyTable(indicConjunctBreak, propertyList)
	defaultValue = int(GraphemeBreakProperty.Other)
	extend = int(GraphemeBreakProperty.Extend)
	linker = int(GraphemeBreakProperty.ConjunctLinker)
	for index, conjunct in enumerate(indicConjunctBreak):
		grapheme = graphemeBreakTable[index]
		if grapheme == defaultValue:
			assert conjunct != 'Virama'
			grapheme = int(GraphemeBreakPropertyMap.get(conjunct, grapheme))
		elif grapheme == extend:
			if conjunct == 'Virama':
				grapheme = linker
		graphemeBreakTable[index] = grapheme

graphemeClusterBoundary = [0x3ffff] * (max(GraphemeBreakProperty.__members__.values()) + 1)
# https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules
def buildGraphemeClusterBoundary():
	table = graphemeClusterBoundary

	notBreak = {
	'Other': 'Extend SpacingMark ZWJ ConjunctLinker',
	'CR': 'LF',
	'Extend': 'Extend SpacingMark ZWJ ConjunctLinker',
	'ZWJ': 'Extend SpacingMark ZWJ ConjunctLinker ExtPict Consonant',
	'Prepend': 'Other Extend SpacingMark ZWJ ConjunctLinker RI Prepend L V T LV LVT ExtPict Consonant',
	'SpacingMark': 'Extend SpacingMark ZWJ ConjunctLinker',
	'L': 'Extend SpacingMark ZWJ ConjunctLinker L V LV LVT',
	'V': 'Extend SpacingMark ZWJ ConjunctLinker V T',
	'T': 'Extend SpacingMark ZWJ ConjunctLinker T',
	'LV': 'Extend SpacingMark ZWJ ConjunctLinker V T',
	'LVT': 'Extend SpacingMark ZWJ ConjunctLinker T',
	'ExtPict': 'Extend SpacingMark ZWJ ConjunctLinker',
	'RI': 'Extend SpacingMark ZWJ ConjunctLinker RI',
	'ConjunctLinker': 'Extend SpacingMark ZWJ ConjunctLinker Consonant',
	'Consonant': 'Extend SpacingMark ZWJ ConjunctLinker',
	}

	for key, row in notBreak.items():
		first = GraphemeBreakPropertyMap[key]
		row = row.split()
		for item in row:
			second = GraphemeBreakPropertyMap[item]
			table[first] &= ~(1 << second)

	for index, value in enumerate(table):
		prev = GraphemeBreakProperty(index)
		for j in range(index + 1, len(table)):
			if value == table[j]:
				first = tuple((item >> index) & 1 for item in table)
				second = tuple((item >> j) & 1 for item in table)
				if first == second:
					current = GraphemeBreakProperty(j)
					print('same row and column:', prev.name, current.name)

def findLongestCharacterSequence(path):
	longestSequenceCount = 0
	longestSequenceBytes = 0
	version, propertyList = readUnicodePropertyFile(path)
	for codeList in propertyList.values():
		for sequence in codeList:
			character = ''.join(chr(code) for code in sequence)
			assert len(sequence) == len(character)
			longestSequenceCount = max(longestSequenceCount, len(sequence))
			longestSequenceBytes = max(longestSequenceBytes, len(character.encode('utf-8')))

	print(path, 'longest character sequence:', longestSequenceCount, longestSequenceBytes)
	return longestSequenceCount, longestSequenceBytes

def testGraphemeBreak(path, graphemeBreakTable):
	opportunity = '×÷'
	allow = opportunity[1]
	total = 0
	fail = [0, 0]
	ignore = 0
	with open(path, encoding='utf-8') as fd:
		lineno = 0
		indent = ' '*4
		for line in fd.readlines():
			lineno += 1
			line = line.strip()
			if not line or line[0] == '#':
				continue

			sequence = line.split('#', 2)[0].split()
			# break at the start and end of text
			assert sequence[0] == allow, (lineno, sequence)
			assert sequence[-1] == allow, (lineno, sequence)
			for index in range(1, len(sequence) - 2, 2):
				ch = sequence[index]
				official = sequence[index + 1]
				chNext = sequence[index + 2]
				if ch == '000D' and chNext == '000A':
					ignore += 1
					continue
				prop = GraphemeBreakProperty(graphemeBreakTable[int(ch, 16)])
				propNext = GraphemeBreakProperty(graphemeBreakTable[int(chNext, 16)])
				result = (graphemeClusterBoundary[prop] >> propNext) & 1
				value = opportunity[result]
				total += 1
				if value != official:
					fail[result ^ 1] += 1
					print(f'test fail on line {lineno}: {ch} {official} {chNext} => {prop.name} {value} {propNext.name}')
					print(f'{indent}{line}')
	print(f'{path} total test: {total}, failed {sum(fail)}: {opportunity[0]} {fail[0]}, {opportunity[1]} {fail[1]}, ignored: {ignore}')

def updateGraphemeBreakTable(headerFile, sourceFile):
	defaultValue = int(GraphemeBreakProperty.Other)
	graphemeBreakTable = [defaultValue] * UnicodeCharacterCount
	# https://www.unicode.org/Public/UCD/latest/ucd/emoji/emoji-data.txt
	version, propertyList = readUnicodePropertyFile('emoji-data.txt')
	updateUnicodePropertyTable(graphemeBreakTable, GraphemeBreakPropertyMap, propertyList)
	# https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakProperty.txt
	version, propertyList = readUnicodePropertyFile('GraphemeBreakProperty.txt')
	updateUnicodePropertyTable(graphemeBreakTable, GraphemeBreakPropertyMap, propertyList)
	updateIndicConjunctBreak(graphemeBreakTable)

	tableSize = getMinTableSize(graphemeBreakTable, defaultValue)
	print(f'Grapheme Break table size: {tableSize}, last value: {GraphemeBreakProperty(graphemeBreakTable[tableSize - 1]).name}')

	# https://www.unicode.org/emoji/charts/full-emoji-modifiers.html
	# https://www.unicode.org/Public/emoji/latest/emoji-zwj-sequences.txt
	longestSequenceCount, longestSequenceBytes = findLongestCharacterSequence('emoji-zwj-sequences.txt')
	buildGraphemeClusterBoundary()

	valueMap = {
		int(GraphemeBreakProperty.CR) : int(GraphemeBreakProperty.Control),
		int(GraphemeBreakProperty.LF) : int(GraphemeBreakProperty.Control),
		int(GraphemeBreakProperty.SpacingMark) : int(GraphemeBreakProperty.Extend),
	}
	graphemeBreakTable = [valueMap.get(value, value) for value in graphemeBreakTable]
	# https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakTest.txt
	testGraphemeBreak('GraphemeBreakTest.txt', graphemeBreakTable)
	graphemeBreakTable = graphemeBreakTable[:tableSize]

	output = []
	output.append('enum class GraphemeBreakProperty {')
	propNext = GraphemeBreakProperty(max(graphemeBreakTable))
	for prop in GraphemeBreakProperty.__members__.values():
		output.append(f'\t{prop.name} = {prop.value},')
		if prop == propNext:
			break
	output.append('\tForwardSentinel = Prepend,')
	output.append('\tBackwardSentinel = Extend,')
	output.append('};')

	output.append('')
	output.append(f'constexpr int maxUnicodeGraphemeBreakCharacter = {hex(len(graphemeBreakTable))};')
	output.append(f'constexpr int longestUnicodeCharacterSequenceCount = {longestSequenceCount};')
	output.append(f'constexpr int longestUnicodeCharacterSequenceBytes = {longestSequenceBytes};')

	output.append('')
	output.append('constexpr uint16_t graphemeClusterBoundary[] = {')
	table = graphemeClusterBoundary[:int(propNext) + 1]
	output.extend(bitValue(value & 0xffff) + ', // ' + GraphemeBreakProperty(index).name for index, value in enumerate(table))
	output.append('};')

	output.append("""
constexpr bool IsGraphemeClusterBoundary(GraphemeBreakProperty prev, GraphemeBreakProperty current) noexcept {
	return (graphemeClusterBoundary[static_cast<int>(prev)] >> (static_cast<int>(current))) & true;
}""")

	sentinel = (UnicodeCharacterCount << 4) | GraphemeBreakProperty.Other
	valueBit, rangeList = rangeEncode('Unicode Grapheme Break range', graphemeBreakTable, sentinel=sentinel)
	assert valueBit == 4

	commonCharacterCount = 0x4000 # graphemeMap
	valueBit, totalBit, data = runLengthEncode('Unicode Grapheme Break BMP', graphemeBreakTable[:commonCharacterCount])
	assert valueBit == 4
	assert totalBit == 16
	cxx_output = ['const uint16_t GraphemeBreakRLE_BMP[] = {']
	cxx_output.extend(dumpArray(data, 20))
	cxx_output.append("};")
	cxx_output.append("")
	cxx_output.append("}") # namespace
	cxx_output.append("")

	config = {
		'tableName': 'GraphemeBreakTable',
		'tableVarName': 'CharClassify::GraphemeBreakTable',
		'function': """static GraphemeBreakProperty GetGraphemeBreakProperty(uint32_t ch) noexcept {
	if (ch < sizeof(graphemeMap)) {
		return static_cast<GraphemeBreakProperty>(graphemeMap[ch]);
	}
	if (ch >= maxUnicodeGraphemeBreakCharacter) {
		return GraphemeBreakProperty::Other;
	}

	ch -= sizeof(graphemeMap);""",
		'returnType': 'GraphemeBreakProperty'
	}

	Regenerate(headerFile, "//grapheme type", output)
	table, function = buildMultiStageTable('Unicode Grapheme Break', graphemeBreakTable[commonCharacterCount:], config=config, level=3)
	output = ['\t' + line for line in function]
	cxx_output.extend(table)
	Regenerate(headerFile, "//grapheme function", output)
	Regenerate(sourceFile, "//grapheme table", cxx_output)

if __name__ == '__main__':
	# parseSegmentationChart('GraphemeBreakTest.html')
	updateGraphemeBreakTable('../src/CharClassify.h', '../src/CharClassify.cxx')
