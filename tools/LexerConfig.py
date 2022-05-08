"""
Lexer Configuration Property

tab_settings: scheme default tab and indentation settings.
tab_width: tab width, default is 4.
indent_width: indentation width, default is 4.
tab_as_spaces: insert tab as spaces, default is False.
use_global_tab_settings: use global tab settings, default is True.

has_line_comment: has line comment, default is True.
has_block_comment: has block comment, default is True.

indent_based_folding: code folding is indentation based, default is False.
indent_guide_style: indentation guide view style, default is SC_IV_LOOKBOTH.
				for indentation based folding, SC_IV_LOOKFORWARD may looks better.
default_fold_level: level list for toggle default code folding.
				level name enclosed with `[]` will be ignored.
default_fold_ignore_inner: inner style to be ignored for toggle default code folding.
				normally this is function definition style.

printf_format_specifier: has C printf like format specifier, default is False.
format_specifier_style: style for format specifier, default is zero.
escape_char_start: character used to escape special characters, default is backslash.
escape_char_style: style for escape character, default is zero.
raw_string_style: styles where backslash is treated literally, default are zeros.

angle_bracket_generic: has C++ like template or generic with angle bracket, default is False.
operator_style:	styles for operator punctuation, default are zeros.
"""

from enum import IntFlag

class LexerAttr(IntFlag):
	Default = 0
	TabAsSpaces = 1 << 0			# tab_as_spaces
	NoGlobalTabSettings = 1 << 1	# use_global_tab_settings
	NoLineComment = 1 << 2			# has_line_comment
	NoBlockComment = 1 << 3			# has_block_comment
	IndentBasedFolding = 1 << 4		# indent_based_folding
	IndentLookForward = 1 << 5		# indent_guide_style
	PrintfFormatSpecifier = 1 << 6	# printf_format_specifier
	AngleBracketGeneric = 1 << 7	# angle_bracket_generic

class KeywordAttr(IntFlag):
	Default = 0
	MakeLower = 1	# need convert to lower case.
	PreSorted = 2	# word list is presorted.
	NoLexer = 4		# not used by Scintilla lexer, listed for auto completion.
	NoAutoComp = 8	# don't add to default auto-completion list.
	Special = 256	# used by context based auto-completion.

TabSettings_Default = {
	'tab_width': 4,
	'indent_width': 4,
	'tab_as_spaces': False,
	'use_global_tab_settings': True,
}
TabSettings_Tab4 = {
	'tab_width': 4,
	'indent_width': 4,
	'tab_as_spaces': False,
	'use_global_tab_settings': False,
}
TabSettings_Space4 = {
	'tab_width': 4,
	'indent_width': 4,
	'tab_as_spaces': True,
	'use_global_tab_settings': False,
}
TabSettings_Space2 = {
	'tab_width': 2,
	'indent_width': 2,
	'tab_as_spaces': True,
	'use_global_tab_settings': False,
}

DefaultFoldLevel = ['level1', 'level2']

