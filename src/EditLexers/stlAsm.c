#include "EditLexer.h"
#include "EditStyle.h"

// https://docs.microsoft.com/en-us/cpp/assembler/masm/microsoft-macro-assembler-reference
// https://sourceware.org/binutils/docs/as/

static KEYWORDLIST Keywords_ASM = {{
// 0 Instruction
"aaa aad aam aas adc add and arpl bound bsf bsr bswap bt btc btr bts call cbw cwde "
"cdqe clc cld cli clts cmc cmova cmovae cmovb cmovbe cmovc cmove cmovg cmovge cmovl "
"cmovle cmovna cmovnae cmovnb cmovnbe cmovnc cmovne cmovng cmovnge cmovnl cmovnle "
"cmovno cmovnp cmovns cmovnz cmovo cmovp cmovpe cmovpo cmovs cmovz cmp cmps cmpsb "
"cmpsw cmpsd cmpsq cmpxchg cmpxch8b cmpxchg8b cmpxchg16b cpuid cwd cdq cqo daa das "
"dec div emms enter hlt idiv imul in inc ins insb insw insd int int3 into invd invlpg "
"iret iretd ja jae jb jbe jc jcxz jecxz jrcxz je jg jge jl jle jna jnae jnb jnbe "
"jnc jne jng jnge jnl jnle jno jnp jns jnz jo jp jpe jpo js jz jmp lahf lar lds les "
"lfs lgs lss lea leave lgdt lidt lldt lmsw lock lods lodsb lodsw lodsd lodsq loop "
"loope loopne loopnz loopz lsl ltr mov movbe movs movsb movsw movsd movsq movsx "
"movsxd movzx mul neg nop not npad or out outs outsb outsw outsd pause pop popa popad "
"popf popfd popfq push pusha pushad pushf pushfd rcl rcr rol ror rdmsr rdpmc rdtsc "
"rdtscp rep repe repz repne repnz ret retf rsm sahf sal sar shl shr sbb scas scasb "
"scasw scasd seta setae setb setbe setc sete setg setge setl setle setna setnae "
"setnb setnbe setnc setne setng setnge setnl setnle setno setnp setns setnz seto "
"setp setpe setpo sets setz sgdt shld shrd sidt sldt smsw stc std sti stos stosb "
"stosw stosd stosq str sub swapgs syscall sysenter sysexit sysret test ud2 verr "
"verw wait wbinvd wrmsr xadd xchg xgetbv xlat xlatb xor xrstor xsave xsetbv "
// AMD
"clgi invlpga iretq scasq stgi "
// GNU Assembler Instruction
"adcl adcb adcq addl addq addw andb andl andq andw bsrl btl btsl cbtw cltd cltq cwtl cmpb cmpl "
"cmpq cmpw decl decq decw divl divq fldl fstpl idivl imull incl incq incw insl jmpl jmpq "
"leal leaq lgdtl lidtl ljmp ljmpl lret lretq lretw movabs movabsw movabsq movb movl movsbl movsl movq movslq "
"movswl movw movzb movzbl movzwl mull mulq negl negw negq notl notq outsl orb orl orw orq popal popaw imulq "
"popfl popl popq popw pushal pushfl pushfq pushl pushq rclb rcll rcrl retl retq roll "
"sall salq sarl sarb sbbb sbbl setaeb shldl shll shrb shrdl shrl shrq shrw stosl subb "
"subl subq subw testb testl testq testw xorb xorl xorq xorw "

, // 1 FPU Instruction
"f2xm1 fabs fadd faddp fiadd fbld fbstp fchs fclex fnclex fcmovb fcmove fcmovbe "
"fcmovu fcmovnb fcmovne fcmovnbe fcmovnu fcom fcomp fcompp fcomi fcomip fucomi "
"fucomip fcos fdecstp fdiv fdivp fidiv fdivr fdivrp fidivr ffree ficom ficomp fild "
"fincstp finit fninit fist fistp fisttp fld fld1 fldl2t fldl2e fldpi fldlg2 fldln2 "
"fldz fldcw fldenv fmul fmulp fimul fnop fpatan fprem fprem1 fptan frndint frstor "
"fsave fnsave fscale fsin fsincos fsqrt fst fstp fstcw fnstcw fstenv fnstenv fstsw "
"fnstsw fsub fsubp fisub fsubr fsubrp fisubr ftst fucom fucomp fucompp fxam fxch "
"fxrstor fxsave fxtract fyl2x fyl2xp1 fwait "
"faddl fstpt flds "

, // 2 Register
"eax ebx ecx edx esi edi esp ebp "
"ax ah al bx bh bl cx ch cl dx dh dl "
"si di sp bp cs ds ss es fs gs eflags eip "
"st0 st1 st2 st3 st4 st5 st6 st7 "
"mm0 mm1 mm2 mm3 mm4 mm5 mm6 mm7 mxcsr msr "
"xmm0 xmm1 xmm2 xmm3 xmm4 xmm5 xmm6 xmm7 "
"xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15 "
"cr0 cr2 cr3 cr4 gdtr ldtr idtr dr0 dr1 dr2 dr3 dr6 dr7 "
"rax rbx rcx rdx rsi rdi rbp rsp "
"r8 r9 r10 r11 r12 r13 r14 r15 "
"r8d r9d r10d r11d r12d r13d r14d r15d "
"r8w r9w r10w r11w r12w r13w r14w r15w "
"sil dil bpl spl r8l r9l r10l r11l r12l r13l r14l r15l "
"r8b r9b r10b r11b r12b r13b r14b r15b "
"ymm0 ymm1 ymm2 ymm3 ymm4 ymm5 ymm6 ymm7 "
"ymm8 ymm9 ymm10 ymm11 ymm12 ymm13 ymm14 ymm15 "
"rip st tr3 tr4 tr5 tr6 tr7 eiz "

, // 3 Directive
".186 .286 .286c .286p .287 .386 .386c .386p .387 .486 .486p .8086 .8087 .alpha .break .code .const .continue .cref .data .data? .dosseg .else .elseif .endif .endw .err .err1 .err2 .errb .errdef .errdif .errdifi .erre .erridn .erridni .errnb .errndef .errnz .exit .fardata .fardata? .if .lall .lfcond .list .listall .listif .listmacro .listmacroall .model .msfloat .no87 .nocref .nolist .nolistif .nolistmacro .radix .repeat .sall .seq .sfcond .stack .startup .tfcond .type .until .untilcxz .while .xall .xcref .xlist "
"absolute alias align alignb assume at begin bits catstr comm comment common cpu db rb dd df dosseg dq dt dup dw "
"echo else elseif elseif1 elseif2 elseifb elseifdef elseifdif elseifdifi elseife elseifidn "
"elseifidni elseifnb elseifndef end endif endm endp ends endstruc eq equ even exitm export extern "
"externdef extrn for forc ge global goto group gt high highword iend if if1 if2 ifb ifdef ifdif "
"ifdifi ife ifidn ifidni ifnb ifndef import incbin include includelib instr invoke irp irpc "
"istruc label le length lengthof local low lowword lroffset lt macro mask mod name ne offset "
"opattr option org page popcontext proc endproc proto ptr public purge pushcontext record repeat rept "
"resb resd resq rest resw return section seg segment short size sizeof sizestr struc struct substr "
"subtitle subttl textequ this times title type typedef union use16 use32 while width "
"%arg %assign %elifctk %elifdef %elifid %elifidn %elifidni %elifmacro %elifnctk "
"%elifndef %elifnid %elifnidn %elifnidni %elifnmacro %elifnnum %elifnstr %elifnum %elifstr "
"%endmacro %endrep %exitrep %iassign %idefine %ifctx %ifid %ifidn "
"%ifidni %ifmacro %ifnctk %ifnid %ifnidn %ifnidni %ifnmacro %ifnnum %ifnstr %ifnum %ifstr "
"%imacro %local %macro %out %pop %push %rep %repl %rotate %stacksize %strlen "
"%substr %xdefine %xidefine "
"cominvk entry format library do switch case default elsif endproc endswitch endsw dialog enddialog "
"comcall interface ccall calll heap stack syscall stdcall locals endlocals "
"$eject $if $else $elseif $endif rseg cseg $restore $save $set data code bit "

, // 4 Directive Operand
"$ $$ %0 %1 %2 %3 %4 %5 %6 %7 %8 %9 .bss .data .rdata .text ? @b @f a16 a32 abs addr all assumes at "
"basic byte c carry? casemap common compact cpu dotname dword emulator epilogue error export "
"expr16 expr32 far far16 far32 farstack flat forceframe fortran fword huge language large listing "
"loadds m510 medium memory near near16 near32 nearstack nodotname noemulator nokeyword "
"noljmp nom510 none nonunique nooldmacros nooldstructs noreadonly noscoped nosignextend nosplit "
"nothing notpublic o16 o32 oldmacros oldstructs os_dos overflow? para parity? pascal private "
"prologue qword radix readonly real10 real4 real8 req sbyte scoped sdword seq setif2 sign? small "
"smallstack sword tbyte tiny use16 use32 uses vararg word wrt zero? "
"ptrdiff_t size_t int_t uint_t int32_t uint32_t int64_t uint64_t null true false "
"const _text pdata xdata imagerel "

, // 5 Extended Instruction
// XMM SSE SSE2 SSE4
"addpd addps addsd addss addsubpd addsubps andpd andps andnpd andnps blendpd blendps "
"blendvpd blendvps clflush cmppd cmpeqpd cmpltpd cmpltpd cmplepd cmpunordpd cmpneqpd "
"cmpnltpd cmpnlepd cmpordpd cmpps cmpeqps cmpltps cmpleps cmpunordps cmpneqps "
"cmpnltps cmpnleps cmpordps cmpsd cmpeqsd cmpltsd cmplesd cmpunordsd cmpneqsd "
"cmpnltsd cmpnlesd cmpordsd cmpss cmpeqss cmpltss cmpunordss cmpneqss cmpnltss "
"cmpnless cmpordss comisd comiss crc32 cvtdq2pd cvtdq2ps cvtpd2dq cvtpd2pi cvtpd2ps "
"cvtpi2pd cvtpi2ps cvtps2dq cvtps2pd cvtps2pi cvtsd2si cvtsd2ss cvtsi2sd cvtsi2ss "
"cvtss2sd cvtss2si cvttpd2dq cvttpd2pi cvttps2dq cvttps2pi cvttsd2si cvttss2si "
"divpd divps divsd divss dppd dpps extractps haddpd haddps hsubpd hsubps insertps "
"lddqu ldmxcsr lfence maskmovdqu maskmovq maxpd maxps maxsd maxss mfence minpd minps "
"minsd minss monitor movapd movaps movd movddup movdqa movdqu movdq2q movhlps "
"movhpd movhps movlhps movlpd movlps movmskpd movmskps movntdqa movntdq movnti "
"movntpd movntps movntq movq2dq movsd movshdup movsldup movss movupd movups mpsadbw "
"mulpd mulps mulsd mulss mwait "
"orpd orps pabsb padsw pabsd packsswb packssdw packusdw packuswb paddb paddw "
"paddd paddq paddsb paddsw paddusb paddusw palignr pand pandn pavgb pavgw "
"pblendvb pblendw pcmpeqb pcmpeqw pcmpeqd pcmpeqq pcmpestri pcmpestrm pcmpistri "
"pcmpistrm pcmpgtb pcmpgtw pcmpgtd pcmpgtq pextrb pextrd pextrq pextrw phaddw "
"phaddd phaddsw phminposuw phsubw phsubd phsubsw pinsrb pinsrd pinsrq pinsrw "
"pmaddubsw pmaddwd pmaxsb pmaxsd pmaxsw pmaxub pmaxud pmaxuw pminsb pminsd pminsw "
"pminub pminud pminuw pmovmskb pmovsx pmovzx pmuldq pmulhrsw pmulhuw pmulhw pmulld "
"pmullw pmuludq popcnt por prefetcht0 prefetcht1 prefetcht2 prefetchnta psadbw "
"pshufb pshufd pshufhw pshuflw pshufw psignb psignw psignd pslldq psllw pslld psllq "
"psraw psrad psrldq psrlw psrld psrlq psubb psubw psubd psubq psubsb psubsw psubusb "
"psubusw ptest punpckhbw punpckhwd punpckhdq punpckhqdq punpcklbw punpcklwd punpckldq "
"punpcklqdq pxor rcpps rcpss roundpd roundps roundsd roundss rsqrtps rsqrtss sfence "
"shufpd shufps sqrtpd sqrtps sqrtsd sqrtss stmxcsr subpd subps subsd subss ucomisd "
"ucomiss unpckhpd unpckhps unpcklpd unpcklps xorpd xorps "
// VMX
"invept invvpid vmcall vmclear vmlaunch vmresume vmptrld vmptrst vmread vmwrite vmxoff vmxon "
// AMD
"lzcnt prefetch prefetchw skinit vmload vmmcall vmrun vmsave "
"extrq insertq movntsd movntss "
// AMD 3DNow!
"femms pavgusb pf2id pf2iw pfacc pfadd pfcmpeq pfcmpge pfcmpgt "
"pfmax pfmin pfmul pfnacc pfpnacc pfrcp pfrcpit1 "
"pfrcpit2 pfrsqit1 pfrsqrt pfsub pfsubr pi2fd pi2fw pmulhrw pswapd "
//AMD XOP/FMA4

, // 6 Fold Begin
"do .while .repeat switch if .if if1 if2 ife ifb ifnb ifdef .ifdef ifndef .ifndef ifidn ifidni macro istruc struc struct union dialog proc .def .func .macro .irp .irpc .rept $if %ifnnum %ifidn %ifidni %ifstr %ifctx %imacro %macro %rep .cfi_startproc .seh_proc "
"segment "
, // 7 Fold End
"while .endw .until endswitch endsw endif endm iend ends enddialog endproc endp .endef .endfunc .endif .endm .endr $endif %endmacro %endrep .cfi_endproc .seh_endproc "

, // 8 GNU Assembler Directive
"abort align altmacro ascii asciz balign balignl balignw byte code16 code16gcc code32 code64 comm def desc dim double eject else elseif end endef endfunc endif endm endr equ equiv eqv err error even exitm extern fail file fill float func global globl gnu_attribute hidden hword ident if ifdef ifndef incbin include int internal irp irpc lcomm lflags line linkonce list ln loc loc_mark_labels local long macro endm mri noaltmacro nolist octa org p2align p2alignl p2alignw popsection previous print protected psize purgem pushsection quad reloc rept sbttl scl section set short single size skip sleb128 space stabd stabn stabs string string8 string16 string32 string64 struct subsection symver tag text title type uleb128 val version vtable_entry vtable_inherit warning weak weakref word secrel32 "
" cfi_startproc cfi_endproc cfi_personality cfi_lsda encoding cfi_def_cfa cfi_def_cfa_register cfi_def_cfa_offset cfi_adjust_cfa_offset cfi_offset cfi_rel_offset cfi_register cfi_restore cfi_undefined cfi_same_value cfi_remember_state cfi_return_column cfi_signal_frame cfi_window_save cfi_escape cfi_val_encoded_addr cfi_sections "
"seh_proc seh_endproc seh_pushreg seh_endprologue seh_stackalloc seh_setframe seh_handler seh_handlerdata "

, // 9 Proprocessor
"define elif else endif error if ifdef ifndef import include line pragma undef using "
"warning message assert unassert include_next ident sccs "

, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_ASM[] = {
	EDITSTYLE_DEFAULT,
	{ MULTI_STYLE(SCE_ASM_COMMENT, SCE_ASM_COMMENTLINE, SCE_ASM_COMMENT2, SCE_ASM_COMMENTDIRECTIVE), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ MULTI_STYLE(SCE_ASM_STRING, SCE_ASM_CHARACTER, SCE_ASM_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_ASM_PREPROCESSOR, NP2STYLE_Preprocessor, EDITSTYLE_HOLE(L"Preprocessor"), L"fore:#FF8000" },
	{ SCE_ASM_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_ASM_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ SCE_ASM_CPUINSTRUCTION, 63551, EDITSTYLE_HOLE(L"CPU Instruction"), L"fore:#0080FF" },
	{ SCE_ASM_MATHINSTRUCTION, 63552, EDITSTYLE_HOLE(L"FPU Instruction"), L"fore:#FF0080" },
	{ SCE_ASM_EXTINSTRUCTION, 63553, EDITSTYLE_HOLE(L"Extended Instruction"), L"fore:#8000FF" },
	{ SCE_ASM_DIRECTIVE, NP2STYLE_Directive, EDITSTYLE_HOLE(L"Directive"), L"fore:#0000FF" },
	{ SCE_ASM_DIRECTIVEOPERAND, 63554, EDITSTYLE_HOLE(L"Directive Operand"), L"fore:#0000FF" },
	{ SCE_ASM_REGISTER, NP2STYLE_Register, EDITSTYLE_HOLE(L"Register"), L"fore:#FF8000" },
	{ SCE_ASM_LABEL, NP2STYLE_Label, EDITSTYLE_HOLE(L"Label"), L"fore:#000000; back:#FFC040" },
};

EDITLEXER lexASM = {
	SCLEX_ASM, NP2LEX_ASM,
	EDITLEXER_HOLE(L"Assembler Source", Styles_ASM),
	L"asm; s; sx; inc; a51",
	&Keywords_ASM,
	Styles_ASM
};

