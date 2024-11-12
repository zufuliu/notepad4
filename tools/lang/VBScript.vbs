' VBScript Language Reference
' https://learn.microsoft.com/en-us/previous-versions/d1wf56tt(v=vs.85)
' VBScript
' https://learn.microsoft.com/en-us/previous-versions/t0aew7h6(v=vs.85)
' https://documentation.help/MS-Office-VBScript/VBSTOC.htm

'! keywords				=======================================================
' Statements https://learn.microsoft.com/en-us/previous-versions/7aw9cadb(v=vs.85)
Call
Class
	Public Property Get
		Set
		Exit Property
	End Property
	Default Property Let
	End Property
	Private Property Set
	End Property
End Class
Const
Dim
Do
	Do While
	Loop
	Do Until
	Loop
	Do
	Loop While
	Do
	Loop Until
	Exit Do
Loop
Erase
Execute
ExecuteGlobal
For Each In
	Exit For
Next
For To Step
Next
Default Function ByVal ByRef
	Exit Function
End Function
If
ElseIf Then
Else
End If
If Then Else
On Error Resume Next
On Error GoTo
Option Explicit
Randomize
ReDim Preserve
Rem
Select Case
	Case Else
End Select
Set New Nothing
Stop
Sub ByVal ByRef
	Exit Sub
End Sub
While
Wend
With
End With

' Operators https://learn.microsoft.com/en-us/previous-versions/1cbft50a(v=vs.85)
Not And Or Xor Eqv Imp Is Mod

' Keywords https://learn.microsoft.com/en-us/previous-versions/f8tbc79x(v=vs.85)
Empty False Nothing Null True

' reserved types
Boolean Byte Currency Date Double Integer Long Single String Variant
' other types
Char Decimal LongLong LongPtr Object

' reserved words https://flylib.com/books/en/2.442.1/vbscript_basics.html
' https://isvbscriptdead.com/reserved-keywords/
As
EndIf Enum Event
Implements
Like LSet
Me
Optional
ParamArray
RaiseEvent RSet
Shared Static
Type TypeOf
' other words
Alias Attribute Begin
Compare Continue
Declare
Friend
Global GoSub
Lib Load
Module
Return
Unload
WithEvents

'! objects				=======================================================
' https://learn.microsoft.com/en-us/previous-versions/22h7a6k8(v=vs.85)
Debug {
	Write([str1 [, str2 [, ... [, strN]]]])
	WriteLine([str1 [, str2 [, ... [, strN]]]])
}

Err {
	Description
	HelpContext
	HelpFile
	Number
	Source

	Clear()
	Raise(number, source, description, helpfile, helpcontext)
}

RegExp {
	Global
	IgnoreCase
	Pattern

	Execute(string)
	Replace(string1, string2)
	Test(string)

	' Match Object
	FirstIndex
	Length
	Value
}

Dictionary {
	Count
	Item(key)
	Key(key)
	CompareMode

	Add(key, item)
	Exists(key)
	Items()
	Keys()
	Remove(key)
	RemoveAll()
}

FileSystemObject {
	Drives

	BuildPath(path, name)
	CopyFile(source, destination[, overwrite])
	CopyFolder(source, destination[, overwrite]);
	CreateFolder(foldername)
	CreateTextFile(filename[, overwrite[, unicode]])
	DeleteFile(filespec[, force]);
	DeleteFolder(folderspec[, force]);
	DriveExists(drivespec)
	FileExists(filespec)
	FolderExists(folderspec)
	GetAbsolutePathName(pathspec)
	GetBaseName(path)
	GetDrive(drivespec)
	GetDriveName(path)
	GetExtensionName(path)
	GetFile(filespec)
	GetFileVersion(pathspec)
	GetFileName(pathspec)
	GetFolder(folderspec)
	GetParentFolderName(path)
	GetSpecialFolder(folderspec)
	GetStandardStream(standardStreamType [, unicode])
	GetTempName()
	MoveFile(source, destination);
	MoveFolder(source, destination);
	OpenTextFile(filename[, iomode[, create[, format]]])
}

{ ' TextStream
	AtEndOfLine
	AtEndOfStream
	Column
	Line

	Close()
	Read(characters)
	ReadAll()
	ReadLine()
	Skip(characters)
	SkipLine()
	Write(string)
	WriteBlankLines(lines)
	WriteLine([string])
	OpenAsTextStream([iomode, [format]])
}

' Windows Script Host
' https://learn.microsoft.com/en-us/previous-versions/ms950396(v=msdn.10)
ActiveXObject {
}
WScript {
}


'! functions			=======================================================
' https://learn.microsoft.com/en-us/previous-versions/3ca8tfek(v=vs.85)
Abs(number)
Array(arglist)
Asc(string)
Atn(number)

