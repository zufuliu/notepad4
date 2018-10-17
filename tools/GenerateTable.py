#!/usr/bin/env python3

def GenerateEOLTable():
	table = [0] * 256
	table[0] = 3
	table[ord('\n')] = 1
	table[ord('\r')] = 2
	lines = []
	for i in range(0, len(table), 32):
		line = ', '.join(str(c) for c in table[i:i + 32])
		line = line + ', // %02X - %02X' % (i, i + 31)
		lines.append(line)
	
	print('EOLTable:')
	print('\n'.join(lines))

if __name__ == '__main__':
	GenerateEOLTable()
