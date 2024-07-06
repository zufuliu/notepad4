/* https://support.sas.com/en/documentation.html */
/* SAS Viya 2024.06 https://go.documentation.sas.com/doc/en/pgmsascdc/v_052/pgmsaswlcm/home.htm */
/* SAS 9.4 / Viya 3.5 https://documentation.sas.com/doc/en/pgmsascdc/9.4_3.5/pgmsashome/home.htm */

/*! keywords 		======================================================== */
/* Operators */
/* https://go.documentation.sas.com/doc/en/pgmsascdc/v_052/lepg/titlepage.htm */
eq ne gt lt ge le in
and or not
min max

/* Global Statements */
/* https://go.documentation.sas.com/doc/en/pgmsascdc/v_052/lestmtsglobal/titlepage.htm */
catname
checkpoint execute_always
endsas
filename
footnote
libname
lock
missing
options
page
resetline
run
sasfile
skip
sysecho
title

/* DATA Step Statements */
/* https://go.documentation.sas.com/doc/en/pgmsascdc/v_052/lestmtsref/titlepage.htm */
abort
array
attrib
by
call
cards
cards4
continue
data
datalines
datalines4
delete
describe
do until while
drop
end
error
execute
file
format
goto
if then else
infile
informat
input
keep
label
leave
length
link
list
lockdown
lostcard
merge
modify
output
put
putlog
redirect
remove
rename
replace
retain
return
select when otherwise
set
stop
update
where

/* SQL Procedure */
/* https://go.documentation.sas.com/doc/en/pgmsascdc/v_052/sqlproc/titlepage.htm */
proc
alter table
connect
create index
create table
create view
delete
describe table
describe view
describe table constraints
disconnect from
drop table
drop view
drop index from
execute by
from
group by
having
insert into set
insert into values
order by asc desc
quit
reset
select distinct unique
update where
validate
where

/*! macro 			======================================================== */
/* Macro Language */
/* https://go.documentation.sas.com/doc/en/pgmsascdc/v_052/mcrolref/titlepage.htm */
%include
%list
%run

/* Macro Statements */
%abort
%copy
%do %to %until %while
%end
%global
%goto
%if %then %else
%let
%local
%macro
%mend
%put
%return
%symdel
%syscall
%sysexec
%syslput
%sysmacdelete
%sysmstoreclear
%sysrput

/* AutoCall Macros */
%cmpres(text | text-expression)
%compstor(pathname=sas-data-library)
%datatyp(text | text-expression)
%kverify(source, excerpt)
%left(text | text-expression)
%lowcase(text | text-expression)
%qcmpres(text | text-expression)
%qleft(text | text-expression)
%qlowcase(text | text-expression)
%qtrim(text | text-expression)
%sysrc(character-string)
%trim(text | text-expression)
%verify(source, excerpt)

/* Macro Functions */
%bquote(character-string)
%eval(expression)
%index(source-string, target-string)
%length(character-string)
%nrbquote(character-string)
%nrquote(character-string)
%nrstr(character-string)
%qscan(character-string, n<, charlist<, modifiers>>)
%qsubstr(character-string, position<, length>)
%qsysfunc(function(s)<, format>)
%quote(character-string)
%qupcase(character-string)
%scan(character-string, n<, delimiters<, modifiers>>)
%str(character-string)
%substr(character-string, position<, length>)
%superq(macro-variable-name)
%symexist(macro-variable-name)
%symglobl(macro-variable-name)
%symlocal(macro-variable-name)
%sysevalf(expression<, conversion-type>)
%sysfunc(function(s)<, format>)
%sysget(environment-variable)
%sysmacexec(macro-name)
%sysmacexist(macro-name)
%sysmexecdepth
%sysmexecname(nesting-level)
%sysprod(product-code)
%unquote(character-string)
%upcase(character-string)

/* Base SAS Utilities */
%ds2csv(argument-1=value-1, argument-2=value-2 <,argument-3=value-3 ...>)
%tslit(literal text)

