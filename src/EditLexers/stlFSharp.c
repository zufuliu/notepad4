#include "EditLexer.h"
#include "EditStyleX.h"

// https://en.wikipedia.org/wiki/F_Sharp_(programming_language)
// https://fsharp.org/

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

, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_FSharp[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_FSHARP_KEYWORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_FSHARP_TYPEKEYWORD, NP2StyleX_TypeKeyword, L"fore:#1E90FF" },
	{ SCE_FSHARP_PREPROCESSOR, NP2StyleX_Preprocessor, L"fore:#FF8000" },
	{ SCE_FSHARP_ATTRIBUTE, NP2StyleX_Attribute, L"fore:#FF8000" },
	{ MULTI_STYLE(SCE_FSHARP_COMMENT, SCE_FSHARP_COMMENTLINE, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_FSHARP_STRING, SCE_FSHARP_STRINGEOL, SCE_FSHARP_CHARACTER, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_FSHARP_VERBATIM, NP2StyleX_VerbatimString, L"fore:#008080" },
	{ SCE_FSHARP_QUOTATION, NP2StyleX_CodeQuotation, L"fore:#006633; back:#FFF1A8" },
	{ SCE_FSHARP_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_FSHARP_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexFSharp = {
	SCLEX_FSHARP, NP2LEX_FSHARP,
	EDITLEXER_HOLE(L"F# Source", Styles_FSharp),
	L"fs; fsi; fsx; fsscript; fsl; fsy; ml; mli; sml; nb",
	&Keywords_FSharp,
	Styles_FSharp
};
