#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikipedia.org/wiki/Go_(programming_language)
// https://golang.org/

static KEYWORDLIST Keywords_Go = {{
"break case chan const continue default defer else fallthrough for func go goto "
"if import interface map package range return select struct switch type var "
"false iota nil true "
, // type keyword
"bool byte complex64 complex128 error float32 float64 int int8 int16 int32 int64 "
"rune string uint uintptr uint8 uint16 uint32 uint64 "
, // preprocessor
""
, // directive
""
, // attribute
"append cap close complex copy imag len make new panic print println real recover "
, // class
""
, // interface
""
, // enumeration
""
, // constant
""

#if NUMKEYWORD == 16
, "", "", "", "", "", "",
""
#endif
}};

EDITLEXER lexGo = { SCLEX_CPP, NP2LEX_GO, L"Go Source", L"go", L"", &Keywords_Go,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_ATTRIBUTE, NP2STYLE_BasicFunction, L"Basic Function", L"fore:#FF8000", L"" },
	{ SCE_C_FUNCTION, NP2STYLE_Function, L"Function", L"fore:#A46000", L"" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, L"Doc Comment Tag", L"bold; fore:#008000F", L"" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2STYLE_DocComment, L"Doc Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_DSTRINGB, NP2STYLE_BackTicks, L"Back Ticks", L"fore:#F08000", L"" },
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
