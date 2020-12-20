#include "EditLexer.h"
#include "EditStyleX.h"

// TOML 0.5 https://github.com/toml-lang/toml

static KEYWORDLIST Keywords_TOML = {{
"false inf nan true "

, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_TOML[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_TOML_KEYWORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_TOML_TABLE, NP2StyleX_Table, L"bold; back:#FFC040; eolfilled" },
	{ SCE_TOML_KEY, NP2StyleX_Key, L"fore:#A46000" },
	{ SCE_TOML_COMMENT, NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_TOML_STRING_SQ, SCE_TOML_STRING_DQ, 0, 0), NP2StyleX_String, L"fore:#008000" },
	{ MULTI_STYLE(SCE_TOML_TRIPLE_STRING_SQ, SCE_TOML_TRIPLE_STRING_DQ, 0, 0), NP2StyleX_TripleQuotedString, L"fore:#F08000" },
	{ SCE_TOML_ESCAPECHAR, NP2StyleX_EscapeSequence, L"fore:#0080C0" },
	{ SCE_TOML_DATETIME, NP2StyleX_DateTime, L"fore:#008080" },
	{ SCE_TOML_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_TOML_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexTOML = {
	SCLEX_TOML, NP2LEX_TOML,
	EDITLEXER_HOLE(L"TOML File", Styles_TOML),
	L"toml",
	&Keywords_TOML,
	Styles_TOML
};
