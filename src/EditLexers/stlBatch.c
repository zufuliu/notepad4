#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikibooks.org/wiki/Windows_Batch_Scripting

static KEYWORDLIST Keywords_Batch = {{
"defined do not else for goto if in "
"assoc break call cd chdir cls color copy date del dir echo echo. endlocal erase exist exit ftype md mkdir move path pause popd prompt pushd rd rem ren rename rmdir set setlocal shift sort start time title tree type ver verify vol "
"errorlevel nul disableextensions enableextensions enabledelayedexpansion "
"and or equ neq lss leq gtr geq "
//"change com con country ctty forfiles icacls kill loadhigh lpt msiexe robocopy sclist timeout "
"mklink "

, // 1 system cmd
""
/* // com
"chcp diskcomp diskcopy format graftabl help loadfix mode more tree "
*/
/* // exe
"at arp attrib bootcfg cacls chkdsk chkntfs cipher cleanmgr cmd comp compact convert defrag diskpart doskey driverquery expand fc find findstr fsutil ftp gpresult gpupdate ipconfig label logman logoff mem msg nbtstat net netstat netsh nslookup openfiles pathping perfmon powercfg print recover reg regedit regsvr32 replace route runas rundll32 sc schtasks sfc shutdown sort subst systeminfo taskkill tasklist tracert typeperf wmic xcopy "
*/
/* // unknown
"bcdedit choice cmdkey "
*/
, // 2
"dp0"

, "", "", ""

, // 6 Upper Case
"DEFINED DO NOT ELSE FOR GOTO IF IN "
"ASSOC BREAK CALL CD CHDIR CLS COLOR COPY DATE DEL DIR ECHO ECHO. ENDLOCAL ERASE EXIST EXIT FTYPE MD MKDIR MOVE PATH PAUSE POPD PROMPT PUSHD RD REM REN RENAME RMDIR SET SETLOCAL SHIFT SORT START TIME TITLE TREE TYPE VER VERIFY VOL "
"ERRORLEVEL NUL DISABLEEXTENSIONS ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION "
"AND OR EQU NEQ LSS LEQ GTR GEQ "
//"CHANGE COM CON COUNTRY CTTY FORFILES ICACLS KILL LOADHIGH LPT MSIEXE ROBOCOPY SCLIST TIMEOUT "
"MKLINK "

, "" , ""

, "", "", "", "", "", "", ""
}};


EDITLEXER lexBatch = { SCLEX_BATCH, NP2LEX_BATCH, EDITLEXER_HOLE(L"Batch File"), L"bat; cmd", &Keywords_Batch,
{
	EDITSTYLE_DEFAULT,
	{ SCE_BAT_COMMENT, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#008000" },
	{ SCE_BAT_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"bold; fore:#FF8000" },
	{ SCE_BAT_COMMAND, NP2STYLE_Command, EDITSTYLE_HOLE(L"Command"), L"bold; fore:#0080FF" },
	{ MULTI_STYLE(SCE_BAT_STRINGDQ, SCE_BAT_STRINGSQ, SCE_BAT_STRINGBT, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008080" },
	{ SCE_BAT_VARIABLE, NP2STYLE_Variable, EDITSTYLE_HOLE(L"Variable"), L"fore:#003CE6; back:#FFF1A8" },
	{ SCE_BAT_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ SCE_BAT_LABEL, NP2STYLE_Label, EDITSTYLE_HOLE(L"Label"), L"fore:#C80000; back:#F4F4F4" },
	EDITSTYLE_SENTINEL
}
};

