; 6.2 https://jrsoftware.org/isinfo.php
; https://jrsoftware.org/ishelp/
; https://jrsoftware.org/ispphelp/
[ISPP]
//! directives		===========================================================
#preproc
#define
#dim
#redim
#undef
#include
#file
#emit
#expr
#insert
#append
#if
#elif
#else
#endif
#ifdef
#endif
#ifndef
#endif
#ifexist
#endif
#ifnexist
#endif
#for
#sub
#endsub
#pragma
#error

//! keywords		===========================================================
private protected public
// Components and Tasks Parameters
and or not

//! types			===========================================================
any int str func void

//! Predefined Variables	===================================================
__COUNTER__
__FILE__
__INCLUDE__
__LINE__
__OPT_X__
__WIN32__
ISPP_INVOKED
ISPPCC_INVOKED
PREPROCVER
WINDOWS
UNICODE
CompilerPath
SourcePath
Ver
NewLine
Tab

// others from ISPPBuiltins.iss
True
False
Yes
No
MaxInt
MinInt
NULL
// TypeOf constants
TYPE_ERROR
TYPE_NULL
TYPE_INTEGER
TYPE_STRING
TYPE_MACRO
TYPE_FUNC
TYPE_ARRAY
// ReadReg constants
HKCR			HKEY_CLASSES_ROOT
HKCU			HKEY_CURRENT_USER
HKLM			HKEY_LOCAL_MACHINE
HKU				HKEY_USERS
HKCC			HKEY_CURRENT_CONFIG
HKCR64			HKEY_CLASSES_ROOT_64
HKCU64			HKEY_CURRENT_USER_64
HKLM64			HKEY_LOCAL_MACHINE_64
HKU64			HKEY_USERS_64
HKCC64			HKEY_CURRENT_CONFIG_64
// Exec constants
SW_HIDE
SW_SHOWNORMAL
SW_NORMAL
SW_SHOWMINIMIZED
SW_SHOWMAXIMIZED
SW_MAXIMIZE
SW_SHOWNOACTIVATE
SW_SHOW
SW_MINIMIZE
SW_SHOWMINNOACTIVE
SW_SHOWNA
SW_RESTORE
SW_SHOWDEFAULT
SW_MAX
// Find constants
FIND_MATCH
FIND_BEGINS
FIND_ENDS
FIND_CONTAINS
FIND_CASESENSITIVE
FIND_SENSITIVE
FIND_AND
FIND_OR
FIND_NOT
FIND_TRIM
// FindFirst constants
faReadOnly
faHidden
faSysFile
faVolumeID
faDirectory
faArchive
faSymLink
faAnyFile
// GetStringFileInfo standard names
COMPANY_NAME
FILE_DESCRIPTION
FILE_VERSION
INTERNAL_NAME
LEGAL_COPYRIGHT
ORIGINAL_FILENAME
PRODUCT_NAME
PRODUCT_VERSION

//! functions		===========================================================
str GetStringFileInfo(str 1, str 2, int? 3)
int Int(any 1, int? 2)
str Str(any)
int FileExists(str)
int DirExists(str)
int ForceDirectories(str)
int FileSize(str)
str ReadIni(str 1, str 2, str 3, str? 4)
void WriteIni(str 1, str 2, str 3, any 4)
str ReadReg(int 1, str 2, str? 3, any? 4)
int Exec(str 1, str? 2, str? 3, int? 4, int? 5)
str Copy(str 1, int 2, int? 3)
int Pos(str 1, str 2)
int RPos(str 1, str 2)
int Len(str)
void SaveToFile(str)
int Find(int 1, str 2, int? 3, str? 4, int? 5, str? 6, int? 7)
str SetupSetting(str)
void SetSetupSetting(str 1, str 2)
str LowerCase(str)
str UpperCase(str)
int EntryCount(str)
str GetEnv(str)
void DeleteFile(str)
void DeleteFileNow(str)
void CopyFile(str 1, str 2)
int FindFirst(str, int)
int FindNext(int)
void FindClose(int)
str FindGetFileName(int)
int FileOpen(str)
str FileRead(int)
void FileReset(int)
int FileEof(int)
void FileClose(int)
int SaveStringToFile(str Filename, str S, int? Append, int? UTF8)
str GetDateTimeString(str, str, str)
str GetFileDateTimeString(str, str, str, str)
str GetMD5OfString(str)
str GetMD5OfUnicodeString(str)
str GetMD5OfFile(str)
str GetSHA1OfString(str)
str GetSHA1OfUnicodeString(str)
str GetSHA1OfFile(str)
str Trim(str)
str StringChange(str, str, str)
int IsWin64()
int Defined(<ident>)
int defined(<ident>)
TypeOf2(any Expr)
int TypeOf(<ident>)
int DimOf(<ident>)
str GetVersionNumbers(str Filename, int *VersionMS, int *VersionLS)
str GetVersionComponents(str Filename, int *Major, int *Minor, int *Revision, int *Build)
str GetVersionNumbersString(str Filename)
str GetPackedVersion(str Filename, int *Version)
int PackVersionNumbers(int VersionMS, int VersionLS)
int PackVersionComponents(int Major, int Minor, int Revision, int Build)
int ComparePackedVersion(int Version1, int Version2)
int SamePackedVersion(int Version1, int Version2)
void UnpackVersionNumbers(int Version, int *VersionMS, int *VersionLS)
void UnpackVersionComponents(int Version, int *Major, int *Minor, int *Revision, int *Build)
void VersionToStr(int Version)
int StrToVersion(str Version)
int EncodeVer(int Major, int Minor, int Revision = 0, int Build = -1)
str DecodeVer(int Version, int Digits = 3)
int FindSection(str Section = "Files")
int FindSectionEnd(str Section = "Files")
int FindCode()
str ExtractFilePath(str PathName)
str ExtractFileDir(str PathName)
str ExtractFileExt(str PathName)
str ExtractFileName(str PathName)
str ChangeFileExt(str FileName, str NewExt)
void RemoveFileExt(str FileName)
str AddBackslash(str S)
str RemoveBackslash(str S)
void Delete(str *S, int Index, int Count = MaxInt)
void Insert(str *S, int Index, str Substr)
int YesNo(str S)
int Power(int X, int P = 2)
int Min(int A, int B, int C = MaxInt)
int Max(int A, int B, int C = MinInt)
int SameText(str S1, str S2)
int SameStr(str S1, str S2)
void Message(str S)
void Warning(str S)
void Error(str S)

// others from ISPPBuiltins.iss
GetFileCompany(str FileName)
GetFileDescription(str FileName)
GetFileVersionString(str FileName)
GetFileCopyright(str FileName)
GetFileOriginalFilename(str FileName)
GetFileProductVersion(str FileName)
DeleteToFirstPeriod(str *S)
FindNextSection(int Line)
IsDirSet(str SetupDirective)
WarnRenamedVersion(str OldName, str NewName)
ParseVersion(str FileName, *Major, *Minor, *Rev, *Build)
GetFileVersion(str FileName)

[Misc]
; pragma options
option
parseroption
inlinestart
inlineend
message
warning
error
verboselevel
include
spansymbol

[Constants]
; Directory Constants
{app}
{win}
{sys}
{sysnative}
{syswow64}
{src}
{sd}
{commonpf}
{commonpf32}
{commonpf64}
{commoncf}
{commoncf32}
{commoncf64}
{tmp}
{commonfonts}
{dao}
{dotnet40}
{dotnet4032}
{dotnet4064}

; Shell Folder Constants
{group}
{localappdata}
{userappdata}
{commonappdata}
{usercf}
{userdesktop}
{commondesktop}
{userdocs}
{commondocs}
{userfavorites}
{userfonts}
{userpf}
{userprograms}
{commonprograms}
{usersavedgames}
{usersendto}
{userstartmenu}
{commonstartmenu}
{userstartup}
{commonstartup}
{usertemplates}
{commontemplates}

; Auto Constants
autoappdata commonappdata userappdata
autocf commoncf usercf
autocf32 commoncf32 usercf
autocf64 commoncf64 usercf
autodesktop commondesktop userdesktop
autodocs commondocs userdocs
autofonts commonfonts userfonts
autopf commonpf userpf
autopf32 commonpf32 userpf
autopf64 commonpf64 userpf
autoprograms commonprograms userprograms
autostartmenu commonstartmenu userstartmenu
autostartup commonstartup userstartup
autotemplates commontemplates usertemplates
; Renamed Constants
cf commoncf
cf32 commoncf32
cf64 commoncf64
fonts commonfonts
pf commonpf
pf32 commonpf32
pf64 commonpf64
sendto usersendto

; Other Constants
{cmd}
{computername}
{drive:Path}
{groupname}
{hwnd}
{wizardhwnd}
{ini:Filename,Section,Key|DefaultValue}
{language}
{cm:MessageName}
{cm:MessageName,Arguments}
{reg:HKxx\SubkeyName,ValueName|DefaultValue}
{param:ParamName|DefaultValue}
{srcexe}
{uninstallexe}
{sysuserinfoname}
{sysuserinfoorg}
{userinfoname}
{userinfoorg}
{userinfoserial}
{username}
{log}
{code:FunctionName|Param}


