#!/usr/bin/env python3
# Script to generate line breaking data from
# http://www.unicode.org/Public/UCD/latest/ucd/LineBreak.txt

import sys, platform, unicodedata
from enum import IntFlag
import re
from FileGenerator import Regenerate
from GenerateCharacterCategory import *

# Unicode Line Breaking Algorithm
# http://www.unicode.org/reports/tr14/

class LineBreak(IntFlag):
	NonBreak = 0
	BreakBefore = 1	# B
	BreakAfter = 2	# A
	BreakAny = 3	# B/A

	RLEValueBit = 2

# http://www.unicode.org/reports/tr14/#Properties
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

def setRange(values, low, high, value):
	while low <= high:
		values[low] = value
		low += 1

kUnicodeLineBreak = []
kUnicodeLineBreakVersion = unicodedata.unidata_version

def readLineBreakFile(filename='LineBreak.txt'):
	global kUnicodeLineBreak, kUnicodeLineBreakVersion

	data = ['XX'] * UnicodeCharacterCount	# @missing
	setRange(data, 0x3400, 0x4DBF, 'ID')	# CJK Unified Ideographs Extension A
	setRange(data, 0x4E00, 0x9FFF, 'ID')	# CJK Unified Ideographs
	setRange(data, 0xF900, 0xFAFF, 'ID')	# CJK Compatibility Ideographs
	setRange(data, 0x20000, 0x2FFFD, 'ID')	# Plane 2
	setRange(data, 0x30000, 0x3FFFD, 'ID')	# Plane 3
	setRange(data, 0x1F000, 0x1FFFD, 'ID')	# Plane 1 range
	setRange(data, 0x20A0, 0x20CF, 'PR')	# Currency Symbols

	version = ''
	with open(filename, encoding='utf-8') as fd:
		for line in fd.readlines():
			line = line.strip()
			if not line or line[0] == '#':
				if not version:
					# first line
					version = re.findall(r'(\d+\.\d+\.\d+)', line)[0]
				continue

			m = re.match(r'(\w+)(\.\.(\w+))?;(\w+)', line)
			start = int(m.groups()[0], 16)
			end = m.groups()[2]
			value = m.groups()[3]
			if end:
				end = int(end, 16)
				setRange(data, start, end, value)
			else:
				data[start] = value

	data[ord('<')] = 'OP' # open punctuation
	data[ord('>')] = 'CL' # close punctuation
	data[ord('#')] = 'PR' # prefix
	data[ord('@')] = 'PR' # prefix

	# AL => break after
	data[ord('&')] = 'BA'
	data[ord('*')] = 'BA'
	data[ord('=')] = 'BA'
	data[ord('^')] = 'BA'

	# AL => break before
	data[ord('~')] = 'BB'

	# fullwidth forms
	# https://en.wikipedia.org/wiki/Halfwidth_and_fullwidth_forms
	for ch in range(0xFF01, 0xFF5F):
		data[ch] = data[ch - 0xFEE0]

	kUnicodeLineBreak = data
	kUnicodeLineBreakVersion = version
	print('Unicode LineBreak version:', version)

def updateUnicodeLineBreak(filename):
	indexTable = [0] * UnicodeCharacterCount
	for ch, prop in enumerate(kUnicodeLineBreak):
		category = unicodedata.category(chr(ch))
		cc = ClassifyMap[category]
		lb = LineBreakMap[prop]
		if cc == CharClassify.ccWord and lb != LineBreak.BreakAny:
			lb = LineBreak.NonBreak
		indexTable[ch] = int(lb)

	#runLengthEncode('Unicode LineBreak', indexTable[:BMPCharacterCharacterCount], LineBreak.RLEValueBit)
	#compressIndexTable('Unicode LineBreak', indexTable, args)

	output = ["// Created with Python %s, Unicode %s" % (
		platform.python_version(), kUnicodeLineBreakVersion)]
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

			prop = kUnicodeLineBreak[ch]
			lb = LineBreakMap[prop]
			category = unicodedata.category(uch)
			name = ''
			try:
				name = unicodedata.name(uch)
			except ValueError:
				pass

			uch = escapeMap.get(uch, uch)
			fd.write(f'{ch :02X} {value}; {category} {prop} {lb.name}; {uch} {name}\n')

if __name__ == '__main__':
	readLineBreakFile()
	updateUnicodeLineBreak("../src/EditView.cxx")
