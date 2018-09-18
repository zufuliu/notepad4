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

EDITLEXER lexJAM = { SCLEX_CPP, NP2LEX_JAM, EDITLEXER_HOLE(L"Jamfile"), L"jam", L"", &Keywords_JAM,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_CONSTANT, NP2STYLE_BuildinRule, L"Build-in Rule", L"fore:#0080FF", L""},
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENT, SCE_C_COMMENTLINE), NP2STYLE_Comment, L"Comment", L"fore:#608060", L"" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_VARIABLE2, NP2STYLE_Variable, L"Variable", L"fore:#CC3300", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	EDITSTYLE_SENTINEL
}
};
