#include "EditLexer.h"
#include "EditStyleX.h"

// https://en.wikibooks.org/wiki/Windows_Batch_Scripting
// https://www.robvanderwoude.com/battech.php

static KEYWORDLIST Keywords_Batch = {{
"defined do not else for goto if in "
"assoc break call cd chdir cls color copy date del dir echo echo. endlocal erase exist exit ftype md mkdir move path pause popd prompt pushd rd rem ren rename rmdir set setlocal shift sort start time title tree type ver verify vol "
"errorlevel nul disableextensions enableextensions enabledelayedexpansion disabledelayedexpansion "
"and or equ neq lss leq gtr geq "
//"change com con country ctty forfiles icacls kill loadhigh lpt msiexe robocopy sclist timeout "
"mklink "
"find findstr "

, // 1 system cmd
NULL
/* // com
"chcp diskcomp diskcopy format graftabl help loadfix mode more tree "
*/
/* // exe
"at arp attrib bootcfg cacls chkdsk chkntfs cipher cleanmgr cmd comp compact convert defrag diskpart doskey driverquery expand fc fsutil ftp gpresult gpupdate ipconfig label logman logoff mem msg nbtstat net netstat netsh nslookup openfiles pathping perfmon powercfg print recover reg regedit regsvr32 replace route runas rundll32 sc schtasks sfc shutdown sort subst systeminfo taskkill tasklist tracert typeperf wmic xcopy "
*/
/* // unknown
"bcdedit choice cmdkey "
*/
, // 2 Built-in Variables
"dp0 __APPDIR__ __AppDir__ __CD__ CMDCMDLINE CmdCmdLine CMDEXTVERSION CmdExtVersion HIGHESTNUMANODENUMBER HighestNumaNodeNumber RANDOM Random random "

, // 3 Options
"eol skip delims tokens usebackq " // FOR
"eof EOF " // special label

, NULL, NULL

, // 6 Upper Case
"DEFINED DO NOT ELSE FOR GOTO IF IN "
"ASSOC BREAK CALL CD CHDIR CLS COLOR COPY DATE DEL DIR ECHO ECHO. ENDLOCAL ERASE EXIST EXIT FTYPE MD MKDIR MOVE PATH PAUSE POPD PROMPT PUSHD RD REM REN RENAME RMDIR SET SETLOCAL SHIFT SORT START TIME TITLE TREE TYPE VER VERIFY VOL "
"ERRORLEVEL NUL DISABLEEXTENSIONS ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION DISABLEDELAYEDEXPANSION "
"ErrorLevel EndLocal SetLocal DisableExtensions EnableExtensions EnableDelayedExpansion DisableDelayedExpansion "
"AND OR EQU NEQ LSS LEQ GTR GEQ "
//"CHANGE COM CON COUNTRY CTTY FORFILES ICACLS KILL LOADHIGH LPT MSIEXE ROBOCOPY SCLIST TIMEOUT "
"MKLINK "
"FIND FINDSTR "

, NULL , NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Batch[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_BAT_COMMENT, NP2StyleX_Comment, L"fore:#008000" },
	{ SCE_BAT_WORD, NP2StyleX_Keyword, L"bold; fore:#FF8000" },
	{ SCE_BAT_COMMAND, NP2StyleX_Command, L"bold; fore:#0080FF" },
	{ SCE_BAT_STRINGDQ, NP2StyleX_DoubleQuotedString, L"fore:#008080" },
	{ SCE_BAT_STRINGSQ, NP2StyleX_SingleQuotedString, L"fore:#C08000" },
	{ SCE_BAT_STRINGBT, NP2StyleX_Backticks, L"fore:#F08000" },
	{ SCE_BAT_ESCAPE, NP2StyleX_EscapeSequence, L"fore:#FF0000" },
	{ SCE_BAT_VARIABLE, NP2StyleX_Variable, L"fore:#003CE6; back:#FFF1A8" },
	{ SCE_BAT_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_BAT_LABEL, SCE_BAT_LABEL_LINE, 0, 0), NP2StyleX_Label, L"fore:#C80000; back:#F4F4F4; eolfilled" },
};

EDITLEXER lexBatch = {
	SCLEX_BATCH, NP2LEX_BATCH,
	EDITLEXER_HOLE(L"Batch File", Styles_Batch),
	L"bat; cmd",
	&Keywords_Batch,
	Styles_Batch
};
