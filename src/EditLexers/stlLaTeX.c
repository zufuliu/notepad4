#include "EditLexer.h"
#include "EditStyleX.h"

// https://www.latex-project.org/

static KEYWORDLIST Keywords_TEX = {{
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_TEX[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_L_COMMAND, NP2StyleX_Command, L"fore:#0000FF" },
	{ MULTI_STYLE(SCE_L_COMMENT, SCE_L_COMMENT2, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_L_TITLE, NP2StyleX_Title, L"fore:#008000" },
	{ SCE_L_CHAPTER, NP2StyleX_Chapter, L"fore:#008000" },
	{ MULTI_STYLE(SCE_L_SECTION, SCE_L_SECTION1, SCE_L_SECTION2, 0), NP2StyleX_Section, L"fore:#008000" },
	{ MULTI_STYLE(SCE_L_MATH, SCE_L_MATH2, 0, 0), NP2StyleX_Math, L"fore:#FF0000" },
	{ SCE_L_SPECIAL, NP2StyleX_SpecialCharacter, L"fore:#3A6EA5" },
	{ MULTI_STYLE(SCE_L_TAG, SCE_L_TAG2, 0, 0), NP2StyleX_Tag, L"fore:#FF8000" },
	{ SCE_L_CMDOPT, NP2StyleX_Option, L"fore:#1E90FF" },
	{ SCE_L_STRING, NP2StyleX_String, L"fore:#008000" },
	{ MULTI_STYLE(SCE_L_QUOTE1, SCE_L_QUOTE2, 0, 0), NP2StyleX_Quote, L"fore:#408080" },
	{ MULTI_STYLE(SCE_L_VERBATIM, SCE_L_VERBATIM2, 0, 0), NP2StyleX_VerbatimSegment, L"fore:#666666" },
	{ SCE_L_LISTCODE, NP2StyleX_ListCode, L"fore:#808080" },
	{ SCE_L_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexLaTeX = {
	SCLEX_LATEX, NP2LEX_LATEX,
	EDITLEXER_HOLE(L"LaTeX File", Styles_TEX),
	L"tex; latex; sty; cls; tpx; bbl; bib; ltx; dtx; ins; toc; info",
	&Keywords_TEX,
	Styles_TEX
};

// https://www.gnu.org/software/texinfo/

static KEYWORDLIST Keywords_TEXI = {{
// cmd
"end setfilename settitle bye insertcopying documentencoding documentlanguage dircategory "
"shorttitlepage center titlefont title subtitle author "
"shortcontents summarycontents contents setcontentsaftertitlepage setshortcontentsaftertitlepage "
"node top novalidate lowersections raisesections part charpter centerchap section subsection subsubsection unnumbered "

, // 1 Fold
"copying frenchspacing direntry documentdescription titlepage menu detailmenu cartouche quotation smallquotation indentedblock smallindentedblock raggedright format smallformat display smalldisplay flushleft flushright example smallexample lisp smalllist verbatim itemize enumerate table ftable vtable multitable float group "
"deffn defun defmac deftypefn deftypefun defcv defivar deftypecv deftypevar deftypevr deftp defvr defvar deftypeivar "
"ignore ifdocbook ifhtml ifinfo ifplaintext iftex ifxml ifnotdocbook ifnothtml ifnotplaintext ifnottex ifnotxml ifnotinfo "
"docbook html tex xml ifset ifclear ifcommanddefined ifcommandnotdefined macro "

, // 2 Tex Command
"input"

, // 3 input
// \input
"texinfo "
// @documentencoding
"US-ASCII UTF-8 ISO-8859-1 "
// @frenchspacing @allowcodebreaks
"on off true false "

, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_TEXI[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_L_COMMAND, NP2StyleX_Command, L"fore:#0000FF" },
	{ MULTI_STYLE(SCE_L_COMMENT, SCE_L_COMMENT2, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_L_TITLE, NP2StyleX_Title, L"back:#FF8040; eolfilled" },
	{ SCE_L_CHAPTER, NP2StyleX_Chapter, L"back:#FF8040; eolfilled" },
	{ MULTI_STYLE(SCE_L_SECTION, SCE_L_SECTION1, SCE_L_SECTION2, 0), NP2StyleX_Section, L"back:#FF8040; eolfilled" },
	{ SCE_L_SPECIAL, NP2StyleX_SpecialCharacter, L"fore:#3A6EA5" },
	{ SCE_L_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexTexinfo = {
	SCLEX_TEXINFO, NP2LEX_TEXINFO,
	EDITLEXER_HOLE(L"Texinfo File", Styles_TEXI),
	L"texi; texinfo; txi",
	&Keywords_TEXI,
	Styles_TEXI
};
