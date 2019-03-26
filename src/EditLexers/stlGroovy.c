#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikipedia.org/wiki/Groovy_(programming_language)
// http://www.groovy-lang.org/

static KEYWORDLIST Keywords_Groovy = {{
"as assert break case catch class continue const def default do else enum extends "
"finally for goto if in implements import instanceof interface native new package "
"return switch threadsafe throw throws try while abstract final private protected "
"public static strictfp synchronized transient "
"volatile false null super this true it trait println "

, // 1 Type Keyword
"boolean byte char class double float int long short void "

, // 2 Preprocessor
NULL
, // 3 Annotation
NULL
/*
"interface Retention Target Override"
*/
, // 4 Attribute
NULL

, // 5 Class
"Void String Double Integer Class Object Byte Short Long Float Boolean Character Boolean BigDecimal BigInteger Number"
"Map Exception Date File"

, // 6 Interface
NULL
, // 7 Enumeration
NULL
, // 8 Constant
NULL

, NULL, NULL, NULL, NULL, NULL, NULL

, // 15 Code Snippet
"for^() if^() switch^() while^() catch^() else^if^() else^{} "
"def^() "
}};

static EDITSTYLE Styles_Groovy[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, EDITSTYLE_HOLE(L"Type Keyword"), L"fore:#0000FF" },
	{ SCE_C_DIRECTIVE, NP2STYLE_Annotation, EDITSTYLE_HOLE(L"Annotation"), L"fore:#FF8000" },
	{ SCE_C_CLASS, NP2STYLE_Class, EDITSTYLE_HOLE(L"Class"), L"fore:#0080FF" },
	{ SCE_C_INTERFACE, NP2STYLE_Interface, EDITSTYLE_HOLE(L"Interface"), L"bold; fore:#1E90FF" },
	{ SCE_C_FUNCTION, NP2STYLE_Method, EDITSTYLE_HOLE(L"Method"), L"fore:#A46000" },
	{ SCE_C_ENUMERATION, NP2STYLE_Enumeration, EDITSTYLE_HOLE(L"Enumeration"), L"fore:#FF8000" },
	{ SCE_C_CONSTANT, NP2STYLE_Constant, EDITSTYLE_HOLE(L"Constant"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, EDITSTYLE_HOLE(L"Doc Comment Tag"), L"fore:#408080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2STYLE_DocComment, EDITSTYLE_HOLE(L"Doc Comment"), L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_TRIPLEVERBATIM, NP2STYLE_TripleString, EDITSTYLE_HOLE(L"Triple Quoted String"), L"fore:#F08000" },
	{ SCE_C_REGEX, NP2STYLE_Regex, EDITSTYLE_HOLE(L"Regex"), L"fore:#006633; back:#FFF1A8" },
	{ SCE_C_LABEL, NP2STYLE_Label, EDITSTYLE_HOLE(L"Label"), L"back:#FFC040" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
};

EDITLEXER lexGroovy = {
	SCLEX_CPP, NP2LEX_GROOVY,
	EDITLEXER_HOLE(L"Groovy Script", Styles_Groovy),
	L"groovy; gvy; gy; gsh; gpp; grunit",
	&Keywords_Groovy,
	Styles_Groovy
};

