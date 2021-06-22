:: https://en.wikibooks.org/wiki/Windows_Batch_Scripting
:: https://www.robvanderwoude.com/battech.php

::! keywords		===========================================================
:: built-in command, output from help command
BREAK
CALL
CLS
ECHO
	ECHO.
ENDLOCAL EndLocal
EXIT
FOR
	IN
	DO
GOTO
IF
	ELSE
	NOT
	EXIST
	DEFINED

	EQU NEQ LSS LEQ GTR GEQ

	ERRORLEVEL ErrorLevel
	CMDEXTVERSION CmdExtVersion
PAUSE
POPD
REM
SET
SETLOCAL SetLocal
	ENABLEEXTENSIONS EnableExtensions
	DISABLEEXTENSIONS DisableExtensions
	ENABLEDELAYEDEXPANSION	EnableDelayedExpansion
	DISABLEDELAYEDEXPANSION	DisableDelayedExpansion
SHIFT
TITLE
VER
VERIFY

:: other
:: https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file#naming-conventions
CON PRN AUX NUL
COM1 COM2 COM3 COM4 COM5 COM6 COM7 COM8 COM9
LPT1 LPT2 LPT3 LPT4 LPT5 LPT6 LPT7 LPT8 LPT9

::! internal command	=======================================================
:: built-in command with arguments
ASSOC
CD
CHDIR
COLOR
COPY
DATE
DEL
DIR
ERASE
FTYPE
MD
MKDIR
MKLINK
MOVE
PATH
PROMPT
PUSHD
RD
REN
RENAME
RMDIR
START
TIME
TYPE
VOL

::! external command	=======================================================
:: output from help command
:: *.com
CHCP
FORMAT
MODE
MORE
TREE
:: *.exe
ATTRIB
BCDEDIT
CACLS
CHKDSK
CHKNTFS
CMD
COMP
COMPACT
CONVERT
DISKPART
DOSKEY
DRIVERQUERY
FC
FIND
FINDSTR
FSUTIL
GPRESULT
HELP
ICACLS
LABEL
OPENFILES
PRINT
RECOVER
REPLACE
ROBOCOPY
SC
SCHTASKS
SHUTDOWN
SORT
SUBST
SYSTEMINFO
TASKKILL
TASKLIST
WMIC
XCOPY
:: unknown command
GRAFTABL

:: other commands	===========================================================
:: https://docs.microsoft.com/en-us/windows-server/administration/windows-commands/windows-commands
:: A				===========================================================
active
add
add alias
add volume
append
arp
assign
assoc
at
atmadm
attach-vdisk
attrib
attributes
	attributes disk
	attributes volume
auditpol
	auditpol backup
	auditpol clear
	auditpol get
	auditpol list
	auditpol remove
	auditpol resourcesacl
	auditpol restore
	auditpol set
autochk
autoconv
autofmt
automount
:: B				===========================================================
bcdboot
bcdedit
bdehdcfg
	bdehdcfg driveinfo
	bdehdcfg newdriveletter
	bdehdcfg quiet
	bdehdcfg restart
	bdehdcfg size
	bdehdcfg target
