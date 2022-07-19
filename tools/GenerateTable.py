def GenerateBraceMatchTable():
	# used in IsBraceMatchChar()
	table = [0] * 8
	for ch in "()[]{}<>":
		c = ord(ch)
		# table[c // 32] |= (1 << (c % 32))
		table[c >> 5] |= (1 << (c & 31))

	line = ', '.join(hex(c) for c in table)
	print('BraceMatch:', line)

def GenerateDefaultWordCharSet():
	table = [0] * 8
	for c in range(256):
		ch = chr(c)
		if c >= 0x80 or ch == '_' or ch.isalnum():
			# table[c // 32] |= (1 << (c % 32))
			table[c >> 5] |= (1 << (c & 31))

	lines = [', '.join(f'0x{c:08x}U' for c in table[:4])]
	lines.append(', '.join(f'0x{c:08x}U' for c in table[4:]))
	print('DefaultWordCharSet:')
	print('\n'.join(lines))

if __name__ == '__main__':
	GenerateBraceMatchTable()
	GenerateDefaultWordCharSet()
