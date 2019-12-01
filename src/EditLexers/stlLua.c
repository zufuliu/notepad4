#include "EditLexer.h"
#include "EditStyleX.h"

// https://en.wikipedia.org/wiki/Lua_(programming_language)
// https://www.lua.org/

static KEYWORDLIST Keywords_Lua = {{
"and break do else elseif end false for function goto if "
"in local nil not or repeat return then true until while"

, // 1 Basic Function
"_VERSION assert collectgarbage dofile error gcinfo loadfile loadstring print rawget rawset "
"require tonumber tostring type unpack _ALERT _ERRORMESSAGE _INPUT _PROMPT _OUTPUT _STDERR "
"_STDIN _STDOUT call dostring foreach foreachi getn globals newtype sort tinsert tremove "
"_G getfenv getmetatable ipairs loadlib next pairs pcall rawequal setfenv setmetatable xpcall "
"string table math coroutine io os debug load module select"

, // 2 String Manipulation, Table Manipulation, Mathematical Function
"abs acos asin atan atan2 ceil cos deg exp floor format frexp gsub ldexp log log10 max min "
"mod rad random randomseed sin sqrt strbyte strchar strfind strlen strlower strrep strsub strupper tan "
"string.byte string.char string.dump string.find string.len string.lower string.rep string.sub string.upper "
"string.format string.gfind string.gsub table.concat table.foreach table.foreachi table.getn table.sort "
"table.insert table.remove table.setn math.abs math.acos math.asin math.atan math.atan2 math.ceil math.cos "
"math.deg math.exp math.floor math.frexp math.ldexp math.log math.log10 math.max math.min math.mod "
"math.pi math.pow math.rad math.random math.randomseed math.sin math.sqrt math.tan string.gmatch "
"string.match string.reverse table.maxn math.cosh math.fmod math.modf math.sinh math.tanh math.huge"

, // 3 Input and Output Facilities & System Facilities Coroutine Manipulation
"openfile closefile readfrom writeto appendto remove rename flush seek tmpfile tmpname read "
"write clock date difftime execute exit getenv setlocale time coroutine.create coroutine.resume "
"coroutine.status coroutine.wrap coroutine.yield io.close io.flush io.input io.lines io.open io.output "
"io.read io.tmpfile io.type io.write io.stdin io.stdout io.stderr os.clock os.date os.difftime "
"os.execute os.exit os.getenv os.remove os.rename os.setlocale os.time os.tmpname coroutine.running "
"package.cpath package.loaded package.loadlib package.path package.preload package.seeall io.popen"

, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Lua[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_LUA_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_LUA_WORD2, NP2StyleX_BasicFunction, L"fore:#FF8000" },
	{ SCE_LUA_WORD3, NP2StyleX_StringTableMathFunc, L"fore:#FF0080" },
	{ SCE_LUA_WORD4, NP2StyleX_IOSystemFacility, L"fore:#0080FF" },
	{ SCE_LUA_FUNCTION, NP2StyleX_Function, L"fore:#A46000" },
	{ MULTI_STYLE(SCE_LUA_COMMENT, SCE_LUA_COMMENTLINE, SCE_LUA_COMMENTDOC, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_LUA_STRING, SCE_LUA_STRINGEOL, SCE_LUA_LITERALSTRING, SCE_LUA_CHARACTER), NP2StyleX_String, L"fore:#008000" },
	{ SCE_LUA_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_LUA_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
	{ SCE_LUA_PREPROCESSOR, NP2StyleX_Preprocessor, L"fore:#FF8000" },
};

EDITLEXER lexLua = {
	SCLEX_LUA, NP2LEX_LUA,
	EDITLEXER_HOLE(L"Lua Script", Styles_Lua),
	L"lua; wlua; nse; luadoc; luax",
	&Keywords_Lua,
	Styles_Lua
};