LexerConfigMap = {
	'NP2LEX_TEXTFILE': {
		'has_line_comment': False,
		'has_block_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'escape_char_start': '\0',
	},
	'NP2LEX_2NDTEXTFILE': {
		'has_line_comment': False,
		'has_block_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'escape_char_start': '\0',
	},
	'NP2LEX_ANSI': {
		'has_line_comment': False,
		'has_block_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'escape_char_start': '\0',
	},

	'NP2LEX_ABAQUS': {
		'has_block_comment': False,
		'operator_style': ['SCE_APDL_OPERATOR'],
	},
	'NP2LEX_ACTIONSCRIPT': {
		'default_fold_level': ['class', 'anonymous object', 'method'],
		'default_fold_ignore_inner': 'SCE_JS_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_JS_ESCAPECHAR',
		'angle_bracket_generic': True,
		'operator_style': ['SCE_JS_OPERATOR', 'SCE_JS_OPERATOR2', 'SCE_JS_OPERATOR_PF'],
	},
	'NP2LEX_APDL': {
		'has_block_comment': False,
		'operator_style': ['SCE_APDL_OPERATOR'],
	},
	'NP2LEX_ASM': {
		'operator_style': ['SCE_ASM_OPERATOR'],
	},
	'NP2LEX_ASYMPTOTE': {
		'default_fold_level': ['struct', 'function'],
		'default_fold_ignore_inner': 'SCE_ASY_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_ASY_ESCAPECHAR',
		'operator_style': ['SCE_ASY_OPERATOR'],
	},
	'NP2LEX_AUTOHOTKEY': {
		'default_fold_level': ['class', 'function'],
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_AHK_FORMAT_SPECIFIER',
		'escape_char_start': '`',
		'escape_char_style': 'SCE_AHK_ESCAPECHAR',
		'operator_style': ['SCE_AHK_OPERATOR'],
	},
	'NP2LEX_AUTOIT3': {
		'operator_style': ['SCE_AU3_OPERATOR'],
	},
	'NP2LEX_AVISYNTH': {
		'default_fold_level': ['function'],
		'escape_char_style': 'SCE_AVS_ESCAPECHAR',
		'operator_style': ['SCE_AVS_OPERATOR'],
	},
	'NP2LEX_AWK': {
		'has_block_comment': False,
		'default_fold_level': ['namespace', 'function'],
		'default_fold_ignore_inner': 'SCE_AWK_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_AWK_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_AWK_ESCAPECHAR',
		'operator_style': ['SCE_AWK_OPERATOR'],
	},

	'NP2LEX_BASH': {
		'has_block_comment': False,
		'raw_string_style': ['SCE_SH_STRING_SQ'],
		'operator_style': ['SCE_SH_OPERATOR'],
	},
	'NP2LEX_BATCH': {
		'has_block_comment': False,
		'escape_char_start': '^',
		'escape_char_style': 'SCE_BAT_ESCAPECHAR',
		'operator_style': ['SCE_BAT_OPERATOR'],
	},
	'NP2LEX_BLOCKDIAG': {
		'default_fold_level': ['graph', 'subgraph'],
		'escape_char_style': 'SCE_GRAPHVIZ_ESCAPECHAR',
		'operator_style': ['SCE_GRAPHVIZ_OPERATOR'],
	},

	'NP2LEX_CIL': {
		'operator_style': ['SCE_C_OPERATOR'],
	},
	'NP2LEX_CMAKE': {
		'escape_char_style': 'SCE_CMAKE_ESCAPECHAR',
		'angle_bracket_generic': True, # for bracket argument $<>
		'operator_style': ['SCE_CMAKE_OPERATOR'],
	},
	'NP2LEX_COFFEESCRIPT': {
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['class', 'function'],
		'escape_char_style': 'SCE_COFFEESCRIPT_ESCAPECHAR',
		'operator_style': ['SCE_COFFEESCRIPT_OPERATOR', 'SCE_COFFEESCRIPT_OPERATOR2', 'SCE_COFFEESCRIPT_OPERATOR_PF'],
	},
	'NP2LEX_CONFIG': {
		'has_block_comment': False,
		'operator_style': ['SCE_CONF_OPERATOR'],
	},
	'NP2LEX_CPP': {
		'default_fold_level': ['preprocessor', 'namespace', 'class', 'method'],
		'printf_format_specifier': True,
		'raw_string_style': ['SCE_C_STRINGRAW'],
		'angle_bracket_generic': True,
		'operator_style': ['SCE_C_OPERATOR'],
	},
	'NP2LEX_CSHARP': {
		'default_fold_level': ['namespace', 'class', 'method'],
		'default_fold_ignore_inner': 'SCE_CSHARP_FUNCTION_DEFINITION',
		'format_specifier_style': 'SCE_CSHARP_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_CSHARP_ESCAPECHAR',
		'raw_string_style': ['SCE_CSHARP_VERBATIM_STRING', 'SCE_CSHARP_INTERPOLATED_VERBATIM_STRING',
			'SCE_CSHARP_RAWSTRING_SL', 'SCE_CSHARP_INTERPOLATED_RAWSTRING_SL',
			'SCE_CSHARP_RAWSTRING_ML', 'SCE_CSHARP_INTERPOLATED_RAWSTRING_ML',
		],
		'angle_bracket_generic': True,
		'operator_style': ['SCE_CSHARP_OPERATOR', 'SCE_CSHARP_OPERATOR2'],
	},
	'NP2LEX_CSS': {
		'escape_char_style': 'SCE_CSS_ESCAPECHAR',
		'operator_style': ['SCE_CSS_OPERATOR'],
	},

	'NP2LEX_DLANG': {
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_D_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_D_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_D_ESCAPECHAR',
		'raw_string_style': ['SCE_D_RAWSTRING', 'SCE_D_STRING_BT'],
		'operator_style': ['SCE_D_OPERATOR'],
	},
	'NP2LEX_DART': {
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_DART_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_DART_ESCAPECHAR',
		'raw_string_style': ['SCE_DART_RAWSTRING_SQ', 'SCE_DART_RAWSTRING_DQ',
			'SCE_DART_TRIPLE_RAWSTRING_SQ', 'SCE_DART_TRIPLE_RAWSTRING_DQ',
		],
		'angle_bracket_generic': True,
		'operator_style': ['SCE_DART_OPERATOR', 'SCE_DART_OPERATOR2'],
	},
	'NP2LEX_DIFF': {
		'has_line_comment': False,
		'has_block_comment': False,
		'default_fold_level': ['command', '[file]', 'diff'],
		'escape_char_start': '\0',
	},

	'NP2LEX_FORTRAN': {
		'operator_style': ['SCE_F_OPERATOR', 'SCE_F_OPERATOR2'],
	},
	'NP2LEX_FSHARP': {
		'operator_style': ['SCE_FSHARP_OPERATOR'],
	},

	'NP2LEX_GN': {
		'has_block_comment': False,
		'escape_char_style': 'SCE_GN_ESCAPECHAR',
		'operator_style': ['SCE_GN_OPERATOR', 'SCE_GN_OPERATOR2'],
	},
	'NP2LEX_GO': {
		'default_fold_level': ['struct', 'function'],
		'default_fold_ignore_inner': 'SCE_GO_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_GO_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_GO_ESCAPECHAR',
		'raw_string_style': ['SCE_GO_RAW_STRING'],
		'operator_style': ['SCE_GO_OPERATOR', 'SCE_GO_OPERATOR2'],
	},
	'NP2LEX_GRADLE': {
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_GROOVY_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_GROOVY_ESCAPECHAR',
		'angle_bracket_generic': True,
		'operator_style': ['SCE_GROOVY_OPERATOR', 'SCE_GROOVY_OPERATOR2', 'SCE_GROOVY_OPERATOR_PF'],
	},
	'NP2LEX_GRAPHVIZ': {
		'default_fold_level': ['graph', 'subgraph'],
		'escape_char_style': 'SCE_GRAPHVIZ_ESCAPECHAR',
		'operator_style': ['SCE_GRAPHVIZ_OPERATOR'],
	},
	'NP2LEX_GROOVY': {
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_GROOVY_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_GROOVY_ESCAPECHAR',
		'angle_bracket_generic': True,
		'operator_style': ['SCE_GROOVY_OPERATOR', 'SCE_GROOVY_OPERATOR2', 'SCE_GROOVY_OPERATOR_PF'],
	},

	'NP2LEX_HAXE': {
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_HAXE_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_HAXE_ESCAPECHAR',
		'angle_bracket_generic': True,
		'operator_style': ['SCE_HAXE_OPERATOR', 'SCE_HAXE_OPERATOR2'],
	},
	'NP2LEX_HTML': {
		'tab_settings': TabSettings_Space2,
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
		#'escape_char_start': '\0', # backslash for embedded script or style
	},

	'NP2LEX_INI': {
		'has_block_comment': False,
		'default_fold_level': ['section', 'comment'],
		'escape_char_start': '\0',
	},
	'NP2LEX_INNOSETUP': {
		'default_fold_level': ['section', 'code'],
		'escape_char_start': '\0',
		'operator_style': ['SCE_INNO_OPERATOR'],
	},

	'NP2LEX_JAMFILE': {
		'escape_char_style': 'SCE_JAM_ESCAPECHAR',
		'operator_style': ['SCE_JAM_OPERATOR'],
	},
	'NP2LEX_JAVA': {
		'default_fold_level': ['class', 'inner class', 'method'],
		'default_fold_ignore_inner': 'SCE_JAVA_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_JAVA_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_JAVA_ESCAPECHAR',
		'angle_bracket_generic': True,
		'operator_style': ['SCE_JAVA_OPERATOR'],
	},
	'NP2LEX_JAVASCRIPT': {
		'default_fold_level': ['class', 'anonymous object', 'method'],
		'default_fold_ignore_inner': 'SCE_JS_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_JS_ESCAPECHAR',
		'operator_style': ['SCE_JS_OPERATOR', 'SCE_JS_OPERATOR2', 'SCE_JS_OPERATOR_PF'],
	},
	'NP2LEX_JSON': {
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
		'escape_char_style': 'SCE_JSON_ESCAPECHAR',
		'operator_style': ['SCE_JSON_OPERATOR'],
	},
	'NP2LEX_JULIA': {
		'default_fold_level': ['struct', 'method'],
		'default_fold_ignore_inner': 'SCE_JULIA_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_JULIA_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_JULIA_ESCAPECHAR',
		'raw_string_style': ['SCE_JULIA_RAWSTRING', 'SCE_JULIA_TRIPLE_RAWSTRING'],
		'operator_style': ['SCE_JULIA_OPERATOR', 'SCE_JULIA_OPERATOR2'],
	},

	'NP2LEX_KOTLIN': {
		'default_fold_level': ['class', 'inner class', 'method'],
		'default_fold_ignore_inner': 'SCE_KOTLIN_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_KOTLIN_ESCAPECHAR',
		'raw_string_style': ['SCE_KOTLIN_RAWSTRING'],
		'angle_bracket_generic': True,
		'operator_style': ['SCE_KOTLIN_OPERATOR', 'SCE_KOTLIN_OPERATOR2'],
	},

	'NP2LEX_LATEX': {
		'escape_char_start': '^',
		'escape_char_style': 'SCE_L_SPECIAL',
		'operator_style': ['SCE_L_OPERATOR'],
	},
	'NP2LEX_LISP': {
		'operator_style': ['SCE_C_OPERATOR'],
	},
	'NP2LEX_LLVM': {
		'has_block_comment': False,
		'escape_char_style': 'SCE_LLVM_ESCAPECHAR',
		'operator_style': ['SCE_LLVM_OPERATOR'],
	},
	'NP2LEX_LUA': {
		'default_fold_level': ['class', 'function'],
		'printf_format_specifier': True,
		'operator_style': ['SCE_LUA_OPERATOR'],
	},

	'NP2LEX_MAKEFILE': {
		'tab_settings': TabSettings_Tab4,
		'has_block_comment': False,
		'escape_char_start': '\0',
		'operator_style': ['SCE_MAKE_OPERATOR'],
	},
	'NP2LEX_MARKDOWN': {
		'has_line_comment': False,
		'default_fold_level': ['header1', 'header2'],
		'escape_char_style': 'SCE_MARKDOWN_ESCAPECHAR',
	},
	'NP2LEX_MATLAB': {
		'printf_format_specifier': True,
		#'escape_char_start': '\0', # backslash for Octave escape character
		'operator_style': ['SCE_MAT_OPERATOR'],
	},

	'NP2LEX_NSIS': {
		'default_fold_level': ['section', 'function'],
		'escape_char_style': 'SCE_NSIS_ESCAPECHAR',
		'operator_style': ['SCE_NSIS_OPERATOR'],
	},

	'NP2LEX_PASCAL': {
		'operator_style': ['SCE_PAS_OPERATOR'],
	},
	'NP2LEX_PERL': {
		'has_block_comment': False,
		'printf_format_specifier': True,
		'raw_string_style': ['SCE_PL_STRING_SQ'],
		'operator_style': ['SCE_PL_OPERATOR'],
	},
	'NP2LEX_PHP': {
		'default_fold_level': ['[php tag]', 'class', 'method'],
		'default_fold_ignore_inner': 'SCE_PHP_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_PHP_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_PHP_ESCAPECHAR',
		'raw_string_style': ['SCE_PHP_STRING_SQ', 'SCE_PHP_NOWDOC'],
		'operator_style': ['SCE_PHP_OPERATOR', 'SCE_PHP_OPERATOR2'],
	},
	'NP2LEX_POWERSHELL': {
		'escape_char_start': '`',
		'raw_string_style': ['SCE_POWERSHELL_STRING_SQ'],
		'operator_style': ['SCE_POWERSHELL_OPERATOR'],
	},
	'NP2LEX_PYTHON': {
		'has_block_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_PY_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_PY_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_PY_ESCAPECHAR',
		'raw_string_style': ['SCE_PY_RAWSTRING_SQ', 'SCE_PY_RAWSTRING_DQ',
			'SCE_PY_TRIPLE_RAWSTRING_SQ', 'SCE_PY_TRIPLE_RAWSTRING_DQ',
			'SCE_PY_RAWFMTSTRING_SQ', 'SCE_PY_RAWFMTSTRING_DQ',
			'SCE_PY_TRIPLE_RAWFMTSTRING_SQ', 'SCE_PY_TRIPLE_RAWFMTSTRING_DQ',
			'SCE_PY_RAWBYTES_SQ', 'SCE_PY_RAWBYTES_DQ',
			'SCE_PY_TRIPLE_RAWBYTES_SQ', 'SCE_PY_TRIPLE_RAWBYTES_DQ',
		],
		'operator_style': ['SCE_PY_OPERATOR', 'SCE_PY_OPERATOR2'],
	},

	'NP2LEX_RLANG': {
		'default_fold_level': ['function'],
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_R_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_R_ESCAPECHAR',
		'operator_style': ['SCE_R_OPERATOR', 'SCE_R_INFIX'],
	},
	'NP2LEX_REBOL': {
		'escape_char_start': '^',
		'escape_char_style': 'SCE_REBOL_ESCAPECHAR',
		'operator_style': ['SCE_REBOL_OPERATOR'],
	},
	'NP2LEX_RESOURCESCRIPT': {
		'default_fold_level': ['preprocessor', 'resource'],
		'printf_format_specifier': True,
		'operator_style': ['SCE_C_OPERATOR'],
	},
	'NP2LEX_RUBY': {
		'has_block_comment': False,
		'default_fold_level': ['module', 'class', 'method'],
		'default_fold_ignore_inner': 'SCE_RB_DEF_NAME',
		'printf_format_specifier': True,
		'raw_string_style': ['SCE_RB_STRING_SQ'],
		'operator_style': ['SCE_RB_OPERATOR'],
	},
	'NP2LEX_RUST': {
		'default_fold_level': ['struct', 'function'],
		'default_fold_ignore_inner': 'SCE_RUST_FUNCTION_DEFINITION',
		'format_specifier_style': 'SCE_RUST_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_RUST_ESCAPECHAR',
		'raw_string_style': ['SCE_RUST_RAW_STRING', 'SCE_RUST_RAW_BYTESTRING'],
		'operator_style': ['SCE_RUST_OPERATOR'],
	},

	'NP2LEX_SCALA': {
		'default_fold_level': ['class', 'inner class', 'method'],
		'operator_style': ['SCE_C_OPERATOR'],
	},
	'NP2LEX_SMALI': {
		'has_block_comment': False,
		'default_fold_level': ['.method', '.switch'],
		'operator_style': ['SCE_SMALI_OPERATOR'],
	},
	'NP2LEX_SQL': {
		'default_fold_level': ['function'],
		'escape_char_style': 'SCE_SQL_ESCAPECHAR',
		'operator_style': ['SCE_SQL_OPERATOR'], # ignore q'{SCE_SQL_QOPERATOR}'
	},
	'NP2LEX_SWIFT': {
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_SWIFT_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_SWIFT_ESCAPECHAR',
		'angle_bracket_generic': True,
		'operator_style': ['SCE_SWIFT_OPERATOR', 'SCE_SWIFT_OPERATOR2'],
	},

	'NP2LEX_TCL': {
		'printf_format_specifier': True,
		'operator_style': ['SCE_TCL_MODIFIER'],
	},
	'NP2LEX_TEXINFO': {
		'has_block_comment': False,
		'escape_char_start': '@',
		'escape_char_style': 'SCE_L_SPECIAL',
		'operator_style': ['SCE_L_OPERATOR'],
	},
	'NP2LEX_TOML': {
		'has_block_comment': False,
		'default_fold_level': ['table', 'comment'],
		'escape_char_style': 'SCE_TOML_ESCAPECHAR',
		'operator_style': ['SCE_TOML_OPERATOR'],
	},
	'NP2LEX_TYPESCRIPT': {
		'default_fold_level': ['class', 'anonymous object', 'method'],
		'default_fold_ignore_inner': 'SCE_JS_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_JS_ESCAPECHAR',
		'angle_bracket_generic': True,
		'operator_style': ['SCE_JS_OPERATOR', 'SCE_JS_OPERATOR2', 'SCE_JS_OPERATOR_PF'],
	},

	'NP2LEX_VBSCRIPT': {
		'has_block_comment': False,
		'default_fold_level': ['function'],
		'escape_char_start': '\0',
		'operator_style': ['SCE_B_OPERATOR'],
	},
	'NP2LEX_VERILOG': {
		'operator_style': ['SCE_V_OPERATOR'],
	},
	'NP2LEX_VHDL': {
		'operator_style': ['SCE_VHDL_OPERATOR'],
	},
	'NP2LEX_VIM': {
		'has_block_comment': False,
		'escape_char_style': 'SCE_YAML_ESCAPECHAR',
		'raw_string_style': ['SCE_VIM_STRING_SQ'],
		'operator_style': ['SCE_VIM_OPERATOR'],
	},
	'NP2LEX_VISUALBASIC': {
		'has_block_comment': False,
		'default_fold_level': ['class', 'function'],
		'escape_char_start': '\0',
		'operator_style': ['SCE_B_OPERATOR'],
	},

	'NP2LEX_WASM': {
		'escape_char_style': 'SCE_WASM_ESCAPECHAR',
		'operator_style': ['SCE_WASM_OPERATOR'],
	},
	'NP2LEX_XML': {
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
		'escape_char_start': '\0',
	},
	'NP2LEX_YAML': {
		'tab_settings': TabSettings_Space2,
		'has_block_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
		'escape_char_style': 'SCE_YAML_ESCAPECHAR',
		'operator_style': ['SCE_YAML_OPERATOR'],
	},
}

