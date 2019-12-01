#include "EditLexer.h"
#include "EditStyleX.h"

// https://docs.microsoft.com/en-us/cpp/build/nmake-reference
// https://www.gnu.org/software/make/
// https://doc.qt.io/qt-5/qmake-manual.html

static KEYWORDLIST Keywords_MAK = {{
// gmake
"define endef undefine ifdef ifndef ifeq ifneq else endif include -include sinclude override export unexport private vpath load "

, // 1 nmake Preprocessor
"!CMDSWITCHES !ERROR !MESSAGE !INCLUDE !IF !ELSE !ELSEIF !IFDEF !IFNDEF !ELSEIFDEF !ELSEIFNDEF !ENDIF !UNDEF "
"!cmdswitches !error !message !include !if !else !elseif !ifdef !ifndef !elseifdef !elseifndef !endif !undef "
// Borland
"!LOADDLL !WARNING "
"!loaddll warning "

, // 2 gmake Function
"subst patsubst strip findstring filter filter-out sort word wordlist words firstword lastword "
"dir notdir suffix basename addsuffix addprefix join wildcard realpath abspath "
"errno warning info shell origin flavor "
"foreach if or and "
"call eval file value "
"guile "

, // 3 Special Built-in Target
"all clean "
".IGNORE .PHONY .SUFFIXES .PRECIOUS .SILENT "
".DEFAULT .INTERMEDIATE .SECONDARY .SECONDEXPANSION .DELETE_ON_ERROR .LOW_RESOLUTION_TIME .EXPORT_ALL_VARIABLES .NOTPARALLEL .ONESHELL .POSIX "

, // 4
"VPATH SHELL "
"CC CXX "
"CFLAGS CXXFLAGS LDFLAGS LDLIBS "

, // 5 bmake
"include if endif error for endfor else "

, // 6 bmake
"in defined empty exists "

, // 7 ninja
"rule build default "

, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_MAK[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_MAKE_COMMENT, NP2StyleX_Comment, L"fore:#008000" },
	{ SCE_MAKE_PREPROCESSOR, NP2StyleX_Preprocessor, L"fore:#FF8000" },
	{ SCE_MAKE_FUNCTION, NP2StyleX_Function, L"fore:#FF8000" },
	{ MULTI_STYLE(SCE_MAKE_VARIABLE, SCE_MAKE_VARIABLE2, SCE_MAKE_VARIABLE3, 0), NP2StyleX_Variable, L"fore:#003CE6" },
	{ SCE_MAKE_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
	{ SCE_MAKE_TARGET, NP2StyleX_Target, L"fore:#003CE6; back:#FFC000" },
};

EDITLEXER lexMake = {
	SCLEX_MAKEFILE, NP2LEX_MAKE,
	EDITLEXER_HOLE(L"Makefile", Styles_MAK),
	L"mak; make; mk; dsp; dsw; am; pro; pri; gmk; ninja",
	&Keywords_MAK,
	Styles_MAK
};
