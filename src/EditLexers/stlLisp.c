#include "EditLexer.h"
#include "EditStyleX.h"

// https://en.wikipedia.org/wiki/Common_Lisp
// https://common-lisp.net/

static KEYWORDLIST Keywords_LISP = {{
"nil car cdr list "
"cond defun defvar defmacro let setq setf and or not equal if while lambda loop when "
,
NULL, NULL, NULL, NULL, NULL, NULL, NULL,

NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_LISP[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#FF8000" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, 0, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENTLINE, SCE_C_COMMENT, 0, 0), NP2StyleX_Comment, L"fore:#008000" },
};

EDITLEXER lexLisp = {
	SCLEX_LISP, NP2LEX_LISP,
	EDITLEXER_HOLE(L"Lisp Script", Styles_LISP),
	L"lisp; el; lsp; clj; cljs; scm",
	&Keywords_LISP,
	Styles_LISP
};
