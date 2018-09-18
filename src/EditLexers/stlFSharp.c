#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikipedia.org/wiki/F_Sharp_(programming_language)
// http://fsharp.org/

static KEYWORDLIST Keywords_FSharp = {{
"abstract and as assert async base begin class default delegate do done downcast downto elif else end exception extern false finally for fun function global if in inherit inline interface internal lazy let match member module mutable namespace new not null of open or override private public rec return static struct then to true try type upcast use val void when while with yield "

// OCaml
"constraint external functor include initializer method object sig virtual parser "
"asr land lor lxor lsl lsr mod " " lnot div "
"atomic break ckecked component const constructor continue eager event fixed mixin parallel process protected pure sealed tailcall trait volatile "
"byref enum ref " "assembly field property param"

, // 1 Type Keyword
"bool byte sbyte int8 uint8 int16 uint16 int uint int32 uint32 int64 uint64 nativeint unativeint char string decimal uint void float32 single float double "
"bigint unit "
"array list big_int "

, // 2 Preprocessor
"if else endif line light nowarn indent load r I"

, // 3 Attribute
"DllImport Interface Sealed "

, "", "", "", "", ""

, "", "", "", "", "", "", ""
}};

EDITLEXER lexFSharp = { SCLEX_FSHARP, NP2LEX_FSHARP, EDITLEXER_HOLE, L"F# Source", L"fs; fsi; fsx; fsscript; fsl; fsy; ml; mli; sml; nb", L"", &Keywords_FSharp,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_FSHARP_KEYWORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_FSHARP_TYPEKEYWORD, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#1E90FF", L"" },
	{ SCE_FSHARP_PREPROCESSOR, NP2STYLE_Preprocessor, L"Preprocessor", L"fore:#FF8000", L"" },
	{ SCE_FSHARP_ATTRIBUTE, NP2STYLE_Attribute, L"Attribute", L"fore:#FF8000", L""},
	{ MULTI_STYLE(SCE_FSHARP_COMMENT, SCE_FSHARP_COMMENTLINE, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#608060", L"" },
	{ MULTI_STYLE(SCE_FSHARP_STRING, SCE_FSHARP_STRINGEOL, SCE_FSHARP_CHARACTER, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_FSHARP_VERBATIM, NP2STYLE_VerbatimString, L"Verbatim String", L"fore:#008080", L"" },
	{ SCE_FSHARP_QUOTATION, NP2STYLE_CodeQuotation, L"Code Quotation", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_FSHARP_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_FSHARP_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	EDITSTYLE_SENTINEL
}
};
