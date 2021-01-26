#!/usr/bin/env python3
import unicodedata

from FileGenerator import Regenerate
from GenerateCharacterCategory import dumpArray

def GenerateUTF8Table():
	# for UTF8ClassifyTable in UniConversion.cxx
	def BytesFromLead(leadByte):
		# Single byte or invalid
		if leadByte < 0xC2:
			return 1
		if leadByte < 0xE0:
			return 2
		if leadByte < 0xF0:
			return 3
		if leadByte < 0xF5:
			return 4
		# Characters longer than 4 bytes not possible in current UTF-8
		return 1

	def UTF8IsLeadByte(ch):
		if ch <= 0x7F:
			return True
		if ch >= 0xC2 and ch <= 0xF4:
			return True
		return False

	def UTF8IsTrailByte(ch):
		return ch >= 0x80 and ch <= 0xBF

	def UTF8IntervalNumber(ch):
		# UTF8-1
		if ch <= 0x7F:
			return 0
		# UTF8-tail
		if ch <= 0x8F:
			return 1
		if ch <= 0x9F:
			return 2
		if ch <= 0xBF:
			return 3
		# UTF8-invalid
		if ch == 0xC0 or ch == 0xC1:
			return 11
		# UTF8-2
		if ch >= 0xC2 and ch <= 0xDF:
			return 4
		# UTF8-3
		if ch == 0xE0:
			return 5
		if ch >= 0xE1 and ch <= 0xEC:
			return 6
		if ch == 0xED:
			return 7
		if ch == 0xEE or ch == 0xEF:
			return 6
		# UTF8-4
		if ch == 0xF0:
			return 8
		if ch >= 0xF1 and ch <= 0xF3:
			return 9
		if ch == 0xF4:
			return 10
		# UTF8-invalid
		return 11

	def MakeUTF8ClassifyMask(ch):
		mask = BytesFromLead(ch)
		if UTF8IsTrailByte(ch):
			mask |= 1 << 3

		number = UTF8IntervalNumber(ch)
		mask |= number << 4
		return mask

	UTF8ClassifyTable = [MakeUTF8ClassifyMask(ch) for ch in range(256)]
	# combined intervals for second and first UTF8-3 bytes
	UTF8_3ByteMask = 0
	for ch in [0x35, 0x16, 0x26, 0x36, 0x17, 0x27]:
		UTF8_3ByteMask |= 1 << ch
		UTF8ClassifyTable[ch] |= 0x40
	# combined intervals for second and first UTF8-4 bytes
	UTF8_4ByteMask = 0
	for ch in [0x28, 0x38, 0x19, 0x29, 0x39, 0x1a]:
		UTF8_4ByteMask |= 1 << ch
		UTF8ClassifyTable[ch] |= 0x80

	lines = []
	for i in range(0, 255, 16):
		line = ', '.join('0x%02X' % ch for ch in UTF8ClassifyTable[i:i + 16]) + ','
		line += ' // %02X - %02X' % (i, i + 15)
		lines.append(line)

	print('UTF8ClassifyTable:', len(UTF8ClassifyTable), len(lines))
	print('\n'.join(lines))
	print('UTF8_3ByteMask: 0x%016x' % UTF8_3ByteMask)
	print('UTF8_4ByteMask: 0x%016x' % UTF8_4ByteMask)

def GetCharName(ch):
	try:
		return unicodedata.name(ch).title()
	except ValueError:
		return ''