begin backup
begin restore
bitsadmin
	bitsadmin addfile
	bitsadmin addfileset
	bitsadmin addfilewithranges
	bitsadmin cache
		bitsadmin cache and delete
		bitsadmin cache and deleteurl
		bitsadmin cache and getexpirationtime
		bitsadmin cache and getlimit
		bitsadmin cache and help
		bitsadmin cache and info
		bitsadmin cache and list
		bitsadmin cache and setexpirationtime
		bitsadmin cache and setlimit
		bitsadmin cache and clear
	bitsadmin cancel
	bitsadmin complete
	bitsadmin create
	bitsadmin examples
	bitsadmin getaclflags
	bitsadmin getbytestotal
	bitsadmin getbytestransferred
	bitsadmin getclientcertificate
	bitsadmin getcompletiontime
	bitsadmin getcreationtime
	bitsadmin getcustomheaders
	bitsadmin getdescription
	bitsadmin getdisplayname
	bitsadmin geterror
	bitsadmin geterrorcount
	bitsadmin getfilestotal
	bitsadmin getfilestransferred
	bitsadmin gethelpertokenflags
	bitsadmin gethelpertokensid
	bitsadmin gethttpmethod
	bitsadmin getmaxdownloadtime
	bitsadmin getminretrydelay
	bitsadmin getmodificationtime
	bitsadmin getnoprogresstimeout
	bitsadmin getnotifycmdline
	bitsadmin getnotifyflags
	bitsadmin getnotifyinterface
	bitsadmin getowner
	bitsadmin getpeercachingflags
	bitsadmin getpriority
	bitsadmin getproxybypasslist
	bitsadmin getproxylist
	bitsadmin getproxyusage
	bitsadmin getreplydata
	bitsadmin getreplyfilename
	bitsadmin getreplyprogress
	bitsadmin getsecurityflags
	bitsadmin getstate
	bitsadmin gettemporaryname
	bitsadmin gettype
	bitsadmin getvalidationstate
	bitsadmin help
	bitsadmin info
	bitsadmin list
	bitsadmin listfiles
	bitsadmin makecustomheaderswriteonly
	bitsadmin monitor
	bitsadmin nowrap
	bitsadmin peercaching
		bitsadmin peercaching and getconfigurationflags
		bitsadmin peercaching and help
		bitsadmin peercaching and setconfigurationflags
	bitsadmin peers
		bitsadmin peers and clear
		bitsadmin peers and discover
		bitsadmin peers and help
		bitsadmin peers and list
	bitsadmin rawreturn
	bitsadmin removeclientcertificate
	bitsadmin removecredentials
	bitsadmin replaceremoteprefix
	bitsadmin reset
	bitsadmin resume
	bitsadmin setaclflag
	bitsadmin setclientcertificatebyid
	bitsadmin setclientcertificatebyname
	bitsadmin setcredentials
	bitsadmin setcustomheaders
	bitsadmin setdescription
	bitsadmin setdisplayname
	bitsadmin sethelpertoken
	bitsadmin sethelpertokenflags
	bitsadmin sethttpmethod
	bitsadmin setmaxdownloadtime
	bitsadmin setminretrydelay
	bitsadmin setnoprogresstimeout
	bitsadmin setnotifycmdline
	bitsadmin setnotifyflags
	bitsadmin setpeercachingflags
	bitsadmin setpriority
	bitsadmin setproxysettings
	bitsadmin setreplyfilename
	bitsadmin setsecurityflags
	bitsadmin setvalidationstate
	bitsadmin suspend
	bitsadmin takeownership
	bitsadmin transfer
	bitsadmin util
		bitsadmin util and enableanalyticchannel
		bitsadmin util and getieproxy
		bitsadmin util and help
		bitsadmin util and repairservice
		bitsadmin util and setieproxy
		bitsadmin util and version
	bitsadmin wrap
bootcfg
	bootcfg addsw
	bootcfg copy
	bootcfg dbg1394
	bootcfg debug
	bootcfg default
	bootcfg delete
	bootcfg ems
	bootcfg query
	bootcfg raw
	bootcfg rmsw
	bootcfg timeout
break
:: C				===========================================================
cacls
call
cd
certreq
certutil
change
	change logon
	change port
	change user
chcp
chdir
chglogon
chgport
chgusr
chkdsk
chkntfs
choice
cipher
clean
cleanmgr
clip
cls
cmd
cmdkey
cmstp
color
comp
compact
compact vdisk
convert
	convert basic
	convert dynamic
	convert gpt
	convert mbr
