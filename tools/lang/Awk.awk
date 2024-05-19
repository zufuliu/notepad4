# 5.3 https://www.gnu.org/software/gawk/manual/

#! keywords			===========================================================
# https://www.gnu.org/software/gawk/manual/html_node/BEGIN_002fEND.html#BEGIN_002fEND
BEGIN END
# https://www.gnu.org/software/gawk/manual/html_node/BEGINFILE_002fENDFILE.html#BEGINFILE_002fENDFILE
BEGINFILE ENDFILE
# https://www.gnu.org/software/gawk/manual/html_node/Getline.html#Getline
getline
# https://www.gnu.org/software/gawk/manual/html_node/Print.html
print printf
# https://www.gnu.org/software/gawk/manual/html_node/Delete.html#Delete
delete
# https://www.gnu.org/software/gawk/manual/html_node/Definition-Syntax.html
function return
# https://www.gnu.org/software/gawk/manual/html_node/Statements.html
if else while do for in switch case default break continue next nextfile exit
# https://www.gnu.org/software/gawk/manual/html_node/Include-Files.html
@include
# https://www.gnu.org/software/gawk/manual/html_node/Loading-Shared-Libraries.html
@load
# https://www.gnu.org/software/gawk/manual/html_node/Namespaces.html
@namespace


#! Predefined Variables	=======================================================
# https://www.gnu.org/software/gawk/manual/html_node/Built_002din-Variables.html
# https://www.gnu.org/software/gawk/manual/html_node/User_002dmodified.html
BINMODE CONVFMT FIELDWIDTHS FPAT FS IGNORECASE LINT OFMT OFS ORS PREC ROUNDMODE RS SUBSEP TEXTDOMAIN
# https://www.gnu.org/software/gawk/manual/html_node/Auto_002dset.html
ARGC ARGV ARGIND ENVIRON ERRNO FILENAME FNR NF FUNCTAB NR PROCINFO RLENGTH RSTART RT SYMTAB


#! Built-in Functions	=======================================================
# https://www.gnu.org/software/gawk/manual/html_node/Built_002din.html#Built_002din
mkbool(expression)
# Numeric Functions
# https://www.gnu.org/software/gawk/manual/html_node/Numeric-Functions.html
atan2(y, x)
cos(x)
exp(x)
int(x)
log(x)
rand()
sin(x)
sqrt(x)
srand([x])

# String-Manipulation Functions
# https://www.gnu.org/software/gawk/manual/html_node/String-Functions.html
asort(source [, dest [, how ] ])
asorti(source [, dest [, how ] ])
gensub(regexp, replacement, how [, target])
gsub(regexp, replacement [, target])
index(in, find)
length([string])
match(string, regexp [, array])
patsplit(string, array [, fieldpat [, seps ] ])
split(string, array [, fieldsep [, seps ] ])
sprintf(format, expression1, …)
strtonum(str)
sub(regexp, replacement [, target])
substr(string, start [, length ])
tolower(string)
toupper(string)

# Input/Output Functions
# https://www.gnu.org/software/gawk/manual/html_node/I_002fO-Functions.html
close(filename [, how])
fflush([filename])
system(command)

# Time Functions
# https://www.gnu.org/software/gawk/manual/html_node/Time-Functions.html
mktime(datespec [, utc-flag ])
strftime([format [, timestamp [, utc-flag] ] ])
systime()

# Bit-Manipulation Functions
# https://www.gnu.org/software/gawk/manual/html_node/Bitwise-Functions.html
and(v1, v2 [, …])
compl(val)
lshift(val, count)
or(v1, v2 [, …])
rshift(val, count)
xor(v1, v2 [, …])

# Getting Type Information
# https://www.gnu.org/software/gawk/manual/html_node/Type-Functions.html
isarray(x)
typeof(x)

# String-Translation Functions
# https://www.gnu.org/software/gawk/manual/html_node/I18N-Functions.html
bindtextdomain(directory [, domain])
dcgettext(string [, domain [, category] ])
dcngettext(string1, string2, number [, domain [, category] ])


#! Library Functions	=======================================================
# https://www.gnu.org/software/gawk/manual/html_node/Library-Functions.html
# General Functions
# https://www.gnu.org/software/gawk/manual/html_node/General-Functions.html
function assert(condition, string)
function round(x, ival, aval, fraction)
function cliff_rand()
function ord(str, c)
function chr(c)
function join(array, start, end, sep, result, i)
function getlocaltime(time, ret, now, i)
function readfile(file, tmp, save_rs)
function shell_quote(s, SINGLE, QSINGLE, i, X, n, ret)
function isnumeric(x, f)
function tocsv(fields, sep, i, j, nfields, result)
function tocsv_rec(sep, i, fields)

# Data file Management
# https://www.gnu.org/software/gawk/manual/html_node/Data-File-Management.html
beginfile(filename)
endfile(filename)
function rewind(i)
zerofile(ARGV[Argind], Argind)
function disable_assigns(argc, argv, i)

# Getopt Function
# https://www.gnu.org/software/gawk/manual/html_node/Getopt-Function.html
function getopt(argc, argv, options, longopts, thisopt, i, j)

# Passwd Functions
# https://www.gnu.org/software/gawk/manual/html_node/Passwd-Functions.html
function getpwnam(name)
function getpwuid(uid)
function getpwent()
function endpwent()

# Group Functions
# https://www.gnu.org/software/gawk/manual/html_node/Group-Functions.html
function getgrnam(group)
function getgrgid(gid)
function getgruser(user)
function getgrent()
function endgrent()

# Walking Arrays
# https://www.gnu.org/software/gawk/manual/html_node/Walking-Arrays.html
function walk_array(arr, name, i)
function process_array(arr, name, process, do_arrays, i, new_name)

# other functions from /usr/share/awk/
function bits2str(bits, data, mask)
function ctime(ts, format)
function adequate_math_precision(n)
function intdiv0(numerator, denominator, result)
function quicksort(data, left, right, less_than, i, last)
function quicksort_swap(data, i, j, temp)


#! misc				===========================================================
# default namespace
awk gawk

# result of typeof(x)
array regexp number string strnum unassigned untyped

# PROCINFO
argv egid errno euid FS gid
identifiers array builtin extension scalar untyped user
platform
pgrpid pid pma ppid strftime uid version
gmp_version mpfr_version prec_max prec_min api_major api_minor
BUFFERPIPE NONFATAL pty READ_TIMEOUT RETRY sorted_in

# Environment Variables
AWKPATH AWKLIBPATH
GAWK_MSEC_SLEEP GAWK_READ_TIMEOUT GAWK_SOCK_RETRIES POSIXLY_CORRECT
AWKBUFSIZE AWK_HASH AWKREADFUNC GAWK_MSG_SRC GAWK_LOCALE_DIR GAWK_NO_DFA
GAWK_STACKSIZE INT_CHAIN_MAX STR_CHAIN_MAX TIDYMEM
