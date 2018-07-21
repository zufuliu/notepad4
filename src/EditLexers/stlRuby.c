#include "EditLexer.h"
#include "EditStyle.h"

// https://www.ruby-lang.org/en/documentation/
// https://docs.ruby-lang.org/en/

static KEYWORDLIST Keywords_Ruby = {{
"__ENCODING__ __FILE__ __LINE__ __END__ BEGIN END "
"alias and begin break case class def defined? do else elsif end ensure false for if in module next nil "
"not or redo rescue retry return self super then true undef unless until when while yield "
// Pre-defined global constants
"ARGF ARGV ENV DATA FALSE NIL TRUE STDERR STDIN STDOUT RUBY_VERSION RUBY_RELEASE_DATE  RUBY_PLATFORM"
"require raise public private protected include extend "

, // Pre-defined variables
"$DEBUG $LOADED_FEATURES $FILENAME $LOAD_PATH $stderr $stdin $stdout $VERBOSE "

,"", "", "", "", "", ""

, // 8 Fold
"and begin break case do else elsif if next return when unless until not or"

, "", "", "", "", "", "", ""
}};

EDITLEXER lexRuby = { SCLEX_RUBY, NP2LEX_RUBY, L"Ruby Script", L"rb; ruby; rbw; rake; rjs", L"", &Keywords_Ruby,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_P_DEFAULT, L"Default", L"", L"" },
	{ SCE_RB_WORD, NP2STYLE_Keyword, L"Keyword", L"bold; fore:#FF8000", L"" },
	{ SCE_RB_MODULE_NAME, 63471, L"Module", L"bold; fore:#007F7F", L"" },
	{ SCE_RB_CLASSNAME, NP2STYLE_Class, L"Class", L"bold; fore:#007F7F", L"" },
	{ SCE_RB_DEFNAME, NP2STYLE_FunctionDef, L"Function Define", L"fore:#0080C0", L"" },
	{ SCE_RB_FUNCTION, NP2STYLE_Function, L"Function", L"fore:#A46000", L"" },
	{ SCE_RB_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_RB_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ SCE_RB_COMMENTLINE, NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_RB_STRING, SCE_RB_CHARACTER, 0, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_RB_POD, 63472, L"POD", L"fore:#004000; back:#C0FFC0; eolfilled", L"" },
	{ SCE_RB_BACKTICKS, NP2STYLE_BackTicks, L"Back Ticks", L"fore:#FF0080", L"" },
	{ SCE_RB_REGEX, NP2STYLE_Regex, L"Regex", L"fore:#000000; back:#A0FFA0", L"" },
	{ SCE_RB_SYMBOL, 63473, L"Symbol", L"bold; fore:#FF4F0F", L"" },
	{ MULTI_STYLE(SCE_RB_CLASS_VAR, SCE_RB_INSTANCE_VAR, SCE_RB_GLOBAL, 0), NP2STYLE_Variable, L"Variable", L"fore:#003CE6", L"" },
	{ SCE_RB_DATASECTION, 63475, L"Data Section", L"fore:#600000; back:#FFF0D8; eolfilled", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
