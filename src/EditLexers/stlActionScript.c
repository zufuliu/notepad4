#include "EditLexer.h"
#include "EditStyle.h"

// http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/index.html

static KEYWORDLIST Keywords_AS = {{
"as break case catch class const continue default delete do dynamic each else extends "
"final finally for function get if implements import include interface internal "
"instanceof in is namespace native new override package private protected public "
"rest return set static super switch this throw try typeof use var void while with intrinsic "
"false null true undefined Infinity NaN"

, // 1 Type Keyword
"int uint "

, // 2 Preprocessor
""
, // 3 Directive
""
, // 4 Attribute
""

, // 5 Class
"Array Boolean Class Date Error Function Namespace Number Null Object Void "
"QName RegExp String Vector XML XMLList"

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

EDITLEXER lexAS = { SCLEX_CPP, NP2LEX_AS, L"ActionScript", L"as", L"", &Keywords_AS,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#1E90FF", L"" },
	{ SCE_C_CLASS, NP2STYLE_Class, L"Class", L"fore:#0000FF", L"" },
	{ SCE_C_INTERFACE, NP2STYLE_Interface, L"Interface", L"bold; fore:#1E90FF", L""},
	{ SCE_C_FUNCTION, NP2STYLE_Function, L"Function", L"fore:#A46000", L"" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#608060", L"" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, L"Doc Comment Tag", L"fore:#408080", L"" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2STYLE_DocComment, L"Doc Comment", L"fore:#408040", L"" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_REGEX, NP2STYLE_Regex, L"Regex", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_C_LABEL, NP2STYLE_Label, L"Label", L"fore:#000000; back:#FFC040", L""},
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
