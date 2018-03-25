#include "EditLexer.h"
#include "EditStyle.h"

// http://haxe.org/

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
"for^() if^() switch^() while^() else^if^() "
}};

EDITLEXER lexHaXe = { SCLEX_CPP, NP2LEX_HAXE, L"HaXe Script", L"hx", L"", &Keywords_HaXe,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#1E90FF", L"" },
	{ SCE_C_PREPROCESSOR, NP2STYLE_Preprocessor, L"Preprocessor", L"fore:#FF8000", L"" },
	{ SCE_C_CLASS, NP2STYLE_Class, L"Class", L"fore:#0000FF", L"" },
	{ SCE_C_INTERFACE, NP2STYLE_Interface, L"Interface", L"bold; fore:#1E90FF", L""},
	{ SCE_C_FUNCTION, NP2STYLE_Function, L"Function", L"fore:#A46000", L"" },
	{ SCE_C_ENUMERATION, NP2STYLE_Enumeration, L"Enumeration", L"fore:#FF8000", L""},
	{ SCE_C_CONSTANT, NP2STYLE_Constant, L"Constant", L"bold; fore:#B000B0", L""},
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, L"Doc Comment Tag", L"bold; fore:#008000F", L"" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2STYLE_DocComment, L"Doc Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_REGEX, NP2STYLE_Regex, L"Regex", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_VARIABLE, NP2STYLE_Variable, L"Variable", L"fore:#CC3300", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
