#include "EditLexer.h"
#include "EditStyle.h"

// http://www.tcl.tk/about/language.html

static KEYWORDLIST Keywords_Tcl = {{
// TCL Keywords
"after append array auto_execok auto_import auto_load auto_load_index auto_qualify beep "
"bgerror binary break case catch cd clock close concat continue dde default echo else "
"elseif encoding eof error eval exec exit expr fblocked fconfigure fcopy file fileevent "
"flush for foreach format gets glob global history http if incr info interp join lappend "
"lindex linsert list llength load loadTk lrange lreplace lsearch lset lsort memory msgcat "
"namespace open package pid pkg::create pkg_mkIndex Platform-specific proc puts pwd "
"re_syntax read regexp registry regsub rename resource return scan seek set socket source "
"split string subst switch tclLog tclMacPkgSearch tclPkgSetup tclPkgUnknown tell time trace "
"unknown unset update uplevel upvar variable vwait while",
// TK Keywords
"bell bind bindtags bitmap button canvas checkbutton clipboard colors console cursors "
"destroy entry event focus font frame grab grid image Inter-client keysyms label labelframe "
"listbox lower menu menubutton message option options pack panedwindow photo place "
"radiobutton raise scale scrollbar selection send spinbox text tk tk_chooseColor "
"tk_chooseDirectory tk_dialog tk_focusNext tk_getOpenFile tk_messageBox tk_optionMenu "
"tk_popup tk_setPalette tkerror tkvars tkwait toplevel winfo wish wm",
// iTCL Keywords
"@scope body class code common component configbody constructor define destructor hull "
"import inherit itcl itk itk_component itk_initialize itk_interior itk_option iwidgets keep "
"method private protected public",
"", "", "", "", "", ""

#if NUMKEYWORD == 16
, "", "", "", "", "", "", ""
#endif
}};

#define SCE_TCL__MULTI_COMMENT      MULTI_STYLE(SCE_TCL_COMMENT, SCE_TCL_COMMENTLINE, SCE_TCL_COMMENT_BOX, SCE_TCL_BLOCK_COMMENT)
#define SCE_TCL__MULTI_KEYWORD      MULTI_STYLE(SCE_TCL_WORD, SCE_TCL_WORD2, SCE_TCL_WORD3, SCE_TCL_WORD_IN_QUOTE)
#define SCE_TCL__MULTI_SUBSTITUTION MULTI_STYLE(SCE_TCL_SUBSTITUTION, SCE_TCL_SUB_BRACE, 0, 0)

EDITLEXER lexTcl = { SCLEX_TCL, NP2LEX_TCL, L"Tcl Script", L"tcl; itcl; tm", L"", &Keywords_Tcl,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_TCL__MULTI_KEYWORD, NP2STYLE_Keyword, L"Keyword", L"bold; fore:#0000FF", L"" },
	{ SCE_TCL__MULTI_COMMENT, NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_TCL_IN_QUOTE, NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_TCL_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_TCL_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ SCE_TCL__MULTI_SUBSTITUTION, 63441, L"Substitution", L"fore:#0080FF", L"" },
	{ SCE_TCL_MODIFIER, 63442, L"Modifier", L"fore:#FF00FF", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
