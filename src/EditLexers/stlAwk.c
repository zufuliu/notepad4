#include "EditLexer.h"
#include "EditStyleX.h"

// https://en.wikipedia.org/wiki/AWK
// https://www.gnu.org/software/gawk/manual/gawk.html

static KEYWORDLIST Keywords_Awk = {{
"break case continue do else exit function for if in next return switch while "
"@include delete nextfile BEGIN END"

, // 1 Type Keyword
"ARGC ARGIND ARGV FILENAME FNR FS NF NR OFMT OFS ORS RLENGTH RS RSTART SUBSEP "
"BINMODE CONVFMT FIELDWIDTHS FPAT IGNORECASE LINT TEXTDOMAiN ENVIRON ERRNO PROCINFO RT"

, // 2 Preprocessor
NULL
, // 3 Directive
NULL
, // 4 Attribute
NULL
, // 5 Class
NULL
, // 6 Interface
NULL
, // 7 Enumeration
NULL
, // 8 Constant
NULL

, NULL, NULL, NULL, NULL, NULL, NULL

, // 15 Code Snippet
"for^() if^() switch^() while^() else^if^() else^{} "
}};

static EDITSTYLE Styles_Awk[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_FUNCTION, NP2StyleX_Function, L"fore:#A46000" },
	{ SCE_C_WORD2, NP2StyleX_PredefinedVariable, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC), NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_REGEX, NP2StyleX_Regex, L"fore:#006633; back:#FFF1A8" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_VARIABLE, NP2StyleX_Variable, L"fore:#CC3300" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexAwk = {
	SCLEX_CPP, NP2LEX_AWK,
	EDITLEXER_HOLE(L"Awk Script", Styles_Awk),
	L"awk",
	&Keywords_Awk,
	Styles_Awk
};
