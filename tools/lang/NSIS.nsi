; 3.09 https://nsis.sourceforge.io/Main_Page
; https://nsis.sourceforge.io/Docs/

;! keywords			===========================================================
Page
UninstPage
PageEx
PageExEnd
SectionGroup
Section
SectionEnd
SectionGroupEnd
Function
FunctionEnd
Var
Goto
Return

;! preprocessor		===========================================================
; https://nsis.sourceforge.io/Docs/Chapter5.html#
; Compiler Utility Commands
!include
!addincludedir
!addplugindir
!appendfile
!cd
!delfile
!echo
!error
!assert
!execute
!makensis
!packhdr
!finalize
!uninstfinalize
!system
!tempfile
!getdllversion
!gettlbversion
!warning
!pragma
!verbose

; Conditional Compilation
!define
!undef
!ifdef
!endif
!ifndef
!endif
!if
!endif
!ifmacrodef
!endif
!ifmacrondef
!else
!endif
!insertmacro
!macro
!macroend
!macroundef
!searchparse
!searchreplace

;! instructions		===========================================================
PageCallbacks
; Section Commands
AddSize
InstType
SectionInstType
SectionIn

; https://nsis.sourceforge.io/Docs/Chapter4.html#instr
; Basic Instructions
Delete
Exec
ExecShell
ExecShellWait
ExecWait
File
Rename
ReserveFile
RMDir
SetOutPath

; Registry, INI, File Instructions
DeleteINISec
DeleteINIStr
DeleteRegKey
DeleteRegValue
EnumRegKey
EnumRegValue
ExpandEnvStrings
FlushINI
ReadEnvStr
ReadINIStr
ReadRegDWORD
ReadRegStr
WriteINIStr
WriteRegBin
WriteRegNone
WriteRegDWORD
WriteRegStr
WriteRegExpandStr
WriteRegMultiStr
SetRegView

; General Purpose Instructions
CallInstDLL
CopyFiles
CreateDirectory
CreateShortcut
GetWinVer
GetDLLVersion
GetDLLVersionLocal
GetFileTime
GetFileTimeLocal
GetKnownFolderPath
GetFullPathName
GetTempFileName
SearchPath
SetFileAttributes
RegDLL
UnRegDLL

; Flow Control Instructions
Abort
Call
ClearErrors
GetCurrentAddress
GetFunctionAddress
GetLabelAddress
Goto
IfAbort
IfErrors
IfFileExists
IfRebootFlag
IfSilent
IfShellVarContextAll
IfRtlLanguage
IntCmp
IntCmpU
Int64Cmp
Int64CmpU
IntPtrCmp
IntPtrCmpU
MessageBox
Return
Quit
SetErrors
StrCmp
StrCmpS

; File Instructions
FileClose
FileOpen
FileRead
FileReadUTF16LE
FileReadByte
FileReadWord
FileSeek
FileWrite
FileWriteUTF16LE
FileWriteByte
FileWriteWord
FindClose
FindFirst
FindNext

; Uninstaller Instructions
WriteUninstaller

; Miscellaneous Instructions
GetErrorLevel
GetInstDirError
InitPluginsDir
Nop
SetErrorLevel
SetShellVarContext
Sleep

; String Manipulation Instructions
StrCpy
StrLen

; Stack Support
Exch
Pop
Push

; Integer Support
IntFmt
Int64Fmt
IntOp
IntPtrOp

; Reboot Instructions
Reboot
SetRebootFlag

; Install Logging Instructions
LogSet
LogText

; Section Management
SectionSetFlags
SectionGetFlags
SectionSetText
SectionGetText
SectionSetInstTypes
SectionGetInstTypes
SectionSetSize
SectionGetSize
SetCurInstType
GetCurInstType
InstTypeSetText
InstTypeGetText

; User Interface Instructions
BringToFront
CreateFont
DetailPrint
EnableWindow
FindWindow
GetDlgItem
HideWindow
IsWindow
LoadAndSetImage
LockWindow
SendMessage
SetAutoClose
SetBrandingImage
SetDetailsView
SetDetailsPrint
SetCtlColors
SetSilent
ShowWindow

; Multiple Languages Instructions
LoadLanguageFile
LangString
LicenseLangString

