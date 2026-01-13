# 5.42 https://www.perl.org/
# https://perldoc.perl.org/index-language.html

#! keywords				=======================================================
# https://learn.perl.org/docs/keywords.html
# Perl syntax
CORE
#__DATA__ __END__ package
__FILE__
__LINE__
__PACKAGE__
__SUB__
and
cmp
continue
do
else elsif eq exp
for foreach
ge gt
if
le lock lt
ne no
or
sub
tr
unless until
while
xor

# Non-function keywords
catch
default defer
elseif
finally
given
isa
try
when

# https://perldoc.perl.org/perlclass
feature
class method field

# https://perldoc.perl.org/attributes#Built-in-Attributes
lvalue
method
prototype
const
shared

# Perl functions
# https://perldoc.perl.org/functions
AUTOLOAD
BEGIN
CHECK
DESTROY
END
INIT
UNITCHECK

abs
accept
alarm
all
any
atan2
bind
binmode
bless
break

caller
chdir
chmod
chomp
chop
chown
chr
chroot
class
close
closedir
connect
continue
cos
crypt

dbmclose
dbmopen
defined
delete
die
do
dump

each
endgrent
endhostent
endnetent
endprotoent
endpwent
endservent
eof
eval
evalbytes
exec
exists
exit
exp

fc
fcntl
fileno
flock
fork
format
formline

getc
getgrent
getgrgid
getgrnam
gethostbyaddr
gethostbyname
gethostent
getlogin
getnetbyaddr
getnetbyname
getnetent
getpeername
getpgrp
getppid
getpriority
getprotobyname
getprotobynumber
getprotoent
getpwent
getpwnam
getpwuid
getservbyname
getservbyport
getservent
getsockname
getsockopt
glob
gmtime
goto
grep

hex
import
index
int
ioctl
join
keys
kill

last
lc
lcfirst
length
link
listen
local
localtime
lock
log
lstat

map
method
mkdir
msgctl
msgget
msgrcv
msgsnd
my
next
not
no

oct
open
opendir
ord
our

pack
# package
pipe
pop
pos
print
printf
prototype
push
quotemeta

rand
read
readdir
readline
readlink
readpipe
recv
redo
ref
rename
require
reset
return
reverse
rewinddir
rindex
rmdir

say
scalar
seek
seekdir
select
semctl
semget
semop
send
setgrent
sethostent
setnetent
setpgrp
setpriority
setprotoent
setpwent
setservent
setsockopt
shift
shmctl
shmget
shmread
shmwrite
shutdown
sin
sleep
socket
socketpair
sort
splice
split
sprintf
sqrt
srand
stat
state
study
sub
substr
symlink
syscall
sysopen
sysread
sysseek
system
syswrite

tell
telldir
tie
tied
time
times
truncate

uc
ucfirst
umask
undef
unlink
unpack
unshift
untie
use
utime

values
vec
wait
waitpid
wantarray
warn
write

# File Handles
ARGV
ARGVOUT
STDERR
STDIN
STDOUT

#! regex			===========================================================
# keywords that forces /PATTERN/ at all times; should track vim's behaviour
elsif if split while

#! variables		===========================================================
# https://perldoc.perl.org/perlvar
$ACCUMULATOR
$ARG
$ARGV
$BASETIME
$CHILD_ERROR
$COMPILING
$DEBUGGING
$EFFECTIVE_GROUP_ID
$EFFECTIVE_USER_ID
$EGID
$ERRNO
$EUID
$EVAL_ERROR
$EXCEPTIONS_BEING_CAUGHT
$EXECUTABLE_NAME
$EXTENDED_OS_ERROR
$FORMAT_FORMFEED
$FORMAT_LINES_LEFT
$FORMAT_LINES_PER_PAGE
$FORMAT_LINE_BREAK_CHARACTERS
$FORMAT_NAME
$FORMAT_PAGE_NUMBER
$FORMAT_TOP_NAME
$GID
$INPLACE_EDIT
$INPUT_LINE_NUMBER
$INPUT_RECORD_SEPARATOR
$LAST_REGEXP_CODE_RESULT
$LIST_SEPARATOR
$MATCH
$MULTILINE_MATCHING
$NR
$OFMT
$OFS
$ORS
$OSNAME
$OS_ERROR
$OUTPUT_AUTO_FLUSH
$OUTPUT_FIELD_SEPARATOR
$OUTPUT_RECORD_SEPARATOR
$PERLDB
$PERL_VERSION
$PID
$POSTMATCH
$PREMATCH
$PROCESS_ID
$PROGRAM_NAME
$REAL_GROUP_ID
$REAL_USER_ID
$RS
$SUBSCRIPT_SEPARATOR
$SUBSEP
$SYSTEM_FD_MAX
$UID
$WARNING
$^CHILD_ERROR_NATIVE
$^ENCODING
$^OPEN
$^RE_TRIE_MAXBUF
$^TAINT
$^UNICODE
$^UTF8LOCALE
$^WARNING_BITS
$^WIDE_SYSTEM_CALLS
%ENV
%INC
%OVERLOAD
%SIG
@ARGV
@INC
@LAST_MATCH_START
