# https://www.unicode.org/reports/tr41/
# https://www.unicode.org/reports/tr11/
# https://www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt

from FileGenerator import Regenerate
from MultiStageTable import *
from UnicodeData import *

def updateEastAsianWidthTable(filename):
	version, propertyList = readUnicodePropertyFile('EastAsianWidth.txt')
	# https://www.unicode.org/reports/tr11/#Recommendations
	widthMap = {
		'N': 0, # Neutral
		'Na': 0,# Narrow
		'A': 0, # Ambiguous
		'W': 1, # Wide
		'H': 0, # Halfwidth
		'F': 1, # Fullwidth
	}
	widthTable = [0] * UnicodeCharacterCount
	updateUnicodePropertyTable(widthTable, widthMap, propertyList)

	tableSize = getMinTableSize(widthTable, 0)
	tableSize = alignUp(tableSize, 32)
	maskTable = [0] * (tableSize >> 5)
	minCh = 0
	maxCh = 0
	for ch in range(tableSize):
		if widthTable[ch]:
			maxCh = ch
			if not minCh:
				minCh = ch
			maskTable[ch >> 5] |= (1 << (ch & 31))
	print('EastAsianWidth minCh:', minCh, 'maxCh:', maxCh)

	maskList = { 0 : 0 }
	indexList = []
	for mask in maskTable:
		if mask in maskList:
			index = maskList[mask]
		else:
			index = len(maskList)
			maskList[mask] = index
		indexList.append(index)

	config = {
		'tableName': 'EastAsianWidthIndex',
		'function': f"""int GetEastAsianWidth(uint32_t ch) noexcept {{
	if (ch < {minCh} || ch > {maxCh}) {{
		return 0;
	}}
	const uint32_t lower = ch & 31;
	ch >>= 5;
""",
		'returnType': None,
	}

	maskList = list(maskList.keys())
	print('EastAsianWidth maskList:', len(maskList), len(maskList)*4)
	table, function = buildMultiStageTable('EastAsianWidth', indexList, config=config, level=2)
	function.append('\treturn bittest(EastAsianWidthMask + ch, lower);\n}')

	output = []
	output.append("""#ifndef MeasureWidthsUseEastAsianWidth
#define MeasureWidthsUseEastAsianWidth
#endif
""")
	output.append('namespace {')
	output.append('')
	output.extend(table)
	output.append('')
	output.append('const uint32_t EastAsianWidthMask[] = {')
	output.extend(dumpArray(maskList, 8, '0x%08XU'))
	output.append('};')
	output.append('')
	output.extend(function)
	output.append('')
	output.append('}')

	with open(filename, 'w', encoding='utf-8', newline='\n') as fd:
		fd.write('\n'.join(output))

if __name__ == '__main__':
	updateEastAsianWidthTable('../src/EastAsianWidth.h')