;! predefined variables	=======================================================
; https://nsis.sourceforge.io/Docs/Chapter4.html#varconstant
$PROGRAMFILES
$PROGRAMFILES32
$PROGRAMFILES64
$DESKTOP
$EXEDIR
$EXEFILE
$EXEPATH
${NSISDIR}
$WINDIR
$SYSDIR
$TEMP
$STARTMENU
$SMPROGRAMS
$SMSTARTUP
$QUICKLAUNCH
$DOCUMENTS
$SENDTO
$RECENT
$FAVORITES
$MUSIC
$PICTURES
$VIDEOS
$NETHOOD
$FONTS
$TEMPLATES
$APPDATA
$LOCALAPPDATA
$PRINTHOOD
$INTERNET_CACHE
$COOKIES
$HISTORY
$PROFILE
$ADMINTOOLS
$RESOURCES
$RESOURCES_LOCALIZED
$CDBURN_AREA
$HWNDPARENT
$PLUGINSDIR

; https://nsis.sourceforge.io/Docs/Chapter5.html#comppredefines
${__COUNTER__}
${__FILE__}
${__FILEDIR__}
${__LINE__}
${__DATE__}
${__TIME__}
${__TIMESTAMP__}
${NSIS_VERSION}
${NSIS_PACKEDVERSION}
${NSIS_CHAR_SIZE}
${NSIS_PTR_SIZE}

;! functions		===========================================================
; Callback Functions
; https://nsis.sourceforge.io/Docs/Chapter4.html#callbacks
.onGUIInit
.onInit
.onInstFailed
.onInstSuccess
.onGUIEnd
.onMouseOverSection
.onRebootFailed
.onSelChange
.onUserAbort
.onVerifyInstDir
un.onGUIInit
un.onInit
un.onUninstFailed
un.onUninstSuccess
un.onGUIEnd
un.onRebootFailed
un.onSelChange
un.onUserAbort

; https://nsis.sourceforge.io/Docs/AppendixE.html#headers
; File Functions Header
Locate
GetSize
DriveSpace
GetDrives
GetTime
GetFileAttributes
GetFileVersion
GetExeName
GetExePath
GetParameters
GetOptions
GetOptionsS
GetRoot
GetParent
GetFileName
GetBaseName
GetFileExt
BannerTrimPath
DirState
RefreshShellIcons

; Text Functions Header
LineFind
LineRead
FileReadFromEnd
LineSum
FileJoin
TextCompare
TextCompareS
ConfigRead
ConfigReadS
ConfigWrite
ConfigWriteS
FileRecode
TrimNewLines

; Word Functions Header
WordFind
WordFindS
WordFind2X
WordFind2XS
WordFind3X
WordFind3XS
WordReplace
WordReplaceS
WordAdd
WordAddS
WordInsert
WordInsertS
StrFilter
StrFilterS
VersionCompare
VersionConvert

;! attributes		===========================================================
; https://nsis.sourceforge.io/Docs/Chapter4.html#instattribs
; General Attributes
AddBrandingImage
AllowRootDirInstall
AutoCloseWindow
BGFont
BGGradient
BrandingText
Caption
ChangeUI
CheckBitmap
CompletedText
ComponentText
CRCCheck
DetailsButtonText
DirText
DirVar
DirVerify
FileErrorText
Icon
InstallButtonText
InstallColors
InstallDir
InstallDirRegKey
InstProgressFlags
InstType
LicenseBkColor
LicenseData
LicenseForceSelection
LicenseText
ManifestDPIAware
ManifestLongPathAware
ManifestSupportedOS
MiscButtonText
Name
OutFile
PEAddResource
PERemoveResource
RequestExecutionLevel
SetFont
ShowInstDetails
ShowUninstDetails
SilentInstall
SilentUnInstall
SpaceTexts
SubCaption
UninstallButtonText
UninstallCaption
UninstallIcon
UninstallSubCaption
UninstallText
WindowIcon
XPStyle

; Compiler Flags
AllowSkipFiles
FileBufSize
SetCompress
SetCompressor
SetCompressorDictSize
SetDatablockOptimize
SetDateSave
SetOverwrite
Unicode

; Version Information
VIAddVersionKey
VIProductVersion
VIFileVersion
