#include "EditLexer.h"
#include "EditStyleX.h"

// VBScript Language Reference
// https://docs.microsoft.com/en-us/previous-versions//d1wf56tt(v=vs.85)
// https://docs.microsoft.com/en-us/previous-versions//ydz4cfk3(v=vs.85)

static KEYWORDLIST Keywords_VBS = {{
"Alias And As Attribute Array Begin ByRef ByVal Call Case Class Compare Const Continue "
"Declare Dim Do Each Else ElseIf Empty End Enum Eqv Erase Error Event Exit "
"Explicit False For Friend Function Get Global GoSub Goto If Imp Implement In Is Let Lib "
"Load Loop LSet Me Mid Mod Module New Next Not Nothing Null On Option Optional Or "
"Preserve Private Property Public RaiseEvent ReDim Rem Resume Return RSet Select Set "
"Static Stop Sub Then To True Type Unload Until Wend While With WithEvents Xor"

, // 1 Type Keyword
"Boolean Byte Currency Date Double Integer Long Object Single String Variant "
"RegExp "

, // 2 not used keyword
"Err WScript"

, // 3 Preprocessor
NULL
, // 4 Attribute/Property
NULL

, // 5 VB Const
// String
"vbCr vbCrLf vbFormFeed vbLf vbNewLine vbNullChar vbNullString vbTab vbVerticalTab "
// MsgBox
"vbOK vbCancel vbAbort vbRetry vbIgnore vbYes vbNo "
"vbOKOnly vbOKCancel vbAbortRetryIgnore vbYesNoCancel vbYesNo vbRetryCancel "
"vbCritical vbQuestion vbExclamation vbInformation vbDefaultButton1 vbDefaultButton2 "
"vbDefaultButton3 vbDefaultButton4 vbApplicationModal vbSystemModal "
// Color
"vbBlack vbRed vbGreen vbYellow vbBlue vbMagenta vbCyan vbWhite "
// Comparison
"vbBinaryCompare vbTextCompare "
// Date and Time
"vbSunday vbMonday vbTuesday vbWednesday vbThursday vbFriday vbSaturday "
"vbUseSystemDayOfWeek vbFirstJan1 vbFirstFourDays vbFirstFullWeek "
// Date Format
"vbGeneralDate vbLongDate vbShortDate vbLongTime vbShortTime "
// Tristate
"vbUseDefault vbTrue vbFalse"
// VarType
"vbEmpty vbNull vbInteger vbLong vbSingle vbDouble vbCurrency vbDate vbString "
"vbObject vbError vbBoolean vbVariant vbDataObject vbDecimal vbByte vbArray "
// Miscellaneous
"vbObjectError"

, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_VBS[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_B_KEYWORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_B_KEYWORD2, NP2StyleX_TypeKeyword, L"fore:#0000FF" },
	{ SCE_B_CONSTANT, NP2StyleX_Constant, L"fore:#B000B0" },
	{ SCE_B_COMMENT, NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_B_STRING, SCE_B_STRINGEOL, 0, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_B_LABEL, NP2StyleX_Label, L"back:#FFC040" },
	{ SCE_B_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_B_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexVBS = {
	SCLEX_VBSCRIPT, NP2LEX_VBS,
	EDITLEXER_HOLE(L"VBScript", Styles_VBS),
	L"vbs; dsm",
	&Keywords_VBS,
	Styles_VBS
};
