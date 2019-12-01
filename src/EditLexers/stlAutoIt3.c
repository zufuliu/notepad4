#include "EditLexer.h"
#include "EditStyleX.h"

// https://www.autoitscript.com/site/autoit/

static KEYWORDLIST Keywords_AU3 = {{
"and byref case const continuecase continueloop default dim do else elseif endfunc endif "
"endselect endswitch endwith enum exit exitloop false for func global if in local next not "
"or redim return select step switch then to true until wend while with",
"abs acos adlibdisable adlibenable asc ascw asin assign atan autoitsetoption autoitwingettitle "
"autoitwinsettitle beep binary binarylen binarymid binarytostring bitand bitnot bitor bitrotate "
"bitshift bitxor blockinput break call cdtray ceiling chr chrw clipget clipput consoleread "
"consolewrite consolewriteerror controlclick controlcommand controldisable controlenable "
"controlfocus controlgetfocus controlgethandle controlgetpos controlgettext controlhide "
"controllistview controlmove controlsend controlsettext controlshow controltreeview cos dec "
"dircopy dircreate dirgetsize dirmove dirremove dllcall dllcallbackfree dllcallbackgetptr "
"dllcallbackregister dllclose dllopen dllstructcreate dllstructgetdata dllstructgetptr "
"dllstructgetsize dllstructsetdata drivegetdrive drivegetfilesystem drivegetlabel drivegetserial "
"drivegettype drivemapadd drivemapdel drivemapget drivesetlabel drivespacefree drivespacetotal "
"drivestatus envget envset envupdate eval execute exp filechangedir fileclose filecopy "
"filecreatentfslink filecreateshortcut filedelete fileexists filefindfirstfile filefindnextfile "
"filegetattrib filegetlongname filegetshortcut filegetshortname filegetsize filegettime "
"filegetversion fileinstall filemove fileopen fileopendialog fileread filereadline filerecycle "
"filerecycleempty filesavedialog fileselectfolder filesetattrib filesettime filewrite filewriteline "
"floor ftpsetproxy guicreate guictrlcreateavi guictrlcreatebutton guictrlcreatecheckbox "
"guictrlcreatecombo guictrlcreatecontextmenu guictrlcreatedate guictrlcreatedummy guictrlcreateedit "
"guictrlcreategraphic guictrlcreategroup guictrlcreateicon guictrlcreateinput guictrlcreatelabel "
"guictrlcreatelist guictrlcreatelistview guictrlcreatelistviewitem guictrlcreatemenu "
"guictrlcreatemenuitem guictrlcreatemonthcal guictrlcreateobj guictrlcreatepic guictrlcreateprogress "
"guictrlcreateradio guictrlcreateslider guictrlcreatetab guictrlcreatetabitem guictrlcreatetreeview "
"guictrlcreatetreeviewitem guictrlcreateupdown guictrldelete guictrlgethandle guictrlgetstate "
"guictrlread guictrlrecvmsg guictrlregisterlistviewsort guictrlsendmsg guictrlsendtodummy "
"guictrlsetbkcolor guictrlsetcolor guictrlsetcursor guictrlsetdata guictrlsetdefbkcolor "
"guictrlsetdefcolor guictrlsetfont guictrlsetgraphic guictrlsetimage guictrlsetlimit guictrlsetonevent "
"guictrlsetpos guictrlsetresizing guictrlsetstate guictrlsetstyle guictrlsettip guidelete "
"guigetcursorinfo guigetmsg guigetstyle guiregistermsg guisetaccelerators guisetbkcolor guisetcoord "
"guisetcursor guisetfont guisethelp guiseticon guisetonevent guisetstate guisetstyle guistartgroup "
"guiswitch hex hotkeyset httpsetproxy hwnd inetget inetgetsize inidelete iniread inireadsection "
"inireadsectionnames inirenamesection iniwrite iniwritesection inputbox int isadmin "
"isarray isbinary isbool isdeclared isdllstruct isfloat ishwnd isint iskeyword isnumber "
"isobj isptr isstring log memgetstats mod mouseclick mouseclickdrag mousedown mousegetcursor "
"mousegetpos mousemove mouseup mousewheel msgbox number objcreate objevent objevent "
"objget objname opt ping pixelchecksum pixelgetcolor pixelsearch pluginclose pluginopen "
"processclose processexists processgetstats processlist processsetpriority processwait "
"processwaitclose progressoff progresson progressset ptr random regdelete regenumkey "
"regenumval regread regwrite round run runas runaswait runwait send sendkeepactive "
"seterror setextended shellexecute shellexecutewait shutdown sin sleep soundplay soundsetwavevolume "
"splashimageon splashoff splashtexton sqrt srandom statusbargettext stderrread stdinwrite "
"stdioclose stdoutread string stringaddcr stringcompare stringformat stringinstr stringisalnum "
"stringisalpha stringisascii stringisdigit stringisfloat stringisint stringislower "
"stringisspace stringisupper stringisxdigit stringleft stringlen stringlower stringmid "
"stringregexp stringregexpreplace stringreplace stringright stringsplit stringstripcr "
"stringstripws stringtobinary stringtrimleft stringtrimright stringupper tan tcpaccept "
"tcpclosesocket tcpconnect tcplisten tcpnametoip tcprecv tcpsend tcpshutdown tcpstartup "
"timerdiff timerinit tooltip traycreateitem traycreatemenu traygetmsg trayitemdelete "
"trayitemgethandle trayitemgetstate trayitemgettext trayitemsetonevent trayitemsetstate "
"trayitemsettext traysetclick trayseticon traysetonevent traysetpauseicon traysetstate "
"traysettooltip traytip ubound udpbind udpclosesocket udpopen udprecv udpsend udpshutdown "
"udpstartup vargettype winactivate winactive winclose winexists winflash wingetcaretpos "
"wingetclasslist wingetclientsize wingethandle wingetpos wingetprocess wingetstate "
"wingettext wingettitle winkill winlist winmenuselectitem winminimizeall winminimizeallundo "
"winmove winsetontop winsetstate winsettitle winsettrans winwait winwaitactive winwaitclose "
"winwaitnotactive"

, // 1
"@appdatacommondir @appdatadir @autoitexe @autoitpid @autoitunicode @autoitversion @autoitx64 "
"@com_eventobj @commonfilesdir @compiled @computername @comspec @cr @crlf @desktopcommondir "
"@desktopdepth @desktopdir @desktopheight @desktoprefresh @desktopwidth @documentscommondir "
"@error @exitcode @exitmethod @extended @favoritescommondir @favoritesdir @gui_ctrlhandle "
"@gui_ctrlid @gui_dragfile @gui_dragid @gui_dropid @gui_winhandle @homedrive @homepath @homeshare "
"@hotkeypressed @hour @inetgetactive @inetgetbytesread @ipaddress1 @ipaddress2 @ipaddress3 "
"@ipaddress4 @kblayout @lf @logondnsdomain @logondomain @logonserver @mday @min @mon @mydocumentsdir "
"@numparams @osbuild @oslang @osservicepack @ostype @osversion @processorarch @programfilesdir "
"@programscommondir @programsdir @scriptdir @scriptfullpath @scriptlinenumber @scriptname @sec "
"@startmenucommondir @startmenudir @startupcommondir @startupdir @sw_disable @sw_enable @sw_hide @sw_lock "
"@sw_maximize @sw_minimize @sw_restore @sw_show @sw_showdefault @sw_showmaximized "
"@sw_showminimized @sw_showminnoactive @sw_showna @sw_shownoactivate @sw_shownormal "
"@sw_unlock @systemdir @tab @tempdir @tray_id @trayiconflashing @trayiconvisible @username "
"@userprofiledir @wday @windowsdir @workingdir @yday @year"

, // 2
"{!} {#} {^} {{} {}} {+} {alt} {altdown} {altup} {appskey} "
"{asc} {backspace} {break} {browser_back} {browser_favorites} {browser_forward} {browser_home} "
"{browser_refresh} {browser_search} {browser_stop} {bs} {capslock} {ctrldown} {ctrlup} "
"{del} {delete} {down} {end} {enter} {esc} {escape} {f1} {f10} {f11} {f12} {f2} {f3} "
"{f4} {f5} {f6} {f7} {f8} {f9} {home} {ins} {insert} {lalt} {launch_app1} {launch_app2} "
"{launch_mail} {launch_media} {lctrl} {left} {lshift} {lwin} {lwindown} {lwinup} {media_next} "
"{media_play_pause} {media_prev} {media_stop} {numlock} {numpad0} {numpad1} {numpad2} "
"{numpad3} {numpad4} {numpad5} {numpad6} {numpad7} {numpad8} {numpad9} {numpadadd} "
"{numpaddiv} {numpaddot} {numpadenter} {numpadmult} {numpadsub} {pause} {pgdn} {pgup} "
"{printscreen} {ralt} {rctrl} {right} {rshift} {rwin} {rwindown} {rwinup} {scrolllock} "
"{shiftdown} {shiftup} {sleep} {space} {tab} {up} {volume_down} {volume_mute} {volume_up}"

, // 3
"#ce #comments-end #comments-start #cs #include #include-once #notrayicon #requireadmin"

, // 4
"#autoit3wrapper_au3check_parameters #autoit3wrapper_au3check_stop_onwarning "
"#autoit3wrapper_change2cui #autoit3wrapper_compression #autoit3wrapper_cvswrapper_parameters "
"#autoit3wrapper_icon #autoit3wrapper_outfile #autoit3wrapper_outfile_type #autoit3wrapper_plugin_funcs "
"#autoit3wrapper_res_comment #autoit3wrapper_res_description #autoit3wrapper_res_field "
"#autoit3wrapper_res_file_add #autoit3wrapper_res_fileversion #autoit3wrapper_res_fileversion_autoincrement "
"#autoit3wrapper_res_icon_add #autoit3wrapper_res_language #autoit3wrapper_res_legalcopyright "
"#autoit3wrapper_res_requestedexecutionlevel #autoit3wrapper_res_savesource #autoit3wrapper_run_after "
"#autoit3wrapper_run_au3check #autoit3wrapper_run_before #autoit3wrapper_run_cvswrapper "
"#autoit3wrapper_run_debug_mode #autoit3wrapper_run_obfuscator #autoit3wrapper_run_tidy "
"#autoit3wrapper_tidy_stop_onerror #autoit3wrapper_useansi #autoit3wrapper_useupx "
"#autoit3wrapper_usex64 #autoit3wrapper_version #endregion #forceref #obfuscator_ignore_funcs "
"#obfuscator_ignore_variables #obfuscator_parameters #region #tidy_parameters"

, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_AU3[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_AU3_KEYWORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_AU3_FUNCTION, NP2StyleX_Function, L"fore:#0000FF" },
	{ SCE_AU3_MACRO, NP2StyleX_Macro, L"fore:#0080FF" },
	{ MULTI_STYLE(SCE_AU3_COMMENT, SCE_AU3_COMMENTBLOCK, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_AU3_STRING, NP2StyleX_String, L"fore:#008080" },
	{ SCE_AU3_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_AU3_OPERATOR, NP2StyleX_Operator, L"fore:#C000C0" },
	{ SCE_AU3_VARIABLE, NP2StyleX_Variable, L"fore:#808000" },
	{ SCE_AU3_SENT, NP2StyleX_SendKey, L"fore:#FF0000" },
	{ SCE_AU3_PREPROCESSOR, NP2StyleX_Preprocessor, L"fore:#FF8000" },
	{ SCE_AU3_SPECIAL, NP2StyleX_Special, L"fore:#FF8000" },
};

EDITLEXER lexAU3 = {
	SCLEX_AU3, NP2LEX_AU3,
	EDITLEXER_HOLE(L"AutoIt3 Script", Styles_AU3),
	L"au3",
	&Keywords_AU3,
	Styles_AU3
};