[Setup]
; Common Parameters
Check:
Languages:
MinVersion:
OnlyBelowVersion:

; Compiler-related
ASLRCompatible=
Compression=
CompressionThreads=
DEPCompatible=
DiskClusterSize=
DiskSliceSize=
DiskSpanning=
Encryption=
InternalCompressLevel=
LZMAAlgorithm=
LZMABlockSize=
LZMADictionarySize=
LZMAMatchFinder=
LZMANumBlockThreads=
LZMANumFastBytes=
LZMAUseSeparateProcess=
MergeDuplicateFiles=
MissingMessagesWarning=
MissingRunOnceIdsWarning=
NotRecognizedMessagesWarning=
Output=
OutputBaseFilename=
OutputDir=
OutputManifestFile=
ReserveBytes=
SignedUninstaller=
SignedUninstallerDir=
SignTool=
SignToolMinimumTimeBetween=
SignToolRetryCount=
SignToolRetryDelay=
SignToolRunMinimized=
SlicesPerDisk=
SolidCompression=
SourceDir=
TerminalServicesAware=
UsedUserAreasWarning=
UseSetupLdr=
VersionInfoCompany=
VersionInfoCopyright=
VersionInfoDescription=
VersionInfoOriginalFileName=
VersionInfoProductName=
VersionInfoProductTextVersion=
VersionInfoProductVersion=
VersionInfoTextVersion=
VersionInfoVersion=

; Installer-related
AllowCancelDuringInstall=
AllowNetworkDrive=
AllowNoIcons=
AllowRootDirectory=
AllowUNCPath=
AlwaysRestart=
AlwaysShowComponentsList=
AlwaysShowDirOnReadyPage=
AlwaysShowGroupOnReadyPage=
AlwaysUsePersonalGroup=
AppendDefaultDirName=
AppendDefaultGroupName=
AppComments=
AppContact=
AppId=
AppModifyPath=
AppMutex=
AppName=
AppPublisher=
AppPublisherURL=
AppReadmeFile=
AppSupportPhone=
AppSupportURL=
AppUpdatesURL=
AppVerName=
AppVersion=
ArchitecturesAllowed=
ArchitecturesInstallIn64BitMode=
ChangesAssociations=
ChangesEnvironment=
CloseApplications=
CloseApplicationsFilter=
CreateAppDir=
CreateUninstallRegKey=
DefaultDialogFontName=
DefaultDirName=
DefaultGroupName=
DefaultUserInfoName=
DefaultUserInfoOrg=
DefaultUserInfoSerial=
DirExistsWarning=
DisableDirPage=
DisableFinishedPage=
DisableProgramGroupPage=
DisableReadyMemo=
DisableReadyPage=
DisableStartupPrompt=
DisableWelcomePage=
EnableDirDoesntExistWarning=
ExtraDiskSpaceRequired=
InfoAfterFile=
InfoBeforeFile=
LanguageDetectionMethod=
LicenseFile=
MinVersion=
OnlyBelowVersion=
Password=
PrivilegesRequired=
PrivilegesRequiredOverridesAllowed=
RestartApplications=
RestartIfNeededByRun=
SetupLogging=
SetupMutex=
ShowLanguageDialog=
TimeStampRounding=
TimeStampsInUTC=
TouchDate=
TouchTime=
Uninstallable=
UninstallDisplayIcon=
UninstallDisplayName=
UninstallDisplaySize=
UninstallFilesDir=
UninstallLogMode=
UninstallRestartComputer=
UpdateUninstallLogAppName=
UsePreviousAppDir=
UsePreviousGroup=
UsePreviousLanguage=
UsePreviousPrivigeles=
UsePreviousSetupType=
UsePreviousTasks=
UsePreviousUserInfo=
UserInfoPage=

; Cosmetic
AppCopyright=
BackColor=
BackColor2=
BackColorDirection=
BackSolid=
FlatComponentsList=
SetupIconFile=
ShowComponentSizes=
ShowTasksTreeLines=
WindowShowCaption=
WindowStartMaximized=
WindowResizable=
WindowVisible=
WizardImageAlphaFormat=
WizardImageFile=
WizardImageStretch=
WizardResizable=
WizardSizePercent=
WizardSmallImageFile=
WizardStyle=

[Types]
Name:
Description:
Flags:

[Components]
Name:
Description:
Types:
ExtraDiskSpaceRequired:
Flags:

[Tasks]
Name:
Description:
GroupDescription:
Components:
Flags:

[Dirs]
Name:
Attribs:
Permissions:
Flags:

[Files]
Source:
DestDir:
DestName:
Excludes:
ExternalSize:
CopyMode:
Attribs:
Permissions:
FontInstall:
StrongAssemblyName:
Flags:

[Icons]
Name:
Filename:
Parameters:
WorkingDir:
HotKey:
Comment:
IconFilename:
IconIndex:
AppUserModelID:
AppUserModelToastActivatorCLSID:
Flags:

[INI]
Filename:
Section:
Key:
String:
Flags:

[InstallDelete]
Type:
Name:

[Languages]
Name:
MessagesFile:
LicenseFile:
InfoBeforeFile:
InfoAfterFile:

[Messages]
BeveledLabel=
HelpTextNote=

[CustomMessages]
NameAndVersion=
AdditionalIcons=
CreateDesktopIcon=
CreateQuickLaunchIcon=
ProgramOnTheWeb=
UninstallProgram=
LaunchProgram=
AssocFileExtension=
AssocingFileExtension=
AutoStartProgramGroupDescription=
AutoStartProgram=
AddonHostProgramNotFound=

[LangOptions]
LanguageName=
LanguageID=
LanguageCodePage=
DialogFontName=
DialogFontSize=
WelcomeFontName=
WelcomeFontSize=
TitleFontName=
TitleFontSize=
CopyrightFontName=
CopyrightFontSize=
RightToLeft=

[Registry]
Root:
Subkey:
ValueName:
ValueData:
Permissions:
Flags:

[Run]
Filename:
Description:
Parameters:
WorkingDir:
StatusMsg:
RunOnceId:
Verb:
Flags:

[UninstallDelete]
Type:
Name:

[UninstallRun]
Filename:
Description:
Parameters:
WorkingDir:
StatusMsg:
RunOnceId:
Verb:
Flags:

[Code]
//! keywords		===========================================================
and
break
const continue constructor
do downto
else event except external
finally for function
if
not
of or
procedure property
read repeat
then to type
until uses
var
while with write

begin
end
case
end
class
end
interface
end
record
end
try
end

//! event			===========================================================
// Setup event functions
function InitializeSetup(): Boolean;
procedure InitializeWizard();
procedure DeinitializeSetup();
procedure CurStepChanged(CurStep: TSetupStep);
procedure CurInstallProgressChanged(CurProgress, MaxProgress: Integer);
function NextButtonClick(CurPageID: Integer): Boolean;
function BackButtonClick(CurPageID: Integer): Boolean;
procedure CancelButtonClick(CurPageID: Integer; var Cancel, Confirm: Boolean);
function ShouldSkipPage(PageID: Integer): Boolean;
procedure CurPageChanged(CurPageID: Integer);
function CheckPassword(Password: String): Boolean;
function NeedRestart(): Boolean;
function UpdateReadyMemo(Space, NewLine, MemoUserInfoInfo, MemoDirInfo, MemoTypeInfo, MemoComponentsInfo, MemoGroupInfo, MemoTasksInfo: String): String;
procedure RegisterPreviousData(PreviousDataKey: Integer);
function CheckSerial(Serial: String): Boolean;
function GetCustomSetupExitCode: Integer;
function PrepareToInstall(var NeedsRestart: Boolean): String;
procedure RegisterExtraCloseApplicationsResources;
// Uninstall event functions
function InitializeUninstall(): Boolean;
procedure InitializeUninstallProgressForm();
procedure DeinitializeUninstall();
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
function UninstallNeedRestart(): Boolean;

//! functions		===========================================================
// Setup or Uninstall Info functions
function GetCmdTail: String;
function ParamCount: Integer;
function ParamStr(Index: Integer): String;
function ActiveLanguage: String;
function CustomMessage(const MsgName: String): String;
function FmtMessage(const S: String; const Args: array of String): String;
function SetupMessage(const ID: TSetupMessageID): String;
function WizardDirValue: String;
function WizardGroupValue: String;
function WizardNoIcons: Boolean;
function WizardSetupType(const Description: Boolean): String;
function WizardSelectedComponents(const Descriptions: Boolean): String;
function WizardIsComponentSelected(const Components: String): Boolean;
procedure WizardSelectComponents(const Components: String);
function WizardSelectedTasks(const Descriptions: Boolean): String;
function WizardIsTaskSelected(const Tasks: String): Boolean;
procedure WizardSelectTasks(const Tasks: String);
function WizardSilent: Boolean;
function IsUninstaller: Boolean;
function UninstallSilent: Boolean;
function CurrentFilename: String;
function CurrentSourceFilename: String;
function ExpandConstant(const S: String): String;
function ExpandConstantEx(const S: String; const CustomConst, CustomValue: String): String;
function GetPreviousData(const ValueName, DefaultValueData: String): String;
function SetPreviousData(const PreviousDataKey: Integer; const ValueName, ValueData: String): Boolean;
function Terminated: Boolean;
function RegisterExtraCloseApplicationsResource(const DisableFsRedir: Boolean; const AFilename: String): Boolean;
function RmSessionStarted: Boolean;
function GetWizardForm: TWizardForm;
function GetUninstallProgressForm: TUninstallProgressForm;
function GetMainForm: TMainForm;

