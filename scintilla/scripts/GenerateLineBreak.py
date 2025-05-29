# script to generate line breaking data.

import platform
import unicodedata
from enum import IntFlag

from FileGenerator import Regenerate
from GenerateCharacterCategory import CharacterClass, CategoryClassifyMap
from MultiStageTable import *
from UnicodeData import *

# Unicode Line Breaking Algorithm
# https://www.unicode.org/reports/tr14/
# https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/LineBreakTest.html
class LineBreak(IntFlag):
	NonBreak = 0
	BreakBefore = 1	# B
	BreakAfter = 2	# A
	BreakAny = 7	# B/A
	Undefined = 3

# https://www.unicode.org/reports/tr14/#Properties
# https://github.com/unicode-org/cldr/blob/main/common/segments/root.xml
LineBreakPropertyMap = {
	LineBreak.NonBreak: [
		'AK',	# Aksara (XB/XA)
		'AL',	# Ordinary Alphabetic and Symbol Characters (XP)
		'AS',	# Aksara Start (XB/XA)
		'EM',	# Emoji Modifier (A), Do not break between an emoji base and an emoji modifier.
		'GL', 	# Non-breaking (“Glue”) (XB/XA) (Non-tailorable)
		'HL', 	# Hebrew Letter (XB)
		'IN',	# Inseparable Characters (XP)
		'JV', 	# Hangul V Jamo (XA/XB)
		'NU',	# Numeric (XP)
		'QU',	# Quotation (XB/XA)
		'RI',	# Regional Indicator (B/A/XP)
		'SA',	# Complex-Context Dependent (South East Asian) (P)
		'SG', 	# Surrogate (XP) (Non-tailorable)
		'VI',	# Virama (XB/XA)
		'WJ',	# Word Joiner (XB/XA) (Non-tailorable)
		'XX',	# Unknown (XP)
		'ZWJ',	# Zero Width Joiner (XA/XB) (Non-tailorable)
	],
	LineBreak.BreakBefore: [
		'AP',	# Aksara Pre-Base (B/XA)
		'BB',	# Break Before (B)
		'EB',	# Emoji Base (B/A), may followed by emoji modifier

		'OP',	# Open Punctuation (XA)
		'OP30',	# Open Punctuation (XA)
		'PR',	# Prefix Numeric (XA)
	],
	LineBreak.BreakAfter: [
		'BA',	# Break After (A)
		'BK',	# Mandatory Break (A) (Non-tailorable)
		'CR',	# Carriage Return (A) (Non-tailorable)
		'LF',	# Line Feed (A) (Non-tailorable)
		'NL', 	# Next Line (A) (Non-tailorable)
		'SP',	# Space (A) (Non-tailorable)
		'SY',	# Symbols Allowing Break After (A)
		'ZW',	# Zero Width Space (A) (Non-tailorable)

		'HY',	# Hyphen (XA), U+2010 HYPHEN, TODO: fix minus
		'CM',	# Combining Mark (XB) (Non-tailorable)
		'CL',	# Close Punctuation (XB)
		'CP', 	# Closing Parenthesis (XB), TODO: fix (s)he
		'CP30', # Closing Parenthesis (XB), TODO: fix (s)he
		'EX',	# Exclamation/Interrogation (XB)
		'IS',	# Infix Numeric Separator (XB)
		'NS',	# Nonstarters (XB)
		'PO',	# Postfix Numeric (XB)
		'VF',	# Virama Final (XB/A)
	],
	LineBreak.BreakAny: [
		'AI',	# Ambiguous (Alphabetic or Ideograph)
		'B2',	# Break Opportunity Before and After (B/A/XP)
		'CB',	# Contingent Break Opportunity (B/A)
		'CJ',	# Conditional Japanese Starter, treat as ID: CSS normal breaking
		'H2',	# Hangul LV Syllable (B/A)
		'H3',	# Hangul LVT Syllable (B/A)
		'JT',	# Hangul T Jamo (A)
		'JL',	# Hangul L Jamo (B)
		'ID',	# Ideographic (B/A)
	],
}

LineBreakMap = flattenPropertyMap(LineBreakPropertyMap)
LineBreakList = list(LineBreakMap.keys())
LineBreakOpportunity = [2**len(LineBreakList) - 1]*(len(LineBreakList))

