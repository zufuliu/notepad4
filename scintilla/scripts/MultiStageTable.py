# script to build multistage table.
# see Unicode Standard "5.1 Data Structures for Character Conversion".
import sys
import itertools
import time
import re

def getItemSize(items):
	try:
		maxItem = max(items)
	except TypeError:
		maxItem = items
	if maxItem < 256:
		return 1
	elif maxItem < 65536:
		return 2
	else:
		return 4

def alignUp(value, align):
	value, rem = divmod(value, align)
	if rem:
		value += 1
	return value * align

def bitValue(value, bitCount=16, separator='\''):
	s = bin(value)[2:]
	bit = len(s)
	if bit < bitCount:
		s = '0'*(bitCount - bit) + s
	return '0b' + separator.join(s[i:i+8] for i in range(0, bitCount, 8))

def setRange(table, low, high, value):
	while low <= high:
		table[low] = value
		low += 1

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

def getMinTableSize(table, defaultValue):
	lastIndex = 0
	for index, value in enumerate(table):
		if value != defaultValue:
			lastIndex = index
	return lastIndex + 1

def _startsWith(prev, current):
	for index, value in enumerate(current):
		if prev[index] != value:
			return False
	return True

def _compressTable(table, shift):
	indexList = []
	blockMap = {}
	tableSize = len(table)
	blockSize = 1 << shift
	if remain := tableSize & (blockSize - 1):
		tableSize -= blockSize
	for i in range(0, tableSize, blockSize):
		block = tuple(table[i:i + blockSize])
		try:
			index = blockMap[block]
		except KeyError:
			index = len(blockMap)
			blockMap[block] = index
		indexList.append(index)

	blockList = list(blockMap.keys())
	dataCount = len(blockList) << shift
	if remain:
		block = table[-remain:]
		found = False
		for index, prev in enumerate(blockList):
			if _startsWith(prev, block):
				found = True
				break
		if not found:
			dataCount += remain
			index = len(blockList)
			blockList.append(block)
		indexList.append(index)

	if False:
		indexMask = blockSize - 1
		for index, expect in enumerate(table):
			offset = indexList[index >> shift]
			value = blockList[offset][index & indexMask]
			assert expect == value, (index, expect, value)
	return indexList, blockList, dataCount

def _compressTableEx(table, secondLevel=True):
	level1ItemSize = getItemSize(table)
	level1MaxShift = len(table).bit_length()

	minSize = sys.maxsize
	minResult = {}
	for level1Shift in range(1, level1MaxShift):
		level1IndexList, level1BlockList, level1DataCount = _compressTable(table, level1Shift)
		level1Size = level1DataCount*level1ItemSize

		minLevel2Result = None
		if secondLevel:
			level2ItemSize = getItemSize(level1IndexList)
			level2MaxShift = len(level1IndexList).bit_length()
			minLevel2Size = sys.maxsize
			for level2Shift in range(1, level2MaxShift):
				level2IndexList, level2BlockList, level2DataCount = _compressTable(level1IndexList, level2Shift)
				level2Size = level2DataCount*level2ItemSize + len(level2IndexList)*getItemSize(level2IndexList)
				if level2Size < minLevel2Size:
					minLevel2Size = level2Size
					minLevel2Result = {
						'level2Shift': level2Shift,
						'level2IndexList': level2IndexList,
						'level2BlockList': level2BlockList,
					}
			level1Size += minLevel2Size
		else:
			level1Size += len(level1IndexList)*getItemSize(level1IndexList)

		if level1Size < minSize:
			minSize = level1Size
			minResult = {
				'level1Shift': level1Shift,
				'level1IndexList': level1IndexList,
				'level1BlockList': level1BlockList,
			}
			if minLevel2Result:
				minResult.update(minLevel2Result)

	return minSize, minResult

def _preShift(indexList, shift):
	maxItem = max(indexList)
	remain = 8*getItemSize(maxItem) - maxItem.bit_length();
	if remain != 0:
		remain = min(remain, shift)
		shift -= remain
		for index in range(len(indexList)):
			indexList[index] <<= remain
	return shift

