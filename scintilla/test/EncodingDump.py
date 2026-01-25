import sys
import codecs
import unicodedata
sys.path.append('../scripts')
from UnicodeData import *

SBCSCodePages = [1250, 1251, 1252, 1253, 1254, 1255, 1256, 1257, 1258, 874]
DBCSCodePages = [932, 936, 949, 950, 1361]

def diffCodePage(codePage, dbcs=False):
	decode_pd = PlatformDecoder(codePage)
	decode_py = codecs.getdecoder(f'cp{codePage}')
	diff_pd = []
	diff_py = []
	range_pd = []
	range_py = []

	def add_range(target, ch):
		if target and ch - target[-1][1] == 1:
			target[-1][1] = ch
		else:
			target.append([ch, ch])
	def format_range(target, source):
		for first, second in source:
			lead, trail = first >> 8, first & 0xff
			if first == second:
				target.append(f'{lead:02X}{trail:02X}')
			else:
				target.append(f'{lead:02X}{trail:02X}-{(second & 0xff):02X}')

	count = DBCSCharacterCount if dbcs else 256
	for ch in range(0x80, count):
		if 0xff < ch < DBCSMinCharacter:
			continue
		buf = bytes([ch]) if ch < 256 else bytes([ch >> 8, ch & 0xff])
		mask = 0
		try:
			result = decode_pd(buf)
			if len(result[0]) == 1:
				mask |= 1
		except UnicodeDecodeError:
			pass
		try:
			result = decode_py(buf)
			if len(result[0]) == 1:
				mask |= 2
		except UnicodeDecodeError:
			pass

		if mask in (0, 3):
			continue
		if ch < 256:
			value = f'{ch:02X}'
			if mask == 1:
				diff_pd.append(value)
			else:
				diff_py.append(value)
		elif mask == 1:
			add_range(range_pd, ch)
		else:
			add_range(range_py, ch)

	format_range(diff_pd, range_pd)
	format_range(diff_py, range_py)
	print(f'cp{codePage} {len(diff_py)}: {" ".join(diff_py)}')
	print(f'  {codePage} {len(diff_pd)}: {" ".join(diff_pd)}')

def diffAllCodePage():
	for codePage in SBCSCodePages:
		diffCodePage(codePage)
		print('')
	for codePage in DBCSCodePages:
		diffCodePage(codePage, True)
		print('')

def dumpHighByte(codePage):
	bestfit = PlatformDecoder(codePage, 0, errors='ignore')
	decode = PlatformDecoder(codePage, errors='ignore')
	undefined = 0
	control = 0
	diff = 0
	for ch in range(0x80, 256):
		buf = bytes([ch])
		try:
			result = bestfit(buf)
			first = result[1], ord(result[0][0])
		except UnicodeDecodeError:
			first = (0, 0)
		try:
			result = decode(buf)
			second = result[1], ord(result[0][0])
		except UnicodeDecodeError:
			second = (0, 0)

		status = ''
		if second[0] == 0:
			undefined += 1
			status = 'undefined'
		if first[1] < 0x9F:
			control += 1
		if first[1] != second[1]:
			diff += 1
		category = unicodedata.category(chr(first[1]))
		print(f'code page {codePage} {ch:02X}: {first[0]}/{second[0]} {first[1]:04X} {second[1]:04X} {category} {status}')
	print(f'code page {codePage} total undefined: {undefined} diff: {diff} control: {control}\n')

def dumpDualByte(codePage, lead):
	bestfit = PlatformDecoder(codePage, 0, errors='ignore')
	decode = PlatformDecoder(codePage, errors='ignore')
	undefined = 0
	control = 0
	diff = 0
	for trail in range(DBCSMinTrailByte, 0xff):
		buf = bytes([lead, trail])
		try:
			result = bestfit(buf)
			first = result[1], ord(result[0][0])
		except UnicodeDecodeError:
			first = (0, 0)
		try:
			result = decode(buf)
			second = result[1], ord(result[0][0])
		except UnicodeDecodeError:
			second = (0, 0)

		status = ''
		if second[0] == 0:
			undefined += 1
			status = 'undefined'
		if first[1] < 0x9F:
			control += 1
		if first[1] != second[1]:
			diff += 1
		category = unicodedata.category(chr(first[1]))
		print(f'code page {codePage} {lead:02X}{trail:02X}: {first[0]}/{second[0]} {first[1]:04X} {second[1]:04X} {category} {status}')
	print(f'code page {codePage} total undefined: {undefined} diff: {diff} control: {control}\n')

def dumpSBCSHighByte():
	for codePage in SBCSCodePages:
		dumpHighByte(codePage)

def dumpDBCSHighByte():
	for codePage in DBCSCodePages:
		dumpHighByte(codePage)

def testSBCSWholeDecode():
	for codePage in SBCSCodePages:
		bestfit = PlatformDecoder(codePage, 0, errors='ignore')
		decode = PlatformDecoder(codePage, errors='ignore')
		buf = bytes(range(0x80, 256))
		whole = bestfit(buf)[0]
		control = 0
		diff = 0
		for index, first in enumerate(whole):
			ch = buf[index]
			second = decode(bytes([ch]))[0]
			if ord(first) <= 0x9F:
				control += 1
			if first != second:
				diff += 1
				print(f'code page {codePage} {ch:02X} diff: {ord(first):04X} {ord(second):04X}')
		print(f'code page {codePage} total diff: {diff} control: {control}')

# diffAllCodePage()
# dumpSBCSHighByte()
# dumpDBCSHighByte()
# dumpDualByte(936, 0xA1)
# testSBCSWholeDecode()
