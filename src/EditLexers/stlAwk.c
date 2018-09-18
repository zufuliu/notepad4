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

EDITLEXER lexAwk = { SCLEX_CPP, NP2LEX_AWK, EDITLEXER_HOLE(L"Awk Script"), L"awk", L"", &Keywords_Awk,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_FUNCTION, NP2STYLE_Function, L"Function", L"fore:#A46000", L"" },
	{ SCE_C_WORD2, NP2STYLE_PredefinedVariable, L"Predefined Variable", L"fore:#B000B0", L""},
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC), NP2STYLE_Comment, L"Comment", L"fore:#608060", L"" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_REGEX, NP2STYLE_Regex, L"Regex", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_VARIABLE, NP2STYLE_Variable, L"Variable", L"fore:#CC3300", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	EDITSTYLE_SENTINEL
}
};
