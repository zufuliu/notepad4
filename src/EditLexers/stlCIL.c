#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikipedia.org/wiki/Common_Intermediate_Language
// http://www.ecma-international.org/publications/standards/Ecma-335.htm

static KEYWORDLIST Keywords_CIL = {
"THE_END abstract algorithm alignment as assembly assert at beforefieldinit catch cdecl "
"cil class const default demand deny endfault "
"endfinally endmac enum explicit extends extern false famandassem family famorassem "
"fastcall fault field filter final finally forwardref fromunmanaged hidebysig il "
"illegal implements implicitcom implicitres import in inheritcheck init "
"initonly instance interface internalcall lasterr linkcheck "
"literal managed marshal method nested newslot "
"noappdomain  noinlining nomachine nomangle nometadata noncasdemand "
"noncasinheritance noncaslinkdemand noprocess not_in_gc_heap notremotable "
"notserialized null nullref opt optil out permitonly pinned pinvokeimpl "
"prefix prefix1 prefix2 prefix3 prefix4 prefix5 prefix6 prefix7 "
"prefixref prejitdeny prejitgrant preservesig private privatescope "
"protected public  request  retval rtspecialname runtime sealed "
"sequential serializable special specialname static stdcall struct  "
"synchronized  thiscall  to true unaligned unmanaged unmanagedexp unused "
"userdefined value virtual void winapi with wrapper "
//
"break switch throw rethrow ref sizeof endfilter    "
"constrained no readonly tail  volatile " // "unsigned"

, // type keyword
"ansi any array auto autochar blob blob_object bool bstr bytearray byvalstr "
"carray cf char clsid currency custom date decimal error filetime fixed float float32 "
"float64 handler hresult idispatch int int16 int32 int64 int8 iunknown lcid lpstr "
"lptstr lpstruct lpvoid lpwstr native object objectref record refany "
"safearray storage stored_object stream streamed_object string syschar "
"sysstring tbstr typedref unicode unsigned valuetype vararg variant vector wchar "
, // preprocessor
"line"
, // directive
""
, // attribute
""
, // class
""
, // interface
""
, // enumeration
""
, // constant
""

#if NUMKEYWORD == 16
, // directive
"addon assembly cctor class corflags ctor custom data emitbyte entrypoint "
"event export field file fire get hash imagebase import language line "
"locale localized locals manifestres maxstack method module mresource "
"namespace other override pack param pdirect permission permissionset "
"property publickey publickeytoken removeon set size stackreserve subsystem "
"try ver vtable vtentry vtfixup zeroinit "
, // instructions
"add ovf and arglist beq bge bgt ble blt bne br brfalse brtrue call calli "
"ceq cgt ckfinite clt conv cpblk div dup initblk jmp "
"ldarg ldarga ldc ldftn ldind ldloc ldnull leave localloc mul neg nop not or pop "
"rem ret shl shr starg stind stloc sub xor "
// object model instructions
"box callvirt castclass cpobj initobj isinst ldelem ldelema ldfld ldflda ldlen ldobj "
"ldsfld ldsflda ldstr ldtoken ldvirtftn mkrefany newobj refanytype refanyval "
"stelem stfld stobj stsfld unbox "
" newarr           brinst brnull  brzero "
"  callmostderived          "
"             "
"ldloca         modopt modreq "
"       reqmin reqopt reqrefuse reqsecobj     "
"       tls   "
,"","","","",
"ctor() "
#endif
};

EDITLEXER lexCIL = { SCLEX_CIL, NP2LEX_CIL, L".NET IL", L"il", L"", &Keywords_CIL,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_DIRECTIVE, NP2STYLE_Directive, L"Directive", L"fore:#0000FF", L"" },
	{ SCE_C_ASM_INSTRUCTION, NP2STYLE_Instruction, L"Instruction", L"fore:#0080FF", L"" },
	//{ SCE_C_PREPROCESSOR, NP2STYLE_Preprocessor, L"Preprocessor", L"fore:#FF8000", L"" },
	//{ MULTI_STYLE(SCE_C_COMMENT,SCE_C_COMMENTLINE,SCE_C_COMMENTDOC,SCE_C_COMMENTLINEDOC), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	//{ MULTI_STYLE(SCE_C_STRING,SCE_C_CHARACTER,SCE_C_STRINGEOL,0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_C_COMMENT,SCE_C_COMMENTLINE,0,0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_C_STRING,SCE_C_CHARACTER,0,0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_LABEL, NP2STYLE_Label, L"Label", L"fore:#000000; back:#FFC040", L""},
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	//{ SCE_C_ASM_REGISTER, NP2STYLE_Register, L"Register", L"fore:#FF8000", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