def buildMultiStageTable(head, dataTable, args):
	secondLevel = args.get('secondLevel', True)
	startTime = time.perf_counter_ns()
	minSize, minResult = _compressTableEx(dataTable, secondLevel)
	endTime = time.perf_counter_ns()

	level1Shift = minResult['level1Shift']
	level1BlockList = list(itertools.chain.from_iterable(minResult['level1BlockList']))
	level1ItemSize = getItemSize(level1BlockList)
	size = len(level1BlockList)*level1ItemSize
	if secondLevel:
		level2Shift = minResult['level2Shift']
		level2IndexList = minResult['level2IndexList']
		level2BlockList = list(itertools.chain.from_iterable(minResult['level2BlockList']))
		indexItemSize = getItemSize(level2IndexList)
		level2ItemSize = getItemSize(level2BlockList)
		size += len(level2IndexList)*indexItemSize + len(level2BlockList)*level2ItemSize
	else:
		level1IndexList = minResult['level1IndexList']
		indexItemSize = getItemSize(level1IndexList)
		size += len(level1IndexList)*indexItemSize

	assert minSize == size, (head, minSize, size)
	print(f'{head} compress time: {(endTime - startTime)/1e6}, {secondLevel} size: {minSize}, {minSize/1024}')

	typeMap = {
		1: 'unsigned char',
		2: 'unsigned short',
	}
	tableName = args['tableName']
	tableVarName = args.get('tableVarName', tableName)
	definition = args['function']
	varName = re.findall(r'\w+\s+(\w+)\s*\)', definition)[0]
	returnType = args.get('returnType', None)
	table = []
	function = definition.splitlines()

	if secondLevel:
		level1Mask = (1 << level1Shift) - 1
		level1LeftShift = _preShift(level2BlockList, level1Shift)
		level2Shift += level1Shift
		level2Mask = (1 << level2Shift) - 1
		level2LeftShift = _preShift(level2IndexList, level2Shift)
		for index, expect in enumerate(dataTable):
			offset = (level2IndexList[index >> level2Shift] << level2LeftShift) | (index & level2Mask)
			offset = (level2BlockList[offset >> level1Shift] << level1LeftShift) | (offset & level1Mask)
			value = level1BlockList[offset]
			if expect != value:
				print(f'{head} verify fail:', '%04X, expect: %d, got: %d' % (index, expect, value))
				return None

		indexTableName = [tableName, tableName]
		dataOffset = [0, 0]
		if indexItemSize == level2ItemSize == level1ItemSize:
			table.append(f'const {typeMap[indexItemSize]} {tableVarName}[] = {{')
			table.append(f'// {tableName} index 1')
			table.extend(dumpArray(level2IndexList, 20))

			dataOffset[0] = len(level2IndexList)
			table.append(f'// {tableName} index 2')
			table.extend(dumpArray(level2BlockList, 20))

			dataOffset[1] = len(level2IndexList) + len(level2BlockList)
			table.append(f'// {tableName} values')
			table.extend(dumpArray(level1BlockList, 20))
			table.append('};')
		elif indexItemSize == level2ItemSize:
			indexTableName[0] = tableName + 'Index'
			table.append(f'const {typeMap[indexItemSize]} {tableVarName}Index[] = {{')
			table.append(f'// {tableName} index 1')
			table.extend(dumpArray(level2IndexList, 20))

			dataOffset[0] = len(level2IndexList)
			indexTableName[1] = tableName + 'Index'
			table.append(f'// {tableName} index 2')
			table.extend(dumpArray(level2BlockList, 20))
			table.append('};')

			table.append('')
			table.append(f'const {typeMap[level1ItemSize]} {tableVarName}[] = {{')
			table.extend(dumpArray(level1BlockList, 20))
			table.append('};')
		elif indexItemSize == level1ItemSize:
			table.append(f'const {typeMap[indexItemSize]} {tableVarName}[] = {{')
			table.append(f'// {tableName} index 1')
			table.extend(dumpArray(level2IndexList, 20))

			dataOffset[1] = len(level2IndexList)
			table.append(f'// {tableName} values')
			table.extend(dumpArray(level1BlockList, 20))
			table.append('};')

			indexTableName[1] = tableName + 'Index'
			table.append('')
			table.append(f'const {typeMap[level2ItemSize]} {tableVarName}Index[] = {{')
			table.extend(dumpArray(level2BlockList, 20))
			table.append('};')
		elif level2ItemSize == level1ItemSize:
			indexTableName[0] = tableName + 'Index'
			table.append(f'const {typeMap[indexItemSize]} {tableVarName}Index[] = {{')
			table.extend(dumpArray(level2IndexList, 20))
			table.append('};')

			table.append('')
			table.append(f'const {typeMap[level2ItemSize]} {tableVarName}[] = {{')
			table.append(f'// {tableName} index 2')
			table.extend(dumpArray(level2BlockList, 20))

			dataOffset[1] = len(level2BlockList)
			table.append(f'// {tableName} values')
			table.extend(dumpArray(level1BlockList, 20))
			table.append('};')
		else:
			indexTableName[0] = tableName + 'Index1'
			table.append(f'const {typeMap[indexItemSize]} {tableVarName}Index1[] = {{')
			table.extend(dumpArray(level2IndexList, 20))
			table.append('};')

			indexTableName[1] = tableName + 'Index2'
			table.append('')
			table.append(f'const {typeMap[level2ItemSize]} {tableVarName}Index2[] = {{')
			table.extend(dumpArray(level2BlockList, 20))
			table.append('};')

			table.append('')
			table.append(f'const {typeMap[level1ItemSize]} {tableVarName}[] = {{')
			table.extend(dumpArray(level1BlockList, 20))
			table.append('};')

		function.append(f'\t{varName} = ({indexTableName[0]}[{varName} >> {level2Shift}] << {level2LeftShift}) | ({varName} & {level2Mask});')
		function.append(f'\t{varName} = ({indexTableName[1]}[({varName} >> {level1Shift}) + {dataOffset[0]}] << {level1LeftShift}) | ({varName} & {level1Mask});')
		if returnType == None:
			function.append(f'\t{varName} = {tableName}[{varName} + {dataOffset[1]}];')
		elif returnType:
			function.append(f'\treturn static_cast<{returnType}>({tableName}[{varName} + {dataOffset[1]}]);')
			function.append('}')
		else:
			function.append(f'\treturn {tableName}[{varName} + {dataOffset[1]}];')
			function.append('}')
	else:
		level1LeftShift = _preShift(level1IndexList, level1Shift)
		level1Mask = (1 << level1Shift) - 1
		for index, expect in enumerate(dataTable):
			offset = (level1IndexList[index >> level1Shift] << level1LeftShift) | (index & level1Mask)
			value = level1BlockList[offset]
			if expect != value:
				print(f'{head} verify fail:', '%04X, expect: %d, got: %d' % (index, expect, value))
				return None

		indexTableName = tableName
		dataOffset = 0
		if indexItemSize == level1ItemSize:
			table.append(f'const {typeMap[indexItemSize]} {tableVarName}[] = {{')
			table.append(f'// {tableName} index')
			table.extend(dumpArray(level1IndexList, 20))

			dataOffset = len(level1IndexList)
			table.append(f'// {tableName} values')
			table.extend(dumpArray(level1BlockList, 20))
			table.append('};')
		else:
			indexTableName = tableName + 'Index'
			table.append(f'const {typeMap[indexItemSize]} {tableVarName}Index[] = {{')
			table.extend(dumpArray(level1IndexList, 20))
			table.append('};')

			table.append('')
			table.append(f'const {typeMap[level1ItemSize]} {tableVarName}[] = {{')
			table.extend(dumpArray(level1BlockList, 20))
			table.append('};')

		function.append(f'\t{varName} = ({indexTableName}[{varName} >> {level1Shift}] << {level1LeftShift}) | ({varName} & {level1Mask});')
		if returnType == None:
			function.append(f'\t{varName} = {tableName}[{varName} + {dataOffset}];')
		elif returnType:
			function.append(f'\treturn static_cast<{returnType}>({tableName}[{varName} + {dataOffset}]);')
			function.append('}')
		else:
			function.append(f'\treturn {tableName}[{varName} + {dataOffset}];')
			function.append('}')

	return table, function