def GenerateUnicodeControlCharacters():
	# for kUnicodeControlCharacterTable in Edit.c
	ucc_table = [
		"\u200E", # U+200E	LRM		Left-to-right mark
		"\u200F", # U+200F	RLM		Right-to-left mark
		"\u200D", # U+200D	ZWJ		Zero width joiner
		"\u200C", # U+200C	ZWNJ	Zero width non-joiner
		"\u202A", # U+202A	LRE		Start of left-to-right embedding
		"\u202B", # U+202B	RLE		Start of right-to-left embedding
		"\u202D", # U+202D	LRO		Start of left-to-right override
		"\u202E", # U+202E	RLO		Start of right-to-left override
		"\u202C", # U+202C	PDF		Pop directional formatting
		"\u206E", # U+206E	NADS	National digit shapes substitution
		"\u206F", # U+206F	NODS	Nominal (European) digit shapes
		"\u206B", # U+206B	ASS		Activate symmetric swapping
		"\u206A", # U+206A	ISS		Inhibit symmetric swapping
		"\u206D", # U+206D	AAFS	Activate Arabic form shaping
		"\u206C", # U+206C	IAFS	Inhibit Arabic form shaping
		"\u001E", # U+001E	RS		Record Separator (Block separator)
		"\u001F", # U+001F	US		Unit Separator (Segment separator)
		"\u2028", # U+2028	LS		Line Separator
		"\u2029", # U+2029	PS		Paragraph Separator
		"\u200B", # U+200B	ZWSP	Zero width space
		"\u2060", # U+2060	WJ		Word joiner
		"\u2066", # U+2066	LRI		Left-to-right isolate
		"\u2067", # U+2067	RLI		Right-to-left isolate
		"\u2068", # U+2068	FSI		First strong isolate
		"\u2069", # U+2069	PDI		Pop directional isolate
	]

	print('UnicodeControlCharacters:')
	for ucc in ucc_table:
		utf8bytes = ucc.encode('utf-8')
		utf8str = ''.join('\\x%02x' % b for b in utf8bytes)
		print(utf8str, 'U+%04X' % ord(ucc), unicodedata.category(ucc), GetCharName(ucc))

def GenerateJsonCharClass():
	keywords = ["false", "null", "true", "Infinity", "NaN"]
	wordStart = [item[0] for item in keywords]
	operator = "{}[]:,+-"

	SCE_JSON_DEFAULT = 0
	SCE_JSON_OPERATOR = 1
	SCE_JSON_NUMBER = 2
	SCE_JSON_MAYBE_KEYWORD = 3
	SCE_JSON_IDENTIFIER = 4
	SCE_JSON_STRING = 5
	SCE_JSON_CHARACTER = 6

	JsonChar_None = 0
	JsonChar_BraceOpen = 1
	JsonChar_BraceClose = 2
	JsonChar_WordStart = 3
	JsonChar_Dot = 4
	JsonChar_Slash = 5
	JsonChar_Ignore = 6

	JsonMask_Number = 1 << 3
	JsonMask_Identifier = 1 << 4

	table = [0] * 128
	# https://www.ecma-international.org/ecma-262/#sec-ecmascript-language-lexical-grammar
	for i in range(0x21, 0x80):
		ch = chr(i)
		state = SCE_JSON_DEFAULT
		mask = 0
		charClass = JsonChar_Ignore
		if ch in operator:
			state = SCE_JSON_OPERATOR
			if ch in '{[':
				charClass = JsonChar_BraceOpen
			elif ch in '}]':
				charClass = JsonChar_BraceClose
			else:
				if ch in '+-':
					# SignedInteger in ExponentPart
					mask = JsonMask_Number
		elif ch == '\"':
			state = SCE_JSON_STRING
		elif ch == '\'':
			state = SCE_JSON_CHARACTER
		elif ch == '/':
			charClass = JsonChar_Slash
		elif ch == '.':
			state = SCE_JSON_OPERATOR
			mask = JsonMask_Number
			charClass = JsonChar_Dot
		elif ch.isdigit():
			state = SCE_JSON_NUMBER
			mask = JsonMask_Number | JsonMask_Identifier
		elif ch in wordStart:
			state = SCE_JSON_MAYBE_KEYWORD
			mask = JsonMask_Number | JsonMask_Identifier
			charClass = JsonChar_WordStart
		elif ch.isalpha() or ch == '_':
			state = SCE_JSON_IDENTIFIER
			mask = JsonMask_Number | JsonMask_Identifier
		elif ch in '$\\':
			# '\\': UnicodeEscapeSequence
			state = SCE_JSON_IDENTIFIER
			mask = JsonMask_Identifier
		else:
			charClass = JsonChar_None

		value = charClass | mask | (state << 5)
		table[i] = value

	nonAscii = (SCE_JSON_IDENTIFIER << 5) | JsonMask_Identifier | JsonChar_Ignore
	table.extend([nonAscii]*128)
	lines = dumpArray(table, 16)
	Regenerate("../lexers/LexJSON.cxx", "//", lines)

if __name__ == '__main__':
	#GenerateUTF8Table()
	GenerateUnicodeControlCharacters();
	GenerateJsonCharClass()
