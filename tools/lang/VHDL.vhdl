-- VHDL 2019

--! keywords			=======================================================
abs access after alias all and array attribute
begin body buffer bus
constant
disconnect downto else elsif ertial exit
file force
generate generic group guarded
impure in inertial inout is
label library linkage literal lkage
map mod nand
new next nor not null
of on open or others out
parameter port postponed private pure
range register reject release rem report return rol ror
select severity shared signal sla sll sra srl subtype
then to transport type
unaffected until use
variable
wait while with
xnor xor

-- PSL keywords
assert assume assume_guarantee
cover
default
fairness
property
restrict restrict_guarantee
sequence strong
vmode vpkg vprop vunit

--! code folding		=======================================================
-- Design entities and configurations
entity identifier is entity_header entity_declarative_part [begin entity_statement_part]
end [entity] [entity_simple_name];
architecture identifier of entity_name is architecture_declarative_part begin architecture_statement_part
end [architecture] [architecture_simple_name];
configuration identifier of entity_name is configuration_declarative_part
	for block_specification
	end for ;
end [configuration] [configuration_simple_name];
-- Subprograms and packages
function designator subprogram_header is subprogram_declarative_part begin subprogram_statement_part
end [function] [designator];
procedure designator subprogram_header is subprogram_declarative_part begin subprogram_statement_part
end [procedure] [designator];
function designator is new uninstantiated_subprogram_name [signature] [generic_map_aspect];
procedure designator is new uninstantiated_subprogram_name [signature] [generic_map_aspect];
package identifier is package_header package_declarative_part
end [package] [package_simple_name];
package body package_simple_name is package_body_declarative_part
end [package body] [package_simple_name];
package identifier is new uninstantiated_package_name [generic_map_aspect];
-- Types
units identifier; -- primary_unit_declaration
end units [physical_type_simple_name]
units <> -- physical_incomplete_type_definition
record element_declaration
end record [record_type_simple_name]
protected protected_type_header protected_type_declarative_part
end protected [protected_type_simple_name]
protected body protected_type_body_declarative_part
end protected body [protected_type_simple name]
-- Declarations
view mode_view_name [of unresolved_record_subtype_indication]; -- record_mode_view_indication
view identifier of unresolved_record_subtype_indication is
end view [mode_view_simple_name];
component identifier [is]
end [component] [component_simple_name];
-- Specifications
-- entity_specification ::= entity_name_list : entity_class
-- Sequential statements
if condition then
end if [if_label];
case [?] expression is
end case [?] [case_label];
loop sequential_statement_body
end loop [loop_label];
block [is] sequential_block_declarative_part begin sequential_block_statement_part
end [block] [sequential_block_label];
-- Concurrent statements
process [(process_sensitivity_list)] [is] process_declarative_part begin process_statement_part
end process [process_label]; postponed
process [(process_sensitivity_list)] [is] process_declarative_part begin process_statement_part
end postponed process [process_label];
when begin -- case_generate_alternative
end ;
-- Design units and their analysis
context identifier is context_clause
end [context] [context_simple_name];
context selected_name {, selected_name}; -- context_reference