def buildLineBreakOpportunity():
	table = LineBreakOpportunity

	notBreak = {
	'AI': 'AI AL BA BK CJ CL CM CP CR EX GL HL HY IN IS LF NL NS NU OP30 PO PR QU SA SP SY WJ XX ZW ZWJ',
	'AK': 'BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU SP SY VF VI WJ ZW ZWJ',
	'AL': 'AI AL BA BK CJ CL CM CP CR EX GL HL HY IN IS LF NL NS NU OP30 PO PR QU SA SP SY VI WJ XX ZW ZWJ',
	'AP': 'AK AS BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU SP SY WJ ZW ZWJ',
	'AS': 'BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU SP SY VF VI WJ ZW ZWJ',
	'B2': 'B2 BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU SP SY WJ ZW ZWJ',
	'BA': 'BA BK CJ CL CM CP CR EX HY IN IS LF NL NS QU SP SY WJ ZW ZWJ',
	'BB': 'AI AK AL AP AS B2 BA BB BK CJ CL CM CP CR EB EM EX GL H2 H3 HL HY ID IN IS JL JT JV LF NL NS NU OP OP30 PO PR QU RI SA SP SY VF VI WJ XX ZW ZWJ',
	'CB': 'BK CL CM CP CR EX GL IS LF NL QU SP SY WJ ZW ZWJ',
	'CJ': 'BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU SP SY WJ ZW ZWJ',
	'CL': 'BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU SP SY WJ ZW ZWJ',
	'CM': 'AI AL BA BK CJ CL CM CP CR EX GL HL HY IN IS LF NL NS NU PO QU SA SP SY WJ XX ZW ZWJ',
	'CP': 'AI AL BA BK CJ CL CM CP CR EX GL HL HY IN IS LF NL NS NU QU SA SP SY WJ XX ZW ZWJ',
	'CR': 'LF',
	'EB': 'BA BK CJ CL CM CP CR EM EX GL HY IN IS LF NL NS PO QU SP SY WJ ZW ZWJ',
	'EM': 'BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS PO QU SP SY WJ ZW ZWJ',
	'EX': 'BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU SP SY WJ ZW ZWJ',
	'GL': 'AI AK AL AP AS B2 BA BB BK CB CJ CL CM CP CR EB EM EX GL H2 H3 HL HY ID IN IS JL JT JV LF NL NS NU OP OP30 PO PR QU RI SA SP SY VF VI WJ XX ZW ZWJ',
	'H2': 'BA BK CJ CL CM CP CR EX GL HY IN IS JT JV LF NL NS PO QU SP SY WJ ZW ZWJ',
	'H3': 'BA BK CJ CL CM CP CR EX GL HY IN IS JT LF NL NS PO QU SP SY WJ ZW ZWJ',
	'HL': 'AI AL BA BK CJ CL CM CP CR EX GL HL HY IN IS LF NL NS NU OP30 PO PR QU SA SP SY WJ XX ZW ZWJ',
	'HY': 'AI BA BK CJ CL CM CP CR EX HY IN IS LF NL NS NU QU SA SP SY WJ XX ZW ZWJ',
	'ID': 'BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS PO QU SP SY WJ ZW ZWJ',
	'IN': 'BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU SP SY WJ ZW ZWJ',
	'IS': 'AI AL BA BK CJ CL CM CP CR EX GL HL HY IN IS LF NL NS NU QU SA SP SY WJ XX ZW ZWJ',
	'JL': 'BA BK CJ CL CM CP CR EX GL H2 H3 HY IN IS JL JV LF NL NS PO QU SP SY WJ ZW ZWJ',
	'JT': 'BA BK CJ CL CM CP CR EX GL HY IN IS JT LF NL NS PO QU SP SY WJ ZW ZWJ',
	'JV': 'BA BK CJ CL CM CP CR EX GL HY IN IS JT JV LF NL NS PO QU SP SY WJ ZW ZWJ',
	'NS': 'BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU SP SY WJ ZW ZWJ',
	'NU': 'AI AL BA BK CJ CL CM CP CR EX GL HL HY IN IS LF NL NS NU OP30 PO PR QU SA SP SY WJ XX ZW ZWJ',
	'OP': 'AI AK AL AP AS B2 BA BB BK CB CJ CL CM CP CR EB EM EX GL H2 H3 HL HY ID IN IS JL JT JV LF NL NS NU OP OP30 PO PR QU RI SA SP SY VF VI WJ XX ZW ZWJ',
	'OP30': 'AI AK AL AP AS B2 BA BB BK CB CJ CL CM CP CR EB EM EX GL H2 H3 HL HY ID IN IS JL JT JV LF NL NS NU OP OP30 PO PR QU RI SA SP SY VF VI WJ XX ZW ZWJ',
	'PO': 'AI AL BA BK CJ CL CM CP CR EX GL HL HY IN IS LF NL NS NU QU SA SP SY WJ XX ZW ZWJ',
	'PR': 'AI AL BA BK CJ CL CM CP CR EB EM EX GL H2 H3 HL HY ID IN IS JL JT JV LF NL NS NU QU SA SP SY WJ XX ZW ZWJ',
	'QU': 'AI AK AL AP AS B2 BA BB BK CB CJ CL CM CP CR EB EM EX GL H2 H3 HL HY ID IN IS JL JT JV LF NL NS NU OP OP30 PO PR QU RI SA SP SY VF VI WJ XX ZW ZWJ',
	'RI': 'BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU RI SP SY WJ ZW ZWJ',
	'SA': 'AI AL BA BK CJ CL CM CP CR EX GL HL HY IN IS LF NL NS NU OP OP30 PO PR QU SA SP SY WJ XX ZW ZWJ',
	'SP': 'BK CL CP CR EX IS LF NL SP SY WJ ZW',
	'SY': 'BA BK CJ CL CM CP CR EX GL HL HY IN IS LF NL NS QU SP SY WJ ZW ZWJ',
	'VF': 'BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU SP SY WJ ZW ZWJ',
	'VI': 'AK BA BK CJ CL CM CP CR EX GL HY IN IS LF NL NS QU SP SY WJ ZW ZWJ',
	'WJ': 'AI AK AL AP AS B2 BA BB BK CB CJ CL CM CP CR EB EM EX GL H2 H3 HL HY ID IN IS JL JT JV LF NL NS NU OP OP30 PO PR QU RI SA SP SY VF VI WJ XX ZW ZWJ',
	'XX': 'AI AL BA BK CJ CL CM CP CR EX GL HL HY IN IS LF NL NS NU PO PR QU SA SP SY WJ XX ZW ZWJ',
	'ZW': 'BK CR LF NL SP ZW',
	'ZWJ': 'AI AK AL AP AS B2 BA BB BK CB CJ CL CM CP CR EB EM EX GL H2 H3 HL HY ID IN IS JL JT JV LF NL NS NU OP OP30 PO PR QU RI SA SP SY VF VI WJ XX ZW ZWJ',
	}

	for key, row in notBreak.items():
		first = LineBreakList.index(key)
		row = row.split()
		expected = sorted(set(row))
		if row != expected:
			print(f'{key}: {" ".join(expected)}', file=sys.stderr)
		for item in row:
			second = LineBreakList.index(item)
			table[first] &= ~(1 << second)

	for index, value in enumerate(table):
		prev = LineBreakList[index]
		for j in range(index + 1, len(table)):
			if value == table[j]:
				first = tuple((item >> index) & 1 for item in table)
				second = tuple((item >> j) & 1 for item in table)
				if first == second:
					current = LineBreakList[j]
					print('same row and column:', prev, current)

