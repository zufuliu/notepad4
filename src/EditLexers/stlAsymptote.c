#include "EditLexer.h"
#include "EditStyleX.h"

// http://asymptote.sourceforge.net/

static KEYWORDLIST Keywords_ASY = {{
"private public import include from access as if else false true default restricted struct "
"return for new explicit do while break operator this static typedef var continue unravel "
"controls and cycle tension void null object"

, // 1 Type Keyword
"bool int real string "
"pair triple pen path frame guide transform file "

"bool3 picture projection surface path3 guide3 transform3 "
"revolution animation "

, // 2 Preprocessor
NULL
, // 3 Directive
NULL
, // 4 Attribute
NULL
, // 5 Class
NULL
, // 6 Interface
NULL
, // 7 Enumeration
NULL
, // 8 Constant
NULL

, NULL, NULL, NULL, NULL, NULL, NULL

, // 15 Code Snippet
"for^() if^() while^() else^if^() else^{} "
}};

static EDITSTYLE Styles_ASY[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2StyleX_TypeKeyword, L"fore:#1E90FF" },
	{ SCE_C_STRUCT, NP2StyleX_Struct, L"bold; fore:#007F7F" },
	{ SCE_C_FUNCTION, NP2StyleX_Function, L"fore:#A46000" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC), NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexASY = {
	SCLEX_CPP, NP2LEX_ASY,
	EDITLEXER_HOLE(L"Asymptote Code", Styles_ASY),
	L"asy",
	&Keywords_ASY,
	Styles_ASY
};
