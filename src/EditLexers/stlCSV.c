#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_Csv = {{
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Csv[] = {
	{ SCE_CSV_COL_0, NP2StyleX_Keyword, L"fore:#9400D3" },
	{ SCE_CSV_COL_1, NP2StyleX_Keyword, L"fore:#1C01AF" },
	{ SCE_CSV_COL_2, NP2StyleX_Keyword, L"fore:#0162F3" },
	{ SCE_CSV_COL_3, NP2StyleX_Keyword, L"fore:#28A4FF" },
	{ SCE_CSV_COL_4, NP2StyleX_Keyword, L"fore:#01C2C2" },
	{ SCE_CSV_COL_5, NP2StyleX_Keyword, L"fore:#00D530" },
	{ SCE_CSV_COL_6, NP2StyleX_Keyword, L"fore:#80D500" },
	{ SCE_CSV_COL_7, NP2StyleX_Keyword, L"fore:#D3E401" },
	{ SCE_CSV_COL_8, NP2StyleX_Keyword, L"fore:#FE9901" },
	{ SCE_CSV_COL_9, NP2StyleX_Keyword, L"fore:#D90000" },
};

EDITLEXER lexCSV = {
	SCLEX_CSV, NP2LEX_CSV,
	{
		LexerAttr_NoLineComment |
		LexerAttr_NoBlockComment |
		LexerAttr_IndentBasedFolding |
		LexerAttr_IndentLookForward,
		TAB_WIDTH_4, INDENT_WIDTH_4,
		(1 << 1) | (1 << 2), // level1, level2
		0,
		'\0', 0, 0,
		0,
		0, 0,
		0, 0,
		KeywordAttr_Default
	},
	EDITLEXER_HOLE(L"CSV File", Styles_Csv),
	L"csv",
	&Keywords_Csv,
	Styles_Csv
};
