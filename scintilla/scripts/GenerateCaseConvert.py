# Script to generate CaseConvert.cxx from Python's Unicode data
# Should be run rarely when a Python with a new version of Unicode data is available.

# Current best approach divides case conversions into two cases:
# simple symmetric and complex.
# Simple symmetric is where a lower and upper case pair convert to each
# other and the folded form is the same as the lower case.
# There are 1006 symmetric pairs.
# These are further divided into ranges (stored as lower, upper, range length,
# range pitch and singletons (stored as lower, upper).
# Complex is for cases that don't fit the above: where there are multiple
# characters in one of the forms or fold is different to lower or
# lower(upper(x)) or upper(lower(x)) are not x. These are represented as UTF-8
# strings with original, folded, upper, and lower separated by '|'.
# There are 126 complex cases.

import platform
import unicodedata
import io
import itertools
import string
from pprint import pprint

from FileGenerator import Regenerate
from MultiStageTable import *
from UnicodeData import *

def isCaseSensitive(ch):
	return ch != ch.upper() or ch != ch.lower() or ch != ch.casefold()

def contiguousRanges(ll, diff):
	# ll is s list of lists
	# group into lists where first element of each element differs by diff
	out = [[ll[0]]]
	for s in ll[1:]:
		if s[0] != out[-1][-1][0] + diff:
			out.append([])
		out[-1].append(s)
	return out

def flatten(listOfLists):
	"Flatten one level of nesting"
	return itertools.chain.from_iterable(listOfLists)

def conversionSets():
	# For all Unicode characters, see whether they have case conversions
	# Return 2 sets: one of simple symmetric conversion cases and another
	# with complex cases.
	complexes = []
	symmetrics = []
	for ch in range(UnicodeCharacterCount):
		if ch >= 0xd800 and ch <= 0xDBFF:
			continue
		if ch >= 0xdc00 and ch <= 0xDFFF:
			continue
		uch = chr(ch)

		fold = uch.casefold()
		upper = uch.upper()
		lower = uch.lower()
		symmetric = False
		if uch != upper and len(upper) == 1 and uch == lower and uch == fold:
			lowerUpper = upper.lower()
			foldUpper = upper.casefold()
			if lowerUpper == foldUpper and lowerUpper == uch:
				symmetric = True
				symmetrics.append((ch, ord(upper), ch - ord(upper)))
		if uch != lower and len(lower) == 1 and uch == upper and lower == fold:
			upperLower = lower.upper()
			if upperLower == uch:
				symmetric = True

		if fold == uch:
			fold = ""
		if upper == uch:
			upper = ""
		if lower == uch:
			lower = ""

		if (fold or upper or lower) and not symmetric:
			complexes.append((uch, fold, upper, lower))

	return symmetrics, complexes

def groupRanges(symmetrics):
	# Group the symmetrics into groups where possible, returning a list
	# of ranges and a list of symmetrics that didn't fit into a range

	def distance(s):
		return s[2]

	groups = []
	uniquekeys = []
	for k, g in itertools.groupby(symmetrics, distance):
		groups.append(list(g))      # Store group iterator as a list
		uniquekeys.append(k)

	contiguousGroups = flatten([contiguousRanges(g, 1) for g in groups])
	longGroups = [(x[0][0], x[0][1], len(x), 1) for x in contiguousGroups if len(x) > 4]

	oneDiffs = [s for s in symmetrics if s[2] == 1]
	contiguousOnes = flatten([contiguousRanges(g, 2) for g in [oneDiffs]])
	longOneGroups = [(x[0][0], x[0][1], len(x), 2) for x in contiguousOnes if len(x) > 4]

	rangeGroups = sorted(longGroups+longOneGroups, key=lambda s: s[0])

	rangeCoverage = list(flatten([range(r[0], r[0]+r[2]*r[3], r[3]) for r in rangeGroups]))

	nonRanges = [(x, u) for x, u, _d in symmetrics if x not in rangeCoverage]

	return rangeGroups, nonRanges

def escape(s):
	return "".join((chr(c) if chr(c) in string.ascii_letters else f"\\x{c:02x}") for c in s.encode('utf-8'))

