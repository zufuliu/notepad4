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
	},
	'NP2LEX_2NDTEXTFILE': {
		'has_line_comment': False,
		'has_block_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
	},
	'NP2LEX_ANSI': {
		'has_line_comment': False,
		'has_block_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
	},

	'NP2LEX_ABAQUS': {
		'has_block_comment': False,
	},
	'NP2LEX_ACTIONSCRIPT': {
		'default_fold_level': ['class', 'anonymous object', 'method'],
		'default_fold_ignore_inner': 'SCE_JS_FUNCTION_DEFINITION',
	},
	'NP2LEX_APDL': {
		'has_block_comment': False,
	},
	'NP2LEX_ASYMPTOTE': {
		'default_fold_level': ['struct', 'function'],
		'default_fold_ignore_inner': 'SCE_ASY_FUNCTION_DEFINITION',
	},
	'NP2LEX_AUTOHOTKEY': {
		'default_fold_level': ['class', 'function'],
	},
	'NP2LEX_AVISYNTH': {
		'default_fold_level': ['function'],
	},
	'NP2LEX_AWK': {
		'has_block_comment': False,
		'default_fold_level': ['namespace', 'function'],
		'default_fold_ignore_inner': 'SCE_AWK_FUNCTION_DEFINITION',
	},

	'NP2LEX_BASH': {
		'has_block_comment': False,
	},
	'NP2LEX_BATCH': {
		'has_block_comment': False,
	},
	'NP2LEX_BLOCKDIAG': {
		'default_fold_level': ['graph', 'subgraph'],
	},

	'NP2LEX_COFFEESCRIPT': {
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['class', 'function'],
	},
	'NP2LEX_CONFIG': {
		'has_block_comment': False,
	},
	'NP2LEX_CPP': {
		'default_fold_level': ['preprocessor', 'namespace', 'class', 'method'],
	},
	'NP2LEX_CSHARP': {
		'default_fold_level': ['namespace', 'class', 'method'],
		'default_fold_ignore_inner': 'SCE_CSHARP_FUNCTION_DEFINITION',
	},

	'NP2LEX_DLANG': {
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_D_FUNCTION_DEFINITION',
	},
	'NP2LEX_DART': {
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_DART_FUNCTION_DEFINITION',
	},
	'NP2LEX_DIFF': {
		'has_line_comment': False,
		'has_block_comment': False,
		'default_fold_level': ['command', '[file]', 'diff'],
	},

	'NP2LEX_GN': {
		'has_block_comment': False,
	},
	'NP2LEX_GO': {
		'default_fold_level': ['struct', 'function'],
		'default_fold_ignore_inner': 'SCE_GO_FUNCTION_DEFINITION',
	},
	'NP2LEX_GRADLE': {
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_GROOVY_FUNCTION_DEFINITION',
	},
	'NP2LEX_GRAPHVIZ': {
		'default_fold_level': ['graph', 'subgraph'],
	},
	'NP2LEX_GROOVY': {
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_GROOVY_FUNCTION_DEFINITION',
	},

	'NP2LEX_HAXE': {
		'default_fold_level': ['class', 'method'],
		'default_fold_ignore_inner': 'SCE_HAXE_FUNCTION_DEFINITION',
	},
	'NP2LEX_HTML': {
		'tab_settings': TabSettings_Space2,
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
	},

	'NP2LEX_INI': {
		'has_block_comment': False,
		'default_fold_level': ['section', 'comment'],
	},
	'NP2LEX_INNOSETUP': {
		'default_fold_level': ['section', 'code'],
	},

	'NP2LEX_JAVA': {
		'default_fold_level': ['class', 'inner class', 'method'],
		'default_fold_ignore_inner': 'SCE_JAVA_FUNCTION_DEFINITION',
	},
	'NP2LEX_JAVASCRIPT': {
		'default_fold_level': ['class', 'anonymous object', 'method'],
		'default_fold_ignore_inner': 'SCE_JS_FUNCTION_DEFINITION',
	},
	'NP2LEX_JSON': {
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
	},
	'NP2LEX_JULIA': {
		'default_fold_level': ['struct', 'method'],
		'default_fold_ignore_inner': 'SCE_JULIA_FUNCTION_DEFINITION',
	},

	'NP2LEX_KOTLIN': {
		'default_fold_level': ['class', 'inner class', 'method'],
		'default_fold_ignore_inner': 'SCE_KOTLIN_FUNCTION_DEFINITION',
	},

	'NP2LEX_LLVM': {
		'has_block_comment': False,
	},
	'NP2LEX_LUA': {
		'default_fold_level': ['class', 'function'],
	},

	'NP2LEX_MAKEFILE': {
		'tab_settings': TabSettings_Tab4,
		'has_block_comment': False,
	},
	'NP2LEX_MARKDOWN': {
		'has_line_comment': False,
		'default_fold_level': ['header1', 'header2'],
	},

	'NP2LEX_NSIS': {
		'default_fold_level': ['section', 'function'],
	},

	'NP2LEX_PERL': {
		'has_block_comment': False,
	},
	'NP2LEX_PHP': {
		'default_fold_level': ['[php tag]', 'class', 'method'],
		'default_fold_ignore_inner': 'SCE_PHP_FUNCTION_DEFINITION',
	},
	'NP2LEX_PYTHON': {
		'has_block_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_PY_FUNCTION_DEFINITION',
	},

	'NP2LEX_RLANG': {
		'default_fold_level': ['function'],
	},
	'NP2LEX_RESOURCESCRIPT': {
		'default_fold_level': ['preprocessor', 'resource'],
	},
	'NP2LEX_RUBY': {
		'has_block_comment': False,
		'default_fold_level': ['module', 'class', 'method'],
		'default_fold_ignore_inner': 'SCE_RB_DEF_NAME',
	},
	'NP2LEX_RUST': {
		'default_fold_level': ['struct', 'function'],
		'default_fold_ignore_inner': 'SCE_RUST_FUNCTION_DEFINITION',
	},

	'NP2LEX_SCALA': {
		'default_fold_level': ['class', 'inner class', 'method'],
	},
	'NP2LEX_SMALI': {
		'has_block_comment': False,
		'default_fold_level': ['.method', '.switch'],
	},
	'NP2LEX_SQL': {
		'default_fold_level': ['function'],
	},
	'NP2LEX_SWIFT': {
		'default_fold_level': ['class', 'function'],
		'default_fold_ignore_inner': 'SCE_SWIFT_FUNCTION_DEFINITION',
	},

	'NP2LEX_TEXINFO': {
		'has_block_comment': False,
	},
	'NP2LEX_TOML': {
		'has_block_comment': False,
		'default_fold_level': ['table', 'comment'],
	},
	'NP2LEX_TYPESCRIPT': {
		'default_fold_level': ['class', 'anonymous object', 'method'],
		'default_fold_ignore_inner': 'SCE_JS_FUNCTION_DEFINITION',
	},

	'NP2LEX_VBSCRIPT': {
		'has_block_comment': False,
		'default_fold_level': ['function'],
	},
	'NP2LEX_VIM': {
		'has_block_comment': False,
	},
	'NP2LEX_VISUALBASIC': {
		'has_block_comment': False,
		'default_fold_level': ['class', 'function'],
	},

	'NP2LEX_XML': {
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
	},
	'NP2LEX_YAML': {
		'tab_settings': TabSettings_Space2,
		'has_block_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
		'default_fold_level': ['level1', 'level2', 'level13', 'level4'],
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

	prefix = '' if rid else '#define SCHEME_SETTINGS_DEFAULT'
	output = [prefix + '\t{']
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
		indent_width = len(indent)
		max_width = max(len(line) for line in output) - indent_width
		max_width = (max_width + 8) & ~3
		result = []
		for index, line in enumerate(output):
			width = len(line)
			if index == 0:
				width -= indent_width*4
			else:
				width -= indent_width
			width = (width + 4) & ~3
			padding = (max_width - width) // 4
			result.append(line + '\t'*padding + '\\')
		output = result
		suffix = suffix[:-1]
	output.append(suffix)
	return output
