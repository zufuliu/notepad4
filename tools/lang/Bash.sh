#!/bin/bash
# 5.2 https://www.gnu.org/software/bash/manual/
# https://en.wikipedia.org/wiki/Almquist_shell
# https://en.wikipedia.org/wiki/Bourne_shell
# https://man.archlinux.org/man/dash.1
# http://kornshell.com/
# https://en.wikipedia.org/wiki/KornShell
# https://man.archlinux.org/man/ksh.1.en
# https://zsh.sourceforge.io/
# https://en.wikipedia.org/wiki/Z_shell
# https://man.archlinux.org/man/zsh.1.en

#! keywords 			=======================================================
# Reserved Words
if then
elif then
else
fi
time
for in
until do
done
while do
done
case in
esac
select
coproc
function

# Bourne Shell Builtins
break
cd
continue
eval
exec
exit
export
getopts
hash
pwd
readonly
return
shift
test
times
trap
umask
unset

# Bash Builtin Commands
alias
bind
builtin
caller
command
declare
echo
enable
help
let
local
logout
mapfile
printf
read
readarray
source
type
typeset
ulimit
unalias

# Modifying Shell Behavior
set
shopt

# Directory Stack Builtins
dirs
popd
pushd

# Job Control Builtins
bg
fg
jobs
kill
wait
disown
suspend

#! variables 			=======================================================
# Bourne Shell Variables
$CDPATH
$HOME
$IFS
$MAIL
$MAILPATH
$OPTARG
$OPTIND
$PATH
$PS1
$PS2

# Bash Variables
$BASH
$BASHOPTS
$BASHPID
$BASH_ALIASES
$BASH_ARGC
$BASH_ARGV
$BASH_ARGV0
$BASH_CMDS
$BASH_COMMAND
$BASH_COMPAT
$BASH_ENV
$BASH_EXECUTION_STRING
$BASH_LINENO
$BASH_LOADABLES_PATH
$BASH_REMATCH
$BASH_SOURCE
$BASH_SUBSHELL
$BASH_VERSINFO
$BASH_VERSION
$BASH_XTRACEFD
$CHILD_MAX
$COLUMNS
$COMP_CWORD
$COMP_LINE
$COMP_POINT
$COMP_TYPE
$COMP_KEY
$COMP_WORDBREAKS
$COMP_WORDS
$COMPREPLY
$COPROC
$DIRSTACK
$EMACS
$ENV
$EPOCHREALTIME
$EPOCHSECONDS
$EUID
$EXECIGNORE
$FCEDIT
$FIGNORE
$FUNCNAME
$FUNCNEST
$GLOBIGNORE
$GROUPS
$HISTCMD
$HISTCONTROL
$HISTFILE
$HISTFILESIZE
$HISTIGNORE
$HISTSIZE
$HISTTIMEFORMAT
$HOSTFILE
$HOSTNAME
$HOSTTYPE
$IGNOREEOF
$INPUTRC
$INSIDE_EMACS
$LANG
$LC_ALL
$LC_COLLATE
$LC_CTYPE
$LC_MESSAGES
$LC_NUMERIC
$LC_TIME
$LINENO
$LINES
$MACHTYPE
$MAILCHECK
$MAPFILE
$OLDPWD
$OPTERR
$OSTYPE
$PIPESTATUS
$POSIXLY_CORRECT
$PPID
$PROMPT_COMMAND
$PROMPT_DIRTRIM
$PS0
$PS3
$PS4
$PWD
$RANDOM
$READLINE_ARGUMENT
$READLINE_LINE
$READLINE_MARK
$READLINE_POINT
$REPLY
$SECONDS
$SHELL
$SHELLOPTS
$SHLVL
$SRANDOM
$TIMEFORMAT
$TMOUT
$TMPDIR
$UID

# Environment Variables
# https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap08.html
# https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html

# Tomcat
$JAVA_HOME
$JAVA_OPTS
$CATALINA_HOME
$CATALINA_BASE
$CATALINA_OPTS

#! commands				=======================================================
ar
asa
awk

banner
basename
bash
bc
bdiff
bunzip2
bzip2

cal
calendar
cat
cc
chgrp
chmod
chown
chroot
cksum
clear
cmp
col
comm
compress
cp
cpio
crypt
csplit
ctags
cut

date
dc
dd
deroff
dev
df
diff
diff3
dir
dircmp
dircolors
dirname
du

ed
egrep
env
ex
expand
expr

factor
false
fc
fgrep
file
find
fmt
fold

getconf
getopt
grep
gres
groups

head
history
hostid

iconv
id
install
integer

join
lc
line
link
ln
logname
look
ls

mail
mailx
make
man
md5sum
mkdir
mkfifo
mknod
more
mt
mv

newgrp
nice
nl
nm
no
nohup
ntps

od
pack
paste
patch
pathchk
pax
pcat
perl
pg
pinky
pr
print
printenv
ps
ptx

readlink
red
repeat
rev
rm
rmdir

sed
seq
sh
sha1sum
shred
size
sleep
sort
spell
split
start
stat
stop
strings
strip
stty
su
sum
sync

tac
tail
tar
tee
touch
tr
true
tsort
tty

uname
uncompress
unexpand
uniq
unlink
unpack
users
uudecode
uuencode

vdir
vi
vim
vpax

wc
whence
which
who
whoami
wpaste
wstart

xargs
yes
zcat
