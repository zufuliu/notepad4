"""
Lexer Configuration Property

tab_settings: scheme default tab and indentation settings.
	tab_width: tab width, default is 4.
	indent_width: indentation width, default is 4.
	tab_as_spaces: insert tab as spaces, default is False.
	use_global_tab_settings: use global tab settings, default is True.
default_encoding: scheme default encoding.
default_line_ending: scheme default line ending.

line_comment_string: line comment start string.
line_comment_at_line_start: put start string at line start, default is False.
block_comment_string: block comment start and end string.
block_comment_on_new_line: put start and end string on new line, default is False.
cpp_style_comment: shortcut to use C++ style line and block comment, default is False.
comment_style_list: comment style list.
comment_style_marker: comment style only inside [1, marker].
shebang_exe_name: executable name for shebang.

indent_based_folding: code folding is indentation based, default is False.
indent_guide_style: indentation guide view style, default is SC_IV_LOOKBOTH.
				for indentation based folding, SC_IV_LOOKFORWARD may looks better.
default_fold_level: level list for toggle default code folding.
				level name enclosed with `[]` will be ignored.
default_fold_ignore_inner: inner style to be ignored for toggle default code folding.
				normally this is function definition style.

printf_format_specifier: has C printf like format specifier, default is False.
format_specifier_style: style for format specifier.
escape_char_start: character used to escape special characters, default is backslash.
escape_char_style: style for escape character.
escape_punctuation: only escape with punctuation.
raw_string_style: styles where backslash is treated literally.
character_style: styles for character literal.
character_prefix: prefix for character literal or single quoted string.
none_quote_style: style for single quote not used for quotation.

angle_bracket_generic: has C++ like template or generic with angle bracket, default is False.
generic_type_style: styles can have generic type argument.
operator_style:	styles for operator punctuation.
cpp_preprocessor: has C++ style #preprocessor, default is False.

extra_word_char: extra identifier characters excludes dot.
ignore_word_style: word styles to be ignored for auto-completion word scanning.
autoc_extra_keyword: [deprecated] extra keyword for auto-completion.
auto_ident_word_style: word styles that supports auto ident.
plain_text_file: treat as plain text file for auto-completion.
plain_text_style: plain text styles for auto-completion.
string_style_range: first and last string or regex style for auto-completion.
string_style_list: string or regex styles for auto-completion.
"""

from enum import IntFlag

class LexerAttr(IntFlag):
	Default = 0
	TabAsSpaces = 1 << 0			# tab_as_spaces
	NoGlobalTabSettings = 1 << 1	# use_global_tab_settings
	NoLineComment = 1 << 2			# line_comment_string
	NoBlockComment = 1 << 3			# block_comment_string
	IndentBasedFolding = 1 << 4		# indent_based_folding
	IndentLookForward = 1 << 5		# indent_guide_style
	PrintfFormatSpecifier = 1 << 6	# printf_format_specifier
	AngleBracketGeneric = 1 << 7	# angle_bracket_generic
	CppPreprocessor = 1 << 8		# cpp_preprocessor
	CharacterPrefix = 1 << 9		# character_prefix
	EscapePunctuation = 1 << 10		# escape_punctuation
	PlainTextFile = 1 << 11			# plain_text_file

class KeywordAttr(IntFlag):
	Default = 0
	MakeLower = 1	# need converted to lower case for lexer.
	PreSorted = 2	# word list is presorted.
	NoLexer = 4		# not used by lexer, listed for auto-completion.
	NoAutoComp = 8	# don't add to default auto-completion list.
	Special = 256	# used by context based auto-completion.
	PrefixSpace = 512	# prefix first item with extra space.

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
NoEscapeCharacter = '\0'

