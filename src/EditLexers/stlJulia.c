#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikipedia.org/wiki/Julia_%28programming_language%29
// https://julialang.org/

static KEYWORDLIST Keywords_Julia = {{
"abstract begin bitstype break catch continue const do else elseif end export finally for function global if in import "
"let local macro module mutable primitive quote return struct try type typealias using where while "
"true false nothing missing ans Inf NaN Inf32 NaN32 Inf16 NaN16"

, // 1 Type Keyword
"ANY Any Void Exception String Number Real Float Integer Signed Unsigned "
"Int8 UInt8 Int16 UInt16 Int32 UInt32 Int64 UInt64 Int128 UInt128 Int UInt Float16 Float32 Float64 Bool Char "
"ASCIIString UTF8String ByteString Type Ptr Complex Complex64 Complex128 Rational Symbol Function Expr Stream "
"Array Vector Matrix AbstractArray AbstractVector AbstractMatrix StridedArray StridedMatrix SparseMatrixCSC "
"BitArray BitVector BitMatrix SubArray StridedVector StridedVecOrMat Set IntSet Dict Associative Tuple NTuple "
"Buffer Union BigFloat BigInt FileOffset WeakRef Regex IOStream AsyncStream AbstractKind BitsKind UnionKind "
"Module Nothing None TypeName TypeVar Vararg "

, // 2
NULL

, // 3 Basic Function
"int8 uint8 int16 uint16 int32 uint32 int64 uint64 int128 uint128 int uint float32 float64 bool char "
"float integer signed unsigned complex complex64 complex128 cstring string "
"include is isa() throw() rethrow() typeof() new error() show() print() "
"ccall() box() unbox() yield() "
"sizeof() convert() length() size() numel() ndims() isequal() isempty() assign() "

, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Julia[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_MAT_KEYWORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_MAT_ATTRIBUTE, NP2STYLE_TypeKeyword, EDITSTYLE_HOLE(L"Type Keyword"), L"fore:#0080FF" },
	{ SCE_MAT_CALLBACK, NP2STYLE_Macro, EDITSTYLE_HOLE(L"Macro"), L"fore:#FF8000" },
	{ MULTI_STYLE(SCE_MAT_FUNCTION1, SCE_MAT_FUNCTION2, 0, 0), NP2STYLE_BasicFunction, EDITSTYLE_HOLE(L"Basic Function"), L"fore:#FF0080" },
	{ SCE_MAT_FUNCTION, NP2STYLE_Function, EDITSTYLE_HOLE(L"Function"), L"fore:#A46000" },
	{ MULTI_STYLE(SCE_MAT_COMMENT, SCE_MAT_COMMENTBLOCK, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ MULTI_STYLE(SCE_MAT_STRING, SCE_MAT_DOUBLEQUOTESTRING, 0, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_MAT_RAW_STRING2, NP2STYLE_RawString, EDITSTYLE_HOLE(L"Raw String"), L"fore:#008080" },
	{ SCE_MAT_TRIPLE_STRING2, NP2STYLE_TripleString, EDITSTYLE_HOLE(L"Triple Quoted String"), L"fore:#F08000" },
	{ MULTI_STYLE(SCE_MAT_NUMBER, SCE_MAT_HEXNUM, 0, 0), NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_MAT_BACKTICK, NP2STYLE_Backticks, EDITSTYLE_HOLE(L"Backticks"), L"fore:#FF0080" },
	{ SCE_MAT_REGEX, NP2STYLE_Regex, EDITSTYLE_HOLE(L"Regex"), L"fore:#006633; back:#FFF1A8" },
	{ SCE_MAT_VARIABLE, NP2STYLE_Variable, EDITSTYLE_HOLE(L"Variable"), L"fore:#CC3300" },
	{ SCE_MAT_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
};

EDITLEXER lexJulia = {
	SCLEX_MATLAB, NP2LEX_JULIA,
	EDITLEXER_HOLE(L"Julia Script", Styles_Julia),
	L"jl",
	&Keywords_Julia,
	Styles_Julia
};

