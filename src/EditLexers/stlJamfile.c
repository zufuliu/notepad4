#include "EditLexer.h"
#include "EditStyle.h"

// http://www.boost.org/build/doc/html/bbv2/jam.html

static KEYWORDLIST Keywords_JAM = {{
"class else for if import in local module return rule while "
"switch case actions true false try catch include alias using "
, // 1 Type Keyword
""
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
"ECHO EXIT GLOB MATCH "

, "", "", "", "", "", "", ""
}};

GCC_NO_WARNING_MISSING_BRACES_BEGIN

EDITLEXER lexJAM = { SCLEX_CPP, NP2LEX_JAM, EDITLEXER_HOLE(L"Jamfile"), L"jam", &Keywords_JAM,
{
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_C_CONSTANT, NP2STYLE_BuildinRule, EDITSTYLE_HOLE(L"Build-in Rule"), L"fore:#0080FF" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENT, SCE_C_COMMENTLINE), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_VARIABLE2, NP2STYLE_Variable, EDITSTYLE_HOLE(L"Variable"), L"fore:#CC3300" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	EDITSTYLE_SENTINEL
}
};

GCC_NO_WARNING_MISSING_BRACES_END
