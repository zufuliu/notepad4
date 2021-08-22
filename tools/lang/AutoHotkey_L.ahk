; https://github.com/Lexikos/AutoHotkey_L-Docs.git
; AutoHotkey_L https://lexikos.github.io/v2/docs/AutoHotkey.htm

;! keywords			===========================================================
; https://lexikos.github.io/v2/docs/Concepts.htm#names
; https://lexikos.github.io/v2/docs/v2-changes.htm
as and
Contains
False
in is isset
not
or
True
unset

base
class
extends
get Global
Local
new
set super Static
this

;! Flow of Control	===========================================================
Break [LoopLabel]
Catch [ErrorClass] [as OutputVar]
Continue [LoopLabel]
Critical [OnOffNumeric]
Else Statement
Exit [ExitCode]
ExitApp [ExitCode]
Finally Statement
For Value1 [, Value2] in Expression
Goto Label
If (Expression)
Loop [Count]
Loop Files FilePattern [, Mode]
Loop Parse String [, Delimiters, OmitChars]
Loop Read InputFile [, OutputFile]
Loop Reg KeyName [, Mode]
OnError Function [, AddRemove]
OnExit Function [, AddRemove]
Pause [NewState]
Reload
Return [Expression]
SetTimer [Function, Period, Priority]
Sleep DelayInMilliseconds
Suspend [Mode]
Switch [SwitchValue, CaseSense]
	Case CaseValue1:
	Case CaseValue2a, CaseValue2b:
	Default:
Thread SubFunction [, Value1, Value2]
Throw [Value]
Try Statement
Until Expression
While Expression

;! Directives		===========================================================
#ClipboardTimeout Milliseconds
#DllLoad [FileOrDirName]
#ErrorStdOut [Encoding]
#HotIf [Expression]
#HotIfTimeout Timeout
#Hotstring NoMouse
#Hotstring EndChars NewChars
#Hotstring NewOptions
#Include FileOrDirName
#Include <LibName>
#IncludeAgain FileOrDirName
#InputLevel [Level]
#MaxThreads Value
#MaxThreadsBuffer [Setting]
#MaxThreadsPerHotkey Value
#NoTrayIcon
#Requires Requirement
#SingleInstance [Force Ignore Prompt Off]
#UseHook [Setting]
#Warn [WarningType, WarningMode]
#WinActivateForce

;! Script Compiler Directives
; https://lexikos.github.io/v2/docs/misc/Ahk2ExeDirectives.htm
/*@Ahk2Exe-Keep */
/*@Ahk2Exe-IgnoreBegin */
/*@Ahk2Exe-IgnoreEnd */
/*@Ahk2Exe-AddResource FileName , ResourceName */
/*@Ahk2Exe-Bin [Path\]Name , [Exe_path\][Name], Codepage */
/*@Ahk2Exe-ConsoleApp */
/*@Ahk2Exe-Cont Text */
/*@Ahk2Exe-Debug Text */
/*@Ahk2Exe-ExeName [Path\][Name] */
/*@Ahk2Exe-Let Name = Value , Name = Value, ... */
/*@Ahk2Exe-Obey Name, CmdOrExp , Extra */
/*@Ahk2Exe-PostExec Program [parameters] , When, WorkingDir, Hidden, IgnoreErrors */
/*@Ahk2Exe-SetMainIcon IcoFile */
/*@Ahk2Exe-SetProp Value */
/*@Ahk2Exe-Set Prop, Value */
/*@Ahk2Exe-UpdateManifest RequireAdmin , Name, Version, UIAccess */
/*@Ahk2Exe-UseResourceLang LangCode */

