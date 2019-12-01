#include "EditLexer.h"
#include "EditStyleX.h"

// https://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/index.html

static KEYWORDLIST Keywords_AS = {{
"as break case catch class const continue default delete do dynamic each else extends "
"final finally for function get if implements import include interface internal "
"instanceof in is namespace native new override package private protected public "
"rest return set static super switch this throw try typeof use var void while with intrinsic "
"false null true undefined Infinity NaN"

, // 1 Type Keyword
"int uint "

, // 2 Preprocessor
NULL
, // 3 Directive
NULL
, // 4 Attribute
NULL

, // 5 Class
"Array Boolean Class Date Error Function Namespace Number Null Object Void "
"QName RegExp String Vector XML XMLList"

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

static EDITSTYLE Styles_AS[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2StyleX_TypeKeyword, L"fore:#1E90FF" },
	{ SCE_C_CLASS, NP2StyleX_Class, L"fore:#0080FF" },
	{ SCE_C_INTERFACE, NP2StyleX_Interface, L"bold; fore:#1E90FF" },
	{ SCE_C_FUNCTION, NP2StyleX_Function, L"fore:#A46000" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_C_COMMENTDOC_TAG, NP2StyleX_DocCommentTag, L"fore:#408080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2StyleX_DocComment, L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_REGEX, NP2StyleX_Regex, L"fore:#006633; back:#FFF1A8" },
	{ SCE_C_LABEL, NP2StyleX_Label, L"back:#FFC040" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexAS = {
	SCLEX_CPP, NP2LEX_AS,
	EDITLEXER_HOLE(L"ActionScript", Styles_AS),
	L"as",
	&Keywords_AS,
	Styles_AS
};
