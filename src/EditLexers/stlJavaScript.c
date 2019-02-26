#include "EditLexer.h"
#include "EditStyle.h"

// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Lexical_grammar
// https://www.ecma-international.org/publications/standards/Ecma-262.htm
static KEYWORDLIST Keywords_JS = {{
// ECMA-262
"async await break case catch class const continue debugger default delete do else extends finally for function if in "
"instanceof let new return static super switch this throw try typeof var void while with yield "
"null true false undefined NaN Infinity "

, // 1 Reserved Word
// ECMA-262
"enum implements interface package private protected public "
// ECMAScript 1 till 3
//"abstract boolean byte char double final float goto int long native short synchronized throws transient volatile "
// Firefox
"each of get set "

, // 2 Preprocessor
NULL
, // 3 Directive
NULL
, // 4 Attribute
// Scripts and Modules
"export import from as "
// Node.js
"module exports require __dirname __filename "

, // 5 Class
// ECMA-262
"Global Object Function Boolean Symbol Array String Number Date RegExp Math JSON Null Atomics "
"Error EvalError RangeError ReferenceError SyntaxError TypeError URIError NativeError "
"Float32Array Float64Array Int8Array Int16Array Int32Array Uint8Array Uint8ClampedArray Uint16Array Uint32Array "
"Map Set WeakMap WeakSet ArrayBuffer SharedArrayBuffer DataView Promise Reflect Proxy "
// JScript
"arguments ActiveXObject ConversionError Debug Enumerator VBArray WScript WinRTError "
// HTML DOM
//
"XMLHttpRequest FormData jQuery "

, // 6 Interface
NULL
, // 7 Enumeration
NULL

, // 8 Constant
// Math
"E LN2 LN10 LOG2E LOG10E PI SQRT1_2 SQRT2 "
// Number
"EPSILON MAX_SAFE_INTEGER MIN_SAFE_INTEGER MAX_VALUE MIN_VALUE NaN NEGATIVE_INFINITY POSITIVE_INFINITY "
// TypedArray
"BYTES_PER_ELEMENT "
// XMLHttpRequest
"UNSENT OPENED HEADERS_RECEIVED LOADING DONE "

, // 9 Function
// Global
"decodeURI() decodeURIComponent() encodeURI() encodeURIComponent() escape() eval() GetObject() isFinite() isNaN() parseFloat() parseInt() ScriptEngine() ScriptEngineBuildVersion() ScriptEngineMajorVersion() ScriptEngineMinorVersion() nescape() "
// Math
"abs() acos() acosh() asin() asinh() atan() atanh() atan2() cbrt() ceil() clz32() cos() cosh() exp() expm1() floor() fround() hypot() imul() log() log1p() log10() log2() max() min() pow() random() round() sin() sinh() sqrt() tan() tanh() trunc() "
// Object
"assign() create() defineProperties() defineProperty() entries() freeze() getOwnPropertyDescriptor() getOwnPropertyDescriptors() getOwnPropertyNames() getOwnPropertySymbols() getPrototypeOf() is() isExtensible() isFrozen() isSealed() keys() preventExtensions() seal() setPrototypeOf() values() "
// Number
"isInteger() isSafeInteger() "
// Symbol
"keyFor() "
// Array, ArrayBuffer, String, JSON
"of() from() isArray() " "isView() " "fromCharCode() " "parse() stringify() "
// Debug, WScript
"write() writeln() echo() quit() "
// Date
"now() parse() UTC() "

// HTML DOM
// WindowOrWorkerGlobalScope
"atob() btoa() clearInterval() clearTimeout() createImageBitmap() fetch() setInterval() setTimeout() "

, // 10 Property
"use strict "
// All
"constructor prototype __proto__ "
// arguments, Function, Error
"callee length name message "
// Symbol
"asyncIterator hasInstance isConcatSpreadable iterator match replace search species split toPrimitive toStringTag unscopables prototype "
// Array, ArrayBuffer, DataView, Map
"length " "byteLength buffer byteOffset " "size "
// RegExp
"index input lastIndex lastMatch lastParen leftContext rightContext " "dotAll global ignoreCase multiline source unicode "
// Debug
"debuggerEnabled setNonUserCodeExceptions "

// HTML DOM
"document window body "
// XMLHttpRequest
"onreadystatechange readyState response responseText responseType responseURL responseXML status statusText timeout upload withCredentials "
// Storage
"localStorage sessionStorage "

, // 11 Method
// All
"hasOwnProperty() isPrototypeOf() propertyIsEnumerable() toLocaleString() toString() valueOf() "
"__defineGetter__() __defineSetter__() __lookupGetter__() __lookupSetter__() "
// Number
"toExponential() toFixed() toPrecision() "
// Function
"apply() bind() call() "
// Array
"copyWithin() every() filter() find() findIndex() forEach() join() map() pop() push() reduce() reduceRight() reverse() shift() some() sort() splice() unshift() "
// Enumerator
"atEnd() item() moveFirst() moveNext() "
"get() set() subarray() "
// VBArray
"dimensions() getItem() lbound() toArray() ubound() "
// DataView
"getInt8() getUint8() getInt16() getUint16() getInt32() getUint32() getFloat32() getFloat64() setInt8() setUint8() setInt16() setUint16() setInt32() setUint32() setFloat32() setFloat64() "
// RegExp
"compile() exec() test() "
// String
"charAt() charCodeAt() codePointAt() concat() endsWith() includes() indexOf() lastIndexOf() localeCompare() match() normalize() padEnd() padStart() repeat() replace() search() slice() split() startsWith() strike() substring() toLocaleLowerCase() toLocaleUpperCase() toLowerCase() toUpperCase() trim() "
"substr() anchor() big() blink() bold() fixed() fontcolor() fontsize() italics() link() small() sub() sup() "
// Date
"getDate() getDay() getFullYear() getHours() getMilliseconds() getMinutes() getMonth() getSeconds() getTime() getTimezoneOffset() getUTCDate() getUTCDay() getUTCFullYear() getUTCHours() getUTCMilliseconds() getUTCMinutes() getUTCMonth() getUTCSeconds() getVarDate() getYear() "
"setDate() setFullYear() setHours() setMilliseconds() setMinutes() setMonth() setSeconds() setTime() setUTCDate() setUTCFullYear() setUTCHours() setUTCMilliseconds() setUTCMinutes() setUTCMonth() setUTCSeconds() setYear() "
"toDateString() toGMTString() toISOString() toJSON() toLocaleDateString() toLocaleTimeString() toTimeString() toUTCString() "
// Map, Set
"clear() delete() get() has() set() add() "
// Generator, Promise
"next() all() race() reject() resolve() "
// Reflect, Proxy
"construct() defineProperty() deleteProperty() ownKeys() " "revocable() "

// HTML DOM
"getElementById() getElementsByClassName() getElementsByName() getElementsByTagName() "
// XMLHttpRequest
"abort() getAllResponseHeaders() getResponseHeader() open() overrideMimeType() send() setRequestHeader() "
// FormData
"append() getAll() "
// Storage
"getItem() key() removeItem() setItem() "
, NULL, NULL, NULL

, // 15 Code Snippet
"for^() if^() switch^() while^() else^if^() else^{} function() require() "
}};

