-- Lua 5.0 to 5.4 https://www.lua.org/manual/
-- http://lua-users.org/wiki/

--! keywords
and
break
false
for do
end
function
end
goto
in
if then
else
elseif
end
local
nil not
or
repeat
until
return
then true
while

--! Metamethod
-- Metatables and Metamethods
__add
__band
__bnot
__bor
__bxor
__call
__close
__concat
__div
__eq
__gc
__idiv
__index
__le
__len
__lt
__metatable
__mod
__mode
__mul
__name
__newindex
__pairs
__pow
__shl
__shr
__sub
__tostring
__unm

--! Basic Function
_G
_VERSION
assert(v [, message])
collectgarbage([opt [, arg]])
-- opt for collectgarbage()
"collect stop restart count step isrunning incremental generational"
dofile([filename])
error(message [, level])
getmetatable(object)
gcinfo()								-- removed in Lua 5.1
ipairs(t)
load(chunk [, chunkname [, mode [, env]]])	-- added in Lua 5.1
loadfile([filename [, mode [, env]]])
loadlib(libname, funcname)				-- removed in Lua 5.1
loadstring(string [, chunkname])		-- removed in Lua 5.2
next(table [, index])
pairs(t)
pcall(f [, arg1, ···])
print(···)
rawequal(v1, v2)
rawget(table, index)
rawlen(v)								-- added in Lua 5.2
rawset(table, index, value)
require(modname)						-- added in Lua 5.0
select(index, ···)						-- added in Lua 5.1
setfenv(f, table)						-- removed in Lua 5.2
setmetatable(table, metatable)
tonumber(e [, base])
tostring(v)
type(v)
-- results for type() function
"nil number string boolean table function thread userdata"
unpack(list [, i [, j]])				-- removed in Lua 5.2
warn(msg1, ···)							-- added in Lua 5.4
xpcall(f, msgh [, arg1, ···])
module(name [, ···])					-- Modules, removed in Lua 5.2

--! library
-- The Standard Libraries

-- Coroutine Manipulation
coroutine.close(co)						-- added in Lua 5.4
coroutine.create(f)
coroutine.isyieldable([co])				-- added in Lua 5.3
coroutine.resume(co [, val1, ···])
coroutine.running()						-- added in Lua 5.1
coroutine.status(co)
coroutine.wrap(f)
coroutine.yield(···)

-- Modules								-- added in Lua 5.1
module(name [, ···])					-- removed in Lua 5.2
require(modname)
package.config							-- added in Lua 5.2
package.cpath
package.loaded
package.loaders							-- removed in Lua 5.2
package.loadlib(libname, funcname)
package.path
package.preload
package.searchers						-- added in Lua 5.2
package.searchpath(name, path [, sep [, rep]])	-- added in Lua 5.2
package.seeall(module)					-- removed in Lua 5.2

-- String Manipulation
string.byte(s [, i [, j]])
string.char(···)
string.dump(Function [, strip])
string.find(s, pattern [, init [, plain]])
string.format(formatstring, ···)
string.gmatch(s, pattern [, init])
string.gfind(s, pat)					-- removed in Lua 5.1
string.gsub(s, pattern, repl [, n])
string.len(s)
string.lower(s)
string.match(s, pattern [, init])
string.pack(fmt, v1, v2, ···)			-- added in Lua 5.2
string.packsize(fmt)					-- added in Lua 5.3
string.rep(s, n [, sep])
string.reverse(s)						-- added in Lua 5.1
string.sub(s, i [, j])
string.unpack(fmt, s [, pos])
string.upper(s)

-- UTF-8 Support						-- added in Lua 5.3
utf8.char(···)
utf8.charpattern
utf8.codes(s [, lax])
utf8.codepoint(s [, i [, j [, lax]]])
utf8.len(s [, i [, j [, lax]]])
utf8.offset(s, n [, i])

-- Table Manipulation
table.concat(list [, sep [, i [, j]]])
table.foreach(table, f)					-- removed in Lua 5.1
table.foreachi(table, f)				-- removed in Lua 5.1
table.getn(table)						-- removed in Lua 5.1
table.insert(list, [pos,] value)
table.move(a1, f, e, t [,a2])			-- added in Lua 5.3
table.maxn(table)						-- removed in Lua 5.2
table.pack(···)							-- added in Lua 5.2
table.remove(list [, pos])
table.sort(list [, comp])
table.setn(table, n)					-- removed in Lua 5.1
table.unpack(list [, i [, j]])			-- added in Lua 5.2