def get_enum_flag_expr(flag, merge=True, separator='_'):
	cls = flag.__class__
	prefix = cls.__name__ + separator
	if flag.name:
		return prefix + flag.name

	result = []
	values = cls.__members__.values()
	for value in values:
		if flag & value:
			result.append(prefix + value.name)
	if merge:
		return ' | '.join(result)
	return result

def dump_enum_flag(cls, indent='', anonymous=True, as_shift=False, max_value=None, separator='_'):
	prefix = cls.__name__ + separator
	values = cls.__members__.values()
	name = '' if anonymous else cls.__name__ + ' '
	output = [f'{indent}enum {name}{{']
	for flag in values:
		value = int(flag)
		if not max_value or value < max_value:
			if value and as_shift:
				assert value.bit_count() == 1, flag
				value = f'1 << {value.bit_length() - 1}'
			output.append(f'{indent}\t{prefix}{flag.name} = {value},')
	output.append(f'{indent}}};')
	return output

CEscapeMap = {
	'\0': r'\0',
	'\a': r'\a',
	'\b': r'\b',
	'\n': r'\n',
	'\r': r'\r',
	'\f': r'\f',
	'\v': r'\v',
	'\\': r'\\',
	'\'': r'\'',
	'\"': r'\"',
}

def escape_c_char(ch):
	if ch in CEscapeMap:
		return CEscapeMap[ch]
	if ch < ' ' or ord(ch) >= 127:
		return f'\\x{ord(ch):02x}'
	return ch

