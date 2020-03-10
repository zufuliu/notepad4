#include "EditLexer.h"
#include "EditStyleX.h"

// YAML 1.2 https://yaml.org/

static KEYWORDLIST Keywords_YAML = {{
".inf .nan "
"Inf NaN None "
"false inf nan no none null off on true yes "

, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_YAML[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_YAML_KEYWORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_YAML_DIRECTIVE, NP2StyleX_Directive, L"fore:#FF8000" },
	{ SCE_YAML_DOCUMENT, NP2StyleX_Document, L"bold; back:#FFC040; eolfilled" },
	{ SCE_YAML_KEY, NP2StyleX_Property, L"fore:#A46000" },
	{ SCE_YAML_ANCHOR, NP2StyleX_Anchor, L"fore:#9E4D2A" },
	{ SCE_YAML_ALIAS, NP2StyleX_Alias, L"fore:#9E4D2A" },
	{ MULTI_STYLE(SCE_YAML_TAG, SCE_YAML_VERBATIM_TAG, 0, 0), NP2StyleX_Tag, L"fore:#7C5AF3" },
	{ SCE_YAML_COMMENT, NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_YAML_STRING1, SCE_YAML_STRING2, 0, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_YAML_ESCAPECHAR, NP2StyleX_EscapeSequence, L"fore:#0080C0" },
	{ MULTI_STYLE(SCE_YAML_BLOCK_SCALAR, SCE_YAML_INDENTED_TEXT, 0, 0), NP2StyleX_TextBlock, L"fore:#F08000" },
	{ SCE_YAML_DATETIME, NP2StyleX_DateTime, L"fore:#008080" },
	{ SCE_YAML_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_YAML_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexYAML = {
	SCLEX_YAML, NP2LEX_YAML,
	EDITLEXER_HOLE(L"YAML Document", Styles_YAML),
	L"yml; yaml; clang-format; clang-tidy; mir; apinotes; ifs",
	&Keywords_YAML,
	Styles_YAML
};
