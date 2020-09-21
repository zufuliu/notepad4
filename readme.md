# Notepad2 and metapath

> *Notepad2* is a modified version (fork) of [Notepad2-mod](https://xhmikosr.github.io/notepad2-mod/). *metapath* is a bug-fixed version of original [metapath](http://www.flos-freeware.ch/archive.html).

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](../master/License.txt)

## Binaries
Latest release: [![Latest release](https://img.shields.io/github/release/zufuliu/notepad2.svg)](https://github.com/zufuliu/notepad2/releases)

Latest development builds (artifacts in Release configuration for each compiler and platform) are available on [GitHub Actions](https://github.com/zufuliu/notepad2/actions) and [AppVeyor](https://ci.appveyor.com/project/zufuliu/notepad2). Or you can build from source, see the [Wiki](https://github.com/zufuliu/notepad2/wiki/Build-Notepad2).

| GitHub Actions | AppVeyor |
| --------------- | -------- |
| [![Build status](https://github.com/zufuliu/notepad2/workflows/CI/badge.svg)](https://github.com/zufuliu/notepad2/actions) | [![Build status](https://ci.appveyor.com/api/projects/status/github/zufuliu/notepad2?branch=master&svg=true)](https://ci.appveyor.com/project/zufuliu/notepad2) |

## [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots)
![v4.20.04](https://raw.githubusercontent.com/wiki/zufuliu/notepad2/screenshots/v4.20.04.png)

## Changes compared to the official [Notepad2](http://www.flos-freeware.ch/notepad2.html) or [Notepad2-mod](https://xhmikosr.github.io/notepad2-mod/):

* Support for bookmarks
* Option to mark all occurrences of a word or selection, and show matched count
* [Enhanced auto-completion for words and functions](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2)
* [Context-based auto-completion for some words](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#context-based-auto-completion)
* [Enhanced auto indentation](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#auto-indent-text)
* [Auto completion for braces, brackets and quotes](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#auto-insert-text)
* Syntax highlighting, code folding and API list for (see built-in [file extension list](../master/doc/FileExt.txt)):
	* Plain Text
	* ActionScript
	* Android Smali, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#android-smali)
		* javap
		* Jasmin
	* ANSI Art
	* Assembler, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#assembler)
		* MASM
		* GNU Assembler
		* NASM
	* Asymptote Vector Graphics Language
	* AutoIt Script
	* AWK, GNU AWK
	* Batch Script, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#batch)
	* C#, Vala, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#c)
	* C/C++, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#cc)
		* Objective-C/C++, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#objective-cc)
		* [C Standard Library](../master/tools/lang/C.c), up to C2x.
		* [C++ STL](../master/tools/lang/CPP.cpp), up to C++20.
	* CSS Style Sheet
		* Sassy CSS
		* Less CSS
		* HSS
	* [CMake](../master/tools/lang/CMake.cmake), up to CMake 3.18. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#cmake)
	* Common Intermediate Language
	* Configuration / Properties File
		* Apache Configuration File
	* D
	* Diff/Patch File
	* Doxygen
	* F#
	* Fortran, GNU Fortran
	* [GN Build](../master/tools/lang/GN.gn), up to GN 2019-03-13.
	* [Go](../master/tools/lang/Go.go), up to Go 1.14.
		* Go+ (GoPlus) and the Q language
	* Gradle, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#gradle)
	* GraphViz Dot, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#graphviz)
		* blockdiag, seqdiag, actdiag and nwdiag
	* Groovy
	* Haxe
	* [HTML](../master/tools/lang/html.html), up to [WHATWG](https://html.spec.whatwg.org/multipage/) 2020-02-12. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#html)
		* ASP
		* ASP.NET
		* JSP
		* PHP
	* INI Configuration File
	* Inno Setup
	* Boost Jam
	* Java, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#java)
	* [JavaScript](../master/tools/lang/JavaScript.js), up to ECMAScript 2020 (11th edition) with [Web APIs](https://developer.mozilla.org/en-US/docs/Web/API).
	* JSON and JSON5 Document
	* [Julia Script](../master/tools/lang/Julia.jl), up to Julia 1.2. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#julia)
	* [Kotlin](../master/tools/lang/Kotlin.kt), up to Kotlin 1.3
	* LaTeX
	* Lisp Script (Common Lisp, Clojure, Scheme, etc.)
	* [LLVM IR](../master/tools/lang/LLVM.ll), up to LLVM 10.
	* Lua Script
	* Makefile, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#makefile)
		* nmake
		* gmake
		* Borland bmake
		* qmake
		* ninja
	* MATLAB, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#matlab-octave)
		* Octave
		* Scilab
		* Gnuplot
	* NSIS
	* Pascal, Delphi
	* Perl, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#perl)
	* PowerShell
	* Python, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#python)
		* IronPython
		* Cobra
		* Jython
		* Boo
	* PHP Script, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#php)
	* Windows Rescouce Script
	* [Ruby](../master/tools/lang/Ruby.rb), [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#ruby)
	* [Rust](../master/tools/lang/Rust.rs), up to Rust 1.38. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#rust)
	* Scala Script
	* Shell Script
		* Bash likes (sh, bash, dash, ash, zsh, ksh), [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#shell-script)
		* C Shell (csh, tcsh)
		* M4 Macro, limited support
	* SQL Query
		* [SQL Standard](../master/tools/lang/SQL.sql), up to SQL:2016.
		* [MySQL](../master/tools/lang/MySQL.sql), up to MySQL 8.0, MariaDB 10.3.
		* [SQLite3](../master/tools/lang/SQLite3.sql), up to 3.33.
		* [PostgreSQL](../master/tools/lang/PostgreSQL.sql), up to 12.
		* [Transact-SQL](../master/tools/lang/Transact-SQL.sql), up to SQL Server 2019.
		* [Oracle](../master/tools/lang/Oracle.sql), up Oracle 20, incomplete.
	* TCL, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#tcl)
	* Texinfo
	* TOML File
	* Visual Basic, VB6, VB.NET
	* VBScript
	* VHDL
	* Verilog HDL
	* [Vim Script](../master/tools/lang/Vim.vim)
	* [WebAssembly](https://github.com/WebAssembly/wabt/blob/master/src/lexer-keywords.txt), up to wabt 1.0.19
	* XML Document, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#xml)
		* XML Schema
		* XML Stylesheet
		* SGML and DTD
		* Ant Build
		* Maven POM
		* Maven Settings
		* Ivy Module
		* Ivy Settings
		* PMD Ruleset
		* Checkstyle Module
		* Tomcat Config
		* Java web.xml
		* Struts config
		* Hibernate config and mapping
		* Spring Beans
		* Apple Property List
		* Android Manifest
	* YAML Document
	* Many Others
* Toggle fold for specific level, shortcuts: <kbd>Alt</kbd>+<kbd>Number</kbd>
* Toggle fold for current block without scrolling back to the block start line, shortcut: <kbd>Alt</kbd>+<kbd>C</kbd>, or context menu "Toggle Fold"
* Improved support for Chinese, Japanese and Korean: inline mode IME, word wrapping, word selection, case conversions, etc.
* Insert new GUID and current timestamps
* Number conversion between decimal, hexadecimal, octal and binary
* Display caret location and length of selected text in count of characters
* Improved support for NFO ANSI art
* Support various [Windows system integration](https://github.com/zufuliu/notepad2/wiki/System-Integration)
* Per-Monitor DPI-Aware, improved high DPI display
* Support rendering using Direct2D DirectWrite, and switch between GDI and Direct2D
* Support font ligature with font like [FiraCode](https://github.com/tonsky/FiraCode) in Direct2D mode
* Support color font (Windows 8.1 and above) in Direct2D mode, useful to display color [Emoji](http://unicode.org/emoji/charts/full-emoji-list.html) 😊 and others
* Experimental support for RTL layout (GDI) and bidirectional (Direct2D), useful for Arabic and Hebrew
* Support using fractional font size
* Support full screen mode
* Extra support for drag and drop file from Visual Studio, Android Studio, IntelliJ IDEA, etc.
* Other various major or minor changes and tweaks

## Supported Operating Systems:
* Windows XP, Vista, 7, 8, 8.1 and 10, both 32-bit and 64-bit
* Windows Server 2003, 2008, 2012 and 2016, both 32-bit and 64-bit
* Windows 10 on ARM64, Windows 8/8.1 on ARM32 (Windows RT)
* Unix-like Operating Systems (Linux, macOS, BSD, etc.) using [Wine](https://www.winehq.org/)
* [ReactOS](https://www.reactos.org/)

## Notes:
* If you find any bugs or have any suggestions feel free to **write proposals/issues** or **provide patches/pull requests**.

## Contributors:
* [XhmikosR](https://xhmikosr.github.io/notepad2-mod/)
* [Kai Liu](https://code.kliu.org/misc/notepad2/)
* [RL Vision](https://www.rlvision.com/notepad2/about.asp)
* Aleksandar Lekov
* Bruno Barbieri
* Homam
* Everyone else on [GitHub](https://github.com/zufuliu/notepad2/graphs/contributors) and on Notepad2-mod's [Contributor List](https://github.com/XhmikosR/notepad2-mod/graphs/contributors)

## More information:
* [Official Notepad2 website](http://www.flos-freeware.ch/notepad2.html)
* [Official Notepad2-mod website](https://xhmikosr.github.io/notepad2-mod/)
* [Scintilla's website](https://www.scintilla.org/)
* [Build Notepad2 and metapath](https://github.com/zufuliu/notepad2/wiki/Build-Notepad2)
* [Localization](https://github.com/zufuliu/notepad2/wiki/Localization)
	* [日本語](https://github.com/maboroshin/notepad2)
	* [中文 (繁體)](https://github.com/zufuliu/notepad2/tree/master/locale/zh-Hant)
	* [中文 (简体)](https://github.com/zufuliu/notepad2/tree/master/locale/zh-Hans)
	* [한국어](https://teus.me/category/IT/Notepad2)
* [Notepad2 Wiki](https://github.com/zufuliu/notepad2/wiki)
* Other projects based on (original) Notepad2 or Notepad2-mod:
	* [Notepad3](https://github.com/rizonesoft/Notepad3)
	* [Notepad2e](https://github.com/ProgerXP/Notepad2e)
