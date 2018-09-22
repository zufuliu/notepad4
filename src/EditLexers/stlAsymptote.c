#include "EditLexer.h"
#include "EditStyle.h"

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
""
, // 3 Directive
""
, // 4 Attribute
""
, // 5 Class
""
, // 6 Interface
""
, // 7 Enumeration
""
, // 8 Constant
""

, "", "", "", "", "", ""

, // 15 Code Snippet
"for^() if^() while^() else^if^() else^{} "
}};

GCC_NO_WARNING_MISSING_BRACES_BEGIN

EDITLEXER lexASY = { SCLEX_CPP, NP2LEX_ASY, EDITLEXER_HOLE(L"Asymptote Code"), L"asy", &Keywords_ASY,
{
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, EDITSTYLE_HOLE(L"Type Keyword"), L"fore:#1E90FF" },
	{ SCE_C_STRUCT, NP2STYLE_Struct, EDITSTYLE_HOLE(L"Struct"), L"bold; fore:#007F7F" },
	{ SCE_C_FUNCTION, NP2STYLE_Function, EDITSTYLE_HOLE(L"Function"), L"fore:#A46000" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	EDITSTYLE_SENTINEL
}
};

GCC_NO_WARNING_MISSING_BRACES_END