// Exception functions
procedure Abort;
procedure RaiseException(const Msg: String);
function GetExceptionMessage: String;
procedure ShowExceptionMessage;


// System functions
function IsAdmin: Boolean;
function IsAdminInstallMode: Boolean;
function GetWindowsVersion: Cardinal;
procedure GetWindowsVersionEx(var Version: TWindowsVersion);
function GetWindowsVersionString: String;
function IsWin64: Boolean;
function Is64BitInstallMode: Boolean;
function ProcessorArchitecture: TSetupProcessorArchitecture;
function IsX86: Boolean;
function IsX64: Boolean;
function IsIA64: Boolean;
function IsARM64: Boolean;

function InstallOnThisVersion(const MinVersion, OnlyBelowVersion: String): Boolean;
function IsDotNetInstalled(const MinVersion: TDotNetVersion; const MinServicePack: Cardinal): Boolean;
function IsMsiProductInstalled(const UpgradeCode: String; const PackedMinVersion: Int64): Boolean;
function GetEnv(const EnvVar: String): String;
function GetUserNameString: String;
function GetComputerNameString: String;
function GetUILanguage: Integer;
function FontExists(const FaceName: String): Boolean;
function FindWindowByClassName(const ClassName: String): HWND;
function FindWindowByWindowName(const WindowName: String): HWND;
function SendMessage(const Wnd: HWND; const Msg, WParam, LParam: Longint): Longint;
function PostMessage(const Wnd: HWND; const Msg, WParam, LParam: Longint): Boolean;
function SendNotifyMessage(const Wnd: HWND; const Msg, WParam, LParam: Longint): Boolean;
function RegisterWindowMessage(const Name: String): Longint;
function SendBroadcastMessage(const Msg, WParam, LParam: Longint): Longint;
function PostBroadcastMessage(const Msg, WParam, LParam: Longint): Boolean;
function SendBroadcastNotifyMessage(const Msg, WParam, LParam: Longint): Boolean;
procedure CreateMutex(const Name: String);
function CheckForMutexes(Mutexes: String): Boolean;
procedure MakePendingFileRenameOperationsChecksum: String;
function CreateCallback(Method: AnyMethod): Longword;
procedure UnloadDLL(Filename: String);
function DLLGetLastError(): Longint;

// String functions
function Chr(B: Byte): Char;
function Ord(C: Char): Byte;
function Copy(S: String; Index, Count: Integer): String;
function Length(s: String): Longint;
function Lowercase(S: String): String;
function Uppercase(S: String): String;
function AnsiLowercase(S: String): String;
function AnsiUppercase(S: String): String;
function StringOfChar(c: Char; I : Longint): String;
procedure Delete(var S: String; Index, Count: Integer);
procedure Insert(Source: String; var Dest: String; Index: Integer);
function StringChange(var S: String; const FromStr, ToStr: String): Integer;
function StringChangeEx(var S: String; const FromStr, ToStr: String; const SupportDBCS: Boolean): Integer;
function Pos(SubStr, S: String): Integer;
function AddQuotes(const S: String): String;
function RemoveQuotes(const S: String): String;
function ConvertPercentStr(var S: String): Boolean;
function CompareText(const S1, S2: string): Integer;
function CompareStr(const S1, S2: string): Integer;
function SameText(const S1, S2: string): Boolean;
function SameStr(const S1, S2: string): Boolan;
function IsWildcard(const Pattern: String): Boolean;
function WildcardMatch(const Text, Pattern: String): Boolean;
function Format(const Format: string; const Args: array of const): string;
function Trim(const S: string): String;
function TrimLeft(const S: string): String;
function TrimRight(const S: string): String;
function StrToIntDef(s: string; def: Longint): Longint;
function StrToInt(s: string): Longint;
function StrToInt64Def(s: string; def: Int64): Int64;
function StrToInt64(s: string): Int64;
function StrToFloat(s: string): Extended;
function IntToStr(i: Int64): String;
function FloatToStr(e: extended): String;
function CharLength(const S: String; const Index: Integer): Integer;
function AddBackslash(const S: String): String;
function RemoveBackslashUnlessRoot(const S: String): String;
function RemoveBackslash(const S: String): String;
function AddPeriod(const S: String): String;
function ChangeFileExt(const FileName, Extension: string): String;
function ExtractFileExt(const FileName: string): String;
function ExtractFileDir(const FileName: string): String;
function ExtractFilePath(const FileName: string): String;
function ExtractFileName(const FileName: string): String;
function ExtractFileDrive(const FileName: string): String;
function ExtractRelativePath(const BaseName, DestName: String): String;
function ExpandFileName(const FileName: string): String;
function ExpandUNCFileName(const FileName: string): String;
function GetDateTimeString(const DateTimeFormat: String; const DateSeparator, TimeSeparator: Char): String;
procedure SetLength(var S: String; L: Longint);
procedure CharToOemBuff(var S: AnsiString);
procedure OemToCharBuff(var S: AnsiString);
function GetMD5OfString(const S: AnsiString): String;
function GetMD5OfUnicodeString(const S: String): String;
function GetSHA1OfString(const S: AnsiString): String;
function GetSHA1OfUnicodeString(const S: String): String;
function GetSHA256OfString(const S: AnsiString): String;
function GetSHA256OfUnicodeString(const S: String): String;
function SysErrorMessage(ErrorCode: Integer): String;
function MinimizePathName(const Filename: String; const Font: TFont; MaxLen: Integer): String;

// Array functions
function GetArrayLength(var Arr: Array): Longint;
procedure SetArrayLength(var Arr: Array; I: Longint);

// Variant functions
function Null: Variant;
function Unassigned: Variant;
function VarIsEmpty(const V: Variant): Boolean;
function VarIsClear(const V: Variant): Boolean;
function VarIsNull(const V: Variant): Boolean;
function VarType(const V: Variant): TVarType;


// File System functions
function DirExists(const Name: String): Boolean;
function FileExists(const Name: String): Boolean;
function FileOrDirExists(const Name: String): Boolean;
function FileSize(const Name: String; var Size: Integer): Boolean;
function FileSize64(const Name: String; var Size: Int64): Boolean;
function GetSpaceOnDisk(const Path: String; const InMegabytes: Boolean; var Free, Total: Cardinal): Boolean;
function GetSpaceOnDisk64(const Path: String; var Free, Total: Int64): Boolean;
function FileSearch(const Name, DirList: string): String;
function FindFirst(const FileName: String; var FindRec: TFindRec): Boolean;
function FindNext(var FindRec: TFindRec): Boolean;
procedure FindClose(var FindRec: TFindRec);

function GetCurrentDir: String;
function SetCurrentDir(const Dir: string): Boolean;
function GetWinDir: String;
function GetSystemDir: String;
function GetSysWow64Dir: String;
function GetTempDir: String;
function GetShellFolderByCSIDL(const Folder: Integer; const Create: Boolean): String;
function GetShortName(const LongName: String): String;
function GenerateUniqueName(Path: String; const Extension: String): String;
function IsProtectedSystemFile(const Filename: String): Boolean;
function GetMD5OfFile(const Filename: String): String;
function GetSHA1OfFile(const Filename: String): String;
function GetSHA256OfFile(const Filename: String): String;
function EnableFsRedirection(const Enable: Boolean): Boolean;

