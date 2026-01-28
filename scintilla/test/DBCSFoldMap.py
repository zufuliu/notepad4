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
			encode = PlatformEncoder(codePage)
		else:
			decode = codecs.getdecoder(codePage)
			encode = codecs.getencoder(codePage)
		for ch in range(DBCSMinCharacter, DBCSCharacterCount):
			buf = bytes([ch >> 8, ch & 0xff])
			try:
				result = decode(buf)
			except UnicodeDecodeError:
				continue
			uch = result[0]
			if len(uch) == 1 and result[1] == len(buf):
				fold = uch.casefold()
				if uch == fold:
					continue
				back = False
				try:
					result = encode(fold)
					buf = result[0]
					if len(buf) == 2 and result[1] == len(fold) and buf[0] >= DBCSMinLeadByte and buf[1] >= DBCSMinTrailByte:
						foldSet[ch] = 1
						back = True
				except UnicodeEncodeError:
					pass
				if len(fold) > 1:
					text = f'{ch:04X}'
					multiFold.append((codePage, text, uch, fold, len(fold), back))

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

findCaseFoldBlock([932, 'cp932', 'shift_jis', 'shift_jis_2004', 'shift_jisx0213'])
findCaseFoldBlock([936, 'cp936', 'gbk'])
findCaseFoldBlock([949, 'cp949', 'uhc'])
findCaseFoldBlock([950, 'cp950', 'big5', 'big5hkscs'])
findCaseFoldBlock([1361, 'cp1361', 'johab'])
