import sys
import codecs
sys.path.append('../scripts')
from UnicodeData import *

# see https://sourceforge.net/p/scintilla/feature-requests/1564/

def findCaseFoldBlock(encodingList):
	foldSet = [0]*DBCSCharacterCount
	multiFold = []

	for codePage in encodingList:
		if isinstance(codePage, int):
			decode = PlatformDecoder(codePage)
			codePage = f'cp{codePage}'
		else:
			decode = codecs.getdecoder(codePage)
		for ch in range(DBCSMinCharacter, DBCSCharacterCount):
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
						fold.encode(codePage)
						multiFold.append((codePage, text, uch, fold, len(fold), True))
					except UnicodeEncodeError:
						multiFold.append((codePage, text, uch, fold, len(fold), False))

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
