#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_NULL = {{
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Global[] = {
	{ STYLE_DEFAULT, NP2StyleX_DefaultCodeStyle, L"font:$(Code)" },
	{ STYLE_DEFAULT, NP2StyleX_DefaultTextStyle, L"font:$(Text)" },
	{ STYLE_LINENUMBER, NP2StyleX_MarginLineNumber, L"size:-2; fore:#2B91AF" },
	{ 0, NP2StyleX_MatchingBrace, L"fore:#02EA0A; alpha:50; outline:200" },
	{ 0, NP2StyleX_MatchingBraceError, L"fore:#FF0000; alpha:50; outline:200" },
	{ STYLE_CONTROLCHAR, NP2StyleX_ControlCharacter, L"size:-1" },
	{ STYLE_INDENTGUIDE, NP2StyleX_IndentationGuide, L"fore:#FF8000" },
	{ SC_ELEMENT_SELECTION_BACK, NP2StyleX_SelectedText, L"eolfilled; alpha:95; outline:50" },
	{ SC_ELEMENT_WHITE_SPACE, NP2StyleX_Whitespace, L"size:2; fore:#FF4000; alpha:65" },
	{ SC_ELEMENT_CARET_LINE_BACK, NP2StyleX_CurrentLine, L"size:2; fore:#C2C0C3; back:#FFFF00; alpha:90; outline:90" },
	{ SC_ELEMENT_CARET, NP2StyleX_Caret, L"back:#FF0000" },
	{ 0, NP2StyleX_IMEIndicator, L"fore:#108010" },
	{ SCI_SETEDGECOLOUR, NP2StyleX_LongLineMarker, L"fore:#FFC000" },
	{ SCI_SETEXTRAASCENT + SCI_SETEXTRADESCENT, NP2StyleX_ExtraLineSpacing, L"size:2" },
	{ 0, NP2StyleX_CodeFolding, L"" },
	{ 0, NP2StyleX_FoldingMarker, L"fore:#8080FF; back:#ADD8E6" },
	{ STYLE_FOLDDISPLAYTEXT, NP2StyleX_FoldEllipsis, L"font:$(Code); size:-2; bold; fore:#808080" },
	{ 0, NP2StyleX_MarkOccurrences, L"alpha:100; outline:150" },
	{ 0, NP2StyleX_Bookmark, L"fore:#408040; back:#00FF00; alpha:40" },
	{ STYLE_CALLTIP, NP2StyleX_CallTip, L"" },
	{ STYLE_LINK, NP2StyleX_Link, L"fore:#648000" },
};

static EDITSTYLE Styles_2ndGlobal[] = {
	{ STYLE_DEFAULT, NP2StyleX_DefaultCodeStyle, L"font:$(Code); fore:#D4D4D4; back:#1E1E1E" },
	{ STYLE_DEFAULT, NP2StyleX_DefaultTextStyle, L"font:$(Text); fore:#D4D4D4; back:#1E1E1E" },
	{ STYLE_LINENUMBER, NP2StyleX_MarginLineNumber, L"size:-2; fore:#A0A0A0; back:#2A2A2E" },
	{ 0, NP2StyleX_MatchingBrace, L"fore:#02EA0A; alpha:50; outline:150" },
	{ 0, NP2StyleX_MatchingBraceError, L"fore:#FF0000; alpha:50; outline:200" },
	{ STYLE_CONTROLCHAR, NP2StyleX_ControlCharacter, L"size:-1" },
	{ STYLE_INDENTGUIDE, NP2StyleX_IndentationGuide, L"fore:#605F63" },
	{ SC_ELEMENT_SELECTION_BACK, NP2StyleX_SelectedText, L"eolfilled; alpha:95; outline:50" },
	{ SC_ELEMENT_WHITE_SPACE, NP2StyleX_Whitespace, L"size:2; fore:#FF4000; alpha:65" },
	{ SC_ELEMENT_CARET_LINE_BACK, NP2StyleX_CurrentLine, L"size:2; fore:#C2C0C3; back:#FFFF00; alpha:25; outline:25" },
	{ SC_ELEMENT_CARET, NP2StyleX_Caret, L"fore:#FFFFFF; back:#00FF00" },
	{ 0, NP2StyleX_IMEIndicator, L"fore:#108010" },
	{ SCI_SETEDGECOLOUR, NP2StyleX_LongLineMarker, L"fore:#605F63" },
	{ SCI_SETEXTRAASCENT + SCI_SETEXTRADESCENT, NP2StyleX_ExtraLineSpacing, L"size:2" },
	{ 0, NP2StyleX_CodeFolding, L"fore:#FF8000" },
	{ 0, NP2StyleX_FoldingMarker, L"fore:#808080; back:#606060" },
	{ STYLE_FOLDDISPLAYTEXT, NP2StyleX_FoldEllipsis, L"font:$(Code); size:-2; bold; fore:#606060" },
	{ 0, NP2StyleX_MarkOccurrences, L"alpha:100; outline:150" },
	{ 0, NP2StyleX_Bookmark, L"fore:#408040; back:#00FF00; alpha:40" },
	{ STYLE_CALLTIP, NP2StyleX_CallTip, L"" },
	{ STYLE_LINK, NP2StyleX_Link, L"fore:#648000" },
};