LexerConfigMap = {
	'NP2LEX_TEXTFILE': {
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'escape_char_start': NoEscapeCharacter,
		'extra_word_char': '-',
		'plain_text_file': True,
	},
	'NP2LEX_2NDTEXTFILE': {
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'escape_char_start': NoEscapeCharacter,
		'extra_word_char': '-',
		'plain_text_file': True,
	},
	'NP2LEX_ANSI': {
		'default_encoding': 'DOS-437',
		'default_line_ending': 'CRLF',
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'escape_char_start': NoEscapeCharacter,
		'extra_word_char': '-',
		'plain_text_file': True,
	},

	'NP2LEX_ABAQUS': {
		'line_comment_string': '**',
		'comment_style_marker': 'SCE_APDL_COMMENT',
		'operator_style': ['SCE_APDL_OPERATOR'],
		'string_style_range': ['SCE_APDL_STRING', 'SCE_APDL_STRING'],
	},
	'NP2LEX_ACTIONSCRIPT': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_JS_TASKMARKER',
		'default_fold_level': ['class', 'anonymous object', 'method'],
		'default_fold_ignore_inner': 'SCE_JS_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_JS_ESCAPECHAR',
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_JS_CLASS', 'SCE_JS_INTERFACE', 'SCE_JS_ENUM', 'SCE_JS_WORD2'],
		'extra_word_char': '$#@',
		'operator_style': ['SCE_JS_OPERATOR', 'SCE_JS_OPERATOR2', 'SCE_JS_OPERATOR_PF'],
		#'ignore_word_style': ['SCE_JS_WORD', 'SCE_JS_WORD2', 'SCE_JS_DIRECTIVE'],
		'string_style_range': ['SCE_JSX_TEXT', 'SCE_JS_ESCAPECHAR'],
	},
	'NP2LEX_APDL': {
		'line_comment_string': '!',
		'comment_style_marker': 'SCE_APDL_COMMENT',
		'operator_style': ['SCE_APDL_OPERATOR'],
		'string_style_range': ['SCE_APDL_STRING', 'SCE_APDL_STRING'],
	},
	'NP2LEX_ASM': {
		'line_comment_string': [';', '# ', '//', '@ '],
		'block_comment_string': ('/*', '*/'),
		'comment_style_marker': 'SCE_ASM_TASKMARKER',
		'operator_style': ['SCE_ASM_OPERATOR'],
		#'cpp_preprocessor': True,
		'string_style_range': ['SCE_ASM_CHARACTER', 'SCE_ASM_STRING'],
	},
	'NP2LEX_ASYMPTOTE': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_ASY_TASKMARKER',
		'default_fold_level': ['struct', 'function'],
		'default_fold_ignore_inner': 'SCE_ASY_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_ASY_ESCAPECHAR',
		'operator_style': ['SCE_ASY_OPERATOR'],
		#'ignore_word_style': ['SCE_ASY_WORD', 'SCE_ASY_TYPE', 'SCE_ASY_CONSTANT'],
		'string_style_range': ['SCE_ASY_STRING_SQ', 'SCE_ASY_ESCAPECHAR'],
	},
	'NP2LEX_AUTOHOTKEY': {
		'line_comment_string': ';',
		'block_comment_string': ('/*', '*/'),
		'comment_style_marker': 'SCE_AHK_TASKMARKER',
		#'block_comment_on_new_line': True,
		'default_fold_level': ['class', 'function'],
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_AHK_FORMAT_SPECIFIER',
		'escape_char_start': '`',
		'escape_char_style': 'SCE_AHK_ESCAPECHAR',
		'operator_style': ['SCE_AHK_OPERATOR'],
		'string_style_range': ['SCE_AHK_SECTION_SQ', 'SCE_AHK_FORMAT_SPECIFIER'],
	},
	'NP2LEX_AUTOIT3': {
		'line_comment_string': ';',
		'block_comment_string': ('#cs', '#ce'),
		'comment_style_marker': 'SCE_AU3_COMMENTBLOCK',
		'block_comment_on_new_line': True,
		'operator_style': ['SCE_AU3_OPERATOR'],
		'extra_word_char': '$',
		#'auto_ident_word_style': ['SCE_AU3_KEYWORD'],
		'string_style_range': ['SCE_AU3_STRING', 'SCE_AU3_STRING'],
	},
	'NP2LEX_AVISYNTH': {
		'line_comment_string': '#',
		'block_comment_string': ('/*', '*/'),
		'comment_style_marker': 'SCE_AVS_TASKMARKER',
		'default_fold_level': ['function'],
		'escape_char_style': 'SCE_AVS_ESCAPECHAR',
		'operator_style': ['SCE_AVS_OPERATOR'],
		'string_style_range': ['SCE_AVS_STRING', 'SCE_AVS_ESCAPECHAR'],
	},
	'NP2LEX_AWK': {
		'line_comment_string': '#',
		'comment_style_marker': 'SCE_AWK_TASKMARKER',
		'shebang_exe_name': 'awk',
		'default_fold_level': ['namespace', 'function'],
		'default_fold_ignore_inner': 'SCE_AWK_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_AWK_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_AWK_ESCAPECHAR',
		'operator_style': ['SCE_AWK_OPERATOR'],
		'extra_word_char': '$@:',
		#'ignore_word_style': ['SCE_AWK_WORD', 'SCE_AWK_BUILTIN_VARIABLE', 'SCE_AWK_BUILTIN_FUNCTION'],
		'string_style_range': ['SCE_AWK_STRING', 'SCE_AWK_REGEX'],
	},

	'NP2LEX_BASH': {
		'default_encoding': 'utf-8',
		'default_line_ending': 'LF',
		'line_comment_string': ['#', 'dnl '],
		'comment_style_marker': 'SCE_SH_COMMENTLINE',
		'shebang_exe_name': ['bash', 'm4', 'csh'],
		'raw_string_style': ['SCE_SH_STRING_SQ'],
		'operator_style': ['SCE_SH_OPERATOR'],
		'extra_word_char': '-$',
		'auto_ident_word_style': ['SCE_SH_WORD'],
		'string_style_range': ['SCE_SH_HERE_DELIM', 'SCE_SH_STRING_DQ'],
	},
	'NP2LEX_BATCH': {
		'default_encoding': 'ANSI',
		'default_line_ending': 'CRLF',
		'line_comment_string': '@rem ',
		'comment_style_marker': 'SCE_BAT_COMMENT',
		'escape_char_start': '^',
		'escape_char_style': 'SCE_BAT_ESCAPECHAR',
		'escape_punctuation': True,
		'operator_style': ['SCE_BAT_OPERATOR'],
		'extra_word_char': '-',
	},
	'NP2LEX_BLOCKDIAG': {
		'line_comment_string': '//',
		'block_comment_string': [('/*', '*/'), ('<!--', '-->')],
		'comment_style_marker': 'SCE_GRAPHVIZ_TASKMARKER',
		'default_fold_level': ['graph', 'subgraph'],
		'escape_char_style': 'SCE_GRAPHVIZ_ESCAPECHAR',
		'operator_style': ['SCE_GRAPHVIZ_OPERATOR'],
		'extra_word_char': '-',
		#'ignore_word_style': ['SCE_GRAPHVIZ_WORD'],
		'string_style_range': ['SCE_GRAPHVIZ_STRING', 'SCE_GRAPHVIZ_ESCAPECHAR'],
	},

	'NP2LEX_CANGJIE': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_CANGJIE_TASKMARKER',
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_CANGJIE_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_CANGJIE_ESCAPECHAR',
		'raw_string_style': ['SCE_CANGJIE_RAWSTRING_SQ', 'SCE_CANGJIE_RAWSTRING_DQ'],
		'character_style': ['SCE_CANGJIE_RUNE_SQ', 'SCE_CANGJIE_RUNE_DQ'],
		'character_prefix': ['r'],
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_CANGJIE_CLASS', 'SCE_CANGJIE_INTERFACE', 'SCE_CANGJIE_STRUCT', 'SCE_CANGJIE_ENUM', 'SCE_CANGJIE_WORD2'],
		'operator_style': ['SCE_CANGJIE_OPERATOR', 'SCE_CANGJIE_OPERATOR2'],
		'string_style_range': ['SCE_CANGJIE_STRING_SQ', 'SCE_CANGJIE_ESCAPECHAR'],
	},
	'NP2LEX_CIL': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_CIL_COMMENTBLOCK',
		'operator_style': ['SCE_CIL_OPERATOR'],
		'cpp_preprocessor': True,
		'extra_word_char': '$',
		'string_style_range': ['SCE_CIL_CHARACTER', 'SCE_CIL_STRING'],
	},
	'NP2LEX_CMAKE': {
		'line_comment_string': '#',
		'block_comment_string': ('#[[', ']]'),
		'comment_style_marker': 'SCE_CMAKE_TASKMARKER',
		'escape_char_style': 'SCE_CMAKE_ESCAPECHAR',
		'escape_punctuation': True,
		'angle_bracket_generic': True, # for bracket argument $<>
		'operator_style': ['SCE_CMAKE_OPERATOR'],
		'auto_ident_word_style': ['SCE_CMAKE_WORD'],
		'string_style_range': ['SCE_CMAKE_STRING', 'SCE_CMAKE_BRACKET_ARGUMENT'],
	},
	'NP2LEX_COFFEESCRIPT': {
		'line_comment_string': '#',
		'block_comment_string': ('###', '###'),
		'comment_style_marker': 'SCE_COFFEESCRIPT_TASKMARKER',
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['class', 'function'],
		'escape_char_style': 'SCE_COFFEESCRIPT_ESCAPECHAR',
		'operator_style': ['SCE_COFFEESCRIPT_OPERATOR', 'SCE_COFFEESCRIPT_OPERATOR2', 'SCE_COFFEESCRIPT_OPERATOR_PF'],
		#'ignore_word_style': ['SCE_COFFEESCRIPT_WORD', 'SCE_COFFEESCRIPT_WORD2', 'SCE_COFFEESCRIPT_DIRECTIVE'],
		'string_style_range': ['SCE_COFFEESCRIPT_STRING_SQ', 'SCE_COFFEESCRIPT_XML_TEXT'],
	},
	'NP2LEX_CONFIG': {
		'line_comment_string': '#',
		'comment_style_marker': 'SCE_CONF_COMMENT',
		'operator_style': ['SCE_CONF_OPERATOR'],
	},
	'NP2LEX_CSV': {
		'escape_char_start': NoEscapeCharacter,
		'extra_word_char': '-',
		'plain_text_file': True,
	},
	'NP2LEX_CPP': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_C_COMMENTDOC_TAG_XML',
		'default_fold_level': ['preprocessor', 'namespace', 'class', 'method'],
		'printf_format_specifier': True,
		'escape_char_style': 'SCE_C_ESCAPECHAR',
		'raw_string_style': ['SCE_C_STRINGRAW', 'SCE_C_COMMENTDOC_TAG'],
		'character_style': ['SCE_C_CHARACTER'],
		'character_prefix': ['L', 'u', 'U', 'u8'],
		'none_quote_style': 'SCE_C_NUMBER',
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_C_CLASS', 'SCE_C_INTERFACE', 'SCE_C_STRUCT', 'SCE_C_WORD2'],
		'operator_style': ['SCE_C_OPERATOR'],
		'cpp_preprocessor': True,
		'extra_word_char': ':',
		'ignore_word_style': ['SCE_C_WORD', 'SCE_C_WORD2', 'SCE_C_PREPROCESSOR', 'SCE_C_ASM_REGISTER', 'SCE_C_ASM_INSTRUCTION'],
		'autoc_extra_keyword': 'kwDoxyDoc',
		#'auto_ident_word_style': ['SCE_C_PREPROCESSOR'],
		'string_style_range': ['SCE_C_CHARACTER', 'SCE_C_ESCAPECHAR'],
	},
	'NP2LEX_CSHARP': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_CSHARP_TASKMARKER',
		'default_fold_level': ['namespace', 'class', 'method'],
		'default_fold_ignore_inner': 'SCE_CSHARP_FUNCTION_DEFINITION',
		'format_specifier_style': 'SCE_CSHARP_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_CSHARP_ESCAPECHAR',
		'raw_string_style': ['SCE_CSHARP_VERBATIM_STRING', 'SCE_CSHARP_INTERPOLATED_VERBATIM_STRING',
			'SCE_CSHARP_RAWSTRING_SL', 'SCE_CSHARP_INTERPOLATED_RAWSTRING_SL',
			'SCE_CSHARP_RAWSTRING_ML', 'SCE_CSHARP_INTERPOLATED_RAWSTRING_ML',
		],
		'character_style': ['SCE_CSHARP_CHARACTER'],
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_CSHARP_CLASS', 'SCE_CSHARP_INTERFACE', 'SCE_CSHARP_STRUCT', 'SCE_CSHARP_ENUM', 'SCE_CSHARP_WORD2'],
		'operator_style': ['SCE_CSHARP_OPERATOR', 'SCE_CSHARP_OPERATOR2'],
		'cpp_preprocessor': True,
		'extra_word_char': '@',
		#'ignore_word_style': ['SCE_CSHARP_WORD', 'SCE_CSHARP_WORD2'],
		'string_style_range': ['SCE_CSHARP_CHARACTER', 'SCE_CSHARP_PREPROCESSOR_MESSAGE'],
	},
	'NP2LEX_CSS': {
		'tab_settings': TabSettings_Space2,
		'line_comment_string': ('', '//'),
		'block_comment_string': ('/*', '*/'),
		'comment_style_marker': 'SCE_CSS_CDO_CDC',
		'escape_char_style': 'SCE_CSS_ESCAPECHAR',
		'operator_style': ['SCE_CSS_OPERATOR', 'SCE_CSS_OPERATOR2'],
		'extra_word_char': '-$@',
		#'ignore_word_style': ['SCE_CSS_PROPERTY', 'SCE_CSS_PSEUDOCLASS', 'SCE_CSS_PSEUDOELEMENT'],
		'string_style_range': ['SCE_CSS_ESCAPECHAR', 'SCE_CSS_URL'],
	},

	'NP2LEX_DLANG': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_D_TASKMARKER',
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_D_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_D_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_D_ESCAPECHAR',
		'raw_string_style': ['SCE_D_RAWSTRING', 'SCE_D_STRING_BT'],
		'character_style': ['SCE_D_CHARACTER'],
		'operator_style': ['SCE_D_OPERATOR'],
		'cpp_preprocessor': True,
		#'ignore_word_style': ['SCE_D_WORD', 'SCE_D_WORD2', 'SCE_D_ASM_REGISTER', 'SCE_D_ASM_INSTRUCTION'],
		'string_style_range': ['SCE_D_CHARACTER', 'SCE_D_DELIMITED_STRING'],
	},
	'NP2LEX_DART': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_DART_TASKMARKER',
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_DART_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_DART_ESCAPECHAR',
		'raw_string_style': ['SCE_DART_RAWSTRING_SQ', 'SCE_DART_RAWSTRING_DQ',
			'SCE_DART_TRIPLE_RAWSTRING_SQ', 'SCE_DART_TRIPLE_RAWSTRING_DQ',
		],
		'character_prefix': ['r'],
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_DART_CLASS', 'SCE_DART_ENUM', 'SCE_DART_WORD2'],
		'operator_style': ['SCE_DART_OPERATOR', 'SCE_DART_OPERATOR2'],
		'extra_word_char': '$',
		#'ignore_word_style': ['SCE_DART_WORD', 'SCE_DART_WORD2'],
		'string_style_range': ['SCE_DART_STRING_SQ', 'SCE_DART_ESCAPECHAR'],
	},
	'NP2LEX_DIFF': {
		'comment_style_marker': 'SCE_DIFF_COMMENT',
		'default_fold_level': ['command', '[file]', 'diff'],
		'escape_char_start': NoEscapeCharacter,
		'plain_text_file': True,
	},

	'NP2LEX_FORTRAN': {
		'default_fold_ignore_inner': 'SCE_F_FUNCTION_DEFINITION',
		'line_comment_string': ['!'], # omited '*'
		'comment_style_marker': 'SCE_F_COMMENT',
		'escape_char_style': 'SCE_F_ESCAPECHAR',
		'character_prefix': ['b', 'B', 'o', 'O', 'z', 'Z'],
		'operator_style': ['SCE_F_OPERATOR', 'SCE_F_OPERATOR2'],
		'cpp_preprocessor': True,
		'extra_word_char': '%',
		'string_style_range': ['SCE_F_STRING_SQ', 'SCE_F_ESCAPECHAR'],
	},
	'NP2LEX_FSHARP': {
		'line_comment_string': '//',
		'block_comment_string': ('(*', '*)'),
		'comment_style_marker': 'SCE_FSHARP_TASKMARKER',
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['function'],
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_FSHARP_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_FSHARP_ESCAPECHAR',
		'raw_string_style': ['SCE_FSHARP_VERBATIM_STRING', 'SCE_FSHARP_INTERPOLATED_VERBATIM_STRING',
			'SCE_FSHARP_TRIPLE_STRING', 'SCE_FSHARP_INTERPOLATED_TRIPLE_STRING'],
		'character_style': ['SCE_FSHARP_CHARACTER'],
		'none_quote_style': 'SCE_FSHARP_IDENTIFIER',
		'operator_style': ['SCE_FSHARP_OPERATOR', 'SCE_FSHARP_OPERATOR2'],
		'cpp_preprocessor': True,
		'string_style_range': ['SCE_FSHARP_CHARACTER', 'SCE_FSHARP_FORMAT_SPECIFIER'],
	},

	'NP2LEX_GN': {
		'line_comment_string': '#',
		'comment_style_marker': 'SCE_GN_COMMENT',
		'escape_char_style': 'SCE_GN_ESCAPECHAR',
		'escape_punctuation': True,
		'operator_style': ['SCE_GN_OPERATOR', 'SCE_GN_OPERATOR2'],
		'extra_word_char': '-$',
		#'ignore_word_style': ['SCE_GN_KEYWORD', 'SCE_GN_BUILTIN_FUNCTION', 'SCE_GN_BUILTIN_VARIABLE'],
		'string_style_range': ['SCE_GN_STRING', 'SCE_GN_ESCAPECHAR'],
	},
	'NP2LEX_GO': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_GO_TASKMARKER',
		'default_fold_level': ['struct', 'function'],
		'default_fold_ignore_inner': 'SCE_GO_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_GO_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_GO_ESCAPECHAR',
		'raw_string_style': ['SCE_GO_RAW_STRING'],
		'character_style': ['SCE_GO_CHARACTER'],
		'operator_style': ['SCE_GO_OPERATOR', 'SCE_GO_OPERATOR2'],
		#'ignore_word_style': ['SCE_GO_WORD', 'SCE_GO_WORD', 'SCE_GO_BUILTIN_FUNC'],
		'string_style_range': ['SCE_GO_CHARACTER', 'SCE_GO_FORMAT_SPECIFIER'],
	},
	'NP2LEX_GRADLE': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_GROOVY_TASKMARKER',
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_GROOVY_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_GROOVY_ESCAPECHAR',
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_GROOVY_CLASS', 'SCE_GROOVY_INTERFACE', 'SCE_GROOVY_TRAIT', 'SCE_GROOVY_ENUM'],
		'operator_style': ['SCE_GROOVY_OPERATOR', 'SCE_GROOVY_OPERATOR2', 'SCE_GROOVY_OPERATOR_PF'],
		'extra_word_char': '$',
		#'ignore_word_style': ['SCE_GROOVY_WORD', 'SCE_GROOVY_WORD_DEMOTED', 'SCE_GROOVY_WORD2'],
		'string_style_range': ['SCE_GROOVY_STRING_DQ', 'SCE_GROOVY_ESCAPECHAR'],
	},
	'NP2LEX_GRAPHVIZ': {
		'line_comment_string': '//',
		'block_comment_string': [('/*', '*/'), ('<!--', '-->')],
		'comment_style_marker': 'SCE_GRAPHVIZ_TASKMARKER',
		'default_fold_level': ['graph', 'subgraph'],
		'escape_char_style': 'SCE_GRAPHVIZ_ESCAPECHAR',
		'operator_style': ['SCE_GRAPHVIZ_OPERATOR'],
		'extra_word_char': '-',
		#'ignore_word_style': ['SCE_GRAPHVIZ_WORD'],
		'string_style_range': ['SCE_GRAPHVIZ_STRING', 'SCE_GRAPHVIZ_ESCAPECHAR'],
	},
	'NP2LEX_GROOVY': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_GROOVY_TASKMARKER',
		'shebang_exe_name': 'groovy',
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_GROOVY_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_GROOVY_ESCAPECHAR',
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_GROOVY_CLASS', 'SCE_GROOVY_INTERFACE', 'SCE_GROOVY_TRAIT', 'SCE_GROOVY_ENUM'],
		'operator_style': ['SCE_GROOVY_OPERATOR', 'SCE_GROOVY_OPERATOR2', 'SCE_GROOVY_OPERATOR_PF'],
		'extra_word_char': '$',
		#'ignore_word_style': ['SCE_GROOVY_WORD', 'SCE_GROOVY_WORD_DEMOTED', 'SCE_GROOVY_WORD2'],
		'string_style_range': ['SCE_GROOVY_STRING_DQ', 'SCE_GROOVY_ESCAPECHAR'],
	},

	'NP2LEX_HASKELL': {
		'line_comment_string': '--',
		'block_comment_string': ('{-', '-}'),
		'comment_style_marker': 'SCE_HA_COMMENTBLOCK',
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['function'],
		'escape_char_style': 'SCE_HA_ESCAPECHAR',
		'character_style': ['SCE_HA_CHARACTER'],
		'none_quote_style': 'SCE_HA_IDENTIFIER',
		'operator_style': ['SCE_HA_OPERATOR'],
		'string_style_range': ['SCE_HA_CHARACTER', 'SCE_HA_ESCAPECHAR'],
	},
	'NP2LEX_HAXE': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_HAXE_TASKMARKER',
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_HAXE_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_HAXE_ESCAPECHAR',
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_HAXE_CLASS', 'SCE_HAXE_INTERFACE', 'SCE_HAXE_ENUM'],
		'operator_style': ['SCE_HAXE_OPERATOR', 'SCE_HAXE_OPERATOR2'],
		'cpp_preprocessor': True,
		#'ignore_word_style': ['SCE_HAXE_WORD', 'SCE_HAXE_PREPROCESSOR'],
		'string_style_range': ['SCE_HAXE_STRINGDQ', 'SCE_HAXE_REGEX'],
	},
	'NP2LEX_HTML': {
		'tab_settings': TabSettings_Space2,
		'line_comment_string': ['//', "'", '#'],
		'block_comment_string': [('<!--', '-->'), ('/*', '*/'), ('--', '--')],
		'comment_style_list': ['SCE_H_COMMENT', 'SCE_H_XCCOMMENT', 'SCE_H_SGML_COMMENT', 'SCE_H_SGML_1ST_PARAM_COMMENT',
			'SCE_HJ_COMMENT', 'SCE_HJ_COMMENTLINE', 'SCE_HJ_COMMENTDOC',
			'SCE_HJA_COMMENT', 'SCE_HJA_COMMENTLINE', 'SCE_HJA_COMMENTDOC',
			'SCE_HB_COMMENTLINE', 'SCE_HBA_COMMENTLINE'],
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
		#'escape_char_start': NoEscapeCharacter, # backslash for embedded script or style
		'extra_word_char': '-:',
		'string_style_list': ['SCE_H_DOUBLESTRING', 'SCE_H_SINGLESTRING', 'SCE_H_SGML_DOUBLESTRING', 'SCE_H_SGML_SIMPLESTRING',
			'SCE_HJ_DOUBLESTRING', 'SCE_HJ_SINGLESTRING', 'SCE_HJ_REGEX', 'SCE_HJ_TEMPLATELITERAL',
			'SCE_HJA_DOUBLESTRING', 'SCE_HJA_SINGLESTRING', 'SCE_HJA_REGEX', 'SCE_HJA_TEMPLATELITERAL',
			'SCE_HB_STRING', 'SCE_HBA_STRING'],
		'plain_text_style': ['SCE_H_DEFAULT'],
	},

	'NP2LEX_INI': {
		'line_comment_string': ';',
		'comment_style_marker': 'SCE_PROPS_COMMENT',
		'default_fold_level': ['section', 'comment'],
		'escape_char_start': NoEscapeCharacter,
	},
	'NP2LEX_INNOSETUP': {
		'line_comment_string': [';', '//'],
		'block_comment_string': [('/*', '*/'), ('{', '}'), ('(*', '*)')],
		'comment_style_marker': 'SCE_INNO_TASKMARKER',
		'default_fold_level': ['section', 'code'],
		'escape_char_start': NoEscapeCharacter,
		'operator_style': ['SCE_INNO_OPERATOR'],
		'cpp_preprocessor': True,
		#'auto_ident_word_style': ['SCE_INNO_PREPROCESSOR_WORD'],
		'string_style_range': ['SCE_INNO_STRING_DQ', 'SCE_INNO_PREPROCESSOR_MESSAGE'],
	},

	'NP2LEX_JAMFILE': {
		'line_comment_string': '#',
		'block_comment_string': ('#|', '|#'),
		'comment_style_marker': 'SCE_JAM_TASKMARKER',
		'escape_char_style': 'SCE_JAM_ESCAPECHAR',
		'operator_style': ['SCE_JAM_OPERATOR'],
		#'ignore_word_style': ['SCE_JAM_WORD'],
		'string_style_range': ['SCE_JAM_STRING', 'SCE_JAM_ESCAPECHAR'],
	},
	'NP2LEX_JAVA': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_JAVA_TASKMARKER',
		'default_fold_level': ['class', 'inner class', 'method'],
		'default_fold_ignore_inner': 'SCE_JAVA_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_JAVA_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_JAVA_ESCAPECHAR',
		'character_style': ['SCE_JAVA_CHARACTER'],
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_JAVA_CLASS', 'SCE_JAVA_INTERFACE', 'SCE_JAVA_ENUM'],
		'operator_style': ['SCE_JAVA_OPERATOR', 'SCE_JAVA_OPERATOR2'],
		'extra_word_char': '$:',
		#'ignore_word_style': ['SCE_JAVA_WORD', 'SCE_JAVA_WORD2', 'SCE_JAVA_DIRECTIVE'],
		'string_style_range': ['SCE_JAVA_STRING', 'SCE_JAVA_CHARACTER'],
	},
	'NP2LEX_JAVASCRIPT': {
		'tab_settings': TabSettings_Space2,
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_JS_TASKMARKER',
		'shebang_exe_name': 'node',
		'default_fold_level': ['class', 'anonymous object', 'method'],
		'default_fold_ignore_inner': 'SCE_JS_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_JS_ESCAPECHAR',
		'operator_style': ['SCE_JS_OPERATOR', 'SCE_JS_OPERATOR2', 'SCE_JS_OPERATOR_PF'],
		'extra_word_char': '$#@',
		#'ignore_word_style': ['SCE_JS_WORD', 'SCE_JS_WORD2', 'SCE_JS_DIRECTIVE'],
		'string_style_range': ['SCE_JSX_TEXT', 'SCE_JS_ESCAPECHAR'],
	},
	'NP2LEX_JSON': {
		'tab_settings': TabSettings_Space2,
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_JSON_BLOCKCOMMENT',
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
		'escape_char_style': 'SCE_JSON_ESCAPECHAR',
		'operator_style': ['SCE_JSON_OPERATOR'],
		#'ignore_word_style': ['SCE_JSON_KEYWORD'],
		'string_style_range': ['SCE_JSON_STRING_DQ', 'SCE_JSON_ESCAPECHAR'],
	},
	'NP2LEX_JULIA': {
		'line_comment_string': '#',
		'block_comment_string': ('#=', '=#'),
		'comment_style_marker': 'SCE_JULIA_TASKMARKER',
		'default_fold_level': ['struct', 'method'],
		'default_fold_ignore_inner': 'SCE_JULIA_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_JULIA_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_JULIA_ESCAPECHAR',
		'raw_string_style': ['SCE_JULIA_RAWSTRING', 'SCE_JULIA_TRIPLE_RAWSTRING'],
		'character_style': ['SCE_JULIA_CHARACTER'],
		'none_quote_style': 'SCE_JULIA_OPERATOR',
		'operator_style': ['SCE_JULIA_OPERATOR', 'SCE_JULIA_OPERATOR2'],
		'extra_word_char': '$:',
		#'ignore_word_style': ['SCE_JULIA_WORD', 'SCE_JULIA_WORD_DEMOTED', 'SCE_JULIA_CONSTANT', 'SCE_JULIA_BASIC_FUNCTION'],
		'auto_ident_word_style': ['SCE_JULIA_WORD'],
		'string_style_range': ['SCE_JULIA_CHARACTER', 'SCE_JULIA_TRIPLE_REGEX'],
	},

	'NP2LEX_KOTLIN': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_KOTLIN_TASKMARKER',
		#'shebang_exe_name': 'kotlin',
		'default_fold_level': ['class', 'inner class', 'method'],
		'default_fold_ignore_inner': 'SCE_KOTLIN_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_KOTLIN_ESCAPECHAR',
		'raw_string_style': ['SCE_KOTLIN_RAWSTRING'],
		'character_style': ['SCE_KOTLIN_CHARACTER'],
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_KOTLIN_CLASS', 'SCE_KOTLIN_INTERFACE', 'SCE_KOTLIN_ENUM'],
		'operator_style': ['SCE_KOTLIN_OPERATOR', 'SCE_KOTLIN_OPERATOR2'],
		'extra_word_char': '$:',
		#'ignore_word_style': ['SCE_KOTLIN_WORD'],
		'string_style_range': ['SCE_KOTLIN_CHARACTER', 'SCE_KOTLIN_ESCAPECHAR'],
	},

	'NP2LEX_LATEX': {
		'line_comment_string': '%',
		'block_comment_string': ('\\begin{comment}', '\\end{comment}'),
		'block_comment_on_new_line': True,
		'comment_style_marker': 'SCE_L_COMMENT2',
		'escape_char_start': '^',
		'escape_char_style': 'SCE_L_SPECIAL',
		'escape_punctuation': True,
		'operator_style': ['SCE_L_OPERATOR'],
		'plain_text_style': ['SCE_L_DEFAULT', 'SCE_L_VERBATIM2', 'SCE_L_VERBATIM',
			'SCE_L_TITLE', 'SCE_L_CHAPTER', 'SCE_L_SECTION', 'SCE_L_SECTION1', 'SCE_L_SECTION2'],
	},
	'NP2LEX_LISP': {
		'line_comment_string': ';',
		'block_comment_string': ('#|',  '|#'),
		'comment_style_marker': 'SCE_LISP_TASKMARKER',
		'none_quote_style': 'SCE_LISP_OPERATOR',
		'operator_style': ['SCE_LISP_OPERATOR'],
		'extra_word_char': '-',
		'string_style_range': ['SCE_LISP_CHARACTER', 'SCE_LISP_STRING'],
	},
	'NP2LEX_LLVM': {
		'line_comment_string': ';',
		'comment_style_marker': 'SCE_LLVM_TASKMARKER',
		'escape_char_style': 'SCE_LLVM_ESCAPECHAR',
		'operator_style': ['SCE_LLVM_OPERATOR'],
		'extra_word_char': '-@%$',
		'string_style_range': ['SCE_LLVM_STRING', 'SCE_LLVM_ESCAPECHAR'],
	},
	'NP2LEX_LUA': {
		'line_comment_string': '--',
		'block_comment_string': ('--[[', '--]]'),
		'comment_style_marker': 'SCE_LUA_COMMENTLINE',
		'shebang_exe_name': 'lua',
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_LUA_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_LUA_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_LUA_ESCAPECHAR',
		'raw_string_style': ['SCE_LUA_LITERALSTRING'],
		'operator_style': ['SCE_LUA_OPERATOR'],
		#'ignore_word_style': ['SCE_LUA_WORD'],
		'auto_ident_word_style': ['SCE_LUA_WORD'],
		'string_style_range': ['SCE_LUA_STRING_SQ', 'SCE_LUA_LITERALSTRING'],
	},

	'NP2LEX_MAKEFILE': {
		'tab_settings': TabSettings_Tab4,
		'line_comment_string': '#',
		'comment_style_marker': 'SCE_MAKE_COMMENT',
		'escape_char_start': NoEscapeCharacter,
		'operator_style': ['SCE_MAKE_OPERATOR'],
		'extra_word_char': '-$!',
		'auto_ident_word_style': ['SCE_MAKE_PREPROCESSOR'],
	},
	'NP2LEX_MATHEMATICA': {
		'block_comment_string': ('(*', '*)'),
		'comment_style_marker': 'SCE_MATHEMATICA_COMMENT',
		'escape_char_style': 'SCE_MARKDOWN_ESCAPECHAR',
		'escape_punctuation': True,
		'none_quote_style': 'SCE_MATHEMATICA_OPERATOR',
		'operator_style': ['SCE_MATHEMATICA_OPERATOR'],
		'string_style_range': ['SCE_MATHEMATICA_STRING', 'SCE_MATHEMATICA_ESCAPECHAR'],
	},
	'NP2LEX_MARKDOWN': {
		'block_comment_string': ('<!--', '-->'),
		'comment_style_list': ['SCE_H_COMMENT', 'SCE_H_SGML_COMMENT', 'STYLE_COMMENT_LINK'],
		'default_fold_level': ['header1', 'header2', 'header3'],
		'escape_char_style': 'SCE_MARKDOWN_ESCAPECHAR',
		'escape_punctuation': True,
		'string_style_list': ['SCE_H_DOUBLESTRING', 'SCE_H_SINGLESTRING', 'SCE_H_SGML_DOUBLESTRING', 'SCE_H_SGML_SIMPLESTRING'],
		'plain_text_style': ['SCE_H_DEFAULT'],
	},
	'NP2LEX_MATLAB': {
		'line_comment_string': ['%', '//'],
		'block_comment_string': [('%{', '}%'), ('/*', '*/')],
		'block_comment_on_new_line': True,
		'comment_style_marker': 'SCE_MAT_TASKMARKER',
		'printf_format_specifier': True,
		#'escape_char_start': NoEscapeCharacter, # backslash for Octave escape character
		'none_quote_style': 'SCE_MAT_OPERATOR',
		'operator_style': ['SCE_MAT_OPERATOR'],
		#'ignore_word_style': ['SCE_MAT_KEYWORD'],
		'auto_ident_word_style': ['SCE_MAT_KEYWORD'],
		'string_style_range': ['SCE_MAT_STRING', 'SCE_MAT_DOUBLEQUOTESTRING'],
	},

	'NP2LEX_NIM': {
		'line_comment_string': '#',
		'block_comment_string': ('#[', ']#'),
		'comment_style_marker': 'SCE_NIM_TASKMARKER',
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['type', 'function'],
		'default_fold_ignore_inner': 'SCE_NIM_FUNCTION_DEFINITION',
		#'printf_format_specifier': True,
		'format_specifier_style': 'SCE_NIM_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_NIM_ESCAPECHAR',
		'raw_string_style': ['SCE_NIM_RAWSTRING', 'SCE_NIM_RAWFMTSTRING',
			'SCE_NIM_TRIPLE_STRING', 'SCE_NIM_TRIPLE_FMTSTRING',
		],
		'character_style': ['SCE_NIM_CHARACTER'],
		'none_quote_style': 'SCE_NIM_NUMBER',
		'operator_style': ['SCE_NIM_OPERATOR', 'SCE_NIM_OPERATOR2'],
		'string_style_range': ['SCE_NIM_CHARACTER', 'SCE_NIM_FORMAT_SPECIFIER'],
	},
	'NP2LEX_NSIS': {
		'line_comment_string': ['#'], # omited ';'
		'block_comment_string': ('/*', '*/'),
		'comment_style_marker': 'SCE_NSIS_TASKMARKER',
		'default_fold_level': ['section', 'function'],
		'escape_char_style': 'SCE_NSIS_ESCAPECHAR',
		'operator_style': ['SCE_NSIS_OPERATOR'],
		'extra_word_char': '-$!',
		#'auto_ident_word_style': ['SCE_NSIS_WORD', 'SCE_NSIS_PREPROCESSOR'],
		'string_style_range': ['SCE_NSIS_STRINGDQ', 'SCE_NSIS_ESCAPECHAR'],
	},

	'NP2LEX_OCAML': {
		'block_comment_string': ('(*', '*)'),
		'comment_style_marker': 'SCE_OCAML_COMMENT',
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['function'],
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_OCAML_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_OCAML_ESCAPECHAR',
		'raw_string_style': ['SCE_OCAML_QUOTED_STRING'],
		'character_style': ['SCE_OCAML_CHARACTER'],
		'none_quote_style': 'SCE_OCAML_IDENTIFIER',
		'operator_style': ['SCE_OCAML_OPERATOR'],
		'string_style_range': ['SCE_OCAML_CHARACTER', 'SCE_OCAML_FORMAT_SPECIFIER'],
	},

	'NP2LEX_PASCAL': {
		'line_comment_string': '//',
		'block_comment_string': [('{', '}')], # omited ('(*', '*)')
		'comment_style_marker': 'SCE_PAS_TASKMARKER',
		'operator_style': ['SCE_PAS_OPERATOR'],
		#'auto_ident_word_style': ['SCE_PAS_PREPROCESSOR'],
		'string_style_range': ['SCE_PAS_CHARACTER', 'SCE_PAS_STRING_DQ'],
	},
	'NP2LEX_PERL': {
		'line_comment_string': '#',
		'comment_style_marker': 'SCE_PL_COMMENTLINE',
		'shebang_exe_name': 'perl',
		'printf_format_specifier': True,
		'raw_string_style': ['SCE_PL_STRING_SQ'],
		'none_quote_style': 'SCE_PL_OPERATOR',
		'operator_style': ['SCE_PL_OPERATOR'],
		'extra_word_char': '$@',
		'string_style_range': ['SCE_PL_STRING_DQ', 'SCE_PL_FORMAT'],
		'plain_text_style': ['SCE_PL_POD', 'SCE_PL_POD_VERB', 'SCE_PL_DATASECTION'],
	},
	'NP2LEX_PHP': {
		'line_comment_string': ['//', '#'],
		'block_comment_string': [('/*', '*/'), ('<!--', '-->'), ('--', '--')],
		'comment_style_list': ['SCE_H_COMMENT', 'SCE_H_SGML_COMMENT',
			'SCE_PHP_COMMENTLINE', 'SCE_PHP_COMMENTBLOCK', 'SCE_PHP_COMMENTBLOCKDOC', 'SCE_PHP_COMMENTTAGAT', 'SCE_PHP_TASKMARKER',
			'js_style(SCE_JS_COMMENTLINE)', 'js_style(SCE_JS_COMMENTBLOCK)', 'js_style(SCE_JS_COMMENTBLOCKDOC)',
			'js_style(SCE_JS_COMMENTTAGAT)', 'js_style(SCE_JS_TASKMARKER)',
			'css_style(SCE_CSS_COMMENTBLOCK)', 'css_style(SCE_CSS_CDO_CDC)'],
		'shebang_exe_name': 'php',
		'default_fold_level': ['[php tag]', 'class', 'method'],
		'default_fold_ignore_inner': 'SCE_PHP_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_PHP_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_PHP_ESCAPECHAR',
		'raw_string_style': ['SCE_PHP_STRING_SQ', 'SCE_PHP_NOWDOC'],
		'operator_style': ['SCE_PHP_OPERATOR', 'SCE_PHP_OPERATOR2'],
		'extra_word_char': '-$:',
		'string_style_range': ['SCE_PHP_ESCAPECHAR', 'SCE_PHP_NOWDOC_ID'],
		'string_style_list': ['SCE_H_DOUBLESTRING', 'SCE_H_SINGLESTRING', 'SCE_H_SGML_DOUBLESTRING', 'SCE_H_SGML_SIMPLESTRING',
			'js_style(SCE_JS_STRING_SQ)', 'js_style(SCE_JS_STRING_DQ)',
			'js_style(SCE_JS_STRING_BT)', 'js_style(SCE_JS_REGEX)', 'js_style(SCE_JS_ESCAPECHAR)',
			'css_style(SCE_CSS_ESCAPECHAR)', 'css_style(SCE_CSS_STRING_SQ)', 'css_style(SCE_CSS_STRING_DQ)', 'css_style(SCE_CSS_URL)'],
		'plain_text_style': ['SCE_H_DEFAULT'],
	},
	'NP2LEX_POWERSHELL': {
		'line_comment_string': '#',
		'block_comment_string': ('<#', '#>'),
		'comment_style_marker': 'SCE_POWERSHELL_TASKMARKER',
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_POWERSHELL_FUNCTION_DEFINITION',
		'escape_char_start': '`',
		'escape_char_style': 'SCE_POWERSHELL_ESCAPECHAR',
		'raw_string_style': ['SCE_POWERSHELL_STRING_SQ', 'SCE_POWERSHELL_HERE_STRING_SQ'],
		'operator_style': ['SCE_POWERSHELL_OPERATOR', 'SCE_POWERSHELL_OPERATOR2'],
		'extra_word_char': '-$:@?',
		'string_style_range': ['SCE_POWERSHELL_ESCAPECHAR', 'SCE_POWERSHELL_HERE_STRING_SQ'],
	},
	'NP2LEX_PYTHON': {
		'line_comment_string': '#',
		'shebang_exe_name': 'python3',
		'comment_style_marker': 'SCE_PY_TASKMARKER',
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_PY_FUNCTION_DEFINITION',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_PY_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_PY_ESCAPECHAR',
		'character_prefix': ['r', 'b', 'f', 'u', 'R', 'B', 'F', 'U'],
		'raw_string_style': ['SCE_PY_RAWSTRING_SQ', 'SCE_PY_RAWSTRING_DQ',
			'SCE_PY_TRIPLE_RAWSTRING_SQ', 'SCE_PY_TRIPLE_RAWSTRING_DQ',
			'SCE_PY_RAWFMTSTRING_SQ', 'SCE_PY_RAWFMTSTRING_DQ',
			'SCE_PY_TRIPLE_RAWFMTSTRING_SQ', 'SCE_PY_TRIPLE_RAWFMTSTRING_DQ',
			'SCE_PY_RAWBYTES_SQ', 'SCE_PY_RAWBYTES_DQ',
			'SCE_PY_TRIPLE_RAWBYTES_SQ', 'SCE_PY_TRIPLE_RAWBYTES_DQ',
		],
		'operator_style': ['SCE_PY_OPERATOR', 'SCE_PY_OPERATOR2'],
		'extra_word_char': '$',
		'ignore_word_style': ['SCE_PY_WORD', 'SCE_PY_WORD2', 'SCE_PY_BUILTIN_CONSTANT', 'SCE_PY_BUILTIN_FUNCTION', 'SCE_PY_ATTRIBUTE', 'SCE_PY_OBJECT_FUNCTION'],
		'string_style_range': ['SCE_PY_STRING_SQ', 'SCE_PY_TRIPLE_RAWBYTES_DQ'],
		'string_style_list': ['SCE_PY_ESCAPECHAR', 'SCE_PY_FORMAT_SPECIFIER'],
	},

	'NP2LEX_RLANG': {
		'line_comment_string': '#',
		'comment_style_marker': 'SCE_R_TASKMARKER',
		'shebang_exe_name': 'Rscript',
		'default_fold_level': ['function'],
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_R_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_R_ESCAPECHAR',
		'character_prefix': ['r', 'R'],
		'raw_string_style': ['SCE_R_RAWSTRING_SQ', 'SCE_R_RAWSTRING_DQ'],
		'operator_style': ['SCE_R_OPERATOR', 'SCE_R_INFIX'],
		#'ignore_word_style': ['SCE_R_KEYWORD'],
		'string_style_range': ['SCE_R_STRING_SQ', 'SCE_R_FORMAT_SPECIFIER'],
	},
	'NP2LEX_REBOL': {
		'line_comment_string': ';',
		'block_comment_string': ('comment {', '}'),
		'block_comment_on_new_line': True,
		'comment_style_marker': 'SCE_REBOL_TASKMARKER',
		'escape_char_start': '^',
		'escape_char_style': 'SCE_REBOL_ESCAPECHAR',
		'none_quote_style': 'SCE_REBOL_SYMBOL',
		'operator_style': ['SCE_REBOL_OPERATOR'],
		'cpp_preprocessor': True,
		# http://www.rebol.com/r3/docs/guide/code-syntax.html#section-4
		'extra_word_char': '-!?~+&*=',
		'string_style_range': ['SCE_REBOL_BRACEDSTRING', 'SCE_REBOL_QUOTEDSTRING'],
	},
	'NP2LEX_RESOURCESCRIPT': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_C_COMMENTDOC_TAG_XML',
		'default_fold_level': ['preprocessor', 'resource'],
		'printf_format_specifier': True,
		'escape_char_style': 'SCE_C_ESCAPECHAR',
		'raw_string_style': ['SCE_C_STRINGRAW', 'SCE_C_COMMENTDOC_TAG'],
		'character_style': ['SCE_C_CHARACTER'],
		'character_prefix': ['L', 'u', 'U', 'u8'],
		'none_quote_style': 'SCE_C_NUMBER',
		'operator_style': ['SCE_C_OPERATOR'],
		'cpp_preprocessor': True,
		'ignore_word_style': ['SCE_C_WORD', 'SCE_C_WORD2', 'SCE_C_PREPROCESSOR'],
		#'auto_ident_word_style': ['SCE_C_PREPROCESSOR'],
		'string_style_range': ['SCE_C_CHARACTER', 'SCE_C_ESCAPECHAR'],
	},
	'NP2LEX_RUBY': {
		'line_comment_string': '#',
		'comment_style_marker': 'SCE_RB_COMMENTLINE',
		'shebang_exe_name': 'ruby',
		'default_fold_level': ['module', 'class', 'method'],
		'default_fold_ignore_inner': 'SCE_RB_DEF_NAME',
		'printf_format_specifier': True,
		'raw_string_style': ['SCE_RB_STRING_SQ'],
		'operator_style': ['SCE_RB_OPERATOR'],
		'extra_word_char': '$@?!',
		#'ignore_word_style': ['SCE_RB_WORD', 'SCE_RB_WORD_DEMOTED'],
		'auto_ident_word_style': ['SCE_RB_WORD'],
		'string_style_range': ['SCE_RB_STRING_DQ', 'SCE_RB_STRING_W'],
		'plain_text_style': ['SCE_RB_DATASECTION'],
	},
	'NP2LEX_RUST': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_RUST_TASKMARKER',
		#'shebang_exe_name': 'rust',
		'default_fold_level': ['struct', 'function'],
		'default_fold_ignore_inner': 'SCE_RUST_FUNCTION_DEFINITION',
		'format_specifier_style': 'SCE_RUST_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_RUST_ESCAPECHAR',
		'raw_string_style': ['SCE_RUST_RAW_STRING', 'SCE_RUST_RAW_BYTESTRING'],
		'character_style': ['SCE_RUST_CHARACTER', 'SCE_RUST_BYTE_CHARACTER'],
		'character_prefix': ['b'],
		'none_quote_style': 'SCE_RUST_LIFETIME',
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_RUST_TYPE', 'SCE_RUST_STRUCT', 'SCE_RUST_TRAIT', 'SCE_RUST_ENUMERATION', 'SCE_RUST_UNION'],
		'operator_style': ['SCE_RUST_OPERATOR'],
		'extra_word_char': '$:',
		#'ignore_word_style': ['SCE_RUST_WORD', 'SCE_RUST_WORD2'],
		'string_style_range': ['SCE_RUST_CHARACTER', 'SCE_RUST_FORMAT_SPECIFIER'],
	},

	'NP2LEX_SAS': {
		'block_comment_string': ('/*', '*/'),
		'comment_style_marker': 'SCE_SAS_COMMENTBLOCK',
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['level1'],
		'operator_style': ['SCE_SAS_OPERATOR'],
		'string_style_range': ['SCE_SAS_STRINGDQ', 'SCE_SAS_STRINGSQ'],
	},
	'NP2LEX_SCALA': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_SCALA_TASKMARKER',
		'shebang_exe_name': 'scala',
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['class', 'inner class', 'method'],
		'default_fold_ignore_inner': 'SCE_SCALA_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_SCALA_ESCAPECHAR',
		'character_style': ['SCE_SCALA_CHARACTER'],
		'operator_style': ['SCE_SCALA_OPERATOR', 'SCE_SCALA_OPERATOR2'],
		'extra_word_char': '$',
		#'ignore_word_style': ['SCE_SCALA_KEYWORD'],
		'string_style_range': ['SCE_SCALA_BACKTICKS', 'SCE_SCALA_XML_TEXT'],
	},
	'NP2LEX_SMALI': {
		'line_comment_string': '#',
		'comment_style_marker': 'SCE_SMALI_COMMENTLINE',
		'default_fold_level': ['.method', '.switch'],
		'operator_style': ['SCE_SMALI_OPERATOR'],
		'extra_word_char': '-',
		#'ignore_word_style': ['SCE_SMALI_WORD', 'SCE_SMALI_DIRECTIVE', 'SCE_SMALI_INSTRUCTION'],
		'string_style_range': ['SCE_SMALI_CHARACTER', 'SCE_SMALI_STRING'],
	},
	'NP2LEX_SQL': {
		'line_comment_string': '-- ', # extra space
		'block_comment_string': ('/*', '*/'),
		'comment_style_marker': 'SCE_SQL_COMMENTLINEDOC',
		'default_fold_level': ['function'],
		'escape_char_style': 'SCE_SQL_ESCAPECHAR',
		'character_prefix': ['q', 'Q', 'x', 'X', 'b', 'B'],
		'operator_style': ['SCE_SQL_OPERATOR'], # ignore q'{SCE_SQL_QOPERATOR}'
		'extra_word_char': '$@',
		'ignore_word_style': ['SCE_SQL_WORD', 'SCE_SQL_WORD2', 'SCE_SQL_USER1', 'SCE_SQL_HEX', 'SCE_SQL_HEX2'],
		'auto_ident_word_style': ['SCE_SQL_WORD'],
		'string_style_range': ['SCE_SQL_STRING_DQ', 'SCE_SQL_BIT'],
	},
	'NP2LEX_SWIFT': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_SWIFT_TASKMARKER',
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_SWIFT_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_SWIFT_ESCAPECHAR',
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_SWIFT_CLASS', 'SCE_SWIFT_STRUCT', 'SCE_SWIFT_PROTOCOL', 'SCE_SWIFT_ENUM'],
		'operator_style': ['SCE_SWIFT_OPERATOR', 'SCE_SWIFT_OPERATOR2'],
		'cpp_preprocessor': True,
		#'ignore_word_style': ['SCE_SWIFT_WORD', 'SCE_SWIFT_DIRECTIVE'],
		#'auto_ident_word_style': ['SCE_SWIFT_DIRECTIVE'],
		'string_style_range': ['SCE_SWIFT_ESCAPECHAR', 'SCE_SWIFT_REGEX'],
	},

	'NP2LEX_TCL': {
		'line_comment_string': '#',
		'comment_style_marker': 'SCE_TCL_TASKMARKER',
		'shebang_exe_name': 'wish',
		'printf_format_specifier': True,
		'operator_style': ['SCE_TCL_MODIFIER'],
		'extra_word_char': '$@',
	},
	'NP2LEX_TEXINFO': {
		'line_comment_string': '@c ',
		'block_comment_string': ('@ignore', '@end ignore'),
		'block_comment_on_new_line': True,
		'comment_style_marker': 'SCE_TEXINFO_COMMENT2',
		'default_fold_level': ['chapter', 'section', 'subsection'],
		'escape_char_start': '@',
		'escape_char_style': 'SCE_TEXINFO_SPECIAL',
		'escape_punctuation': True,
		'operator_style': ['SCE_TEXINFO_OPERATOR'],
		'plain_text_style': ['SCE_TEXINFO_DEFAULT', 'SCE_TEXINFO_VERBATIM2', 'SCE_TEXINFO_VERBATIM',
			'SCE_TEXINFO_TITLE', 'SCE_TEXINFO_CHAPTER', 'SCE_TEXINFO_SECTION', 'SCE_TEXINFO_SECTION1', 'SCE_TEXINFO_SECTION2'],
	},
	'NP2LEX_TOML': {
		'tab_settings': TabSettings_Space2,
		'line_comment_string': '#',
		'comment_style_marker': 'SCE_TOML_COMMENT',
		'default_fold_level': ['table', 'comment'],
		'escape_char_style': 'SCE_TOML_ESCAPECHAR',
		'operator_style': ['SCE_TOML_OPERATOR'],
		'string_style_range': ['SCE_TOML_STRING_SQ', 'SCE_TOML_ESCAPECHAR'],
	},
	'NP2LEX_TYPESCRIPT': {
		'tab_settings': TabSettings_Space2,
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_JS_TASKMARKER',
		'default_fold_level': ['class', 'anonymous object', 'method'],
		'default_fold_ignore_inner': 'SCE_JS_FUNCTION_DEFINITION',
		'escape_char_style': 'SCE_JS_ESCAPECHAR',
		'angle_bracket_generic': True,
		'generic_type_style': ['SCE_JS_CLASS', 'SCE_JS_INTERFACE', 'SCE_JS_ENUM', 'SCE_JS_WORD2'],
		'operator_style': ['SCE_JS_OPERATOR', 'SCE_JS_OPERATOR2', 'SCE_JS_OPERATOR_PF'],
		'extra_word_char': '$#@',
		#'ignore_word_style': ['SCE_JS_WORD', 'SCE_JS_WORD2', 'SCE_JS_DIRECTIVE'],
		'string_style_range': ['SCE_JSX_TEXT', 'SCE_JS_ESCAPECHAR'],
	},

	'NP2LEX_VBSCRIPT': {
		'line_comment_string': "'",
		'comment_style_marker': 'SCE_VB_COMMENTLINE',
		'default_fold_level': ['function'],
		'default_fold_ignore_inner': 'SCE_VB_FUNCTION_DEFINITION',
		'format_specifier_style': 'SCE_VB_FORMAT_SPECIFIER',
		'escape_char_start': NoEscapeCharacter,
		'escape_char_style': 'SCE_VB_ESCAPECHAR',
		'none_quote_style': 'SCE_VB_COMMENTLINE',
		'operator_style': ['SCE_VB_OPERATOR', 'SCE_VB_OPERATOR2'],
		#'auto_ident_word_style': ['SCE_VB_KEYWORD'],
		'string_style_range': ['SCE_VB_STRING', 'SCE_VB_PLACEHOLDER'],
	},
	'NP2LEX_VERILOG': {
		'cpp_style_comment': True,
		'comment_style_marker': 'SCE_V_TASKMARKER',
		'none_quote_style': 'SCE_V_NUMBER',
		'printf_format_specifier': True,
		'format_specifier_style': 'SCE_V_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_V_ESCAPECHAR',
		'operator_style': ['SCE_V_OPERATOR'],
		'extra_word_char': '$:',
		#'auto_ident_word_style': ['SCE_V_FOLDING_KEYWORD'],
		'string_style_range': ['SCE_V_STRING', 'SCE_V_FORMAT_SPECIFIER'],
	},
	'NP2LEX_VHDL': {
		'default_encoding': 'iso-8859-1',
		'line_comment_string': '--',
		'block_comment_string': ('/*', '*/'),
		'comment_style_marker': 'SCE_VHDL_TASKMARKER',
		'escape_char_start': NoEscapeCharacter,
		'character_style': ['SCE_VHDL_CHARACTER'],
		'raw_string_style': ['SCE_VHDL_STRING'],
		'operator_style': ['SCE_VHDL_OPERATOR', 'SCE_VHDL_OPERATOR2'],
		#'auto_ident_word_style': ['SCE_VHDL_FOLDING_KEYWORD'],
		'string_style_range': ['SCE_VHDL_CHARACTER', 'SCE_VHDL_STRING'],
	},
	'NP2LEX_VIM': {
		'line_comment_string': ('"', '#'),
		'comment_style_marker': 'SCE_VIM_TASKMARKER',
		'escape_char_style': 'SCE_YAML_ESCAPECHAR',
		'raw_string_style': ['SCE_VIM_STRING_SQ'],
		'operator_style': ['SCE_VIM_OPERATOR'],
		'string_style_range': ['SCE_VIM_STRING_DQ', 'SCE_VIM_REGEX'],
	},
	'NP2LEX_VISUALBASIC': {
		'line_comment_string': "'",
		'comment_style_marker': 'SCE_VB_COMMENTLINE',
		'default_fold_level': ['namespace', 'class', 'function'],
		'default_fold_ignore_inner': 'SCE_VB_FUNCTION_DEFINITION',
		'format_specifier_style': 'SCE_VB_FORMAT_SPECIFIER',
		'escape_char_start': NoEscapeCharacter,
		'escape_char_style': 'SCE_VB_ESCAPECHAR',
		'none_quote_style': 'SCE_VB_COMMENTLINE',
		'operator_style': ['SCE_VB_OPERATOR', 'SCE_VB_OPERATOR2'],
		'cpp_preprocessor': True,
		#'auto_ident_word_style': ['SCE_VB_KEYWORD', 'SCE_VB_PREPROCESSOR'],
		'string_style_range': ['SCE_VB_STRING', 'SCE_VB_PLACEHOLDER'],
	},

	'NP2LEX_WASM': {
		'line_comment_string': ';;',
		'block_comment_string': ('(;', ';)'),
		'comment_style_marker': 'SCE_WASM_TASKMARKER',
		'escape_char_style': 'SCE_WASM_ESCAPECHAR',
		'operator_style': ['SCE_WASM_OPERATOR'],
		'extra_word_char': '-@%$',
		'string_style_range': ['SCE_WASM_STRING', 'SCE_WASM_ESCAPECHAR'],
	},
	'NP2LEX_WINHEX': {
		'line_comment_string': '//',
		'comment_style_marker': 'SCE_WINHEX_COMMENTLINE',
		'operator_style': ['SCE_WINHEX_OPERATOR'],
		'escape_char_style': 'SCE_WINHEX_ESCAPECHAR',
		'escape_punctuation': True,
		'extra_word_char': '-',
		'string_style_range': ['SCE_WINHEX_STRING', 'SCE_WINHEX_ESCAPECHAR'],
	},
	'NP2LEX_XML': {
		'line_comment_string': ['//', "'", '#'],
		'block_comment_string': [('<!--', '-->'), ('/*', '*/'), ('--', '--')],
		'comment_style_list': ['SCE_H_COMMENT', 'SCE_H_XCCOMMENT', 'SCE_H_SGML_COMMENT', 'SCE_H_SGML_1ST_PARAM_COMMENT',
			'SCE_HJ_COMMENT', 'SCE_HJ_COMMENTLINE', 'SCE_HJ_COMMENTDOC',
			'SCE_HJA_COMMENT', 'SCE_HJA_COMMENTLINE', 'SCE_HJA_COMMENTDOC',
			'SCE_HB_COMMENTLINE', 'SCE_HBA_COMMENTLINE'],
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
		'escape_char_start': NoEscapeCharacter,
		'extra_word_char': '-:',
		'string_style_list': ['SCE_H_DOUBLESTRING', 'SCE_H_SINGLESTRING', 'SCE_H_SGML_DOUBLESTRING', 'SCE_H_SGML_SIMPLESTRING',
			'SCE_HJ_DOUBLESTRING', 'SCE_HJ_SINGLESTRING', 'SCE_HJ_REGEX', 'SCE_HJ_TEMPLATELITERAL',
			'SCE_HJA_DOUBLESTRING', 'SCE_HJA_SINGLESTRING', 'SCE_HJA_REGEX', 'SCE_HJA_TEMPLATELITERAL',
			'SCE_HB_STRING', 'SCE_HBA_STRING'],
		'plain_text_style': ['SCE_H_DEFAULT'],
	},
	'NP2LEX_YAML': {
		'tab_settings': TabSettings_Space2,
		'line_comment_string': '#',
		'comment_style_marker': 'SCE_YAML_COMMENT',
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
		'escape_char_style': 'SCE_YAML_ESCAPECHAR',
		'operator_style': ['SCE_YAML_OPERATOR'],
	},
	'NP2LEX_ZIG': {
		'line_comment_string': '//',
		'comment_style_marker': 'SCE_ZIG_TASKMARKER',
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_ZIG_FUNCTION_DEFINITION',
		#'printf_format_specifier': True,
		'format_specifier_style': 'SCE_ZIG_FORMAT_SPECIFIER',
		'escape_char_style': 'SCE_ZIG_ESCAPECHAR',
		'raw_string_style': ['SCE_ZIG_MULTISTRING'],
		'character_style': ['SCE_ZIG_CHARACTER'],
		'operator_style': ['SCE_ZIG_OPERATOR'],
		'string_style_range': ['SCE_ZIG_CHARACTER', 'SCE_ZIG_FORMAT_SPECIFIER'],
	},
}

