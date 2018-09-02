#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_NULL = {{
"", "", "", "", "", "", "", "", ""

, "", "", "", "", "", "", ""
}};

EDITLEXER lexDefault = { SCLEX_NULL, NP2LEX_DEFAULT, L"Default Text", L"txt; text; wtx; log; asc; doc", L"", &Keywords_NULL,
{
	{ STYLE_DEFAULT, 63100, L"Default Style", L"font:Default; size:11", L"" },
	//{ STYLE_DEFAULT, 63100, L"Default Style", L"font:DejaVu Sans Mono; size:11", L"" },
	//{ STYLE_DEFAULT, 63100, L"Default Style", L"font:Consolas; size:11", L"" },
	{ STYLE_LINENUMBER, 63101, L"Margins and Line Numbers", L"size:-2; fore:#2B91AF", L"" },
	{ STYLE_BRACELIGHT, 63102, L"Matching Braces", L"size:+1; bold; fore:#FF0000", L"" },
	{ STYLE_BRACEBAD, 63103, L"Matching Braces Error", L"size:+1; bold; fore:#000080", L"" },
	{ STYLE_CONTROLCHAR, 63104, L"Control Characters (Font)", L"size:-1", L"" },
	{ STYLE_INDENTGUIDE, 63105, L"Indentation Guide (Color)", L"fore:#FF8000", L"" },
	{ SCI_SETSELFORE + SCI_SETSELBACK, 63106, L"Selected Text (Colors)", L"back:#0A246A; eolfilled; alpha:95", L"" },
	{ SCI_SETWHITESPACEFORE + SCI_SETWHITESPACEBACK + SCI_SETWHITESPACESIZE, 63107, L"Whitespace (Colors, Size 0-5)", L"fore:#FF4000", L"" },
	{ SCI_SETCARETLINEBACK, 63108, L"Current Line Background (Color)", L"back:#FFFF00; alpha:50", L"" },
	{ SCI_SETCARETFORE + SCI_SETCARETWIDTH, 63109, L"Caret (Color)", L"", L"" },
	{ SCI_SETEDGECOLOUR, 63110, L"Long Line Marker (Colors)", L"fore:#FFC000", L"" },
	{ SCI_SETEXTRAASCENT + SCI_SETEXTRADESCENT, 63111, L"Extra Line Spacing (Size)", L"size:2", L"" },

	{ STYLE_DEFAULT, 63112, L"2nd Default Style", L"font:Default; size:11", L"" },
	//{ STYLE_DEFAULT, 63112, L"2nd Default Style", L"font:Courier New; size:11", L"" },
	{ STYLE_LINENUMBER, 63113, L"2nd Margins and Line Numbers", L"font:Tahoma; size:-2; fore:#2B91AF", L"" },
	{ STYLE_BRACELIGHT, 63114, L"2nd Matching Braces", L"bold; fore:#FF0000", L"" },
	{ STYLE_BRACEBAD, 63115, L"2nd Matching Braces Error", L"bold; fore:#000080", L"" },
	{ STYLE_CONTROLCHAR, 63116, L"2nd Control Characters (Font)", L"size:-1", L"" },
	{ STYLE_INDENTGUIDE, 63117, L"2nd Indentation Guide (Color)", L"fore:#FF8000", L"" },
	{ SCI_SETSELFORE + SCI_SETSELBACK, 63118, L"2nd Selected Text (Colors)", L"eolfilled", L"" },
	{ SCI_SETWHITESPACEFORE + SCI_SETWHITESPACEBACK + SCI_SETWHITESPACESIZE, 63119, L"2nd Whitespace (Colors, Size 0-5)", L"fore:#FF4000", L"" },
	{ SCI_SETCARETLINEBACK, 63120, L"2nd Current Line Background (Color)", L"back:#FFFF00; alpha:50", L"" },
	{ SCI_SETCARETFORE + SCI_SETCARETWIDTH, 63121, L"2nd Caret (Color)", L"", L"" },
	{ SCI_SETEDGECOLOUR, 63122, L"2nd Long Line Marker (Colors)", L"fore:#FFC000", L"" },
	{ SCI_SETEXTRAASCENT + SCI_SETEXTRADESCENT, 63123, L"2nd Extra Line Spacing (Size)", L"", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};

EDITLEXER lexANSI = { SCLEX_NULL, NP2LEX_ANSI, L"ANSI Art", L"nfo; diz", L"", &Keywords_NULL,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"font:Lucida Console", L"" },
	{ STYLE_LINENUMBER, 63101, L"Margins and Line Numbers", L"font:Lucida Console; size:-2", L"" },
	{ STYLE_BRACELIGHT, 63102, L"Matching Braces", L"size:+0", L"" },
	{ STYLE_BRACEBAD, 63103, L"Matching Braces Error", L"size:+0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};

EDITLEXER lexCONF = { SCLEX_CONF, NP2LEX_CONF, L"Config File", L"conf; cfg; cnf; properties; prefs", L"", &Keywords_NULL,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_CONF_DIRECTIVE, NP2STYLE_Directive, L"Directive", L"fore:#003CE6", L"" },
	{ SCE_CONF_COMMENT, NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_CONF_STRING, NP2STYLE_String, L"String", L"fore:#B000B0", L"" },
	{ MULTI_STYLE(SCE_CONF_NUMBER, SCE_CONF_HEXNUM, 0, 0), NP2STYLE_Number, L"Number", L"fore:#FF4000", L"" },
	{ SCE_CONF_SECTION, 63511, L"Section", L"bold; fore:#000000; back:#FFC040; eolfilled", L"" },
	//{ SCE_CONF_IP, 63521, L"IP Address", L"bold; fore:#FF4000", L"" },
	//{ SCE_CONF_IDENTIFIER, L"Identifier", L"", L"" },
	{ SCE_CONF_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	//{ SCE_CONF_PARAMETER, L"Runtime Directive Parameter", L"", L"" },
	//{ SCE_CONF_EXTENSION, L"Extension", L"", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};

EDITLEXER lexINI = { SCLEX_PROPERTIES, NP2LEX_INI, L"Ini Config File", L"ini; inf; reg; oem; sif; url; sed; theme; clw", L"", &Keywords_NULL,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_PROPS_DEFAULT, L"Default", L"", L"" },
	{ SCE_PROPS_COMMENT, NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_PROPS_SECTION, 63511, L"Section", L"bold; fore:#000000; back:#FFC040; eolfilled", L"" },
	{ SCE_PROPS_ASSIGNMENT, 63512, L"Assignment", L"fore:#FF0000", L"" },
	{ SCE_PROPS_DEFVAL, 63513, L"Default Value", L"fore:#FF0000", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};

EDITLEXER lexDIFF = { SCLEX_DIFF, NP2LEX_DIFF, L"Diff File", L"diff; patch", L"", &Keywords_NULL,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_DIFF_DEFAULT, L"Default", L"", L"" },
	{ SCE_DIFF_COMMENT, NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_DIFF_COMMAND, NP2STYLE_Command, L"Command", L"bold; fore:#0A246A", L"" },
	{ SCE_DIFF_HEADER, 63541, L"Source and Destination", L"fore:#C80000; back:#FFF1A8; eolfilled", L"" },
	{ SCE_DIFF_POSITION, 63542, L"Position Setting", L"fore:#0000FF", L"" },
	{ MULTI_STYLE(SCE_DIFF_ADDED, SCE_DIFF_PATCH_ADD, SCE_DIFF_REMOVED_PATCH_ADD, 0), 63543, L"Line Addition", L"fore:#002000; back:#80FF80; eolfilled", L"" },
	{ MULTI_STYLE(SCE_DIFF_DELETED, SCE_DIFF_PATCH_DELETE, SCE_DIFF_REMOVED_PATCH_DELETE, 0), 63544, L"Line Removal", L"fore:#200000; back:#FF8080; eolfilled", L"" },
	{ SCE_DIFF_CHANGED, 63545, L"Line Change", L"fore:#000020; back:#8080FF; eolfilled", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};

