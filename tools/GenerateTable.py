import string

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

def GenerateBase64Table():
	encoding = string.ascii_uppercase + string.ascii_lowercase + string.digits + '+/'
	print('base64 encoding:', encoding)
	decoding = [128]*128
	for index, ch in enumerate(encoding):
		decoding[ord(ch)] = index
	# url safe
	decoding[ord('-')] = encoding.index('+')
	decoding[ord('_')] = encoding.index('/')
	output = []
	for i in range(0, len(decoding), 16):
		line = ', '.join(f'{ch:3d}' for ch in decoding[i:i+16])
		output.append(line + ',')
	print('base64 decoding:')
	print('\n'.join(output))

if __name__ == '__main__':
	GenerateBraceMatchTable()
	GenerateDefaultWordCharSet()
	GenerateBase64Table()
