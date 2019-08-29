#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_NULL = {{
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Global[] = {
	{ STYLE_DEFAULT, 63114, EDITSTYLE_HOLE(L"Default Code Style"), L"font:$(Code)" },
	{ STYLE_DEFAULT, 63115, EDITSTYLE_HOLE(L"Default Text Style"), L"font:$(Text)" },
	{ STYLE_LINENUMBER, 63101, EDITSTYLE_HOLE(L"Margin and Line Number"), L"size:-2; fore:#2B91AF" },
	{ STYLE_BRACELIGHT, 63102, EDITSTYLE_HOLE(L"Matching Brace"), L"size:+1; bold; fore:#FF0000" },
	{ STYLE_BRACEBAD, 63103, EDITSTYLE_HOLE(L"Matching Brace Error"), L"size:+1; bold; fore:#000080" },
	{ STYLE_CONTROLCHAR, 63104, EDITSTYLE_HOLE(L"Control Character"), L"size:-1" },
	{ STYLE_INDENTGUIDE, 63105, EDITSTYLE_HOLE(L"Indentation Guide"), L"fore:#FF8000" },
	{ SCI_SETSELFORE + SCI_SETSELBACK, 63106, EDITSTYLE_HOLE(L"Selected Text"), L"eolfilled; alpha:95" },
	{ SCI_SETWHITESPACEFORE + SCI_SETWHITESPACEBACK + SCI_SETWHITESPACESIZE, 63107, EDITSTYLE_HOLE(L"Whitespace"), L"size:1; fore:#FF4000" },
	{ SCI_SETCARETLINEBACK, 63108, EDITSTYLE_HOLE(L"Current Line"), L"size:2; fore:#C2C0C3; back:#FFFF00; alpha:90" },
	{ SCI_SETCARETFORE + SCI_SETCARETWIDTH, 63109, EDITSTYLE_HOLE(L"Caret"), L"" },
	{ 0, 63117, EDITSTYLE_HOLE(L"IME Indicator"), L"fore:#108010" },
	{ SCI_SETEDGECOLOUR, 63110, EDITSTYLE_HOLE(L"Long Line Marker"), L"fore:#FFC000" },
	{ SCI_SETEXTRAASCENT + SCI_SETEXTRADESCENT, 63111, EDITSTYLE_HOLE(L"Extra Line Spacing"), L"size:2" },
	{ 0, 63112, EDITSTYLE_HOLE(L"Folding Marker"), L"fore:#8080FF; back:#ADD8E6" },
	{ STYLE_FOLDDISPLAYTEXT, 63116, EDITSTYLE_HOLE(L"Fold Ellipsis"), L"font:$(Code); size:-2; bold; fore:#808080" },
	{ 0, 63113, EDITSTYLE_HOLE(L"Mark Occurrences"), L"alpha:100" },
	{ 0, 63118, EDITSTYLE_HOLE(L"Bookmark"), L"fore:#408040; back:#00FF00; alpha:40" },
	{ STYLE_CALLTIP, 63119, EDITSTYLE_HOLE(L"CallTips"), L"" },
};

