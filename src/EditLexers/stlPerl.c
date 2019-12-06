#include "EditLexer.h"
#include "EditStyleX.h"

// https://www.perl.org/
// https://perldoc.perl.org/index-language.html
// https://learn.perl.org/docs/keywords.html

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
	{ SCE_PL_COMMENTLINE, NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_PL_WORD, NP2StyleX_Keyword, L"bold; fore:#FF8000" },
	{ MULTI_STYLE(SCE_PL_STRING, SCE_PL_CHARACTER, 0, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_PL_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_PL_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_PL_SCALAR, SCE_PL_STRING_VAR, 0, 0), NP2StyleX_ScalarVar, L"fore:#0000FF" },
	{ SCE_PL_ARRAY, NP2StyleX_ArrayVar, L"fore:#FF0080" },
	{ SCE_PL_HASH, NP2StyleX_HashVar, L"fore:#B000B0" },
	{ SCE_PL_SYMBOLTABLE, NP2StyleX_SymbolTableVar, L"fore:#3A6EA5" },
	{ SCE_PL_REGEX, NP2StyleX_Regex_mre, L"fore:#006633; back:#FFF1A8" },
	{ SCE_PL_REGSUBST, NP2StyleX_Substitution_re, L"fore:#006633; back:#FFF1A8" },
	{ SCE_PL_BACKTICKS, NP2StyleX_Backticks, L"fore:#E24000; back:#FFF1A8" },
	{ SCE_PL_HERE_DELIM, NP2StyleX_HeredocDelimiter, L"fore:#648000" },
	{ SCE_PL_HERE_Q, NP2StyleX_HeredocSingleQuoted, L"fore:#648000" },
	{ SCE_PL_HERE_QQ, NP2StyleX_HeredocDoubleQuoted, L"fore:#648000" },
	{ SCE_PL_HERE_QX, NP2StyleX_HeredocBackticks, L"fore:#E24000; back:#FFF1A8" },
	{ SCE_PL_STRING_Q, NP2StyleX_SingleQuotedString_q, L"fore:#008000" },
	{ SCE_PL_STRING_QQ, NP2StyleX_DoubleQuotedString_qq, L"fore:#008000" },
	{ SCE_PL_STRING_QX, NP2StyleX_Backticks_qx, L"fore:#E24000; back:#FFF1A8" },
	{ SCE_PL_STRING_QR, NP2StyleX_Regex_qr, L"fore:#006633; back:#FFF1A8" },
	{ SCE_PL_STRING_QW, NP2StyleX_Array_qw, L"fore:#003CE6" },
	{ SCE_PL_SUB_PROTOTYPE, NP2StyleX_Prototype, L"fore:#800080; back:#FFE2FF" },
	{ SCE_PL_FORMAT_IDENT, NP2StyleX_FormatIdentifier, L"bold; fore:#648000; back:#FFF1A8" },
	{ SCE_PL_FORMAT, NP2StyleX_FormatBody, L"fore:#648000; back:#FFF1A8" },
	{ SCE_PL_POD, NP2StyleX_PODCommon, L"fore:#A46000; back:#FFFFC0; eolfilled" },
	{ SCE_PL_POD_VERB, NP2StyleX_PODVerbatim, L"fore:#A46000; back:#FFFFC0; eolfilled" },
	{ SCE_PL_DATASECTION, NP2StyleX_DataSection, L"fore:#A46000; back:#FFFFC0; eolfilled" },
	{ SCE_PL_ERROR, NP2StyleX_ParsingError, L"fore:#C80000; back:#FFFF80" },
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
