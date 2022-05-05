from enum import IntFlag

def get_enum_flag_expr(flag, separator='_'):
	cls = flag.__class__
	prefix = cls.__name__ + separator
	if flag.name:
		return prefix + flag.name

	comb = []
	for value in cls.__members__.values():
		if flag & value:
			comb.append(prefix + value.name)
	return ' | '.join(comb)

class LexerAttr(IntFlag):
	Default = 0
	NoGlobalTabSettings = 1 << 0
	TabAsSpaces = 1 << 1
	NoLineComment = 1 << 2
	NoBlockComment = 1 << 3
	IndentBasedFolding = 1 << 4
	IndentLookForward = 1 << 5

TabSettings_Default = {
	'tab_width': 4,
	'indent_width': 4,
	'tab_as_space': False,
	'use_global_tab_settings': True,
}
TabSettings_Tab4 = {
	'tab_width': 4,
	'indent_width': 4,
	'tab_as_space': False,
	'use_global_tab_settings': False,
}
TabSettings_Space4 = {
	'tab_width': 4,
	'indent_width': 4,
	'tab_as_space': True,
	'use_global_tab_settings': False,
}
TabSettings_Space2 = {
	'tab_width': 2,
	'indent_width': 2,
	'tab_as_space': True,
	'use_global_tab_settings': False,
}

LexerConfigMap = {
	'NP2LEX_TEXTFILE': {
		'has_block_comment': False,
		'has_line_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
	},
	'NP2LEX_2NDTEXTFILE': {
		'has_block_comment': False,
		'has_line_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
	},
	'NP2LEX_ANSI': {
		'has_block_comment': False,
		'has_line_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
	},

	'NP2LEX_ABAQUS': {
		'has_block_comment': False,
	},
	'NP2LEX_APDL': {
		'has_block_comment': False,
	},
	'NP2LEX_AWK': {
		'has_block_comment': False,
	},

	'NP2LEX_BASH': {
		'has_block_comment': False,
	},
	'NP2LEX_BATCH': {
		'has_block_comment': False,
	},

	'NP2LEX_COFFEESCRIPT': {
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
	},
	'NP2LEX_CONF': {
		'has_block_comment': False,
	},

	'NP2LEX_DIFF': {
		'has_block_comment': False,
		'has_line_comment': False,
	},

	'NP2LEX_GN': {
		'has_block_comment': False,
	},

	'NP2LEX_HTML': {
		'tab_settings': TabSettings_Space2,
	},

	'NP2LEX_INI': {
		'has_block_comment': False,
	},

	'NP2LEX_LLVM': {
		'has_block_comment': False,
	},

	'NP2LEX_MAKE': {
		'has_block_comment': False,
		'tab_settings': TabSettings_Tab4,
	},
	'NP2LEX_MARKDOWN': {
		'has_line_comment': False,
	},

	'NP2LEX_PERL': {
		'has_block_comment': False,
	},
	'NP2LEX_PYTHON': {
		'has_block_comment': False,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
	},

	'NP2LEX_RUBY': {
		'has_block_comment': False,
	},

	'NP2LEX_SMALI': {
		'has_block_comment': False,
	},

	'NP2LEX_TEXINFO': {
		'has_block_comment': False,
	},
	'NP2LEX_TOML': {
		'has_block_comment': False,
	},

	'NP2LEX_VB': {
		'has_block_comment': False,
	},
	'NP2LEX_VBS': {
		'has_block_comment': False,
	},
	'NP2LEX_VIM': {
		'has_block_comment': False,
	},

	'NP2LEX_YAML': {
		'has_block_comment': False,
		'tab_settings': TabSettings_Space2,
		'indent_based_folding': True,
		'indent_guide_style': 'forward',
	},
}

def BuildLexerConfigContent(rid):
	config = LexerConfigMap.get(rid, {})
	if rid and not config:
		return []

	tab_settings = config.get('tab_settings', TabSettings_Default)
	flag = LexerAttr.Default
	if tab_settings['tab_as_space']:
		flag |= LexerAttr.TabAsSpaces
	if not tab_settings['use_global_tab_settings']:
		flag |= LexerAttr.NoGlobalTabSettings
	if not config.get('has_line_comment', True):
		flag |= LexerAttr.NoLineComment
	if not config.get('has_block_comment', True):
		flag |= LexerAttr.NoBlockComment
	if config.get('indent_based_folding', False):
		flag |= LexerAttr.IndentBasedFolding
	style = config.get('indent_guide_style', '')
	if style == 'forward':
		flag |= LexerAttr.IndentLookForward

	output = ['\t{']
	indent = '\t\t'

	expr = get_enum_flag_expr(flag)
	output.append(f'{indent}{expr},')
	output.append(f"{indent}TAB_WIDTH_{tab_settings['tab_width']}, INDENT_WIDTH_{tab_settings['indent_width']},")

	output[-1] = output[-1][:-1] # remove extra comma
	if rid:
		output.append('\t},')
	else:
		line = ' '.join(line.strip() for line in output)
		output = ['#define SCHEME_SETTINGS_DEFAULT\t\t\t' + line + ' }']
	return output