def buildWrapBreakMask(filename, prevIndex=False):
	wbMax = max(LineBreak.__members__.values())
	ccMax = max(CharacterClass.__members__.values())
	maxValue = 8
	table = [0]*8
	assert LineBreak.BreakAny == LineBreak.Undefined + CharacterClass.CJKWord
	for prev in range(maxValue):
		wbPrev = LineBreak(min(prev, LineBreak.Undefined))
		ccPrev = CharacterClass(max(prev - LineBreak.Undefined, 0))
		if ccPrev == CharacterClass.CJKWord:
			wbPrev = LineBreak.BreakAny
		for pos in range(maxValue):
			wbPos = LineBreak(min(pos, LineBreak.Undefined))
			ccPos = CharacterClass(max(pos - LineBreak.Undefined, 0))
			if ccPos == CharacterClass.CJKWord:
				wbPos = LineBreak.BreakAny
			if wbPrev != LineBreak.BreakBefore and wbPos != LineBreak.BreakAfter:
				if wbPrev == LineBreak.BreakAny or wbPos == LineBreak.BreakAny \
				or ccPrev == CharacterClass.CJKWord or ccPos == CharacterClass.CJKWord \
				or (wbPrev != wbPos and (wbPrev == LineBreak.BreakAfter or wbPos == LineBreak.BreakBefore)) \
				or (ccPrev != ccPos and (wbPrev == LineBreak.Undefined or wbPos == LineBreak.Undefined)):
					#print(f'({wbPrev.name}, {ccPrev.name}), ({wbPos.name}, {ccPos.name})')
					if prevIndex:
						table[prev] |= (1 << pos)
					else:
						table[pos] |= (1 << prev)

	output = [f'{bitValue(value, 8)},' for value in table]
	Regenerate(filename, "//mask", output)

