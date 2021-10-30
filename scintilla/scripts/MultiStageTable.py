# script to build multistage table.
# see Unicode Standard "5.1 Data Structures for Character Conversion".
import sys
import itertools
from collections import Counter
import time
import re

_sizeTypeMap = {
	1: 'uint8_t',
	2: 'uint16_t',
	4: 'uint32_t',
	8: 'uint64_t',
}

def getItemSize(items):
	try:
		maxItem = max(items)
	except TypeError:
		maxItem = items
	if maxItem < 256:
		return 1
	if maxItem < 65536:
		return 2
	if maxItem < 0x10000_0000:
		return 4
	return 8

def getValueSize(value):
	if value < 256:
		return 1
	if value < 65536:
		return 2
	if value < 0x10000_0000:
		return 4
	return 8

def sizeForBitCount(bitCount):
	if bitCount <= 8:
		return 1
	if bitCount <= 16:
		return 2
	if bitCount <= 32:
		return 4
	return 8

def _roundValueBit(value):
	valueBit = value.bit_length()
	if valueBit & (valueBit - 1):
		valueBit = 1 << valueBit.bit_length()
	return valueBit

def alignUp(value, align):
	remain = value % align
	if remain:
		value += align - remain
	return value

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
	if step == 1:
		fmt += ','
		lines.extend(fmt % value for value in items)
	elif step:
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
		block = tuple(table[-remain:])
		found = False
		for index, prev in enumerate(blockList):
			if block == prev[:remain]:
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

def _compressTableEx(table, itemSize, level):
	minSize = sys.maxsize
	minResult = None
	for shift in range(1, len(table).bit_length()):
		indexList, blockList, dataCount = _compressTable(table, shift)
		dataSize = dataCount*itemSize

		indexItemSize = getValueSize(len(blockList) - 1)
		if level > 1:
			result = _compressTableEx(indexList, indexItemSize, level - 1)
			dataSize += result[0]
			if dataSize < minSize:
				minSize = dataSize
				minResult = result[1]
				minResult.append((shift, indexList, blockList))
		else:
			dataSize += len(indexList)*indexItemSize
			if dataSize < minSize:
				minSize = dataSize
				minResult = shift, indexList, blockList

	if level == 1:
		minResult = [minResult]
	return minSize, minResult

def _getMergedLength(prev, current):
	if prev[-1] == current[0]:
		length = len(current)
		for merged in range(2, length + 1):
			if prev[-merged:] != current[:merged]:
				return merged - 1
		return length
	return 0

def _mergeBlockList(blockList, indexList):
	blockData = []
	blockOffset = []
	prev = None
	for index, block in enumerate(blockList):
		offset = len(blockData)
		if prev == None:
			blockData.extend(block)
		else:
			if merged := _getMergedLength(prev, block):
				offset -= merged
				blockData.extend(block[merged:])
			else:
				blockData.extend(block)
		blockOffset.append(offset)
		prev = block

	offsetList = [blockOffset[value] for value in indexList]
	return offsetList, blockData

def _preShift(indexList, maxItem, shift):
	remain = 8*getValueSize(maxItem) - maxItem.bit_length();
	if remain != 0:
		remain = min(remain, shift)
		shift -= remain
		for index in range(len(indexList)):
			indexList[index] <<= remain
	return shift

