; LLVM 10 https://llvm.org/docs/LangRef.html

;! Keywords			===========================================================
; Linkage Types
private
internal
available_externally
linkonce
weak
common
appending
extern_weak
linkonce_odr
weak_odr
external

; Global Variables
global
constant

; Functions
define
section
comdat
gc
prefix
prologue
personality

; Aliases
alias
; IFuncs
ifunc
; Parameter Attributes
declare
; Attribute Groups
attributes
; Module-Level Inline Assembly
module asm
; Data Layout
target datalayout
; Target Triple
target triple
; Use-list Order Directives
uselistorder uselistorder_bb
; Source Filename
source_filename

; Constants
true false
null none
global
undef

; Metadata
distinct

;! Type				===========================================================
void
i1 i8 i16 i32 i64 i128
half
float
double
fp128
x86_fp80
ppc_fp128
x86_mmx
vscale 			; Vector Type
label			; Label Type
token			; Token Type
metadata		; Metadata Type
type			; Structure Type
opaque			; Opaque Structure Types

;! Attribute		===========================================================
; Calling Conventions
ccc
fastcc
coldcc
webkit_jscc
anyregcc
preserve_mostcc
preserve_allcc
cxx_fast_tlscc
swiftcc
tailcc
cfguard_checkcc

; Visibility Styles
default
hidden
protected

; DLL Storage Classes
dllimport
dllexport

; Thread Local Storage Models
thread_local()
localdynamic
initialexec
localexec

; Runtime Preemption Specifiers
dso_preemptable
dso_local

; Global Variables
unnamed_addr
local_unnamed_addr

; Comdats
any
exactmatch
largest
noduplicates
samesize

; Parameter Attributes
zeroext
signext
inreg
byval
byval(<ty>)
inalloca
sret
align <n>
noalias
nocapture
nofree
nest
returned
nonnull
dereferenceable(<n>)
dereferenceable_or_null(<n>)
swiftself
swifterror
immarg

; Function Attributes
alignstack(<n>)
allocsize(<EltSizeParam>[, <NumEltsParam>])
alwaysinline
builtin
cold
convergent
inaccessiblememonly
inaccessiblemem_or_argmemonly
inlinehint
jumptable
minsize
naked
no-inline-line-tables
no-jump-tables
nobuiltin
noduplicate
nofree
noimplicitfloat
noinline
nonlazybind
noredzone
indirect-tls-seg-refs
noreturn
norecurse
willreturn
nosync
nounwind
null-pointer-is-valid
optforfuzzing
optnone
optsize
patchable-function
probe-stack
readnone
readonly
stack-probe-size
no-stack-arg-probe
writeonly
argmemonly
returns_twice
safestack
sanitize_address
sanitize_memory
sanitize_thread
sanitize_hwaddress
sanitize_memtag
speculative_load_hardening
speculatable
ssp
sspreq
sspstrong
strictfp
thunk
uwtable
nocf_check
shadowcallstack

; Atomic Memory Ordering Constraints
unordered
monotonic
acquire
release
acq_rel
seq_cst

; Fast-Math Flags
nnan
ninf
nsz
arcp
contract
afn
reassoc
fast

; Pointer Type
addrspace()

; Constants
zeroinitializer
blockaddress(@function, %block)

; Inline Assembler
sideeffect
alignstack
inteldialect

; Instruction
nuw
nsw
exact
atomic
volatile
inbounds
inrange
syncscope("<target-scope>")

;! Instruction		===========================================================
; Terminator Instructions
ret <type> <value>
ret void
br i1 <cond>, label <iftrue>, label <iffalse>
br label <dest>
switch <intty> <value>, label <defaultdest> [ <intty> <val>, label <dest> ... ]
indirectbr <somety>* <address>, [ label <dest1>, label <dest2>, ... ]
invoke [cconv] [ret attrs] [addrspace(<num>)] <ty>|<fnty> <fnptrval>(<function args>) [fn attrs]
	[operand bundles] to label <normal label>
	unwind label <exception label>
callbr [cconv] [ret attrs] [addrspace(<num>)] <ty>|<fnty> <fnptrval>(<function args>) [fn attrs]
	[operand bundles] to label <normal label> [other labels]
resume <type> <value>
catchswitch
	within <parent> [ label <handler1>, label <handler2>, ... ] unwind to
	caller
catchswitch within <parent> [ label <handler1>, label <handler2>, ... ] unwind label <default>
catchret
	from <token> to label <normal>
