import codecs
import ctypes

# see https://sourceforge.net/p/scintilla/feature-requests/1564/

MB_ERR_INVALID_CHARS = 0x00000008
MultiByteToWideChar = ctypes.windll.kernel32.MultiByteToWideChar
class PlatformDecoder:
	def __init__(self, cp):
		self.cp = cp
	def __call__(self, buf):
		size = len(buf)*4
		result = (ctypes.c_wchar*size)()
		length = MultiByteToWideChar(self.cp, MB_ERR_INVALID_CHARS, ctypes.c_char_p(buf), len(buf), result, size)
		if length == 0:
			code = ctypes.GetLastError()
			msg = ctypes.FormatError(code).strip()
			raise UnicodeDecodeError(f'{self.cp}', buf, 0, len(buf), f'{code}: {msg}')
		value = result.value[:length]
		value += '\0'*(length - len(value))
		return (value, len(buf))

def findCaseFoldBlock(encodingList):
	characterCount = 0xffff + 1
	foldSet = [0]*characterCount
	multiFold = []

	for cp in encodingList:
		if isinstance(cp, int):
			decode = PlatformDecoder(cp)
			cp = f'cp{cp}'
		else:
			decode = codecs.getdecoder(cp)
		for ch in range(0x80, characterCount):
			buf = bytes([ch >> 8, ch & 0xff])
			try:
				result = decode(buf)
			except UnicodeDecodeError:
				continue
			uch = result[0]
			if len(uch) == 1 and result[1] == len(buf):
				fold = uch.casefold()
				if uch != fold:
					foldSet[ch] = 1
				if len(fold) != 1:
					text = f'{ch:04X}'
					try:
						fold.encode(cp)
						multiFold.append((cp, text, uch, fold, len(fold), True))
					except UnicodeEncodeError:
						multiFold.append((cp, text, uch, fold, len(fold), False))

	foldCount = 0
	leadCount = {}
	for ch, value in enumerate(foldSet):
		if value:
			foldCount += 1
			lead = f'{(ch >> 8):02X}'
			if lead in leadCount:
				leadCount[lead] += 1
			else:
				leadCount[lead] = 1

	codePage = encodingList[0]
	indent = ' '*4
	lead = ', '.join(f'0x{ch}' for ch in leadCount.keys())
	print(f'{codePage} case fold total: {foldCount}')
	print(f'{indent}lead {len(leadCount)}:', lead)
	print(f'{indent}lead {len(leadCount)}:', leadCount)
	print(f'{indent}fold {len(multiFold)}:', multiFold)

findCaseFoldBlock(['cp932', 'shift_jis', 'shift_jis_2004', 'shift_jisx0213', 932])
findCaseFoldBlock(['cp936', 'gbk', 936])
findCaseFoldBlock(['cp949', 'uhc', 949])
findCaseFoldBlock(['cp950', 'big5', 'big5hkscs', 950])
findCaseFoldBlock(['cp1361', 'johab', 1361])
