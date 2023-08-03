# 2.0 https://nim-lang.org/
# https://github.com/nim-lang/Nim
# https://nim-lang.org/docs/manual.html

#! keywords				=======================================================
# https://nim-lang.org/docs/manual.html#lexical-analysis-identifiers-amp-keywords
# https://github.com/nim-lang/Nim/blob/devel/doc/keywords.txt
addr and as asm
bind block break
case cast concept const continue converter
defer discard distinct div do
elif else end enum except export
finally for from func
if import in include interface is isnot iterator
let
macro method mixin mod
nil not notin
object of or out
proc ptr
raise ref return
shl shr static
template try tuple type
using
var
when while
xor
yield

true false

#! types				=======================================================
# https://nim-lang.org/docs/manual.html#types
int int8 int16 int32 int64
uint uint8 uint16 uint32 uint64 byte
range
float float32 float64
bool
char Rune
string cstring
array seq
openarray openArray
varargs
UncheckedArray
tuple object
set
auto
typed untyped
iterable
typedesc
void

#! API					=======================================================
# https://nim-lang.org/docs/lib.html
# https://nim-lang.org/docs/system.html
system
	proc alignof(x: typedesc): int
	proc declared(x: untyped): bool
	proc defined(x: untyped): bool
	proc sizeof[T](x: T): int
	proc typeof(x: untyped; mode = typeOfIter): typedesc

# https://nim-lang.org/docs/assertions.html
system/assertions
	template assert(cond: untyped; msg = "")
