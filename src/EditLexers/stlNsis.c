#include "EditLexer.h"
#include "EditStyle.h"

// http://nsis.sourceforge.net/Main%5FPage

static KEYWORDLIST Keywords_NSIS = {{
"Function FunctionEnd Section SectionEnd SectionGroup SectionGroupEnd PageEx PageExEnd Goto Var "
"Page UninstPage"

, // 1
""

, // 2 Preprocessor
"define undef ifdef ifndef if ifmacrodef ifmacrondef else endif insertmacro macro macroend searchparse searchreplace "
"include addincludedir addplugindir appendfile cd delfile echo error execute packhdr system tempfile warning verbose"

, //
""
, //
""
, "", "", "", ""

, "", "", "", "", "", "", ""
}};

EDITLEXER lexNsis = { SCLEX_NSIS, NP2LEX_NSIS, EDITLEXER_HOLE, L"Nsis Setup Script", L"nsi; nsh", L"", &Keywords_NSIS,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_PREPROCESSOR, NP2STYLE_Preprocessor, L"Preprocessor", L"fore:#FF8000", L"" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_C_CHARACTER, SCE_C_STRING, SCE_C_STRINGEOL, 0), NP2STYLE_String, L"String", L"fore:#008080", L"" },
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ MULTI_STYLE(SCE_C_ASM_INSTRUCTION, SCE_C_ASM_REGISTER, SCE_C_COMMENTDOC_TAG, 0), NP2STYLE_Variable, L"Variable", L"fore:#CC3300", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ SCE_C_LABEL, NP2STYLE_Label, L"Label", L"fore:#000000; back:#FFC040", L"" },
	EDITSTYLE_SENTINEL
}
};