def quote_c_char(ch):
	return "'" + escape_c_char(ch) + "'"

def BuildLexerConfigContent(rid, keywordAttr):
	config = LexerConfigMap.get(rid, {})
	if rid and not (config or keywordAttr):
		return ['\tSCHEME_SETTINGS_DEFAULT,']

	tab_settings = config.get('tab_settings', TabSettings_Default)
	# lexer attribute
	flag = LexerAttr.Default
	if tab_settings['tab_as_spaces']:
		flag |= LexerAttr.TabAsSpaces
	if not tab_settings['use_global_tab_settings']:
		flag |= LexerAttr.NoGlobalTabSettings

	if not config.get('has_line_comment', True):
		flag |= LexerAttr.NoLineComment
	if not config.get('has_block_comment', True):
		flag |= LexerAttr.NoBlockComment

	indent_based_folding = config.get('indent_based_folding', False)
	if indent_based_folding:
		flag |= LexerAttr.IndentBasedFolding
	style = config.get('indent_guide_style', '')
	if style == 'forward':
		flag |= LexerAttr.IndentLookForward

	if config.get('printf_format_specifier', False):
		flag |= LexerAttr.PrintfFormatSpecifier
	if config.get('angle_bracket_generic', False):
		flag |= LexerAttr.AngleBracketGeneric

	output = ['\t{']
	indent = '\t\t'
	expr = get_enum_flag_expr(flag, merge=False)
	if isinstance(expr, str):
		output.append(f'{indent}{expr},')
	else:
		output.extend(f'{indent}{item} |' for item in expr)
		output[-1] = output[-1][:-2] + ','

	# tab width, indent width
	output.append(f"{indent}TAB_WIDTH_{tab_settings['tab_width']}, INDENT_WIDTH_{tab_settings['indent_width']},")

	# default fold level, default fold ignore inner
	foldLevel = config.get('default_fold_level', DefaultFoldLevel)
	levelList = []
	for index, comment in enumerate(foldLevel):
		if comment[0] != '[': # omit this level
			levelList.append(f'(1 << {index + indent_based_folding})')
	expr = ' | '.join(levelList)
	comment = ', '.join(foldLevel)
	if rid:
		output.append(f'{indent}{expr}, // {comment}')
	else:
		output.append(f'{indent}{expr}, /* {comment} */')
	output.append(f"{indent}{config.get('default_fold_ignore_inner', '0')},")

	# format specifier, escape character
	start = config.get('escape_char_start', '\\')
	style = config.get('escape_char_style', '0')
	assert len(start) == 1, (rid, style, start)
	output.append(f"{indent}{config.get('format_specifier_style', '0')}, '{escape_c_char(start)}', {style},")
	# raw string styles
	styles = config.get('raw_string_style', ['0'])
	output.append(f"{indent}{styles[0]},")
	# operator styles
	styles = config.get('operator_style', []) + ['0', '0']
	output.append(f"{indent}{styles[0]}, {styles[1]},")

	# keyword attribute
	if keywordAttr:
		output[-1] = output[-1][:-1] # remove extra comma
		prefix = ','
		for index, attr, comment in keywordAttr:
			expr = get_enum_flag_expr(attr)
			bit = 64 if index >= 7 else 32
			output.append(f'{indent}{prefix} KeywordAttr{bit}({index}, {expr}) // {comment}')
			prefix = '|'
	else:
		expr = get_enum_flag_expr(KeywordAttr.Default)
		output.append(indent + expr)

	suffix = '\t},'
	if not rid:
		# right align continuation backslash in multi-line macro
		indent_count = len(indent)
		max_width = max(len(line) for line in output) - indent_count
		max_width = (max_width + 8) & ~3
		result = ['#define SCHEME_SETTINGS_DEFAULT\t\\']
		for index, line in enumerate(output):
			width = len(line) - indent_count
			width = (width + 4) & ~3
			padding = (max_width - width) // 4
			padding += index == 0
			result.append(line + '\t'*padding + '\\')
		output = result
		suffix = suffix[:-1]
	output.append(suffix)
	return output
