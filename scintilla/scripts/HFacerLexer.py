#!/usr/bin/env python3
# HFacer.py - regenerate the SciLexer.h file from the SciLexer.iface interface
# definition file.
# Implemented 2000 by Neil Hodgson neilh@scintilla.org
# Requires Python 2.5 or later

import sys
import os
import Face

from FileGenerator import UpdateFile, Generate, Regenerate, UpdateLineInFile, lineEnd

def printLexHFile(f):
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
					if val > 31 and val < 40:
						raise Exception("Invalid Style Value: %s = %d" % (name, val))
				out.append("#define " + name + " " + v["Value"])
	return out

def RegenerateAll(root):
	f = Face.Face()
	f.ReadFromFile(root + "include/SciLexer.iface")
	Regenerate(root + "include/SciLexer.h", "/* ", printLexHFile(f))

if __name__ == "__main__":
	RegenerateAll("../")
