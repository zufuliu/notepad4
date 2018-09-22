#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikipedia.org/wiki/AWK
// https://www.gnu.org/software/gawk/manual/gawk.html

static KEYWORDLIST Keywords_Awk = {{
"break case continue do else exit function for if in next return switch while "
"@include delete nextfile BEGIN END"

, // 1 Type Keyword
"ARGC ARGIND ARGV FILENAME FNR FS NF NR OFMT OFS ORS RLENGTH RS RSTART SUBSEP "
"BINMODE CONVFMT FIELDWIDTHS FPAT IGNORECASE LINT TEXTDOMAiN ENVIRON ERRNO PROCINFO RT"

, // 2 Preprocessor
""
, // 3 Directive
""
, // 4 Attribute
""
, // 5 Class
""
, // 6 Interface
""
, // 7 Enumeration
""
, // 8 Constant
""

, "", "", "", "", "", ""

, // 15 Code Snippet
"for^() if^() switch^() while^() else^if^() else^{} "
}};

GCC_NO_WARNING_MISSING_BRACES_BEGIN

EDITLEXER lexAwk = { SCLEX_CPP, NP2LEX_AWK, EDITLEXER_HOLE(L"Awk Script"), L"awk", &Keywords_Awk,
{
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_C_FUNCTION, NP2STYLE_Function, EDITSTYLE_HOLE(L"Function"), L"fore:#A46000" },
	{ SCE_C_WORD2, NP2STYLE_PredefinedVariable, EDITSTYLE_HOLE(L"Predefined Variable"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_REGEX, NP2STYLE_Regex, EDITSTYLE_HOLE(L"Regex"), L"fore:#006633; back:#FFF1A8" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_VARIABLE, NP2STYLE_Variable, EDITSTYLE_HOLE(L"Variable"), L"fore:#CC3300" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	EDITSTYLE_SENTINEL
}
};

GCC_NO_WARNING_MISSING_BRACES_END