def updateCaseConvert():
	symmetrics, complexes = conversionSets()

	rangeGroups, nonRanges = groupRanges(symmetrics)

	print(len(rangeGroups), "ranges")
	rangeLines = ["%d,%d,%d,%d," % x for x in rangeGroups]

	print(len(nonRanges), "non ranges")
	nonRangeLines = ["%d,%d," % x for x in nonRanges]

	print(len(symmetrics), "symmetric")

	complexLines = ['"%s|%s|%s|%s|"' % tuple(escape(t) for t in x) for x in complexes]
	print(len(complexLines), "complex")

	Regenerate("../src/CaseConvert.cxx", "//", rangeLines, nonRangeLines, complexLines)

def getUnicodeCaseSensitivityGroup(caseList):
	ranges = []
	count = len(caseList)
	start = 0
	while start < count:
		index = start
		while index + 1 < count and caseList[index][0] + 1 == caseList[index + 1][0]:
			index += 1
		begin = caseList[start]
		end = caseList[index]
		begin = begin[0]
		end = end[0]
		ranges.append((begin, end, end - begin + 1, hex(begin), hex(end)))
		start = index + 1

	groups = []
	count = len(ranges)
	start = 0
	totalSize = 0
	headerSize = 3*4
	while start < count:
		index = start
		while index + 1 < count and ranges[index][1] + 256 >= ranges[index + 1][0]:
			index += 1
		items = ranges[start:index + 1]
		size = headerSize
		begin = items[0][0]
		end = items[-1][1]
		if len(items) == 1:
			end += 1
		else:
			if begin < 256:
				begin = 0
				size -= headerSize
			word = (end - begin + 32) // 32
			end = begin + 32*word
			size += 4*word
		totalSize += size
		groups.append({
			'min': begin,
			'min_hex': hex(begin),
			'max': end,
			'max_hex': hex(end),
			'size': (size, headerSize*len(items)),
			'count': sum(item[2] for item in items),
			'ranges': items
		})
		start = index + 1

	print('Unicode Case Sensitivity ranges:', len(groups), 'size:', totalSize)
	return groups

def addCaseSensitivityTest(fd, caseTable, charCount):
	output = ['', '#if 1']
	output.append('static const uint8_t UnicodeCaseSensitivityTable[] = {')
	for i in range(0, charCount, 64):
		line = ','.join(caseTable[i:i+64])
		output.append(line + ',')
	output.append('};')
	output.append('')
	fd.write('\n'.join(output))

	fd.write(r"""
#include <cassert>
#include <cstdio>
#include <chrono>
#include "../src/ElapsedPeriod.h"

int main() {
	assert(sizeof(UnicodeCaseSensitivityTable) > kUnicodeCaseSensitiveMax);
	const Scintilla::Internal::ElapsedPeriod period;
	for (uint32_t ch = 0; ch < sizeof(UnicodeCaseSensitivityTable); ch++) {
		const int result = IsCharacterCaseSensitive(ch);
		const int expect = UnicodeCaseSensitivityTable[ch];
		if (result != expect) {
			printf("case diff: %u %04x %d %d\n", ch, ch, result, expect);
			break;
		}
	}

	const double duration = period.Duration()*1e3;
	const double avg = duration/sizeof(UnicodeCaseSensitivityTable);
	printf("count=%zu, duration=%.6f, avg=%.6f\n", sizeof(UnicodeCaseSensitivityTable), duration, avg);
	return 0;
}
#endif
""")

