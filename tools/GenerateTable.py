#!/usr/bin/env python3

def GenerateEOLTable():
	# used in EditDetectEOLMode() and CellBuffer::BasicInsertString()
	table = [0] * 16
	table[ord('\n')] = 1
	table[ord('\r')] = 2
	line = ', '.join(str(c) for c in table)
	line = line + ', // %02X - %02X' % (0, 15)
	print('EOLTable:', line)

def GenerateC0ControlCharacterMask(excludeSeparator):
	# used in Style_MaybeBinaryFile()
	bits = ['1'] * 32
	# whitespace
	bits[9]  = '0' # 09 '\t'
	bits[10] = '0' # 0A '\n'
	bits[11] = '0' # 0B '\v'
	bits[12] = '0' # 0C '\f'
	bits[13] = '0' # 0D '\r'
	# separator
	if excludeSeparator:
		bits[28] = '0' # 1C File Separator
		bits[29] = '0' # 1D Group Separator
		bits[30] = '0' # 1E Record Separator
		bits[31] = '0' # 1F Unit Separator

	bits = ''.join(reversed(bits))
	value = int(bits, 2)
	s = hex(value)[2:].upper()
	if len(s) < 8:
		s = '0' * (8 - len(s)) + s
	s = '0x' + s + 'U'
	print('C0 Control Character:', s, bin(value))

if __name__ == '__main__':
	GenerateEOLTable()
	GenerateC0ControlCharacterMask(True)
