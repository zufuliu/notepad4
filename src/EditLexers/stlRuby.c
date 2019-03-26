#include "EditLexer.h"
#include "EditStyle.h"

// https://www.ruby-lang.org/en/documentation/
// https://docs.ruby-lang.org/en/

static KEYWORDLIST Keywords_Ruby = {{
"__ENCODING__ __FILE__ __LINE__ __END__ BEGIN END "
"alias and begin break case class def defined? do else elsif end ensure false for if in module next nil "
"not or redo rescue retry return self super then true undef unless until when while yield "
// Pre-defined global constants
"ARGF ARGV ENV DATA FALSE NIL TRUE STDERR STDIN STDOUT RUBY_VERSION RUBY_RELEASE_DATE RUBY_PLATFORM"
"require raise public private protected include extend "

, // Pre-defined variables
"$DEBUG $LOADED_FEATURES $FILENAME $LOAD_PATH $stderr $stdin $stdout $VERBOSE "

, NULL, NULL, NULL, NULL, NULL, NULL

, // 8 Fold
"and begin break case do else elsif if next return when unless until not or"

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Ruby[] = {
	EDITSTYLE_DEFAULT,
	{ MULTI_STYLE(SCE_RB_WORD, SCE_RB_WORD_DEMOTED, 0, 0), NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"bold; fore:#FF8000" },
	{ SCE_RB_MODULE_NAME, 63471, EDITSTYLE_HOLE(L"Module"), L"bold; fore:#007F7F" },
	{ SCE_RB_CLASSNAME, NP2STYLE_Class, EDITSTYLE_HOLE(L"Class"), L"bold; fore:#007F7F" },
	{ SCE_RB_DEFNAME, NP2STYLE_FunctionDef, EDITSTYLE_HOLE(L"Function Define"), L"fore:#0080C0" },
	{ SCE_RB_FUNCTION, NP2STYLE_Function, EDITSTYLE_HOLE(L"Function"), L"fore:#A46000" },
	{ SCE_RB_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_RB_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ SCE_RB_COMMENTLINE, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ MULTI_STYLE(SCE_RB_STRING, SCE_RB_CHARACTER, 0, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_RB_POD, 63472, EDITSTYLE_HOLE(L"POD"), L"fore:#004000; back:#C0FFC0; eolfilled" },
	{ SCE_RB_BACKTICKS, NP2STYLE_Backticks, EDITSTYLE_HOLE(L"Backticks"), L"fore:#FF0080" },
	{ SCE_RB_REGEX, NP2STYLE_Regex, EDITSTYLE_HOLE(L"Regex"), L"back:#A0FFA0" },
	{ SCE_RB_SYMBOL, 63473, EDITSTYLE_HOLE(L"Symbol"), L"bold; fore:#FF4F0F" },
	{ MULTI_STYLE(SCE_RB_CLASS_VAR, SCE_RB_INSTANCE_VAR, SCE_RB_GLOBAL, 0), NP2STYLE_Variable, EDITSTYLE_HOLE(L"Variable"), L"fore:#003CE6" },
	{ SCE_RB_HERE_DELIM, 63408, EDITSTYLE_HOLE(L"Here-doc (Delimiter)"), L"fore:#648000" },
	{ SCE_RB_HERE_Q, 63409, EDITSTYLE_HOLE(L"Here-doc (Single Quoted, q)"), L"fore:#648000" },
	{ SCE_RB_HERE_QQ, 63410, EDITSTYLE_HOLE(L"Here-doc (Double Quoted, qq)"), L"fore:#648000" },
	{ SCE_RB_HERE_QX, 63411, EDITSTYLE_HOLE(L"Here-doc (Backticks, qx)"), L"fore:#E24000; back:#FFF1A8" },
	{ SCE_RB_STRING_Q, 63412, EDITSTYLE_HOLE(L"Single Quoted String (Generic, q)"), L"fore:#008000" },
	{ SCE_RB_STRING_QQ, 63413, EDITSTYLE_HOLE(L"Double Quoted String (qq)"), L"fore:#008000" },
	{ SCE_RB_STRING_QX, 63414, EDITSTYLE_HOLE(L"Backticks (qx)"), L"fore:#E24000; back:#FFF1A8" },
	{ SCE_RB_STRING_QR, 63415, EDITSTYLE_HOLE(L"Regex (qr)"), L"fore:#006633; back:#FFF1A8" },
	{ SCE_RB_STRING_QW, 63416, EDITSTYLE_HOLE(L"Array (qw)"), L"fore:#003CE6" },
	{ SCE_RB_DATASECTION, 63475, EDITSTYLE_HOLE(L"Data Section"), L"fore:#600000; back:#FFF0D8; eolfilled" },
};

EDITLEXER lexRuby = {
	SCLEX_RUBY, NP2LEX_RUBY,
	EDITLEXER_HOLE(L"Ruby Script", Styles_Ruby),
	L"rb; ruby; rbw; rake; rjs; gemspec; podspec",
	&Keywords_Ruby,
	Styles_Ruby
};