static EDITSTYLE Styles_2ndGlobal[] = {
	{ STYLE_DEFAULT, 63114, EDITSTYLE_HOLE(L"Default Code Style"), L"font:$(Code); fore:#D4D4D4; back:#1E1E1E" },
	{ STYLE_DEFAULT, 63115, EDITSTYLE_HOLE(L"Default Text Style"), L"font:$(Text); fore:#D4D4D4; back:#1E1E1E" },
	{ STYLE_LINENUMBER, 63101, EDITSTYLE_HOLE(L"Margin and Line Number"), L"size:-2; fore:#A0A0A0; back:#2A2A2E" },
	{ STYLE_BRACELIGHT, 63102, EDITSTYLE_HOLE(L"Matching Brace"), L"size:+1; bold; fore:#FF0000" },
	{ STYLE_BRACEBAD, 63103, EDITSTYLE_HOLE(L"Matching Brace Error"), L"size:+1; bold; fore:#FFFF00" },
	{ STYLE_CONTROLCHAR, 63104, EDITSTYLE_HOLE(L"Control Character"), L"size:-1" },
	{ STYLE_INDENTGUIDE, 63105, EDITSTYLE_HOLE(L"Indentation Guide"), L"fore:#605F63" },
	{ SCI_SETSELFORE + SCI_SETSELBACK, 63106, EDITSTYLE_HOLE(L"Selected Text"), L"eolfilled; alpha:95" },
	{ SCI_SETWHITESPACEFORE + SCI_SETWHITESPACEBACK + SCI_SETWHITESPACESIZE, 63107, EDITSTYLE_HOLE(L"Whitespace"), L"size:1; fore:#FF4000" },
	{ SCI_SETCARETLINEBACK, 63108, EDITSTYLE_HOLE(L"Current Line"), L"size:2; fore:#C2C0C3; back:#FFFF00; alpha:25" },
	{ SCI_SETCARETFORE + SCI_SETCARETWIDTH, 63109, EDITSTYLE_HOLE(L"Caret"), L"fore:#FFFFFF" },
	{ 0, 63117, EDITSTYLE_HOLE(L"IME Indicator"), L"fore:#108010" },
	{ SCI_SETEDGECOLOUR, 63110, EDITSTYLE_HOLE(L"Long Line Marker"), L"fore:#605F63" },
	{ SCI_SETEXTRAASCENT + SCI_SETEXTRADESCENT, 63111, EDITSTYLE_HOLE(L"Extra Line Spacing"), L"size:2" },
	{ 0, 63112, EDITSTYLE_HOLE(L"Folding Marker"), L"fore:#808080; back:#606060" },
	{ STYLE_FOLDDISPLAYTEXT, 63116, EDITSTYLE_HOLE(L"Fold Ellipsis"), L"font:$(Code); size:-2; bold; fore:#606060" },
	{ 0, 63113, EDITSTYLE_HOLE(L"Mark Occurrences"), L"alpha:100" },
	{ 0, 63118, EDITSTYLE_HOLE(L"Bookmark"), L"fore:#408040; back:#00FF00; alpha:40" },
	{ STYLE_CALLTIP, 63119, EDITSTYLE_HOLE(L"CallTips"), L"" },
};

EDITLEXER lexGlobal = {
	SCLEX_NULL, NP2LEX_GLOBAL,
	EDITLEXER_HOLE(L"Global Styles", Styles_Global),
	NULL,
	NULL,
	Styles_Global
};

EDITLEXER lex2ndGlobal = {
	SCLEX_NULL, NP2LEX_2NDGLOBAL,
	EDITLEXER_HOLE(L"2nd Global Styles", Styles_2ndGlobal),
	NULL,
	NULL,
	Styles_2ndGlobal
};

static EDITSTYLE Styles_Text[] =  {
	EDITSTYLE_DEFAULT,
};

static EDITSTYLE Styles_2ndText[] =  {
	EDITSTYLE_DEFAULT,
};

EDITLEXER lexTextFile = {
	SCLEX_NULL, NP2LEX_TEXTFILE,
	EDITLEXER_TEXT(L"Text File", Styles_Text),
	L"txt; text; wtx; doc",
	&Keywords_NULL,
	Styles_Text
};

EDITLEXER lex2ndTextFile = {
	SCLEX_NULL, NP2LEX_2NDTEXTFILE,
	EDITLEXER_HOLE(L"2nd Text File", Styles_2ndText),
	L"log; asc",
	&Keywords_NULL,
	Styles_2ndText
};

static EDITSTYLE Styles_ANSI[] = {
	{ STYLE_DEFAULT, NP2STYLE_Default, EDITSTYLE_HOLE(L"Default"), L"font:Lucida Console" },
	{ STYLE_LINENUMBER, 63101, EDITSTYLE_HOLE(L"Margin and Line Number"), L"font:Lucida Console; size:-2" },
	{ STYLE_BRACELIGHT, 63102, EDITSTYLE_HOLE(L"Matching Brace"), L"size:+0" },
	{ STYLE_BRACEBAD, 63103, EDITSTYLE_HOLE(L"Matching Brace Error"), L"size:+0" },
};

