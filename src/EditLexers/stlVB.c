#include "EditLexer.h"
#include "EditStyle.h"
//http://msdn.microsoft.com/en-us/library/sh9ywfdk.aspx
//http://msdn.microsoft.com/en-us/library/dd409611.aspx
//http://msdn.microsoft.com/en-us/library/c157t28f.aspx

static KEYWORDLIST Keywords_VB = {
"AddHandler AddressOf Alias And AndAlso As ByRef ByVal Call Case Catch Class Const Continue "
"Declare Default Delegate Dim DirectCast Do Each Else ElseIf End Enum Erase Error Event Exit "
"False Finally For Friend Function Get GetType GetXMLNamespace Global Goto Handles If Implements "
"Imports In Inherits Interface Is IsNot Let Lib Like Loop Me Mod Module MustInherit MustOverride "
"My MyBase MyClass Namespace Narrowing New Next Not Nothing NotInheritable NotOverridable Of On "
"Operator Option Optional Or OrElse Out Overloads Overridable Overrides ParamArray Partial Private "
"Property Protected Public RaiseEvent Readonly ReDim Rem RemoveHandler Resume Return Select Set "
"Shadows Shared Static Step Stop Structure Sub SyncLock Then Throw To True Try TryCast TypeOf Using "
"When While Widening With WithEvents WriteOnly Xor "
//Unreserved Keywords
"Aggregate Ansi Assembly Async Attribute Auto Await Binary By Compare Custom Distinct Equals Explicit "
"From Group Into IsFalse IsTrue Join Key Mid Off Order Preserve Skip Strict Take Text Unicode Until Where Yield "

"Array Any Count GroupBy OrderBy "
// VB6
"Begin BeginProperty EndProperty Type"
, // type keyword
"Boolean Byte CBool CByte CChar CDate CDbl CDec Char CInt CLng CObj CSByte CShort CSng CStr CType CUInt "
"CULng CUShort Date Decimal Double Integer Long Object SByte Short Single String UInteger ULong UShort"
,// not used keyword, used in VBScript
"EndIf GoSub Variant Wend "
, // preprocessor
"If Else ElseIf End Const Region ExternalChecksum ExternalSource "
, // attribute
"ComClass HideModuleName WebMethod Serializable MarshalAs AttributeUsage DllImport StructLayout VBFixedString VBFixedArray"
, // VB Const
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

, "", "", ""

#if NUMKEYWORD == 16
,"","","","","","",""
#endif
};

EDITLEXER lexVB = { SCLEX_VB, NP2LEX_VB, L"Visual Basic", L"vb; bas; frm; cls; ctl; pag; dsr; dob", L"", &Keywords_VB,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_B_DEFAULT, L"Default", L"", L"" },
	{ MULTI_STYLE(SCE_B_KEYWORD,SCE_B_KEYWORD3,0,0), NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_B_KEYWORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#0000FF", L"" },
	{ SCE_B_PREPROCESSOR, NP2STYLE_Preprocessor, L"Preprocessor", L"fore:#FF9C00", L"" },
	{ SCE_B_KEYWORD4, NP2STYLE_Attribute, L"Attribute", L"fore:#FF8000", L""},
	{ SCE_B_CONSTANT, NP2STYLE_Constant, L"Constant", L"bold; fore:#B000B0", L""},
	{ SCE_B_COMMENT, NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_B_STRING,SCE_B_STRINGEOL,0,0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_B_LABEL, NP2STYLE_Label, L"Label", L"fore:#000000; back:#FFC040", L""},
	{ MULTI_STYLE(SCE_B_NUMBER,SCE_B_DATE,0,0), NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_B_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