def checkUnicodeCaseSensitivity(filename=None):
	caseList = []
	caseTable = ['0']*UnicodeCharacterCount

	for ch in range(UnicodeCharacterCount):
		uch = chr(ch)
		if isCaseSensitive(uch):
			caseList.append((ch, hex(ch), uch, getCharacterName(uch)))
			caseTable[ch] = '1'

	print(len(caseList), caseList[-1])
	groups = getUnicodeCaseSensitivityGroup(caseList)

	with open('caseList.log', 'w', encoding='utf-8') as fd:
		output = io.StringIO()
		output.write('caseList:\n')
		pprint(caseList, stream=output, width=150)
		output.write('\ngroups:\n')
		pprint(groups, stream=output, width=80)
		fd.write(output.getvalue())

	if not filename:
		return

	maskTable = []
	first = groups[0]['max']
	maxCh = caseList[-1][0]

	for i in range(0, first, 32):
		mask = int(''.join(reversed(caseTable[i:i+32])), 2)
		maskTable.append(mask)

	output = [f"// Created with Python {platform.python_version()}, Unicode {unicodedata.unidata_version}"]
	output.append(f'#define kUnicodeCaseSensitiveFirst\t0x{first:04x}U')
	output.append(f'#define kUnicodeCaseSensitiveMax\t0x{maxCh:04x}U')
	output.append('')

	output.append('static const UnicodeCaseSensitivityRange UnicodeCaseSensitivityRangeList[] = {')
	for group in groups[1:]:
		minCh = group['min']
		maxCh = group['max']
		if len(group['ranges']) == 1:
			output.append(f'\t{{ 0x{minCh:04x}, 0x{maxCh:04x}, 0 }},')
		else:
			output.append(f'\t{{ 0x{minCh:04x}, 0x{maxCh:04x}, {len(maskTable)} }},')
			for i in range(minCh, maxCh, 32):
				mask = int(''.join(reversed(caseTable[i:i+32])), 2)
				maskTable.append(mask)
	output.append('};')
	output.append('')

	output.append('static const uint32_t UnicodeCaseSensitivityMask[] = {')
	for i in range(0, len(maskTable), 8):
		line = ', '.join(f'0x{mask:08x}U' for mask in maskTable[i:i+8])
		output.append(line + ',')
	output.append('};')
	output.append('')

	with open(filename, 'w', encoding='utf-8') as fd:
		fd.write(r"""#include <cstdint>
#include "../include/VectorISA.h"
#define COUNTOF(a)		(sizeof(a) / sizeof(a[0]))

typedef struct UnicodeCaseSensitivityRange {
	uint32_t low;
	uint32_t high;
	uint32_t offset;
} UnicodeCaseSensitivityRange;

""")
		fd.write('\n'.join(output))
		fd.write(r"""
bool IsCharacterCaseSensitive(uint32_t ch) {
	if (ch < kUnicodeCaseSensitiveFirst) {
		return BitTestEx(UnicodeCaseSensitivityMask, ch);
	}
	for (uint32_t index = 0; index < COUNTOF(UnicodeCaseSensitivityRangeList); index++) {
		const UnicodeCaseSensitivityRange range = UnicodeCaseSensitivityRangeList[index];
		if (ch < range.high) {
			if (ch < range.low) {
				return false;
			}
			if (range.offset) {
				ch -= range.low;
				return BitTestEx(UnicodeCaseSensitivityMask + range.offset, ch);
			}
			return true;
		}
	}
	return false;
}
""")
		addCaseSensitivityTest(fd, caseTable, groups[-1]['max'])

