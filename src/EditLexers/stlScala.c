#include "EditLexer.h"
#include "EditStyle.h"

// http://www.scala-lang.org/index.html

static KEYWORDLIST Keywords_Scala = {{
"abstract case catch class def do else extends false final finally for forSome if "
"implicit import lazy match new null object override package private protected "
"return sealed super this throw trait try true type val var while with yield Nil None"

, // 1 Type Keyword
"Any AnyVal AnyRef ScalaObject Null Nothing Unit Boolean Char Byte Double Float Long Int Short "
"String Object Class Integer Character Void List"

, // 2 Preprocessor
""
, // 3 Annotation
"beanGetter beanSetter field getter param setter "
"bridge elidable implicitNotFound migration serializable strictfp switch tailrec varargs "
"cloneable deprecated deprecatedName inline native noinline remote "
"SerialVersionUID specialized throws transient unchecked volatile "

, // 4 Attribute
""
, // 5 Class
"Annotation Array Error Exception CharSequence Iterator Option Symbol"

, // 6 Interface
""
, // 7 Enumeration
""
, // 8 Constant
""

, "", "", "", "", "", ""

, // 15 Code Snippet
"for^() if^() switch^() while^() catch^() else^if^() else^{} "
}};

GCC_NO_WARNING_MISSING_BRACES_BEGIN

EDITLEXER lexScala = { SCLEX_CPP, NP2LEX_SCALA, EDITLEXER_HOLE(L"Scala Script"), L"scala", L"", &Keywords_Scala,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_DIRECTIVE, NP2STYLE_Annotation, L"Annotation", L"fore:#FF8000", L""},
	{ SCE_C_CLASS, NP2STYLE_Class, L"Class", L"fore:#0080FF", L"" },
	{ SCE_C_TRAIT, NP2STYLE_Trait, L"Trait", L"bold; fore:#007F7F", L"" },
	{ SCE_C_FUNCTION, NP2STYLE_Method, L"Method", L"fore:#A46000", L"" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#608060", L"" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, L"Doc Comment Tag", L"fore:#408080", L"" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2STYLE_DocComment, L"Doc Comment", L"fore:#408040", L"" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_TRIPLEVERBATIM, NP2STYLE_TripleString, L"Triple Quoted String", L"fore:#F08000", L"" },
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ MULTI_STYLE(SCE_C_XML_TAG, SCE_C_XML_ASSIGN, 0, 0), NP2STYLE_XMLTag, L"XML Tag", L"fore:#648000", L"" },
	{ SCE_C_XML_ATTR, NP2STYLE_XMLAttribute, L"XML Attribute", L"fore:#FF0000", L"" },
	{ SCE_C_XML_VALUE, NP2STYLE_XMLValue, L"XML Value", L"fore:#1A1AA6", L"" },
	EDITSTYLE_SENTINEL
}
};

GCC_NO_WARNING_MISSING_BRACES_END