CBool(expression)
CByte(expression)
CCur(expression)
CDate(date)
CDbl(expression)
Chr(charcode)
CInt(expression)
CLng(expression)
CSng(expression)
CStr(expression)
Hex(number)
Oct(number)
Cos(number)
CreateObject(servername.typename [, location])

Date
DateAdd(interval, number, date)
DateDiff(interval, date1, date2 [,firstdayofweek[, firstweekofyear]])
DatePart(interval, date[, firstdayofweek[, firstweekofyear]])
DateSerial(year, month, day)
DateValue(date)
Day(date)

Escape(charString)
Eval(expression)
Exp(number)

Filter(InputStrings, Value[, Include[, Compare]])
FormatCurrency(Expression[,NumDigitsAfterDecimal [,IncludeLeadingDigit [,UseParensForNegativeNumbers [,GroupDigits]]]])
FormatDateTime(Date[, NamedFormat])
FormatNumber(Expression [,NumDigitsAfterDecimal [,IncludeLeadingDigit [,UseParensForNegativeNumbers [,GroupDigits]]]])
FormatPercent(Expression[,NumDigitsAfterDecimal [,IncludeLeadingDigit [,UseParensForNegativeNumbers [,GroupDigits]]]])

GetLocale()
GetObject([pathname] [, class])
GetRef(procname)
Hour(time)

InputBox(prompt[, title][, default][, xpos][, ypos][, helpfile, context])
InStr([start, ]string1, string2[, compare])
InStrRev(string1, string2[, start[, compare]])
Int(number)
Fix(number)
IsArray(varname)
IsDate(expression)
IsEmpty(expression)
IsNull(expression)
IsNumeric(expression)
IsObject(expression)
Join(list[, delimiter])

LBound(arrayname[, dimension])
LCase(string)
Left(string, length)
Len(string | varname)
LoadPicture(picturename)
Log(number)
LTrim(string)
RTrim(string)
Trim(string)

Mid(string, start[, length])
Minute(time)
Month(date)
MonthName(month[, abbreviate])
MsgBox(prompt[, buttons][, title][, helpfile, context])
Now

Replace(expression, find, replacewith[, start[, count[, compare]]])
RGB(red, green, blue)
Right(string, length)
Rnd[(number)]
Round(expression[, numdecimalplaces])

ScriptEngine
ScriptEngineBuildVersion
ScriptEngineMajorVersion
ScriptEngineMinorVersion
Second(time)
SetLocale(lcid)
Sgn(number)
Sin(number)
Space(number)
Split(expression[, delimiter[, count[, compare]]])
Sqr(number)
StrComp(string1, string2[, compare])
String(number, character)
StrReverse(string1)
Tan(number)
Time
Timer
TimeSerial(hour, minute, second)
TimeValue(time)
TypeName(varname)

UBound(arrayname[, dimension])
UCase(string)
Unescape(charString)
VarType(varname)
Weekday(date, [firstdayofweek])
WeekdayName(weekday, abbreviate, firstdayofweek)
Year(date)

'! constants			=======================================================
' https://learn.microsoft.com/en-us/previous-versions/ydz4cfk3(v=vs.85)
' Color Constants
vbBlack vbRed vbGreen vbYellow vbBlue vbMagenta vbCyan vbWhite
' Comparison Constants
vbBinaryCompare vbTextCompare
' Date and Time Constants
vbSunday vbMonday vbTuesday vbWednesday vbThursday vbFriday vbSaturday
vbUseSystemDayOfWeek vbFirstJan1 vbFirstFourDays vbFirstFullWeek
' Date Format Constants
vbGeneralDate vbLongDate vbShortDate vbLongTime vbShortTime
' Miscellaneous Constants
vbObjectError
' MsgBox Constants
vbOKOnly vbOKCancel vbAbortRetryIgnore vbYesNoCancel vbYesNo vbRetryCancel
vbCritical vbQuestion vbExclamation vbInformation
vbDefaultButton1 vbDefaultButton2 vbDefaultButton3 vbDefaultButton4
vbApplicationModal vbSystemModal
vbOK vbCancel vbAbort vbRetry vbIgnore vbYes vbNo
' String Constants
vbCr vbCrLf vbFormFeed vbLf vbNewLine vbNullChar vbNullString vbTab vbVerticalTab
' Tristate Constants
vbUseDefault vbTrue vbFalse
' VarType Constants
vbEmpty vbNull vbInteger vbLong vbSingle vbDouble vbCurrency vbDate vbString
vbObject vbError vbBoolean vbVariant vbDataObject vbDecimal vbByte vbArray
