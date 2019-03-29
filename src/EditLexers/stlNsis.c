#include "EditLexer.h"
#include "EditStyle.h"

// https://nsis.sourceforge.io/Main_Page

static KEYWORDLIST Keywords_NSIS = {{
"Function FunctionEnd Section SectionEnd SectionGroup SectionGroupEnd PageEx PageExEnd Goto Var "
"Page UninstPage"

, // 1
NULL

, // 2 Preprocessor
"define undef ifdef ifndef if ifmacrodef ifmacrondef else endif insertmacro macro macroend searchparse searchreplace "
"include addincludedir addplugindir appendfile cd delfile echo error execute packhdr system tempfile warning verbose"

, //
NULL
, //
NULL
, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_NSIS[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_C_PREPROCESSOR, NP2STYLE_Preprocessor, EDITSTYLE_HOLE(L"Preprocessor"), L"fore:#FF8000" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#008000" },
	{ MULTI_STYLE(SCE_C_CHARACTER, SCE_C_STRING, SCE_C_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008080" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ MULTI_STYLE(SCE_C_ASM_INSTRUCTION, SCE_C_ASM_REGISTER, SCE_C_COMMENTDOC_TAG, 0), NP2STYLE_Variable, EDITSTYLE_HOLE(L"Variable"), L"fore:#CC3300" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ SCE_C_LABEL, NP2STYLE_Label, EDITSTYLE_HOLE(L"Label"), L"back:#FFC040" },
};

EDITLEXER lexNsis = {
	SCLEX_NSIS, NP2LEX_NSIS,
	EDITLEXER_HOLE(L"Nsis Setup Script", Styles_NSIS),
	L"nsi; nsh",
	&Keywords_NSIS,
	Styles_NSIS
};

