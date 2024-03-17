// F# 8.0 https://fsharp.org/
// https://docs.microsoft.com/en-us/dotnet/fsharp/language-reference/
// https://github.com/fsharp/fslang-design

//! keywords		===========================================================
// https://docs.microsoft.com/en-us/dotnet/fsharp/language-reference/keyword-reference
abstract and as assert
base begin
class
default delegate do done downcast downto
elif else end exception extern
false finally fixed for fun function
global
if in inherit inline interface internal
lazy let
match member module mutable
namespace new not null
of open or override
private public
rec return
select static struct
then to try type
upcast use val void
when while with
yield

// OCaml reserved words
asr
land lor lsl lsr lxor
mod
sig

// future reserved
break
checked component const constraint continue
event external
include
mixin
parallel process protected pure
sealed
tailcall trait
virtual

// F# 4.1
atomic
constructor
eager
functor
measure method
object
recursive
volatile

//! types			===========================================================
// https://docs.microsoft.com/en-us/dotnet/fsharp/language-reference/literals
// https://docs.microsoft.com/en-us/dotnet/fsharp/language-reference/basic-types
bool byte sbyte
int16 uint16
int int32 uint uint32 nativeint unativeint
int64 uint64
single float32 float double
bigint decimal
char string
unit
seq

//! preprocessor	===========================================================
// https://learn.microsoft.com/en-us/dotnet/fsharp/language-reference/compiler-directives
#if symbol
#else
#endif
#line int string
#nowarn code
// https://learn.microsoft.com/en-us/dotnet/fsharp/tools/fsharp-interactive/
#r "nuget:..."
#r "assembly-name.dll"
#load "file-name.fsx"
#help
#i
#quit
#time "on"
#time "off"
// other
#indent "on"
#light

// attributes		===========================================================
// https://learn.microsoft.com/en-us/dotnet/fsharp/language-reference/attributes

// https://learn.microsoft.com/en-us/dotnet/fsharp/language-reference/xml-documentation
// omitted, treated same as C#
/// <summary>text</summary>
