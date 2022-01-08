// 2.098.1 https://dlang.org/

//! Keywords		===========================================================
// https://dlang.org/spec/lex.html#keywords
abstract alias align asm assert auto
body break
case cast catch class const continue
debug default delegate delete deprecated do
else enum export extern
false final finally for foreach foreach_reverse function
goto
if immutable import in inout int interface invariant is
lazy macro mixin module
new nothrow null
out override
package pragma private protected public pure ref return
scope shared static struct super switch synchronized
template this throw true try typeid typeof
union unittest
version
while with
__gshared __traits __vector __parameters

//! Types			===========================================================
// https://dlang.org/spec/grammar.html#types
bool byte
cdouble cent cfloat char creal
dchar double
float
idouble ifloat ireal
long
real
short
ubyte ucent uint ulong ushort
void
wchar
// https://dlang.org/spec/type.html#aliased-types
size_t
ptrdiff_t
string wstring dstring

//! Constant		===========================================================
__FILE__
__FILE_FULL_PATH__
__MODULE__
__LINE__
__FUNCTION__
__PRETTY_FUNCTION__
// https://dlang.org/spec/lex.html#specialtokens
__DATE__
__EOF__
__TIME__
__TIMESTAMP__
__VENDOR__
__VERSION__
// asm
__LOCAL_SIZE

//! Preprocessor	===========================================================
#line

//! Attribute		===========================================================
@__future
@disable
@nogc
@live
@safe
@system
@trusted
@property
// https://dlang.org/phobos/core_attribute.html
@selector()
@optional
@gnuAbiTag()

//! Trait			===========================================================
// https://dlang.org/spec/traits.html
^is ^has ^get
allMembers
child
classInstanceSize
compiles
derivedMembers
getVirtualIndex
identifier
initSymbol
parent
toType

//! Library			===========================================================
// https://dlang.org/phobos/index.html
// https://dlang.org/phobos/object.html
module object {

class Object;
struct Interface;
abstract class TypeInfo;
struct ModuleInfo;
class Throwable;
class Exception: Throwable;
class Error: Throwable;

}

// https://dlang.org/spec/iasm.html
// https://dlang.org/spec/grammar.html#d x86 inline assembler
asm {
//! asm Keywords	===========================================================
align even naked
db ds di dl df dd de dw dq
offsetof seg
ptr near far word dword qword

//! asm Register	===========================================================
AL AH AX EAX
BL BH BX EBX
CL CH CX ECX
DL DH DX EDX
BP EBP
SP ESP
DI EDI
SI ESI
ES CS SS DS GS FS
CR0 CR2 CR3 CR4
DR0 DR1 DR2 DR3 DR6 DR7
TR3 TR4 TR5 TR6 TR7
ST
MM0 MM1 MM2 MM3 MM4 MM5 MM6 MM7
XMM0 XMM1 XMM2 XMM3 XMM4 XMM5 XMM6 XMM7
// Register64
RAX RBX RCX RDX
BPL RBP
SPL RSP
DIL RDI
SIL RSI
R8B R8W R8D R8
R9B R9W R9D R9
R10B R10W R10D R10
R11B R11W R11D R11
R12B R12W R12D R12
R13B R13W R13D R13
R14B R14W R14D R14
R15B R15W R15D R15
XMM8 XMM9 XMM10 XMM11 XMM12 XMM13 XMM14 XMM15
YMM0 YMM1 YMM2 YMM3 YMM4 YMM5 YMM6 YMM7
YMM8 YMM9 YMM10 YMM11 YMM12 YMM13 YMM14 YMM15

//! asm Instruction	===========================================================
// https://dlang.org/spec/iasm.html#supported_opcodes

//! dummy
}
