#!/usr/bin/env python3
import sys
from enum import IntFlag

DBCSCodePages = [
	'cp932', 'shift_jis', 'shift_jis_2004', 'shift_jisx0213',
	'cp936', 'gbk',
	'cp949', 'uhc',
	'cp950', 'big5', 'big5hkscs',
	'cp1361', 'johab',
]

def to_byte_ranges(items):
	ranges = []
	if items:
		items = sorted(items)
		start = prev = items[0]
		for index in range(1, len(items)):
			value = items[index]
			if value - prev != 1:
				ranges.append((start, prev))
				start = value
			prev = value
		ranges.append((start, prev))
	return ranges

def format_byte_ranges(ranges):
	result = []
	for start, end in ranges:
		if start == end:
			result.append('[0x%02X]' % start)
		else:
			result.append('[0x%02X, 0x%02X]' % (start, end))
	return ', '.join(result)

class DBCSTailKind(IntFlag):
	Digit = 1
	Punctuation = 2
	Word = 4
	Control = 8
	All = 15
	NonWord = 3

	@staticmethod
	def get_kind(flags, ch_tail):
		if (flags & DBCSTailKind.Digit) and ch_tail.isdigit():
			return DBCSTailKind.Digit
		if (flags & DBCSTailKind.Punctuation) and not ch_tail.isalnum():
			return DBCSTailKind.Punctuation
		if (flags & DBCSTailKind.Word) and (ch_tail == '_' or ch_tail.isalpha()):
			return DBCSTailKind.Word
		if (flags & DBCSTailKind.Control) and not ch_tail.isprintable():
			return DBCSTailKind.Control
		return None

	@staticmethod
	def get_desc(flags):
		if flags.name:
			return flags.name
		comb = []
		for value in [DBCSTailKind.Digit, DBCSTailKind.Punctuation, DBCSTailKind.Word, DBCSTailKind.Control]:
			if flags & value:
				comb.append(value.name)
		return ' or '.join(comb)

def print_dbcs_char_by_tail(codePage, what):
	"""
	print DBCS character with specific type of tail byte.
	used to test whether lexer correctly handles DBCS characters or not.
	"""

	result = {}
	count = 0
	for lead in range(0x81, 0x100):
		for tail in range(0x21, 0x80):
			ch_tail = chr(tail)
			kind = DBCSTailKind.get_kind(what, ch_tail)
			if not kind:
				continue
			try:
				ch = bytes([lead, tail]).decode(codePage)
				if len(ch) != 1:
					continue
				count += 1
				key = (kind, tail, ch_tail)
				if key in result:
					result[key].append(ch)
				else:
					result[key] = [ch]
			except UnicodeDecodeError:
				pass

	description = DBCSTailKind.get_desc(what)
	if count == 0:
		print(f"no result for {description} in code page {codePage}")
		return

	sys.stdout.reconfigure(encoding='utf-8')
	print(f'{count} result in code page {codePage} for {description}')
	step = 20
	for key, items in sorted(result.items()):
		kind, tail, ch_tail = key
		count = len(items)
		print('\t', kind.name, '%02X' % tail, ch_tail, count)
		i = 0
		while i < count:
			print('\t\t', ' '.join(items[i:i + step]))
			i += step

def print_dbcs_test_char(what):
	for codePage in DBCSCodePages:
		print_dbcs_char_by_tail(codePage, what)

def print_dbcs_valid_bytes():
	for codePage in DBCSCodePages:
		validSingle = []
		validLead = set()
		validTail = set()

		for lead in range(0x80, 0x100):
			try:
				bytes([lead]).decode(codePage)
				validSingle.append(lead)
			except UnicodeDecodeError:
				pass

			for tail in range(0x21, 0x100):
				try:
					ch = bytes([lead, tail]).decode(codePage)
					if len(ch) == 1:
						validLead.add(lead)
						validTail.add(tail)
				except UnicodeDecodeError:
					pass

		validSingle = to_byte_ranges(validSingle)
		validLead = to_byte_ranges(validLead)
		validTail = to_byte_ranges(validTail)
		print(codePage)
		print('  single:', format_byte_ranges(validSingle))
		print('    lead:', format_byte_ranges(validLead))
		print('    tail:', format_byte_ranges(validTail))

#print_dbcs_test_char(DBCSTailKind.All)
print_dbcs_valid_bytes()
