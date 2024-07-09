// 0.13 https://ziglang.org/
// https://ziglang.org/documentation/master/

//! keywords			=======================================================
// https://ziglang.org/documentation/master/#Keyword-Reference
addrspace align allowzero and anyframe anytype asm async await
break
callconv catch comptime const continue
defer
else enum errdefer error export extern
fn for
if inline
linksection
noalias noinline nosuspend
opaque or orelse
packed pub
resume return
struct suspend switch
test threadlocal try
union unreachable usingnamespace
var volatile
while

true false null undefined

//! types				=======================================================
// https://ziglang.org/documentation/master/#Primitive-Types
i8 u8 i16 u16 i32 u32 i64 u64 i128 u128 isize usize
f16 f32 f64 f80 f128
bool
anyopaque
void
noreturn
type
anyerror

//! Builtin				=======================================================
// https://ziglang.org/documentation/master/#Builtin-Functions
@as(comptime T: type, expression) T
@export(declaration, comptime options: std.builtin.ExportOptions) void
@import(comptime path: []u8) type

//! api					=======================================================
// https://ziglang.org/documentation/master/std/
// https://ziglang.org/documentation/master/std/#std.debug
std.debug {
	fn assert(ok: bool) void
	fn print(comptime fmt: []const u8, args: anytype) void
}
