#!/usr/bin/env python3
import sys
from enum import IntFlag

class DBCSTailKind(IntFlag):
	Digit = 1
	Punctuation = 2
	Word = 4
	All = 7
	NonWord = 3

	@staticmethod
	def get_kind(flags, ch_tail):
		if (flags & DBCSTailKind.Digit) and ch_tail.isdigit():
			return DBCSTailKind.Digit
		if (flags & DBCSTailKind.Punctuation) and not ch_tail.isalnum():
			return DBCSTailKind.Punctuation
		if (flags & DBCSTailKind.Word) and (ch_tail == '_' or ch_tail.isalpha()):
			return DBCSTailKind.Word
		return None

	@staticmethod
	def get_desc(flags):
		if flags.name:
			return flags.name
		comb = []
		for value in [DBCSTailKind.Digit, DBCSTailKind.Punctuation, DBCSTailKind.Word]:
			if flags & value:
				comb.append(value.name)
		return ' or '.join(comb)

def print_dbcs_char_by_tail(code_page, what):
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
				ch = bytes([lead, tail]).decode(code_page)
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
		print(f"no result for {description} in code page {code_page}")
		return

	sys.stdout.reconfigure(encoding='utf-8')
	print(f'{count} result in code page {code_page} for {description}')
	step = 20
	for key, items in sorted(result.items()):
		kind, tail, ch_tail = key
		count = len(items)
		print('\t', kind.name, '%02X' % tail, ch_tail, count)
		i = 0
		while i < count:
			print('\t\t', ' '.join(items[i:i + step]))
			i += step

def print_dbcs_test_char():
	print_dbcs_char_by_tail('GBK', DBCSTailKind.Punctuation)
	print_dbcs_char_by_tail('Shift-JIS', DBCSTailKind.Punctuation)
	print_dbcs_char_by_tail('UHC', DBCSTailKind.Punctuation)
	print_dbcs_char_by_tail('Johab', DBCSTailKind.Punctuation)

print_dbcs_test_char()
