# script to generate line breaking data.
# https://www.unicode.org/reports/tr41/
# https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/LineBreakTest.html

import platform
import unicodedata
from enum import IntFlag

from FileGenerator import Regenerate
from GenerateCharacterCategory import CharClassify, ClassifyMap
from MultiStageTable import *
from UnicodeData import *

# Unicode Line Breaking Algorithm
# https://www.unicode.org/reports/tr14/
class LineBreak(IntFlag):
	NonBreak = 0
	BreakBefore = 1	# B
	BreakAfter = 2	# A
	BreakAny = 3	# B/A

# https://www.unicode.org/reports/tr14/#Properties
LineBreakPropertyMap = {
	LineBreak.NonBreak: [
		'AL',	# Ordinary Alphabetic and Symbol Characters (XP)
		'B2',	# Break Opportunity Before and After (B/A/XP)
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
		'WJ',	# Word Joiner (XB/XA) (Non-tailorable)
		'XX',	# Unknown (XP)
		'ZWJ',	# Zero Width Joiner (XA/XB) (Non-tailorable)
	],
	LineBreak.BreakBefore: [
		'BB',	# Break Before (B)
		'EB',	# Emoji Base (B/A), may followed by emoji modifier

		'OP',	# Open Punctuation (XA)
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
		'EX',	# Exclamation/Interrogation (XB)
		'IS',	# Infix Numeric Separator (XB)
		'NS',	# Nonstarters (XB)
		'PO',	# Postfix Numeric (XB)
	],
	LineBreak.BreakAny: [
		'AI',	# Ambiguous (Alphabetic or Ideograph)
		'CB',	# Contingent Break Opportunity (B/A)
		'CJ',	# Conditional Japanese Starter, treat as ID: CSS normal breaking
		'H2',	# Hangul LV Syllable (B/A)
		'H3',	# Hangul LVT Syllable (B/A)
		'JT',	# Hangul T Jamo (A)
		'JL',	# Hangul L Jamo (B)
		'ID',	# Ideographic (B/A)
	],
}

LineBreakMap = {}
for key, items in LineBreakPropertyMap.items():
	for prop in items:
		LineBreakMap[prop] = key

def updateUnicodeLineBreak(filename):
	lineBreakTable = ['XX'] * UnicodeCharacterCount	# @missing
	# https://www.unicode.org/Public/UCD/latest/ucd/LineBreak.txt
	version, propertyList = readUnicodePropertyFile('LineBreak.txt')
	flattenUnicodePropertyTable(lineBreakTable, propertyList)

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
		cc = ClassifyMap[category]
		lb = LineBreakMap[prop]
		if cc == CharClassify.ccWord and lb != LineBreak.BreakAny:
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
	updateUnicodeLineBreak("../src/EditView.cxx")
