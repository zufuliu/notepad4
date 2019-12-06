#include "EditLexer.h"
#include "EditStyleX.h"

// https://en.wikipedia.org/wiki/Groovy_(programming_language)
// https://www.groovy-lang.org/

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
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2StyleX_TypeKeyword, L"fore:#0000FF" },
	{ SCE_C_DIRECTIVE, NP2StyleX_Annotation, L"fore:#FF8000" },
	{ SCE_C_CLASS, NP2StyleX_Class, L"fore:#0080FF" },
	{ SCE_C_INTERFACE, NP2StyleX_Interface, L"bold; fore:#1E90FF" },
	{ SCE_C_FUNCTION, NP2StyleX_Method, L"fore:#A46000" },
	{ SCE_C_ENUMERATION, NP2StyleX_Enumeration, L"fore:#FF8000" },
	{ SCE_C_CONSTANT, NP2StyleX_Constant, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_C_COMMENTDOC_TAG, NP2StyleX_DocCommentTag, L"fore:#408080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2StyleX_DocComment, L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_TRIPLEVERBATIM, NP2StyleX_TripleQuotedString, L"fore:#F08000" },
	{ SCE_C_REGEX, NP2StyleX_Regex, L"fore:#006633; back:#FFF1A8" },
	{ SCE_C_LABEL, NP2StyleX_Label, L"back:#FFC040" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexGroovy = {
	SCLEX_CPP, NP2LEX_GROOVY,
	EDITLEXER_HOLE(L"Groovy Script", Styles_Groovy),
	L"groovy; gvy; gy; gsh; gpp; grunit",
	&Keywords_Groovy,
	Styles_Groovy
};
