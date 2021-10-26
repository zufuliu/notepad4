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
	if maxItem < 65536:
		return 2
	return 4

def getValueSize(value):
	if value < 256:
		return 1
	if value < 65536:
		return 2
	return 4

def sizeForBitCount(bitCount):
	if bitCount <= 8:
		return 1
	if bitCount <= 16:
		return 2
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
			size, result = _compressTableEx(indexList, indexItemSize, level - 1)
			size += dataSize
			if size < minSize:
				minSize = size
				result.append((shift, indexList, blockList))
				minResult = result
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

def _preShift(indexList, shift):
	maxItem = max(indexList)
	remain = 8*getValueSize(maxItem) - maxItem.bit_length();
	if remain != 0:
		remain = min(remain, shift)
		shift -= remain
		for index in range(len(indexList)):
			indexList[index] <<= remain
	return shift

def buildMultiStageTable(head, dataTable, config=None, level=2):
	assert level >= 1
	startTime = time.perf_counter_ns()
	itemSize = getItemSize(dataTable)
	minSize, minResult = _compressTableEx(dataTable, itemSize, level)
	endTime = time.perf_counter_ns()

	size = 0
	tableList = []
	shiftList = []
	for index, result in enumerate(minResult):
		shiftList.append(result[0])
		if index == 0:
			indexList = result[1]
			itemSize = getItemSize(indexList)
			size += len(indexList)*itemSize
			tableList.append({
				'itemSize': itemSize,
				'dataList': indexList,
			})

		blockData = list(itertools.chain.from_iterable(result[2]))
		itemSize = getItemSize(blockData)
		size += len(blockData)*itemSize
		tableList.append({
			'itemSize': itemSize,
			'dataList': blockData,
		})

	dataSize = len(tableList[-1]['dataList'])*tableList[-1]['itemSize']
	assert minSize == size, (head, minSize, size)
	print(f'{head} compress {level} time: {(endTime - startTime)/1e6}, size: {minSize, dataSize} {minSize/1024}')
	if not config:
		return [], []

	shiftList = list(itertools.accumulate(reversed(shiftList)))
	shiftList.reverse()
	for index, shift in enumerate(shiftList):
		table = tableList[index]
		table['shift'] = shift
		table['mask'] = (1 << shift) - 1
		table['leftShift'] = _preShift(table['dataList'], shift)

	if True:
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
		for index, expect in enumerate(dataTable):
			offset = index
			for k in range(level):
				table = tableList[k]
				indexList = tableGroup[table['itemSize']]['dataList']
				offset = (indexList[(offset >> table['shift']) + table['offset']] << table['leftShift']) | (offset & table['mask'])
			value = dataList[offset + tableList[-1]['offset']]
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

	typeMap = {
		1: 'unsigned char',
		2: 'unsigned short',
		4: 'unsigned int',
	}

	tableName = config['tableName']
	tableVarName = config.get('tableVarName', tableName)
	itemCount = config.get('itemCount', 20)
	content = []
	for itemSize, group in tableGroup.items():
		if content:
			content.append('')
		content.append(f"const {typeMap[itemSize]} {tableVarName}{group['suffix']}[] = {{")
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
			output.extend(itertools.repeat((maxLength << valueBit) | value, count // maxLength))
			count = count % maxLength
			if count == 0:
				continue
		output.append((count << valueBit) | value)

	size = getItemSize(output)
	assert size == sizeForBitCount(totalBit)
	size *= len(output)
	print(f'{head} RLE size: {len(output)} {size} {size/1024}')

	result = []
	for item in output:
		value = item & mask
		count = item >> valueBit
		result.extend(itertools.repeat(value, count))

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
	size = length*getValueSize(rangeList[-1])
	print(f'{head} lookup: {length} {length.bit_length()}, size: {size} {size/1024}')
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
		block = tuple(table[-remain:])
		if defaultBlock == None or block != defaultBlock[:remain]:
			found = False
			for index, prev in enumerate(blockList):
				if block == prev[:remain]:
					found = True
					break
			if not found:
				dataCount += remain
				index = len(blockList)
				blockList.append(block)
			indexList.append((len(table) >> shift, index))
	return indexList, blockList, dataCount, defaultBlock

def _rangeBlockEncode(head, table):
	startTime = time.perf_counter_ns()
	itemSize = getItemSize(table)
	minSize = sys.maxsize
	minResult = None
	for shift in range(1, len(table).bit_length()):
		indexList, blockList, dataCount, defaultBlock = _compressTableSkipDefault(table, shift)
		indexItemSize = sizeForBitCount(indexList[-1][0].bit_length() + len(blockList).bit_length())
		size = dataCount*itemSize + len(indexList)*indexItemSize
		if size < minSize:
			minSize = size
			minResult = shift, indexList, blockList, defaultBlock

	endTime = time.perf_counter_ns()
	shift, indexList, blockList, defaultBlock = minResult
	defaultValue = defaultBlock[0] if defaultBlock else None
	length = len(indexList)
	print(f'{head} compress time: {(endTime - startTime)/1e6}, lookup: {length} {length.bit_length()}, size: {minSize} {minSize/1024}, default: {defaultValue}')

def _compressTableMergedEx(table, itemSize, level):
	minSize = sys.maxsize
	minResult = None
	for shift in range(1, len(table).bit_length()):
		indexList, blockList, dataCount = _compressTable(table, shift)
		offsetList, blockData = _mergeBlockList(blockList, indexList)
		dataSize = dataCount*itemSize
		merged = False

		indexItemSize = getValueSize(len(blockList) - 1)
		if level > 1:
			size, result = _compressTableMergedEx(indexList, indexItemSize, level - 1)
			dataSize += size
			blockSize = len(blockData)*itemSize
			size = dataSize - blockSize
			if size > 0:
				mergedSize, mergedResult = _compressTableMergedEx(offsetList, getItemSize(offsetList), level - 1)
				if mergedSize < size:
					dataSize = blockSize + mergedSize
					merged = True
					result = mergedResult
			if dataSize < minSize:
				minSize = dataSize
				result.append((shift, merged, indexList, blockList, offsetList, blockData))
				minResult = result
		else:
			dataSize += len(indexList)*indexItemSize
			mergedSize = len(blockData)*itemSize + len(offsetList)*getItemSize(offsetList)
			if dataSize > mergedSize:
				merged = True
				dataSize = mergedSize
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
