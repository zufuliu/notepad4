#!/usr/bin/env python3
# HFacer.py - regenerate the SciLexer.h file from the SciLexer.iface interface
# definition file.
# Implemented 2000 by Neil Hodgson neilh@scintilla.org

import Face

from FileGenerator import Regenerate

def printLexHFile(f):
	# StylesCommon in Scintilla.iface
	STYLE_DEFAULT = 32
	STYLE_LASTPREDEFINED = 39

	out = []
	lex = set()
	for name in f.order:
		v = f.features[name]
		if v["FeatureType"] in ["val"]:
			if "SCE_" in name or "SCLEX_" in name:
				val = int(v["Value"].strip('()'))
				if "SCLEX_" in name:
					if val in lex:
						raise Exception("Duplicate Lexer Value: %s = %d" % (name, val))
					else:
						lex.add(val)
				else:
					if val >= STYLE_DEFAULT and val <= STYLE_LASTPREDEFINED:
						raise Exception("Invalid Style Value: %s = %d" % (name, val))
				out.append("#define " + name + " " + v["Value"])
	return out

def RegenerateAll(root):
	f = Face.Face()
	f.ReadFromFile(root + "include/SciLexer.iface")
	Regenerate(root + "include/SciLexer.h", "/* ", printLexHFile(f))

if __name__ == "__main__":
	RegenerateAll("../")
