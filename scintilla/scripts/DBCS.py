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

class DBCSTrailKind(IntFlag):
	Digit = 1
	Punctuation = 2
	Word = 4
	Control = 8
	All = 15
	NonWord = 3

	@staticmethod
	def get_kind(flags, ch_trail):
		if (flags & DBCSTrailKind.Digit) and ch_trail.isdigit():
			return DBCSTrailKind.Digit
		if (flags & DBCSTrailKind.Punctuation) and not ch_trail.isalnum():
			return DBCSTrailKind.Punctuation
		if (flags & DBCSTrailKind.Word) and (ch_trail == '_' or ch_trail.isalpha()):
			return DBCSTrailKind.Word
		if (flags & DBCSTrailKind.Control) and not ch_trail.isprintable():
			return DBCSTrailKind.Control
		return None

	@staticmethod
	def get_desc(flags):
		if flags.name:
			return flags.name
		comb = []
		for value in [DBCSTrailKind.Digit, DBCSTrailKind.Punctuation, DBCSTrailKind.Word, DBCSTrailKind.Control]:
			if flags & value:
				comb.append(value.name)
		return ' or '.join(comb)

def print_dbcs_char_by_trail(codePage, what):
	"""
	print DBCS character with specific type of trail byte.
	used to test whether lexer correctly handles DBCS characters or not.
	"""

	result = {}
	count = 0
	for lead in range(0x81, 0x100):
		for trail in range(0x21, 0x80):
			ch_trail = chr(trail)
			kind = DBCSTrailKind.get_kind(what, ch_trail)
			if not kind:
				continue
			try:
				ch = bytes([lead, trail]).decode(codePage)
				if len(ch) != 1:
					continue
				count += 1
				key = (kind, trail, ch_trail)
				if key in result:
					result[key].append(ch)
				else:
					result[key] = [ch]
			except UnicodeDecodeError:
				pass

	description = DBCSTrailKind.get_desc(what)
	if count == 0:
		print(f"no result for {description} in code page {codePage}")
		return

	sys.stdout.reconfigure(encoding='utf-8')
	print(f'{count} result in code page {codePage} for {description}')
	step = 20
	for key, items in sorted(result.items()):
		kind, trail, ch_trail = key
		count = len(items)
		print('\t', kind.name, '%02X' % trail, ch_trail, count)
		i = 0
		while i < count:
			print('\t\t', ' '.join(items[i:i + step]))
			i += step

def print_dbcs_test_char(what):
	for codePage in DBCSCodePages:
		print_dbcs_char_by_trail(codePage, what)

def print_dbcs_valid_bytes():
	for codePage in DBCSCodePages:
		validSingle = []
		validLead = set()
		validTrail = set()

		for lead in range(0x80, 0x100):
			try:
				bytes([lead]).decode(codePage)
				validSingle.append(lead)
			except UnicodeDecodeError:
				pass

			for trail in range(0x21, 0x100):
				try:
					ch = bytes([lead, trail]).decode(codePage)
					if len(ch) == 1:
						validLead.add(lead)
						validTrail.add(trail)
				except UnicodeDecodeError:
					pass

		validSingle = to_byte_ranges(validSingle)
		validLead = to_byte_ranges(validLead)
		validTrail = to_byte_ranges(validTrail)
		print(codePage)
		print('  single:', format_byte_ranges(validSingle))
		print('    lead:', format_byte_ranges(validLead))
		print('   trail:', format_byte_ranges(validTrail))

#print_dbcs_test_char(DBCSTrailKind.All)
print_dbcs_valid_bytes()
