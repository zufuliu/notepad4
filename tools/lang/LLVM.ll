; LLVM 15 https://llvm.org/docs/LangRef.html

;! Keywords			===========================================================
; Linkage Types
; https://llvm.org/docs/LangRef.html#linkage-types
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
; https://llvm.org/docs/LangRef.html#global-variables
global
constant
section
partition
; align
comdat

; Functions
; https://llvm.org/docs/LangRef.html#functions
define
section
comdat
declare
; Garbage Collector Strategy Names
; https://llvm.org/docs/LangRef.html#garbage-collector-strategy-names
gc
; Prefix Data
; https://llvm.org/docs/LangRef.html#prefix-data
prefix

; Aliases
; https://llvm.org/docs/LangRef.html#aliases
alias
; IFuncs
; https://llvm.org/docs/LangRef.html#ifuncs
ifunc
; Attribute Groups
; https://llvm.org/docs/LangRef.html#attribute-groups
attributes
; Module-Level Inline Assembly
; https://llvm.org/docs/LangRef.html#module-level-inline-assembly
module asm
; Data Layout
; https://llvm.org/docs/LangRef.html#data-layout
target datalayout
; Target Triple
; https://llvm.org/docs/LangRef.html#target-triple
target triple
; Use-list Order Directives
; https://llvm.org/docs/LangRef.html#use-list-order-directives
uselistorder uselistorder_bb
; Source Filename
; https://llvm.org/docs/LangRef.html#source-filename
source_filename

; Constants
; https://llvm.org/docs/LangRef.html#constants
true false
null none
global
undef
poison

; Metadata
; https://llvm.org/docs/LangRef.html#metadata
distinct

;! Type				===========================================================
; https://llvm.org/docs/LangRef.html#type-system
void
i1 i8 i16 i32 i64 i128
half
bfloat
float
double
fp128
x86_fp80
ppc_fp128
x86_mmx
ptr				; Pointer Type
vscale 			; Vector Type
label			; Label Type
token			; Token Type
metadata		; Metadata Type
type			; Structure Type
opaque			; Opaque Structure Types

;! Attribute		===========================================================
; Calling Conventions
; https://llvm.org/docs/LangRef.html#calling-conventions
ccc
fastcc
coldcc
webkit_jscc
anyregcc
preserve_mostcc
preserve_allcc
cxx_fast_tlscc
swiftcc
swifttailcc
tailcc
cfguard_checkcc

; Visibility Styles
; https://llvm.org/docs/LangRef.html#visibility-styles
default
hidden
protected

; DLL Storage Classes
; https://llvm.org/docs/LangRef.html#dll-storage-classes
dllimport
dllexport

; Thread Local Storage Models
; https://llvm.org/docs/LangRef.html#thread-local-storage-models
thread_local()
localdynamic
initialexec
localexec

; Runtime Preemption Specifiers
; https://llvm.org/docs/LangRef.html#runtime-preemption-specifiers
dso_preemptable
dso_local

; Global Variables
; https://llvm.org/docs/LangRef.html#global-variables
unnamed_addr
local_unnamed_addr
no_sanitize_address
no_sanitize_hwaddress
sanitize_address_dyninit

; Comdats
; https://llvm.org/docs/LangRef.html#comdats
any
exactmatch
largest
noduplicates
samesize

; Parameter Attributes
; https://llvm.org/docs/LangRef.html#parameter-attributes
zeroext
signext
inreg
byval
byval(<ty>)
preallocated(<ty>)
inalloca
sret
align <n>
align(<n>)
noalias
nocapture
nofree
nest
returned
nonnull
dereferenceable(<n>)
dereferenceable_or_null(<n>)
swiftself
swiftasync
swifterror
immarg
noundef
alignstack(<n>)
allocalign
allocptr

; Prologue Data
; https://llvm.org/docs/LangRef.html#prologue-data
prologue
; Personality Function
; https://llvm.org/docs/LangRef.html#personality-function
personality

; Function Attributes
; https://llvm.org/docs/LangRef.html#function-attributes
alignstack(<n>)
allockind("KIND")
allocsize(<EltSizeParam>[, <NumEltsParam>])
alwaysinline
builtin
cold
convergent
disable_sanitizer_instrumentation
dontcall-error
dontcall-warn
frame-pointer
fn_ret_thunk_extern
hot
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
nomerge
nonlazybind
noredzone
indirect-tls-seg-refs
noreturn
norecurse
willreturn
nosync
nounwind
nosanitize_bounds
nosanitize_coverage
null_pointer_is_valid
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
denormal-fp-math
denormal-fp-math-f32
thunk
uwtable
nocf_check
shadowcallstack
mustprogress
warn-stack-size
vscale_range(<min>[, <max>])
min-legal-vector-width