def get_enum_flag_expr(flag, merge=True, separator='_'):
	cls = flag.__class__
	prefix = cls.__name__ + separator
	if name := flag.name:
		if '|' in name:
			# Python 3.11
			result = [prefix + item.strip() for item in name.split('|')]
			return ' | '.join(result) if merge else result
		return prefix + name

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

def escape_c_string(s):
	return ''.join(escape_c_char(ch) for ch in s)

def quote_c_char(ch):
	return "'" + escape_c_char(ch) + "'"

def MergeSwitchCaseList(caseList):
	codeMap = {}
	for label, code in sorted(caseList.items()):
		if code in codeMap:
			codeMap[code].append(label)
		else:
			codeMap[code] = [label]

	output = []
	indent = '\t'
	for code, label in codeMap.items():
		if 'default' in label:
			output.append(indent + 'default:')
		else:
			output.extend(f'{indent}case {key}:' for key in label)
		output.extend(code)
	return output


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

	# line comment, block comment
	if not config.get('cpp_style_comment', False):
		if not config.get('line_comment_string', None):
			flag |= LexerAttr.NoLineComment
		if not config.get('block_comment_string', None):
			flag |= LexerAttr.NoBlockComment

	# indentation guide
	indent_based_folding = config.get('indent_based_folding', False)
	if indent_based_folding:
		flag |= LexerAttr.IndentBasedFolding
	style = config.get('indent_guide_style', '')
	if style == 'forward':
		flag |= LexerAttr.IndentLookForward

	# styles
	if config.get('printf_format_specifier', False):
		flag |= LexerAttr.PrintfFormatSpecifier
	if config.get('angle_bracket_generic', False):
		flag |= LexerAttr.AngleBracketGeneric
	if config.get('cpp_preprocessor', False):
		flag |= LexerAttr.CppPreprocessor
	if config.get('character_prefix', None):
		flag |= LexerAttr.CharacterPrefix
	if config.get('escape_punctuation', None):
		flag |= LexerAttr.EscapePunctuation
	if config.get('plain_text_file', None):
		flag |= LexerAttr.PlainTextFile

	output = []
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

	# escape character, format specifier
	start = config.get('escape_char_start', '\\')
	style = config.get('escape_char_style', '0')
	assert len(start) == 1, (rid, style, start)
	output.append(f"{indent}'{escape_c_char(start)}', {style}, {config.get('format_specifier_style', '0')},")
	# auto ident word style
	styles = config.get('auto_ident_word_style', ['0'])
	output.append(f"{indent}{styles[0]},")
	# character style, none single quoted style
	styles = config.get('character_style', ['0'])
	output.append(f"{indent}{styles[0]}, {config.get('none_quote_style', '0')},")
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

	# style marker
	style = config.get('comment_style_marker', '0')
	output.append(f"{indent}, {style},")
	styles = config.get('string_style_range', ['0', '0'])
	assert len(styles) == 2, rid
	output.append(f"{indent}{styles[0]}, {styles[1]},")

	if not rid:
		output[-1] = output[-1][:-1]
		# right align continuation backslash in multi-line macro
		indent_count = len(indent)
		max_width = max(len(line) for line in output) - indent_count
		max_width = (max_width + 8) & ~3
		result = ['#define SCHEME_SETTINGS_DEFAULT\t\\']
		for index, line in enumerate(output):
			width = len(line) - indent_count
			width = (width + 4) & ~3
			padding = (max_width - width) // 4
			result.append(line + '\t'*padding + '\\')
		output = result
	return output

