; https://github.com/red/red
; https://www.red-lang.org/p/documentation.html
; https://static.red-lang.org/red-system-specs.html

;! keywords			===========================================================
; Reserved keywords
alias all and any as assert
break
case catch comment context continue
declare
either exit
func function
if
loop
not null
or
pop push
return
size? switch
throw
until use
while with
xor
; intrinsics https://github.com/red/red/blob/master/compiler.r
all any
break
case construct context continue
does
either exit
forall foreach forever func function
get
has
if
loop
object
reduce remove-each repeat return routine
set switch
try
unless until
while
; logic-words https://github.com/red/red/blob/master/compiler.r
true false yes no on off
; others
none namespace
variadic

;! directive		===========================================================
; Compiler directives
#call
#case
#default
#define
#do
#either
#enum
#export
#get
#get-definition
#if
#import
#in
#include
#inline
#pop-path
#register-intrinsics
#script
#switch
#syscall
#system
#system-global
#typecheck
#u16
#user-code
#verbose

;! datatypes		===========================================================
; https://github.com/red/red/blob/master/environment/datatypes.red
unset!
none!
logic!
block!
string!
integer!
word!
error!
typeset!
file!
url!
set-word!
get-word!
lit-word!
refinement!
binary!
paren!
char!
issue!
path!
set-path!
get-path!
lit-path!
native!
action!
op!
function!
routine!
object!
bitset!
float!
point!
vector!
map!
hash!
pair!
percent!
tuple!
image!
time!
tag!
email!
handle!
date!
port!
money!
ref!
event!

; https://github.com/red/red/blob/master/system/emitter.r
byte!
float32!
float64!
pointer!
c-string!
subroutine!
byte-ptr!
int-ptr!
struct!

;! functions		===========================================================
print
print-line
