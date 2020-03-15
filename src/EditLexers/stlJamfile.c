#include "EditLexer.h"
#include "EditStyleX.h"

// https://boostorg.github.io/build/manual/develop/index.html

static KEYWORDLIST Keywords_JAM = {{
"class else for if import in local module return rule while "
"switch case actions true false try catch include alias using "
, // 1 Type Keyword
NULL
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
"ECHO EXIT GLOB MATCH "

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_JAM[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_CONSTANT, NP2StyleX_BuiltInRule, L"fore:#0080FF" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENT, SCE_C_COMMENTLINE), NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_VARIABLE2, NP2StyleX_Variable, L"fore:#9E4D2A" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexJAM = {
	SCLEX_CPP, NP2LEX_JAM,
	EDITLEXER_HOLE(L"Jamfile", Styles_JAM),
	L"jam",
	&Keywords_JAM,
	Styles_JAM
};
