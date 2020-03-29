#pragma once
#ifndef RC_INVOKED
#include <stdlib.h>
#include "compiler.h"
#include "Scintilla.h"
#include "SciLexer.h"

#define	NUMKEYWORD				(KEYWORDSET_MAX + 1)

#define MAX_EDITSTYLE_VALUE_SIZE	256
#define MAX_EDITLEXER_EXT_SIZE		512
#define MAX_LEXER_STYLE_EDIT_SIZE	512
#define MAX_EDITLEXER_NAME_SIZE		128
#define MAX_EDITSTYLE_NAME_SIZE		128

enum {
	StyleTheme_Default = 0,
	StyleTheme_Dark = 1,
	StyleTheme_Max = StyleTheme_Dark,
};

#ifndef _INC_WINDOWS
typedef wchar_t * LPWSTR;
typedef const wchar_t * LPCWSTR;
typedef int BOOL;
#endif

typedef struct EDITSTYLE {
	const int iStyle;
	struct {
		const int rid;
		const int iNameLen;
		LPCWSTR const pszName;
		LPWSTR szValue;
	};
	LPCWSTR const pszDefault;
} EDITSTYLE, *PEDITSTYLE;

#define EDITSTYLE_BufferSize(iStyleCount)	((iStyleCount) * MAX_EDITSTYLE_VALUE_SIZE * sizeof(WCHAR))

// Not used by Scintilla lexer, listed for auto completion.
#define KeywordAttr_NoLexer		1
// Convert to lower case before pass to Scintilla.
#define KeywordAttr_MakeLower	KEYWORDSET_TOLOWER
// Don't add to default auto completion list.
#define KeywordAttr_NoAutoComp	4

typedef struct KEYWORDLIST {
	const char * const pszKeyWords[NUMKEYWORD];
} KEYWORDLIST, *PKEYWORDLIST;

typedef struct EDITLEXER {
	const int iLexer;
	const int rid;
	struct {
		int iStyleTheme;
		BOOL bStyleChanged;
		BOOL bUseDefaultCodeStyle;
		int iFavoriteOrder;
		const unsigned int iStyleCount;
		const int iNameLen;
		LPCWSTR const pszName;
		LPWSTR szExtensions;
		LPWSTR szStyleBuf;
	};
	LPCWSTR const pszDefExt;
	const KEYWORDLIST * const pKeyWords;
	EDITSTYLE * const Styles;
} EDITLEXER, *PEDITLEXER;

typedef const EDITLEXER *LPCEDITLEXER;

#endif  // !RC_INVOKED

// NP2LEX_, rid for EDITLEXER
#define NP2LEX_TEXTFILE		63000	// SCLEX_NULL		Text File
#define NP2LEX_CPP			63001	// SCLEX_CPP		C/C++ Source
#define NP2LEX_JAVA			63002	// SCLEX_CPP		Java Source
#define NP2LEX_CSHARP		63003	// SCLEX_CPP		C# Source
#define NP2LEX_JS			63004	// SCLEX_CPP		JavaScript
#define NP2LEX_RC			63005	// SCLEX_CPP		Resource Script
//#define NP2LEX_IDL		63006	// SCLEX_CPP		Interface Definition Language
#define NP2LEX_D			63007	// SCLEX_CPP		D Source
#define NP2LEX_ASY			63008	// SCLEX_CPP		Asymptote Code
#define NP2LEX_CIL			63009	// SCLEX_CIL		CIL Assembly
//#define NP2LEX_OBJC		63010	// SCLEX_CPP		Objective C/C++
#define NP2LEX_AS			63011	// SCLEX_CPP		ActionScript
#define NP2LEX_HAXE			63012	// SCLEX_CPP		HaXe Script
#define NP2LEX_GROOVY		63013	// SCLEX_CPP		Groovy Script
#define NP2LEX_SCALA		63014	// SCLEX_CPP		Scala Script
#define NP2LEX_GO			63015	// SCLEX_CPP		Go Source
#define NP2LEX_GRADLE		63016	// SCLEX_CPP		Gradle Build Script