// File functions
function Exec(const Filename, Params, WorkingDir: String; const ShowCmd: Integer; const Wait: TExecWait; var ResultCode: Integer): Boolean;
function ExecAsOriginalUser(const Filename, Params, WorkingDir: String; const ShowCmd: Integer; const Wait: TExecWait; var ResultCode: Integer): Boolean;
function ShellExec(const Verb, Filename, Params, WorkingDir: String; const ShowCmd: Integer; const Wait: TExecWait; var ErrorCode: Integer): Boolean;
function ShellExecAsOriginalUser(const Verb, Filename, Params, WorkingDir: String; const ShowCmd: Integer; const Wait: TExecWait; var ErrorCode: Integer): Boolean;
procedure ExtractTemporaryFile(const FileName: String);
function ExtractTemporaryFiles(const Pattern: String): Integer;
function DownloadTemporaryFile(const Url, FileName, RequiredSHA256OfFile: String; const OnDownloadProgress: TOnDownloadProgress): Int64;
function DownloadTemporaryFileSize(const Url): Int64;
function RenameFile(const OldName, NewName: string): Boolean;
function FileCopy(const ExistingFile, NewFile: String; const FailIfExists: Boolean): Boolean;
function DeleteFile(const FileName: string): Boolean;
procedure DelayDeleteFile(const Filename: String; const Tries: Integer);
function SetNTFSCompression(const FileOrDir: String; Compress: Boolean): Boolean;
function LoadStringFromFile(const FileName: String; var S: AnsiString): Boolean;
function LoadStringsFromFile(const FileName: String; var S: TArrayOfString): Boolean;
function SaveStringToFile(const FileName: String; const S: AnsiString; const Append: Boolean): Boolean;
function SaveStringsToFile(const FileName: String; const S: TArrayOfString; const Append: Boolean): Boolean;
function SaveStringsToUTF8File(const FileName: String; const S: TArrayOfString; const Append: Boolean): Boolean;
function CreateDir(const Dir: string): Boolean;
function ForceDirectories(Dir: string): Boolean;
function RemoveDir(const Dir: string): Boolean;
function DelTree(const Path: String; const IsDir, DeleteFiles, DeleteSubdirsAlso: Boolean): Boolean;
function CreateShellLink(const Filename, Description, ShortcutTo, Parameters, WorkingDir, IconFilename: String; const IconIndex, ShowCmd: Integer): String;
function UnpinShellLink(const Filename: String): Boolean;
procedure RegisterServer(const Is64Bit: Boolean; const Filename: String; const FailCriticalErrors: Boolean);
function UnregisterServer(const Is64Bit: Boolean; const Filename: String; const FailCriticalErrors: Boolean): Boolean;
procedure RegisterTypeLibrary(const Is64Bit: Boolean; const Filename: String);
function UnregisterTypeLibrary(const Is64Bit: Boolean; const Filename: String): Boolean
procedure IncrementSharedCount(const Is64Bit: Boolean; const Filename: String; const AlreadyExisted: Boolean);
function DecrementSharedCount(const Is64Bit: Boolean; const Filename: String): Boolean;
procedure RestartReplace(const TempFile, DestFile: String);
procedure UnregisterFont(const FontName, FontFilename: String; const PerUserFont: Boolean);
function ModifyPifFile(const Filename: String; const CloseOnExit: Boolean): Boolean;

// File Version functions
function GetVersionNumbers(const Filename: String; var VersionMS, VersionLS: Cardinal): Boolean;
function GetVersionComponents(const Filename: String; var Major, Minor, Revision, Build: Word): Boolean;
function GetVersionNumbersString(const Filename: String; var Version: String): Boolean;
function GetPackedVersion(const Filename: String; var Version: Int64): Boolean;
function PackVersionNumbers(const VersionMS, VersionLS: Cardinal): Int64;
function PackVersionComponents(const Major, Minor, Revision, Build: Word): Int64;
function ComparePackedVersion(const Version1, Version2: Int64): Integer;
function SamePackedVersion(const Version1, Version2: Int64): Boolean;
procedure UnpackVersionNumbers(const Version: Int64; var VersionMS, VersionLS: Cardinal);
procedure UnpackVersionComponents(const Version: Int64; var Major, Minor, Revision, Build: Word);
function VersionToStr(const Version: Int64): String;
function StrToVersion(const Version: String; var Version: Int64): Boolean;

// Registry functions
function RegKeyExists(const RootKey: Integer; const SubKeyName: String): Boolean;
function RegValueExists(const RootKey: Integer; const SubKeyName, ValueName: String): Boolean;
function RegGetSubkeyNames(const RootKey: Integer; const SubKeyName: String; var Names: TArrayOfString): Boolean;
function RegGetValueNames(const RootKey: Integer; const SubKeyName: String; var Names: TArrayOfString): Boolean;
function RegQueryStringValue(const RootKey: Integer; const SubKeyName, ValueName: String; var ResultStr: String): Boolean;
function RegQueryMultiStringValue(const RootKey: Integer; const SubKeyName, ValueName: String; var ResultStr: String): Boolean;
function RegQueryDWordValue(const RootKey: Integer; const SubKeyName, ValueName: String; var ResultDWord: Cardinal): Boolean;
function RegQueryBinaryValue(const RootKey: Integer; const SubKeyName, ValueName: String; var ResultStr: AnsiString): Boolean;
function RegWriteStringValue(const RootKey: Integer; const SubKeyName, ValueName, Data: String): Boolean;
function RegWriteExpandStringValue(const RootKey: Integer; const SubKeyName, ValueName, Data: String): Boolean;
function RegWriteMultiStringValue(const RootKey: Integer; const SubKeyName, ValueName, Data: String): Boolean;
function RegWriteDWordValue(const RootKey: Integer; const SubKeyName, ValueName: String; const Data: Cardinal): Boolean;
function RegWriteBinaryValue(const RootKey: Integer; const SubKeyName, ValueName: String; const Data: AnsiString): Boolean;
function RegDeleteKeyIncludingSubkeys(const RootKey: Integer; const SubkeyName: String): Boolean;
function RegDeleteKeyIfEmpty(const RootKey: Integer; const SubkeyName: String): Boolean;
function RegDeleteValue(const RootKey: Integer; const SubKeyName, ValueName: String): Boolean;

// INI File functions
function IniKeyExists(const Section, Key, Filename: String): Boolean;
function IsIniSectionEmpty(const Section, Filename: String): Boolean;
function GetIniBool(const Section, Key: String; const Default: Boolean; const Filename: String): Boolean
function GetIniInt(const Section, Key: String; const Default, Min, Max: Longint; const Filename: String): Longint;
function GetIniString(const Section, Key, Default, Filename: String): String;
function SetIniBool(const Section, Key: String; const Value: Boolean; const Filename: String): Boolean;
function SetIniInt(const Section, Key: String; const Value: Longint; const Filename: String): Boolean;
function SetIniString(const Section, Key, Value, Filename: String): Boolean;
procedure DeleteIniSection(const Section, Filename: String);
procedure DeleteIniEntry(const Section, Key, Filename: String);

// Custom Setup Wizard Page functions
function CreateInputQueryPage(const AfterID: Integer; const ACaption, ADescription, ASubCaption: String): TInputQueryWizardPage;
function CreateInputOptionPage(const AfterID: Integer; const ACaption, ADescription, ASubCaption: String; Exclusive, ListBox: Boolean): TInputOptionWizardPage;
function CreateInputDirPage(const AfterID: Integer; const ACaption, ADescription, ASubCaption: String; AAppendDir: Boolean; ANewFolderName: String): TInputDirWizardPage;
function CreateInputFilePage(const AfterID: Integer; const ACaption, ADescription, ASubCaption: String): TInputFileWizardPage;
function CreateOutputMsgPage(const AfterID: Integer; const ACaption, ADescription, AMsg: String): TOutputMsgWizardPage;
function CreateOutputMsgMemoPage(const AfterID: Integer; const ACaption, ADescription, ASubCaption: String; const AMsg: AnsiString): TOutputMsgMemoWizardPage;
function CreateOutputProgressPage(const ACaption, ADescription: String): TOutputProgressWizardPage;
function CreateOutputMarqueeProgressPage(const ACaption, ADescription: String): TOutputMarqueeProgressWizardPage;
function CreateDownloadPage(const ACaption, ADescription: String; const OnDownloadProgress: TOnDownloadProgress): TDownloadWizardPage;
function CreateCustomPage(const AfterID: Integer; const ACaption, ADescription: String): TWizardPage;
function CreateCustomForm: TSetupForm;
function PageFromID(const ID: Integer): TWizardPage;
function PageIndexFromID(const ID: Integer): Integer;
function ScaleX(X: Integer): Integer;
function ScaleY(Y: Integer): Integer;
function InitializeBitmapImageFromIcon(const BitmapImage: TBitmapImage; const IconFilename: String; const BkColor: TColor; const AscendingTrySizes: TArrayOfInteger): Boolean;

// Dialog functions
function MsgBox(const Text: String; const Typ: TMsgBoxType; const Buttons: Integer): Integer;
function SuppressibleMsgBox(const Text: String; const Typ: TMsgBoxType; const Buttons, Default: Integer): Integer;
function TaskDialogMsgBox(const Instruction, Text: String; const Typ: TMsgBoxType; const Buttons: Cardinal; const ButtonLabels: TArrayOfString; const ShieldButton: Integer): Integer;
function SuppressibleTaskDialogMsgBox(const Instruction, Text: String; const Typ: TMsgBoxType; const Buttons: Cardinal; const ButtonLabels: TArrayOfString; const ShieldButton: Integer; const Default: Integer): Integer;
function GetOpenFileName(const Prompt: String; var FileName: String; const InitialDirectory, Filter, DefaultExtension: String): Boolean;
function GetOpenFileNameMulti(const Prompt: String; var FileNameList: TStrings; const InitialDirectory, Filter, DefaultExtension: String): Boolean;
function GetSaveFileName(const Prompt: String; var FileName: String; const InitialDirectory, Filter, DefaultExtension: String): Boolean;
function BrowseForFolder(const Prompt: String; var Directory: String; const NewFolderButton: Boolean): Boolean;
function ExitSetupMsgBox: Boolean;
function SelectDisk(const DiskNumber: Integer; const AFilename: String; var Path: String): Boolean;

