#!/usr/bin/env python3
# Script to generate CharacterCategory.cxx from Python's Unicode data
# Should be run rarely when a Python with a new version of Unicode data is available.
# Requires Python 3.3 or later
# Should not be run with old versions of Python.

import codecs, os, platform, sys, unicodedata

from FileGenerator import Regenerate

WordCategory = [
    'Lu', 'Ll', 'Lt', 'Lm', 'Lo', # Letter
    'Nd', 'Nl', 'No', # Number
    'Mn', 'Mc', 'Me', # Mark
]

# https://en.wikipedia.org/wiki/List_of_Unicode_characters
# https://en.wikipedia.org/wiki/CJK_Unified_Ideographs
# https://en.wikipedia.org/wiki/Katakana
# https://en.wikipedia.org/wiki/Hangul
CJKBlockList = [
    (0x4E00, 0x9FFF), # U+4E00-U+9FFF CJK Unified Ideographs
    (0x3400, 0x4DBF), # U+3400-U+4DBF CJK Unified Ideographs Extension A
    (0xF900, 0xFAFF), # U+F900-U+FAFF CJK Compatibility Ideographs
    # surrogate pair
    (0x20000, 0x2A6DF), # U+20000–U+2A6DF CJK Unified Ideographs Extension B
    (0x2A700, 0x2B73F), # U+2A700–U+2B73F CJK Unified Ideographs Extension C
    (0x2B740, 0x2B81F), # U+2B740–U+2B81F CJK Unified Ideographs Extension D
    (0x2B820, 0x2CEAF), # U+2B820–U+2CEAF CJK Unified Ideographs Extension E
    (0x2CEB0, 0x2EBEF), # U+2CEB0-U+2EBEF CJK Unified Ideographs Extension F
    (0x2F800, 0x2FA1F), # U+2F800-U+2FA1F CJK Compatibility Ideographs Supplement

    (0x2E80, 0x2EFF), # U+2E80-U+2EFF CJK Radicals Supplement
    (0x2F00, 0x2FDF), # U+2F00-U+2FDF Kangxi Radicals
    (0x3000, 0x303F), # U+3000-U+303F CJK Symbols and Punctuation
    #(0x3100, 0x312F), # U+3100-U+312F Bopomofo
    #(0x31A0, 0x31BF), # U+31A0-U+31BF Bopomofo Extended
    (0x31C0, 0x31EF), # U+31C0-U+31EF CJK Strokes
    (0x3200, 0x32FF), # U+3200-U+32FF Enclosed CJK Letters and Months
    (0x3300, 0x33FF), # U+3300-U+33FF CJK Compatibility
    (0xFE30, 0xFE4F), # U+FE30-U+FE4F CJK Compatibility Forms
    #(0xFF5F, 0xFFEF), # U+FF5F-U+FFEF Halfwidth and Fullwidth Forms (none ASCII)
    # Japanese
    (0x3040, 0x309F), # U+3040-U+309F Hiragana
    (0x30A0, 0x30FF), # U+30A0–U+30FF Katakana
    (0x3190, 0x319F), # U+3190-U+319F Kanbun
    (0x31F0, 0x31FF), # U+31F0–U+31FF Katakana Phonetic Extensions
    (0x1B000, 0x1B0FF), # U+1B000–U+1B0FF Kana Supplement
    (0x1B100, 0x1B12F), # U+1B100-U+1B12F Kana Extended-A
    # Korean
    (0xAC00, 0xD7AF), # U+AC00–U+D7AF Hangul Syllables
    (0x1100, 0x11FF), # U+1100–U+11FF Hangul Jamo
    (0x3130, 0x318F), # U+3130–U+318F Hangul Compatibility Jamo
    (0xA960, 0xA97F), # U+A960–U+A97F Hangul Jamo Extended-A
    (0xD7B0, 0xD7FF), # U+D7B0–U+D7FF Hangul Jamo Extended-B
    # Yi
    (0xA000, 0xA48F), # U+A000-U+A48F Yi Syllables
    (0xA490, 0xA4CF), # U+A490-U+A4CF Yi Radicals
]

def findCategories(filename):
    with codecs.open(filename, "r", "UTF-8") as infile:
        lines = [x.strip() for x in infile.readlines() if "\tcc" in x]
    values = "".join(lines).replace(" ","").split(",")
    print(values)
    return [v[2:] for v in values]

def isCJKLetter(category, uch):
    if category not in WordCategory:
        return False

    ch = ord(uch)
    for block in CJKBlockList:
        if ch >= block[0] and ch <= block[1]:
            return True

    name = ''
    try:
        name = unicodedata.name(uch).upper()
    except:
        pass
    return 'CJK' in name \
        or 'HIRAGANA' in name \
        or 'KATAKANA' in name \
        or 'HANGUL' in name \
        or 'KANA' in name \

def updateCharacterCategory(filename):
    values = ["// Created with Python %s,  Unicode %s" % (
        platform.python_version(), unicodedata.unidata_version)]
    category = unicodedata.category(chr(0))
    startRange = 0
    for ch in range(sys.maxunicode):
        uch = chr(ch)
        current = unicodedata.category(uch)
        if isCJKLetter(current, uch):
            current = 'CJK'
        if current != category:
            value = startRange * 32 + categories.index(category)
            values.append("%d," % value)
            category = current
            startRange = ch
    value = startRange * 32 + categories.index(category)
    values.append("%d," % value)

    Regenerate(filename, "//", values)

categories = findCategories("../lexlib/CharacterCategory.h")

updateCharacterCategory("../lexlib/CharacterCategory.cxx")