def testLineBreak(path, lineBreakTable, eastAsianWidth):
	opportunity = '×÷'
	allow = opportunity[1]
	total = 0
	fail = [0, 0]
	failStat = {}
	ignore = 0
	cpPrev = 0
	propPrev = 'sot'
	Hyphen = 0x2010
	# DottedCircle = 0x25CC
	with open(path, encoding='utf-8') as fd:
		lineno = 0
		indent = ' '*4
		for line in fd.readlines():
			lineno += 1
			line = line.strip()
			if not line or line[0] == '#':
				continue

			sequence = line.split('#', 2)[0].split()
			assert sequence[0] != allow, (lineno, sequence)
			assert sequence[-1] == allow, (lineno, sequence)
			for index in range(1, len(sequence) - 2, 2):
				ch = sequence[index]
				official = sequence[index + 1]
				chNext = sequence[index + 2]
				# if ch == '000D' and chNext == '000A':
				# 	ignore += 1
				# 	continue
				cp = int(ch, 16)
				cpNext = int(chNext, 16)
				prop = lineBreakTable[cp]
				propNext = lineBreakTable[cpNext]
				result = (LineBreakOpportunity[LineBreakList.index(prop)] >> LineBreakList.index(propNext)) & 1
				if result == 1:
					# 12 $GL ×
					if prop == 'CM' and propPrev == 'GL':
						result = 0
					# 14 $OP $SP* ×
					if prop == 'SP' and propPrev == 'OP':
						result = 0
					if prop == 'CM' and propPrev == 'OP':
						result = 0
					# 16 ($CL | $CP) $SP* × $NS
					if prop == 'SP' and propNext == 'NS' and propPrev in ('CL', 'CP'):
						result = 0
					# 17 $B2 $SP* × $B2
					if prop == 'SP' and propNext == 'B2' and propPrev == 'B2':
						result = 0
					# 20.1 ( $sot | $BK | $CR | $LF | $NL | $SP | $ZW | $CB | $GL ) ( $HY | $Hyphen ) × $AL
					if (prop == 'HY' or cp == Hyphen) and propNext == 'AL' and propPrev in ('sot', 'BK', 'CR', 'LF', 'NL', 'SP', 'ZW', 'CB', 'GL'):
						result = 0
					# 21.1 $HL ($HY | $NonEastAsianBA) × [^$HL]
					if prop == 'HY' and propNext != 'HL' and propPrev == 'HL':
						result = 0
					# 24.03 ($AL | $HL) × ($PR | $PO)
					if prop == 'CM' and propNext in ('PR', 'PO') and propPrev in ('AL', 'HL'):
						result = 0
					# 25.05 $NU ( $SY | $IS )* × $PO
					# 25.06 $NU ( $SY | $IS )* × $PR
					if prop in ('SY', 'IS') and propNext in ('PO', 'PR') and propPrev == 'NU':
						result = 0

				value = opportunity[result]
				total += 1
				if value != official:
					fail[result ^ 1] += 1
					key = f'{prop} {value} {propNext}'
					print(f'test fail on line {lineno}: {ch} {official} {chNext} => {key}')
					print(f'{indent}{line}')
					if key in failStat:
						failStat[key] += 1
					else:
						failStat[key] = 1
				cpPrev = cp
				propPrev = prop

	lines = [f'{key}: {value}' for key, value in sorted(failStat.items())]
	print('\n'.join(lines))
	print(f'{path} total test: {total}, failed {sum(fail)}: {opportunity[0]} {fail[0]}, {opportunity[1]} {fail[1]}, ignored: {ignore}', file=sys.stderr)

