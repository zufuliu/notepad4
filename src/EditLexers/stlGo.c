#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikipedia.org/wiki/Go_(programming_language)
// https://golang.org/
// https://github.com/golang/go/blob/master/doc/go_spec.html

static KEYWORDLIST Keywords_Go = {{
"break case chan const continue default defer else fallthrough for func go goto "
"if import interface map package range return select struct switch type var "
"false iota nil true "

, // 1 Type Keyword
"bool byte complex64 complex128 error float32 float64 int int8 int16 int32 int64 "
"rune string uint uintptr uint8 uint16 uint32 uint64 "

, // 2 Preprocessor
NULL
, // 3 Directive
NULL

, // 4 Attribute
"append cap close complex copy delete imag len make new panic print println real recover "

, // 5 Class
NULL
, // 6 Interface
NULL
, // 7 Enumeration
NULL
, // 8 Constant
NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Go[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, EDITSTYLE_HOLE(L"Type Keyword"), L"fore:#0000FF" },
	{ SCE_C_ATTRIBUTE, NP2STYLE_BasicFunction, EDITSTYLE_HOLE(L"Basic Function"), L"fore:#FF8000" },
	{ SCE_C_FUNCTION, NP2STYLE_Function, EDITSTYLE_HOLE(L"Function"), L"fore:#A46000" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, EDITSTYLE_HOLE(L"Doc Comment Tag"), L"fore:#408080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2STYLE_DocComment, EDITSTYLE_HOLE(L"Doc Comment"), L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_DSTRINGB, NP2STYLE_Backticks, EDITSTYLE_HOLE(L"Raw String"), L"fore:#F08000" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
};

EDITLEXER lexGo = {
	SCLEX_CPP, NP2LEX_GO,
	EDITLEXER_HOLE(L"Go Source", Styles_Go),
	L"go",
	&Keywords_Go,
	Styles_Go
};

