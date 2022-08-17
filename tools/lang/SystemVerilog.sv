// SystemVerilog 2017

//! keywords			=======================================================
accept_on alias always always_comb always_ff always_latch assert assign assume automatic
before bind bins binsof break
cell const constraint context continue cover coverpoint cross
deassign default defparam design disable dist do
edge else enum eventually expect export extends extern
final first_match for force foreach forever forkjoin
genvar global
highz0 highz1
if iff ifnone ignore_bins illegal_bins implements implies import incdir include initial inout input inside instance interconnect intersect
large let liblist library local localparam
matches medium modport
negedge nettype new nexttime noshowcancelled null
output
packed parameter posedge priority protected pull0 pull1 pulldown pullup pulsestyle_ondetect pulsestyle_onevent pure
rand randc ref reject_on release repeat restrict return
s_always s_eventually s_nexttime s_until s_until_with scalared showcancelled small soft solve specparam static strong strong0 strong1 struct super sync_accept_on sync_reject_on
tagged this throughout timeprecision timeunit  type typedef
union unique unique0 until until_with untyped use
var vectored virtual
wait wait_order weak weak0 weak1 while wildcard with within

//! data types			=======================================================
// Built-in net types
supply0 supply1 tri tri0 tri1 triand trior trireg uwire wand wire wor
// Integer data types
shortint int longint byte bit logic reg integer time
signed unsigned
real shortreal realtime
void
chandle
string
event
// gate and switch types
cmos rcmos
bufif0 bufif1 notif0 notif1
nmos pmos rnmos rpmos
and nand or nor xor xnor
buf not
tranif0 tranif1 rtranif1 rtranif0
tran rtran

//! code folding test	=======================================================
; // clear declaration state
interface class PutImp#(type PUT_T = logic);
	pure virtual function void put(PUT_T a);
endclass
virtual interface interface_identifier;
disable fork;
wait fork;
bind + function float faddif(int, float); // Operator overloading
default clocking clocking_identifier;
rand join // Random sequence generation
export "DPI-C" function f;
import "DPI-C" f = function ...;

//! code folding		=======================================================
begin [:block_identifier]
end
case (expression_or_dist)
endcase
casex
endcase
casez
endcase
randcase randcase_item
endcase
checker checker_identifier
endchecker
class [lifetime] class_identifier [parameter_port_list]
endclass [:class_identifier]
clocking clocking_identifier @clocking_event
endclocking
config config_identifier
endconfig
function [lifetime] function_body_declaration
endfunction
fork [:block_identifier]
join
fork [:block_identifier]
join_any
fork [:block_identifier]
join_none
generate
endgenerate
covergroup covergroup_identifier
endgroup
interface class class_identifier [parameter_port_list]
endclass
interface [lifetime] interface_identifier
endinterface
module [lifetime] module_identifier
endmodule
macromodule [lifetime] module_identifier
endmodule
package [lifetime] package_identifier
endpackage
primitive udp_identifier(udp_port_list)
endprimitive
program [lifetime] program_identifier
endprogram
property property_identifier
endproperty
sequence sequence_identifier
endsequence
randsequence ([production_ identifier])
endsequence
specify
endspecify
table
endtable
task [lifetime] task_body_declaration
endtask

//! api					=======================================================
// string
int len();
void putc(int i, byte c);
byte getc(int i);
string toupper();
string tolower();
int compare(string s);
int icompare(string s);
string substr(int i, int j);
integer atoi();
integer atohex();
integer atooct();
integer atobin();
real atoreal();
void itoa(integer i);
void hextoa(integer i);
void octtoa(integer i);
void bintoa(integer i);
void realtoa(real r);

// enum type
enum first();
enum last();
enum next(int unsigned N = 1);
enum prev(int unsigned N = 1);
int num();
string name();

int $cast(singular dest_var, singular source_exp);

// dynamic arrays
int size();
void delete();
// associative array
int num();
int size();
void delete([input index]);
int exists(input index);
int first(ref index);
int last(ref index);
int next(ref index);
int prev(ref index);
// queue
int size();
void insert(input integer index, input element_t item);
void delete([input integer index]);
element_t pop_front();
element_t pop_back();
void push_front(input element_t item);
void push_back(input element_t item);
// Array locator methods
find()
find_index()
find_first()
find_first_index()
find_last()
find_last_index()
min()
max()
unique()
unique_index()
// Array ordering methods
reverse()
sort()
rsort()
shuffle()
// Array reduction methods
sum()
product()
and()
or()
xor()
int_or_index_type index(int dimension = 1);

class process;
	static process self();
	state status();
	void kill();
	/*task*/ await();
	void suspend();
	void resume();
	void srandom(int seed);
	string get_randstate();
	void set_randstate(string state);
endclass

class semaphore
	new(int keyCount = 0);
	void put(int keyCount = 1);
	/*task*/ get(int keyCount = 1);
	int try_get(int keyCount = 1);
endclass

