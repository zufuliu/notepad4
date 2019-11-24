#include "EditLexer.h"
#include "EditStyle.h"

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
	{ SCE_YAML_KEYWORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_YAML_DIRECTIVE, NP2STYLE_Directive, EDITSTYLE_HOLE(L"Directive"), L"fore:#FF8000" },
	{ SCE_YAML_DOCUMENT, NP2STYLE_Document, EDITSTYLE_HOLE(L"Document"), L"bold; back:#FFC040; eolfilled" },
	{ SCE_YAML_KEY, NP2STYLE_Property, EDITSTYLE_HOLE(L"Property"), L"fore:#A46000" },
	{ SCE_YAML_REFERENCE, NP2STYLE_Reference, EDITSTYLE_HOLE(L"Reference"), L"fore:#9E4D2A" },
	{  MULTI_STYLE(SCE_YAML_TAG, SCE_YAML_VERBATIM_TAG, 0, 0), NP2STYLE_Tag, EDITSTYLE_HOLE(L"Tag"), L"fore:#7C5AF3" },
	{ SCE_YAML_COMMENT, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ MULTI_STYLE(SCE_YAML_STRING1, SCE_YAML_STRING2, 0, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_YAML_ESCAPECHAR, NP2STYLE_EscapeSequence, EDITSTYLE_HOLE(L"Escape Sequence"), L"fore:#0080C0" },
	{ SCE_YAML_TEXT_BLOCK, NP2STYLE_TextBlock, EDITSTYLE_HOLE(L"Text Block"), L"fore:#F08000" },
	{ SCE_YAML_DATETIME, NP2STYLE_DateTime, EDITSTYLE_HOLE(L"DateTime"), L"fore:#008080" },
	{ SCE_YAML_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_YAML_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
};

EDITLEXER lexYAML = {
	SCLEX_YAML, NP2LEX_YAML,
	EDITLEXER_HOLE(L"YAML Document", Styles_YAML),
	L"yml; yaml; clang-format; clang-tidy; apinotes; ifs",
	&Keywords_YAML,
	Styles_YAML
};
