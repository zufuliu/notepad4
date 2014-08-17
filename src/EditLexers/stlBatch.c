#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_Batch = {
"defined do not else for goto if in "
"assoc break call cd chdir cls color copy date del dir echo echo. endlocal erase exist exit ftype md mkdir move path pause popd prompt pushd rd rem ren rename rmdir set setlocal shift sort start time title tree type ver verify vol "
"errorlevel nul disableextensions enableextensions enabledelayedexpansion "
//"change com con country ctty equ forfiles geq gtr icacls kill leq loadhigh lpt lss mklink msiexe neq robocopy sclist timeout "
, ""// system cmd
/* // com
"chcp diskcomp diskcopy format graftabl help loadfix mode more tree "
*/
/* // exe
"at arp attrib bootcfg cacls chkdsk chkntfs cipher cleanmgr cmd comp compact convert defrag diskpart doskey driverquery expand fc find findstr fsutil ftp gpresult gpupdate ipconfig label logman logoff mem msg nbtstat net netstat netsh nslookup openfiles pathping perfmon powercfg print recover reg regedit regsvr32 replace route runas rundll32 sc schtasks sfc shutdown sort subst systeminfo taskkill tasklist tracert typeperf wmic xcopy "
*/
/* // unknown
"bcdedit choice cmdkey "
*/
, "dp0", "", "", "", "", "", ""

#if NUMKEYWORD == 16
,"","","","","","",""
#endif
};

EDITLEXER lexBatch = { SCLEX_BATCH, NP2LEX_BATCH, L"Batch File", L"bat; cmd", L"", &Keywords_Batch,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_BAT_DEFAULT, L"Default", L"", L"" },
	{ SCE_BAT_COMMENT, NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_BAT_WORD, NP2STYLE_Keyword, L"Keyword", L"bold; fore:#FF8000", L"" },
	{ SCE_BAT_COMMAND, NP2STYLE_Command, L"Command", L"bold", L"" },
	{ MULTI_STYLE(SCE_BAT_STRINGDQ,SCE_BAT_STRINGSQ,SCE_BAT_STRINGBT,0), NP2STYLE_String, L"String", L"fore:#008080", L"" },
	{ SCE_BAT_VARIABLE, NP2STYLE_Variable, L"Variable", L"fore:#003CE6; back:#FFF1A8", L"" },
	{ SCE_BAT_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ SCE_BAT_LABEL, NP2STYLE_Label, L"Label", L"fore:#C80000; back:#F4F4F4", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
