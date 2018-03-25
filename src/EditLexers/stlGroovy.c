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
""
, // 3 Annotation
""
/*
"interface Retention Target Override"
*/
, // 4 Attribute
""

, // 5 Class
"Void String Double Integer Class Object Byte Short Long Float Boolean Character Boolean BigDecimal BigInteger Number"
"Map Exception Date File"

, // 6 Interface
""
, // 7 Enumeration
""
, // 8 Constant
""

, "", "", "", "", "", ""

, // 15 Code Snippet
"for^() if^() switch^() while^() catch^() else^if^() "
"def^() "
}};

EDITLEXER lexGroovy = { SCLEX_CPP, NP2LEX_GROOVY, L"Groovy Script", L"groovy", L"", &Keywords_Groovy,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_DIRECTIVE, NP2STYLE_Annotation, L"Annotation", L"fore:#FF8000", L""},
	{ SCE_C_CLASS, NP2STYLE_Class, L"Class", L"fore:#0080FF", L"" },
	{ SCE_C_INTERFACE, NP2STYLE_Interface, L"Interface", L"bold; fore:#1E90FF", L""},
	{ SCE_C_FUNCTION, NP2STYLE_Method, L"Method", L"fore:#A46000", L"" },
	{ SCE_C_ENUMERATION, NP2STYLE_Enumeration, L"Enumeration", L"fore:#FF8000", L""},
	{ SCE_C_CONSTANT, NP2STYLE_Constant, L"Constant", L"bold; fore:#B000B0", L""},
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, L"Doc Comment Tag", L"bold; fore:#008000F", L"" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2STYLE_DocComment, L"Doc Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_TRIPLEVERBATIM, NP2STYLE_TripleString, L"Triple Quoted String", L"fore:#F08000", L"" },
	{ SCE_C_REGEX, NP2STYLE_Regex, L"Regex", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_C_LABEL, NP2STYLE_Label, L"Label", L"fore:#000000; back:#FFC040", L""},
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
