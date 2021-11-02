#!/usr/bin/env python3
# script to generate grapheme cluster boundary data.
# https://www.unicode.org/reports/tr41/
from enum import IntEnum

from MultiStageTable import *
from UnicodeData import *

# Unicode Text Segmentation
# https://www.unicode.org/reports/tr29/#Grapheme_Cluster_Break_Property_Values
class GraphemeBreakProperty(IntEnum):
	Other = 0
	CR = 1
	LF = 2
	Control = 3
	Extend = 4
	RegionalIndicator = 5
	Prepend = 6
	SpacingMark = 7
	HangulL = 8
	HangulV = 9
	HangulT = 10
	HangulLV = 11
	HangulLVT = 12
	ExtendedPictographic = 13
	ZeroWidthJoiner = 14

GraphemeBreakPropertyMap = GraphemeBreakProperty.__members__ | {
	'Regional_Indicator': GraphemeBreakProperty.RegionalIndicator,
	'RI': GraphemeBreakProperty.RegionalIndicator,
	'L': GraphemeBreakProperty.HangulL,
	'V': GraphemeBreakProperty.HangulV,
	'T': GraphemeBreakProperty.HangulT,
	'LV': GraphemeBreakProperty.HangulLV,
	'LVT': GraphemeBreakProperty.HangulLVT,
	'Extended_Pictographic': GraphemeBreakProperty.ExtendedPictographic,
	'ExtPict': GraphemeBreakProperty.ExtendedPictographic,
	'ZWJ': GraphemeBreakProperty.ZeroWidthJoiner,
}

graphemeClusterBoundary = [0xffff] * (max(GraphemeBreakProperty.__members__.values()) + 1)
def buildGraphemeClusterBoundary():
	table = graphemeClusterBoundary

	# https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakTest.html
	notBreak = [
		['Other', 'Extend', 'SpacingMark', 'ZWJ'],
		['CR', 'LF'],
		['Extend', 'Extend', 'SpacingMark', 'ZWJ'],
		['RI', 'Extend', 'RI', 'SpacingMark', 'ZWJ'],
		['Prepend', 'Other', 'Extend', 'RI', 'Prepend', 'SpacingMark', 'L', 'V', 'T', 'LV', 'LVT', 'ExtPict', 'ZWJ'],
		['SpacingMark', 'Extend', 'SpacingMark', 'ZWJ'],
		['L', 'Extend', 'SpacingMark', 'L', 'V', 'LV', 'LVT', 'ZWJ'],
		['V', 'Extend', 'SpacingMark', 'V', 'T', 'ZWJ'],
		['T', 'Extend', 'SpacingMark', 'T', 'ZWJ'],
		['LV', 'Extend', 'SpacingMark', 'V', 'T', 'ZWJ'],
		['LVT', 'Extend', 'SpacingMark', 'T', 'ZWJ'],
		['ExtPict', 'Extend', 'SpacingMark', 'ZWJ'],
		#['ZWJ', 'Extend', 'SpacingMark', 'ZWJ'],
		['ZWJ', 'Extend', 'SpacingMark', 'ExtPict', 'ZWJ'],
	]

	for row in notBreak:
		first = GraphemeBreakPropertyMap[row[0]]
		for item in row[1:]:
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

def findLongestCharacterSquence(path):
	longestSquenceCount = 0
	longestSquenceBytes = 0
	version, propertyList = readUnicodePropertyFile(path)
	for codeList in propertyList.values():
		for squence in codeList:
			character = ''.join(chr(code) for code in squence)
			assert len(squence) == len(character)
			longestSquenceCount = max(longestSquenceCount, len(squence))
			longestSquenceBytes = max(longestSquenceBytes, len(character.encode('utf-8')))

	print(path, 'longest character squence:', longestSquenceCount, longestSquenceBytes)
	return longestSquenceCount, longestSquenceBytes

def testGraphemeBreak(path, graphemeBreakTable):
	opportunity = '×÷'
	allow = opportunity[1]
	totalCount = 0
	failCount = 0
	with open(path, encoding='utf-8') as fd:
		lineno = 0
		for line in fd.readlines():
			lineno += 1
			line = line.strip()
			if not line or line[0] == '#':
				continue

			squence = line.split('#', 2)[0].split()
			# break at the start and end of text
			assert squence[0] == allow, (lineno, squence)
			assert squence[-1] == allow, (lineno, squence)
			for index in range(1, len(squence) - 2, 2):
				ch = squence[index]
				offcial = squence[index + 1]
				chNext = squence[index + 2]
				prop = GraphemeBreakProperty(graphemeBreakTable[int(ch, 16)])
				propNext = GraphemeBreakProperty(graphemeBreakTable[int(chNext, 16)])
				value = opportunity[(graphemeClusterBoundary[prop] >> propNext) & 1]
				totalCount += 1
				if value != offcial:
					failCount += 1
					print(f'test fail on line {lineno}: {ch} {offcial} {chNext} => {prop.name} {value} {propNext.name}')
	print(f'{path} total test: {totalCount}, failed test: {failCount}')

