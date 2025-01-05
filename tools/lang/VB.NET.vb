' Visual Basic Language Reference
' https://learn.microsoft.com/en-us/dotnet/visual-basic/language-reference/
' Visual Basic language specification
' https://learn.microsoft.com/en-us/dotnet/visual-basic/reference/language-specification/

'! keywords				=======================================================
' https://learn.microsoft.com/en-us/dotnet/visual-basic/language-reference/statements/
AddHandler AddressOf
Call
	Public Protected Friend Private Shadows MustInherit NotInheritable Partial
Class Of
	Inherits
	Implements
End Class
Const
	Shadows Overloads
Declare Ansi Unicode Auto Sub Lib Alias
Declare Function Lib Alias As
	Shared Shadows Static ReadOnly
Delegate Sub Of As
Delegate Function Of As
Dim WithEvents As New ' With {}
Do
	End
	Continue Do
	Exit Do
	Do While
	Loop
	Do Until
	Loop
	Do
	Loop While
	Do
	Loop Until
Loop
Enum As
End Enum
Erase
Error
Shared Shadows Event As Implements
Custom Event As Implements
	AddHandler()
	End AddHandler
	RemoveHandler()
	End RemoveHandler
	RaiseEvent()
	End RaiseEvent
End Event
For Each As In
	Continue For
	Exit For
Next
For As To Step
Next
	Shared Shadows Async Iterator
Function Of As Implements Handles
	Optional ByVal ByRef ParamArray
	Exit Function
End Function
GoTo
	Default Shared Shadows ReadOnly WriteOnly Iterator
If Then
ElseIf Then
Else
End If
If Then Else
Implements
Imports
Inherits
Interface Of
	Inherits
	Property As Implements
	Function
	Sub
	Event
	Interface
	Class
	Structure
End Interface
Mid()
Module
End Module
Namespace Global
End Namespace
On Error GoTo
On Error Resume Next
	Public Overloads Shared Shadows Widening Narrowing
Operator As
	Return
End Operator
Option Compare Binary Text
Option Explicit On Off
Option Infer On Off
Option Strict On Off
Property As Implements
	Get
	End Get
	Set()
	End Set
End Property
RaiseEvent
ReDim Preserve
Rem
RemoveHandler AddressOf
Resume Next
Return
Select Case
	Case Else
	Case To
End Select
Stop
Structure Of
	Implements
End Structure
Partial Sub
	Exit Sub
End Sub
SyncLock
End SyncLock
Throw New
Try
	Exit Try
Catch As When
Catch
Finally
End Try
Using
End Using
While
	Continue While
	Exit While
End While
With
End With
Yield

' https://learn.microsoft.com/en-us/dotnet/visual-basic/language-reference/queries/
Aggregate As In Into
Distinct
From As In
Group By Into
Group Join As In On Equals And Equals Into
Let
Order By Ascending Descending
Select
Skip While
Take While
Where

' https://learn.microsoft.com/en-us/dotnet/visual-basic/language-reference/operators/
AddressOf And AndAlso Await
DirectCast
GetType() GetXMLNamespace()
Is If() IsFalse() IsNot IsTrue()
Like
Mod
NameOf() New Not
Or OrElse
TryCast() TypeOf Is IsNot
Xor

' https://learn.microsoft.com/en-us/dotnet/visual-basic/programming-guide/language-features/procedures/lambda-expressions
Function() And
Sub() And
Async Sub()
End Sub

' https://learn.microsoft.com/en-us/dotnet/visual-basic/language-reference/modifiers/
Ansi Assembly Async Auto
ByRef ByVal
Default
Friend
In Iterator
Key
MustInherit MustOverride
Narrowing NotInheritable NotOverridable
Optional Out Overloads Overridable Overrides
ParamArray Partial Private Protected Friend Public
ReadOnly
Shadows Shared Static
Unicode
Widening WithEvents WriteOnly

' https://learn.microsoft.com/en-us/dotnet/visual-basic/language-reference/keywords/
Me My MyBase MyClass
False True Nothing