/*! functions 		======================================================== */
/* Functions and CALL Routines */
/* https://go.documentation.sas.com/doc/en/pgmsascdc/v_052/lefunctionsref/titlepage.htm */
abs()
addrlong()
airy()
allcomb()
allperm()
anyalnum()
anyalpha()
anycntrl()
anydigit()
anyfirst()
anygraph()
anylower()
anyname()
anyprint()
anypunct()
anyspace()
anyupper()
anyxdigit()
arcos()
arcosh()
arsin()
arsinh()
artanh()
atan()
atan2()
attrc()
attrn()
band()
beta()
betainv()
bhamming_32()
bhamming_hex()
blackclprc()
blackptprc()
blkshclprc()
blkshptprc()
blshift()
bnot()
bor()
brshift()
bxor()
byte()
call allcomb()
call allcombi()
call allperm()
call cats()
call catt()
call catx()
call compcost()
call execute()
call graycode()
call is8601_convert()
call label()
call lexcomb()
call lexcombi()
call lexperk()
call lexperm()
call logistic()
call missing()
call module()
call pokelong()
call prxchange()
call prxdebug()
call prxfree()
call prxnext()
call prxposn()
call prxsubstr()
call ranbin()
call rancau()
call rancomb()
call ranexp()
call rangam()
call rannor()
call ranperk()
call ranperm()
call ranpoi()
call rantbl()
call rantri()
call ranuni()
call scan()
call set()
call sleep()
call softmax()
call sort()
call sortc()
call sortn()
call stdize()
call stream()
call streaminit()
call streamrewind()
call symput()
call symputx()
call system()
call tanh()
call vname()
call vnext()
cat()
catq()
cats()
catt()
catx()
cdf()
ceil()
ceilz()
cexist()
char()
choosec()
choosen()
cinv()
clibexist()
close()
cmiss()
cnonct()
coalesce()
coalescec()
collate()
comb()
compare()
compbl()
compfuzz()
compfuzz_miss()
compged()
complev()
compound()
compress()
compsrv_oval()
compsrv_unquote2()
constant()
convx()
convxp()
cos()
cosh()
cot()
count()
countc()
countw()
csc()
css()
cumipmt()
cumprinc()
curobs()
cv()
daccdb()
daccdbsl()
daccsl()
daccsyd()
dacctab()
dairy()
datdif()
date()
datejul()
datepart()
datetime()
day()
dclose()
dcreate()
depdb()
depdbsl()
depsl()
depsyd()
deptab()
dequote()
deviance()
dhms()
dif()
digamma()
dim()
dinfo()
divide()
dlgcdir()
dnum()
dopen()
doptname()
doptnum()
dosubl()
dread()
dropnote()
dsname()
dur()
durp()
effrate()
envlen()
erf()
erfc()
euclid()
exist()
exp()
expm1()
fact()
fappend()
fclose()
fcol()
fcopy()
fdelete()
fetch()
fetchobs()
fexist()
fget()
fileexist()
filename()
fileref()
finance()
find()
findc()
findw()
finfo()
finv()
fipname()
fipnamel()
fipstate()
first()
floor()
floorz()
fmtinfo()
fnonct()
fnote()
fopen()
foptname()
foptnum()
fpoint()
fpos()
fput()
fread()
frewind()
frlen()
fsep()
fuzz()
fwrite()
gaminv()
gamma()
garkhclprc()
garkhptprc()
gcd()
geodist()
geomean()
geomeanz()
getcasurl()
getlcaslib()
getlsessref()
getsessopt()
getltag()
getoption()
getvarc()
getvarn()
graycode()
harmean()
harmeanz()
hash_fast_hex()
hash_xx_hex()
hashing()
hashing_file()
hashing_hmac()
hashing_hmac_file()
hashing_hmac_init()
hashing_init()
hashing_part()
hashing_term()
hbound()
hms()
holiday()
holidayck()
holidaycount()
holidayname()
holidaynx()
holidayny()
holidaytest()
hour()
htmldecode()
htmlencode()
ibessel()
ifc()
ifn()
index()
indexc()
indexw()
input()
inputc()
inputn()
int()
intcindex()
intck()
intcycle()
intfit()
intfmt()
intget()
intindex()
intnx()
intrr()
intseas()
intshift()
inttest()
intz()
iorcmsg()
ipmt()
iqr()
irr()
jbessel()
jsonpp()
juldate()
juldate7()
kurtosis()
lag()
largest()
lbound()
lcm()
lcomb()
left()
length()
lengthc()
lengthm()
lengthn()
lexcomb()
lexcombi()
lexperk()
lexperm()
lfact()
lgamma()
libname()
libref()
log()
log10()
log1px()
log2()
logbeta()
logcdf()
logistic()
logpdf()
logsdf()
lowcase()
lperm()
lpnorm()
mad()
margrclprc()
margrptprc()
max()
md5()
mdy()
mean()
median()
min()
minute()
missing()
mod()
modexist()
module()
modulec()
modulen()
modz()
month()
mopen()
mort()
msplint()
mvalid()
n()
netpv()
nliteral()
nmiss()
nomrate()
normal()
notalnum()
notalpha()
notcntrl()
notdigit()
note()
notfirst()
notgraph()
notlower()
notname()
notprint()
notpunct()
notspace()
notupper()
notxdigit()
npv()
nvalid()
nwkdom()
open()
ordinal()
pathname()
pctl()
pdf()
peekclong()
peeklong()
perm()
pmt()
point()
poisson()
ppmt()
probbeta()
probbnml()
probbnrm()
probchi()
probf()
probgam()
probhypr()
probit()
probmc()
probmed()
probnegb()
probnorm()
probt()
propcase()
prxchange()
prxmatch()
prxparen()
prxparse()
prxposn()
ptrlongadd()
put()
putc()
putn()
pvp()
qtr()
quantile()
quote()
ranbin()
rancau()
rand()
ranexp()
rangam()
range()
rank()
rannor()
ranpoi()
rantbl()
rantri()
ranuni()
rename()
repeat()
resolve()
reverse()
rewind()
right()
rms()
round()
rounde()
roundz()
saving()
savings()
scan()
sdf()
sec()
second()
sessbusy()
sessfound()
sha256()
sha256hex()
sha256hmachex()
sign()
sin()
sinh()
skewness()
sleep()
smallest()
sort()
soundex()
spedis()
sqrt()
squantile()
std()
stderr()
stfips()
stname()
stnamel()
strip()
subpad()
substr()
substrn()
sum()
sumabs()
symexist()
symget()
symglobl()
symlocal()
sysexist()
sysget()
sysmsg()
sysparm()
sysprocessid()
sysprocessname()
sysprod()
sysrc()
system()
tan()
tanh()
time()
timepart()
timevalue()
tinv()
tnonct()
today()
translate()
transtrn()
tranwrd()
trigamma()
trim()
trimn()
trunc()
tslvl()
typeof()
tzoneid()
tzonename()
tzoneoff()
tzones2u()
tzoneu2s()
uniform()
upcase()
urldecode()
urlencode()
uss()
uuidgen()
var()
varfmt()
varinfmt()
varlabel()
varlen()
varname()
varnum()
varray()
varrayx()
vartype()
verify()
vformat()
vformatd()
vformatdx()
vformatn()
vformatnx()
vformatw()
vformatwx()
vformatx()
vinarray()
vinarrayx()
vinformat()
vinformatd()
vinformatdx()
vinformatn()
vinformatnx()
vinformatw()
vinformatwx()
vinformatx()
vlabel()
vlabelx()
vlength()
vlengthx()
vname()
vnamex()
vtype()
vtypex()
vvalue()
vvaluex()
week()
weekday()
whichc()
whichn()
year()
yieldp()
yrdif()
yyq()
zipcity()
zipcitydistance()
zipfips()
zipname()
zipnamel()
zipstate()

/* DATA Step Functions for Macros */
resolve()
symexist()
symget()
symgetn()
symglobl()
symlocal()