def runLengthEncode(head, table, valueBit, totalBit=16):
	mask = (1 << valueBit) - 1
	assert max(table) <= mask
	maxLength = (1 << (totalBit - valueBit)) - 1

	values = []
	prevValue = table[0]
	prevIndex = 0;
	for index, value in enumerate(table):
		if value != prevValue:
			values.append((prevValue, index - prevIndex))
			prevValue = value
			prevIndex = index
	values.append((prevValue, len(table) - prevIndex))

	output = []
	for value, count in values:
		if count > maxLength:
			output.extend([(maxLength << valueBit) | value] * (count // maxLength))
			count = count % maxLength
			if count == 0:
				continue
		output.append((count << valueBit) | value)

	size = getItemSize(output)*len(output)
	print(f'{head} RLE size: {len(output)} {size} {size/1024}')

	result = []
	for item in output:
		value = item & mask
		count = item >> valueBit
		result.extend([value] * count)

	assert result == table
	return output

def rangeEncode(head, table, valueBit, sentinel=None):
	rangeList = []
	start = 0
	prev = table[0]
	for index, value in enumerate(table):
		if value != prev:
			rangeList.append((start << valueBit) | prev)
			start = index
			prev = value
	rangeList.append((start << valueBit) | prev)
	if sentinel:
		assert start < (sentinel >> valueBit)
		rangeList.append(sentinel)

	length = len(rangeList)
	size = length*getItemSize(rangeList[-1])
	print(f'{head} length: {length}, lookup: {length.bit_length()}, size: {size} {size/1024}')
	return rangeList

def _compressTableSkipDefault(table, shift):
	indexList = []
	blockMap = {}
	blockFreq = {}
	tableSize = len(table)
	blockSize = 1 << shift
	if remain := tableSize & (blockSize - 1):
		tableSize -= blockSize
	for i in range(0, tableSize, blockSize):
		block = tuple(table[i:i + blockSize])
		try:
			index = blockMap[block]
			blockFreq[index] += 1
		except KeyError:
			index = len(blockMap)
			blockMap[block] = index
			blockFreq[index] = 1
		indexList.append((i >> shift, index))

	blockList = list(blockMap.keys())
	items = sorted(((freq, index) for index, freq in blockFreq.items()), reverse=True)
	defaultBlock = None
	for item in items:
		blockIndex = item[1]
		block = blockList[blockIndex]
		if block.count(block[0]) == blockSize:
			indexList = [(offset, index - (index > blockIndex)) for offset, index in indexList if index != blockIndex]
			defaultBlock = block
			del blockList[blockIndex]
			break

	dataCount = len(blockList) << shift
	if remain:
		block = table[-remain:]
		if not defaultBlock or not _startsWith(defaultBlock, block):
			found = False
			for index, prev in enumerate(blockList):
				if _startsWith(prev, block):
					found = True
					break
			if not found:
				dataCount += remain
				index = len(blockList)
				blockList.append(block)
			indexList.append((len(table) >> shift, index))

	return indexList, blockList, dataCount, defaultBlock

def rangeBlockEncode(head, table):
	startTime = time.perf_counter_ns()
	itemSize = getItemSize(table)
	maxShift = len(table).bit_length()
	minSize = sys.maxsize
	minResult = None
	for shift in range(1, maxShift):
		indexList, blockList, dataCount, defaultBlock = _compressTableSkipDefault(table, shift)
		indexItemSize = alignUp(indexList[-1][0].bit_length() + len(blockList).bit_length(), 8) >> 3
		size = dataCount*itemSize + len(indexList)*indexItemSize
		if size < minSize:
			minSize = size
			minResult = shift, indexList, blockList, defaultBlock

	endTime = time.perf_counter_ns()
	defaultBlock = minResult[-1]
	defaultValue = defaultBlock[0] if defaultBlock else None
	print(f'{head} compress time: {(endTime - startTime)/1e6}, size: {minSize}, {minSize/1024}, default: {defaultValue}')
