#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_Lua = {
"and break do else elseif end false for function goto if "
"in local nil not or repeat return then true until while",
// Basic Functions
"_VERSION assert collectgarbage dofile error gcinfo loadfile loadstring print rawget rawset "
"require tonumber tostring type unpack _ALERT _ERRORMESSAGE _INPUT _PROMPT _OUTPUT _STDERR "
"_STDIN _STDOUT call dostring foreach foreachi getn globals newtype sort tinsert tremove "
"_G getfenv getmetatable ipairs loadlib next pairs pcall rawequal setfenv setmetatable xpcall "
"string table math coroutine io os debug load module select",
// String Manipulation, Table Manipulation, Mathematical Functions
"abs acos asin atan atan2 ceil cos deg exp floor format frexp gsub ldexp log log10 max min "
"mod rad random randomseed sin sqrt strbyte strchar strfind strlen strlower strrep strsub strupper tan "
"string.byte string.char string.dump string.find string.len string.lower string.rep string.sub string.upper "
"string.format string.gfind string.gsub table.concat table.foreach table.foreachi table.getn table.sort "
"table.insert table.remove table.setn math.abs math.acos math.asin math.atan math.atan2 math.ceil math.cos "
"math.deg math.exp math.floor math.frexp math.ldexp math.log math.log10 math.max math.min math.mod "
"math.pi math.pow math.rad math.random math.randomseed math.sin math.sqrt math.tan string.gmatch "
"string.match string.reverse table.maxn math.cosh math.fmod math.modf math.sinh math.tanh math.huge",
// Input and Output Facilities & System Facilities Coroutine Manipulation,
//Input and Output Facilities, System Facilities (coroutine & io & os)
"openfile closefile readfrom writeto appendto remove rename flush seek tmpfile tmpname read "
"write clock date difftime execute exit getenv setlocale time coroutine.create coroutine.resume "
"coroutine.status coroutine.wrap coroutine.yield io.close io.flush io.input io.lines io.open io.output "
"io.read io.tmpfile io.type io.write io.stdin io.stdout io.stderr os.clock os.date os.difftime "
"os.execute os.exit os.getenv os.remove os.rename os.setlocale os.time os.tmpname coroutine.running "
"package.cpath package.loaded package.loadlib package.path package.preload package.seeall io.popen",
"", "", "", "", ""

#if NUMKEYWORD == 16
,"","","","","","",""
#endif
};

EDITLEXER lexLua = { SCLEX_LUA, NP2LEX_LUA, L"Lua Script", L"lua; wlua", L"", &Keywords_Lua,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_LUA_DEFAULT, L"Default", L"", L"" },
	{ SCE_LUA_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_LUA_WORD2, NP2STYLE_BasicFunction, L"Basic Function", L"fore:#FF8000", L"" },
	{ SCE_LUA_WORD3, 63452, L"String, Table & Math Functions", L"fore:#FF0080", L"" },
	{ SCE_LUA_WORD4, 63453, L"Input, Output & System Facilities", L"fore:#0080FF", L"" },
	{ MULTI_STYLE(SCE_LUA_COMMENT,SCE_LUA_COMMENTLINE,SCE_LUA_COMMENTDOC,0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_LUA_STRING,SCE_LUA_STRINGEOL,SCE_LUA_LITERALSTRING,SCE_LUA_CHARACTER), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_LUA_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_LUA_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"fore:#B000B0" },
	{ SCE_LUA_PREPROCESSOR, NP2STYLE_Preprocessor, L"Preprocessor", L"fore:#FF8000", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
