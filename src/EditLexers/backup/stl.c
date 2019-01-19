#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_NULL = {
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL

, // 15 Code Snippet
NULL
};

static KEYWORDLIST Keywords_CPP = {
NULL
, // 1 Type Keyword
NULL
, // 2 #Preprocessor
NULL
, // 3 @Directive
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

, // 9 2nd Keyword
NULL
, // 10 2nd Type Keyword
NULL
, // 11 Assembler Intruction
NULL
, // 12 Assembler Register
NULL
, // 13 C Function
NULL
, // 14 C++ Function
NULL
, // 15 Code Snippet
NULL
};

static EDITSTYLE Styles_CPP[] = {
	EDITSTYLE_DEFAULT,
};

EDITLEXER lexCPP= {
	SCLEX_CPP, NP2LEX_CPP,
	EDITLEXER_HOLE(L"C/C++ Source", Styles_CPP),
	L"",
	&Keywords_CPP,
	Styles_CPP
};