;! Built-in Variables	=======================================================
; https://lexikos.github.io/v2/docs/Variables.htm#BuiltIn
; Special Characters
A_Space
A_Tab
; Script Properties
A_Args
A_WorkingDir
A_InitialWorkingDir
A_ScriptDir
A_ScriptName
A_ScriptFullPath
A_ScriptHwnd
A_LineNumber
A_LineFile
A_ThisFunc
A_AhkVersion
A_AhkPath
A_IsCompiled
; Date and Time
A_YYYY
A_MM
A_DD
A_MMMM
A_MMM
A_DDDD
A_DDD
A_WDay
A_YDay
A_YWeek
A_Hour
A_Min
A_Sec
A_MSec
A_Now
A_NowUTC
A_TickCount
; Script Settings
A_IsSuspended
A_IsPaused
A_IsCritical
A_ListLines
A_TitleMatchMode
A_TitleMatchModeSpeed
A_DetectHiddenWindows
A_DetectHiddenText
A_FileEncoding
A_SendMode
A_SendLevel
A_StoreCapsLockMode
A_KeyDelay
A_KeyDuration
A_KeyDelayPlay
A_KeyDurationPlay
A_WinDelay
A_ControlDelay
A_MenuMaskKey
A_MouseDelay
A_MouseDelayPlay
A_DefaultMouseSpeed
A_CoordModeToolTip
A_CoordModePixel
A_CoordModeMouse
A_CoordModeCaret
A_CoordModeMenu
A_RegView
A_TrayMenu
A_AllowMainWindow
A_IconHidden
A_IconTip
A_IconFile
A_IconNumber
; User Idle Time
A_TimeIdle
A_TimeIdlePhysical
A_TimeIdleKeyboard
A_TimeIdleMouse
; Hotkeys, Hotstrings, and Custom Menu Items
A_ThisHotkey
A_PriorHotkey
A_PriorKey
A_TimeSinceThisHotkey
A_TimeSincePriorHotkey
A_EndChar
A_MaxHotkeysPerInterval
A_HotkeyInterval
A_HotkeyModifierTimeout
; Operating System and User Info
A_ComSpec
A_Temp
A_OSVersion
A_Is64bitOS
A_PtrSize
A_Language
A_ComputerName
A_UserName
A_WinDir
A_ProgramFiles
A_AppData
A_AppDataCommon
A_Desktop
A_DesktopCommon
A_StartMenu
A_StartMenuCommon
A_Programs
A_ProgramsCommon
A_Startup
A_StartupCommon
A_MyDocuments
A_IsAdmin
A_ScreenWidth
A_ScreenHeight
A_ScreenDPI
; Misc
A_Clipboard
A_Cursor
A_EventInfo
A_LastError
; Loop
A_Index
A_LoopFileName
A_LoopReadLine
A_LoopField
; https://lexikos.github.io/v2/docs/commands/LoopReg.htm
A_LoopRegName
A_LoopRegType
A_LoopRegKey
A_LoopRegTimeModified
; https://lexikos.github.io/v2/docs/commands/LoopFiles.htm
A_LoopFileName
A_LoopFileExt
A_LoopFilePath
A_LoopFileFullPath
A_LoopFileShortPath
A_LoopFileShortName
A_LoopFileDir
A_LoopFileTimeModified
A_LoopFileTimeCreated
A_LoopFileTimeAccessed
A_LoopFileAttrib
A_LoopFileSize
A_LoopFileSizeKB
A_LoopFileSizeMB

;! Keys				===========================================================
; https://lexikos.github.io/v2/docs/KeyList.htm
; Mouse
LButton RButton MButton
XButton1 XButton2
WheelDown WheelUp WheelLeft WheelRight
; Keyboard
; General Keys
CapsLock Space Tab Enter Escape Esc Backspace BS
; Cursor Control Keys
ScrollLock Delete Del Insert Ins Home End PgUp PgDn Up Down Left Right
; Numpad Keys
Numpad0 NumpadIns
Numpad1 NumpadEnd
Numpad2 NumpadDown
Numpad3 NumpadPgDn
Numpad4 NumpadLeft
Numpad5 NumpadClear
Numpad6 NumpadRight
Numpad7 NumpadHome
Numpad8 NumpadUp
Numpad9 NumpadPgUp
NumpadDot NumpadDel
NumLock
NumpadDiv
NumpadMult
NumpadAdd
NumpadSub
NumpadEnter
; Function Keys
; F1 - F24
; Modifier Keys
LWin
RWin
Control Ctrl
Alt
Shift
LControl LCtrl
RControl RCtrl
LShift RShift
LAlt RAlt
; Multimedia Keys
Browser_Back
Browser_Forward
Browser_Refresh
Browser_Stop
Browser_Search
Browser_Favorites
Browser_Home
Volume_Mute
Volume_Down
Volume_Up
Media_Next
Media_Prev
Media_Stop
Media_Play_Pause
Launch_Mail
Launch_Media
Launch_App1
Launch_App2
; Other Keys
AppsKey
PrintScreen
CtrlBreak
Pause
Help
Sleep
; SCnnn
; VKnn
; Joystick
; Joy1 - Joy32
JoyX JoyY JoyZ
JoyR JoyU JoyV
JoyPOV JoyName JoyButtons JoyAxes JoyInfo
; Multiple Joysticks

