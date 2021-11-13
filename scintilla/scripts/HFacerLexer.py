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
						raise Exception(f"Duplicate Lexer Value: {name} = {val}")
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
						raise Exception(f"Duplicate Style Value: {name} = {val}, {valueMap[val]}")
					valueMap[val] = name
					if val >= STYLE_FIRSTPREDEFINED and val <= STYLE_LASTPREDEFINED:
						raise Exception(f"Invalid Style Value: {name} = {val}")
				out.append(f"#define {name} {value}")
	return out

def RegenerateAll(root):
	f = Face.Face()
	f.ReadFromFile(root / "include/SciLexer.iface")
	FileGenerator.Regenerate(root / "include/SciLexer.h", "/* ", printLexHFile(f))

if __name__ == "__main__":
	RegenerateAll(pathlib.Path(__file__).resolve().parent.parent)
