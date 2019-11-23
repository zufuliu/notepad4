#include "EditLexer.h"
#include "EditStyle.h"

// https://www.perl.org/
// https://perldoc.perl.org/index-language.html
// http://learn.perl.org/docs/keywords.html

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

, // keywords that forces /PATTERN/ at all times
"elsif if split while"

, NULL, NULL, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Perl[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_PL_COMMENTLINE, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ SCE_PL_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"bold; fore:#FF8000" },
	{ MULTI_STYLE(SCE_PL_STRING, SCE_PL_CHARACTER, 0, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_PL_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_PL_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_PL_SCALAR, SCE_PL_STRING_VAR, 0, 0), NP2STYLE_ScalarVar, EDITSTYLE_HOLE(L"Scalar $var"), L"fore:#0000FF" },
	{ SCE_PL_ARRAY, NP2STYLE_ArrayVar, EDITSTYLE_HOLE(L"Array @var"), L"fore:#FF0080" },
	{ SCE_PL_HASH, NP2STYLE_HashVar, EDITSTYLE_HOLE(L"Hash %var"), L"fore:#B000B0" },
	{ SCE_PL_SYMBOLTABLE, NP2STYLE_SymbolTableVar, EDITSTYLE_HOLE(L"Symbol Table *var"), L"fore:#3A6EA5" },
	{ SCE_PL_REGEX, NP2STYLE_Regex_mre, EDITSTYLE_HOLE(L"Regex /re/ or m{re}"), L"fore:#006633; back:#FFF1A8" },
	{ SCE_PL_REGSUBST, NP2STYLE_Substitution_re, EDITSTYLE_HOLE(L"Substitution s/re/ore/"), L"fore:#006633; back:#FFF1A8" },
	{ SCE_PL_BACKTICKS, NP2STYLE_Backticks, EDITSTYLE_HOLE(L"Backticks"), L"fore:#E24000; back:#FFF1A8" },
	{ SCE_PL_HERE_DELIM, NP2STYLE_HeredocDelimiter, EDITSTYLE_HOLE(L"Here-doc (Delimiter)"), L"fore:#648000" },
	{ SCE_PL_HERE_Q, NP2STYLE_HeredocSingleQuoted, EDITSTYLE_HOLE(L"Here-doc (Single Quoted, q)"), L"fore:#648000" },
	{ SCE_PL_HERE_QQ, NP2STYLE_HeredocDoubleQuoted, EDITSTYLE_HOLE(L"Here-doc (Double Quoted, qq)"), L"fore:#648000" },
	{ SCE_PL_HERE_QX, NP2STYLE_HeredocBackticks, EDITSTYLE_HOLE(L"Here-doc (Backticks, qx)"), L"fore:#E24000; back:#FFF1A8" },
	{ SCE_PL_STRING_Q, NP2STYLE_SingleQuotedString_q, EDITSTYLE_HOLE(L"Single Quoted String (Generic, q)"), L"fore:#008000" },
	{ SCE_PL_STRING_QQ, NP2STYLE_DoubleQuotedString_qq, EDITSTYLE_HOLE(L"Double Quoted String (qq)"), L"fore:#008000" },
	{ SCE_PL_STRING_QX, NP2STYLE_Backticks_qx, EDITSTYLE_HOLE(L"Backticks (qx)"), L"fore:#E24000; back:#FFF1A8" },
	{ SCE_PL_STRING_QR, NP2STYLE_Regex_qr, EDITSTYLE_HOLE(L"Regex (qr)"), L"fore:#006633; back:#FFF1A8" },
	{ SCE_PL_STRING_QW, NP2STYLE_Array_qw, EDITSTYLE_HOLE(L"Array (qw)"), L"fore:#003CE6" },
	{ SCE_PL_SUB_PROTOTYPE, NP2STYLE_Prototype, EDITSTYLE_HOLE(L"Prototype"), L"fore:#800080; back:#FFE2FF" },
	{ SCE_PL_FORMAT_IDENT, NP2STYLE_FormatIdentifier, EDITSTYLE_HOLE(L"Format Identifier"), L"bold; fore:#648000; back:#FFF1A8" },
	{ SCE_PL_FORMAT, NP2STYLE_FormatBody, EDITSTYLE_HOLE(L"Format Body"), L"fore:#648000; back:#FFF1A8" },
	{ SCE_PL_POD, NP2STYLE_PODCommon, EDITSTYLE_HOLE(L"POD (Common)"), L"fore:#A46000; back:#FFFFC0; eolfilled" },
	{ SCE_PL_POD_VERB, NP2STYLE_PODVerbatim, EDITSTYLE_HOLE(L"POD (Verbatim)"), L"fore:#A46000; back:#FFFFC0; eolfilled" },
	{ SCE_PL_DATASECTION, NP2STYLE_DataSection, EDITSTYLE_HOLE(L"Data Section"), L"fore:#A46000; back:#FFFFC0; eolfilled" },
	{ SCE_PL_ERROR, NP2STYLE_ParsingError, EDITSTYLE_HOLE(L"Parsing Error"), L"fore:#C80000; back:#FFFF80" },
	// {SCE_PL_XLAT},
	//{ SCE_PL_PUNCTUATION, EDITSTYLE_HOLE(L"Symbols / Punctuation (not used)"), L"" },
	//{ SCE_PL_PREPROCESSOR, EDITSTYLE_HOLE(L"Preprocessor (not used)"), L"" },
	//{ SCE_PL_LONGQUOTE, EDITSTYLE_HOLE(L"Long Quote (qq, qr, qw, qx) (not used)"), L"" },
};

EDITLEXER lexPerl = {
	SCLEX_PERL, NP2LEX_PERL,
	EDITLEXER_HOLE(L"Perl Script", Styles_Perl),
	L"pl; pm; cgi; pod; plx; stp",
	&Keywords_Perl,
	Styles_Perl
};