def updateCaseSensitivity(filename, test=False):
	caseTable = ['0']*UnicodeCharacterCount
	maskTable = [0] * (UnicodeCharacterCount >> 5)
	first = 0x600
	maxCh = 0

	for ch in range(UnicodeCharacterCount):
		if isCaseSensitive(chr(ch)):
			maxCh = ch
			caseTable[ch] = '1'
			maskTable[ch >> 5] |= (1 << (ch & 31))

	maskCount = 1 + (maxCh >> 5)
	maskList = maskTable[:(first >> 5)]
	maskTable = maskTable[len(maskList):maskCount]
	indexTable = []
	for mask in maskTable:
		try:
			index = maskList.index(mask)
		except ValueError:
			index = len(maskList)
			maskList.append(mask)
		indexTable.append(index)

	print('Unicode Case Sensitivity maskList:', len(maskList), 'indexTable:', len(indexTable), maskCount, maxCh)

	config = {
		'tableName': 'UnicodeCaseSensitivityIndex',
		'function': """static inline bool IsCharacterCaseSensitiveSecond(uint32_t ch) {
	const uint32_t lower = ch & 31;
	ch = (ch - kUnicodeCaseSensitiveFirst) >> 5;""",
	}

	table, function = buildMultiStageTable('Unicode Case Sensitivity', indexTable, config)
	table[0] = 'static ' + table[0]
	function.append('\treturn bittest(UnicodeCaseSensitivityMask + ch, lower);')
	function.append('}')

	output = [f"// Created with Python {platform.python_version()}, Unicode {unicodedata.unidata_version}"]
	output.append(f'#define kUnicodeCaseSensitiveFirst\t0x{first:04x}U')
	output.append(f'#define kUnicodeCaseSensitiveMax\t0x{maxCh:04x}U')
	output.append('')
	output.extend(table)
	output.append('')

	output.append('static const uint32_t UnicodeCaseSensitivityMask[] = {')
	for i in range(0, len(maskList), 8):
		line = ', '.join(f'0x{mask:08x}U' for mask in maskList[i:i+8])
		output.append(line + ',')
	output.append('};')

	output.append('// case sensitivity for ch in [kUnicodeCaseSensitiveFirst, kUnicodeCaseSensitiveMax]')
	output.extend(function)

	if not test:
		Regenerate(filename, "//case", output)
		return

	with open(filename, 'w', encoding='utf-8') as fd:
		fd.write(r"""#include <cstdint>
#include "../include/VectorISA.h"

""")
		fd.write('\n'.join(output))
		fd.write(r"""

bool IsCharacterCaseSensitive(uint32_t ch)	{
	if (ch < kUnicodeCaseSensitiveFirst) {
		return BitTestEx(UnicodeCaseSensitivityMask, ch);
	}
	if (ch > kUnicodeCaseSensitiveMax) {
		return false;
	}
	return IsCharacterCaseSensitiveSecond(ch);
}
""")

		addCaseSensitivityTest(fd, caseTable, maskCount*32)