def buildMultiStageTable(head, dataTable, config=None, level=2, mergeValue=False):
	assert level >= 1
	merged = None
	startTime = time.perf_counter_ns()
	maxItem = max(dataTable)
	itemSize = getValueSize(maxItem)
	minSize, minResult = _compressTableEx(dataTable, itemSize, level)
	if mergeValue:
		valueBit = _roundValueBit(maxItem)
		prevTable = dataTable
		for bitCount in (2, 4, 8, 16, 32, 64):
			if bitCount > valueBit:
				shift = bitCount >> 1
				length = len(prevTable) - 1
				table = [prevTable[i] | (prevTable[i + 1] << shift) for i in range(0, length, 2)]
				if (length & 1) == 0:
					table.append(prevTable[length])
				size, result = _compressTableEx(table, sizeForBitCount(bitCount), level)
				if size < minSize:
					minSize = size
					minResult = result
					merged = bitCount, valueBit
				prevTable = table
	endTime = time.perf_counter_ns()

	size = 0
	tableList = []
	shiftList = []
	for index, result in enumerate(minResult):
		shiftList.append(result[0])
		if index == 0:
			indexList = result[1]
			maxItem = max(indexList)
			itemSize = getValueSize(maxItem)
			size += len(indexList)*itemSize
			tableList.append({
				'itemSize': itemSize,
				'maxItem': maxItem,
				'dataList': indexList,
			})

		blockData = list(itertools.chain.from_iterable(result[2]))
		maxItem = max(blockData)
		itemSize = getValueSize(maxItem)
		size += len(blockData)*itemSize
		tableList.append({
			'itemSize': itemSize,
			'maxItem': maxItem,
			'dataList': blockData,
		})

	dataSize = len(tableList[-1]['dataList'])*tableList[-1]['itemSize']
	assert minSize == size, (head, minSize, size)
	print(f'{head} compress {level} {merged} time: {(endTime - startTime)/1e6}, size: {minSize, dataSize} {minSize/1024}')
	if not config:
		return None

	shiftList = list(itertools.accumulate(reversed(shiftList)))
	shiftList.reverse()
	for index, shift in enumerate(shiftList):
		table = tableList[index]
		table['shift'] = shift
		table['mask'] = (1 << shift) - 1
		table['leftShift'] = _preShift(table['dataList'], table['maxItem'], shift)

	if merged:
		startTime = time.perf_counter_ns()
		dataList = tableList[-1]['dataList']
		bitCount, valueBit = merged
		indexMask = (bitCount // valueBit) - 1
		indexBit = indexMask.bit_length()
		valueMask = (1 << valueBit) - 1
		valueBit = (valueBit - 1).bit_length()
		for index, expect in enumerate(dataTable):
			valueShift = (index & indexMask) << valueBit
			offset = index >> indexBit
			for k in range(level):
				table = tableList[k]
				offset = (table['dataList'][offset >> table['shift']] << table['leftShift']) | (offset & table['mask'])
			value = (dataList[offset] >> valueShift) & valueMask
			if expect != value:
				print(f'{head} first verify {level} {merged} fail: {index:04X}, expect: {expect}, got: {value}')
				return None
		endTime = time.perf_counter_ns()
		print(f'{head} first verify {level} {merged} time: {(endTime - startTime)/1e6}')
	else:
		startTime = time.perf_counter_ns()
		dataList = tableList[-1]['dataList']
		for index, expect in enumerate(dataTable):
			offset = index
			for k in range(level):
				table = tableList[k]
				offset = (table['dataList'][offset >> table['shift']] << table['leftShift']) | (offset & table['mask'])
			value = dataList[offset]
			if expect != value:
				print(f'{head} first verify {level} fail: {index:04X}, expect: {expect}, got: {value}')
				return None
		endTime = time.perf_counter_ns()
		print(f'{head} first verify {level} time: {(endTime - startTime)/1e6}')

	# merge table by item size
	tableGroup = {
		1: {},
		2: {},
		4: {},
		8: {},
	}
	for index, table in enumerate(tableList):
		offset = 0
		group = tableGroup[table['itemSize']]
		if group:
			offset = len(group['dataList'])
		else:
			group['suffix'] = ''
			group['tableList'] = []
			group['dataList'] = []
		table['offset'] = offset
		group['tableList'].append(index)
		group['dataList'].extend(table['dataList'])

	if False:
		startTime = time.perf_counter_ns()
		dataList = tableGroup[tableList[-1]['itemSize']]['dataList']
		dataOffset = tableList[-1]['offset']
		for index, expect in enumerate(dataTable):
			offset = index
			for k in range(level):
				table = tableList[k]
				indexList = tableGroup[table['itemSize']]['dataList']
				offset = (indexList[(offset >> table['shift']) + table['offset']] << table['leftShift']) | (offset & table['mask'])
			value = dataList[offset + dataOffset]
			if expect != value:
				print(f'{head} second verify {level} fail: {index:04X}, expect: {expect}, got: {value}')
				return None
		endTime = time.perf_counter_ns()
		print(f'{head} second verify {level} time: {(endTime - startTime)/1e6}')

	tableGroup = dict(item for item in tableGroup.items() if item[1])
	count = len(tableGroup)
	if count > 1:
		# add suffix for index table
		index = 1
		dataSize = tableList[-1]['itemSize']
		for itemSize, group in tableGroup.items():
			if itemSize != dataSize:
				if count == 2:
					group['suffix'] = 'Index'
				else:
					group['suffix'] = f'Index{index}'
					index += 1
	if count != len(tableList):
		# add comment for merged table content
		count = len(tableList) - 1
		for index, table in enumerate(tableList):
			group = tableGroup[table['itemSize']]
			if index < count:
				if count == 1:
					table['comment'] = 'index'
				else:
					table['comment'] = f'index {index + 1}'
			elif len(group['tableList']) != 1:
				table['comment'] = 'values'

	tableName = config['tableName']
	tableVarName = config.get('tableVarName', tableName)
	itemCount = config.get('itemCount', 20)
	content = []
	for itemSize, group in tableGroup.items():
		if content:
			content.append('')
		content.append(f"const {_sizeTypeMap[itemSize]} {tableVarName}{group['suffix']}[] = {{")
		for index in group['tableList']:
			table = tableList[index]
			if comment := table.get('comment', ''):
				content.append(f'// {tableName} {comment}')
			content.extend(dumpArray(table['dataList'], itemCount))
		content.append('};')

	definition = config['function']
	varName = re.findall(r'\w+\s+(\w+)\s*\)', definition)[0]
	indent = config.get('indent', '\t')
	function = definition.splitlines()
	valueMask = 0
	if merged:
		bitCount, valueBit = merged
		indexMask = (bitCount // valueBit) - 1
		valueMask = (1 << valueBit) - 1
		if valueBit == 1:
			function.append(f'{indent}const int shift = {varName} & {indexMask};')
		else:
			function.append(f'{indent}const int shift = ({varName} & {indexMask}) << {(valueBit - 1).bit_length()};')
		function.append(f'{indent}{varName} >>= {indexMask.bit_length()};')
	for index in range(len(tableList) - 1):
		table = tableList[index]
		suffix = tableGroup[table['itemSize']]['suffix']
		# index = (table[(index >> shift) + offset] << leftShift) | (index & mask)
		stmt = f"{varName} >> {table['shift']}"
		if offset := table['offset']:
			stmt = f'({stmt}) + {offset}'
		stmt = f'{tableName}{suffix}[{stmt}]'
		if leftShift := table['leftShift']:
			stmt = f'({stmt} << {leftShift})'
		stmt = f"{indent}{varName} = {stmt} | ({varName} & {table['mask']});"
		function.append(stmt)
	# return static_cast<returnType>(table[index + offset])
	offset = tableList[-1]['offset']
	stmt = f'{varName} + {offset}' if offset else varName
	stmt = f'{tableName}[{stmt}]'
	if valueMask:
		stmt = f'({stmt} >> shift) & {valueMask}'
	returnType = config.get('returnType', None)
	if returnType == None:
		stmt = f'{indent}{varName} = {stmt};'
	elif returnType:
		stmt = f'{indent}return static_cast<{returnType}>({stmt});'
	else:
		stmt = f'{indent}return {stmt};'
	function.append(stmt)
	if returnType != None:
		function.append('}')

	return content, function

def _divup(m, n):
	quo = m // n
	if m % n:
		quo += 1
	return quo

def _runLengthEncode(table, valueBit=None, totalBit=None):
	maxItem = max(table)
	if not valueBit:
		valueBit = maxItem.bit_length()
	else:
		assert maxItem < (1 << valueBit)

	items = []
	start = 0;
	prev = table[0]
	for index, value in enumerate(table):
		if value != prev:
			items.append((index - start, prev))
			prev = value
			start = index
	items.append((len(table) - start, prev))

	if not totalBit or totalBit <= valueBit:
		minSize = sys.maxsize
		for bitCount in (8, 16, 32):
			if bitCount > valueBit:
				maxLength = (1 << (bitCount - valueBit)) - 1
				size = sum(_divup(item[0], maxLength) for item in items)
				size *= sizeForBitCount(bitCount)
				if size < minSize:
					minSize = size
					totalBit = bitCount

	values = []
	maxLength = (1 << (totalBit - valueBit)) - 1
	for count, value in items:
		if count > maxLength:
			values.extend(itertools.repeat((maxLength << valueBit) | value, count // maxLength))
			count = count % maxLength
			if count == 0:
				continue
		values.append((count << valueBit) | value)

	return valueBit, totalBit, values

def runLengthEncode(head, table, valueBit=None, totalBit=None):
	valueBit, totalBit, values = _runLengthEncode(table, valueBit=valueBit, totalBit=totalBit)
	itemSize = sizeForBitCount(totalBit)
	size = len(values)*itemSize
	print(f'{head} RLE value bit: {totalBit} {valueBit}, length: {len(values)}, size: {size} {size/1024}')

	result = []
	mask = (1 << valueBit) - 1
	for item in values:
		value = item & mask
		count = item >> valueBit
		result.extend(itertools.repeat(value, count))

	assert result == table
	return valueBit, totalBit, values

def rangeEncode(head, table, valueBit=None, sentinel=None):
	if not valueBit:
		valueBit = max(table).bit_length()
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
	itemSize = getValueSize(rangeList[-1])
	size = length*itemSize
	print(f'{head} value bit: {itemSize*8} {valueBit}, lookup: {length} {length.bit_length()}, size: {size} {size/1024}')
	return valueBit, rangeList

def _dumpRunBlock(output, tableName, indexList, blockData, valueBit, shift):
	itemSize = getItemSize(blockData)
	indexItemSize = getItemSize(indexList)
	output.append(f'constexpr int {tableName}IndexBit = {valueBit};')
	output.append(f'constexpr int {tableName}BlockBit = {shift};')
	if itemSize == indexItemSize:
		output.append(f'constexpr int {tableName}Offset = {len(blockData)};')
		output.append('')
		output.append(f'const {_sizeTypeMap[itemSize]} {tableName}Data[] = {{')
		output.append(f'// {tableName} values')
		output.extend(dumpArray(blockData, 20))
		output.append(f'// {tableName} index')
		output.extend(dumpArray(indexList, 20))
		output.append('};')
	else:
		output.append('')
		output.append(f'const {_sizeTypeMap[indexItemSize]} {tableName}Index[] = {{')
		output.extend(dumpArray(indexList, 20))
		output.append('};')
		output.append('')
		output.append(f'const {_sizeTypeMap[itemSize]} {tableName}Data[] = {{')
		output.extend(dumpArray(blockData, 20))
		output.append('};')
	return output

def runBlockEncode(head, table, tableName=''):
	itemSize = getItemSize(table)
	minSize = sys.maxsize
	minResult = None
	for shift in range(2, len(table).bit_length()):
		indexList, blockList, dataCount = _compressTable(table, shift)
		valueBit = (len(blockList) - 1).bit_length()
		valueBit, totalBit, values = _runLengthEncode(indexList, valueBit=valueBit)
		size = dataCount*itemSize + len(values)*sizeForBitCount(totalBit)
		if size < minSize:
			minSize = size
			minResult = shift, valueBit, totalBit, values, blockList

	shift, valueBit, totalBit, values, blockList = minResult
	blockSize = 1 << shift
	print(f'{head} run block value bit: {totalBit} {valueBit}, length: {len(values)}, size: {minSize} {minSize/1024}, block: {len(blockList)} {blockSize}')

	if tableName and (len(table) & (blockSize - 1)) == 0:
		output = []
		mask = (1 << valueBit) - 1
		for value in values:
			index = value & mask
			count = value >> valueBit
			block = blockList[index]
			for i in range(count):
				output.extend(block)
		assert output == table

		output = []
		blockData = list(itertools.chain.from_iterable(blockList))
		_dumpRunBlock(output, tableName, values, blockData, valueBit, shift)
		return output

def skipBlockEncode(head, table, tableName=''):
	itemSize = getItemSize(table)
	minSize = sys.maxsize
	minResult = None
	for shift in range(2, len(table).bit_length()):
		defaultBlock = None
		indexList, blockList, dataCount = _compressTable(table, shift)
		if len(blockList) > 1:
			counter = Counter(indexList)
			blockIndex = counter.most_common(1)[0][0]
			block = blockList[blockIndex]
			blockSize = 1 << shift
			if block.count(block[0]) == blockSize:
				dataCount -= blockSize
				indexList = [(offset, index - (index > blockIndex)) for offset, index in enumerate(indexList) if index != blockIndex]
				defaultBlock = block
				del blockList[blockIndex]

		if defaultBlock:
			indexItemSize = sizeForBitCount(indexList[-1][0].bit_length() + (len(blockList) - 1).bit_length())
		else:
			indexItemSize = getValueSize(len(blockList) - 1)
		size = dataCount*itemSize + len(indexList)*indexItemSize
		if size < minSize:
			minSize = size
			minResult = shift, indexList, blockList, defaultBlock

	shift, indexList, blockList, defaultBlock = minResult
	defaultValue = defaultBlock[0] if defaultBlock else None
	length = len(indexList)
	blockSize = 1 << shift
	print(f'{head} skip block lookup: {length} {length.bit_length()}, size: {minSize} {minSize/1024}, block: {len(blockList)} {blockSize} default: {defaultValue}')

	if tableName and defaultBlock and (len(table) & (blockSize - 1)) == 0:
		bitCount = (len(blockList) - 1).bit_length()
		offsetList = []
		output = [defaultValue]*len(table)
		for offset, index in indexList:
			block = blockList[index]
			offsetList.append((offset << bitCount) | index)
			offset <<= shift
			for k, value in enumerate(block):
				output[offset + k] = value

		blockData = list(itertools.chain.from_iterable(blockList))
		size = len(blockData)*itemSize + len(offsetList)*getItemSize(offsetList)
		assert minSize == size, (minSize, size)
		assert output == table

		output = []
		if defaultValue:
			output.append(f'constexpr {_sizeTypeMap[itemSize]} {tableName}DefaultValue = {defaultValue};')
		_dumpRunBlock(output, tableName, offsetList, blockData, bitCount, shift)
		return output

def _compressTableMergedEx(table, itemSize, level):
	minSize = sys.maxsize
	minResult = None
	for shift in range(1, len(table).bit_length()):
		indexList, blockList, dataCount = _compressTable(table, shift)
		offsetList, blockData = _mergeBlockList(blockList, indexList)
		dataSize = dataCount*itemSize
		merged = False

		indexItemSize = getValueSize(len(blockList) - 1)
		blockSize = len(blockData)*itemSize
		if level > 1:
			result = _compressTableMergedEx(indexList, indexItemSize, level - 1)
			dataSize += result[0]
			if dataSize > blockSize:
				mergedResult = _compressTableMergedEx(offsetList, getItemSize(offsetList), level - 1)
				blockSize += mergedResult[0]
				if dataSize > blockSize:
					merged = True
					dataSize = blockSize
					result = mergedResult
			if dataSize < minSize:
				minSize = dataSize
				minResult = result[1]
				minResult.append((shift, merged, indexList, blockList, offsetList, blockData))
		else:
			dataSize += len(indexList)*indexItemSize
			blockSize += len(offsetList)*getItemSize(offsetList)
			if dataSize > blockSize:
				merged = True
				dataSize = blockSize
			if dataSize < minSize:
				minSize = dataSize
				minResult = shift, merged, indexList, blockList, offsetList, blockData

	if level == 1:
		minResult = [minResult]
	return minSize, minResult

def _compressTableMerged(head, dataTable, level=1):
	assert level >= 1
	startTime = time.perf_counter_ns()
	itemSize = getItemSize(dataTable)
	minSize, minResult = _compressTableMergedEx(dataTable, itemSize, level)
	endTime = time.perf_counter_ns()

	merged = [item[1] for item in minResult]
	merged.reverse()
	print(f'{head} compress {level} time: {(endTime - startTime)/1e6}, size: {minSize} {minSize/1024}', merged)
