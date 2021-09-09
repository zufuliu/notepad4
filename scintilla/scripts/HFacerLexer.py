#!/usr/bin/env python3
# HFacer.py - regenerate the SciLexer.h file from the SciLexer.iface interface
# definition file.
# Implemented 2000 by Neil Hodgson neilh@scintilla.org

import pathlib
import Face

import FileGenerator

def printLexHFile(f):
	# StylesCommon in Scintilla.iface
	STYLE_FIRSTPREDEFINED = 32
	STYLE_LASTPREDEFINED = 39

	out = []
	lex = set()
	autoValue = 0
	valueMap = {}
	for name in f.order:
		v = f.features[name]
		featureType = v["FeatureType"]
		if featureType == "lex":
			autoValue = 0
			valueMap.clear()
		elif featureType == "val":
			if "SCE_" in name or "SCLEX_" in name:
				value = v["Value"]
				if "SCLEX_" in name:
					val = int(value)
					if val in lex:
						raise Exception("Duplicate Lexer Value: %s = %d" % (name, val))
					else:
						lex.add(val)
				else:
					if value == 'auto':
						val = autoValue
						value = str(val)
					else:
						val = int(value)
					autoValue = val + 1
					if autoValue == STYLE_FIRSTPREDEFINED:
						autoValue = STYLE_LASTPREDEFINED + 1
					if val in valueMap:
						raise Exception("Duplicate Style Value: %s = %d, %s" % (name, val, valueMap[val]))
					valueMap[val] = name
					if val >= STYLE_FIRSTPREDEFINED and val <= STYLE_LASTPREDEFINED:
						raise Exception("Invalid Style Value: %s = %d" % (name, val))
				out.append("#define " + name + " " + value)
	return out

def RegenerateAll(root):
	f = Face.Face()
	f.ReadFromFile(root / "include/SciLexer.iface")
	FileGenerator.Regenerate(root / "include/SciLexer.h", "/* ", printLexHFile(f))

if __name__ == "__main__":
	RegenerateAll(pathlib.Path(__file__).resolve().parent.parent)
