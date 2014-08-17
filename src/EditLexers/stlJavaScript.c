#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_JS = {
// ECMA-262
"break case catch continue debugger default delete do else finally for function if in "
"instanceof new return switch this throw try typeof var void while with "
"null true false undefined NaN Infinity "
,// Reserved Word
// ECMA-262
"class const enum export extends import super "
"implements interface let package private protected public static yield "
// Firefox
"each of get set "
, // preprocessor
""
, // directive
""
, // attribute
""
, // class
// ECMA-262
"Global Function Array String Boolean Number Date RegExp Error Math JSON Undefined Null "
"EvalError RangeError ReferenceError SyntaxError TypeError URIError ConversionError "
"Object ActiveXObject ArrayBuffer arguments DataView Debug Enumerator Float32Array Float64Array Int8Array Int16Array Int32Array Uint8Array Uint16Array Uint32Array VBArray WinRTError "
// JScript
"WScript "
// DOM
//
"jQuery "
, // interface
""
, // enumeration
""
, // constant
// Math
"E LN2 LN10 LOG2E LOG10E PI SQRT1_2 SQRT2 "
// Number
"MAX_VALUE MIN_VALUE NaN NEGATIVE_INFINITY POSITIVE_INFINITY "

#if NUMKEYWORD == 16
, // Functions
// Global
"decodeURI() decodeURIComponent() encodeURI() encodeURIComponent() escape() eval() GetObject() isFinite() isNaN() parseFloat() parseInt() ScriptEngine() ScriptEngineBuildVersion() ScriptEngineMajorVersion() ScriptEngineMinorVersion() nescape() "
// Math
"abs() acos() asin() atan() atan2() ceil() cos() exp() floor() log() max() min() pow() random() round() sin() sqrt() tan() "
// Object
"create() defineProperties() defineProperty() freeze() getOwnPropertyDescriptor() getOwnPropertyNames() getPrototypeOf() isExtensible() isFrozen() isSealed() keys() preventExtensions() seal() "
// Array, String, JSON
"isArray() " "fromCharCode() " "parse() stringify() "
// Debug, WScript
"write() writeln() echo() quit() "
// Date
"now() parse() UTC() "
, // Properties
// All
"constructor prototype "
// arguments, Function
"callee length "
// Array, ArrayBuffer
"length " "byteLength buffer byteOffset "
// RegExp
"index input lastIndex lastMatch lastParen leftContext rightContext " "global ignoreCase multiline source "
// Debug
"debuggerEnabled setNonUserCodeExceptions "
, // Methods
// All
"hasOwnProperty() isPrototypeOf() propertyIsEnumerable() toLocaleString() toString() valueOf() "
// Number
"toExponential() toFixed() toPrecision() "
// Function
"apply() bind() call() "
// Array
"concat() every() filter() forEach() indexOf() join() lastIndexOf() map() pop() push() reduce() reduceRight() reverse() shift() slice() some() sort() splice() unshift() "
// Enumerator
"atEnd() item() moveFirst() moveNext() "
"get() set() subarray() "
// VBArray
"dimensions() getItem() lbound() toArray()  ubound() "
// DataView
"getInt8() getUint8() getInt16() getUint16() getInt32() getUint32() getFloat32() getFloat64() setInt8() setUint8() setInt16() setUint16() setInt32() setUint32() setFloat32() setFloat64() "
// RegExp
"compile() exec() test() "
// String
"anchor() big() blink() bold() charAt() charCodeAt() concat() fixed() fontcolor() fontsize() indexOf() italics() lastIndexOf() link() localeCompare() match() replace() search() slice() small() split() strike() sub() substr() substring() sup() toLocaleLowerCase() toLocaleUpperCase() toLowerCase() toUpperCase() trim() "
// Date
"getDate() getDay() getFullYear() getHours() getMilliseconds() getMinutes() getMonth() getSeconds() getTime() getTimezoneOffset() getUTCDate() getUTCDay() getUTCFullYear() getUTCHours() getUTCMilliseconds() getUTCMinutes() getUTCMonth() getUTCSeconds() getVarDate() getYear() "
"setDate() setFullYear() setHours() setMilliseconds() setMinutes() setMonth() setSeconds() setTime() setUTCDate() setUTCFullYear() setUTCHours() setUTCMilliseconds() setUTCMinutes() setUTCMonth() setUTCSeconds() setYear() "
"toDateString() toGMTString() toISOString() toJSON() toLocaleDateString() toLocaleTimeString() toTimeString() toUTCString() "
,"","",""
,
"for^() if^() switch^() while^() else^if^() function() "
#endif
};

EDITLEXER lexJS = { SCLEX_CPP, NP2LEX_JS, L"JavaScript", L"js; jse; jsm; json", L"", &Keywords_JS,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_C_DEFAULT, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_ReservedWord, L"Reserved Word", L"fore:#B000B0", L"" },
	{ SCE_C_CLASS, NP2STYLE_Object, L"Object", L"bold; fore:#007F7F", L"" },
	{ SCE_C_FUNCTION, NP2STYLE_Function, L"Function", L"fore:#0080C0", L"" },
	{ SCE_C_CONSTANT, NP2STYLE_Constant, L"Constant", L"bold; fore:#B000B0", L""},
	{ MULTI_STYLE(SCE_C_COMMENT,SCE_C_COMMENTLINE,0,0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, L"Doc Comment Tag", L"bold; fore:#008000F", L"" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC,SCE_C_COMMENTLINEDOC,SCE_C_COMMENTDOC_TAG_XML,0), NP2STYLE_DocComment, L"Doc Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_C_STRING,SCE_C_CHARACTER,SCE_C_STRINGEOL,0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_REGEX, NP2STYLE_Regex, L"Regex", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_C_LABEL, NP2STYLE_Label, L"Label", L"fore:#000000; back:#FFC040", L""},
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