-- Mathematical Functions
math.abs(x)
math.acos(x)
math.asin(x)
math.atan(y [, x])
math.atan2(y, x)						-- Lua 5.0 to Lua 5.2
math.ceil(x)
math.cos(x)
math.cosh(x)							-- Lua 5.1 to Lua 5.2
math.deg(x)
math.exp(x)
math.floor(x)
math.fmod(x, y)							-- added in Lua 5.1
math.frexp(x)							-- Lua 5.0 to Lua 5.2
math.huge								-- added in Lua 5.1
math.ldexp(m, e)						-- Lua 5.0 to Lua 5.2
math.log(x [, base])
math.log10(x)							-- Lua 5.0 to Lua 5.2
math.max(x, ···)
math.maxinteger							-- added in Lua 5.3
math.min(x, ···)
math.mininteger							-- added in Lua 5.3
math.modf(x)							-- added in Lua 5.1
math.mod(x)								-- Lua 5.0 only
math.pi									-- added in Lua 5.1
math.pow(x, y)							-- Lua 5.0 to Lua 5.2
math.rad(x)
math.random([m [, n]])
math.randomseed([x [, y]])
math.sin(x)
math.sinh(x)							-- Lua 5.0 to Lua 5.2
math.sqrt(x)
math.tan(x)
math.tanh(x)							-- Lua 5.1 to Lua 5.2
math.tointeger(x)						-- added in Lua 5.3
math.type(x)							-- added in Lua 5.3
-- results for math.type() function
"integer float"
math.ult(m, n)							-- added in Lua 5.3

-- Bitwise Operations					-- Lua 5.2 only
bit32.arshift(x, disp)
bit32.band(···)
bit32.bnot(x)
bit32.bor(···)
bit32.btest(···)
bit32.bxor(···)
bit32.extract(n, field [, width])
bit32.replace(n, v, field [, width])
bit32.lrotate(x, disp)
bit32.lshift(x, disp)
bit32.rrotate(x, disp)
bit32.rshift(x, disp)

-- Input and Output Facilities
io.close([file])
io.flush()
io.input([file])
io.lines([filename, ···])
io.open(filename [, mode])
io.output([file])
io.popen(prog [, mode])					-- added in Lua 5.1
io.stdin
io.stdout
io.stderr
io.read(···)
io.tmpfile()
io.type(obj)
-- results for io.type() function
"file"
io.write(···)
file:close()
file:flush()
file:lines(···)
file:read(···)
file:seek([whence [, offset]])
file:setvbuf(mode [, size])				-- added in Lua 5.1
file:write(···)

-- Operating System Facilities
os.clock()
os.date([format [, time]])
os.difftime(t2, t1)
os.execute([command])
os.exit([code [, close]])
os.getenv(varname)
os.remove(filename)
os.rename(oldname, newname)
os.setlocale(locale [, category])
os.time([table])
os.tmpname()

-- The Debug Library
debug.debug()
debug.getfenv(o)						-- Lua 5.1 only
debug.gethook([thread])
debug.getinfo([thread,] f [, what])
debug.getlocal([thread,] f, local)
debug.getmetatable(value)				-- added in Lua 5.1
debug.getregistry()
debug.getupvalue(f, up)
debug.getuservalue(u, n)				-- added in Lua 5.2
debug.setcstacklimit(limit)				-- added in Lua 5.4
debug.setfenv(object, table)			-- Lua 5.1 only
debug.sethook([thread,] hook, mask [, count])
debug.setlocal([thread,] level, local, value)
debug.setmetatable(value, table)		-- added in Lua 5.1
debug.setupvalue(f, up, value)
debug.setuservalue(udata, value, n)		-- added in Lua 5.2
debug.traceback([thread,] [message [, level]])
debug.upvalueid(f, n)					-- added in Lua 5.2
debug.upvaluejoin(f1, n1, f2, n2)		-- added in Lua 5.2

-- environment variables
"LUA_CPATH LUA_INIT LUA_PATH"
