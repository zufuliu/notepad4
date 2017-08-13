#include "EditLexer.h"
#include "EditStyle.h"

// http://www.latex-project.org/

static KEYWORDLIST Keywords_TEX = {{
"", "", "", "", "", "", "", "", ""

#if NUMKEYWORD == 16
, "", "", "", "", "", "", ""
#endif
}};

EDITLEXER lexLaTeX = { SCLEX_LATEX, NP2LEX_LATEX, L"LaTeX File", L"tex; latex; sty; cls; tpx; bbl; bib; ltx; dtx; ins; toc; info", L"", &Keywords_TEX,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_L_COMMAND, NP2STYLE_Command, L"Command", L"fore:#0000FF", L"" },
	{ MULTI_STYLE(SCE_L_COMMENT, SCE_L_COMMENT2, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_L_TITLE, 63575, L"Title", L"fore:#008000", L"" },
	{ SCE_L_CHAPTER, 63579, L"Chapter", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_L_SECTION, SCE_L_SECTION1, SCE_L_SECTION2, 0), 63511, L"Section", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_L_MATH, SCE_L_MATH2, 0, 0), 63571, L"Math", L"fore:#FF0000", L"" },
	{ SCE_L_SPECIAL, 63572, L"Special Char", L"fore:#3A6EA5", L"" },
	{ MULTI_STYLE(SCE_L_TAG, SCE_L_TAG2, 0, 0), 63573, L"Tag", L"fore:#FF8000", L"" },
	{ SCE_L_CMDOPT, 63574, L"Option", L"fore:#1E90FF", L"" },
	{ SCE_L_STRING, NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_L_QUOTE1, SCE_L_QUOTE2, 0, 0), 63576, L"Quote", L"fore:#408080", L""},
	{ MULTI_STYLE(SCE_L_VERBATIM, SCE_L_VERBATIM2, 0, 0), 63577, L"Verbatim Segment", L"fore:#666666", L"" },
	{ SCE_L_LISTCODE, 63578, L"List Code", L"fore:#808080", L"" },
	{ SCE_L_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};


// https://www.gnu.org/software/texinfo/

static KEYWORDLIST Keywords_TEXI = {{
// cmd
"end setfilename settitle bye insertcopying documentencoding documentlanguage dircategory "
"shorttitlepage center titlefont title subtitle author "
"shortcontents summarycontents contents setcontentsaftertitlepage setshortcontentsaftertitlepage "
"node top novalidate lowersections raisesections part charpter centerchap section subsection subsubsection unnumbered "
, // fold
"copying frenchspacing direntry documentdescription titlepage menu detailmenu cartouche quotation smallquotation indentedblock smallindentedblock raggedright format smallformat display smalldisplay flushleft flushright example smallexample lisp smalllist verbatim itemize enumerate table ftable vtable multitable float group "
"deffn defun defmac deftypefn deftypefun defcv defivar deftypecv deftypevar deftypevr deftp defvr defvar deftypeivar "
"ignore ifdocbook ifhtml ifinfo ifplaintext iftex ifxml ifnotdocbook ifnothtml ifnotplaintext ifnottex ifnotxml ifnotinfo "
"docbook html tex xml ifset ifclear ifcommanddefined ifcommandnotdefined macro "
, // Tex cmd
"input"
,
// \input
"texinfo "
// @documentencoding
"US-ASCII UTF-8 ISO-8859-1 "
// @frenchspacing @allowcodebreaks
"on off true false "
, "", "", "", "", ""

#if NUMKEYWORD == 16
, "", "", "", "", "", "", ""
#endif
}};

EDITLEXER lexTexinfo = { SCLEX_TEXINFO, NP2LEX_TEXINFO, L"Texinfo File", L"texi; texinfo; txi", L"", &Keywords_TEXI,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_L_COMMAND, NP2STYLE_Command, L"Command", L"fore:#0000FF", L"" },
	{ MULTI_STYLE(SCE_L_COMMENT, SCE_L_COMMENT2, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_L_TITLE, 63575, L"Title", L"fore:#008000", L"" },
	{ SCE_L_CHAPTER, 63579, L"Chapter", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_L_SECTION, SCE_L_SECTION1, SCE_L_SECTION2, 0), 63511, L"Section", L"fore:#008000", L"" },
	{ SCE_L_SPECIAL, 63572, L"Special Char", L"fore:#3A6EA5", L"" },
	{ SCE_L_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
