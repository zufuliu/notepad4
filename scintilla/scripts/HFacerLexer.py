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

	out = [
		'#define STYLE_LINK 33',
		'#define STYLE_COMMENT_LINK 34',
		'#define STYLE_STRING_LINK 35',
	]
	autoValue = 0
	valueMap = {}
	for name in f.order:
		v = f.features[name]
		featureType = v["FeatureType"]
		if featureType in ("lex", "enu"):
			autoValue = 0
			valueMap.clear()
		elif featureType == "val":
			if name.startswith("SCE_") or name.startswith("SCLEX_"):
				value = v["Value"]
				if value == "auto":
					val = autoValue
					value = str(val)
				else:
					val = int(value)
				autoValue = val + 1
				if val in valueMap:
					kind = "Style" if name.startswith("SCE_") else "Lexer"
					raise Exception(f"Duplicate {kind} Value: {name} = {val}, {valueMap[val]}")
				valueMap[val] = name
				if name.startswith("SCE_"):
					if autoValue == STYLE_FIRSTPREDEFINED:
						autoValue = STYLE_LASTPREDEFINED + 1
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
