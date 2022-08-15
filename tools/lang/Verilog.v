// Verilog 2005

//! keywords			=======================================================
always assign automatic
cell
deassign default defparam design disable
edge else event
for force forever
genvar
highz0 highz1
if ifnone incdir include initial inout input instance
large liblist library localparam
medium
negedge noshowcancelled
output
parameter posedge pull0 pull1 pulldown pullup pulsestyle_onevent pulsestyle_ondetect
release repeat
scalared showcancelled  small specparam strong0 strong1
use
vectored
wait weak0 weak1 while

//! data types 			=======================================================
// net_type
supply0 supply1 tri triand trireg trior tri0 tri1 uwire wire wand wor
// variables type
integer real realtime reg signed time unsigned
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
module ram_model(address, write, chip_select, data);
endmodule
instance top.a2 use work.cfg5:config; // Using hierarchical config
$scope // four-state VCD file
	module name;
$end

//! code folding		=======================================================
begin [:block_identifier]
end
case
endcase
casex
endcase
casez
endcase
config config_identifier
endconfig
function [automatic] [function_range_or_type] function_identifier
endfunction
fork [:block_identifier]
join
generate
endgenerate
module module_identifier
endmodule
macromodule module_identifier
endmodule
primitive udp_identifier(udp_port_list)
endprimitive
specify
endspecify
table
endtable
task [automatic] task_identifier
endtask

//! System task and function		===========================================
// Timing checks
$setup(data_event, reference_event, timing_check_limit [, [notifier]])
$hold(reference_event, data_event, timing_check_limit [, [notifier]])
$setuphold(reference_event, data_event, timing_check_limit, timing_check_limit [, [notifier] [, [stamptime_condition] [, [checktime_condition] [, [delayed_reference] [, [delayed_data]]]]]])
$recovery(reference_event, data_event, timing_check_limit [, [notifier]])
$removal(reference_event, data_event, timing_check_limit [, [notifier]])
$recrem(reference_event, data_event, timing_check_limit, timing_check_limit [, [notifier] [, [stamptime_condition] [, [checktime_condition] [, [delayed_reference] [, [delayed_data]]]]]])
$skew(reference_event, data_event, timing_check_limit [, [notifier]] )
$timeskew(reference_event, data_event, timing_check_limit [, [notifier] [, [event_based_flag] [, [remain_active_flag]]]])
$fullskew(reference_event, data_event, timing_check_limit, timing_check_limit [, [notifier] [, [event_based_flag] [, [remain_active_flag]]]])
$period(controlled_reference_event, timing_check_limit [, [notifier]])
$width(controlled_reference_event, timing_check_limit [, threshold [, notifier]])
$nochange(reference_event, data_event, start_edge_offset, end_edge_offset [, [notifier]])

// Display tasks
$display(arguments)
$displayb(arguments)
$displayh(arguments)
$displayo(arguments)
$write(arguments)
$writeb(arguments)
$writeh(arguments)
$writeo(arguments)
// Strobed monitoring
$strobe(arguments)
$strobeb(arguments)
$strobeh(arguments)
$strobeo(arguments)
// Continuous monitoring
$monitor(arguments)
$monitorb(arguments)
$monitorh(arguments)
$monitoro(arguments)
$monitoroff
$monitoron

// File I/O tasks
$fopen("file_name", type)
$fclose(fd)
//  File output system tasks
$fdisplay(fd [, arguments])
$fdisplayb(fd [, arguments])
$fdisplayh(fd [, arguments])
$fdisplayo(fd [, arguments])
$fwrite(fd [, arguments])
$fwriteb(fd [, arguments])
$fwriteh(fd [, arguments])
$fwriteo(fd [, arguments])
$fstrobe(fd [, arguments])
$fstrobeb(fd [, arguments])
$fstrobeh(fd [, arguments])
$fstrobeo(fd [, arguments])
$fmonitor(fd [, arguments])
$fmonitorb(fd [, arguments])
$fmonitorh(fd [, arguments])
$fmonitoro(fd [, arguments])
// Formatting data to a string
$swrite(output_reg, arguments)
$swriteb(output_reg, arguments)
$swriteh(output_reg, arguments)
$swriteo(output_reg, arguments)
$sformat(output_reg, format_string, arguments)
// Reading data from a file
$fgetc(fd)
$ungetc(c, fd)
$fgets(str, fd)
$fscanf(fd, format, args)
$sscanf(str, format, args)
$fread(mem, fd, start, count)
$ftell(fd)
$fseek(fd, offset, operation)
$rewind(fd)
$fflush(fd)
$ferror(fd, str)
$feof(fd)
// Loading memory data from a file
$readmemb("file_name", memory_name [, start_addr [, finish_addr]])
$readmemh("file_name", memory_name [, start_addr [, finish_addr]])
$sdf_annotate("sdf_file" [, [module_instance] [, ["config_file"] [, ["log_file"] [, ["mtm_spec"] [, ["scale_factors"] [, ["scale_type"]]]]]]])

