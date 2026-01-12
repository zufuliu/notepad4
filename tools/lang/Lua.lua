-- Lua 5.0 to 5.5 https://www.lua.org/manual/
-- http://lua-users.org/wiki/

--! keywords			=======================================================
and
break
false
for do
end
function
end
global
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

--! Metamethod			=======================================================
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

--! Basic Function		=======================================================
_G
_VERSION
assert(v [, message])
collectgarbage([opt [, arg]])
-- opt for collectgarbage()
"collect stop restart count step isrunning incremental generational param"
dofile([filename])
error(message [, level])
getmetatable(object)
ipairs(t)
load(chunk [, chunkname [, mode [, env]]])
loadfile([filename [, mode [, env]]])
next(table [, index])
pairs(t)
pcall(f [, arg1, ···])
print(···)
rawequal(v1, v2)
rawget(table, index)
rawlen(v)
rawset(table, index, value)
require(modname)
select(index, ···)
setmetatable(table, metatable)
tonumber(e [, base])
tostring(v)
type(v)
-- results for type() function
"nil number string boolean table function thread userdata"
warn(msg1, ···)
xpcall(f, msgh [, arg1, ···])

--! library				=======================================================
-- The Standard Libraries

-- Coroutine Manipulation
coroutine.close(co)
coroutine.create(f)
coroutine.isyieldable([co])
coroutine.resume(co [, val1, ···])
coroutine.running()
coroutine.status(co)
coroutine.wrap(f)
coroutine.yield(···)

-- Modules
require(modname)
package.config
package.cpath
package.loaded
package.loadlib(libname, funcname)
package.path
package.preload
package.searchers
package.searchpath(name, path [, sep [, rep]])

-- String Manipulation
string.byte(s [, i [, j]])
string.char(···)
string.dump(Function [, strip])
string.find(s, pattern [, init [, plain]])
string.format(formatstring, ···)
string.gmatch(s, pattern [, init])
string.gsub(s, pattern, repl [, n])
string.len(s)
string.lower(s)
string.match(s, pattern [, init])
string.pack(fmt, v1, v2, ···)
string.packsize(fmt)
string.rep(s, n [, sep])
string.reverse(s)
string.sub(s, i [, j])
string.unpack(fmt, s [, pos])
string.upper(s)

-- UTF-8 Support
utf8.char(···)
utf8.charpattern
utf8.codes(s [, lax])
utf8.codepoint(s [, i [, j [, lax]]])
utf8.len(s [, i [, j [, lax]]])
utf8.offset(s, n [, i])

-- Table Manipulation
table.concat(list [, sep [, i [, j]]])
table.create(nseq [, nrec])
table.insert(list, [pos,] value)
table.move(a1, f, e, t [,a2])
table.pack(···)
table.remove(list [, pos])
table.sort(list [, comp])
table.unpack(list [, i [, j]])

-- Mathematical Functions
math.abs(x)
math.acos(x)
math.asin(x)
math.atan(y [, x])
math.ceil(x)
math.cos(x)
math.deg(x)
math.exp(x)
math.floor(x)
math.fmod(x, y)
math.frexp(x)
math.huge
math.ldexp(m, e)
math.log(x [, base])
math.max(x, ···)
math.maxinteger
math.min(x, ···)
math.mininteger
math.modf(x)
math.pi
math.rad(x)
math.random([m [, n]])
math.randomseed([x [, y]])
math.sin(x)
math.sqrt(x)
math.tan(x)
math.tointeger(x)
math.type(x)
-- results for math.type() function
"integer float"
math.ult(m, n)

-- Input and Output Facilities
io.close([file])
io.flush()
io.input([file])
io.lines([filename, ···])
io.open(filename [, mode])
io.output([file])
io.popen(prog [, mode])
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
file:setvbuf(mode [, size])
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
debug.gethook([thread])
debug.getinfo([thread,] f [, what])
debug.getlocal([thread,] f, local)
debug.getmetatable(value)
debug.getregistry()
debug.getupvalue(f, up)
debug.getuservalue(u, n)
debug.sethook([thread,] hook, mask [, count])
debug.setlocal([thread,] level, local, value)
debug.setmetatable(value, table)
debug.setupvalue(f, up, value)
debug.setuservalue(udata, value, n)
debug.traceback([thread,] [message [, level]])
debug.upvalueid(f, n)
debug.upvaluejoin(f1, n1, f2, n2)

-- environment variables
"LUA_CPATH LUA_INIT LUA_PATH"