;! Functions		===========================================================
; Drive
; https://lexikos.github.io/v2/docs/commands/Drive.htm
DriveEject [Drive]
DriveRetract [Drive]
DriveGetCapacity(Path)
DriveGetFileSystem(Drive)
DriveGetLabel(Drive)
DriveGetList([Type])
DriveGetSerial(Drive)
DriveGetSpaceFree(Path)
DriveGetStatus(Path)
DriveGetStatusCD([Drive])
DriveLock Drive
DriveSetLabel Drive [, NewLabel]
DriveUnlock Drive
DriveGetType(Path)

; Environment
ClipboardAll([Data, Size])
ClipWait [Timeout, WaitForAnyData]
EnvGet(EnvVarName)
EnvSet EnvVar, Value
OnClipboardChange Function [, AddRemove]
SysGet(Property)
SysGetIPAddresses()

; External Libraries
CallbackCreate(Function [, Options := "", ParamCount := Function.MinParams])
DllCall("[DllFile\]Function" [, Type1, Arg1, Type2, Arg2, "Cdecl ReturnType"])
NumGet(Source, [Offset,] Type)
NumPut Type, Number, [Type2, Number2, ...] Target [, Offset]
StrGet(Source [, Length] [, Encoding := None])
StrPtr(Value)
StrPut String [, Encoding := None]
StrPut String, Target [, Length , Encoding := None]
VarSetStrCapacity(TargetVar [, RequestedCapacity])
; COM
ComObjActive(CLSID)
ComObjArray(VarType, Count1 [, Count2, ... Count8])
ComCall(Index, ComObj [, Type1, Arg1, Type2, Arg2, ReturnType])
ComObjConnect ComObj [, Prefix]
ComObjGet(Name)
ComObject(CLSID [, IID])
ComObjFlags(ComObj [, NewFlags, Mask])
ComObjFromPtr(DispPtr)
ComObjQuery(ComObj, [SID,] IID)
ComObjType(ComObj)
ComObjType(ComObj, "Name")
ComObjValue(ComObj)
ComValue(VarType, Value [, Flags])
ObjAddRef(Ptr)
ObjRelease(Ptr)

; File and Directory
DirCopy Source, Dest [, Overwrite]
DirCreate DirName
DirDelete DirName [, Recurse]
AttributeString := DirExist(FilePattern)
DirMove Source, Dest [, Flag]
SelectedFolder := DirSelect([StartingFolder, Options, Prompt])
FileAppend Text [, Filename, Options]
FileCopy SourcePattern, DestPattern [, Overwrite]
FileCreateShortcut Target, LinkFile [, WorkingDir, Args, Description, IconFile, ShortcutKey, IconNumber, RunState]
FileDelete FilePattern
FileEncoding [Encoding]
FileExist(FilePattern)
FileGetAttrib([Filename])
FileGetShortcut LinkFile [, OutTarget, OutDir, OutArgs, OutDescription, OutIcon, OutIconNum, OutRunState]
FileGetSize([Filename, Units])
FileGetTime([Filename, WhichTime])
FileGetVersion([Filename])
FileInstall Source, Dest [, Overwrite]
FileMove SourcePattern, DestPattern [, Overwrite]
FileOpen(Filename, Flags [, Encoding])
FileRead(Filename [, Options])
FileRecycle FilePattern
FileRecycleEmpty [DriveLetter]
FileSelect([Options, RootDir\Filename, Title, Filter])
FileSetAttrib Attributes [, FilePattern, Mode]
FileSetTime [YYYYMMDDHH24MISS, FilePattern, WhichTime, Mode]
IniDelete Filename, Section [, Key]
IniRead(Filename, Section, Key [, Default])
IniRead(Filename, Section)
IniRead(Filename)
IniWrite Value, Filename, Section, Key
IniWrite Pairs, Filename, Section
SetWorkingDir DirName
SplitPath Path [, OutFileName, OutDir, OutExtension, OutNameNoExt, OutDrive]

