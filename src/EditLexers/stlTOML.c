#include "EditLexer.h"
#include "EditStyle.h"

// TOML 0.5 https://github.com/toml-lang/toml

static KEYWORDLIST Keywords_TOML = {{
"false inf nan true "

, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_TOML[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_TOML_KEYWORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_TOML_TABLE, NP2STYLE_Table, EDITSTYLE_HOLE(L"Table"), L"bold; back:#FFC040; eolfilled" },
	{ SCE_TOML_KEY, NP2STYLE_Key, EDITSTYLE_HOLE(L"Key"), L"fore:#A46000" },
	{ SCE_TOML_COMMENT, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ MULTI_STYLE(SCE_TOML_STRING1, SCE_TOML_STRING2, 0, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ MULTI_STYLE(SCE_TOML_TRIPLE_STRING1, SCE_TOML_TRIPLE_STRING2, 0, 0), NP2STYLE_TripleQuotedString, EDITSTYLE_HOLE(L"Triple Quoted String"), L"fore:#F08000" },
	{ SCE_TOML_ESCAPECHAR, NP2STYLE_EscapeSequence, EDITSTYLE_HOLE(L"Escape Sequence"), L"fore:#0080C0" },
	{ SCE_TOML_DATETIME, NP2STYLE_DateTime, EDITSTYLE_HOLE(L"DateTime"), L"fore:#008080" },
	{ SCE_TOML_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_TOML_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
};

EDITLEXER lexTOML = {
	SCLEX_TOML, NP2LEX_TOML,
	EDITLEXER_HOLE(L"TOML File", Styles_TOML),
	L"toml",
	&Keywords_TOML,
	Styles_TOML
};
