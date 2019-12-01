#include "EditLexer.h"
#include "EditStyleX.h"

// https://haxe.org/

static KEYWORDLIST Keywords_HaXe = {{
"break callback case cast catch class continue default do dynamic else enum "
"extends extern false for function here if implements import in inline interface "
"never new null override package private public return static super switch this "
"throw trace true try typedef untyped using var while"

, // 1 Type Keyword
"Class Enum Void Float Int UInt Null Bool Dynamic Iterator Iterable "
"Array Date Hash EReg IntHash IntIter List String StringBuf Xml "

, // 2 Preprocessor
"if elseif else end error"

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

static EDITSTYLE Styles_HaXe[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2StyleX_TypeKeyword, L"fore:#1E90FF" },
	{ SCE_C_PREPROCESSOR, NP2StyleX_Preprocessor, L"fore:#FF8000" },
	{ SCE_C_CLASS, NP2StyleX_Class, L"fore:#0080FF" },
	{ SCE_C_INTERFACE, NP2StyleX_Interface, L"bold; fore:#1E90FF" },
	{ SCE_C_FUNCTION, NP2StyleX_Function, L"fore:#A46000" },
	{ SCE_C_ENUMERATION, NP2StyleX_Enumeration, L"fore:#FF8000" },
	{ SCE_C_CONSTANT, NP2StyleX_Constant, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_C_COMMENTDOC_TAG, NP2StyleX_DocCommentTag, L"fore:#408080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2StyleX_DocComment, L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_REGEX, NP2StyleX_Regex, L"fore:#006633; back:#FFF1A8" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_VARIABLE, NP2StyleX_Variable, L"fore:#CC3300" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexHaXe = {
	SCLEX_CPP, NP2LEX_HAXE,
	EDITLEXER_HOLE(L"HaXe Script", Styles_HaXe),
	L"hx",
	&Keywords_HaXe,
	Styles_HaXe
};