// Timescale tasks
$printtimescale(hierarchical_identifier)
$timeformat(units_number, precision_number, suffix_string, minimum_field_width)

// Simulation control tasks
$finish(n)
$stop(n)

// PLA modeling tasks
$async$and$array(memory_identifier, input_terms, output_terms)
$async$and$plane(memory_identifier, input_terms, output_terms)
$async$nand$array(memory_identifier, input_terms, output_terms)
$async$nand$plane(memory_identifier, input_terms, output_terms)
$async$nor$array(memory_identifier, input_terms, output_terms)
$async$nor$plane(memory_identifier, input_terms, output_terms)
$async$or$array(memory_identifier, input_terms, output_terms)
$async$or$plane(memory_identifier, input_terms, output_terms)
$sync$and$array(memory_identifier, input_terms, output_terms)
$sync$and$plane(memory_identifier, input_terms, output_terms)
$sync$nand$array(memory_identifier, input_terms, output_terms)
$sync$nand$plane(memory_identifier, input_terms, output_terms)
$sync$nor$array(memory_identifier, input_terms, output_terms)
$sync$nor$plane(memory_identifier, input_terms, output_terms)
$sync$or$array(memory_identifier, input_terms, output_terms)
$sync$or$plane(memory_identifier, input_terms, output_terms)

// Stochastic analysis tasks
$q_initialize(q_id, q_type, max_length, status)
$q_add(q_id, job_id, inform_id, status)
$q_remove(q_id, job_id, inform_id, status)
$q_full(q_id, status)
$q_exam(q_id, q_stat_code, q_stat_value, status)

// Simulation time functions
$time
$stime
$realtime

// Conversion functions
$itor(real_val)
$rtoi(int_val)
$realtobits(real_val)
$bitstoreal(bit_val)
$signed()
$unsigned()

// Probabilistic distribution functions
$random
$random(seed)
$dist_uniform( seed, start, end)
$dist_normal(seed, mean, standard_deviation)
$dist_exponential(seed, mean)
$dist_poisson(seed, mean)
$dist_chi_square(seed, degree_of_freedom)
$dist_t(seed, degree_of_freedom)
$dist_erlang(seed, k_stage, mean)

// Command line input
$test$plusargs(string)
$value$plusargs(user_string, variable)

// Math functions
$clog2(n)
$ln(x)
$log10(x)
$exp(x)
$sqrt(x)
$pow(x)
$floor(x)
$ceil(x)
$sin(x)
$cos(x)
$tan(x)
$asin(x)
$acos(x)
$atan(x)
$atan2(x, y)
$hypot(x, y)
$sinh(x)
$cosh(x)
$tanh(x)
$asinh(x)
$acosh(x)
$atanh(x)

// Value change dump (VCD) files
$dumpfile(filename)
$dumpvars
$dumpvars(levels [, list_of_modules_or_variables])
$dumpoff
$dumpon
$dumpall
$dumplimit(filesize)
$dumpflush
$comment comment_text
$end
$date
$end
$enddefinitions
$end
$scope
$end
$timescale
$end
$upscope
$end
$var
$end
$version
$end
// Creating extended VCD file
$dumpports(scope_list, file_pathname)
$dumpportsoff(file_pathname)
$dumpportson(file_pathname)
$dumpportsall(file_pathname)
$dumpportslimit(filesize, file_pathname)
$dumpportsflush(file_pathname)
$vcdclose
$end

// Annex
$countdrivers(net, [net_is_forced, number_of_01x_drivers, number_of_0_drivers, number_of_1_drivers, number_of_x_drivers])
$getpattern(mem_element)
$input("filename")
$key("filename")
$nokey
$list(hierarchical_name)
$log("filename")
$nolog
$reset(stop_value [, reset_value, [diagnostics_value]])
$reset_count
$reset_value
$save("file_name")
$restart("file_name")
$incsave("incremental_file_name")
$scale(hierarchical_name)
$scope(hierarchical_name)
$showscopes(n)
$showvars(list_of_variables)
$sreadmemb(mem_name, start_address, finish_address, string {, string })
$sreadmemh(mem_name, start_address, finish_address, string {, string })

//! directives			=======================================================
`celldefine
`endcelldefine
`default_nettype
`define
`undef
`ifdef
`else
`elsif
`endif
`ifndef
`endif
`include
`resetall
`line
`timescale
`nounconnected_drive
`unconnected_drive
`pragma
`begin_keywords
`end_keywords

`default_decay_time
`default_trireg_strength
`delay_mode_distributed
`delay_mode_path
`delay_mode_unit
`delay_mode_zero

//! misc				=======================================================
PATHPULSE$
STDIN STDOUT STDERR