def updateCaseSensitivityBlock(filename, test=False):
	caseTable = ['0']*UnicodeCharacterCount
	maskTable = [0] * (UnicodeCharacterCount >> 5)
	first = 0x600
	maxCh = 0

	for ch in range(UnicodeCharacterCount):
		if isCaseSensitive(chr(ch)):
			maxCh = ch
			caseTable[ch] = '1'
			maskTable[ch >> 5] |= (1 << (ch & 31))

	# divide characters into blocks, filter out blocks with all character not case sensitive.
	blockSizeBit = 2
	blockSize = 1 << blockSizeBit
	firstCount = first >> 5
	maskCount = 1 + (maxCh >> 5)
	maskCount = blockSize * ((maskCount + blockSize - 1) // blockSize)
	maskList = maskTable[:firstCount]

	blockIndexValueBit = 7
	blockIndexCount = 1 << blockIndexValueBit
	blockList = []
	blockData = [(0, 0)] * blockIndexCount
	blockIndex = [0] * blockIndexCount
	maxBlockId = (maskCount // blockSize - 1) >> blockIndexValueBit
	blockBitCount = maxBlockId.bit_length()
	indexBitCount = 8 - blockBitCount
	maxIndex = 1 << indexBitCount
	overlapped = False

	for i in range(firstCount, maskCount, blockSize):
		block = tuple(maskTable[i:i+blockSize])
		if sum(block) == 0:
			continue
		try:
			index = blockList.index(block)
		except ValueError:
			index = len(blockList)
			blockList.append(block)

		index += 1
		blockId = i // blockSize
		blockSlot = blockId & (blockIndexCount - 1)
		if blockData[blockSlot][1]:
			print('multi block', blockId, blockSlot, blockData[blockSlot], index)
		if index > maxIndex:
			overlapped = True
			print('overlapped block', blockId, blockSlot, index)

		blockId = blockId >> blockIndexValueBit
		blockData[blockSlot] = (blockId, index)
		blockIndex[blockSlot] = index | (blockId << indexBitCount)

	#lines = []
	#for i in range(0, len(blockData), 8):
	#	line = ', '.join('(%d,%2d)' % item for item in blockData[i:i+8])
	#	lines.append(line)
	#print('\n'.join(lines))
	if overlapped:
		return

	indexTable = []
	for block in blockList:
		for mask in block:
			try:
				index = maskList.index(mask)
			except ValueError:
				index = len(maskList)
				maskList.append(mask)
			indexTable.append(index)

	size = len(blockIndex) + len(indexTable) + len(maskList)*4
	print('caseBlock', blockSize, len(maskList), len(blockIndex), len(indexTable), size)

	output = [f"// Created with Python {platform.python_version()}, Unicode {unicodedata.unidata_version}"]
	output.append(f'#define kUnicodeCaseSensitiveFirst\t0x{first:04x}U')
	output.append(f'#define kUnicodeCaseSensitiveMax\t0x{maxCh:04x}U')
	output.append('')

	output.append('static const uint8_t UnicodeCaseSensitivityIndex[] = {')
	output.append('// block index')
	for i in range(0, len(blockIndex), 32):
		line = ', '.join(map(str, blockIndex[i:i+32]))
		output.append(line + ',')
	output.append('// mask index')
	for i in range(0, len(indexTable), 32):
		line = ', '.join(map(str, indexTable[i:i+32]))
		output.append(line + ',')
	output.append('};')

	output.append('')
	output.append('static const uint32_t UnicodeCaseSensitivityMask[] = {')
	for i in range(0, len(maskList), 8):
		line = ', '.join(f'0x{mask:08x}U' for mask in maskList[i:i+8])
		output.append(line + ',')
	output.append('};')

	indexMask = (1 << indexBitCount) - 1
	indexOffset = blockIndexCount - blockSize
	# the condition is: index != 0 && (index >> indexBitCount) == (block >> blockIndexValueBit)
	# => index != 0 && ((index >> indexBitCount) ^ (block >> blockIndexValueBit)) == 0
	# set block = index ^ (block >> (blockIndexValueBit - indexBitCount)
	# => index != 0 && (block >> indexBitCount) == 0
	# => index != 0 && block < indexMask + 1
	# set diff = block - (indexMask + 1), with 2's complement, when diff >= 0, diff >> 8 is zero;
	# when diff < 0, diff >> 8 has 24-bit (or 32-bit using arithmetic shift right) 1s on the right.
	function = f"""
// case sensitivity for ch in [kUnicodeCaseSensitiveFirst, kUnicodeCaseSensitiveMax]
static inline bool IsCharacterCaseSensitiveSecond(uint32_t ch) {{
	uint32_t block = ch >> {blockSizeBit + 5};
	uint32_t index = UnicodeCaseSensitivityIndex[block & {hex(blockIndexCount - 1)}];
	block = index ^ (block >> {blockIndexValueBit - indexBitCount});
	index &= ((block - {hex(indexMask + 1)}) >> 8) & {hex(indexMask)};
	if (index) {{
		ch = ch & {hex(blockSize*32 - 1)};
		index = {indexOffset} + (index << {blockSizeBit});
		index = UnicodeCaseSensitivityIndex[index + (ch >> 5)];
		return bittest(UnicodeCaseSensitivityMask + index, ch & 31);
	}}
	return false;
}}
"""
	output.extend(function.splitlines())

	if not test:
		Regenerate(filename, "//case", output)
		return

	with open(filename, 'w', encoding='utf-8') as fd:
		fd.write(r"""#include <cstdint>
#include "../include/VectorISA.h"

""")
		fd.write('\n'.join(output))
		fd.write(r"""

bool IsCharacterCaseSensitive(uint32_t ch) {
	if (ch < kUnicodeCaseSensitiveFirst) {
		return BitTestEx(UnicodeCaseSensitivityMask, ch);
	}
	if (ch > kUnicodeCaseSensitiveMax) {
		return false;
	}
	return IsCharacterCaseSensitiveSecond(ch);
}
""")

		addCaseSensitivityTest(fd, caseTable, maskCount*32)

updateCaseConvert()
#checkUnicodeCaseSensitivity('caseList.cpp')
#updateCaseSensitivity('CaseSensitivity.cpp', True)
#updateCaseSensitivity('../../src/EditEncoding.c')
#updateCaseSensitivityBlock('caseBlock.cpp', True)
updateCaseSensitivityBlock('../../src/EditEncoding.c')
