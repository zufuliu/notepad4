#include "EditLexer.h"
#include "EditStyle.h"

// https://www.json.org

static KEYWORDLIST Keywords_JSON = {{
"true false null"
, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};


EDITLEXER lexJSON = { SCLEX_JSON, NP2LEX_JSON, EDITLEXER_HOLE(L"JSON Document"), L"json; har; ipynb; wxcp", &Keywords_JSON,
{
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_C_LABEL, NP2STYLE_JSONKey, EDITSTYLE_HOLE(L"Key"), L"fore:#A46000" },
	{ SCE_C_STRING, NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	EDITSTYLE_SENTINEL
}
};