def BuildLexerCommentString():
	scriptShebang = {}
	complexShebang = []
	commentLine = {}
	complexLine = []
	commentBlock = {}
	complexBlock = []
	indent = '\t\t'

	for rid, config in LexerConfigMap.items():
		if name := config.get('shebang_exe_name', None):
			if isinstance(name, str):
				code = (f'{indent}name = "{escape_c_string(name)}";', indent + 'break;', '')
				scriptShebang[rid] = code
			else:
				complexShebang.append(rid)

		if config.get('cpp_style_comment', False):
			line_comment_string = '//'
			block_comment_string = ('/*', '*/')
		else:
			line_comment_string = config.get('line_comment_string', None)
			block_comment_string = config.get('block_comment_string', None)

		if line_comment_string:
			if any(item == '\'' for item in line_comment_string):
				print('single quote comment:', rid, line_comment_string)
			if isinstance(line_comment_string, list) and len(line_comment_string) == 1:
				line_comment_string = line_comment_string[0]
			if isinstance(line_comment_string, str):
				start = config.get('line_comment_at_line_start', False)
				argument = ' lineStart = true;' if start else ''
				start = escape_c_string(line_comment_string)
				code = (f'{indent}pwszComment = L"{start}";{argument}', indent + 'break;', '')
				commentLine[rid] = code
			else:
				complexLine.append(rid)

		if block_comment_string:
			if isinstance(block_comment_string, list) and len(block_comment_string) == 1:
				block_comment_string = block_comment_string[0]
			if isinstance(block_comment_string, tuple):
				assert len(block_comment_string) == 2, (rid, block_comment_string)
				newline = config.get('block_comment_on_new_line', False)
				suffix = ' newLine = true;' if newline else ''
				start, end = escape_c_string(block_comment_string[0]), escape_c_string(block_comment_string[1])
				code = (f'{indent}pwszOpen = L"{start}"; pwszClose = L"{end}";{suffix}', indent + 'break;', '')
				commentBlock[rid] = code
			else:
				complexBlock.append(rid)

	commentLine = MergeSwitchCaseList(commentLine)
	commentBlock = MergeSwitchCaseList(commentBlock)
	scriptShebang = MergeSwitchCaseList(scriptShebang)
	if complexLine:
		print('complex line comment:', ', '.join(sorted(complexLine)))
	if complexBlock:
		print('complex block comment:', ', '.join(sorted(complexBlock)))
	if complexShebang:
		print('complex script shebang:', ', '.join(sorted(complexShebang)))
	return commentLine, commentBlock, scriptShebang