; Call Site Attributes
; https://llvm.org/docs/LangRef.html#call-site-attributes
vector-function-abi-variant
preallocated(<ty>)

; Global Attributes
; https://llvm.org/docs/LangRef.html#global-attributes
no_sanitize_address
no_sanitize_hwaddress
sanitize_memtag
sanitize_address_dyninit

; Atomic Memory Ordering Constraints
; https://llvm.org/docs/LangRef.html#atomic-memory-ordering-constraints
unordered
monotonic
acquire
release
acq_rel
seq_cst

; Fast-Math Flags
; https://llvm.org/docs/LangRef.html#fast-math-flags
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
dso_local_equivalent
no_cfi @func

; Inline Assembler Expressions
; https://llvm.org/docs/LangRef.html#inline-assembler-expressions
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
; https://llvm.org/docs/LangRef.html#instruction-reference
; Terminator Instructions
; https://llvm.org/docs/LangRef.html#terminator-instructions
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
; https://llvm.org/docs/LangRef.html#unary-operations
fneg [fast-math flags]* <ty> <op1>
; Binary Operations
; https://llvm.org/docs/LangRef.html#binary-operations
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
; https://llvm.org/docs/LangRef.html#bitwise-binary-operations
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
; https://llvm.org/docs/LangRef.html#vector-operations
extractelement <n x <ty>> <val>, <ty2> <idx>
extractelement <vscale x n x <ty>> <val>, <ty2> <idx>
insertelement <n x <ty>> <val>, <ty> <elt>, <ty2> <idx>
insertelement <vscale x n x <ty>> <val>, <ty> <elt>, <ty2> <idx>
shufflevector <n x <ty>> <v1>, <n x <ty>> <v2>, <m x i32> <mask>
shufflevector <vscale x n x <ty>> <v1>, <vscale x n x <ty>> v2, <vscale x m x i32> <mask>
; Aggregate Operations
; https://llvm.org/docs/LangRef.html#aggregate-operations
extractvalue <aggregate type> <val>, <idx>{, <idx>}*
insertvalue <aggregate type> <val>, <ty> <elt>, <idx>{, <idx>}*
; Memory Access and Addressing Operations
; https://llvm.org/docs/LangRef.html#memory-access-and-addressing-operations
alloca [inalloca] <type> [, <ty> <NumElements>] [, align <alignment>] [, addrspace(<num>)]
load [volatile] <ty>, ptr <pointer>[, align <alignment>][, !nontemporal !<nontemp_node>][, !invariant.load !<empty_node>][, !invariant.group !<empty_node>][, !nonnull !<empty_node>][, !dereferenceable !<deref_bytes_node>][, !dereferenceable_or_null !<deref_bytes_node>][, !align !<align_node>][, !noundef !<empty_node>]
load atomic [volatile] <ty>, ptr <pointer> [syncscope("<target-scope>")] <ordering>, align <alignment> [, !invariant.group !<empty_node>]
	!<nontemp_node> = !{ i32 1 }
	!<empty_node> = !{}
	!<deref_bytes_node> = !{ i64 <dereferenceable_bytes> }
	!<align_node> = !{ i64 <value_alignment> }
store [volatile] <ty> <value>, ptr <pointer>[, align <alignment>][, !nontemporal !<nontemp_node>][, !invariant.group !<empty_node>]
store atomic [volatile] <ty> <value>, ptr <pointer> [syncscope("<target-scope>")] <ordering>, align <alignment> [, !invariant.group !<empty_node>]
	!<nontemp_node> = !{ i32 1 }
	!<empty_node> = !{}
fence [syncscope("<target-scope>")] <ordering>
cmpxchg [weak] [volatile] ptr <pointer>, <ty> <cmp>, <ty> <new> [syncscope("<target-scope>")] <success ordering> <failure ordering>[, align <alignment>]
atomicrmw [volatile] <operation> ptr <pointer>, <ty> <value> [syncscope("<target-scope>")] <ordering>[, align <alignment>]
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
	fmax
	fmin
getelementptr <ty>, ptr <ptrval>{, [inrange] <ty> <idx>}*
getelementptr inbounds <ty>, ptr <ptrval>{, [inrange] <ty> <idx>}*
getelementptr <ty>, <N x ptr> <ptrval>, [inrange] <vector index type> <idx>; Conversion Operations
; https://llvm.org/docs/LangRef.html#conversion-operations
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
; https://llvm.org/docs/LangRef.html#other-operations
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
freeze ty <val>
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
; https://llvm.org/docs/LangRef.html#metadata

;! Intrinsic		===========================================================
; Intrinsic Global Variables
; https://llvm.org/docs/LangRef.html#intrinsic-global-variables
@llvm.used
@llvm.compiler.used
@llvm.global_ctors
@llvm.global_dtors
; Intrinsic Functions
; https://llvm.org/docs/LangRef.html#intrinsic-functions