#define NP2LEX_VB			63021	// SCLEX_VB			Visual Basic
#define NP2LEX_FSHARP		63022	// SCLEX_FSHARP		F# Source
#define NP2LEX_ASM			63023	// SCLEX_ASM		Assembler Source
#define NP2LEX_PASCAL		63024	// SCLEX_PASCAL		Pascal Source
#define NP2LEX_XML			63025	// SCLEX_XML		XML Document
#define NP2LEX_HTML			63026	// SCLEX_HTML		Web Source Code
#define NP2LEX_CSS			63027	// SCLEX_CSS		CSS Style Sheet
#define NP2LEX_SQL			63028	// SCLEX_SQL		SQL Query
#define NP2LEX_PHP			63029	// SCLEX_CPP		PHP Script
#define NP2LEX_VBS			63030	// SCLEX_VBSCRIPT	VBScript
#define NP2LEX_PERL			63031	// SCLEX_PERL		Perl Script
#define NP2LEX_PYTHON		63032	// SCLEX_PYTHON		Python Script
#define NP2LEX_RUBY			63033	// SCLEX_RUBY		Ruby Script
#define NP2LEX_LUA			63034	// SCLEX_LUA		Lua Script
#define NP2LEX_TCL			63035	// SCLEX_TCL		Tcl Script
#define NP2LEX_BATCH		63036	// SCLEX_BATCH		Batch File
#define NP2LEX_BASH			63037	// SCLEX_BASH		Shell Script
#define NP2LEX_PS1			63038	// SCLEX_POWERSHELL	PowerShell Script
#define NP2LEX_AU3			63039	// SCLEX_AU3		AutoIt3 Script
#define NP2LEX_MATLAB		63040	// SCLEX_MATLAB		MATLAB Code
#define NP2LEX_FORTRAN		63041	// SCLEX_FORTRAN	Fortran Source
#define NP2LEX_LATEX		63042	// SCLEX_LATEX		LaTeX File
#define NP2LEX_VHDL			63043	// SCLEX_VHDL		VHDL Source
#define NP2LEX_VERILOG		63044	// SCLEX_VERILOG	Verilog HDL Source
#define NP2LEX_MAKE			63045	// SCLEX_MAKEFILE	Makefile
#define NP2LEX_CMAKE		63046	// SCLEX_CMAKE		CMake Script
#define NP2LEX_INNO			63047	// SCLEX_INNOSETUP	Inno Setup Script
#define NP2LEX_INI			63048	// SCLEX_PROPERTIES	Ini Config File
#define NP2LEX_CONF			63049	// SCLEX_CONF		Config File
#define NP2LEX_DIFF			63050	// SCLEX_DIFF		Diff/Patch
#define NP2LEX_AWK			63051	// SCLEX_CPP		Awk Script
#define NP2LEX_JAM			63052	// SCLEX_CPP		Jamfile
#define NP2LEX_DOT			63053	// SCLEX_GRAPHVIZ	GraphViz Dot
#define NP2LEX_JSON			63054	// SCLEX_JSON		JSON Document
#define NP2LEX_SMALI		63055	// SCLEX_SMALI		Android Smali
#define NP2LEX_NSIS			63056	// SCLEX_NSIS		Nsis Setup Script
#define NP2LEX_TEXINFO		63057	// SCLEX_TEXINFO	Texinfo File
#define NP2LEX_LISP			63058	// SCLEX_LISP		Lisp Script
#define NP2LEX_VIM			63059	// SCLEX_VIM		Vim Script
#define NP2LEX_LLVM			63060	// SCLEX_LLVM		LLVM IR
#define NP2LEX_OCTAVE		63061	// SCLEX_MATLAB		Octave Code
#define NP2LEX_SCILAB		63062	// SCLEX_MATLAB		SciLab Code

#define NP2LEX_JULIA		63066	// SCLEX_JULIA		Julia Script
#define NP2LEX_RUST			63067	// SCLEX_RUST		Rust Source
#define NP2LEX_KOTLIN		63068	// SCLEX_KOTLIN		Kotlin Source
#define NP2LEX_YAML			63069	// SCLEX_YAML		YAML Document
#define NP2LEX_WASM			63070	// SCLEX_WASM		WebAssembly
#define NP2LEX_TOML			63071	// SCLEX_TOML		TOML File
#define NP2LEX_GN			63072	// SCLEX_GN			GN Build Script

#define NP2LEX_2NDTEXTFILE	63097	// SCLEX_NULL		2nd Text File
#define NP2LEX_ANSI			63098	// SCLEX_NULL		ANSI Art
#define NP2LEX_2NDGLOBAL	63099	// SCLEX_NULL		2nd Global Styles
#define NP2LEX_GLOBAL		63100	// SCLEX_NULL		Global Styles