// COM Automation objects support functions
function CreateOleObject(const ClassName: string): Variant;
function GetActiveOleObject(const ClassName: string): Variant;
function IDispatchInvoke(Self: IDispatch; PropertySet: Boolean; const Name: String; Par: array of Variant): Variant;
function CreateComObject(const ClassID: TGUID): IUnknown;
function StringToGUID(const S: String): TGUID;
procedure OleCheck(Result: HResult);
procedure CoFreeUnusedLibraries;

// Setup Logging functions
procedure Log(const S: String);

// Other functions
procedure Sleep(const Milliseconds: LongInt);
function Random(const Range: Integer): Integer;
procedure Beep;
procedure Set8087CW(NewCW: Word);
function Get8087CW: Word;
procedure BringToFrontAndRestore;

//! constants		===========================================================
// CurStep values
ssInstall, ssPostInstall, ssDone

// CurPage values
wpWelcome, wpLicense, wpPassword, wpInfoBefore, wpUserInfo, wpSelectDir, wpSelectComponents, wpSelectProgramGroup, wpSelectTasks, wpReady, wpPreparing, wpInstalling, wpInfoAfter, wpFinished

// Exec and ShellExec - ShowCmd values
SW_SHOW, SW_SHOWNORMAL, SW_SHOWMAXIMIZED, SW_SHOWMINIMIZED, SW_SHOWMINNOACTIVE, SW_HIDE

// TMsgBoxType
mbInformation, mbConfirmation, mbError, mbCriticalError

// MsgBox - Buttons flags
MB_OK, MB_OKCANCEL, MB_ABORTRETRYIGNORE, MB_YESNOCANCEL, MB_YESNO, MB_RETRYCANCEL, MB_DEFBUTTON1, MB_DEFBUTTON2, MB_DEFBUTTON3, MB_SETFOREGROUND

// MsgBox - return values
IDOK, IDCANCEL, IDABORT, IDRETRY, IDIGNORE, IDYES, IDNO

// Reg* - RootKey values (also see the [Registry] section documentation)
HKEY_AUTO, HKEY_AUTO_32, HKEY_AUTO_64,
HKEY_CLASSES_ROOT, HKEY_CLASSES_ROOT_32, HKEY_CLASSES_ROOT_64,
HKEY_CURRENT_USER, HKEY_CURRENT_USER_32, HKEY_CURRENT_USER_64,
HKEY_LOCAL_MACHINE, HKEY_LOCAL_MACHINE_32, HKEY_LOCAL_MACHINE_64,
HKEY_USERS, HKEY_USERS_32, HKEY_USERS_64,
HKEY_PERFORMANCE_DATA,
HKEY_CURRENT_CONFIG, HKEY_CURRENT_CONFIG_32, HKEY_CURRENT_CONFIG_64,
HKEY_DYN_DATA,
HKA, HKA32, HKA64, HKCR, HKCR32, HKCR64, HKCU, HKCU32, HKCU64, HKLM, HKLM32, HKLM64, HKU, HKU32, HKU64, HKCC, HKCC32, HKCC64

// TShellFolderID
sfDesktop, sfStartMenu, sfPrograms, sfStartup, sfSendTo, sfFonts, sfAppData, sfDocs, sfTemplates, sfFavorites, sfLocalAppData

// TSetupMessageID
//Use 'msg' + the message name. Example: SetupMessage(msgSetupAppTitle)

// TShouldProcessEntryResult
srNo, srYes, srUnknown

//! classes			===========================================================
TObject = class
	constructor Create;
	procedure Free;
end;

TPersistent = class(TObject)
	procedure Assign(Source: TPersistent);
end;

TComponent = class(TPersistent)
	function FindComponent(AName: String): TComponent;
	constructor Create(AOwner: TComponent);
	property Owner: TComponent; read write;
	procedure DestroyComponents;
	procedure Destroying;
	procedure FreeNotification(AComponent: TComponent);
	procedure InsertComponent(AComponent: TComponent);
	procedure RemoveComponent(AComponent: TComponent);
	property Components[Index: Integer]: TComponent; read;
	property ComponentCount: Integer; read;
	property ComponentIndex: Integer; read write;
	property ComponentState: Byte; read;
	property DesignInfo: Longint; read write;
	property Name: String; read write;
	property Tag: Longint; read write;
end;

TStrings = class(TPersistent)
	function Add(S: String): Integer;
	procedure Append(S: String);
	procedure AddStrings(Strings: TStrings);
	procedure Clear;
	procedure Delete(Index: Integer);
	function IndexOf(const S: String): Integer;
	procedure Insert(Index: Integer; S: String);
	property Count: Integer; read;
	property Text: String; read write;
	property CommaText: String; read write;
	procedure LoadFromFile(FileName: String);
	procedure SaveToFile(FileName: String);
	property Strings[Index: Integer]: String; read write;
	property Objects[Index: Integer]: TObject; read write;
end;

TNotifyEvent = procedure(Sender: TObject);

TStringList = class(TStrings)
	function Find(S: String; var Index: Integer): Boolean;
	procedure Sort;
	property Duplicates: TDuplicates; read write;
	property Sorted: Boolean; read write;
	property OnChange: TNotifyEvent; read write;
	property OnChanging: TNotifyEvent; read write;
end;

TStream = class(TObject)
	function Read(Buffer: String; Count: Longint): Longint;
	function Write(Buffer: String; Count: Longint): Longint;
	function Seek(Offset: Int64; Origin: Word): Int64;
	procedure ReadBuffer(Buffer: String; Count: Longint);
	procedure WriteBuffer(Buffer: String; Count: Longint);
	function CopyFrom(Source: TStream; Count: Int64): Int64;
	property Position: Longint; read write;
	property Size: Longint; read write;
end;

THandleStream = class(TStream)
	constructor Create(AHandle: Integer);
	property Handle: Integer; read;
end;

TFileStream = class(THandleStream)
	constructor Create(Filename: String; Mode: Word);
end;

TStringStream = class(TStream)
	constructor Create(AString: String);
end;

TGraphicsObject = class(TPersistent)
	property OnChange: TNotifyEvent; read write;
end;

TFontStyle = (fsBold, fsItalic, fsUnderline, fsStrikeOut);

TFontStyles = set of TFontStyle;

TFont = class(TGraphicsObject)
	constructor Create;
	property Handle: Integer; read;
	property Color: Integer; read write;
	property Height: Integer; read write;
	property Name: String; read write;
	property Pitch: Byte; read write;
	property Size: Integer; read write;
	property PixelsPerInch: Integer; read write;
	property Style: TFontStyles; read write;
end;

TCanvas = class(TPersistent)
	procedure Arc(X1, Y1, X2, Y2, X3, Y3, X4, Y4: Integer);
	procedure Chord(X1, Y1, X2, Y2, X3, Y3, X4, Y4: Integer);
	procedure Draw(X, Y: Integer; Graphic: TGraphic);
	procedure Ellipse(X1, Y1, X2, Y2: Integer);
	procedure FloodFill(X, Y: Integer; Color: TColor; FillStyle: Byte);
	procedure LineTo(X, Y: Integer);
	procedure MoveTo(X, Y: Integer);
	procedure Pie(X1, Y1, X2, Y2, X3, Y3, X4, Y4: Integer);
	procedure Rectangle(X1, Y1, X2, Y2: Integer);
	procedure Refresh;
	procedure RoundRect(X1, Y1, X2, Y2, X3, Y3: Integer);
	function TextHeight(Text: String): Integer;
	procedure TextOut(X, Y: Integer; Text: String);
	function TextWidth(Text: String): Integer;
	property Handle: Integer; read write;
	property Pixels: Integer Integer Integer; read write;
	property Brush: TBrush; read;
	property CopyMode: Byte; read write;
	property Font: TFont; read;
	property Pen: TPen; read;
end;

TPenMode = (pmBlack, pmWhite, pmNop, pmNot, pmCopy, pmNotCopy, pmMergePenNot, pmMaskPenNot, pmMergeNotPen, pmMaskNotPen, pmMerge, pmNotMerge, pmMask, pmNotMask, pmXor, pmNotXor);

TPenStyle = (psSolid, psDash, psDot, psDashDot, psDashDotDot, psClear, psInsideFrame);

TPen = class(TGraphicsObject)
	constructor Create;
	property Color: TColor; read write;
	property Mode: TPenMode; read write;
	property Style: TPenStyle; read write;
	property Width: Integer; read write;
end;

TBrushStyle = (bsSolid, bsClear, bsHorizontal, bsVertical, bsFDiagonal, bsBDiagonal, bsCross, bsDiagCross);

TBrush = class(TGraphicsObject)
	constructor Create;
	property Color: TColor; read write;
	property Style: TBrushStyle; read write;
end;

TGraphic = class(TPersistent)
	procedure LoadFromFile(const Filename: String);
	procedure SaveToFile(const Filename: String);
	property Empty: Boolean; read write;
	property Height: Integer; read write;
	property Modified: Boolean; read write;
	property Width: Integer; read write;
	property OnChange: TNotifyEvent; read write;
end;

TAlphaFormat = (afIgnored, afDefined, afPremultiplied);

TBitmap = class(TGraphic)
	procedure LoadFromStream(Stream: TStream);
	procedure SaveToStream(Stream: TStream);
	property AlphaFormat: TAlphaFormat; read write;
	property Canvas: TCanvas; read write;
	property Handle: HBITMAP; read write;
