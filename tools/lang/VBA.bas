' Office VBA Reference
' https://learn.microsoft.com/en-us/office/vba/api/overview/language-reference
' [MS-VBAL]: VBA Language Specification
' https://learn.microsoft.com/en-us/openspecs/microsoft_general_purpose_programming_languages/ms-vbal/
' https://msopenspecs.azureedge.net/files/MS-VBAL/[MS-VBAL].pdf
' Visual Basic 6.0 Documentation
' https://learn.microsoft.com/en-us/previous-versions/visualstudio/visual-basic-6/visual-basic-6.0-documentation
' FreeBASIC Manual
' https://www.freebasic.net/wiki/DocToc

'! VBA keywords			=======================================================
' https://learn.microsoft.com/en-us/office/vba/language/reference/statements
AppActivate
Beep
Call
ChDir
ChDrive
Close
Const As
Date
Declare PtrSafe Sub Lib Alias
Declare PtrSafe Function Lib Alias As
DefBool DefByte DefInt DefLng DefLngLng DefLngPtr
DefCur DefSng DefDbl DefDec DefDate DefStr DefObj DefVar
DeleteSetting
Dim WithEvents As New
Do
	End
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
Enum
End Enum
Erase
Error
Public Event
FileCopy
For Each In
	Exit For
Next
For To Step
Next
Public Private Friend Static Function As
	Optional ByVal ByRef ParamArray As
	Exit Function
End Function
Get
GoSub Return
GoTo
If Then
ElseIf Then
Else
End If
If Then Else
Implements
Input
Kill
Let
Line Input
Load
Lock To
Unlock To
LSet
Mid()
MkDir
Name As
On Error GoTo
On Error Resume Next
On GoSub
On GoTo
Open For Access As Len
Option Base
Option Compare Binary Text Database
Option Explicit
Option Private Module
Print Spc() Tab()
Private WithEvents As New
Property Get As
	Exit Property
End Property
Property Let
End Property
Property Set
End Property
Public WithEvents As New
Put
RaiseEvent
Randomize
ReDim Preserve As
Rem
Reset
Resume Next
RmDir
RSet
SavePicture		' VB6
SaveSetting
Seek
Select Case
	Case Else
	Case To
End Select
SendKeys
Set New Nothing
SetAttr
Static As New
Stop
Sub
	Exit Sub
End Sub
Time
Private Public Type
End Type
Unload
While
Wend
Width
With
End With
Write

' https://learn.microsoft.com/en-us/office/vba/language/reference/keywords-visual-basic-for-applications
Me
Empty Null
False True

' https://learn.microsoft.com/en-us/office/vba/language/reference/user-interface-help/operator-summary
AddressOf And Eqv Imp Is Like Mod Not Or Xor

' VB6
Class Attribute Version
Begin
	BeginProperty
	EndProperty
End
If
EndIf

'! types				=======================================================
' https://learn.microsoft.com/en-us/office/vba/language/reference/user-interface-help/data-type-summary
Boolean Byte
Currency
Date Decimal Double
Integer
Long LongLong LongPtr
Object
Single String
Variant
Any

'! directives			=======================================================
' https://learn.microsoft.com/en-us/office/vba/language/reference/directives
#Const
#If Then
#ElseIf Then
#Else
#End If
#If
#EndIf

'! objects				=======================================================
' https://learn.microsoft.com/en-us/office/vba/language/reference/objects-visual-basic-for-applications
Collection {
	Add()
	Item(index)
	Remove(index)
	Count
}

Debug {
	Assert()
	Print()
}

Dictionary {
	Add()
	Exists(key)
	Items()
	Keys()
	Remove(key)
	RemoveAll()

	CompareMode
	Count
	Item(key)
	Key(key)
}

{ ' Drive
	AvailableSpace
	DriveLetter
	DriveType
	FileSystem
	FreeSpace
	IsReady
	RootFolder
	SerialNumber
	ShareName
	TotalSize
	VolumeName
}

Err {
	Clear()
	Raise()

	Description
	HelpContext
	HelpFile
	Number
	Source
}

{ ' File
	Copy()
	Delete()
	Move()
	OpenAsTextStream([iomode, [format]])

	Attributes
	DateCreated
	DateLastAccessed
	DateLastModified
	Drive
	Name
	ParentFolder
	Path
	ShortName
	ShortPath
	Size
	' Type
}

