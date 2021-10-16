#!/usr/bin/env python3
import sys
from enum import IntEnum

from GenerateCharacterCategory import *

class GraphemeBreak(IntEnum):
	Other = 0

	CR = 1
	LF = 2
	Control = 3
	Extend = 4
	RegionalIndicator = 5
	Prepend = 6
	SpacingMark = 7
	HangulChoseong_L = 8
	HangulJungseong_V = 9
	HangulJongseong_T = 10
	HangulSyllable_LV = 11
	HangulSyllable_LVT = 12
	ExtendedPictographic = 13
	ZeroWidthJoiner = 14

	ValueBit = 4

GraphemeBreakMap = {
	'Other': GraphemeBreak.Other,
	'CR': GraphemeBreak.CR,
	'LF': GraphemeBreak.LF,
	'Control': GraphemeBreak.Control,
	'Extend': GraphemeBreak.Extend,
	'Regional_Indicator': GraphemeBreak.RegionalIndicator,
	'Prepend': GraphemeBreak.Prepend,
	'SpacingMark': GraphemeBreak.SpacingMark,
	'L': GraphemeBreak.HangulChoseong_L,
	'V': GraphemeBreak.HangulJungseong_V,
	'T': GraphemeBreak.HangulJongseong_T,
	'LV': GraphemeBreak.HangulSyllable_LV,
	'LVT': GraphemeBreak.HangulSyllable_LVT,
	'Extended_Pictographic': GraphemeBreak.ExtendedPictographic,
	'ZWJ': GraphemeBreak.ZeroWidthJoiner,
}

def parsePropertyLine(line):
	line = line.split('#', 2)[0] # strip comment
	code, prop = line.split(';', 2)
	code = code.split('.')
	start = int(code[0], 16)
	if len(code) == 1:
		end = start
	else:
		end = int(code[-1], 16)
	return start, end, prop.strip()

def readGraphemeBreakProperty(table, maxIndex, path, include=None):
	with open(path, encoding='utf-8') as fd:
		for line in fd.readlines():
			line = line.strip()
			if not line or line[0] == '#':
				continue
			start, end, prop = parsePropertyLine(line)
			if include and prop not in include:
				continue
			value = int(GraphemeBreakMap[prop])
			if value == 0:
				continue
			while start <= end:
				table[start] = value
				start += 1
			maxIndex = max(end, maxIndex)
	return maxIndex

def alignUp(value, align):
	value, rem = divmod(value, align)
	if rem:
		value += 1
	return value * align

def compressValueTable(valueTable):
	tableSize = len(valueTable)
	minTotalSize = 0
	minBlockSize = 0
	minBlockList = 0
	minIndexList = 0
	for blockSize in (1024, 512, 256, 128, 64, 32, 16, 8, 4, 2):
		blockList = []
		blockMap = {}
		indexList = [(0, 0)]
		for i in range(0, tableSize, blockSize):
			block = tuple(valueTable[i:i + blockSize])
			if sum(block) == 0:
				continue
			if block in blockMap:
				index = blockMap[block]
			else:
				index = len(blockList) + 1
				blockList.append(block)
				blockMap[block] = index
			blockId = i // blockSize
			indexList.append((blockId, index))

		totalSize = len(blockList)*blockSize
		totalSize += len(indexList)*2
		if minTotalSize == 0 or totalSize < minTotalSize:
			minTotalSize = totalSize
			minBlockSize = blockSize
			minBlockList = blockList
			minIndexList = indexList

	print('block size:', (minBlockSize, minTotalSize/1024), 'block:', len(minBlockList), 'index:', len(minIndexList))

def updateGraphemeBreakTable():
	tableSize = 0
	indexTable = [0] * UnicodeCharacterCount
	tableSize = readGraphemeBreakProperty(indexTable, tableSize, 'GraphemeBreakProperty.txt')
	tableSize = readGraphemeBreakProperty(indexTable, tableSize, 'emoji-data.txt', ['Extended_Pictographic'])
	maxNonOtherChar = tableSize
	print('Grapheme Break table size', tableSize + 1, indexTable[tableSize])
	tableSize = alignUp(tableSize, 128)

	if GraphemeBreak.ValueBit == 4:
		index = alignUp(tableSize // 2, 16)
		valueTable = [0] * index
		index = 0
		for j in range(0, tableSize, 2):
			valueTable[index] = indexTable[j] | (indexTable[j + 1] << 4)
			index += 1
	if GraphemeBreak.ValueBit == 2:
		index = alignUp(tableSize // 4, 16)
		valueTable = [0] * index
		index = 0
		for j in range(0, tableSize, 4):
			valueTable[index] = indexTable[j] | (indexTable[j + 1] << 2) | (indexTable[j + 2] << 4) | (indexTable[j + 3] << 6)
			index += 1
	elif GraphemeBreak.ValueBit == 1:
		index = alignUp(tableSize // 8, 16)
		valueTable = [0] * index
		index = 0
		for j in range(0, tableSize, 8):
			valueTable[index] = indexTable[j] | (indexTable[j + 1] << 1) | (indexTable[j + 2] << 2) | (indexTable[j + 3] << 3) \
							| (indexTable[j + 4] << 5) | (indexTable[j + 5] << 5) | (indexTable[j + 6] << 6) | (indexTable[j + 7] << 7)
			index += 1

	print('Grapheme Break value size', index, max(valueTable))
	if True:
		args = {
			'table_var': 'CharClassify::GraphemeBreakTable',
			'table': 'GraphemeBreakTable',
			'function': """static GraphemeBreak GetGraphemeBreak(unsigned int ch) noexcept {
	if (ch > maxNonOtherChar) {
		return GraphemeBreak::Other;
	}
	ch >>= 1;
""",
			'returnType': 'GraphemeBreak'
		}

		table, function = compressIndexTable('Unicode Grapheme Break', valueTable, args)

	compressValueTable(valueTable)

updateGraphemeBreakTable()