def BuildAutoCompletionCache():
	cache = {}
	indent = '\t\t'

	def make_char_set(table, ch):
		quoted = quote_c_char(ch)
		suffix = 'U' if (ord(ch) & 31) == 31 else ''
		return f"{indent}{table}[{quoted} >> 5] |= (1{suffix} << ({quoted} & 31));"

	def make_bit_set(table, value):
		return f"{indent}{table}[{value} >> 5] |= (1U << ({value} & 31));"

	def make_all_char_set(output, table, string):
		assert len(string) == len(set(string)), (table, string)
		for ch in sorted(string):
			output.append(make_char_set(table, ch))

	def make_all_bit_set(output, table, values):
		assert len(values) == len(set(values)), (table, values)
		for value in values:
			output.append(make_bit_set(table, value))

	for rid, config in LexerConfigMap.items():
		output = []
		if word := config.get('extra_word_char', None):
			assert '.' not in word, (rid, word)
			make_all_char_set(output, 'CurrentWordCharSet', word)

		if word := config.get('character_prefix', None):
			prefix = [item for item in word if len(item) == 1]
			if len(prefix) != len(word):
				print('multiple character prefix:', rid)
			make_all_char_set(output, 'CharacterPrefixMask', prefix)

		if styles := config.get('raw_string_style', None):
			make_all_bit_set(output, 'RawStringStyleMask', styles)
		if styles := config.get('generic_type_style', None):
			make_all_bit_set(output, 'GenericTypeStyleMask', styles)
		if styles := config.get('ignore_word_style', None):
			make_all_bit_set(output, 'IgnoreWordStyleMask', styles)
		if styles := config.get('comment_style_list', None):
			make_all_bit_set(output, 'CommentStyleMask', styles)
		if styles := config.get('string_style_list', None):
			make_all_bit_set(output, 'AllStringStyleMask', styles)
		if styles := config.get('plain_text_style', None):
			make_all_bit_set(output, 'PlainTextStyleMask', styles)
		if rid == 'NP2LEX_MARKDOWN':
			for i in range(1, 4):
				output.append(f'{indent}PlainTextStyleMask[{i}] = UINT32_MAX;')

		if word := config.get('autoc_extra_keyword', None):
			output.append(f'{indent}np2_LexKeyword = &{word};')

		if output:
			output.append(indent + 'break;')
			output.append('')
			cache[rid] = tuple(output)

	cache['default'] = (indent + 'break;', '')
	output = MergeSwitchCaseList(cache)
	return output