def updateGraphemeBreakTable(filename):
	defaultValue = int(GraphemeBreakProperty.Other)
	graphemeBreakTable = [defaultValue] * UnicodeCharacterCount
	# https://www.unicode.org/Public/UCD/latest/ucd/emoji/emoji-data.txt
	version, propertyList = readUnicodePropertyFile('emoji-data.txt')
	updateUnicodePropertyTable(graphemeBreakTable, GraphemeBreakPropertyMap, propertyList)
	# https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakProperty.txt
	version, propertyList = readUnicodePropertyFile('GraphemeBreakProperty.txt')
	updateUnicodePropertyTable(graphemeBreakTable, GraphemeBreakPropertyMap, propertyList)

	tableSize = getMinTableSize(graphemeBreakTable, defaultValue)
	print(f'Grapheme Break table size: {tableSize}, last value: {GraphemeBreakProperty(graphemeBreakTable[tableSize - 1]).name}')

	# https://www.unicode.org/emoji/charts/full-emoji-modifiers.html
	# https://www.unicode.org/Public/emoji/latest/emoji-zwj-sequences.txt
	longestSquenceCount, longestSquenceBytes = findLongestCharacterSquence('emoji-zwj-sequences.txt')
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
	output.append('#pragma once')
	output.append("#include <algorithm>")
	output.append("#include <iterator>")
	output.append('')
	output.append('namespace {')
	output.append('')
	output.append('enum class GraphemeBreakProperty {')
	for prop in GraphemeBreakProperty.__members__.values():
		output.append(f'\t{prop.name} = {prop.value},')
	output.append(f'\tSentinel = Prepend,')
	output.append('};')

	output.append('')
	output.append(f'constexpr int maxUnicodeGraphemeBreakCharacter = {hex(len(graphemeBreakTable))};')
	output.append(f'constexpr int longestUnicodeCharacterSquenceCount = {longestSquenceCount};')
	output.append(f'constexpr int longestUnicodeCharacterSquenceBytes = {longestSquenceBytes};')

	output.append('')
	output.append('constexpr uint16_t graphemeClusterBoundary[] = {')
	output.extend(bitValue(value) + ', // ' + GraphemeBreakProperty(index).name for index, value in enumerate(graphemeClusterBoundary))
	output.append('};')

	output.append("""
constexpr bool IsGraphemeClusterBoundary(GraphemeBreakProperty prev, GraphemeBreakProperty current) noexcept {
	return (graphemeClusterBoundary[static_cast<int>(prev)] >> (static_cast<int>(current))) & true;
}
""")

	output.append('#define GraphemeBreakUseRangeList 0')
	output.append('#if GraphemeBreakUseRangeList')
	sentinel = (UnicodeCharacterCount << 4) | GraphemeBreakProperty.Other
	valueBit, rangeList = rangeEncode('Unicode Grapheme Break range', graphemeBreakTable, sentinel=sentinel)
	assert valueBit == 4
	output.append('constexpr int graphemeBreakMask = 15;')
	output.append('')
	output.append('const int graphemeBreakRanges[] = {')
	output.extend(hex(value) + ',' for value in rangeList)
	output.append("};")
	output.append("""
GraphemeBreakProperty GetGraphemeBreakProperty(int character) noexcept {
	if (character < 0 || character >= maxUnicodeGraphemeBreakCharacter) {
		return GraphemeBreakProperty::Other;
	}
	const int baseValue = character * (graphemeBreakMask + 1) + graphemeBreakMask;
	const int *placeAfter = std::lower_bound(graphemeBreakRanges, std::end(graphemeBreakRanges), baseValue);
	return static_cast<GraphemeBreakProperty>(*(placeAfter - 1) & graphemeBreakMask);
}""")

	output.append('')
	output.append('#else')
	config = {
		'tableName': 'graphemeBreakTable',
		'function': """GraphemeBreakProperty GetGraphemeBreakProperty(uint32_t ch) noexcept {
	if (ch >= maxUnicodeGraphemeBreakCharacter) {
		return GraphemeBreakProperty::Other;
	}
""",
		'returnType': 'GraphemeBreakProperty'
	}

	table, function = buildMultiStageTable('Unicode Grapheme Break', graphemeBreakTable, config=config, level=3)
	output.extend(table)
	output.append('')
	output.extend(function)
	output.append("#endif")
	output.append('')
	output.append('}')
	output.append('')

	with open(filename, 'w', encoding='utf-8', newline='\n') as fd:
		fd.write('\n'.join(output))

if __name__ == '__main__':
	updateGraphemeBreakTable('../src/GraphemeBreak.h')
