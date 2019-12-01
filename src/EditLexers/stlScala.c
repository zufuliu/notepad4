#include "EditLexer.h"
#include "EditStyleX.h"

// https://www.scala-lang.org/index.html

static KEYWORDLIST Keywords_Scala = {{
"abstract case catch class def do else extends false final finally for forSome if "
"implicit import lazy match new null object override package private protected "
"return sealed super this throw trait try true type val var while with yield Nil None"

, // 1 Type Keyword
"Any AnyVal AnyRef ScalaObject Null Nothing Unit Boolean Char Byte Double Float Long Int Short "
"String Object Class Integer Character Void List"

, // 2 Preprocessor
NULL
, // 3 Annotation
"beanGetter beanSetter field getter param setter "
"bridge elidable implicitNotFound migration serializable strictfp switch tailrec varargs "
"cloneable deprecated deprecatedName inline native noinline remote "
"SerialVersionUID specialized throws transient unchecked volatile "

, // 4 Attribute
NULL
, // 5 Class
"Annotation Array Error Exception CharSequence Iterator Option Symbol"

, // 6 Interface
NULL
, // 7 Enumeration
NULL
, // 8 Constant
NULL

, NULL, NULL, NULL, NULL, NULL, NULL

, // 15 Code Snippet
"for^() if^() switch^() while^() catch^() else^if^() else^{} "
}};

static EDITSTYLE Styles_Scala[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2StyleX_TypeKeyword, L"fore:#0000FF" },
	{ SCE_C_DIRECTIVE, NP2StyleX_Annotation, L"fore:#FF8000" },
	{ SCE_C_CLASS, NP2StyleX_Class, L"fore:#0080FF" },
	{ SCE_C_TRAIT, NP2StyleX_Trait, L"bold; fore:#007F7F" },
	{ SCE_C_FUNCTION, NP2StyleX_Method, L"fore:#A46000" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_C_COMMENTDOC_TAG, NP2StyleX_DocCommentTag, L"fore:#408080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2StyleX_DocComment, L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_TRIPLEVERBATIM, NP2StyleX_TripleQuotedString, L"fore:#F08000" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_XML_TAG, SCE_C_XML_ASSIGN, 0, 0), NP2StyleX_XMLTag, L"fore:#648000" },
	{ SCE_C_XML_ATTR, NP2StyleX_XMLAttribute, L"fore:#FF0000" },
	{ SCE_C_XML_VALUE, NP2StyleX_XMLValue, L"fore:#1A1AA6" },
};

EDITLEXER lexScala = {
	SCLEX_CPP, NP2LEX_SCALA,
	EDITLEXER_HOLE(L"Scala Script", Styles_Scala),
	L"scala",
	&Keywords_Scala,
	Styles_Scala
};