static EDITSTYLE Styles_JS[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2STYLE_ReservedWord, EDITSTYLE_HOLE(L"Reserved Word"), L"fore:#B000B0" },
	{ SCE_C_ATTRIBUTE, NP2STYLE_Directive, EDITSTYLE_HOLE(L"Module Directive"), L"fore:#FF8000" },
	{ SCE_C_CLASS, NP2STYLE_Object, EDITSTYLE_HOLE(L"Object"), L"bold; fore:#0080C0" },
	{ SCE_C_FUNCTION, NP2STYLE_Function, EDITSTYLE_HOLE(L"Function"), L"fore:#A46000" },
	{ SCE_C_CONSTANT, NP2STYLE_Constant, EDITSTYLE_HOLE(L"Constant"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, EDITSTYLE_HOLE(L"Doc Comment Tag"), L"fore:#408080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2STYLE_DocComment, EDITSTYLE_HOLE(L"Doc Comment"), L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_DSTRINGB, NP2STYLE_Backticks, EDITSTYLE_HOLE(L"Template Literal"), L"fore:#F08000" },
	{ SCE_C_REGEX, NP2STYLE_Regex, EDITSTYLE_HOLE(L"Regex"), L"fore:#006633; back:#FFF1A8" },
	{ SCE_C_LABEL, NP2STYLE_Label, EDITSTYLE_HOLE(L"Label"), L"fore:#000000; back:#FFC040" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
};

EDITLEXER lexJS = {
	SCLEX_CPP, NP2LEX_JS,
	EDITLEXER_HOLE(L"JavaScript", Styles_JS),
	L"js; jse; jsm; mjs; qs",
	&Keywords_JS,
	Styles_JS
};