EDITLEXER lexANSI = {
	SCLEX_NULL, NP2LEX_ANSI,
	EDITLEXER_HOLE(L"ANSI Art", Styles_ANSI),
	L"nfo; diz",
	&Keywords_NULL,
	Styles_ANSI
};

static EDITSTYLE Styles_CONF[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_CONF_DIRECTIVE, NP2STYLE_Directive, EDITSTYLE_HOLE(L"Directive"), L"fore:#003CE6" },
	{ SCE_CONF_COMMENT, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#008000" },
	{ SCE_CONF_STRING, NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_CONF_NUMBER, SCE_CONF_HEXNUM, 0, 0), NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF4000" },
	{ SCE_CONF_SECTION, 63511, EDITSTYLE_HOLE(L"Section"), L"bold; back:#FFC040; eolfilled" },
	//{ SCE_CONF_IP, 63521, EDITSTYLE_HOLE(L"IP Address"), L"bold; fore:#FF4000" },
	//{ SCE_CONF_IDENTIFIER, EDITSTYLE_HOLE(L"Identifier"), L"" },
	{ SCE_CONF_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	//{ SCE_CONF_PARAMETER, EDITSTYLE_HOLE(L"Runtime Directive Parameter"), L"" },
	//{ SCE_CONF_EXTENSION, EDITSTYLE_HOLE(L"Extension"), L"" },
};

EDITLEXER lexCONF = {
	SCLEX_CONF, NP2LEX_CONF,
	EDITLEXER_HOLE(L"Config File", Styles_CONF),
	L"conf; cfg; cnf; htaccess; properties; prefs; iface; prop; po; te",
	&Keywords_NULL,
	Styles_CONF
};

static EDITSTYLE Styles_INI[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_PROPS_COMMENT, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#008000" },
	{ SCE_PROPS_SECTION, 63511, EDITSTYLE_HOLE(L"Section"), L"bold; back:#FFC040; eolfilled" },
	{ SCE_PROPS_ASSIGNMENT, 63512, EDITSTYLE_HOLE(L"Assignment"), L"fore:#FF0000" },
	{ SCE_PROPS_DEFVAL, 63513, EDITSTYLE_HOLE(L"Default Value"), L"fore:#FF0000" },
};

EDITLEXER lexINI = {
	SCLEX_PROPERTIES, NP2LEX_INI,
	EDITLEXER_HOLE(L"Ini Config File", Styles_INI),
	L"ini; inf; reg; oem; sif; url; sed; theme; clw; toml",
	&Keywords_NULL,
	Styles_INI
};

static EDITSTYLE Styles_DIFF[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_DIFF_COMMENT, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#008000" },
	{ SCE_DIFF_COMMAND, NP2STYLE_Command, EDITSTYLE_HOLE(L"Command"), L"bold; fore:#0A246A" },
	{ SCE_DIFF_HEADER, 63541, EDITSTYLE_HOLE(L"Source and Destination"), L"fore:#C80000; back:#FFF1A8; eolfilled" },
	{ SCE_DIFF_POSITION, 63542, EDITSTYLE_HOLE(L"Position Setting"), L"fore:#0000FF" },
	{ MULTI_STYLE(SCE_DIFF_ADDED, SCE_DIFF_PATCH_ADD, SCE_DIFF_REMOVED_PATCH_ADD, 0), 63543, EDITSTYLE_HOLE(L"Line Addition"), L"fore:#002000; back:#80FF80; eolfilled" },
	{ MULTI_STYLE(SCE_DIFF_DELETED, SCE_DIFF_PATCH_DELETE, SCE_DIFF_REMOVED_PATCH_DELETE, 0), 63544, EDITSTYLE_HOLE(L"Line Removal"), L"fore:#200000; back:#FF8080; eolfilled" },
	{ SCE_DIFF_CHANGED, 63545, EDITSTYLE_HOLE(L"Line Change"), L"fore:#000020; back:#8080FF; eolfilled" },
};

EDITLEXER lexDIFF = {
	SCLEX_DIFF, NP2LEX_DIFF,
	EDITLEXER_HOLE(L"Diff File", Styles_DIFF),
	L"diff; patch",
	&Keywords_NULL,
	Styles_DIFF
};