FileSystemObject {
	BuildPath(path, name)
	CopyFile()
	CopyFolder()
	CreateFolder(foldername)
	CreateTextFile(filename, [overwrite, [unicode]])
	DeleteFile()
	DeleteFolder()
	DriveExists(drivespec)
	FileExists(filespec)
	FolderExists(folderspec)
	GetAbsolutePathName(pathspec)
	GetBaseName(path)
	GetDrive(drivespec)
	GetDriveName(path)
	GetExtensionName(path)
	GetFile(filespec)
	GetFileName(pathspec)
	GetFolder(folderspec)
	GetParentFolderName(path)
	GetSpecialFolder(folderspec)
	GetTempName()
	MoveFile()
	MoveFolder(source, destination)
	OpenTextFile(filename, [iomode, [create, [format]]])

	Drives
}

{ ' Folder
	Add()

	Files
	IsRootFolder
	SubFolders
}

{ ' TextStream
	Close()
	Read(characters)
	ReadAll()
	ReadLine()
	Skip(characters)
	SkipLine()
	Write(string)
	WriteBlankLines(lines)
	WriteLine([string])

	AtEndOfLine
	AtEndOfStream
	Column
	Line
}

{ ' UserForm
	Hide()
	PrintForm()
	Show()

	Calendar
	RightToLeft
	ShowModal
	StartUpPosition
}

'! functions			=======================================================
' https://learn.microsoft.com/en-us/office/vba/language/reference/functions-visual-basic-for-applications
' https://learn.microsoft.com/en-us/office/vba/language/concepts/getting-started/returning-strings-from-functions
' Conversion functions
Asc(string)
AscB(string)
AscW(string)
Chr(charcode)
ChrB(charcode)
ChrW(charcode)
CVErr(errornumber)
Format(Expression, [Format], [FirstDayOfWeek], [FirstWeekOfYear])
Hex(number)
Oct(number)
Str(number)
Val(string)

' Type conversion functions
CBool(expression)
CByte(expression)
CCur(expression)
CDate(expression)
CVDate(expression)
CDbl(expression)
CDec(expression)
CInt(expression)
CLng(expression)
CLngLng(expression)
CLngPtr(expression)
CSng(expression)
CStr(expression)
CVar(expression)

' Math functions
Abs(number)
Atn(number)
Cos(number)
Exp(number)
Int(number)
Fix(number)
Log(number)
Rnd(Number)
Sgn(number)
Sin(number)
Sqr(number)
Tan(number)

' Other functions
Array(arglist)
CallByName(object, procname, calltype, [args])
Choose(index, choice)
Command()
CreateObject(name, [servername])
CurDir(drive)

Date
DateAdd(interval, number, date)
DateDiff(interval, date1, date2, [firstdayofweek, [firstweekofyear]])
DatePart(interval, date, [firstdayofweek, [firstweekofyear]])
DateSerial(year, month, day)
DateValue(date)
Day(date)
DDB(cost, salvage, life, period, [factor])
Dir(pathname, [attributes])
DoEvents()
Environ({ envstring | number })
EOF(filenumber)
Error(errornumber)

FileAttr(filenumber, returntype)
FileDateTime(pathname)
FileLen(pathname)
Filter(sourcearray, match, [include, [compare]])
FormatCurrency(Expression, [NumDigitsAfterDecimal, [IncludeLeadingDigit, [UseParensForNegativeNumbers, [GroupDigits]]]])
FormatDateTime(Date, [NamedFormat])
FormatNumber(Expression, [NumDigitsAfterDecimal, [IncludeLeadingDigit, [UseParensForNegativeNumbers, [GroupDigits]]]])
FormatPercent(Expression, [NumDigitsAfterDecimal, [IncludeLeadingDigit, [UseParensForNegativeNumbers, [GroupDigits]]]])
FreeFile(rangenumber)
FV(rate, nper, pmt, [pv, [type]])

GetAllSettings(appname, section)
GetAttr(pathname)
GetObject([pathname], [class])
GetSetting(appname, section, key, [default])
Hour(time)

IIf(expr, truepart, falsepart)
Input(number, filenumber)
InputB(number, filenumber)
InputBox(prompt, [title], [default], [xpos], [ypos], [helpfile, context])
InStr([start], string1, string2, [compare])
InStrB([start], string1, string2, [compare])
InStrRev(stringcheck, stringmatch, [start, [compare]])
IPmt(rate, per, nper, pv, [fv, [type]])
IRR(values(), [guess])
IsArray(varname)
IsDate(expression)
IsEmpty(expression)
IsError(expression)
IsMissing(argname)
IsNull(expression)
IsNumeric(expression)
IsObject(identifier)

