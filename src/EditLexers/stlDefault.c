#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_NULL = {{
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Global[] = {
	{ STYLE_DEFAULT, NP2StyleX_DefaultCodeStyle, L"font:$(Code)" },
	{ STYLE_DEFAULT, NP2StyleX_DefaultTextStyle, L"font:$(Text)" },
	{ STYLE_LINENUMBER, NP2StyleX_MarginLineNumber, L"size:-2; fore:#2B91AF" },
	{ STYLE_BRACELIGHT, NP2StyleX_MatchingBrace, L"size:+1; bold; fore:#FF0000" },
	{ STYLE_BRACEBAD, NP2StyleX_MatchingBraceError, L"size:+1; bold; fore:#000080" },
	{ STYLE_CONTROLCHAR, NP2StyleX_ControlCharacter, L"size:-1" },
	{ STYLE_INDENTGUIDE, NP2StyleX_IndentationGuide, L"fore:#FF8000" },
	{ SCI_SETSELFORE + SCI_SETSELBACK, NP2StyleX_SelectedText, L"eolfilled; alpha:95" },
	{ SCI_SETWHITESPACEFORE + SCI_SETWHITESPACEBACK + SCI_SETWHITESPACESIZE, NP2StyleX_Whitespace, L"size:1; fore:#FF4000" },
	{ SCI_SETCARETLINEBACK, NP2StyleX_CurrentLine, L"size:2; fore:#C2C0C3; back:#FFFF00; alpha:90" },
	{ SCI_SETCARETFORE + SCI_SETCARETWIDTH, NP2StyleX_Caret, L"" },
	{ 0, NP2StyleX_IMEIndicator, L"fore:#108010" },
	{ SCI_SETEDGECOLOUR, NP2StyleX_LongLineMarker, L"fore:#FFC000" },
	{ SCI_SETEXTRAASCENT + SCI_SETEXTRADESCENT, NP2StyleX_ExtraLineSpacing, L"size:2" },
	{ 0, NP2StyleX_FoldingMarker,  L"fore:#8080FF; back:#ADD8E6" },
	{ STYLE_FOLDDISPLAYTEXT, NP2StyleX_FoldEllipsis, L"font:$(Code); size:-2; bold; fore:#808080" },
	{ 0, NP2StyleX_MarkOccurrences, L"alpha:100" },
	{ 0, NP2StyleX_Bookmark, L"fore:#408040; back:#00FF00; alpha:40" },
	{ STYLE_CALLTIP, NP2StyleX_CallTips, L"" },
};

