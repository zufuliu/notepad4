#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikipedia.org/wiki/Common_Lisp
// https://common-lisp.net/

static KEYWORDLIST Keywords_LISP = {{
"nil car cdr list "
"cond defun defvar defmacro let setq setf and or not equal if while lambda loop when "
,
"", "", "", "", "", "", "",

"", "", "", "", "", "", "", ""
}};


EDITLEXER lexLisp = { SCLEX_LISP, NP2LEX_LISP, EDITLEXER_HOLE(L"Lisp Script"), L"lisp; el; lsp; clj; cljs", &Keywords_LISP,
{
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#FF8000" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, 0, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENTLINE, SCE_C_COMMENT, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#008000" },
	EDITSTYLE_SENTINEL
}
};

