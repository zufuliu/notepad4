#include "EditLexer.h"

static KEYWORDLIST Keywords_IDL = {
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

#if NUMKEYWORD == 16
,"","","","","","",""
#endif
};

EDITLEXER lexIDL = { SCLEX_CPP, 63006, L"Interface Definition File", L"idl; odl", L"", &Keywords_IDL,
{
	{ STYLE_DEFAULT, 63126, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
	{ MULTI_STYLE(SCE_C_COMMENT,SCE_C_COMMENTLINE,0,0), 63127, L"Comment", L"fore:#008000", L"" },
	{ SCE_C_WORD, 63128, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, 63129, L"Type Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_PREPROCESSOR, 63133, L"Preprocessor", L"fore:#FF8000", L"" },
	{ SCE_C_GINTERFACE, 63321, L"Interface", L"bold; fore:#1E90FF", L""},
	//{ SCE_C_GENUMERATION, 63322, L"Enumeration", L"fore:#FF8000", L""},
	//{ SCE_C_GCONSTANT, 63323, L"Macro", L"bold; fore:#B000B0", L""},
	{ MULTI_STYLE(SCE_C_STRING,SCE_C_CHARACTER,SCE_C_STRINGEOL,0), 63131, L"String", L"fore:#008000", L"" },
	{ SCE_C_NUMBER, 63130, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, 63132, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