; GUI
DirSelect([StartingFolder, Options, Prompt])
FileSelect([Options, RootDir\Filename, Title, Filter])
GuiCtrlFromHwnd(Hwnd)
GuiFromHwnd(Hwnd [, RecurseParent := false])
InputBox([Prompt, Title, Options, Default])
LoadPicture(Filename [, Options, &ImageType])
MenuFromHandle(Handle)
MsgBox [Text, Title, Options]
MsgBox([Text, Title, Options])
OnMessage MsgNumber [, Function, MaxThreads]
ToolTip [Text, X, Y, WhichToolTip]
TraySetIcon([FileName, IconNumber, Freeze])
TrayTip [Text, Title, Options]

; Math
; https://lexikos.github.io/v2/docs/commands/Math.htm
Abs(Number)
Ceil(Number)
DateAdd(DateTime, Time, TimeUnits)
DateDiff(DateTime1, DateTime2, TimeUnits)
Exp(N)
Float(Value)
Floor(Number)
Integer(Value)
Log(Number)
Ln(Number)
Max(Number1 [, Number2, ...])
Min(Number1 [, Number2, ...])
Mod(Dividend, Divisor)
Number(Value)
Random([A, B])
Round(Number [, N])
Sqrt(Number)
Sin(Number)
Cos(Number)
Tan(Number)
ASin(Number)
ACos(Number)
ATan(Number)

; Monitor
; https://lexikos.github.io/v2/docs/commands/Monitor.htm
MonitorGet([N, Left, Top, Right, Bottom])
MonitorGetCount()
MonitorGetName([N])
MonitorGetPrimary()
MonitorGetWorkArea([N, Left, Top, Right, Bottom])

