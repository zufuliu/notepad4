#include "EditLexer.h"
#include "EditStyle.h"

// smali smaliLexer.flex
// android src/dalvik/opcode-gen/bytecode.txt

static KEYWORDLIST Keywords_Smail = {{
"true false null "
"build runtime system "
"public private protected static final synchronized bridge varargs native abstract "
"strictfp synthetic constructor declared-synchronized class interface enum annotation volatile transient "
"no-error generic-error no-such-class no-such-field no-such-method "
"illegal-class-access illegal-field-access illegal-method-access "
"class-change-error instantiation-error "
"inline vtable field this "

// java keyword
"@interface assert break case catch continue default do else "
"extends finally for if implements import instanceof "
"package new return super switch throws try while "
"boolean byte char double float int long short void "

, // 1 Type Keyword
NULL
, // 2 Preprocessor
NULL
, // 3 Directive
NULL
, // 4 Attribute
NULL
, // 5 Class
NULL
, // 6 Interface
NULL
, // 7 Enumeration
NULL
, // 8 Constant
"init clinit value "

, // 9 Directive
"end class super implements source field subannotation annotation enum method "
"registers locals array-data packed-switch sparse-switch catch catchall line param local restart prologue epilogue "
"parameter "

, // 10 Instruction
// Regular opcodes
"nop "
"move move/from16 move/16 move-wide move-wide/from16 move-wide/16 "
"move-object move-object/from16 move-object/16 "
"move-result move-result-wide move-result-object "
"move-exception "
"return-void return return-wide return-object "
"const/4 const/16 const const/high16 "
"const-wide/16 const-wide/32 const-wide const-wide/high16 "
"const-string const-string/jumbo const-class "
"monitor-enter monitor-exit check-cast instance-of "
"array-length new-instance new-array filled-new-array filled-new-array/range fill-array-data "
"throw goto goto/16 goto/32 packed-switch sparse-switch "
"cmpl-float cmpg-float cmpl-double cmpg-double cmp-long "
"if-eq if-ne if-lt if-ge if-gt if-le if-eqz if-nez if-ltz if-gez if-gtz if-lez "
"aget aget-wide aget-object aget-boolean aget-byte aget-char aget-short "
"aput aput-wide aput-object aput-boolean aput-byte aput-char aput-short "
"iget iget-wide iget-object iget-boolean iget-byte iget-char iget-short "
"iput iput-wide iput-object iput-boolean iput-byte iput-char iput-short "
"sget sget-wide sget-object sget-boolean sget-byte sget-char sget-short "
"sput sput-wide sput-object sput-boolean sput-byte sput-char sput-short "
"invoke-virtual invoke-super invoke-direct invoke-static invoke-interface "
"invoke-virtual/range invoke-super/range invoke-direct/range invoke-static/range invoke-interface/range "
"neg-int not-int neg-long not-long neg-float neg-double "
"int-to-long int-to-float int-to-double long-to-int long-to-float long-to-double "
"float-to-int float-to-long float-to-double double-to-int double-to-long double-to-float "
"int-to-byte int-to-char int-to-short "
"add-int sub-int mul-int div-int rem-int and-int or-int xor-int shl-int shr-int ushr-int "
"add-long sub-long mul-long div-long rem-long and-long or-long xor-long shl-long shr-long ushr-long "
"add-float sub-float mul-float div-float rem-float add-double sub-double mul-double div-double rem-double "
"add-int/2addr sub-int/2addr mul-int/2addr div-int/2addr rem-int/2addr "
"and-int/2addr or-int/2addr xor-int/2addr shl-int/2addr shr-int/2addr ushr-int/2addr "
"add-long/2addr sub-long/2addr mul-long/2addr div-long/2addr rem-long/2addr "
"and-long/2addr or-long/2addr xor-long/2addr shl-long/2addr shr-long/2addr ushr-long/2addr "
"add-float/2addr sub-float/2addr mul-float/2addr div-float/2addr rem-float/2addr "
"add-double/2addr sub-double/2addr mul-double/2addr div-double/2addr rem-double/2addr "
"add-int/lit16 rsub-int mul-int/lit16 div-int/lit16 rem-int/lit16 "
"and-int/lit16 or-int/lit16 xor-int/lit16 "
"add-int/lit8 rsub-int/lit8 mul-int/lit8 div-int/lit8 rem-int/lit8 "
"and-int/lit8 or-int/lit8 xor-int/lit8 shl-int/lit8 shr-int/lit8 ushr-int/lit8 "

// Optimized opcodes
"iget-volatile iput-volatile sget-volatile sput-volatile iget-object-volatile iget-wide-volatile "
"iput-wide-volatile sget-wide-volatile sput-wide-volatile "
"breakpoint throw-verification-error "
"execute-inline execute-inline/range invoke-object-init/range return-void-barrier "
"iget-quick iget-wide-quick iget-object-quick iput-quick iput-wide-quick iput-object-quick "
"invoke-virtual-quick invoke-virtual-quick/range invoke-super-quick invoke-super-quick/range "
"iput-object-volatile sget-object-volatile sput-object-volatile "

// Extended-width opcodes
"const-class/jumbo check-cast/jumbo instance-of/jumbo new-instance/jumbo new-array/jumbo filled-new-array/jumbo "
"iget/jumbo iget-wide/jumbo iget-object/jumbo iget-boolean/jumbo iget-byte/jumbo iget-char/jumbo iget-short/jumbo "
"iput/jumbo iput-wide/jumbo iput-object/jumbo iput-boolean/jumbo iput-byte/jumbo iput-char/jumbo iput-short/jumbo "
"sget/jumbo sget-wide/jumbo sget-object/jumbo sget-boolean/jumbo sget-byte/jumbo sget-char/jumbo sget-short/jumbo "
"sput/jumbo sput-wide/jumbo sput-object/jumbo sput-boolean/jumbo sput-byte/jumbo sput-char/jumbo sput-short/jumbo "
"invoke-virtual/jumbo invoke-super/jumbo invoke-direct/jumbo invoke-static/jumbo invoke-interface/jumbo "

// Optimized opcodes
"invoke-object-init/jumbo "
"iget-volatile/jumbo iget-wide-volatile/jumbo iget-object-volatile/jumbo "
"iput-volatile/jumbo iput-wide-volatile/jumbo iput-object-volatile/jumbo "
"sget-volatile/jumbo sget-wide-volatile/jumbo sget-object-volatile/jumbo "
"sput-volatile/jumbo sput-wide-volatile/jumbo sput-object-volatile/jumbo "
"throw-verification-error/jumbo "

// Invalid: used for highlight
"invoke-object-init invoke-direct-empty "

, // 11 Instruction
"from16 high16 jumbo range 2addr lit16 lit8 "

, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Smail[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_SMALI_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"fore:#0000FF" },
	{ SCE_SMALI_DIRECTIVE, NP2STYLE_Directive, EDITSTYLE_HOLE(L"Directive"), L"fore:#FF8000" },
	{ SCE_SMALI_INSTRUCTION, NP2STYLE_Instruction, EDITSTYLE_HOLE(L"Instruction"), L"fore:#0080FF" },
	{ SCE_SMALI_REGISTER, NP2STYLE_Register, EDITSTYLE_HOLE(L"Register"), L"fore:#FF8000" },
	{ SCE_SMALI_TYPE, NP2STYLE_Type, EDITSTYLE_HOLE(L"Type"), L"bold; fore:#007F7F" },
	{ SCE_SMALI_FIELD, NP2STYLE_Field, EDITSTYLE_HOLE(L"Field"), L"fore:#648000" },
	{ SCE_SMALI_METHOD, NP2STYLE_Method, EDITSTYLE_HOLE(L"Method"), L"fore:#A46000" },
	{ SCE_SMALI_COMMENTLINE, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ MULTI_STYLE(SCE_SMALI_STRING, SCE_SMALI_CHARACTER, 0, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ MULTI_STYLE(SCE_SMALI_LABEL, SCE_SMALI_LABEL_EOL, 0, 0), NP2STYLE_Label, EDITSTYLE_HOLE(L"Label"), L"back:#FFC040; eolfilled" },
	{ SCE_SMALI_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_SMALI_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
};

EDITLEXER lexSmali = {
	SCLEX_SMALI, NP2LEX_SMALI,
	EDITLEXER_HOLE(L"Android Smali", Styles_Smail),
	L"smali; ddx",
	&Keywords_Smail,
	Styles_Smail
};

