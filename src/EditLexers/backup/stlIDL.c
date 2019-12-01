#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_IDL = {{
"const enum import in interface out typedef uuid "
,	// type keyword
"BSTR LONG DWORD HRESULT ULONG "
,	// preprocessor
"define endif else if ifdef ifndef include pragma"
,	// directive
""
,	// attribute
""
,	// class
""
,	// interface
"IDispatch IUnknown "
,	// enumeration
""
,	// constant
""

, NULL, NULL, NULL, NULL, NULL, NULL
, NULL
}};

static EDITSTYLE Styles_IDL[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2StyleX_TypeKeyword, L"fore:#0000FF" },
	{ SCE_C_PREPROCESSOR, NP2StyleX_Preprocessor, L"fore:#FF8000" },
	{ SCE_C_INTERFACE, NP2StyleX_Interface, L"bold; fore:#1E90FF" },
	{ SCE_C_ENUMERATION, NP2StyleX_Enumeration, L"fore:#FF8000" },
	{ MULTI_STYLE(SCE_C_MACRO2, SCE_C_MACRO, 0, 0), NP2StyleX_Macro, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_C_COMMENTDOC_TAG, NP2StyleX_DocCommentTag, L"fore:#408080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2StyleX_DocComment, L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexIDL = {
	SCLEX_CPP, NP2LEX_IDL,
	EDITLEXER_HOLE(L"Interface Definition File", Styles_IDL),
	L"idl; odl; midl",
	&Keywords_IDL,
	Styles_IDL
};