end;

TAlign = (alNone, alTop, alBottom, alLeft, alRight, alClient);

TAnchorKind = (akLeft, akTop, akRight, akBottom);

TAnchors = set of TAnchorKind;

TControl = class(TComponent)
	constructor Create(AOwner: TComponent);
	procedure BringToFront;
	procedure Hide;
	procedure Invalidate;
	procedure Refresh;
	procedure Repaint;
	procedure SendToBack;
	procedure Show;
	procedure Update;
	procedure SetBounds(ALeft, ATop, AWidth, AHeight: Integer);
	property Left: Integer; read write;
	property Top: Integer; read write;
	property Width: Integer; read write;
	property Height: Integer; read write;
	property Hint: String; read write;
	property Align: TAlign; read write;
	property ClientHeight: Longint; read write;
	property ClientWidth: Longint; read write;
	property ShowHint: Boolean; read write;
	property Visible: Boolean; read write;
	property Enabled: Boolean; read write;
	property Cursor: Integer; read write;
end;

TWinControl = class(TControl)
	property Parent: TWinControl; read write;
	property ParentBackground: Boolean; read write;
	property Handle: Longint; read write;
	property Showing: Boolean; read;
	property TabOrder: Integer; read write;
	property TabStop: Boolean; read write;
	function CanFocus: Boolean;
	function Focused: Boolean;
	property Controls[Index: Integer]: TControl; read;
	property ControlCount: Integer; read;
end;

TGraphicControl = class(TControl)
end;

TCustomControl = class(TWinControl)
end;

TScrollBarKind = (sbHorizontal, sbVertical);

TScrollBarInc = SmallInt;

TScrollingWinControl = class(TWinControl)
	procedure ScrollInView(AControl: TControl);
end;

TFormBorderStyle = (bsNone, bsSingle, bsSizeable, bsDialog, bsToolWindow, bsSizeToolWin);

TBorderIcon = (biSystemMenu, biMinimize, biMaximize, biHelp);

TBorderIcons = set of TBorderIcon;

TConstraintSize = 0..MaxInt;

TSizeConstraints = class(TPersistent);
	property MaxHeight: TConstraintSize; read write;
	property MaxWidth: TConstraintSize; read write;
	property MinHeight: TConstraintSize; read write;
	property MinWidth: TConstraintSize; read write;
end;

TPosition = (poDesigned, poDefault, poDefaultPosOnly, poDefaultSizeOnly, poScreenCenter, poDesktopCenter, poMainFormCenter, poOwnerFormCenter);

TCloseAction = (caNone, caHide, caFree, caMinimize);

TCloseEvent = procedure(Sender: TObject; var Action: TCloseAction);

TCloseQueryEvent = procedure(Sender: TObject; var CanClose: Boolean);

TEShiftState = (ssShift, ssAlt, ssCtrl, ssLeft, ssRight, ssMiddle, ssDouble);

TShiftState = set of TEShiftState;

TKeyEvent = procedure(Sender: TObject; var Key: Word; Shift: TShiftState);

TKeyPressEvent = procedure(Sender: TObject; var Key: Char);

TForm = class(TScrollingWinControl)
	constructor CreateNew(AOwner: TComponent);
	procedure Close;
	procedure Hide;
	procedure Show;
	function ShowModal: Integer;
	procedure Release;
	property Active: Boolean; read;
	property ActiveControl: TWinControl; read write;
	property Anchors: TAnchors; read write;
	property AutoScroll: Boolean; read write;
	property BorderIcons: TBorderIcons; read write;
	property BorderStyle: TFormBorderStyle; read write;
	property Caption: String; read write;
	property Color: TColor; read write;
	property Constraints: TSizeConstraints; read write;
	property Font: TFont; read write;
	property FormStyle: TFormStyle; read write;
	property KeyPreview: Boolean; read write;
	property Position: TPosition; read write;
	property OnActivate: TNotifyEvent; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
	property OnClose: TCloseEvent; read write;
	property OnCloseQuery: TCloseQueryEvent; read write;
	property OnCreate: TNotifyEvent; read write;
	property OnDestroy: TNotifyEvent; read write;
	property OnDeactivate: TNotifyEvent; read write;
	property OnHide: TNotifyEvent; read write;
	property OnKeyDown: TKeyEvent; read write;
	property OnKeyPress: TKeyPressEvent; read write;
	property OnKeyUp: TKeyEvent; read write;
	property OnResize: TNotifyEvent; read write;
	property OnShow: TNotifyEvent; read write;
end;

TCustomLabel = class(TGraphicControl)
end;

TAlignment = (taLeftJustify, taRightJustify, taCenter);

TLabel = class(TCustomLabel)
	property Alignment: TAlignment; read write;
	property Anchors: TAnchors; read write;
	property AutoSize: Boolean; read write;
	property Caption: String; read write;
	property Color: TColor; read write;
	property FocusControl: TWinControl; read write;
	property Font: TFont; read write;
	property WordWrap: Boolean; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
end;

TCustomEdit = class(TWinControl)
	procedure Clear;
	procedure ClearSelection;
	procedure SelectAll;
	property Modified: Boolean; read write;
	property SelLength: Integer; read write;
	property SelStart: Integer; read write;
	property SelText: String; read write;
	property Text: String; read write;
end;

TBorderStyle = TFormBorderStyle;

TEditCharCase = (ecNormal, ecUpperCase, ecLowerCase);

TEdit = class(TCustomEdit)
	property Anchors: TAnchors; read write;
	property AutoSelect: Boolean; read write;
	property AutoSize: Boolean; read write;
	property BorderStyle: TBorderStyle; read write;
	property CharCase: TEditCharCase; read write;
	property Color: TColor; read write;
	property Font: TFont; read write;
	property HideSelection: Boolean; read write;
	property MaxLength: Integer; read write;
	property PasswordChar: Char; read write;
	property ReadOnly: Boolean; read write;
	property Text: String; read write;
	property OnChange: TNotifyEvent; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
	property OnKeyDown: TKeyEvent; read write;
	property OnKeyPress: TKeyPressEvent; read write;
	property OnKeyUp: TKeyEvent; read write;
end;

TNewEdit = class(TEdit)
end;

TCustomMemo = class(TCustomEdit)
	property Lines: TStrings; read write;
end;

TScrollStyle = (ssNone, ssHorizontal, ssVertical, ssBoth);

TMemo = class(TCustomMemo)
	property Alignment: TAlignment; read write;
	property Anchors: TAnchors; read write;
	property BorderStyle: TBorderStyle; read write;
	property Color: TColor; read write;
	property Font: TFont; read write;
	property HideSelection: Boolean; read write;
	property Lines: TStrings; read write;
	property MaxLength: Integer; read write;
	property ReadOnly: Boolean; read write;
	property ScrollBars: TScrollStyle; read write;
	property WantReturns: Boolean; read write;
	property WantTabs: Boolean; read write;
	property WordWrap: Boolean; read write;
	property OnChange: TNotifyEvent; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
	property OnKeyDown: TKeyEvent; read write;
	property OnKeyPress: TKeyPressEvent; read write;
	property OnKeyUp: TKeyEvent; read write;
end;

TNewMemo = class(TMemo)
end;

TCustomComboBox = class(TWinControl)
	property DroppedDown: Boolean; read write;
	property Items: TStrings; read write;
	property ItemIndex: Integer; read write;
end;

TComboBoxStyle = (csDropDown, csSimple, csDropDownList, csOwnerDrawFixed, csOwnerDrawVariable);

TComboBox = class(TCustomComboBox)
	property Anchors: TAnchors; read write;
	property Color: TColor; read write;
	property DropDownCount: Integer; read write;
	property Font: TFont; read write;
	property MaxLength: Integer; read write;
	property Sorted: Boolean; read write;
	property Style: TComboBoxStyle; read write;
	property Text: String; read write;
	property OnChange: TNotifyEvent; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
	property OnDropDown: TNotifyEvent; read write;
	property OnKeyDown: TKeyEvent; read write;
	property OnKeyPress: TKeyPressEvent; read write;
	property OnKeyUp: TKeyEvent; read write;
end;

TNewComboBox = class(TComboBox)
end;

TButtonControl = class(TWinControl)
end;

TButton = class(TButtonControl)
	property Anchors: TAnchors; read write;
	property Cancel: Boolean; read write;
	property Caption: String; read write;
	property Default: Boolean; read write;
	property Font: TFont; read write;
	property ModalResult: Longint; read write;
	property OnClick: TNotifyEvent; read write;
end;

TNewButton = class(TButton)
end;

TCustomCheckBox = class(TButtonControl)
end;

TCheckBoxState = (cbUnchecked, cbChecked, cbGrayed);

TCheckBox = class(TCustomCheckBox)
	property Alignment: TAlignment; read write;
	property AllowGrayed: Boolean; read write;
	property Anchors: TAnchors; read write;
	property Caption: String; read write;
	property Checked: Boolean; read write;
	property Color: TColor; read write;
	property Font: TFont; read write;
	property State: TCheckBoxState; read write;
	property OnClick: TNotifyEvent; read write;
end;

