#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_VIM = {{
"if endif end else elseif while endwhile endw fun endfun func endfunc function endfunction for in endfor try catch endtry "
"break continue return next let unlet set setf setlocal call map cmap unmap "
"finish quit exe silent echo "

, // 1
NULL

, // 2
"exists() has() "
"strlen() stridx() strpart() "

, NULL, NULL, NULL, NULL, NULL,

NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};


EDITLEXER lexVim = { SCLEX_VIM, NP2LEX_VIM, EDITLEXER_HOLE(L"Vim Script"), L"vim", &Keywords_VIM,
{
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#FF8000" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, 0, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ SCE_C_COMMENTLINE, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	EDITSTYLE_SENTINEL
}
};

