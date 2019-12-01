#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_VHDL = {{
"access after alias all architecture array assert attribute begin block body buffer bus case component "
"configuration constant disconnect downto else elsif end entity exit file for function generate generic "
"group guarded if impure in inertial inout is label library linkage literal loop map new next null of "
"on open others out package port postponed procedure process pure range record register reject report "
"return select severity shared signal subtype then to transport type unaffected units until use variable "
"wait when while with"

, // 1 Operator
"abs and mod nand nor not or rem rol ror sla sll sra srl xnor xor"

, // 2 Attribute
"active ascending base delayed driving driving_value event high image instance_name last_active last_event "
"last_value left leftof length low path_name pred pos quiet range reverse_range right rightof simple_name "
"stable transaction value val succ"

, // 3 Function
"endfile falling_edge is_x now read readline resize resolved rising_edge rotate_left rotate_right "
"shift_left shift_right std_match to_bit to_bitvector to_integer to_signed to_stdlogicvector "
"to_stdulogic to_stdulogicvector to_UX01 to_unsigned to_x01 to_x01z to_01 writeline write "

, // 4 Package
"ieee numeric_bit numeric_std math_complex math_real std std_logic_arith std_logic_misc "
"std_logic_signed std_logic_textio std_logic_unsigned std_logic_1164 "
"standard textio vital_primitives vital_timing work"

, // 5 Type
"bit bit_vector boolean character delay_length file_open_kind file_open_status integer line natural positive "
"real severity_level side signed std_logic std_logic_vector std_ulogic std_ulogic_vector string text time "
"unsigned UX01 UX01Z width X01 X01Z"

, // 6 User
NULL
, // 7 User2
NULL

, // 8 Fold
"architecture begin case component else elsif end entity generate loop package process record then procedure function when"

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_VHDL[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_VHDL_KEYWORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ MULTI_STYLE(SCE_VHDL_COMMENT, SCE_VHDL_COMMENTLINEBANG, SCE_VHDL_BLOCK_COMMENT, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_VHDL_STRING, NP2StyleX_String, L"fore:#008000" },
	{ SCE_VHDL_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_VHDL_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
	{ SCE_VHDL_ATTRIBUTE, NP2StyleX_Attribute, L"fore:#8080FF" },
	{ SCE_VHDL_STDPACKAGE, NP2StyleX_StdPackage, L"bold; fore:#0000FF" },
	{ SCE_VHDL_STDOPERATOR, NP2StyleX_StdOperator, L"fore:#0080C0" },
	{ SCE_VHDL_STDFUNCTION, NP2StyleX_StdFunction, L"bold; fore:#0080C0" },
	{ SCE_VHDL_STDTYPE, NP2StyleX_StdType, L"bold; fore:#FF8000" },
	{ SCE_VHDL_USERWORD, NP2StyleX_UserDefine, L"fore:#B5E71F" },
};

EDITLEXER lexVHDL = {
	SCLEX_VHDL, NP2LEX_VHDL,
	EDITLEXER_HOLE(L"VHDL Source", Styles_VHDL),
	L"vhd; vhdl",
	&Keywords_VHDL,
	Styles_VHDL
};