EDITLEXER lexGlobal = {
	SCLEX_NULL, NP2LEX_GLOBAL,
	SCHEME_SETTINGS_DEFAULT,
	EDITLEXER_HOLE(L"Global Styles", Styles_Global),
	NULL,
	NULL,
	Styles_Global
};

EDITLEXER lex2ndGlobal = {
	SCLEX_NULL, NP2LEX_2NDGLOBAL,
	SCHEME_SETTINGS_DEFAULT,
	EDITLEXER_HOLE(L"2nd Global Styles", Styles_2ndGlobal),
	NULL,
	NULL,
	Styles_2ndGlobal
};

static EDITSTYLE Styles_Text[] = {
	EDITSTYLE_DEFAULT,
};

static EDITSTYLE Styles_2ndText[] = {
	EDITSTYLE_DEFAULT,
};

EDITLEXER lexTextFile = {
	SCLEX_NULL, NP2LEX_TEXTFILE,
	SCHEME_SETTINGS_DEFAULT,
	EDITLEXER_TEXT(L"Text File", Styles_Text),
	L"txt; text; wtx; doc",
	&Keywords_NULL,
	Styles_Text
};

EDITLEXER lex2ndTextFile = {
	SCLEX_NULL, NP2LEX_2NDTEXTFILE,
	SCHEME_SETTINGS_DEFAULT,
	EDITLEXER_HOLE(L"2nd Text File", Styles_2ndText),
	L"log; asc; map; bnf",
	&Keywords_NULL,
	Styles_2ndText
};

static EDITSTYLE Styles_ANSI[] = {
	// override font used in global styles to ensure line height is same as measured from STYLE_DEFAULT.
	// negative extra line spacing is required when using Direct2D, not required when using GDI.
	{ STYLE_DEFAULT, NP2StyleX_Default, L"font:Lucida Console" },
	{ STYLE_LINENUMBER, NP2StyleX_MarginLineNumber, L"font:Lucida Console" },
	{ SCI_SETEXTRAASCENT + SCI_SETEXTRADESCENT, NP2StyleX_ExtraLineSpacing, L"size:-1" },
	{ STYLE_FOLDDISPLAYTEXT, NP2StyleX_FoldEllipsis, L"font:Lucida Console" },
};

EDITLEXER lexANSI = {
	SCLEX_NULL, NP2LEX_ANSI,
	SCHEME_SETTINGS_DEFAULT,
	EDITLEXER_HOLE(L"ANSI Art", Styles_ANSI),
	L"nfo; diz",
	&Keywords_NULL,
	Styles_ANSI
};

static EDITSTYLE Styles_CONF[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_CONF_DIRECTIVE, NP2StyleX_Directive, L"fore:#003CE6" },
	{ SCE_CONF_COMMENT, NP2StyleX_Comment, L"fore:#008000" },
	{ SCE_CONF_STRING, NP2StyleX_String, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_CONF_NUMBER, SCE_CONF_HEXNUM, 0, 0), NP2StyleX_Number, L"fore:#FF4000" },
	{ SCE_CONF_SECTION, NP2StyleX_Section, L"bold; back:#FFC040; eolfilled" },
	//{ SCE_CONF_IP, EDITSTYLE_HOLE(L"IP Address"), L"bold; fore:#FF4000" },
	//{ SCE_CONF_IDENTIFIER, EDITSTYLE_HOLE(L"Identifier"), L"" },
	{ SCE_CONF_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
	//{ SCE_CONF_PARAMETER, EDITSTYLE_HOLE(L"Runtime Directive Parameter"), L"" },
	//{ SCE_CONF_EXTENSION, EDITSTYLE_HOLE(L"Extension"), L"" },
};

EDITLEXER lexCONF = {
	SCLEX_CONF, NP2LEX_CONF,
	SCHEME_SETTINGS_DEFAULT,
	EDITLEXER_HOLE(L"Config File", Styles_CONF),
	L"conf; cfg; cnf; htaccess; properties; prefs; iface; prop; po; te",
	&Keywords_NULL,
	Styles_CONF
};