'! types				=======================================================
' https://learn.microsoft.com/en-us/dotnet/visual-basic/language-reference/data-types/
Boolean Byte
Char
Date Decimal Double
Integer Long
Object
SByte Short Single String
UInteger ULong UShort

Variant

'! attributes			=======================================================
' https://learn.microsoft.com/en-us/dotnet/visual-basic/programming-guide/concepts/attributes/
<Serializable()>
<AttributeUsage()>
<StructLayout()>
<DllImport()>
<MarshalAs()>
<Obsolete()>
<Conditional()>
' https://learn.microsoft.com/en-us/dotnet/visual-basic/language-reference/attributes
<ComClass()>
<HideModuleName()>
<MyGroupCollection()>
<VBFixedArray()>
<VBFixedString()>
' https://learn.microsoft.com/en-us/dotnet/visual-basic/programming-guide/language-features/procedures/extension-methods
<Extension()>

'! directives			=======================================================
' https://learn.microsoft.com/en-us/dotnet/visual-basic/reference/language-specification/preprocessing-directives
#If Then
#ElseIf Then
#Else
#End If
#Const
#ExternalChecksum
#ExternalSource
#End ExternalSource
#Region
#End Region
#Disable
#Enable

'! objects				=======================================================
' https://learn.microsoft.com/en-us/dotnet/api/microsoft.visualbasic?view=net-8.0
Collection {
	Count
	Item()

	Add()
	Clear()
	Contains()
	GetEnumerator()
	Remove()
}

ControlChars {}
Conversion {}
DateAndTime {}

ErrObject {
	Description
	HelpContext
	HelpFile
	LastDllError
	Number
	Source

	Clear()
	GetException()
	Raise()
}
Err {}

FileSystem {}
Financial {}
Information {}
Interaction {}
Strings {}
VBMath {}

' https://learn.microsoft.com/en-us/dotnet/api/system?view=net-8.0
Console {}
Math {
	Abs()
	Acos()
	Asin()
	Atan()
	Atan2()
	BigMul()
	Ceiling()
	Cos()
	Cosh()
	DivRem()
	Exp()
	Floor()
	IEEERemainder()
	Log()
	Log10()
	Max()
	Min()
	Pow()
	Round()
	Sign()
	Sin()
	Sinh()
	Sqrt()
	Tan()
	Tanh()
	Truncate()

	Acosh()
	Asinh()
	Atanh()
	BitDecrement()
	BitIncrement()
	Cbrt()
	Clamp()
	CopySign()
	FusedMultiplyAdd()
	ILogB()
	Log2()
	MaxMagnitude()
	MinMagnitude()
	ScaleB()
}

'! functions			=======================================================
' https://learn.microsoft.com/en-us/dotnet/visual-basic/language-reference/functions/
' Type Conversion Functions
CBool(expression)
CByte(expression)
CChar(expression)
CDate(expression)
CDbl(expression)
CDec(expression)
CInt(expression)
CLng(expression)
CObj(expression)
CSByte(expression)
CShort(expression)
CSng(expression)
CStr(expression)
CUInt(expression)
CULng(expression)
CUShort(expression)
CType()

' Conversion Class
CTypeDynamic()
ErrorToString()
Fix()
Hex()
Int()
Oct()
Str()
Val()

' DateAndTime Class
DateString
Now
TimeOfDay
Timer
TimeString
Today

DateAdd()
DateDiff()
DatePart()
DateSerial()
DateValue()
Day()
Hour()
Minute()
Month()
MonthName()
Second()
TimeSerial()
TimeValue()
Weekday()
WeekdayName()
Year()

' FileSystem Class
ChDir()
ChDrive()
CurDir()
Dir()
EOF()
FileAttr()
FileClose()
FileCopy()
FileDateTime()
FileGet()
FileGetObject()
FileLen()
FileOpen()
FilePut()
FilePutObject()
FileWidth()
FreeFile()
GetAttr()
Input()
InputString()
Kill()
LineInput()
Loc()
Lock()
LOF()
MkDir()
Print()
PrintLine()
Rename()
Reset()
RmDir()
Seek()
SetAttr()
Spc()
Tab()
Unlock()
Write()
WriteLine()