TNewCheckBox = class(TCheckBox)
end;

TRadioButton = class(TButtonControl)
	property Alignment: TAlignment; read write;
	property Anchors: TAnchors; read write;
	property Caption: String; read write;
	property Checked: Boolean; read write;
	property Color: TColor; read write;
	property Font: TFont; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
end;

TNewRadioButton = class(TRadioButton)
end;

TCustomListBox = class(TWinControl)
	property Items: TStrings; read write;
	property ItemIndex: Integer; read write;
	property SelCount: Integer; read;
	property Selected[Index: Integer]: Boolean; read write;
end;

TListBoxStyle = (lbStandard, lbOwnerDrawFixed, lbOwnerDrawVariable);

TListBox = class(TCustomListBox)
	property Anchors: TAnchors; read write;
	property BorderStyle: TBorderStyle; read write;
	property Color: TColor; read write;
	property Font: TFont; read write;
	property MultiSelect: Boolean; read write;
	property Sorted: Boolean; read write;
	property Style: TListBoxStyle; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
	property OnKeyDown: TKeyEvent; read write;
	property OnKeyPress: TKeyPressEvent; read write;
	property OnKeyUp: TKeyEvent; read write;
end;

TNewListBox = class(TListBox)
end;

TBevelKind = (bkNone, bkTile, bkSoft, bkFlat);

TBevelShape = (bsBox, bsFrame, bsTopLine, bsBottomLine, bsLeftLine, bsRightLine, bsSpacer);

TBevelStyle = (bsLowered, bsRaised);

TBevel = class(TGraphicControl)
	property Anchors: TAnchors; read write;
	property Shape: TBevelShape; read write;
	property Style: TBevelStyle; read write;
end;

TCustomPanel = class(TCustomControl)
end;

TPanelBevel = (bvNone, bvLowered, bvRaised, bvSpace);

TBevelWidth = Longint;

TBorderWidth = Longint;

TPanel = class(TCustomPanel)
	property Alignment: TAlignment; read write;
	property Anchors: TAnchors; read write;
	property BevelInner: TPanelBevel; read write;
	property BevelKind: TBevelKind; read write;
	property BevelOuter: TPanelBevel; read write;
	property BevelWidth: TBevelWidth; read write;
	property BorderWidth: TBorderWidth; read write;
	property BorderStyle: TBorderStyle; read write;
	property Caption: String; read write;
	property Color: TColor; read write;
	property Font: TFont; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
end;

TNewStaticText = class(TWinControl)
	function AdjustHeight: Integer;
	property Anchors: TAnchors; read write;
	property AutoSize: Boolean; read write;
	property Caption: String; read write;
	property Color: TColor; read write;
	property FocusControl: TWinControl; read write;
	property Font: TFont; read write;
	property ForceLTRReading: Boolean; read write;
	property ShowAccelChar: Boolean; read write;
	property WordWrap: Boolean; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
end;

TCheckItemOperation = (coUncheck, coCheck, coCheckWithChildren);

TNewCheckListBox = class(TCustomListBox)
	function AddCheckBox(const ACaption, ASubItem: String; ALevel: Byte; AChecked, AEnabled, AHasInternalChildren, ACheckWhenParentChecked: Boolean; AObject: TObject): Integer;
	function AddGroup(ACaption, ASubItem: String; ALevel: Byte; AObject: TObject): Integer;
	function AddRadioButton(const ACaption, ASubItem: String; ALevel: Byte; AChecked, AEnabled: Boolean; AObject: TObject): Integer;
	function CheckItem(const Index: Integer; const AOperation: TCheckItemOperation): Boolean;
	property Anchors: TAnchors; read write;
	property Checked[Index: Integer]: Boolean; read write;
	property State[Index: Integer]: TCheckBoxState; read write;
	property ItemCaption[Index: Integer]: String; read write;
	property ItemEnabled[Index: Integer]: Boolean; read write;
	property ItemFontStyle[Index: Integer]: TFontStyles; read write;
	property ItemLevel[Index: Integer]: Byte; read;
	property ItemObject[Index: Integer]: TObject; read write;
	property ItemSubItem[Index: Integer]: String; read write;
	property SubItemFontStyle[Index: Integer]: TFontStyles; read write;
	property Flat: Boolean; read write;
	property MinItemHeight: Integer; read write;
	property Offset: Integer; read write;
	property OnClickCheck: TNotifyEvent; read write;
	property BorderStyle: TBorderStyle; read write;
	property Color: TColor; read write;
	property Font: TFont; read write;
	property Sorted: Boolean; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
	property OnKeyDown: TKeyEvent; read write;
	property OnKeyPress: TKeyPressEvent; read write;
	property OnKeyUp: TKeyEvent; read write;
	property ShowLines: Boolean; read write;
	property WantTabs: Boolean; read write;
	property RequireRadioSelection: Boolean; read write;
end;

TNewProgressBarState = (npbsNormal, npbsError, npbsPaused);

TNewProgressBarStyle = (npbstNormal, npbstMarquee);

TNewProgressBar = class(TWinControl)
	property Anchors: TAnchors; read write;
	property Min: Longint; read write;
	property Max: Longint; read write;
	property Position: Longint; read write;
	property State: TNewProgressBarState; read write;
	property Style: TNewProgressBarStyle; read write;
	property Visible: Boolean; read write;
end;

TRichEditViewer = class(TMemo)
	property Anchors: TAnchors; read write;
	property BevelKind: TBevelKind; read write;
	property BorderStyle: TBorderStyle; read write;
	property RTFText: AnsiString; write;
	property UseRichEdit: Boolean; read write;
end;

TPasswordEdit = class(TCustomEdit)
	property Anchors: TAnchors; read write;
	property AutoSelect: Boolean; read write;
	property AutoSize: Boolean; read write;
	property BorderStyle: TBorderStyle; read write;
	property Color: TColor; read write;
	property Font: TFont; read write;
	property HideSelection: Boolean; read write;
	property MaxLength: Integer; read write;
	property Password: Boolean; read write;
	property ReadOnly: Boolean; read write;
	property Text: String; read write;
	property OnChange: TNotifyEvent; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
	property OnKeyDown: TKeyEvent; read write;
	property OnKeyPress: TKeyPressEvent; read write;
	property OnKeyUp: TKeyEvent; read write;
end;

TCustomFolderTreeView = class(TWinControl)
	procedure ChangeDirectory(const Value: String; const CreateNewItems: Boolean);
	procedure CreateNewDirectory(const ADefaultName: String);
	property: Directory: String; read write;
end;

TFolderRenameEvent = procedure(Sender: TCustomFolderTreeView; var NewName: String; var Accept: Boolean);

TFolderTreeView = class(TCustomFolderTreeView)
	property Anchors: TAnchors; read write;
	property OnChange: TNotifyEvent; read write;
	property OnRename: TFolderRenameEvent; read write;
end;

TStartMenuFolderTreeView = class(TCustomFolderTreeView)
	procedure SetPaths(const AUserPrograms, ACommonPrograms, AUserStartup, ACommonStartup: String);
	property Anchors: TAnchors; read write;
	property OnChange: TNotifyEvent; read write;
	property OnRename: TFolderRenameEvent; read write;
end;

TBitmapImage = class(TGraphicControl)
	property Anchors: TAnchors; read write;
	property AutoSize: Boolean; read write;
	property BackColor: TColor; read write;
	property Center: Boolean; read write;
	property Bitmap: TBitmap; read write;
	property ReplaceColor: TColor; read write;
	property ReplaceWithColor: TColor; read write;
	property Stretch: Boolean; read write;
	property OnClick: TNotifyEvent; read write;
	property OnDblClick: TNotifyEvent; read write;
end;

TNewNotebook = class(TWinControl)
	function FindNextPage(CurPage: TNewNotebookPage; GoForward: Boolean): TNewNotebookPage;
	property Anchors: TAnchors; read write;
	property PageCount: Integer; read write;
	property Pages[Index: Integer]: TNewNotebookPage; read;
	property ActivePage: TNewNotebookPage; read write;
end;

TNewNotebookPage = class(TCustomControl)
	property Color: TColor; read write;
	property Notebook: TNewNotebook; read write;
	property PageIndex: Integer; read write;
end;

TWizardPageNotifyEvent = procedure(Sender: TWizardPage);
TWizardPageButtonEvent = function(Sender: TWizardPage): Boolean;
TWizardPageCancelEvent = procedure(Sender: TWizardPage; var ACancel, AConfirm: Boolean);
TWizardPageShouldSkipEvent = function(Sender: TWizardPage): Boolean;

TWizardPage = class(TComponent)
	property ID: Integer; read;
	property Caption: String; read write;
	property Description: String; read write;
	property Surface: TNewNotebookPage; read;
	property SurfaceColor: TColor; read;
	property SurfaceHeight: Integer; read;
	property SurfaceWidth: Integer; read;
	property OnActivate: TWizardPageNotifyEvent; read write;
	property OnBackButtonClick: TWizardPageButtonEvent; read write;
	property OnCancelButtonClick: TWizardPageCancelEvent; read write;
	property OnNextButtonClick: TWizardPageButtonEvent; read write;
	property OnShouldSkipPage: TWizardPageShouldSkipEvent; read write;