static EDITSTYLE Styles_DIFF[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_DIFF_COMMENT, NP2StyleX_Comment, L"fore:#008000" },
	{ SCE_DIFF_COMMAND, NP2StyleX_Command, L"bold; fore:#0A246A" },
	{ SCE_DIFF_HEADER, NP2StyleX_SourceDestination, L"fore:#C80000; back:#FFF1A8; eolfilled" },
	{ SCE_DIFF_POSITION, NP2StyleX_PositionSetting, L"fore:#0000FF" },
	{ MULTI_STYLE(SCE_DIFF_ADDED, SCE_DIFF_PATCH_ADD, SCE_DIFF_REMOVED_PATCH_ADD, 0), NP2StyleX_LineAddition, L"fore:#002000; back:#80FF80; eolfilled" },
	{ MULTI_STYLE(SCE_DIFF_DELETED, SCE_DIFF_PATCH_DELETE, SCE_DIFF_REMOVED_PATCH_DELETE, 0), NP2StyleX_LineRemoval, L"fore:#200000; back:#FF8080; eolfilled" },
	{ SCE_DIFF_CHANGED, NP2StyleX_LineChange, L"fore:#000020; back:#8080FF; eolfilled" },
};

EDITLEXER lexDIFF = {
	SCLEX_DIFF, NP2LEX_DIFF,
	SCHEME_SETTINGS_DEFAULT,
	EDITLEXER_HOLE(L"Diff File", Styles_DIFF),
	L"diff; patch",
	&Keywords_NULL,
	Styles_DIFF
};

static EDITSTYLE Styles_INI[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_PROPS_KEY, NP2StyleX_Property, L"fore:#A46000" },
	{ SCE_PROPS_COMMENT, NP2StyleX_Comment, L"fore:#008000" },
	{ SCE_PROPS_SECTION, NP2StyleX_Section, L"bold; back:#FFC040; eolfilled" },
	{ SCE_PROPS_ASSIGNMENT, NP2StyleX_Assignment, L"fore:#FF0000" },
	{ SCE_PROPS_DEFVAL, NP2StyleX_DefaultValue, L"fore:#FF0000" },
};

EDITLEXER lexINI = {
	SCLEX_PROPERTIES, NP2LEX_INI,
	SCHEME_SETTINGS_DEFAULT,
	EDITLEXER_HOLE(L"Ini Config File", Styles_INI),
	L"ini; inf; reg; oem; sif; url; sed; theme; clw; abnf",
	&Keywords_NULL,
	Styles_INI
};

static KEYWORDLIST Keywords_Markdown = {{
//markdown++Autogenerated -- start of section automatically generated
"address article aside base basefont blockquote body caption center col colgroup dd details dialog dir div dl dt "
"fieldset figcaption figure footer form frame frameset h1 h2 h3 h4 h5 h6 head header hr html iframe legend li link "
"main menu menuitem nav noframes ol optgroup option p param pre script section source style summary "
"table tbody td textarea tfoot th thead title tr track ul "

, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
//markdown--Autogenerated -- end of section automatically generated
}};

