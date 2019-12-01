#include "EditLexer.h"
#include "EditStyleX.h"

// Visual Basic Language Reference
// https://docs.microsoft.com/en-us/dotnet/visual-basic/language-reference/
// https://docs.microsoft.com/en-us/dotnet/visual-basic/language-reference/keywords/
// https://docs.microsoft.com/en-us/dotnet/visual-basic/language-reference/runtime-library-members

static KEYWORDLIST Keywords_VB = {{
"AddHandler AddressOf Alias And AndAlso As ByRef ByVal Call Case Catch Class Const Continue "
"Declare Default Delegate Dim DirectCast Do Each Else ElseIf End Enum Erase Error Event Exit "
"False Finally For Friend Function Get GetType GetXMLNamespace Global Goto Handles If Implements "
"Imports In Inherits Interface Is IsNot Let Lib Like Loop Me Mod Module MustInherit MustOverride "
"My MyBase MyClass Namespace Narrowing New Next Not Nothing NotInheritable NotOverridable Of On "
"Operator Option Optional Or OrElse Out Overloads Overridable Overrides ParamArray Partial Private "
"Property Protected Public RaiseEvent Readonly ReDim Rem RemoveHandler Resume Return Select Set "
"Shadows Shared Static Step Stop Structure Sub SyncLock Then Throw To True Try TryCast TypeOf Using "
"When While Widening With WithEvents WriteOnly Xor "
// Unreserved Keyword
"Aggregate Ansi Assembly Async Attribute Auto Await Binary By Compare Custom Distinct Equals Explicit "
"From Group Into IsFalse IsTrue Join Key Mid Off Order Preserve Skip Strict Take Text Unicode Until Where Yield "

"Array Any Count GroupBy OrderBy "
// VB6
"Begin BeginProperty EndProperty Type"

, // 1 Type Keyword
"Boolean Byte CBool CByte CChar CDate CDbl CDec Char CInt CLng CObj CSByte CShort CSng CStr CType CUInt "
"CULng CUShort Date Decimal Double Integer Long Object SByte Short Single String UInteger ULong UShort"

, // 2 not used keyword, used in VBScript
"EndIf GoSub Variant Wend "

, // 3 Preprocessor
"If Else ElseIf End Const Region ExternalChecksum ExternalSource "

, // 4 Attribute
"ComClass HideModuleName WebMethod Serializable MarshalAs AttributeUsage DllImport StructLayout VBFixedString VBFixedArray"

, // 5 VB Const
// String
"vbCr VbCrLf vbFormFeed vbLf vbNewLine vbNullChar vbNullString vbTab vbVerticalTab "
// MsgBox
"vbOK vbCancel vbAbort vbRetry vbIgnore vbYes vbNo "
"vbOKOnly vbOKCancel vbAbortRetryIgnore vbYesNoCancel vbYesNo vbRetryCancel "
"vbCritical vbQuestion vbExclamation vbInformation vbDefaultButton1 vbDefaultButton2 "
"vbDefaultButton3 vbApplicationModal vbSystemModal "
"vbMsgBoxHelp vbMsgBoxRight vbMsgBoxRtlReading vbMsgBoxSetForeground "
// Comparison
"vbBinaryCompare vbTextCompare "
// Date and Time
"vbSunday vbMonday vbTuesday vbWednesday vbThursday vbFriday vbSaturday "
"vbUseSystemDayOfWeek vbFirstJan1 vbFirstFourDays vbFirstFullWeek "
// Date Format
"vbGeneralDate vbLongDate vbShortDate vbLongTime vbShortTime "
// Tristate
"vbUseDefault vbTrue vbFalse "
// VarType
"vbEmpty vbNull vbInteger vbLong vbSingle vbDouble vbCurrency vbDate vbString "
"vbObject vbBoolean vbVariant vbDecimal vbByte vbArray vbDirectory "
// Miscellaneous
"vbObjectError vbArchive vbBack vbGet vbHidden vbHide vbLet vbLinguisticCasing "
"vbLowerCase vbMaximizedFocus vbMethod vbMinimizedFocus vbMinimizedNoFocus "
"vbNarrow vbNormal vbNormalFocus vbNormalNoFocus vbProperCase vbReadOnly vbSet "
"vbSystem vbUpperCase vbUserDefinedType vbUseSystem vbVolume vbWide"
"vbHiragana vbKatakana vbSimplifiedChinese vbTraditionalChinese "

, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_VB[] = {
	EDITSTYLE_DEFAULT,
	{ MULTI_STYLE(SCE_B_KEYWORD, SCE_B_KEYWORD3, 0, 0), NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_B_KEYWORD2, NP2StyleX_TypeKeyword, L"fore:#0000FF" },
	{ SCE_B_PREPROCESSOR, NP2StyleX_Preprocessor, L"fore:#FF8000" },
	{ SCE_B_KEYWORD4, NP2StyleX_Attribute, L"fore:#FF8000" },
	{ SCE_B_CONSTANT, NP2StyleX_Constant, L"fore:#B000B0" },
	{ SCE_B_COMMENT, NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_B_STRING, SCE_B_STRINGEOL, 0, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_B_LABEL, NP2StyleX_Label, L"back:#FFC040" },
	{ MULTI_STYLE(SCE_B_NUMBER, SCE_B_DATE, 0, 0), NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_B_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexVB = {
	SCLEX_VB, NP2LEX_VB,
	EDITLEXER_HOLE(L"Visual Basic", Styles_VB),
	L"vb; bas; frm; cls; ctl; pag; dsr; dob",
	&Keywords_VB,
	Styles_VB
};
