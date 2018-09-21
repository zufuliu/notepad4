#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_VIM = {{
"if endif end else elseif while endwhile endw fun endfun func endfunc function endfunction for in endfor try catch endtry "
"break continue return next let unlet set setf setlocal call map cmap unmap "
"finish quit exe silent echo "

, // 1
""

, // 2
"exists() has() "
"strlen() stridx() strpart() "

, "", "", "", "", "",

"", "", "", "", "", "", "", ""
}};

GCC_NO_WARNING_MISSING_BRACES_BEGIN

EDITLEXER lexVim = { SCLEX_VIM, NP2LEX_VIM, EDITLEXER_HOLE(L"Vim Script"), L"vim", L"", &Keywords_VIM,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#FF8000", L"" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, 0, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ SCE_C_COMMENTLINE, NP2STYLE_Comment, L"Comment", L"fore:#608060", L"" },
	EDITSTYLE_SENTINEL
}
};

GCC_NO_WARNING_MISSING_BRACES_END
