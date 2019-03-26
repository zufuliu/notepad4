#include "EditLexer.h"
#include "EditStyle.h"

// https://llvm.org/docs/LangRef.html

static KEYWORDLIST Keywords_LLVM = {{
"define declare type alias comdat "
"global constant "
"prefix prologue attributes module asm target datalayout triple uselistorder uselistorder_bb section "
// linkage
"private internal available_externally linkonce weak common appending extern_weak linkonce_odr weak_odr external "
"true false null undef "

, // 1 Type Keyword
"void i1 i8 i16 i32 i64 i128 "
"half float double fp128 x86_fp80 ppc_fp128 x86_mmx label metadata "

, // 2 #Preprocessor
NULL

, // 3 @Directive
NULL

, // 4 Attribute
"unnamed_addr "

// DLL storage class
"dllimport dllexport "
// Thread Local Storage Models
"localdynamic initialexec localexec "
// comdat selection kind
"any exactmatch largest noduplicates samesize "
// parameter attributes
"zeroext signext inreg byval inalloca sret align noalias nocapture nest returned nonnull dereferenceable "
// Function attributes
"alignstack alwaysinline builtin cold cold jumptable minsize naked nobuiltin noduplicate noimplicitfloat noinline "
"nonlazybind noredzone noreturn nounwind optnone optsize readnone readonly returns_twice "
"sanitize_address sanitize_memory sanitize_thread ssp sspreq sspstrong uwtable "
// Atomic Memory Ordering Constraints
"unordered monotonic acquire release acq_rel seq_cst "
// fast-math flags
"nnan ninf nsz arcp fast "

"atomic volatile sideeffect alignstack inteldialect distinct "
"inbounds "
"nuw nsw exact "

"xchg nand max min umax umin "					// atomicrmw
"eq ne ugt uge ult ule sgt sgt sge slt sle "	// icmp
"oeq ogt oge oge ole one ord une uno "			// fcmp

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

, // 10 Intruction
"ret br switch indirectbr invoke resume unreachable "			// terminator
"add fadd sub fsub mul fmul udiv sdiv fdiv urem srem frem "		// binary
"shl lshr ashr and or xor "										// bitwise binary
"extractelement insertelement shufflevector "					// vector
"extractvalue insertvalue "										// aggregate
"alloca load store fence cmpxchg atomicrww getelementptr "		// memory access
"to trunc zext sext fptrunc fpext fptoui fptosi uitofp sitofp ptrtoint inttoptr bitcast addrspacecast "	// conversion
"icmp fcmp phi select call va_arg landingpad "

, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_LLVM[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, EDITSTYLE_HOLE(L"Type Keyword"), L"fore:#0000FF" },
	{ SCE_C_DIRECTIVE, NP2STYLE_BuildinRule, EDITSTYLE_HOLE(L"Intrinsic"), L"fore:#FF0080" },
	{ SCE_C_ASM_INSTRUCTION, NP2STYLE_Instruction, EDITSTYLE_HOLE(L"Instruction"), L"fore:#0080FF" },
	{ SCE_C_ATTRIBUTE, NP2STYLE_Attribute, EDITSTYLE_HOLE(L"Attribute"), L"fore:#FF8000" },
	{ SCE_C_STRUCT, NP2STYLE_PredefinedVariable, EDITSTYLE_HOLE(L"Global Variable"), L"fore:#CC3300" },
	{ SCE_C_IDENTIFIER, NP2STYLE_Variable, EDITSTYLE_HOLE(L"Variable"), L"fore:#808000" },
	{ SCE_C_PREPROCESSOR, NP2STYLE_Function, EDITSTYLE_HOLE(L"Function"), L"fore:#A46000" },
	{ SCE_C_CLASS, NP2STYLE_Type, EDITSTYLE_HOLE(L"Type"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#008000" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, 0, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_LABEL, NP2STYLE_Label, EDITSTYLE_HOLE(L"Label"), L"back:#FFC040" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
};

EDITLEXER lexLLVM = {
	SCLEX_LLVM, NP2LEX_LLVM,
	EDITLEXER_HOLE(L"LLVM IR", Styles_LLVM),
	L"ll",
	&Keywords_LLVM,
	Styles_LLVM
};