copy
cprofile
create
	create partition efi
	[create partition extended
	create partition logical
	create partition msr
	create partition primary
	create volume mirror
	create volume raid
	create volume simple
	create volume stripe
cscript
:: D				===========================================================
date
dcgpofix
defrag
del
delete
	delete disk
	delete partition
	delete shadows
	delete volume
detach vdisk
detail
	detail disk
	detail partition
	detail vdisk
	detail volume
dfsdiag
	dfsdiag testdcs
	dfsdiag testdfsconfig
	dfsdiag testdfsintegrity
	dfsdiag testreferral
	dfsdiag testsites
dfsrmig
diantz
dir
diskcomp
diskcopy
diskpart
diskperf
diskraid
diskshadow
dispdiag
dnscmd
doskey
driverquery
:: E				===========================================================
echo
edit
endlocal
end restore
erase
eventcreate
eventquery
eventtriggers
Evntcmd
exec
exit
expand
expand vdisk
expose
extend
extract
:: F				===========================================================
fc
filesystems
find
findstr
finger
flattemp
fondue
for
forfiles
format
freedisk
fsutil
	fsutil 8dot3name
	fsutil behavior
	fsutil dirty
	fsutil file
	fsutil fsinfo
	fsutil hardlink
	fsutil objectid
	fsutil quota
	fsutil repair
	fsutil reparsepoint
	fsutil resource
	fsutil sparse
	fsutil tiering
	fsutil transaction
	fsutil usn
	fsutil volume
	fsutil wim
ftp
	ftp append
	ftp ascii
	ftp bell
	ftp binary
	ftp bye
	ftp cd
	ftp close
	ftp debug
	ftp delete
	ftp dir
	ftp disconnect
	ftp get
	ftp glob
	ftp hash
	ftp lcd
	ftp literal
	ftp ls
	ftp mget
	ftp mkdir
	ftp mls
	ftp mput
	ftp open
	ftp prompt
	ftp put
	ftp pwd
	ftp quit
	ftp quote
	ftp recv
	ftp remotehelp
	ftp rename
	ftp rmdir
	ftp send
	ftp status
	ftp trace
	ftp type
	ftp user
	ftp verbose
	ftp mdelete
	ftp mdir
ftype
fveupdate
:: G				===========================================================
getmac
gettype
goto
gpfixup
gpresult
gpt
gpupdate
graftabl
:: H				===========================================================
help
helpctr
hostname
:: I				===========================================================
icacls
if
import (shadowdisk)
import (diskpart)
inactive
inuse
ipconfig
ipxroute
irftp
:: J				===========================================================
jetpack
:: K				===========================================================
klist
ksetup
	ksetup addenctypeattr
	ksetup addhosttorealmmap
	ksetup addkdc
	ksetup addkpasswd
	ksetup addrealmflags
	ksetup changepassword
	ksetup delenctypeattr
	ksetup delhosttorealmmap
	ksetup delkdc
	ksetup delkpasswd
	ksetup delrealmflags
	ksetup domain
	ksetup dumpstate
	ksetup getenctypeattr
	ksetup listrealmflags
	ksetup mapuser
	ksetup removerealm
	ksetup server
	ksetup setcomputerpassword
	ksetup setenctypeattr
	ksetup setrealm
	ksetup setrealmflags
ktmutil
ktpass
:: L				===========================================================
label
list
	list providers
	list shadows
	list writers
load metadata
lodctr
logman
	logman create
	logman create alert
	logman create api
	logman create cfg
	logman create counter
	logman create trace
	logman delete
	logman import and logman export
	logman query
	logman start and logman stop
	logman update
	logman update alert
	logman update api
	logman update cfg
	logman update counter
	logman update trace
logoff
lpq
lpr
:: M				===========================================================
macfile
makecab
manage bde
	manage bde status
	manage bde on
	manage bde off
	manage bde pause
	manage bde resume
	manage bde lock
	manage bde unlock
	manage bde autounlock
	manage bde protectors
	manage bde tpm
	manage bde setidentifier
	manage bde forcerecovery
	manage bde changepassword
	manage bde changepin
	manage bde changekey
	manage bde keypackage
	manage bde upgrade
	manage bde wipefreespace
mapadmin
md
merge vdisk
mkdir
mklink
mmc
mode
more
mount
mountvol
move
mqbkup
mqsvc
mqtgsvc
msdt
msg
msiexec
msinfo32
mstsc
:: N				===========================================================
nbtstat
netcfg
net print
netsh
netstat
nfsadmin
nfsshare
nfsstat
nlbmgr
nslookup
	nslookup exit
	nslookup finger
	nslookup help
	nslookup ls
	nslookup lserver
	nslookup root
	nslookup server
	nslookup set
	nslookup set all
	nslookup set class
	nslookup set d2
	nslookup set debug
	nslookup set domain
	nslookup set port
	nslookup set querytype
	nslookup set recurse
	nslookup set retry
	nslookup set root
	nslookup set search
	nslookup set srchlist
	nslookup set timeout
	nslookup set type
	nslookup set vc
	nslookup view
ntbackup
ntcmdprompt
ntfrsutl
:: O				===========================================================
offline
	offline disk
	offline volume
online
	online disk
	online volume
openfiles
:: P				===========================================================
pagefileconfig
path
pathping
pause
pbadmin
pentnt
perfmon
ping
pnpunattend
pnputil
popd
powershell
powershell ise
print
prncnfg
prndrvr
prnjobs
prnmngr
prnport
prnqctl
prompt
pubprn
pushd
pushprinterconnections
pwlauncher
:: Q				===========================================================
qappsrv
qprocess
query
	query process
	query session
	query termserver
	query user
quser
qwinsta
:: R				===========================================================
rcp
rd
rdpsign
recover
recover disk group
refsutil
reg
	reg add
	reg compare
	reg copy
	reg delete
	reg export
	reg import
	reg load
	reg query
	reg restore
	reg save
	reg unload
regini
regsvr32
relog
rem
remove
ren
rename
repair
	repair bde
replace
rescan
reset
	reset session
retain
revert
rexec
risetup
rmdir
robocopy
route ws2008
rpcinfo
rpcping
rsh
rundll32
rundll32 printui
rwinsta
:: S				===========================================================
san
sc config
sc create
sc delete
sc query
schtasks
scwcmd
	scwcmd analyze
	scwcmd configure
	scwcmd register
	scwcmd rollback
	scwcmd transform
	scwcmd view
secedit
	secedit analyze
	secedit configure
	secedit export
	secedit generaterollback
	secedit import
	secedit validate
select
	select disk
	select partition
	select vdisk
	select volume
serverceipoptin
servermanagercmd
serverweroptin
set environmental variables
set shadow copy
	set context
	set id
	setlocal
	set metadata
	set option
	set verbose
setx
sfc
shadow
shift
showmount
shrink
shutdown
simulate restore
sort
start
subcommand set device
subcommand set drivergroup
subcommand set drivergroupfilter
subcommand set driverpackage
subcommand set image
subcommand set imagegroup
subcommand set server
subcommand set transportserver
subcommand set multicasttransmission
subcommand start namespace
subcommand start server
subcommand start transportserver
subcommand stop server
subcommand stop transportserver
subst
sxstrace
sysocmgr
systeminfo
:: T				===========================================================
takeown
tapicfg
taskkill
tasklist
tcmsetup
telnet
	telnet close
	telnet display
	telnet open
	telnet quit
	telnet send
	telnet set
	telnet status
	telnet unset
tftp
time
timeout
title
tlntadmn
tpmtool
tpmvscmgr
tracerpt
tracert
tree
tscon
tsdiscon
tsecimp
tskill
tsprof
type
typeperf
tzutil
:: U				===========================================================
unexpose
uniqueid
unlodctr
:: V				===========================================================
ver
verifier
verify
vol
vssadmin
	vssadmin delete shadows
	vssadmin list shadows
	vssadmin list writers
	vssadmin resize shadowstorage
:: W				===========================================================
waitfor
wbadmin
	wbadmin delete catalog
	wbadmin delete systemstatebackup
	wbadmin disable backup
	wbadmin enable backup
	wbadmin get disks
	wbadmin get items
	wbadmin get status
	wbadmin get versions
	wbadmin restore catalog
	wbadmin start backup
	wbadmin start recovery
	wbadmin start sysrecovery
	wbadmin start systemstatebackup
	wbadmin start systemstaterecovery
	wbadmin stop job
wdsutil
wecutil
wevtutil
where
whoami
winnt
winnt32
winpop
winrs
winsat mem
winsat mfmedia
wmic
writer
wscript
:: X				===========================================================
xcopy


::! environment variables	===================================================
:: IF /?
CMDCMDLINE CmdCmdLine
:: SET /?
RANDOM Random random
HIGHESTNUMANODENUMBER HighestNumaNodeNumber
:: Dynamic Environment Variables, https://www.robvanderwoude.com/ntset.php
ExitCode
ExitCodeAscii
__APPDIR__ __AppDir__
__CD__
:: some system environment variables: SET
ALLUSERSPROFILE AllUsersProfile
APPDATA AppData
CommonProgramFiles
CommonProgramFiles(x86)
CommonProgramW6432
COMPUTERNAME ComputerName
ComSpec
DriverData
HOMEDRIVE HomeDrive
HOMEPATH HomePath
LOCALAPPDATA LocalAppData
LOGONSERVER LogonServer
NUMBER_OF_PROCESSORS Number_Of_Processors
OS
Path
PATHEXT PathExt
PROCESSOR_ARCHITECTURE Processor_Architecture
PROCESSOR_IDENTIFIER Processor_Identifier
PROCESSOR_LEVEL Processor_Level
PROCESSOR_REVISION Processor_Revision
ProgramData
ProgramFiles
ProgramFiles(x86)
ProgramW6432
PSModulePath
PUBLIC Public
SESSIONNAME	SessionName
SystemDrive
SystemRoot
TEMP
TMP
USERDOMAIN UserDomain
USERDOMAIN_ROAMINGPROFILE UserDomain_RoamingProfile
USERNAME UserName
USERPROFILE UserProfile
windir WINDIR WinDir

::! options			===========================================================
:: FOR /?
eol skip delims tokens usebackq
:: START /?
MIN MAX SEPARATE SHARED LOW NORMAL HIGH REALTIME ABOVENORMAL BELOWNORMAL NODE AFFINITY WAIT
min max separate shared low normal high realtime abovenormal belownormal node affinity wait
:: GOTO /?
EOF eof
