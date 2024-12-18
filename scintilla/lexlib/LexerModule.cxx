// Scintilla source code edit control
/** @file LexerModule.cxx
 ** Colourise for particular languages.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cassert>
#include <cstring>

#include <string>
#include <string_view>
#include <vector>
//#include <map>

#include "ILexer.h"
#include "Scintilla.h"
//#include "SciLexer.h"

#include "PropSetSimple.h"
#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "LexerModule.h"
#include "LexerBase.h"

using namespace Lexilla;

//++Autogenerated -- run scripts/LexGen.py to regenerate
//**\(extern const LexerModule \*;\n\)
extern const LexerModule lmAPDL;
extern const LexerModule lmAsm;
extern const LexerModule lmAsymptote;
extern const LexerModule lmAutoHotkey;
extern const LexerModule lmAutoIt3;
extern const LexerModule lmAviSynth;
extern const LexerModule lmAwk;
extern const LexerModule lmBash;
extern const LexerModule lmBatch;
extern const LexerModule lmCangjie;
extern const LexerModule lmCIL;
extern const LexerModule lmCMake;
extern const LexerModule lmCoffeeScript;
extern const LexerModule lmConfig;
extern const LexerModule lmCPP;
extern const LexerModule lmCSharp;
extern const LexerModule lmCSS;
extern const LexerModule lmCSV;
extern const LexerModule lmDart;
extern const LexerModule lmDiff;
extern const LexerModule lmDLang;
extern const LexerModule lmFortran;
extern const LexerModule lmFSharp;
extern const LexerModule lmGN;
extern const LexerModule lmGo;
extern const LexerModule lmGraphViz;
extern const LexerModule lmGroovy;
extern const LexerModule lmHaskell;
extern const LexerModule lmHaxe;
extern const LexerModule lmHTML;
extern const LexerModule lmInnoSetup;
extern const LexerModule lmJamfile;
extern const LexerModule lmJava;
extern const LexerModule lmJavaScript;
extern const LexerModule lmJSON;
extern const LexerModule lmJulia;
extern const LexerModule lmKotlin;
extern const LexerModule lmLatex;
extern const LexerModule lmLisp;
extern const LexerModule lmLLVM;
extern const LexerModule lmLua;
extern const LexerModule lmMakefile;
extern const LexerModule lmMarkdown;
extern const LexerModule lmMathematica;
extern const LexerModule lmMatlab;
extern const LexerModule lmNim;
extern const LexerModule lmNsis;
extern const LexerModule lmNull;
extern const LexerModule lmOCaml;
extern const LexerModule lmPascal;
extern const LexerModule lmPerl;
extern const LexerModule lmPHPScript;
extern const LexerModule lmPowerShell;
extern const LexerModule lmProps;
extern const LexerModule lmPython;
extern const LexerModule lmRebol;
extern const LexerModule lmRLang;
extern const LexerModule lmRuby;
extern const LexerModule lmRust;
extern const LexerModule lmSAS;
extern const LexerModule lmScala;
extern const LexerModule lmSmali;
extern const LexerModule lmSQL;
extern const LexerModule lmSwift;
extern const LexerModule lmTCL;
extern const LexerModule lmTexinfo;
extern const LexerModule lmTOML;
extern const LexerModule lmVerilog;
extern const LexerModule lmVHDL;
extern const LexerModule lmVim;
extern const LexerModule lmVisualBasic;
extern const LexerModule lmWASM;
extern const LexerModule lmWinHex;
extern const LexerModule lmXML;
extern const LexerModule lmYAML;
extern const LexerModule lmZig;

//--Autogenerated -- end of automatically generated section

namespace {

const LexerModule * const lexerCatalogue[] = {
//++Autogenerated -- run scripts/LexGen.py to regenerate
//**\(\t&\*,\n\)
	&lmAPDL,
	&lmAsm,
	&lmAsymptote,
	&lmAutoHotkey,
	&lmAutoIt3,
	&lmAviSynth,
	&lmAwk,
	&lmBash,
	&lmBatch,
	&lmCangjie,
	&lmCIL,
	&lmCMake,
	&lmCoffeeScript,
	&lmConfig,
	&lmCPP,
	&lmCSharp,
	&lmCSS,
	&lmCSV,
	&lmDart,
	&lmDiff,
	&lmDLang,
	&lmFortran,
	&lmFSharp,
	&lmGN,
	&lmGo,
	&lmGraphViz,
	&lmGroovy,
	&lmHaskell,
	&lmHaxe,
	&lmHTML,
	&lmInnoSetup,
	&lmJamfile,
	&lmJava,
	&lmJavaScript,
	&lmJSON,
	&lmJulia,
	&lmKotlin,
	&lmLatex,
	&lmLisp,
	&lmLLVM,
	&lmLua,
	&lmMakefile,
	&lmMarkdown,
	&lmMathematica,
	&lmMatlab,
	&lmNim,
	&lmNsis,
	&lmNull,
	&lmOCaml,
	&lmPascal,
	&lmPerl,
	&lmPHPScript,
	&lmPowerShell,
	&lmProps,
	&lmPython,
	&lmRebol,
	&lmRLang,
	&lmRuby,
	&lmRust,
	&lmSAS,
	&lmScala,
	&lmSmali,
	&lmSQL,
	&lmSwift,
	&lmTCL,
	&lmTexinfo,
	&lmTOML,
	&lmVerilog,
	&lmVHDL,
	&lmVim,
	&lmVisualBasic,
	&lmWASM,
	&lmWinHex,
	&lmXML,
	&lmYAML,
	&lmZig,

//--Autogenerated -- end of automatically generated section
};

}

const LexerModule *LexerModule::Find(int language_) noexcept {
	for (const LexerModule *lm : lexerCatalogue) {
		if (lm->GetLanguage() == language_) {
			return lm;
		}
	}
	return &lmNull;
}

Scintilla::ILexer5 *LexerModule::Create() const {
	if (fnFactory) {
		return fnFactory();
	}
	return new LexerBase(this);
}