--! directives			=======================================================
`protect
`if then
`elsif then
`else
`end [if]
`error
`warning

--! api					=======================================================
-- Predefined language environment
-- Predefined attributes of types and objects
'BASE
'LEFT
'RIGHT
'HIGH
'LOW
'ASCENDING
'LENGTH
'RANGE
'REVERSE_RANGE
'IMAGE
'SUBTYPE
'POS
'SUCC
'PRED
'LEFTOF
'RIGHTOF
'IMAGE(X)
'VALUE(X)
'POS(X)
'VAL(X)
'SUCC(X)
'PRED(X)
'LEFTOF(X)
'RIGHTOF(X)
'DESIGNATED_SUBTYPE
'REFLECT
-- Predefined attributes of arrays
'INDEX[(N)]
'ELEMENT
-- Predefined attributes of signals
'DELAYED[(T)]
'STABLE[(T)]
'QUIET[(T)]
'TRANSACTION
'EVENT
'ACTIVE
'LAST_EVENT
'LAST_ACTIVE
'LAST_VALUE
'DRIVING
'DRIVING_VALUE
-- Predefined attributes of named entities
'SIMPLE_NAME
'INSTANCE_NAME
'PATH_NAME
-- Predefined attributes of ranges
'RECORD
'VALUE
-- Predefined attributes of PSL Objects
'SIGNAL
-- Predefined attributes of named mode views
'CONVERSE

-- IEEE 1076: VHDL Packages https://opensource.ieee.org/vasg/Packages
package STANDARD is
	type DIRECTION is (ASCENDING, DESCENDING);
		function MINIMUM(L, R: DIRECTION) return DIRECTION;
		function MAXIMUM(L, R: DIRECTION) return DIRECTION;
	type BOOLEAN is (FALSE, TRUE);
		function RISING_EDGE(signal S: BOOLEAN) return BOOLEAN;
		function FALLING_EDGE(signal S: BOOLEAN) return BOOLEAN;
	type BIT is ();
	type CHARACTER is ();
	type SEVERITY_LEVEL is (NOTE, WARNING, ERROR, FAILURE);
	type INTEGER is range;
	type REAL is range;
	type TIME is range;
	subtype DELAY_LENGTH is TIME range 0 fs to TIME'HIGH;
	impure function NOW return DELAY_LENGTH;
	subtype NATURAL is INTEGER range 0 to INTEGER'HIGH;
	subtype POSITIVE is INTEGER range 1 to INTEGER'HIGH;
	type STRING is array (POSITIVE range <>) of CHARACTER;
	type BOOLEAN_VECTOR is array (NATURAL range <>) of BOOLEAN;
	type BIT_VECTOR is array (NATURAL range <>) of BIT;
		function TO_STRING(VALUE: BIT_VECTOR) return STRING;
		alias TO_BSTRING is TO_STRING [BIT_VECTOR return STRING];
		alias TO_BINARY_STRING is TO_STRING [BIT_VECTOR return STRING];
		function TO_OSTRING(VALUE: BIT_VECTOR) return STRING;
		alias TO_OCTAL_STRING is TO_OSTRING [BIT_VECTOR return STRING];
		function TO_HSTRING(VALUE: BIT_VECTOR) return STRING;
		alias TO_HEX_STRING is TO_HSTRING [BIT_VECTOR return STRING];
	type INTEGER_VECTOR is array (NATURAL range <>) of INTEGER;
	type REAL_VECTOR is array (NATURAL range <>) of REAL;
	type TIME_VECTOR is array (NATURAL range <>) of TIME;
	type FILE_OPEN_KIND is (READ_MODE, READ_WRITE_MODE, WRITE_MODE, APPEND_MODE);
	type FILE_OPEN_STATUS is (OPEN_OK, STATUS_ERROR, NAME_ERROR, MODE_ERROR);
	type FILE_OPEN_STATE is (STATE_OPEN, STATE_CLOSED);
	type FILE_ORIGIN_KIND is (FILE_ORIGIN_BEGIN, FILE_ORIGIN_CURRENT, FILE_ORIGIN_END);
	attribute FOREIGN: STRING;
end package STANDARD;

package TEXTIO is
	type LINE is access STRING;
		procedure DEALLOCATE(P: inout LINE);
	type LINE_VECTOR is array();
	type TEXT is file of STRING;
		procedure FILE_OPEN(file F: TEXT; External_Name; in STRING; Open_Kind: in FILE_OPEN_KIND := READ_MODE);
		procedure FILE_OPEN(Status: out FILE_OPEN_STATUS; file F: TEXT; External_Name: in STRING; Open_Kind: in FILE_OPEN_KIND := READ_MODE);
		procedure FILE_REWIND(file F: FT);
		procedure FILE_SEEK(file F: FT; Offset: INTEGER; Origin: FILE_ORIGIN_KIND := FILE_ORIGIN_BEGIN);
		procedure FILE_TRUNCATE(file F: FT; Size: INTEGER; Origin: FILE_ORIGIN_KIND := FILE_ORIGIN_BEGIN);
		function  FILE_MODE(file F: FT) return FILE_OPEN_KIND;
		function  FILE_TELL(file F: FT; Origin: FILE_ORIGIN_KIND := FILE_ORIGIN_BEGIN) return INTEGER;
		function  FILE_SIZE(file F: FT) return INTEGER;
		procedure FILE_CLOSE(file F: TEXT);
		procedure READ(file F: TEXT; VALUE: out STRING);
		procedure WRITE(file F: TEXT; VALUE: in STRING);
		procedure FLUSH(file F: TEXT);
		function  ENDFILE(file F: TEXT) return BOOLEAN;
	type SIDE is (RIGHT, LEFT);
	subtype WIDTH is NATURAL;
	function JUSTIFY(VALUE: STRING; JUSTIFIED: SIDE := RIGHT; FIELD: WIDTH := 0 ) return STRING;
	file INPUT: TEXT open READ_MODE is "STD_INPUT";
	file OUTPUT: TEXT open WRITE_MODE is "STD_OUTPUT";
	procedure READLINE(file F: TEXT; L: inout LINE);
	procedure READ(L: inout LINE; VALUE: out BIT; GOOD: out BOOLEAN);
	procedure SREAD(L: inout LINE; VALUE: out STRING; STRLEN: out NATURAL);
	alias STRING_READ is SREAD [LINE, STRING, NATURAL];
	alias BREAD is READ [LINE, BIT_VECTOR, BOOLEAN];
	alias BINARY_READ is READ [LINE, BIT_VECTOR, BOOLEAN];
	procedure OREAD(L: inout LINE; VALUE: out BIT_VECTOR; GOOD: out BOOLEAN);
	alias OCTAL_READ is OREAD [LINE, BIT_VECTOR, BOOLEAN];
	procedure HREAD(L: inout LINE; VALUE: out BIT_VECTOR; GOOD: out BOOLEAN);
	alias HEX_READ is HREAD [LINE, BIT_VECTOR, BOOLEAN];
	procedure WRITELINE(file F: TEXT; L: inout LINE);
	procedure TEE(file F: TEXT; L: inout LINE);
	procedure WRITE(L: inout LINE; VALUE: in BIT; JUSTIFIED: in SIDE:= RIGHT; FIELD: in WIDTH := 0);
	alias SWRITE is WRITE [LINE, STRING, SIDE, WIDTH];
	alias STRING_WRITE is WRITE [LINE, STRING, SIDE, WIDTH];
	alias BWRITE is WRITE [LINE, BIT_VECTOR, SIDE, WIDTH];
	alias BINARY_WRITE is WRITE [LINE, BIT_VECTOR, SIDE, WIDTH];
	procedure OWRITE(L: inout LINE; VALUE: in BIT_VECTOR; JUSTIFIED: in SIDE := RIGHT; FIELD: in WIDTH := 0);
	alias OCTAL_WRITE is OWRITE [LINE, BIT_VECTOR, SIDE, WIDTH];
	procedure HWRITE(L: inout LINE; VALUE: in BIT_VECTOR; JUSTIFIED: in SIDE := RIGHT; FIELD: in WIDTH := 0);
	alias HEX_WRITE is HWRITE [LINE, BIT_VECTOR, SIDE, WIDTH];
end package TEXTIO;

package ENV is
	procedure STOP(STATUS: INTEGER);
	procedure STOP;
	procedure FINISH(STATUS: INTEGER);
	procedure FINISH;
	function RESOLUTION_LIMIT return DELAY_LENGTH;
	type DAYOFWEEK is (SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY);
	type TIME_RECORD is record
	end record TIME_RECORD;
	impure function LOCALTIME return TIME_RECORD;
	impure function GMTIME return TIME_RECORD;
	impure function EPOCH return REAL;
	function LOCALTIME(TIMER: REAL) return TIME_RECORD;
	function GMTIME(TIMER: REAL) return TIME_RECORD;
	function EPOCH(TREC: TIME_RECORD) return REAL;
	function TIME_TO_SECONDS(TIME_VAL: IN TIME) return REAL;
	function SECONDS_TO_TIME(REAL_VAL: IN REAL) return TIME;
	impure function GETENV(Name : STRING) return STRING;
	impure function VHDL_VERSION return STRING;
	function TOOL_TYPE    return STRING;
	function TOOL_VENDOR  return STRING;
	function TOOL_NAME    return STRING;
	function TOOL_EDITION return STRING;
	function TOOL_VERSION return STRING;
	type DIRECTORY_ITEMS is access LINE_VECTOR;
	type DIRECTORY is record
	end record;
	type DIR_OPEN_STATUS is (STATUS_OK, STATUS_NOT_FOUND, STATUS_NO_DIRECTORY, STATUS_ACCESS_DENIED, STATUS_ERROR);
	type DIR_CREATE_STATUS is (STATUS_OK, STATUS_ITEM_EXISTS, STATUS_ACCESS_DENIED, STATUS_ERROR);
	type DIR_DELETE_STATUS is (STATUS_OK, STATUS_NO_DIRECTORY, STATUS_NOT_EMPTY, STATUS_ACCESS_DENIED, STATUS_ERROR);
	type FILE_DELETE_STATUS is (STATUS_OK, STATUS_NO_FILE, STATUS_ACCESS_DENIED, STATUS_ERROR);
	procedure DIR_OPEN(Dir: out DIRECTORY; Path : in STRING; Status : out DIR_OPEN_STATUS);
	procedure DIR_CLOSE(Dir: in DIRECTORY);
	constant DIR_SEPARATOR: STRING;
	impure function DIR_ITEMEXISTS(Path: in STRING) return BOOLEAN;
	impure function DIR_ITEMISDIR(Path: in STRING) return BOOLEAN;
	impure function DIR_ITEMISFILE(Path: in STRING) return BOOLEAN;
	procedure DIR_WORKINGDIR(Path: in STRING; Status : out DIR_OPEN_STATUS);
	procedure DIR_CREATEDIR(Path: in STRING; Status : out DIR_CREATE_STATUS);
	procedure DIR_DELETEDIR(Path: in STRING; Status : out DIR_DELETE_STATUS);
	procedure DIR_DELETEFILE(Path: in STRING; Status : out FILE_DELETE_STATUS);
	type CALL_PATH_ELEMENT is record
	end record;
	type CALL_PATH_VECTOR is array (natural range <>) of CALL_PATH_ELEMENT;
	type CALL_PATH_VECTOR_PTR is access CALL_PATH_VECTOR;
	impure function GET_CALL_PATH return CALL_PATH_VECTOR_PTR;
	impure function FILE_NAME return LINE;
	impure function FILE_NAME return STRING;
	impure function FILE_PATH return LINE;
	impure function FILE_PATH return STRING;
	impure function FILE_LINE return POSITIVE;
	impure function FILE_LINE return STRING;
end package ENV;

package MATH_REAL is
	constant MATH_E             : REAL;
	constant MATH_1_OVER_E      : REAL;
	constant MATH_PI            : REAL;
	constant MATH_2_PI          : REAL;
	constant MATH_1_OVER_PI     : REAL;
	constant MATH_PI_OVER_2     : REAL;
	constant MATH_PI_OVER_3     : REAL;
	constant MATH_PI_OVER_4     : REAL;
	constant MATH_3_PI_OVER_2   : REAL;
	constant MATH_LOG_OF_2      : REAL;
	constant MATH_LOG_OF_10     : REAL;
	constant MATH_LOG2_OF_E     : REAL;
	constant MATH_LOG10_OF_E    : REAL;
	constant MATH_SQRT_2        : REAL;
	constant MATH_1_OVER_SQRT_2 : REAL;
	constant MATH_SQRT_PI       : REAL;
	constant MATH_DEG_TO_RAD    : REAL;
	constant MATH_RAD_TO_DEG    : REAL;

	function SIGN(X : in REAL) return REAL;
	function CEIL(X : in REAL) return REAL;
	function FLOOR(X : in REAL) return REAL;
	function ROUND(X : in REAL) return REAL;
	function TRUNC(X : in REAL) return REAL;
	function REALMAX(X, Y : in REAL) return REAL;
	function REALMIN(X, Y : in REAL) return REAL;
	procedure UNIFORM(variable SEED1, SEED2 : inout POSITIVE; variable X : out REAL);
	function SQRT(X : in REAL) return REAL;
	function CBRT(X : in REAL) return REAL;
	function EXP(X : in REAL) return REAL;
	function LOG(X : in REAL) return REAL;
	function LOG2(X : in REAL) return REAL;
	function LOG10(X : in REAL) return REAL;
	function LOG(X : in REAL; BASE : in REAL) return REAL;
	function SIN(X : in REAL) return REAL;
	function COS(X : in REAL) return REAL;
	function TAN(X : in REAL) return REAL;
	function ARCSIN(X : in REAL) return REAL;
	function ARCCOS(X : in REAL) return REAL;
	function ARCTAN(Y : in REAL) return REAL;
	function ARCTAN(Y : in REAL; X : in REAL) return REAL;
	function SINH(X : in REAL) return REAL;
	function COSH(X : in REAL) return REAL;
	function TANH(X : in REAL) return REAL;
	function ARCSINH(X : in REAL) return REAL;
	function ARCCOSH(X : in REAL) return REAL;
	function ARCTANH(X : in REAL) return REAL;
end package MATH_REAL;

package MATH_COMPLEX is
	type COMPLEX is record
	end record;
	subtype POSITIVE_REAL is REAL range 0.0 to REAL'high;
	subtype PRINCIPAL_VALUE is REAL range -MATH_PI to MATH_PI;
	type COMPLEX_POLAR is record
	end record;
	constant MATH_CBASE_1 : COMPLEX;
	constant MATH_CBASE_J : COMPLEX;
	constant MATH_CZERO   : COMPLEX;

	function CMPLX(X : in REAL; Y : in REAL := 0.0) return COMPLEX;
	function GET_PRINCIPAL_VALUE(X : in REAL) return PRINCIPAL_VALUE;
	function COMPLEX_TO_POLAR(Z : in COMPLEX) return COMPLEX_POLAR;
	function POLAR_TO_COMPLEX(Z : in COMPLEX_POLAR) return COMPLEX;
	function ARG(Z : in COMPLEX) return PRINCIPAL_VALUE;
	function CONJ(Z : in COMPLEX) return COMPLEX;
end package MATH_COMPLEX;

package std_logic_1164 is
	type STD_ULOGIC is ();
	type STD_ULOGIC_VECTOR is array (NATURAL range <>) of STD_ULOGIC;
	function resolved(s : STD_ULOGIC_VECTOR) return STD_ULOGIC;
	subtype STD_LOGIC is resolved STD_ULOGIC;
	subtype STD_LOGIC_VECTOR is (resolved) STD_ULOGIC_VECTOR;
	subtype X01 is resolved STD_ULOGIC range 'X' to '1';
	subtype X01Z is resolved STD_ULOGIC range 'X' to 'Z';
	subtype UX01 is resolved STD_ULOGIC range 'U' to '1';
	subtype UX01Z is resolved STD_ULOGIC range 'U' to 'Z';

	function To_bit(s : STD_ULOGIC; xmap : BIT        := '0') return BIT;
	function To_bitvector(s : STD_ULOGIC_VECTOR; xmap : BIT := '0') return BIT_VECTOR;
	function To_StdULogic(b : BIT) return STD_ULOGIC;
	function To_StdLogicVector(b : BIT_VECTOR) return STD_LOGIC_VECTOR;
	function To_StdULogicVector(b : BIT_VECTOR) return STD_ULOGIC_VECTOR;
	alias To_Bit_Vector is To_bitvector[STD_ULOGIC_VECTOR, BIT return BIT_VECTOR];
	alias To_BV is To_bitvector[STD_ULOGIC_VECTOR, BIT return BIT_VECTOR];
	alias To_Std_Logic_Vector is To_StdLogicVector[BIT_VECTOR return STD_LOGIC_VECTOR];
	alias To_SLV is To_StdLogicVector[BIT_VECTOR return STD_LOGIC_VECTOR];
	alias To_Std_ULogic_Vector is To_StdULogicVector[BIT_VECTOR return STD_ULOGIC_VECTOR];
	alias To_SULV is To_StdULogicVector[BIT_VECTOR return STD_ULOGIC_VECTOR];
	function TO_01(s : STD_ULOGIC_VECTOR; xmap : STD_ULOGIC := '0') return STD_ULOGIC_VECTOR;
	function To_X01(s : STD_ULOGIC_VECTOR) return STD_ULOGIC_VECTOR;
	function To_X01Z(s : STD_ULOGIC_VECTOR) return STD_ULOGIC_VECTOR;
	function To_UX01(s : STD_ULOGIC_VECTOR) return STD_ULOGIC_VECTOR;
	function Is_X(s : STD_ULOGIC_VECTOR) return BOOLEAN;
end package std_logic_1164;

package std_logic_textio is
end package std_logic_textio;

package NUMERIC_STD is
	type UNRESOLVED_UNSIGNED is array (NATURAL range <>) of STD_ULOGIC;
	type UNRESOLVED_SIGNED is array (NATURAL range <>) of STD_ULOGIC;
	alias U_UNSIGNED is UNRESOLVED_UNSIGNED;
	alias U_SIGNED is UNRESOLVED_SIGNED;
	subtype UNSIGNED is (resolved) UNRESOLVED_UNSIGNED;
	subtype SIGNED is (resolved) UNRESOLVED_SIGNED;

	function find_leftmost(ARG : UNRESOLVED_UNSIGNED; Y : STD_ULOGIC) return INTEGER;
	function find_rightmost(ARG : UNRESOLVED_UNSIGNED; Y : STD_ULOGIC) return INTEGER;
	function SHIFT_LEFT(ARG : UNRESOLVED_UNSIGNED; COUNT : NATURAL) return UNRESOLVED_UNSIGNED;
	function SHIFT_RIGHT(ARG : UNRESOLVED_UNSIGNED; COUNT : NATURAL) return UNRESOLVED_UNSIGNED;
	function ROTATE_LEFT(ARG : UNRESOLVED_UNSIGNED; COUNT : NATURAL) return UNRESOLVED_UNSIGNED;
	function ROTATE_RIGHT(ARG : UNRESOLVED_UNSIGNED; COUNT : NATURAL) return UNRESOLVED_UNSIGNED;
	function RESIZE(ARG : UNRESOLVED_SIGNED; NEW_SIZE : NATURAL) return UNRESOLVED_SIGNED;
	function TO_INTEGER(ARG : UNRESOLVED_UNSIGNED) return NATURAL;
	function TO_UNSIGNED(ARG, SIZE : NATURAL) return UNRESOLVED_UNSIGNED;
	function TO_SIGNED(ARG : INTEGER; SIZE : NATURAL) return UNRESOLVED_SIGNED;
	function STD_MATCH(L, R : STD_ULOGIC) return BOOLEAN;
end package NUMERIC_STD;

package NUMERIC_BIT is
end package NUMERIC_BIT;
package NUMERIC_BIT_UNSIGNED is
end package NUMERIC_BIT_UNSIGNED;
context IEEE_BIT_CONTEXT is
end context IEEE_BIT_CONTEXT;
context IEEE_STD_CONTEXT is
end context IEEE_STD_CONTEXT;

package fixed_float_types is
	type fixed_round_style_type is (fixed_round, fixed_truncate);
	type fixed_overflow_style_type is (fixed_saturate, fixed_wrap);
	type round_type is (round_nearest, round_inf, round_neginf, round_zero);
end package fixed_float_types;
package fixed_generic_pkg is
	type UNRESOLVED_ufixed is array (INTEGER range <>) of STD_ULOGIC;
	type UNRESOLVED_sfixed is array (INTEGER range <>) of STD_ULOGIC;
	alias U_ufixed is UNRESOLVED_ufixed;
	alias U_sfixed is UNRESOLVED_sfixed;
	subtype ufixed is (resolved) UNRESOLVED_ufixed;
	subtype sfixed is (resolved) UNRESOLVED_sfixed;

	function divide() return UNRESOLVED_ufixed
	function reciprocal() return UNRESOLVED_ufixed
	function remainder() return UNRESOLVED_ufixed
	function modulo() return UNRESOLVED_ufixed
	procedure add_carry();
	function scalb(y : UNRESOLVED_ufixed; N : INTEGER) return UNRESOLVED_ufixed;
	function Is_Negative(arg : UNRESOLVED_sfixed) return BOOLEAN;
	function to_ufixed() return UNRESOLVED_ufixed;
	function to_sfixed() return UNRESOLVED_sfixed;
	function to_real(arg : UNRESOLVED_ufixed) return REAL;
	function ufixed_high() return INTEGER;
	function ufixed_low() return INTEGER;
	function sfixed_high() return INTEGER;
	function sfixed_low() return INTEGER;
	function saturate(constant left_index: INTEGER; constant right_index: INTEGER) return UNRESOLVED_ufixed;
	function to_UFix() return UNRESOLVED_ufixed;
	function to_SFix() return UNRESOLVED_sfixed;
	function UFix_high() return INTEGER;
	function UFix_low() return INTEGER;
	function SFix_high() return INTEGER;
	function SFix_low() return INTEGER;
	function from_string(bstring: STRING; constant left_index: INTEGER; constant right_index: INTEGER) return UNRESOLVED_ufixed;
	alias from_bstring is from_string [STRING, INTEGER, INTEGER return UNRESOLVED_ufixed];
	alias from_binary_string is from_string [STRING, INTEGER, INTEGER return UNRESOLVED_ufixed];
	function from_ostring( ostring: STRING; constant left_index: INTEGER; constant right_index: INTEGER) return UNRESOLVED_ufixed;
	alias from_octal_string is from_ostring [STRING, INTEGER, INTEGER return UNRESOLVED_ufixed];
	function from_hstring(hstring: STRING; constant left_index: INTEGER; constant right_index: INTEGER) return UNRESOLVED_ufixed;
	alias from_hex_string is from_hstring [STRING, INTEGER, INTEGER return UNRESOLVED_ufixed];
end package fixed_generic_pkg;
package fixed_pkg is new IEEE.fixed_generic_pkg;

package float_generic_pkg is
	type UNRESOLVED_float is array (INTEGER range <>) of STD_ULOGIC;
	alias U_float is UNRESOLVED_float;
	subtype float is (resolved) UNRESOLVED_float;
	subtype UNRESOLVED_float32 is UNRESOLVED_float (8 downto -23);
	alias U_float32 is UNRESOLVED_float32;
	subtype float32 is float (8 downto -23);
	subtype UNRESOLVED_float64 is UNRESOLVED_float (11 downto -52);
	alias U_float64 is UNRESOLVED_float64;
	subtype float64 is float (11 downto -52);
	subtype UNRESOLVED_float128 is UNRESOLVED_float (15 downto -112);
	alias U_float128 is UNRESOLVED_float128;
	subtype float128 is float (15 downto -112);
	type valid_fpstate is (nan, quiet_nan, neg_inf, neg_normal, neg_denormal, neg_zero, pos_zero, pos_denormal, pos_normal, pos_inf, isx);
	constant fphdlsynth_or_real : BOOLEAN;

	function Classfp(x: UNRESOLVED_float;  check_error: BOOLEAN := float_check_error) return valid_fpstate;
	function add() return UNRESOLVED_float;
	function subtract() return UNRESOLVED_float;
	function multiply() return UNRESOLVED_float;
	function dividebyp2() return UNRESOLVED_float;
	function mac() return UNRESOLVED_float;
	function eq() return BOOLEAN;
	function ne() return BOOLEAN;
	function lt() return BOOLEAN;
	function gt() return BOOLEAN;
	function le() return BOOLEAN;
	function ge() return BOOLEAN;
	function to_float32() return UNRESOLVED_float;
	function to_float64() return UNRESOLVED_float64;
	function to_float128() return UNRESOLVED_float128;
	function to_float() return UNRESOLVED_float;
	function realtobits(arg: REAL) return STD_ULOGIC_VECTOR;
	function bitstoreal(arg: STD_ULOGIC_VECTOR) return REAL;
	procedure break_number();
	function normalize() return UNRESOLVED_float;
	function Copysign(x, y: UNRESOLVED_float) return UNRESOLVED_float;
	function Logb(x: UNRESOLVED_float) return INTEGER;
	function Logb(x: UNRESOLVED_float) return UNRESOLVED_SIGNED;
	function Nextafter() return UNRESOLVED_float;
	function Unordered(x, y: UNRESOLVED_float) return BOOLEAN;
	function Finite(x: UNRESOLVED_float) return BOOLEAN;
	function Isnan(x: UNRESOLVED_float) return BOOLEAN;
	function zerofp() return UNRESOLVED_float;
	function nanfp() return UNRESOLVED_float;
	function qnanfp() return UNRESOLVED_float;
	function pos_inffp() return UNRESOLVED_float;
	function neg_inffp() return UNRESOLVED_float;
	function neg_zerofp() return UNRESOLVED_float;
end package float_generic_pkg;
package float_pkg is new IEEE.float_generic_pkg;

package REFLECTION is
end package REFLECTION;
