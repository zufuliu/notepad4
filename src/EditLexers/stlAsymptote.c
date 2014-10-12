#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_ASY = {
"private public import include from access as if else false true default restricted struct "
"return for new explicit do while break operator this static typedef var continue unravel "
"controls and cycle tension void null object"
, // type keywor
"bool int real string "
"pair triple pen path frame guide transform file "

"bool3 picture projection surface path3 guide3 transform3 "
"revolution animation "

, // preprocesso
""
, // directiv
""
, // attribute
""
, // clas
""
, // interfac
""
, // enumeratio
""
, // constan
""

#if NUMKEYWORD == 1
, "","","","","","",
"for^() if^() while^() else^if^() "
#endif
};

EDITLEXER lexASY = { SCLEX_CPP, NP2LEX_ASY, L"Asymptote Code", L"asy", L"", &Keywords_ASY,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#1E90FF", L"" },
	{ SCE_C_STRUCT, NP2STYLE_Struct, L"Struct", L"bold; fore:#007F7F", L"" },
	{ SCE_C_FUNCTION, NP2STYLE_Function, L"Function", L"fore:#A46000", L"" },
	{ MULTI_STYLE(SCE_C_COMMENT,SCE_C_COMMENTLINE,SCE_C_COMMENTDOC,SCE_C_COMMENTLINEDOC), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_C_STRING,SCE_C_CHARACTER,SCE_C_STRINGEOL,0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L""}
}
};
