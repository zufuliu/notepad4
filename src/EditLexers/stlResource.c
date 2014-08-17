#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_RC = {
"defined "
// Resources:
"ACCELERATORS BITMAP CURSOR DIALOG DIALOGEX FONT HTML ICON MENU MENUEX MESSAGETABLE POPUP PNG RCDATA REGISTRY RT_MANIFEST STRINGTABLE STYLE_XML TEXTINCLUDE TYPELIB VERSIONINFO "
// Controls:
"AUTO3STATE AUTOCHECKBOX AUTORADIOBUTTON CHECKBOX COMBOBOX CONTROL CTEXT DEFPUSHBUTTON EDITTEXT GROUPBOX LISTBOX LTEXT PUSHBOX PUSHBUTTON RADIOBUTTON RTEXT SCROLLBAR STATE3 "
// Statements
"CAPTION CHARACTERISTICS CLASS EXSTYLE LANGUAGE MENUITEM STYLE VERSION "
"BEGIN END BLOCK BUTTON SEPARATOR VALUE "
"VIRTKEY ALT SHIFT NOINVERT "
"PRELOAD LOADONCALL FIXED DISCARDABLE PURE IMPURE SHARED NONSHARED "
, // type keyword
""
, // preprocessor
"define undef ifdef ifndef if elif else endif include pragma error code_page "
, // directive
"" 
, // attribute
""
, // class
""
, // interface
""
, // enumeration
""
, // constant
"RC_INVOKED __DATE__ __FILE__ __LINE__ __STDC__ __TIME__ __TIMESTAMP__ "

#if NUMKEYWORD == 16
,"","","","","",""
,
"defined() code_page() "
#endif
};

EDITLEXER lexRC = { SCLEX_CPP, NP2LEX_RC, L"Resource Script", L"rc; rc2; rct; rh; dlg", L"", &Keywords_RC,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_PREPROCESSOR, NP2STYLE_Preprocessor, L"Preprocessor", L"fore:#FF8000", L"" },
	{ MULTI_STYLE(SCE_C_COMMENT,SCE_C_COMMENTLINE,SCE_C_COMMENT,SCE_C_COMMENTLINE), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_C_STRING,SCE_C_CHARACTER,SCE_C_STRINGEOL,0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
