# Notepad2 and metapath

> *Notepad2* is a modified version (fork) of [Notepad2-mod](https://xhmikosr.github.io/notepad2-mod/). *metapath* is a bug-fixed version of original [metapath](http://www.flos-freeware.ch/archive.html).

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](../master/License.txt)

## Binaries
Latest release: [![Latest release](https://img.shields.io/github/release/zufuliu/notepad2.svg)](https://github.com/zufuliu/notepad2/releases)

Latest development builds (artifacts in Release configuration for each compiler and platform) are available on [GitHub Actions](https://github.com/zufuliu/notepad2/actions) and [AppVeyor](https://ci.appveyor.com/project/zufuliu/notepad2). Or you can build from source, see the [Wiki](https://github.com/zufuliu/notepad2/wiki/Build-Notepad2).

| GitHub Actions (MSVC, Clang, GCC) | AppVeyor (MSVC, Clang) |
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
	* [ABAQUS](../master/tools/lang/ABAQUS.inp)
	* [ActionScript](../master/tools/lang/ActionScript.as) for ActionScript 3 and 4.
	* Android Smali, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#android-smali)
		* javap
		* Jasmin
	* ANSI Art
	* [ANSYS](../master/tools/lang/APDL.cdb) Parametric Design Language
	* Assembler, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#assembler)
		* MASM
		* GNU Assembler
		* NASM
	* Asymptote Vector Graphics Language
	* AutoIt Script
	* [AviSynth Script](../master/tools/lang/AviSynth.avs), up to AviSynth 2.6, AviSynth+ 3.6, GScript 1.1.
	* [AWK](../master/tools/lang/Awk.awk), up to GNU AWK 5.1.
	* [Batch Script](../master/tools/lang/Batch.bat), [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#batch)
	* C#, Vala, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#c)
	* C/C++, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#cc)
		* Objective-C/C++, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#objective-cc)
		* [C Standard Library](../master/tools/lang/C.c), up to C2x.
		* [C++ STL](../master/tools/lang/CPP.cpp), up to C++20.
	* CSS Style Sheet
		* Sassy CSS
		* Less CSS
		* HSS
	* [CMake](../master/tools/lang/CMake.cmake), up to CMake 3.20. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#cmake)
	* Common Intermediate Language
	* Configuration / Properties File
		* Apache Configuration File
	* D
	* [Dart](../master/tools/lang/Dart.dart), up to Dart 2.12.
	* Diff/Patch File
	* Doxygen
	* F#
	* Fortran, GNU Fortran
	* [GN Build](../master/tools/lang/GN.gn), up to GN 2019-03-13.
	* [Go](../master/tools/lang/Go.go), up to Go 1.14.
		* Go+ (GoPlus) and the Q language
	* [Gradle](../master/tools/lang/Gradle.gradle), up to Gradle 6.8, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#gradle)
	* GraphViz Dot, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#graphviz)
		* blockdiag, seqdiag, actdiag and nwdiag
	* [Groovy](../master/tools/lang/Groovy.groovy), up to Groovy 4.0.
	* [Haxe](../master/tools/lang/Haxe.hx), up to Haxe 4.1
	* [HTML](../master/tools/lang/html.html), up to [WHATWG](https://html.spec.whatwg.org/multipage/) 2020-02-12. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#html)
		* ASP
		* ASP.NET
		* JSP
		* PHP
	* INI Configuration File
	* Inno Setup
	* [Boost Jam](../master/tools/lang/Jamfile.jam), up to Jam 4.4.1.
	* [Java](../master/tools/lang/Java.java), up to Java 16. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#java)
		* [BeanShell](https://github.com/beanshell/beanshell)
	* [JavaScript](../master/tools/lang/JavaScript.js), up to ECMAScript 2021 (12th edition) with [JSX](https://facebook.github.io/jsx/) and [Web APIs](https://developer.mozilla.org/en-US/docs/Web/API).
	* JSON and JSON5 Document
	* [Julia Script](../master/tools/lang/Julia.jl), up to Julia 1.2. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#julia)
	* [Kotlin](../master/tools/lang/Kotlin.kt), up to Kotlin 1.5.
	* LaTeX
	* Lisp Script (Common Lisp, Clojure, Scheme, etc.)
	* [LLVM IR](../master/tools/lang/LLVM.ll), up to LLVM 12.
	* [Lua Script](../master/tools/lang/Lua.lua), up to Lua 5.4.
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
	* [Nullsoft Scriptable Install System](../master/tools/lang/NSIS.nsi), up to NSIS 3.06.
	* Pascal, Delphi
	* Perl, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#perl)
	* PowerShell
	* [Python](../master/tools/lang/Python.py), up to Python 3.10. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#python)
	* PHP Script, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#php)
	* Windows Rescouce Script
	* [R](../master/tools/lang/R.r), up to R 4.0.3.
	* [REBOL](../master/tools/lang/Rebol.r) 3 and [Red](../master/tools/lang/Red.red)
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
		* [PostgreSQL](../master/tools/lang/PostgreSQL.sql), PostgreSQL 13.
		* [Transact-SQL](../master/tools/lang/Transact-SQL.sql), SQL Server 2019.
		* [Oracle](../master/tools/lang/Oracle.sql), Oracle 20, incomplete.
	* [Swift](../master/tools/lang/Swift.swift), Swift 5.5.
	* TCL, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#tcl)
	* Texinfo
	* TOML File
	* [TypeScript](../master/tools/lang/TypeScript.ts), TypeScript 4.3.
	* Visual Basic, VB6, VB.NET
	* VBScript
	* VHDL
	* Verilog HDL
	* [Vim Script](../master/tools/lang/Vim.vim)
	* [WebAssembly](https://github.com/WebAssembly/wabt/blob/master/src/lexer-keywords.txt), up to wabt 1.0.19.
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
	* [日本語](https://github.com/maboroshin/notepad2) by [@maboroshin](https://github.com/maboroshin)
	* [中文 (繁體)](https://github.com/zufuliu/notepad2/tree/master/locale/zh-Hant)
	* [中文 (简体)](https://github.com/zufuliu/notepad2/tree/master/locale/zh-Hans)
	* [한국어](https://github.com/VenusGirl/notepad2) by [@VenusGirl](https://github.com/VenusGirl)
	* [한국어](https://teus.me/category/IT/Notepad2) by [@bluenlive](https://github.com/bluenlive)
* [Notepad2 Wiki](https://github.com/zufuliu/notepad2/wiki)
* Other projects based on (original) Notepad2 or Notepad2-mod:
	* [Notepad3](https://github.com/rizonesoft/Notepad3)
	* [Notepad2e](https://github.com/ProgerXP/Notepad2e)