class mailbox
	new(int bound = 0);
	int num();
	/*task*/ put(singular message);
	int try_put(singular message);
	/*task*/ get(ref singular message);
	int try_get(ref singular message);
	/*task*/ peek(ref singular message);
	int try_peek(ref singular message);
endclass

wait_order(hierarchical_identifier {, hierarchical_identifier}) action_block
assert(expression) action_block
assume(expression) action_block
cover(expression) statement_or_null
assert property(property_spec);
cover sequence(sequence_expr) statement_or_null

first_match(sequence_expr {, sequence_match_item})
accept_on(expression_or_dist) property_expr
reject_on(expression_or_dist) property_expr
sync_accept_on(expression_or_dist) property_expr
sync_reject_on(expression_or_dist) property_expr
expect(property_spec) action_block

// Randomization methods
int randomize();
void pre_randomize();
void post_randomize();
rand_mode(bit on_off);
constraint_mode(bit on_off);
$urandom
$urandom(int seed)
int unsigned $urandom_range(int unsigned maxval, int unsigned minval = 0);
void srandom(int seed);
string get_randstate();
void set_randstate(string state);

// Predefined coverage methods
void sample()
real get_coverage()
real get_coverage(ref int, ref int)
real get_inst_coverage()
real get_inst_coverage(ref int, ref int)
void set_inst_name(string)
void start()
void stop()

//! System task and function		===========================================
// Simulation control tasks
$exit

// Simulation time functions

// Timescale tasks
$printtimescale(hierarchical_identifier)
$timeformat(units_number , precision_number , suffix_string , minimum_field_width)

// Conversion functions
$bitstoshortreal(bit_val)
$shortrealtobits(shortreal_val)

// Data query functions
$typename(expression)
$bits(expression)
$isunbounded(constant_expression)

// Array query functions
$dimensions(array_identifier)
$unpacked_dimensions(array_identifier)
$left(array_identifier [, dimension_expression])
$right(array_identifier [, dimension_expression])
$low(array_identifier [, dimension_expression])
$high(array_identifier [, dimension_expression])
$increment(array_identifier [, dimension_expression])
$size(array_identifier [, dimension_expression])

// Math functions

// Bit vector system functions
$countbits(expression)
$countones(expression)
$onehot(expression)
$onehot0(expression)
$isunknown(expression)

// Severity tasks
$fatal(finish_number [, arguments])
$error(arguments)
$warning(arguments)
$info(arguments)
// Elaboration tasks
$fatal(finish_number [, arguments])
$error(arguments)
$warning(arguments)
$info(arguments)

// Assertion control tasks
$asserton(levels [, list_of_scopes_or_assertions])
$assertoff(levels [, list_of_scopes_or_assertions])
$assertkill(levels [, list_of_scopes_or_assertions])
$assertcontrol(control_type [, [assertion_type] [, [directive_type] [, [levels] [, list_of_scopes_or_assertions]]]])
$assertpasson(levels [, list_of_scopes_or_assertions])
$assertpassoff(levels [, list_of_scopes_or_assertions])
$assertfailon(levels [, list_of_scopes_or_assertions])
$assertfailoff(levels [, list_of_scopes_or_assertions])
$assertnonvacuouson(levels [, list_of_scopes_or_assertions])
$assertvacuousoff(levels [, list_of_scopes_or_assertions])

// Sampled value system functions
$sampled(expression)
$rose(expression [, [clocking_event]])
$fell(expression [, [clocking_event]])
$stable(expression [, [clocking_event]])
$changed(expression [, [clocking_event]])
$past(expression1 [, [number_of_ticks] [, [expression2] [, [clocking_event]]]])
$past_gclk(expression)
$rose_gclk(expression)
$fell_gclk(expression)
$stable_gclk(expression)
$changed_gclk(expression)
$future_gclk(expression)
$rising_gclk(expression)
$falling_gclk(expression)
$steady_gclk(expression)
$changing_gclk(expression)

// Coverage control functions
$coverage_control
$coverage_get_max
$coverage_get
$coverage_merge
$coverage_save
$get_coverage
$set_coverage_db_name
$load_coverage_db

// Probabilistic distribution functions
// Stochastic analysis tasks and functions
// PLA modeling tasks

// Miscellaneous tasks and functions
$system("terminal_command_line")

// Display tasks
// File I/O tasks and functions
$sformatf(format_string, arguments)
// Memory load tasks
// Memory dump tasks
$writememb(filename, memory_name [, start_addr [, finish_addr]])
$writememh(filename, memory_name [, start_addr [, finish_addr]])
// Command line input
// VCD tasks

//! directives			=======================================================
`resetall
`include
`define
`undef
`undefineall
`ifdef
`else
`elsif
`endif
`ifndef
`endif
`timescale
`default_nettype
`unconnected_drive
`nounconnected_drive
`celldefine
`endcelldefine
`pragma
`line
`__FILE__
`__LINE__
`begin_keywords
`end_keywords

//! misc				=======================================================
std
