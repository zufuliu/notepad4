import sys
import unicodedata

MaxCharacter = sys.maxunicode
UnicodeCharacterCount = sys.maxunicode + 1
BMPCharacterCharacterCount = 0xffff + 1
DBCSCharacterCount = 0xffff + 1

def getCharacterName(ch):
	try:
		return unicodedata.name(ch).title()
	except ValueError:
		return ''
