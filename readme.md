# Notepad4 and matepath

Notepad4 (Notepad2⨯2, Notepad2++) is a light-weight Scintilla based text editor for Windows with syntax highlighting, code folding, auto-completion and API list for many programming languages and documents, it's based on (rewritten in modern C++) Florian's [Notepad2](https://www.flos-freeware.ch/notepad2.html) and XhmikosR's [Notepad2-mod](https://xhmikosr.github.io/notepad2-mod/). matepath is a file browser plugin and based on (rewritten in modern C++) Florian's [metapath](https://www.flos-freeware.ch/archive.html).

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](License.txt) [![Latest release](https://img.shields.io/github/release/zufuliu/notepad4.svg)](https://github.com/zufuliu/notepad4/releases) [![Downloads](https://img.shields.io/github/downloads/zufuliu/notepad4/total.svg)](https://github.com/zufuliu/notepad4/releases)

## Binaries
Latest development builds (artifacts in Release configuration for each compiler and platform) are available on [GitHub Actions](https://github.com/zufuliu/notepad4/actions) and [AppVeyor](https://ci.appveyor.com/project/zufuliu/notepad4). Or you can build from source, see [Wiki](https://github.com/zufuliu/notepad4/wiki/Build-Notepad4).

| GitHub Actions (MSVC, Clang, GCC) | AppVeyor (MSVC, Clang) |
| --------------- | -------- |
| [![Build status](https://github.com/zufuliu/notepad4/workflows/CI/badge.svg)](https://github.com/zufuliu/notepad4/actions) | [![Build status](https://ci.appveyor.com/api/projects/status/github/zufuliu/notepad4?branch=main&svg=true)](https://ci.appveyor.com/project/zufuliu/notepad4) |

## [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots)
![v24.07 HD](https://raw.githubusercontent.com/wiki/zufuliu/notepad4/screenshots/v24.07HD.png)

## Changes compared to [Notepad2](https://www.flos-freeware.ch/notepad2.html) or [Notepad2-mod](https://xhmikosr.github.io/notepad2-mod/):

* Support for bookmarks
* Option to mark all occurrences of a word or selection, and show matched count
* [Enhanced auto-completion for words and functions](https://github.com/zufuliu/notepad4/wiki/Auto-Completion-in-Notepad4)
* [Context-based auto-completion for some words](https://github.com/zufuliu/notepad4/wiki/Auto-Completion-in-Notepad4#context-based-auto-completion)
* [Enhanced auto indentation](https://github.com/zufuliu/notepad4/wiki/Auto-Completion-in-Notepad4#auto-indent-text)
* [Auto completion for braces, brackets and quotes](https://github.com/zufuliu/notepad4/wiki/Auto-Completion-in-Notepad4#auto-insert-text)
* [Color preview via CallTip and click CallTip to open color dialog](https://github.com/zufuliu/notepad4/wiki/CallTip)
* Support Base64 encoding and decoding
* Syntax highlighting, code folding and API list for (see built-in [file extension list](doc/FileExt.txt)):
	* Plain Text
	* [ABAQUS](tools/lang/ABAQUS.inp)
	* [ActionScript](tools/lang/ActionScript.as) for ActionScript 3 and 4.
	* Android Smali, [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#android-smali)
		* [javap](https://docs.oracle.com/en/java/javase/17/docs/specs/man/javap.html)
		* [Jasmin](https://jasmin.sourceforge.net/)
	* ANSI Art
	* [ANSYS](tools/lang/APDL.cdb) Parametric Design Language
	* Assembler, [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#assembler)
		* MASM
		* GNU Assembler
		* [NASM](https://nasm.us/)
	* [Asymptote Vector Graphics Language](tools/lang/Asymptote.asy), up to Asymptote 2.70.
	* [AutoHotkey_L](tools/lang/AutoHotkey_L.ahk) and [AutoHotkey_H](tools/lang/AutoHotkey_H.ahk) Script, up to v2.
	* [AutoIt3](tools/lang/AutoIt3.au3) Script, up to 3.3.16.
	* [AviSynth Script](tools/lang/AviSynth.avs), up to AviSynth 2.6, AviSynth+ 3.6, GScript 1.1.
	* [AWK](tools/lang/Awk.awk), up to GNU AWK 5.3.
	* [Batch Script](tools/lang/Batch.bat), [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#batch)
	* [blockdiag](tools/lang/blockdiag.diag), seqdiag, actdiag and nwdiag
	* [C#](tools/lang/CSharp.cs), up to C# 12.0. [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#c)
		* [Vala](https://wiki.gnome.org/Projects/Vala) 0.54
	* C/C++, [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#cc)
		* Objective-C/C++, [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#objective-cc)
		* [C Standard Library](tools/lang/C.c), up to C2x.
		* [C++ STL](tools/lang/CPP.cpp), up to C++20.
	* [Cangjie (仓颉)](tools/lang/Cangjie.cj), up to June 2024.
	* [CSS Style Sheet](tools/lang/CSS.css), up to September 2024.
		* [SCSS Style Sheet](tools/lang/SCSS.scss), up to Dart Sass 1.50.
		* [Less Style Sheet](tools/lang/Less.less), up to Less 4.1.
		* [HSS Style Sheet](tools/lang/HSS.hss)
	* [CMake](tools/lang/CMake.cmake), up to CMake 3.29. [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#cmake)
	* [CoffeeScript](tools/lang/CoffeeScript.coffee), CoffeeScript 2.7.
	* Common Intermediate Language
	* Configuration / Properties File
		* Apache Configuration File
	* [CSV File](https://www.rfc-editor.org/rfc/rfc4180)
	* [D](tools/lang/D.d), up to D language 2.0.
	* [Dart](tools/lang/Dart.dart), up to Dart 3.4.
	* Diff/Patch File
	* [F#](tools/lang/FSharp.fs), up to F# 8.0.
	* [Fortran](tools/lang/Fortran.f), up to Fortran 2023.
	* [GN Build](tools/lang/GN.gn), up to GN 2019-03-13.
	* [Go](tools/lang/Go.go), up to Go 1.21.
		* [Go+ (GoPlus)](https://goplus.org/) and the Q language
	* [Gradle](tools/lang/Gradle.gradle), up to Gradle 8.3, [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#gradle)
	* [GraphViz Dot](tools/lang/GraphViz.dot), up to GraphViz 2.49, [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#graphviz)
	* [Groovy](tools/lang/Groovy.groovy), up to Groovy 5.0.
	* [Haskell](tools/lang/Haskell.hs), up to GHC 9.6.
	* [Haxe](tools/lang/Haxe.hx), up to Haxe 4.3.
	* [HTML](tools/lang/html.html), up to [WHATWG](https://html.spec.whatwg.org/multipage/) September 2024. [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#html)
		* ASP
		* ASP.NET
		* JSP
		* PHP
	* INI Configuration File
	* [Inno Setup](tools/lang/InnoSetup.iss), up to Inno Setup 6.2.
	* [Boost Jam](tools/lang/Jamfile.jam), up to Jam 4.4.
	* [Java](tools/lang/Java.java), up to Java 22. [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#java)
		* [BeanShell](https://github.com/beanshell/beanshell)
	* [JavaScript](tools/lang/JavaScript.js), up to ECMAScript 2024 with [JSX](https://facebook.github.io/jsx/) and [Web APIs](https://developer.mozilla.org/en-US/docs/Web/API).
	* [JSON](https://www.json.org) and [JSON5](https://json5.org/) Document
	* [Julia Script](tools/lang/Julia.jl), up to Julia 1.9. [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#julia)
	* [Kotlin](tools/lang/Kotlin.kt), up to Kotlin 2.0.
	* LaTeX
	* Lisp Script (Common Lisp, Clojure, Scheme, etc.)
	* [LLVM IR](tools/lang/LLVM.ll), up to LLVM 16.
	* [Lua Script](tools/lang/Lua.lua), up to Lua 5.4.
	* Makefile, [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#makefile)
		* nmake
		* gmake
		* Borland bmake
		* qmake
		* ninja
	* Markdown, up to [CommonMark](https://spec.commonmark.org/) 0.31, with following extensions:
		* [GitHub Flavored Markdown](https://github.github.com/gfm/)
		* [GitLab Flavored Markdown](https://docs.gitlab.com/ee/user/markdown.html)
		* [Pandoc’s Markdown](https://pandoc.org/MANUAL.html#pandocs-markdown)
	* [Mathematica](tools/lang/Mathematica.nb) and [WolframScript](https://www.wolfram.com/wolframscript/)
	* MATLAB, [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#matlab-octave)
		* Octave
		* Scilab
		* Gnuplot
	* [Nim Script](tools/lang/Nim.nim), up to Nim 2.0.
	* [Nullsoft Scriptable Install System](tools/lang/NSIS.nsi), up to NSIS 3.09.
	* [OCaml](tools/lang/OCaml.ml), up to OCaml 5.0.
	* Pascal, Delphi
	* [Perl](tools/lang/Perl.pl), up to Perl 5.36. [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#perl)
	* [PowerShell](tools/lang/PowerShell.ps1), up to PowerShell 7.2.
	* [Python](tools/lang/Python.py), up to Python 3.13. [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#python)
	* [PHP Script](tools/lang/PHP.php), up to PHP 8.3. [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#php)
	* Windows Rescouce Script
	* [R](tools/lang/R.r), up to R 4.2.
	* [REBOL](tools/lang/Rebol.r) 3 and [Red](tools/lang/Red.red)
	* [Ruby](tools/lang/Ruby.rb), up to Ruby 3.3. [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#ruby)
	* [Rust](tools/lang/Rust.rs), up to Rust 1.38. [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#rust)
	* [SAS](tools/lang/SAS.sas), up to SAS Viya 2024.06.
	* [Scala](tools/lang/Scala.scala), up to Scala 3.2.
	* Shell Script
		* [Bash](tools/lang/Bash.sh) likes (sh, bash, dash, ash, zsh, ksh), up to Bash 5.2. [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#shell-script)
		* [C Shell](tools/lang/CShell.csh) (csh, tcsh), up to tcsh 6.24.
		* [M4 Macro](https://www.gnu.org/software/m4/) and [Autoconf](https://www.gnu.org/software/autoconf/)
	* SQL Query
		* [SQL Standard](tools/lang/SQL.sql), up to SQL:2016.
		* [MySQL](tools/lang/MySQL.sql), up to MySQL 8.0, MariaDB 10.9.
		* [SQLite3](tools/lang/SQLite3.sql), up to 3.44.
		* [PostgreSQL](tools/lang/PostgreSQL.sql), PostgreSQL 14.
		* [Transact-SQL](tools/lang/Transact-SQL.sql), SQL Server 2019.
		* [Oracle](tools/lang/Oracle.sql), Oracle 20, incomplete.
	* [Swift](tools/lang/Swift.swift), up to Swift 5.10.
	* TCL, [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#tcl)
	* [Texinfo](tools/lang/Texinfo.texi), up to Texinfo 7.1.
	* [TOML File](https://toml.io/)
	* [TypeScript](tools/lang/TypeScript.ts), up to TypeScript 5.4.
	* Visual Basic, VB6, VB.NET
	* VBScript
	* [VHDL](tools/lang/VHDL.vhdl), up to VHDL 2019.
	* Verilog HDL, up to [Verilog](tools/lang/Verilog.v) 2005 and [SystemVerilog](tools/lang/SystemVerilog.sv) 2017.
	* [Vim Script](tools/lang/Vim.vim)
	* [WebAssembly](https://github.com/WebAssembly/wabt/blob/main/src/lexer-keywords.txt), up to wabt 1.0.
	* [WinHex Script](tools/lang/WinHex.whs) and template, up to WinHex 20.9.
	* XML Document, [Screenshots](https://github.com/zufuliu/notepad4/wiki/Screenshots#xml)
		* XML Schema
		* XML Stylesheet
		* SGML and DTD
		* Apple Property List
	* [YAML Document](https://yaml.org/)
	* [Zig](tools/lang/Zig.zig), up to Zig 0.13.
	* Many Others
* Toggle fold for specific level, shortcuts: <kbd>Alt</kbd>+<kbd>Number</kbd>
* Toggle fold for current block without scrolling back to the block start line, shortcut: <kbd>Alt</kbd>+<kbd>C</kbd>, or context menu "Toggle Fold"
* Improved support for Chinese, Japanese and Korean: inline mode IME, word wrapping, word selection, case conversions, etc.
* Insert new GUID and current timestamps
* Number conversion between decimal, hexadecimal, octal and binary
* Display caret location and length of selected text in count of characters
* Improved regex support with [Boost regex library](https://www.boost.org/doc/libs/release/libs/regex/doc/html/boost_regex/syntax/perl_syntax.html)
* Improved support for NFO ANSI art
* Support various [Windows system integration](https://github.com/zufuliu/notepad4/wiki/System-Integration)
* Support high DPI display with HD version
* Support rendering using Direct2D DirectWrite, and switch between GDI and Direct2D
* Support font ligature with font like [FiraCode](https://github.com/tonsky/FiraCode) in Direct2D mode
* Support color font (Windows 8.1 and above) in Direct2D mode, useful to display color [Emoji](https://unicode.org/emoji/charts/full-emoji-list.html) 😊 and others
* Experimental support for RTL layout (GDI) and bidirectional (Direct2D), useful for Arabic and Hebrew
* Support using fractional font size
* Support full screen mode
* Support compress & pretty for CSS, JavaScript, JSON and JSON5
* Support copy as RTF
* Support text transliteration
* Support drag and drop file from Visual Studio, Android Studio, IntelliJ IDEA, etc.
* Other various major or minor changes and tweaks

## Supported Operating Systems:
* Windows XP, Vista, 7, 8, 8.1, 10 and 11, both 32-bit and 64-bit
* Windows Server 2003, 2008, 2012, 2016, 2019 and 2022, both 32-bit and 64-bit
* Windows 10 on ARM64, Windows 8/8.1 on ARM32 (Windows RT)
* Unix-like Operating Systems (Linux, macOS, BSD, etc.) using [Wine](https://www.winehq.org/)
* [ReactOS](https://www.reactos.org/)

## Contributors:
* [XhmikosR](https://xhmikosr.github.io/notepad2-mod/)
* [Kai Liu](https://code.kliu.org/misc/notepad2/)
* [RL Vision](https://www.rlvision.com/notepad2/about.asp)
* Aleksandar Lekov
* Bruno Barbieri
* Homam
* Everyone else on [GitHub](https://github.com/zufuliu/notepad4/graphs/contributors) and on Notepad2-mod's [Contributor List](https://github.com/XhmikosR/notepad2-mod/graphs/contributors)

## More information:
* [Florian's Notepad2 website](https://www.flos-freeware.ch/notepad2.html)
* [XhmikosR's Notepad2-mod website](https://xhmikosr.github.io/notepad2-mod/)
* [Scintilla's website](https://www.scintilla.org/)
* [Build Notepad4 and matepath](https://github.com/zufuliu/notepad4/wiki/Build-Notepad4)
* [Localization](https://github.com/zufuliu/notepad4/wiki/Localization)
	* [Deutsch](https://github.com/zufuliu/notepad4/tree/main/locale/de) by [@xelsios](https://github.com/xelsios)
	* [French](https://github.com/zufuliu/notepad4/tree/main/locale/fr) by [@VoilierBleu](https://github.com/VoilierBleu)
	* [Italiano](https://github.com/zufuliu/notepad4/tree/main/locale/it) by [@Matteo-Nigro](https://github.com/Matteo-Nigro)
	* [Русский](https://github.com/zufuliu/notepad4/tree/main/locale/ru) by [@tretdm](https://github.com/tretdm)
	* [日本語](https://github.com/zufuliu/notepad4/tree/main/locale/ja) by [@maboroshin](https://github.com/maboroshin)
	* [中文 (繁體)](https://github.com/zufuliu/notepad4/tree/main/locale/zh-Hant)
	* [中文 (简体)](https://github.com/zufuliu/notepad4/tree/main/locale/zh-Hans)
	* [한국어](https://github.com/zufuliu/notepad4/tree/main/locale/ko) by [@VenusGirl](https://github.com/VenusGirl)
	* [한국어](https://teus.me/Notepad4) by [@bluenlive](https://github.com/bluenlive)
* [Notepad4 Wiki](https://github.com/zufuliu/notepad4/wiki)