' Financial Class
DDB()
FV()
IPmt()
IRR()
MIRR()
NPer()
NPV()
Pmt()
PPmt()
PV()
Rate()
SLN()
SYD()

' Information Class
Erl()
IsArray()
IsDate()
IsDBNull()
IsError()
IsNothing()
IsNumeric()
IsReference()
LBound()
QBColor()
RGB()
SystemTypeName()
TypeName()
UBound()
VarType()
VbTypeName()

' Interaction Class
AppActivate()
Beep()
CallByName()
Choose()
Command()
CreateObject()
DeleteSetting()
Environ()
GetAllSettings()
GetObject()
GetSetting()
IIf()
InputBox()
MsgBox()
Partition()
SaveSetting()
Shell()
Switch()

' Strings Class
Asc()
AscW()
Chr()
ChrW()
Filter()
Format()
FormatCurrency()
FormatDateTime()
FormatNumber()
FormatPercent()
GetChar()
InStr()
InStrRev()
Join()
LCase()
Left()
Len()
LSet()
LTrim()
Mid()
Replace()
Right()
RSet()
RTrim()
Space()
Split()
StrComp()
StrConv()
StrDup()
StrReverse()
Trim()
UCase()

' VBMath Class
Randomize()
Rnd()

'! enumeration			=======================================================
AppWinStyle
CallType CompareMethod
DateFormat DateInterval DueDate
FileAttribute FirstDayOfWeek FirstWeekOfYear
MsgBoxStyle MsgBoxResult
OpenAccess OpenMode OpenShare
TriState
VariantType VbStrConv

AttributeTargets LayoutKind

'! constants			=======================================================
' https://learn.microsoft.com/en-us/dotnet/visual-basic/language-reference/constants-and-enumerations
vbNullString vbObjectError
' https://learn.microsoft.com/en-us/dotnet/api/microsoft.visualbasic.constants?view=net-8.0
' AppWinStyle
vbHide vbNormalFocus vbMinimizedFocus vbMaximizedFocus vbNormalNoFocus vbMinimizedNoFocus
' CallType
vbMethod vbGet vbLet vbSet
' CompareMethod
vbBinaryCompare vbTextCompare
' ControlChars
vbBack vbCr vbCrLf vbFormFeed vbLf vbNewLine vbNullChar vbTab vbVerticalTab
' DateFormat
vbGeneralDate vbLongDate vbShortDate vbLongTime vbShortTime
' FileAttribute
vbNormal vbReadOnly vbHidden vbSystem vbVolume vbDirectory vbArchive
' FirstDayOfWeek
vbUseSystemDayOfWeek vbSunday vbMonday vbTuesday vbWednesday vbThursday vbFriday vbSaturday
' FirstWeekOfYear
vbUseSystem vbFirstJan1 vbFirstFourDays vbFirstFullWeek
' MsgBoxStyle
vbOKOnly vbOKCancel vbAbortRetryIgnore vbYesNoCancel vbYesNo vbRetryCancel
vbCritical vbQuestion vbExclamation vbInformation
vbDefaultButton1 vbDefaultButton2 vbDefaultButton3
vbApplicationModal vbSystemModal
vbMsgBoxSetForeground vbMsgBoxHelp vbMsgBoxRight vbMsgBoxRtlReading
' MsgBoxResult
vbOK vbCancel vbAbort vbRetry vbIgnore vbYes vbNo
' TriState
vbUseDefault vbTrue vbFalse
' VariantType
vbEmpty vbNull vbInteger vbSingle vbDouble vbCurrency vbDate vbString vbObject
vbBoolean vbVariant vbDecimal vbByte vbChar vbLong vbUserDefinedType vbArray
' VbStrConv
vbUpperCase vbLowerCase vbProperCase vbWide vbNarrow vbKatakana vbHiragana
vbSimplifiedChinese vbTraditionalChinese vbLinguisticCasing