cleanupret from <value> unwind label <continue>
cleanupret from <value> unwind to caller
unreachable
; Unary Operations
fneg [fast-math flags]* <ty> <op1>
; Binary Operations
add <ty> <op1>, <op2>
add nuw <ty> <op1>, <op2>
add nsw <ty> <op1>, <op2>
add nuw nsw <ty> <op1>, <op2>
fadd [fast-math flags]* <ty> <op1>, <op2>
sub <ty> <op1>, <op2>
sub nuw <ty> <op1>, <op2>
sub nsw <ty> <op1>, <op2>
sub nuw nsw <ty> <op1>, <op2>
fsub [fast-math flags]* <ty> <op1>, <op2>
mul <ty> <op1>, <op2>
mul nuw <ty> <op1>, <op2>
mul nsw <ty> <op1>, <op2>
mul nuw nsw <ty> <op1>, <op2>
fmul [fast-math flags]* <ty> <op1>, <op2>
udiv <ty> <op1>, <op2>
udiv exact <ty> <op1>, <op2>
sdiv <ty> <op1>, <op2>
sdiv exact <ty> <op1>, <op2>
fdiv [fast-math flags]* <ty> <op1>, <op2>
urem <ty> <op1>, <op2>
srem <ty> <op1>, <op2>
frem [fast-math flags]* <ty> <op1>, <op2>
; Bitwise Binary Operations
shl <ty> <op1>, <op2>
shl nuw <ty> <op1>, <op2>
shl nsw <ty> <op1>, <op2>
shl nuw nsw <ty> <op1>, <op2>
lshr <ty> <op1>, <op2>
lshr exact <ty> <op1>, <op2>
ashr <ty> <op1>, <op2>
ashr exact <ty> <op1>, <op2>
and <ty> <op1>, <op2>
or <ty> <op1>, <op2>
xor <ty> <op1>, <op2>
; Vector Operations
extractelement <n x <ty>> <val>, <ty2> <idx>
extractelement <vscale x n x <ty>> <val>, <ty2> <idx>
insertelement <n x <ty>> <val>, <ty> <elt>, <ty2> <idx>
insertelement <vscale x n x <ty>> <val>, <ty> <elt>, <ty2> <idx>
shufflevector <n x <ty>> <v1>, <n x <ty>> <v2>, <m x i32> <mask>
shufflevector <vscale x n x <ty>> <v1>, <vscale x n x <ty>> v2, <vscale x m x i32> <mask>
; Aggregate Operations
extractvalue <aggregate type> <val>, <idx>{, <idx>}*
insertvalue <aggregate type> <val>, <ty> <elt>, <idx>{, <idx>}*
; Memory Access and Addressing Operations
alloca [inalloca] <type> [, <ty> <NumElements>] [, align <alignment>] [, addrspace(<num>)]
load [volatile] <ty>, <ty>* <pointer>[, align <alignment>][, !nontemporal !<index>][, !invariant.load !<index>][, !invariant.group !<index>][, !nonnull 	!<index>][, !dereferenceable !<deref_bytes_node>][, !dereferenceable_or_null !<deref_bytes_node>][, !align !<align_node>]
load atomic [volatile] <ty>, <ty>* <pointer> [syncscope("<target-scope>")] <ordering>, align <alignment> [, !invariant.group !<index>]
	!<index> = !{ i32 1 }
	!<deref_bytes_node> = !{i64 <dereferenceable_bytes>}
	!<align_node> = !{ i64 <value_alignment> }
store [volatile] <ty> <value>, <ty>* <pointer>[, align <alignment>][, !nontemporal !<index>][, !invariant.group !<index>]
store atomic [volatile] <ty> <value>, <ty>* <pointer> [syncscope("<target-scope>")] <ordering>, align <alignment> [, !invariant.group !<index>]
fence [syncscope("<target-scope>")] <ordering>
cmpxchg [weak] [volatile] <ty>* <pointer>, <ty> <cmp>, <ty> <new> [syncscope("<target-scope>")] <success ordering> <failure ordering>
atomicrmw [volatile] <operation> <ty>* <pointer>, <ty> <value> [syncscope("<target-scope>")] <ordering>
	xchg
	add
	sub
	and
	nand
	or
	xor
	max
	min
	umax
	umin
	fadd
	fsub
getelementptr <ty>, <ty>* <ptrval>{, [inrange] <ty> <idx>}*
getelementptr inbounds <ty>, <ty>* <ptrval>{, [inrange] <ty> <idx>}*
getelementptr <ty>, <ptr vector> <ptrval>, [inrange] <vector index type> <idx>
; Conversion Operations
trunc <ty> <value>
	to <ty2>
zext <ty> <value> to <ty2>
sext <ty> <value> to <ty2>
fptrunc <ty> <value> to <ty2>
fpext <ty> <value> to <ty2>
fptoui <ty> <value> to <ty2>
fptosi <ty> <value> to <ty2>
uitofp <ty> <value> to <ty2>
sitofp <ty> <value> to <ty2>
ptrtoint <ty> <value> to <ty2>
inttoptr <ty> <value> to <ty2>[, !dereferenceable !<deref_bytes_node>][, !dereferenceable_or_null !<deref_bytes_node]
bitcast <ty> <value> to <ty2>
addrspacecast <pty> <ptrval> to <pty2>
; Other Operations
icmp <cond> <ty> <op1>, <op2>
	eq
	ne
	ugt
	uge
	ult
	ule
	sgt
	sge
	slt
	sle
fcmp [fast-math flags]* <cond> <ty> <op1>, <op2>
	oeq
	ogt
	oge
	olt
	ole
	one
	ord
	ueq
	ugt
	uge
	ult
	ule
	une
	uno
phi [fast-math-flags] <ty> [ <val0>, <label0>], ...
select [fast-math flags] selty <cond>, <ty> <val1>, <ty> <val2>
call [fast-math flags] [cconv] [ret attrs] [addrspace(<num>)]
	<ty>|<fnty> <fnptrval>(<function args>) [fn attrs] [ operand bundles ]
	tail
	musttail
	notail
va_arg <va_list*> <arglist>, <argty>
landingpad <resultty> <clause>+
landingpad <resultty>
	cleanup <clause>*
	catch <type> <value>
	filter <array constant type> <array constant>
catchpad within <catchswitch> [<args>*]
cleanuppad within <parent> [<args>*]

;! Metadata			===========================================================

;! Intrinsic		===========================================================
; Intrinsic Global Variables
@llvm.used
@llvm.compiler.used
@llvm.global_ctors
@llvm.global_dtors
; Intrinsic Functions