static EDITSTYLE Styles_2ndGlobal[] = {
	{ STYLE_DEFAULT, NP2StyleX_DefaultCodeStyle, L"font:$(Code); fore:#D4D4D4; back:#1E1E1E" },
	{ STYLE_DEFAULT, NP2StyleX_DefaultTextStyle, L"font:$(Text); fore:#D4D4D4; back:#1E1E1E" },
	{ STYLE_LINENUMBER, NP2StyleX_MarginLineNumber, L"size:-2; fore:#A0A0A0; back:#2A2A2E" },
	{ STYLE_BRACELIGHT, NP2StyleX_MatchingBrace, L"size:+1; bold; fore:#FF0000" },
	{ STYLE_BRACEBAD, NP2StyleX_MatchingBraceError, L"size:+1; bold; fore:#FFFF00" },
	{ STYLE_CONTROLCHAR, NP2StyleX_ControlCharacter, L"size:-1" },
	{ STYLE_INDENTGUIDE, NP2StyleX_IndentationGuide, L"fore:#605F63" },
	{ SCI_SETSELFORE + SCI_SETSELBACK, NP2StyleX_SelectedText, L"eolfilled; alpha:95" },
	{ SCI_SETWHITESPACEFORE + SCI_SETWHITESPACEBACK + SCI_SETWHITESPACESIZE, NP2StyleX_Whitespace, L"size:1; fore:#FF4000" },
	{ SCI_SETCARETLINEBACK, NP2StyleX_CurrentLine, L"size:2; fore:#C2C0C3; back:#FFFF00; alpha:25" },
	{ SCI_SETCARETFORE + SCI_SETCARETWIDTH, NP2StyleX_Caret, L"fore:#FFFFFF" },
	{ 0, NP2StyleX_IMEIndicator, L"fore:#108010" },
	{ SCI_SETEDGECOLOUR, NP2StyleX_LongLineMarker, L"fore:#605F63" },
	{ SCI_SETEXTRAASCENT + SCI_SETEXTRADESCENT, NP2StyleX_ExtraLineSpacing, L"size:2" },
	{ 0, NP2StyleX_FoldingMarker, L"fore:#808080; back:#606060" },
	{ STYLE_FOLDDISPLAYTEXT, NP2StyleX_FoldEllipsis, L"font:$(Code); size:-2; bold; fore:#606060" },
	{ 0, NP2StyleX_MarkOccurrences, L"alpha:100" },
	{ 0, NP2StyleX_Bookmark, L"fore:#408040; back:#00FF00; alpha:40" },
	{ STYLE_CALLTIP, NP2StyleX_CallTips, L"" },
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
	{ STYLE_DEFAULT, NP2StyleX_Default, L"font:Lucida Console" },
	{ STYLE_LINENUMBER, NP2StyleX_MarginLineNumber, L"font:Lucida Console; size:-2" },
	{ STYLE_BRACELIGHT, NP2StyleX_MatchingBrace, L"size:+0" },
	{ STYLE_BRACEBAD, NP2StyleX_MatchingBraceError, L"size:+0" },
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
	{ SCE_CONF_DIRECTIVE, NP2StyleX_Directive, L"fore:#003CE6" },
	{ SCE_CONF_COMMENT, NP2StyleX_Comment, L"fore:#008000" },
	{ SCE_CONF_STRING, NP2StyleX_String, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_CONF_NUMBER, SCE_CONF_HEXNUM, 0, 0), NP2StyleX_Number, L"fore:#FF4000" },
	{ SCE_CONF_SECTION, NP2StyleX_Section, L"bold; back:#FFC040; eolfilled" },
	//{ SCE_CONF_IP, EDITSTYLE_HOLE(L"IP Address"), L"bold; fore:#FF4000" },
	//{ SCE_CONF_IDENTIFIER, EDITSTYLE_HOLE(L"Identifier"), L"" },
	{ SCE_CONF_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
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
	{ SCE_PROPS_KEY, NP2StyleX_Property, L"fore:#A46000" },
	{ SCE_PROPS_COMMENT, NP2StyleX_Comment, L"fore:#008000" },
	{ SCE_PROPS_SECTION, NP2StyleX_Section, L"bold; back:#FFC040; eolfilled" },
	{ SCE_PROPS_ASSIGNMENT, NP2StyleX_Assignment, L"fore:#FF0000" },
	{ SCE_PROPS_DEFVAL, NP2StyleX_DefaultValue, L"fore:#FF0000" },
};

EDITLEXER lexINI = {
	SCLEX_PROPERTIES, NP2LEX_INI,
	EDITLEXER_HOLE(L"Ini Config File", Styles_INI),
	L"ini; inf; reg; oem; sif; url; sed; theme; clw; abnf",
	&Keywords_NULL,
	Styles_INI
};

static EDITSTYLE Styles_DIFF[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_DIFF_COMMENT, NP2StyleX_Comment, L"fore:#008000" },
	{ SCE_DIFF_COMMAND, NP2StyleX_Command, L"bold; fore:#0A246A" },
	{ SCE_DIFF_HEADER, NP2StyleX_SourceDestination, L"fore:#C80000; back:#FFF1A8; eolfilled" },
	{ SCE_DIFF_POSITION, NP2StyleX_PositionSetting, L"fore:#0000FF" },
	{ MULTI_STYLE(SCE_DIFF_ADDED, SCE_DIFF_PATCH_ADD, SCE_DIFF_REMOVED_PATCH_ADD, 0), NP2StyleX_LineAddition, L"fore:#002000; back:#80FF80; eolfilled" },
	{ MULTI_STYLE(SCE_DIFF_DELETED, SCE_DIFF_PATCH_DELETE, SCE_DIFF_REMOVED_PATCH_DELETE, 0), NP2StyleX_LineRemoval, L"fore:#200000; back:#FF8080; eolfilled" },
	{ SCE_DIFF_CHANGED, NP2StyleX_LineChange, L"fore:#000020; back:#8080FF; eolfilled" },
};

EDITLEXER lexDIFF = {
	SCLEX_DIFF, NP2LEX_DIFF,
	EDITLEXER_HOLE(L"Diff File", Styles_DIFF),
	L"diff; patch",
	&Keywords_NULL,
	Styles_DIFF
};
