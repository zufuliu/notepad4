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

	htmlStyle = """
#define SCE_STYLE_OFFSET_JS		(SCE_HB_OPERATOR + 1)
#define SCE_STYLE_OFFSET_CSS	(SCE_STYLE_OFFSET_JS + SCE_JS_LABEL + 1)
#define SCE_STYLE_OFFSET_PHP	(SCE_STYLE_OFFSET_CSS + SCE_CSS_PLACEHOLDER + 1)
#define js_style(style)		((style) + SCE_STYLE_OFFSET_JS)
#define css_style(style)	((style) + SCE_STYLE_OFFSET_CSS)
#define php_style(style)	((style) + SCE_STYLE_OFFSET_PHP)
""".strip().splitlines()
	output = [
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
				if val in valueMap:
					kind = "Style" if name.startswith("SCE_") else "Lexer"
					raise ValueError(f"Duplicate {kind} Value: {name} = {val}, {valueMap[val]}")
				autoValue = val + 1
				valueMap[val] = name
				if name.startswith('SCE_PHP_'):
					if htmlStyle:
						output.extend(htmlStyle)
						htmlStyle = None
					value = f'php_style({value})'
				elif name.startswith("SCE_"):
					if autoValue == STYLE_FIRSTPREDEFINED:
						autoValue = STYLE_LASTPREDEFINED + 1
					if STYLE_FIRSTPREDEFINED <= val <= STYLE_LASTPREDEFINED:
						raise ValueError(f"Invalid Style Value: {name} = {val}")
				output.append(f"#define {name} {value}")
	return output

def RegenerateAll(root):
	f = Face.Face()
	f.ReadFromFile(root / "include/SciLexer.iface")
	FileGenerator.Regenerate(root / "include/SciLexer.h", "/* ", printLexHFile(f))

if __name__ == "__main__":
	RegenerateAll(pathlib.Path(__file__).resolve().parent.parent)
