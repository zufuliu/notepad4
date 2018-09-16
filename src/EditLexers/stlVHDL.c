#include "EditLexer.h"
#include "EditStyle.h"

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
""
, // 7 User2
""

, // 8 Fold
"architecture begin case component else elsif end entity generate loop package process record then procedure function when"

, "", "", "", "", "", "", ""
}};

EDITLEXER lexVHDL = { SCLEX_VHDL, NP2LEX_VHDL, EDITLEXER_HOLE, L"VHDL Source", L"vhd; vhdl", L"", &Keywords_VHDL,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	// { SCE_VHDL_DEFAULT, L"Default", L"", L"" },
	{ SCE_VHDL_KEYWORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ MULTI_STYLE(SCE_VHDL_COMMENT, SCE_VHDL_COMMENTLINEBANG, SCE_VHDL_BLOCK_COMMENT, 0), NP2STYLE_Comment, L"Comment", L"fore:#608060", L"" },
	{ SCE_VHDL_STRING, NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_VHDL_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_VHDL_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ SCE_VHDL_ATTRIBUTE, NP2STYLE_Attribute, L"Attribute", L"fore:#8080FF", L"" },
	{ SCE_VHDL_STDPACKAGE, 63581, L"STD Package", L"bold; fore:#0000FF", L"" },
	{ SCE_VHDL_STDOPERATOR, 63582, L"STD Operator", L"fore:#0080C0", L"" },
	{ SCE_VHDL_STDFUNCTION, 63583, L"STD Function", L"bold; fore:#0080C0", L"" },
	{ SCE_VHDL_STDTYPE, 63584, L"STD Type", L"bold; fore:#FF8000", L"" },
	{ SCE_VHDL_USERWORD, 63585, L"User Define", L"fore:#B5E71F", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