static EDITSTYLE Styles_Markdown[] = {
	EDITSTYLE_DEFAULT,
	{ MULTI_STYLE(SCE_MARKDOWN_HEADER1, SCE_MARKDOWN_SETEXT_H1, 0, 0), NP2StyleX_Header1, L"bold; fore:#0080FF" },
	{ MULTI_STYLE(SCE_MARKDOWN_HEADER2, SCE_MARKDOWN_SETEXT_H2, 0, 0), NP2StyleX_Header2, L"bold; fore:#0066CC" },
	{ SCE_MARKDOWN_HEADER3, NP2StyleX_Header3, L"bold; fore:#0080C0" },
	{ SCE_MARKDOWN_HEADER4, NP2StyleX_Header4, L"bold; fore:#007F7F" },
	{ SCE_MARKDOWN_HEADER5, NP2StyleX_Header5, L"bold; fore:#408080" },
	{ SCE_MARKDOWN_HEADER6, NP2StyleX_Header6, L"bold; fore:#408040" },
	{ MULTI_STYLE(SCE_MARKDOWN_METADATA_YAML, SCE_MARKDOWN_METADATA_TOML, SCE_MARKDOWN_METADATA_JSON, SCE_MARKDOWN_TITLE_BLOCK), NP2StyleX_Metadata, L"fore:#F08000" },
	{ SCE_MARKDOWN_HRULE, NP2StyleX_HorizontalRule, L"bold; back:#FFC040; eolfilled" },
	{ MULTI_STYLE(SCE_MARKDOWN_BLOCKQUOTE, SCE_MARKDOWN_MULTI_BLOCKQUOTE, 0, 0), NP2StyleX_BlockQuote, L"fore:#A46000; back:#FFFFC0; eolfilled" },
	{ MULTI_STYLE(SCE_MARKDOWN_ORDERED_LIST, SCE_MARKDOWN_EXT_ORDERED_LIST, 0, 0), NP2StyleX_OrderedList, L"bold; fore:#0080FF" },
	{ SCE_MARKDOWN_BULLET_LIST, NP2StyleX_UnorderedList, L"bold; fore:#0080FF" },
	{ SCE_MARKDOWN_DEFINITION_LIST, NP2StyleX_DefinitionList, L"bold; fore:#0080FF" },
	{ SCE_MARKDOWN_TASK_LIST, NP2StyleX_TaskList, L"bold; fore:#408040" },
	{ SCE_MARKDOWN_EXAMPLE_LIST, NP2StyleX_ExampleList, L"bold; fore:#408040" },
	{ MULTI_STYLE(SCE_MARKDOWN_INDENTED_BLOCK, SCE_MARKDOWN_BACKTICK_BLOCK, SCE_MARKDOWN_TILDE_BLOCK, 0), NP2StyleX_CodeBlock, L"back:#C5C5C5; eolfilled" },
	{ SCE_MARKDOWN_CODE_SPAN, NP2StyleX_InlineCode, L"back:#C5C5C5" },
	{ MULTI_STYLE(SCE_MARKDOWN_DISPLAY_MATH, SCE_MARKDOWN_BACKTICK_MATH, SCE_MARKDOWN_TILDE_MATH, 0), NP2StyleX_DisplayMath, L"back:#C5C5C5; eolfilled" },
	{ MULTI_STYLE(SCE_MARKDOWN_INLINE_MATH, SCE_MARKDOWN_INLINE_DISPLAY_MATH, SCE_MARKDOWN_MATH_SPAN, 0), NP2StyleX_InlineMath, L"back:#C5C5C5" },
	{ MULTI_STYLE(SCE_MARKDOWN_LINK_TEXT, SCE_MARKDOWN_LINK_TITLE_SQ, SCE_MARKDOWN_LINK_TITLE_DQ, SCE_MARKDOWN_LINK_TITLE_PAREN), NP2StyleX_LinkText, L"fore:#3A6EA5" },
	{ MULTI_STYLE(SCE_MARKDOWN_EM_ASTERISK, SCE_MARKDOWN_EM_UNDERSCORE, 0, 0), NP2StyleX_Emphasis, L"italic" },
	{ MULTI_STYLE(SCE_MARKDOWN_STRONG_ASTERISK, SCE_MARKDOWN_STRONG_UNDERSCORE, 0, 0), NP2StyleX_Strong, L"bold" },
	{ SCE_MARKDOWN_STRIKEOUT, NP2StyleX_Strikethrough, L"strike" },
	{ MULTI_STYLE(SCE_MARKDOWN_SUPERSCRIPT, SCE_MARKDOWN_SHORT_SUPERSCRIPT, 0, 0), NP2StyleX_Superscript, L"overline" },
	{ MULTI_STYLE(SCE_MARKDOWN_SUBSCRIPT, SCE_MARKDOWN_SHORT_SUBSCRIPT, 0, 0), NP2StyleX_Subscript, L"underline" },
	{ SCE_MARKDOWN_CITATION_AT, NP2StyleX_Citation, L"fore:#408040" },
	{ SCE_MARKDOWN_DELIMITER_ROW, NP2StyleX_DelimiterRow, L"bold; back:#FFC040; eolfilled" },
	{ SCE_MARKDOWN_DELIMITER, NP2StyleX_Delimiter, L"bold; fore:#0080C0" },
	{ SCE_MARKDOWN_EMOJI, NP2StyleX_Emoji, L"fore:#FF8000" },
	{ SCE_MARKDOWN_ESCAPECHAR, NP2StyleX_EscapeSequence, L"fore:#0080C0" },
	{ MULTI_STYLE(SCE_MARKDOWN_DIFF_ADD_CURLY, SCE_MARKDOWN_DIFF_ADD_SQUARE, 0, 0), NP2StyleX_LineAddition, L"back:#80FF80" },
	{ MULTI_STYLE(SCE_MARKDOWN_DIFF_DEL_CURLY, SCE_MARKDOWN_DIFF_DEL_SQUARE, 0, 0), NP2StyleX_LineRemoval, L"back:#FF8080" },
};

EDITLEXER lexMarkdown = {
	SCLEX_MARKDOWN, NP2LEX_MARKDOWN,
	SCHEME_SETTINGS_DEFAULT,
	EDITLEXER_HOLE(L"Markdown", Styles_Markdown),
	L"md; markdown; mdown; mkdn; mkd; litcoffee",
	&Keywords_Markdown,
	Styles_Markdown
};