; Mouse and Keyboard
; https://lexikos.github.io/v2/docs/Hotkeys.htm
HotIf ["Expression"]
HotIf [Function]
Hotkey KeyName [, Action, Options]
Hotstring String [, Replacement, OnOffToggle]
Hotstring NewOptions
Hotstring("EndChars" [, NewValue])
Hotstring("MouseReset" [, NewValue])
Hotstring "Reset"
ListHotkeys
Suspend [Mode]
BlockInput OnOff
BlockInput SendMouse
BlockInput MouseMove
CaretGetPos([OutputVarX, OutputVarY])
Click [Options]
ControlClick [Control-or-Pos, WinTitle, WinText, WhichButton, ClickCount, Options, ExcludeTitle, ExcludeText]
ControlSend Keys [, Control, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlSendText Keys [, Control, WinTitle, WinText, ExcludeTitle, ExcludeText]
CoordMode TargetType [, RelativeTo]
GetKeyName(KeyName)
GetKeySC(KeyName)
GetKeyState(KeyName [, Mode])
GetKeyVK(KeyName)
KeyHistory [MaxEvents]
KeyWait KeyName [, Options]
InputHook([Options, EndKeys, MatchList])
InstallKeybdHook [Install, Force]
InstallMouseHook [Install, Force]
MouseClick [WhichButton, X, Y, ClickCount, Speed, DownOrUp, Relative]
MouseClickDrag WhichButton, X1, Y1, X2, Y2 [, Speed, Relative]
MouseGetPos [OutputVarX, OutputVarY, OutputVarWin, OutputVarControl, Flag]
MouseMove X, Y [, Speed, Relative]
; https://lexikos.github.io/v2/docs/commands/Send.htm
Send Keys
SendText Keys
SendInput Keys
SendPlay Keys
SendEvent Keys
SendLevel Level
SendMode Mode
SetCapsLockState [State]
SetNumLockState [State]
SetScrollLockState [State]
SetDefaultMouseSpeed Speed
SetKeyDelay [Delay, PressDuration, "Play"]
SetMouseDelay Delay [, "Play"]
SetStoreCapsLockMode Mode

; Misc
Download URL, Filename
Edit
GetMethod(Value [, Name, ParamCount])
HasBase(Value, BaseObj)
HasMethod(Value [, Name, ParamCount])
HasProp(Value, Name)
IsInteger(expression)
IsFloat(expression)
IsNumber(expression)
IsObject(ObjectValue)
IsLabel(LabelName)
IsSet(Var)
IsSetRef(Ref)
IsDigit(expression)
IsXDigit(expression)
IsAlpha(Value, "Locale")
IsUpper(Value, "Locale")
IsLower(Value, "Locale")
IsAlnum(expression)
IsSpace(expression)
IsTime(expression)
ListLines [Mode]
ListVars
OutputDebug Text
Persistent [Persist]

; Process
; https://lexikos.github.io/v2/docs/commands/Process.htm
ProcessClose PIDOrName
ProcessExist([PIDOrName])
ProcessSetPriority Level [, PIDOrName]
ProcessWait(PIDOrName [, Timeout])
ProcessWaitClose(PIDOrName [, Timeout])
Run Target [, WorkingDir, Options, OutputVarPID]
RunWait(Target [, WorkingDir, Options, OutputVarPID])
RunAs [User, Password, Domain]
Shutdown Flag

; Registry
RegDelete [KeyName, ValueName]
RegDeleteKey [KeyName]
RegRead([KeyName, ValueName, Default])
RegWrite Value, ValueType, KeyName [, ValueName]
RegWrite Value [, ValueType, , ValueName]
SetRegView RegView

; Screen
ImageSearch OutputVarX, OutputVarY, X1, Y1, X2, Y2, ImageFile
PixelGetColor(X, Y [, Mode])
PixelSearch OutputVarX, OutputVarY, X1, Y1, X2, Y2, ColorID [, Variation]

; Sound
; https://lexikos.github.io/v2/docs/commands/Sound.htm
SoundBeep [Frequency, Duration]
SoundGetInterface(IID [, Component, Device])
SoundGetMute([Component, Device])
SoundGetName([Component, Device])
SoundGetVolume([Component, Device])
SoundPlay Filename [, Wait]
SoundSetMute NewSetting [, Component, Device]
SoundSetVolume NewSetting [, Component, Device]

; String
Chr(Number)
Format(FormatStr [, Values...])
FormatTime([YYYYMMDDHH24MISS, Format])
InStr(Haystack, Needle [, CaseSense := false, StartingPos := 1, Occurrence := 1])
Ord(String)
RegExMatch(Haystack, NeedleRegEx [, OutputVar, StartingPos := 1])
RegExReplace(Haystack, NeedleRegEx [, Replacement := "", OutputVarCount := "", Limit := -1, StartingPos := 1])
Sort(String [, Options, Function])
StrCompare(String1, String2 [, CaseSense := false])
String(Value)
StrLower(String)
StrUpper(String)
StrTitle(String)
StrLen(String)
StrGet(Source [, Length] [, Encoding := None])
StrPut String [, Encoding := None]
StrPut String, Target [, Length] [, Encoding := None]
StrReplace(Haystack, Needle [, ReplaceText, CaseSense, OutputVarCount, Limit])
StrSplit(String [, Delimiters, OmitChars, MaxParts])
SubStr(String, StartingPos [, Length])
Trim(String [, OmitChars := " `t"])
LTrim(String [, OmitChars := " `t"])
RTrim(String [, OmitChars := " `t"])
VarSetStrCapacity(TargetVar [, RequestedCapacity])
VerCompare(VersionA, VersionB)

; Window
; https://lexikos.github.io/v2/docs/commands/Win.htm
GroupActivate(GroupName [, Mode])
GroupAdd GroupName [, WinTitle, WinText, ExcludeTitle, ExcludeText]
GroupClose GroupName [, Mode]
GroupDeactivate GroupName [, Mode]
DetectHiddenText Mode
DetectHiddenWindows Mode
SetTitleMatchMode MatchMode
SetTitleMatchMode Speed
SetWinDelay Delay
StatusBarGetText([Part#, WinTitle, WinText, ExcludeTitle, ExcludeText])
StatusBarWait [BarText, Timeout, Part#, WinTitle, WinText, Interval, ExcludeTitle, ExcludeText]
WinActivate [WinTitle, WinText, ExcludeTitle, ExcludeText]
WinActivateBottom [WinTitle, WinText, ExcludeTitle, ExcludeText]
WinActive([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinClose [WinTitle, WinText, SecondsToWait, ExcludeTitle, ExcludeText]
WinExist([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetClass([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetClientPos [X, Y, Width, Height, WinTitle, WinText, ExcludeTitle, ExcludeText]
WinGetControls([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetControlsHwnd([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetCount([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetID([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetIDLast([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetList([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetMinMax([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetPID([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetPos [X, Y, Width, Height, WinTitle, WinText, ExcludeTitle, ExcludeText]
WinGetProcessName([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetProcessPath([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetStyle([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetExStyle([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetText([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetTitle([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetTransColor([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinGetTransparent([WinTitle, WinText, ExcludeTitle, ExcludeText])
WinHide [WinTitle, WinText, ExcludeTitle, ExcludeText]
WinKill [WinTitle, WinText, SecondsToWait, ExcludeTitle, ExcludeText]
WinMaximize [WinTitle, WinText, ExcludeTitle, ExcludeText]
WinMinimize [WinTitle, WinText, ExcludeTitle, ExcludeText]
WinMinimizeAll
WinMinimizeAllUndo
WinMove X, Y [, Width, Height, WinTitle, WinText, ExcludeTitle, ExcludeText]
WinMoveBottom [WinTitle, WinText, ExcludeTitle, ExcludeText]
WinMoveTop [WinTitle, WinText, ExcludeTitle, ExcludeText]
WinRedraw [WinTitle, WinText, ExcludeTitle, ExcludeText]
WinRestore [WinTitle, WinText, ExcludeTitle, ExcludeText]
WinSetAlwaysOnTop [Value, WinTitle, WinText, ExcludeTitle, ExcludeText]
WinSetEnabled [Value , WinTitle, WinText, ExcludeTitle, ExcludeText]
WinSetRegion [Options, WinTitle, WinText, ExcludeTitle, ExcludeText]
WinSetStyle Value [, WinTitle, WinText, ExcludeTitle, ExcludeText]
WinSetExStyle Value [, WinTitle, WinText, ExcludeTitle, ExcludeText]
WinSetTitle NewTitle [, WinTitle, WinText, ExcludeTitle, ExcludeText]
WinSetTransColor Color [, WinTitle, WinText, ExcludeTitle, ExcludeText]
WinSetTransparent [N, WinTitle, WinText, ExcludeTitle, ExcludeText]
WinShow [WinTitle, WinText, ExcludeTitle, ExcludeText]
WinWait [WinTitle, WinText, Timeout, ExcludeTitle, ExcludeText]
WinWaitActive [WinTitle, WinText, Timeout, ExcludeTitle, ExcludeText]
WinWaitNotActive [WinTitle, WinText, Timeout, ExcludeTitle, ExcludeText]
WinWaitClose [WinTitle, WinText, Timeout, ExcludeTitle, ExcludeText]

; Control
; https://lexikos.github.io/v2/docs/commands/Control.htm
ControlAddItem String, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlChooseIndex N, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlChooseString String, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlClick [Control-or-Pos, WinTitle, WinText, WhichButton, ClickCount, Options, ExcludeTitle, ExcludeText]
ControlDeleteItem N, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlFindItem(String, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlFocus Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlGetChecked(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlGetChoice(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlGetClassNN(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlGetEnabled(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlGetFocus([WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlGetHwnd(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlGetIndex(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlGetItems(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlGetPos [X, Y, Width, Height, Control, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlGetStyle(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlGetExStyle(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlGetText(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlGetVisible(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
ControlHide Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlHideDropDown Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlMove [X, Y, Width, Height, Control, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlSend Keys [, Control, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlSendText Keys [, Control, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlSetChecked Value, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlSetEnabled Value, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlSetStyle Value, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlSetExStyle Value, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlSetText NewText, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlShow Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ControlShowDropDown Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
EditGetCurrentCol(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
EditGetCurrentLine(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
EditGetLine(N, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
EditGetLineCount(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
EditGetSelectedText(Control [, WinTitle, WinText, ExcludeTitle, ExcludeText])
EditPaste String, Control [, WinTitle, WinText, ExcludeTitle, ExcludeText]
ListViewGetContent([Options, Control, WinTitle, WinText, ExcludeTitle, ExcludeText])
MenuSelect WinTitle, WinText, Menu [, SubMenu1, SubMenu2, SubMenu3, SubMenu4, SubMenu5, SubMenu6, ExcludeTitle, ExcludeText]
PostMessage Msg [, wParam, lParam, Control, WinTitle, WinText, ExcludeTitle, ExcludeText]
SendMessage(Msg [, wParam, lParam, Control, WinTitle, WinText, ExcludeTitle, ExcludeText, Timeout])
SetControlDelay Delay

; Object Types
; https://lexikos.github.io/v2/docs/objects/index.htm
; https://lexikos.github.io/v2/docs/Objects.htm
__Enum(NumberOfVars)
__Item[name]
__New()
__Delete()
__Init()
__Get(Name, Params)
__Set(Name, Params, Value)
__Call(Name, Params)
Call(Params*)
; https://lexikos.github.io/v2/docs/objects/Any.htm
GetMethod(Name)
HasBase(BaseObj)
HasMethod(Name)
HasProp(Name)
ObjGetBase(Value)
; https://lexikos.github.io/v2/docs/objects/Object.htm
Clone()
DefineProp(Name, Desc)
DeleteProp(Name)
GetOwnPropDesc(Name)
HasOwnProp(Name)
OwnProps()
ObjSetBase(Obj, BaseObj)
ObjOwnPropCount(Obj)
ObjSetCapacity(Obj, MaxProps)
ObjGetCapacity(Obj)
; https://lexikos.github.io/v2/docs/objects/Array.htm
Delete(Index)
Has(Index)
InsertAt(Index, Value1 [, Value2, ... ValueN])
Pop()
Push([Value, Value2, ..., ValueN])
RemoveAt(Index [, Length])
; https://lexikos.github.io/v2/docs/objects/Map.htm
Clear()
Delete(Key)
Get(Key [, Default])
Has(Key)
Set([Key, Value, Key2, Value2, ...])
; https://lexikos.github.io/v2/docs/objects/File.htm
Read([Characters])
Write(String)
ReadLine()
WriteLine([String])
ReadNumType()
WriteNumType(Num)
RawRead(Buffer [, Bytes])
RawWrite(Data [, Bytes])
Seek(Distance [, Origin := 0])
Close()
; https://lexikos.github.io/v2/docs/objects/Func.htm
Bind(Param1, Param2, ...)
IsByRef([ParamIndex])
IsOptional([ParamIndex])

; https://lexikos.github.io/v2/docs/objects/Gui.htm
Add(ControlType [, Options, Text])
AddControlType([Options, Text])
Destroy()
Flash([Blink := true])
GetClientPos([X, Y, Width, Height])
GetPos([X, Y, Width, Height])
Hide()
Maximize()
Minimize()
Move([X, Y, Width, Height])
Restore()
OnEvent(EventName, Callback [, AddRemove := 1])
Opt(Options)
SetFont([Options, FontName])
Show([Options])
Submit([Hide := true])
; https://lexikos.github.io/v2/docs/objects/GuiControl.htm
Add(Items)
Choose(Value)
Delete([Value])
Focus()
GetPos([X, Y, Width, Height])
Move([X, Y, Width, Height])
OnEvent(EventName, Callback [, AddRemove := 1])
OnNotify(NotifyCode, Callback [, AddRemove := 1])
Opt(Options)
Redraw()
SetFont([Options, FontName])
SetFormat([Format])
UseTab([Value, ExactMatch := false])

; https://lexikos.github.io/v2/docs/objects/Menu.htm
Add([MenuItemName, Function-or-Submenu, Options])
Check(MenuItemName)
Delete([MenuItemName])
Disable(MenuItemName)
Enable(MenuItemName)
Insert([ItemToInsertBefore, NewItemName, Callback-or-Submenu, Options])
Rename(MenuItemName [, NewName])
SetColor([ColorValue, Submenus := true])
SetIcon(MenuItemName, FileName [, IconNumber, IconWidth])
Show([X, Y])
ToggleCheck(MenuItemName)
ToggleEnable(MenuItemName)
Uncheck(MenuItemName)
AddStandard()

;! Objects			===========================================================
; https://lexikos.github.io/v2/docs/objects/index.htm
Any
	Object
		Array
		Buffer
			ClipboardAll
		Class
		Error
			IndexError
				KeyError
			MemberError
				PropertyError
				MethodError
			MemoryError
			OSError
			TargetError
			TimeoutError
			TypeError
			ValueError
			ZeroDivisionError
		File
		Func
			BoundFunc
			Closure
			Enumerator
		Gui
		Gui.Control
			Gui.ActiveX
			Gui.Button
			Gui.CheckBox
			Gui.Custom
			Gui.DateTime
			Gui.Edit
			Gui.GroupBox
			Gui.Hotkey
			Gui.Link
			Gui.List
				Gui.ComboBox
				Gui.DDL
				Gui.ListBox
				Gui.Tab
			Gui.ListView
			Gui.MonthCal
			Gui.Pic
			Gui.Progress
			Gui.Radio
			Gui.Slider
			Gui.StatusBar
			Gui.Text
			Gui.TreeView
			Gui.UpDown
		InputHook
		Map
		Menu
			MenuBar
		RegExMatchInfo
	Primitive
		Number
			Float
			Integer
		String
	VarRef
	ComValue
		ComObjArray
		ComObject
		ComValueRef

;! Misc				===========================================================
; Properties
; Array
Length Capacity
; Buffer
Ptr Size
; Map
Count Capacity CaseSense Default
; File
Pos Length AtEOF Encoding Handle
; Func
Name IsBuiltIn IsVariadic MinParams MaxParams
; Gui
BackColor FocusedCtrl Hwnd MarginX MarginY MenuBar Name Title
; GuiControl
ClassNN Enabled Focused Gui Hwnd Name Text Type Value Visible
; Menu/MenuBar
ClickCount Default Handle
; Match
Pos Len Name Count Mark

; Continuation section
; https://lexikos.github.io/v2/docs/Scripts.htm#continuation-section
Join LTrim RTrim Comments Comment Com C
; Loop
Files Parse Read Reg
; Loop Reg
; https://lexikos.github.io/v2/docs/commands/LoopReg.htm
HKEY_LOCAL_MACHINE HKEY_USERS HKEY_CURRENT_USER HKEY_CLASSES_ROOT HKEY_CURRENT_CONFIG
HKLM HKCU
REG_SZ REG_EXPAND_SZ REG_MULTI_SZ REG_DWORD REG_QWORD REG_BINARY REG_LINK REG_RESOURCE_LIST REG_FULL_RESOURCE_DESCRIPTOR REG_RESOURCE_REQUIREMENTS_LIST REG_DWORD_BIG_ENDIAN