Join(sourcearray, [delimiter])
LBound(arrayname, [dimension])
LCase(string)
Left(string, length)
LeftB(string, length)
Len(string | varname)
LenB(string | varname)
Loc(filenumber)
LOF(filenumber)
LTrim(string)
RTrim(string)
Trim(string)

Mid(string, start, [length])
MidB(string, start, [length])
Minute(time)
MIRR(values( ), finance_rate, reinvest_rate)
Month(date)
MonthName(month, [abbreviate])
MsgBox(prompt, [buttons,] [title,] [helpfile, context])
Now
NPer(rate, pmt, pv, [fv, [type]])
NPV(rate, values( ))
Partition(number, start, stop, interval)
Pmt(rate, nper, pv, [fv, [type]])
PPmt(rate, per, nper, pv, [fv, [type]])
PV(rate, nper, pmt, [fv, [type]])
QBColor(color)

Rate(nper, pmt, pv, [fv, [type, [guess]]])
Replace(expression, find, replace, [start, [count, [compare]]])
RGB(red, green, blue)
Right(string, length)
RightB(string, length)
Round(expression, [numdecimalplaces])
Second(time)
Seek(filenumber)
Shell(pathname, [windowstyle])
SLN(cost, salvage, life)
Space(number)
Spc(n)
Split(expression, [delimiter, [limit, [compare]]])
StrComp(string1, string2, [compare])
StrConv(string, conversion, [LCID])
String(number, character)
StrReverse(expression)
Switch(expr-1, value-1, [expr-2, value-2…, [expr-n, value-n]])
SYD(cost, salvage, life, period)

Tab(n)
Time
Timer
TimeSerial(hour, minute, second)
TimeValue(time)
TypeName(varname)
UBound(arrayname, [dimension])
UCase(string)
VarType(varname)
Weekday(date, [firstdayofweek])
WeekdayName(weekday, abbreviate, firstdayofweek)
Year(date)

'! constants			=======================================================
' https://learn.microsoft.com/en-us/office/vba/language/reference/constants-visual-basic-for-applications
' Calendar constants
vbCalGreg vbCalHijri
' CallType constants
vbMethod vbGet vbLet vbSet
' Color constants
vbBlack vbRed vbGreen vbYellow vbBlue vbMagenta vbCyan vbWhite
' Comparison constants
vbUseCompareOption vbBinaryCompare vbTextCompare vbDatabaseCompare
' Date constants
vbUseSystem vbSunday vbMonday vbTuesday vbWednesday vbThursday vbFriday vbSaturday
vbUseSystemDayOfWeek vbFirstJan1 vbFirstFourDays vbFirstFullWeek
' Date Format constants
vbGeneralDate vbLongDate vbShortDate vbLongTime vbShortTime
' Dir, GetAttr, and SetAttr constants
vbNormal vbReadOnly vbHidden vbSystem vbVolume vbDirectory vbArchive vbAlias
' DriveType constants
' File Attribute constants
' File Input/Output constants
' Form constants
vbModeless vbModal
' Keycode constants
' Miscellaneous constants
vbCrLf vbCr vbLf vbNewLine vbNullChar vbNullString vbObjectError vbTab vbBack vbFormFeed vbVerticalTab
' MsgBox constants
vbOKOnly vbOKCancel vbAbortRetryIgnore vbYesNoCancel vbYesNo vbRetryCancel
vbCritical vbQuestion vbExclamation vbInformation
vbDefaultButton1 vbDefaultButton2 vbDefaultButton3 vbDefaultButton4
vbApplicationModal vbSystemModal
vbMsgBoxHelpButton vbMsgBoxSetForeground vbMsgBoxRight vbMsgBoxRtlReading
vbOK vbCancel vbAbort vbRetry vbIgnore vbYes vbNo
' QueryClose constants
vbFormControlMenu vbFormCode vbAppWindows vbAppTaskManager
' Shell constants
vbHide vbNormalFocus vbMinimizedFocus vbMaximizedFocus vbNormalNoFocus vbMinimizedNoFocus
' SpecialFolder constants
' StrConv constants
vbUpperCase vbLowerCase vbProperCase vbWide vbNarrow vbKatakana vbHiragana vbUnicode vbFromUnicode
' System Color constants
' Tristate constants
vbTrue vbFalse vbUseDefault
' VarType constants
vbEmpty vbNull vbInteger vbLong vbSingle vbDouble vbCurrency vbDate vbString vbObject vbError
vbBoolean vbVariant vbDataObject vbDecimal vbByte vbLongLong vbUserDefinedType vbArray
