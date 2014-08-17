#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_Julia = {
"abstract begin bitstype break catch continue const do else elseif end export for function global if in import "
"let local macro module quote return try type typealias using while "
"true false nothing Inf NaN Inf32 NaN32"
,
"ANY Any Void Exception String Number Real Float Integer Signed Unsigned "
"Int8 Uint8 Int16 Uint16 Int32 Uint32 Int64 Uint64 Int128 Uint128 Int Uint Float32 Float64 Bool Char "
"ASCIIString UTF8String ByteString Type Ptr Complex Complex64 Complex128 Rational Symbol Function Expr Stream "
"Array Vector Matrix AbstractArray AbstractVector AbstractMatrix StridedArray StridedMatrix SparseMatrixCSC "
"BitArray BitVector BitMatrix SubArray StridedVector StridedVecOrMat Set IntSet Dict Associative Tuple NTuple "
"Buffer Union BigFloat BigInt FileOffset WeakRef Regex IOStream AsyncStream AbstractKind BitsKind UnionKind "
"Module Nothing None TypeName TypeVar "
, "",
"int8 uint8 int16 uint16 int32 uint32 int64 uint64 int128 uint128 int uint float32 float64 bool char "
"float integer signed unsigned complex complex64 complex128 cstring string "
"include is isa throw typeof new error show print "
"ccall box unbox "
"sizeof convert length size numel ndims isequal isempty assign "
 , "", "", "", "", ""

#if NUMKEYWORD == 16
,"","","","","","",""
#endif
};

EDITLEXER lexJulia = { SCLEX_MATLAB, NP2LEX_JULIA, L"Julia Script", L"jl", L"", &Keywords_Julia,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
//	{ SCE_MAT_DEFAULT, L"Default", L"", L"" },
	{ SCE_MAT_KEYWORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_MAT_ATTRIBUTE, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#0080FF", L"" },
	{ SCE_MAT_CALLBACK, NP2STYLE_Macro, L"Macro", L"fore:#FF8000", L"" },
	{ MULTI_STYLE(SCE_MAT_FUNCTION1,SCE_MAT_FUNCTION2,0,0), NP2STYLE_BasicFunction, L"Basic Function", L"fore:#FF0080", L"" },
	{ MULTI_STYLE(SCE_MAT_COMMENT,SCE_MAT_COMMENTBLOCK,0,0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_MAT_STRING,SCE_MAT_DOUBLEQUOTESTRING,0,0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_MAT_NUMBER,SCE_MAT_HEXNUM,0,0), NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_MAT_BACKTICK, NP2STYLE_BackTicks, L"Back Ticks", L"fore:#FF0080", L"" },
	{ SCE_MAT_REGEX, NP2STYLE_Regex, L"Regex", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_MAT_VARIABLE, NP2STYLE_Variable, L"Variable", L"fore:#CC3300", L"" },
	{ SCE_MAT_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};