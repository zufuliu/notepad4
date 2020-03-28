#include "EditLexer.h"
#include "EditStyleX.h"

// https://en.wikipedia.org/wiki/Common_Intermediate_Language
// https://www.ecma-international.org/publications/standards/Ecma-335.htm

static KEYWORDLIST Keywords_CIL = {{
"THE_END abstract algorithm alignment as assembly assert at beforefieldinit catch cdecl "
"cil class const default demand deny endfault "
"endfinally endmac enum explicit extends extern false famandassem family famorassem "
"fastcall fault field filter final finally forwardref fromunmanaged hidebysig il "
"illegal implements implicitcom implicitres import in inheritcheck init "
"initonly instance interface internalcall lasterr linkcheck "
"literal managed marshal method nested newslot "
"noappdomain noinlining nomachine nomangle nometadata noncasdemand "
"noncasinheritance noncaslinkdemand noprocess not_in_gc_heap notremotable "
"notserialized null nullref opt optil out permitonly pinned pinvokeimpl "
"prefix prefix1 prefix2 prefix3 prefix4 prefix5 prefix6 prefix7 "
"prefixref prejitdeny prejitgrant preservesig private privatescope "
"protected public request retval rtspecialname runtime sealed "
"sequential serializable special specialname static stdcall struct "
"synchronized thiscall to true unaligned unmanaged unmanagedexp unused "
"userdefined value virtual void winapi with wrapper "
//
"break switch throw rethrow ref sizeof endfilter "
"constrained no readonly tail volatile " // "unsigned"

, // 1 Type Keyword
"ansi any array auto autochar blob blob_object bool bstr bytearray byvalstr "
"carray cf char clsid currency custom date decimal error filetime fixed float float32 "
"float64 handler hresult idispatch int int16 int32 int64 int8 iunknown lcid lpstr "
"uint uint16 uint32 uint64 uint8 "
"lptstr lpstruct lpvoid lpwstr native object objectref record refany "
"safearray storage stored_object stream streamed_object string syschar "
"sysstring tbstr typedref unicode unsigned valuetype vararg variant vector wchar "

, // 2 Preprocessor
"line"

, // 3 directive
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

, // 9 Directive
"addon assembly cctor class corflags ctor custom data emitbyte entrypoint "
"event export field file fire get hash imagebase import language line "
"locale localized locals manifestres maxstack method module mresource "
"namespace other override pack param pdirect permission permissionset "
"property publickey publickeytoken removeon set size stackreserve subsystem "
"try ver vtable vtentry vtfixup zeroinit "

, // 10 Instruction
"add ovf and arglist beq bge bgt ble blt bne br brfalse brtrue call calli "
"ceq cgt ckfinite clt conv cpblk div dup initblk jmp "
"ldarg ldarga ldc ldftn ldind ldloc ldnull leave localloc mul neg nop not or pop "
"rem ret shl shr starg stind stloc sub xor "
// Object Model Instruction
"box callvirt castclass cpobj initobj isinst ldelem ldelema ldfld ldflda ldlen ldobj "
"ldsfld ldsflda ldstr ldtoken ldvirtftn mkrefany newobj refanytype refanyval "
"stelem stfld stobj stsfld unbox "
"newarr brinst brnull brzero "
"callmostderived "
"ldloca modopt modreq "
"reqmin reqopt reqrefuse reqsecobj "
"tls "

, NULL, NULL, NULL, NULL

, // 15 Code Snippet
"ctor() "
}};

static EDITSTYLE Styles_CIL[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2StyleX_TypeKeyword, L"fore:#0000FF" },
	{ SCE_C_DIRECTIVE, NP2StyleX_Directive, L"fore:#FF8000" },
	{ SCE_C_ASM_INSTRUCTION, NP2StyleX_Instruction, L"fore:#0080FF" },
	//{ SCE_C_PREPROCESSOR, NP2StyleX_Preprocessor, L"fore:#FF8000" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, 0, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_LABEL, NP2StyleX_Label, L"back:#FFC040" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
	//{ SCE_C_ASM_REGISTER, NP2StyleX_Register, L"fore:#FF8000" },
};

EDITLEXER lexCIL = {
	SCLEX_CIL, NP2LEX_CIL,
	EDITLEXER_HOLE(L"CIL Assembly", Styles_CIL),
	L"il",
	&Keywords_CIL,
	Styles_CIL
};