end;

TInputQueryWizardPage = class(TWizardPage)
	function Add(const APrompt: String; const APassword: Boolean): Integer;
	property Edits[Index: Integer]: TPasswordEdit; read;
	property PromptLabels[Index: Integer]: TNewStaticText; read;
	property SubCaptionLabel: TNewStaticText; read;
	property Values[Index: Integer]: String; read write;
end;

TInputOptionWizardPage = class(TWizardPage)
	function Add(const ACaption: String): Integer;
	function AddEx(const ACaption: String; const ALevel: Byte; const AExclusive: Boolean): Integer;
	property CheckListBox: TNewCheckListBox; read;
	property SelectedValueIndex: Integer; read write;
	property SubCaptionLabel: TNewStaticText; read;
	property Values[Index: Integer]: Boolean; read write;
end;

TInputDirWizardPage = class(TWizardPage)
	function Add(const APrompt: String): Integer;
	property Buttons[Index: Integer]: TNewButton; read;
	property Edits[Index: Integer]: TEdit; read;
	property PromptLabels[Index: Integer]: TNewStaticText; read;
	property SubCaptionLabel: TNewStaticText; read;
	property Values[Index: Integer]: String; read write;
end;

TInputFileWizardPage = class(TWizardPage)
	function Add(const APrompt, AFilter, ADefaultExtension: String): Integer;
	property Buttons[Index: Integer]: TNewButton; read;
	property Edits[Index: Integer]: TEdit; read;
	property PromptLabels[Index: Integer]: TNewStaticText; read;
	property SubCaptionLabel: TNewStaticText; read;
	property Values[Index: Integer]: String; read write;
	property IsSaveButton[Index: Integer]: Boolean; read write;
end;

TOutputMsgWizardPage = class(TWizardPage)
	property MsgLabel: TNewStaticText; read;
end;

TOutputMsgMemoWizardPage = class(TWizardPage)
	property RichEditViewer: TRichEditViewer; read;
	property SubCaptionLabel: TNewStaticText; read;
end;

TOutputProgressWizardPage = class(TWizardPage)
	procedure Hide;
	property Msg1Label: TNewStaticText; read;
	property Msg2Label: TNewStaticText; read;
	property ProgressBar: TNewProgressBar; read;
	procedure SetProgress(const Position, Max: Longint);
	procedure SetText(const Msg1, Msg2: String);
	procedure Show;
end;

TOutputMarqueeProgressWizardPage = class(TOutputProgressWizardPage)
	procedure Animate;
end;

TDownloadWizardPage = class(TOutputProgressWizardPage)
	property AbortButton: TNewButton; read;
	property AbortedByUser: Boolean; read;
	procedure Add(const Url, BaseName, RequiredSHA256OfFile: String);
	procedure Clear;
	function Download: Int64;
end;

TUIStateForm = class(TForm)
end;

TSetupForm = class(TUIStateForm)
	function CalculateButtonWidth(const ButtonCaptions: array of String): Integer;
	function ShouldSizeX: Boolean;
	function ShouldSizeY: Boolean;
	procedure FlipSizeAndCenterIfNeeded(const ACenterInsideControl: Boolean; const CenterInsideControlCtl: TWinControl; const CenterInsideControlInsideClientArea: Boolean);
	property ControlsFlipped: Boolean; read;
	property FlipControlsOnShow: Boolean; read write;
	property KeepSizeY: Boolean; read; write;
	property RightToLeft: Boolean; read;
	property SizeAndCenterOnShow: Boolean; read write;
end;

TMainForm = class(TSetupForm)
	procedure ShowAboutBox;
end;

TWizardForm = class(TSetupForm)
	property CancelButton: TNewButton; read;
	property NextButton: TNewButton; read;
	property BackButton: TNewButton; read;
	property OuterNotebook: TNotebook; read;
	property InnerNotebook: TNotebook; read;
	property WelcomePage: TNewNotebookPage; read;
	property InnerPage: TNewNotebookPage; read;
	property FinishedPage: TNewNotebookPage; read;
	property LicensePage: TNewNotebookPage; read;
	property PasswordPage: TNewNotebookPage; read;
	property InfoBeforePage: TNewNotebookPage; read;
	property UserInfoPage: TNewNotebookPage; read;
	property SelectDirPage: TNewNotebookPage; read;
	property SelectComponentsPage: TNewNotebookPage; read;
	property SelectProgramGroupPage: TNewNotebookPage; read;
	property SelectTasksPage: TNewNotebookPage; read;
	property ReadyPage: TNewNotebookPage; read;
	property PreparingPage: TNewNotebookPage; read;
	property InstallingPage: TNewNotebookPage; read;
	property InfoAfterPage: TNewNotebookPage; read;
	property DiskSpaceLabel: TNewStaticText; read;
	property DirEdit: TEdit; read;
	property GroupEdit: TNewEdit; read;
	property NoIconsCheck: TNewCheckBox; read;
	property PasswordLabel: TNewStaticText; read;
	property PasswordEdit: TPasswordEdit; read;
	property PasswordEditLabel: TNewStaticText; read;
	property ReadyMemo: TNewMemo; read;
	property TypesCombo: TNewComboBox; read;
	property Bevel: TBevel; read;
	property WizardBitmapImage: TBitmapImage; read;
	property WelcomeLabel1: TNewStaticText; read;
	property InfoBeforeMemo: TRichEditViewer; read;
	property InfoBeforeClickLabel: TNewStaticText; read;
	property MainPanel: TPanel; read;
	property Bevel1: TBevel; read;
	property PageNameLabel: TNewStaticText; read;
	property PageDescriptionLabel: TNewStaticText; read;
	property WizardSmallBitmapImage: TBitmapImage; read;
	property ReadyLabel: TNewStaticText; read;
	property FinishedLabel: TNewStaticText; read;
	property YesRadio: TNewRadioButton; read;
	property NoRadio: TNewRadioButton; read;
	property WizardBitmapImage2: TBitmapImage; read;
	property WelcomeLabel2: TNewStaticText; read;
	property LicenseLabel1: TNewStaticText; read;
	property LicenseMemo: TRichEditViewer; read;
	property InfoAfterMemo: TRichEditViewer; read;
	property InfoAfterClickLabel: TNewStaticText; read;
	property ComponentsList: TNewCheckListBox; read;
	property ComponentsDiskSpaceLabel: TNewStaticText; read;
	property BeveledLabel: TNewStaticText; read;
	property StatusLabel: TNewStaticText; read;
	property FilenameLabel: TNewStaticText; read;
	property ProgressGauge: TNewProgressBar; read;
	property SelectDirLabel: TNewStaticText; read;
	property SelectStartMenuFolderLabel: TNewStaticText; read;
	property SelectComponentsLabel: TNewStaticText; read;
	property SelectTasksLabel: TNewStaticText; read;
	property LicenseAcceptedRadio: TNewRadioButton; read;
	property LicenseNotAcceptedRadio: TNewRadioButton; read;
	property UserInfoNameLabel: TNewStaticText; read;
	property UserInfoNameEdit: TNewEdit; read;
	property UserInfoOrgLabel: TNewStaticText; read;
	property UserInfoOrgEdit: TNewEdit; read;
	property PreparingErrorBitmapImage: TBitmapImage; read;
	property PreparingLabel: TNewStaticText; read;
	property FinishedHeadingLabel: TNewStaticText; read;
	property UserInfoSerialLabel: TNewStaticText; read;
	property UserInfoSerialEdit: TNewEdit; read;
	property TasksList: TNewCheckListBox; read;
	property RunList: TNewCheckListBox; read;
	property DirBrowseButton: TNewButton; read;
	property GroupBrowseButton: TNewButton; read;
	property SelectDirBitmapImage: TBitmapImage; read;
	property SelectGroupBitmapImage: TBitmapImage; read;
	property SelectDirBrowseLabel: TNewStaticText; read;
	property SelectStartMenuFolderBrowseLabel: TNewStaticText; read;
	property PreparingYesRadio: TNewRadioButton; read;
	property PreparingNoRadio: TNewRadioButton; read;
	property PreparingMemo: TNewMemo; read;
	property CurPageID: Integer; read;
	function AdjustLabelHeight(ALabel: TNewStaticText): Integer;
	procedure IncTopDecHeight(AControl: TControl; Amount: Integer);
	property PrevAppDir: String; read;
end;

TUninstallProgressForm = class(TSetupForm)
	property OuterNotebook: TNewNotebook; read;
	property InnerPage: TNewNotebookPage; read;
	property InnerNotebook: TNewNotebook; read;
	property InstallingPage: TNewNotebookPage; read;
	property MainPanel: TPanel; read;
	property PageNameLabel: TNewStaticText; read;
	property PageDescriptionLabel: TNewStaticText; read;
	property WizardSmallBitmapImage: TBitmapImage; read;
	property Bevel1: TBevel; read;
	property StatusLabel: TNewStaticText; read;
	property ProgressBar: TNewProgressBar; read;
	property BeveledLabel: TNewStaticText; read;
	property Bevel: TBevel; read;
	property CancelButton: TNewButton; read;
end;
