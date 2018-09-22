#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikipedia.org/wiki/Almquist_shell
// https://en.wikipedia.org/wiki/Bourne_shell
// https://www.gnu.org/software/bash/manual/
// https://en.wikipedia.org/wiki/C_shell
// https://en.wikipedia.org/wiki/KornShell
// https://en.wikipedia.org/wiki/Z_shell

static KEYWORDLIST Keywords_Bash = {{
// build-in
"alias true false yes no if then fi elif else for function functions in return while "
" case esac eval ex exec do done du echo EOF "
// C Shell
"end endif foreach switch default breaksw endsw repeat "
// common
"ar asa awk banner basename bash bc bdiff break bunzip2 bzip2 cal calendar cat "
"cc cd chmod cksum clear cmp col comm compress continue cp cpio crypt csplit ctags cut date "
"dc dd declare deroff dev df diff diff3 dircmp dirname ed egrep "
"env exit expand export expr fc fgrep file find fmt fold  "
"getconf getopt getopts grep gres hash head help history iconv id integer "
"jobs join kill local lc let line ln logname look ls m4 mail mailx make man mkdir more mt mv "
"newgrp nl nm nohup ntps od pack paste patch pathchk pax pcat perl pg pr print printf ps pwd "
"read readonly red rev rm rmdir sed select set sh shift size sleep sort spell split "
"start stop strings strip stty sum suspend sync tail tar tee test time times touch tr "
"trap tsort tty type typeset ulimit umask unalias uname uncompress unexpand uniq unpack "
"unset until uudecode uuencode vi vim vpax wait wc whence which who wpaste wstart xargs "
"zcat chgrp chown chroot dir dircolors factor groups hostid install link md5sum mkfifo mknod "
"nice pinky printenv ptx readlink seq sha1sum shred stat su tac unlink users vdir whoami yes dnl"

, // 1
"$PATH $MANPATH $INFOPATH $USER $TMP $TEMP $PRINTER $HOSTNAME $PS1 $SHELL "
"$JAVA_HOME $JAVA_OPTS $CATALINA_HOME $CATALINA_BASE $CATALINA_OPTS "

, "", "", "", "", "", "", ""

, "", "", "", "", "", "", ""
}};

GCC_NO_WARNING_MISSING_BRACES_BEGIN

EDITLEXER lexBash = { SCLEX_BASH, NP2LEX_BASH, EDITLEXER_HOLE(L"Shell Script"), L"sh; csh; zsh; bash; tcsh; m4; in; po", &Keywords_Bash,
{
	EDITSTYLE_DEFAULT,
	//{ SCE_SH_ERROR, 63531, L"Error", L"", L"" },
	{ MULTI_STYLE(SCE_SH_COMMENTLINE, 0, 0, 0), NP2STYLE_Comment, L"Comment", L"fore:#608060", L"" },
	{ SCE_SH_WORD, NP2STYLE_Keyword, L"Keyword", L"bold; fore:#FF8000", L"" },
	{ SCE_SH_STRING, 63532, L"Double Quoted String", L"fore:#008080", L"" },
	{ SCE_SH_CHARACTER, 63533, L"Single Quoted String", L"fore:#800080", L"" },
	{ SCE_SH_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_SH_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ SCE_SH_SCALAR, 63534, L"Scalar", L"fore:#808000", L"" },
	{ SCE_SH_PARAM, 63535, L"Parameter Expansion", L"fore:#808000; back:#FFFF99", L"" },
	{ SCE_SH_BACKTICKS, NP2STYLE_Backticks, L"Backticks", L"fore:#FF0080", L"" },
	{ SCE_SH_HERE_DELIM, 63537, L"Here-doc (Delimiter)", L"fore:#A46000; back:#FFFFC0; eolfilled", L"" },
	{ SCE_SH_HERE_Q, 63538, L"Here-doc (Single Quoted, q)", L"fore:#A46000; back:#FFFFC0; eolfilled", L"" },
	EDITSTYLE_SENTINEL
}
};

GCC_NO_WARNING_MISSING_BRACES_END
