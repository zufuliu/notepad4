#include "EditLexer.h"
#include "EditStyle.h"

// http://www.json.org

static KEYWORDLIST Keywords_JSON = {{
"true false null"
, "", "", "", "", "", "", "",

"", "", "", "", "", "", "", ""
}};

EDITLEXER lexJSON = { SCLEX_JSON, NP2LEX_JSON, EDITLEXER_HOLE, L"JSON Document", L"json; har; ipynb", L"", &Keywords_JSON,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_LABEL, NP2STYLE_JSONKey, L"Key", L"fore:#A46000", L"" },
	{ SCE_C_STRING, NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#608060", L"" },
	EDITSTYLE_SENTINEL
}
};