def updateUnicodeLineBreak(filename):
	# https://www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt
	version, propertyList = readUnicodePropertyFile('EastAsianWidth.txt')
	eastAsianWidth = ['N'] * UnicodeCharacterCount
	flattenUnicodePropertyTable(eastAsianWidth, propertyList)

	lineBreakTable = ['XX'] * UnicodeCharacterCount	# @missing
	# https://www.unicode.org/Public/UCD/latest/ucd/LineBreak.txt
	version, propertyList = readUnicodePropertyFile('LineBreak.txt')
	flattenUnicodePropertyTable(lineBreakTable, propertyList)
	for index, prop in enumerate(lineBreakTable):
		width = eastAsianWidth[index]
		east = width in ('F', 'W', 'H')
		if prop == 'OP' and not east:
			lineBreakTable[index] = 'OP30'

	# https://www.unicode.org/Public/UCD/latest/ucd/extracted/DerivedLineBreak.txt
	# derived = ['XX'] * UnicodeCharacterCount
	# missingMap = {'Unknown': 'XX', 'Ideographic': 'ID', 'Prefix_Numeric': 'PR'};
	# version, propertyList = readUnicodePropertyFile('DerivedLineBreak.txt', withMissing=True, missingMap=missingMap)
	# flattenUnicodePropertyTable(derived, propertyList)
	# assert lineBreakTable == derived

	# buildLineBreakOpportunity()
	# testLineBreak('LineBreakTest.txt', lineBreakTable, eastAsianWidth)

	lineBreakTable[ord('<')] = 'OP' # open punctuation
	lineBreakTable[ord('>')] = 'CL' # close punctuation
	lineBreakTable[ord('#')] = 'PR' # prefix
	lineBreakTable[ord('@')] = 'PR' # prefix

	# AL => break after
	lineBreakTable[ord('&')] = 'BA'
	lineBreakTable[ord('*')] = 'BA'
	lineBreakTable[ord('=')] = 'BA'
	lineBreakTable[ord('^')] = 'BA'

	# AL => break before
	lineBreakTable[ord('~')] = 'BB'

	# fullwidth forms
	# https://en.wikipedia.org/wiki/Halfwidth_and_fullwidth_forms
	for ch in range(0xFF01, 0xFF5F):
		lineBreakTable[ch] = lineBreakTable[ch - 0xFEE0]

	indexTable = [0] * UnicodeCharacterCount
	for ch, prop in enumerate(lineBreakTable):
		category = unicodedata.category(chr(ch))
		cc = CategoryClassifyMap[category]
		lb = LineBreakMap[prop]
		if cc == CharacterClass.Word and lb != LineBreak.BreakAny:
			lb = LineBreak.NonBreak
		indexTable[ch] = int(lb)

	runLengthEncode('Unicode LineBreak', indexTable[:BMPCharacterCharacterCount])
	config = {
		'tableName': 'lineBreakTable',
		'function': """LineBreakProperty GetLineBreakProperty(uint32_t ch) noexcept {
	if (ch >= maxUnicode) {
		return LineBreakProperty::XX;
	}
""",
		'returnType': 'LineBreakProperty',
	}
	buildMultiStageTable('Unicode LineBreak', indexTable, config)

	output = [f"// Created with Python {platform.python_version()}, Unicode {version}"]
	lines = dumpArray(indexTable[:128], 16)
	output.extend(lines)
	Regenerate(filename, "//", output)

	escapeMap = {
		'\r': '\\r',
		'\n': '\\n',
		'\t': '\\t',
		' ': 'SP'
	}
	with open('linebreak.log', 'w', encoding='utf-8', newline='\n') as fd:
		for ch, value in enumerate(indexTable[:128]):
			uch = chr(ch)
			if uch.isalnum():
				continue

			prop = lineBreakTable[ch]
			lb = LineBreakMap[prop]
			category = unicodedata.category(uch)
			name = getCharacterName(uch)

			uch = escapeMap.get(uch, uch)
			fd.write(f'{ch :02X} {value}; {category} {prop} {lb.name}; {uch} {name}\n')

if __name__ == '__main__':
	# parseSegmentationChart('LineBreakTest.html')
	buildWrapBreakMask("../src/PositionCache.cxx")
	updateUnicodeLineBreak("../src/PositionCache.cxx")
