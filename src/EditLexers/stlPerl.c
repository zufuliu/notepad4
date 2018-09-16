#include "EditLexer.h"
#include "EditStyle.h"

// https://www.perl.org/

static KEYWORDLIST Keywords_Perl = {{
"__DATA__ __END__ __FILE__ __LINE__ __PACKAGE__ abs accept alarm and atan2 AUTOLOAD BEGIN "
"bind binmode bless break caller chdir CHECK chmod chomp chop chown chr chroot close closedir cmp "
"connect continue CORE cos crypt dbmclose dbmopen default defined delete DESTROY die do "
"dump each else elsif END endgrent endhostent endnetent endprotoent endpwent endservent eof "
"eq EQ eval exec exists exit exp fcntl fileno flock for foreach fork format formline ge GE "
"getc getgrent getgrgid getgrnam gethostbyaddr gethostbyname gethostent getlogin "
"getnetbyaddr getnetbyname getnetent getpeername getpgrp getppid getpriority getprotobyname "
"getprotobynumber getprotoent getpwent getpwnam getpwuid getservbyname getservbyport "
"getservent getsockname getsockopt given glob gmtime goto grep gt GT hex if index INIT int "
"ioctl join keys kill last lc lcfirst le LE length link listen local localtime lock log "
"lstat lt LT map mkdir msgctl msgget msgrcv msgsnd my ne NE next no not NULL oct open "
"opendir or ord our pack package pipe pop pos print printf prototype push qu quotemeta rand "
"read readdir readline readlink readpipe recv redo ref rename require reset return reverse "
"rewinddir rindex rmdir say scalar seek seekdir select semctl semget semop send setgrent "
"sethostent setnetent setpgrp setpriority setprotoent setpwent setservent setsockopt shift "
"shmctl shmget shmread shmwrite shutdown sin sleep socket socketpair sort splice split "
"sprintf sqrt srand stat state study sub substr symlink syscall sysopen sysread sysseek "
"system syswrite tell telldir tie tied time times truncate uc ucfirst umask undef UNITCHECK "
"unless unlink unpack unshift untie until use utime values vec wait waitpid wantarray warn "
"when while write xor"

, // 1
"elsif if split while"

, "", "", "", "", "", "", ""

, "", "", "", "", "", "", ""
}};

EDITLEXER lexPerl = { SCLEX_PERL, NP2LEX_PERL, EDITLEXER_HOLE, L"Perl Script", L"pl; pm; cgi; pod; plx; stp", L"", &Keywords_Perl,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_PL_DEFAULT, L"Default", L"", L"" },
	{ SCE_PL_COMMENTLINE, NP2STYLE_Comment, L"Comment", L"fore:#608060", L"" },
	{ SCE_PL_WORD, NP2STYLE_Keyword, L"Keyword", L"bold; fore:#FF8000", L"" },
	{ MULTI_STYLE(SCE_PL_STRING, SCE_PL_CHARACTER, 0, 0), NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_PL_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_PL_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ SCE_PL_SCALAR, 63401, L"Scalar $var", L"fore:#0000FF", L"" },
	{ SCE_PL_ARRAY, 63402, L"Array @var", L"fore:#FF0080", L"" },
	{ SCE_PL_HASH, 63403, L"Hash %var", L"fore:#B000B0", L"" },
	{ SCE_PL_SYMBOLTABLE, 63404, L"Symbol Table *var", L"fore:#3A6EA5", L"" },
	{ SCE_PL_REGEX, 63405, L"Regex /re/ or m{re}", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_PL_REGSUBST, 63406, L"Substitution s/re/ore/", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_PL_BACKTICKS, 63407, L"Backticks", L"fore:#E24000; back:#FFF1A8", L"" },
	{ SCE_PL_HERE_DELIM, 63408, L"Here-doc (Delimiter)", L"fore:#648000", L"" },
	{ SCE_PL_HERE_Q, 63409, L"Here-doc (Single Quoted, q)", L"fore:#648000", L"" },
	{ SCE_PL_HERE_QQ, 63410, L"Here-doc (Double Quoted, qq)", L"fore:#648000", L"" },
	{ SCE_PL_HERE_QX, 63411, L"Here-doc (Backticks, qx)", L"fore:#E24000; back:#FFF1A8", L"" },
	{ SCE_PL_STRING_Q, 63412, L"Single Quoted String (Generic, q)", L"fore:#008000", L"" },
	{ SCE_PL_STRING_QQ, 63413, L"Double Quoted String (qq)", L"fore:#008000", L"" },
	{ SCE_PL_STRING_QX, 63414, L"Backticks (qx)", L"fore:#E24000; back:#FFF1A8", L"" },
	{ SCE_PL_STRING_QR, 63415, L"Regex (qr)", L"fore:#006633; back:#FFF1A8", L"" },
	{ SCE_PL_STRING_QW, 63416, L"Array (qw)", L"fore:#003CE6", L"" },
	{ SCE_PL_SUB_PROTOTYPE, 63417, L"Prototype", L"fore:#800080; back:#FFE2FF", L"" },
	{ SCE_PL_FORMAT_IDENT, 63418, L"Format Identifier", L"bold; fore:#648000; back:#FFF1A8", L"" },
	{ SCE_PL_FORMAT, 63419, L"Format Body", L"fore:#648000; back:#FFF1A8", L"" },
	{ SCE_PL_POD, 63420, L"POD (Common)", L"fore:#A46000; back:#FFFFC0; eolfilled", L"" },
	{ SCE_PL_POD_VERB, 63421, L"POD (Verbatim)", L"fore:#A46000; back:#FFFFC0; eolfilled", L"" },
	{ SCE_PL_DATASECTION, 63422, L"Data Section", L"fore:#A46000; back:#FFFFC0; eolfilled", L"" },
	{ SCE_PL_ERROR, 63423, L"Parsing Error", L"fore:#C80000; back:#FFFF80", L"" },
	//{ SCE_PL_PUNCTUATION, L"Symbols / Punctuation (not used)", L"", L"" },
	//{ SCE_PL_PREPROCESSOR, L"Preprocessor (not used)", L"", L"" },
	//{ SCE_PL_LONGQUOTE, L"Long Quote (qq, qr, qw, qx) (not used)", L"", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
