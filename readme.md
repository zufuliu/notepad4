# Notepad2-zufuliu and metapath-zufuliu

Notepad2-zufuliu is a light-weight Scintilla based text editor for Windows with syntax highlighting, code folding, auto-completion and API list for many programming languages and documents, it's based on official [Notepad2](https://www.flos-freeware.ch/notepad2.html) and [Notepad2-mod](https://xhmikosr.github.io/notepad2-mod/). metapath-zufuliu is a file browser plugin and based on official [metapath](https://www.flos-freeware.ch/archive.html).

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](License.txt) [![Latest release](https://img.shields.io/github/release/zufuliu/notepad2.svg)](https://github.com/zufuliu/notepad2/releases) [![Downloads](https://img.shields.io/github/downloads/zufuliu/notepad2/total.svg)](https://github.com/zufuliu/notepad2/releases)

## Binaries
Latest development builds (artifacts in Release configuration for each compiler and platform) are available on [GitHub Actions](https://github.com/zufuliu/notepad2/actions) and [AppVeyor](https://ci.appveyor.com/project/zufuliu/notepad2). Or you can build from source, see [Wiki](https://github.com/zufuliu/notepad2/wiki/Build-Notepad2).

| GitHub Actions (MSVC, Clang, GCC) | AppVeyor (MSVC, Clang) |
| --------------- | -------- |
| [![Build status](https://github.com/zufuliu/notepad2/workflows/CI/badge.svg)](https://github.com/zufuliu/notepad2/actions) | [![Build status](https://ci.appveyor.com/api/projects/status/github/zufuliu/notepad2?branch=main&svg=true)](https://ci.appveyor.com/project/zufuliu/notepad2) |

## [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots)
![v4.23.01 HD](https://raw.githubusercontent.com/wiki/zufuliu/notepad2/screenshots/v4.23.01HD.png)

## Changes compared to the official [Notepad2](https://www.flos-freeware.ch/notepad2.html) or [Notepad2-mod](https://xhmikosr.github.io/notepad2-mod/):

* Support for bookmarks
* Option to mark all occurrences of a word or selection, and show matched count
* [Enhanced auto-completion for words and functions](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2)
* [Context-based auto-completion for some words](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#context-based-auto-completion)
* [Enhanced auto indentation](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#auto-indent-text)
* [Auto completion for braces, brackets and quotes](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#auto-insert-text)
* Syntax highlighting, code folding and API list for (see built-in [file extension list](doc/FileExt.txt)):
	* Plain Text
	* [ABAQUS](tools/lang/ABAQUS.inp)
	* [ActionScript](tools/lang/ActionScript.as) for ActionScript 3 and 4.
	* Android Smali, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#android-smali)
		* [javap](https://docs.oracle.com/en/java/javase/17/docs/specs/man/javap.html)
		* [Jasmin](https://jasmin.sourceforge.net/)
	* ANSI Art
	* [ANSYS](tools/lang/APDL.cdb) Parametric Design Language
	* Assembler, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#assembler)
		* MASM
		* GNU Assembler
		* [NASM](https://nasm.us/)
	* [Asymptote Vector Graphics Language](tools/lang/Asymptote.asy), up to Asymptote 2.70.
	* [AutoHotkey_L](tools/lang/AutoHotkey_L.ahk) and [AutoHotkey_H](tools/lang/AutoHotkey_H.ahk) Script, up to v2.
	* [AutoIt3](tools/lang/AutoIt3.au3) Script, up to 3.3.16.
	* [AviSynth Script](tools/lang/AviSynth.avs), up to AviSynth 2.6, AviSynth+ 3.6, GScript 1.1.
	* [AWK](tools/lang/Awk.awk), up to GNU AWK 5.2.
	* [Batch Script](tools/lang/Batch.bat), [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#batch)
	* [blockdiag](tools/lang/blockdiag.diag), seqdiag, actdiag and nwdiag
	* [C#](tools/lang/CSharp.cs), up to C# 11.0. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#c)
		* [Vala](https://wiki.gnome.org/Projects/Vala) 0.54
	* C/C++, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#cc)
		* Objective-C/C++, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#objective-cc)
		* [C Standard Library](tools/lang/C.c), up to C2x.
		* [C++ STL](tools/lang/CPP.cpp), up to C++20.
	* [CSS Style Sheet](tools/lang/CSS.css), up to April 2023 snapshot.
		* [SCSS Style Sheet](tools/lang/SCSS.scss), up to Dart Sass 1.50.
		* [Less Style Sheet](tools/lang/Less.less), up to Less 4.1.
		* [HSS Style Sheet](tools/lang/HSS.hss)
	* [CMake](tools/lang/CMake.cmake), up to CMake 3.25. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#cmake)
	* [CoffeeScript](tools/lang/CoffeeScript.coffee), CoffeeScript 2.7.
	* Common Intermediate Language
	* Configuration / Properties File
		* Apache Configuration File
	* [CSV File](https://www.rfc-editor.org/rfc/rfc4180)
	* [D](tools/lang/D.d), up to D language 2.0.
	* [Dart](tools/lang/Dart.dart), up to Dart 2.12.
	* Diff/Patch File
	* F#
	* [Fortran](tools/lang/Fortran.f), up to Fortran 2018.
	* [GN Build](tools/lang/GN.gn), up to GN 2019-03-13.
	* [Go](tools/lang/Go.go), up to Go 1.14.
		* [Go+ (GoPlus)](https://goplus.org/) and the Q language
	* [Gradle](tools/lang/Gradle.gradle), up to Gradle 6.8, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#gradle)
	* [GraphViz Dot](tools/lang/GraphViz.dot), up to GraphViz 2.49, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#graphviz)
	* [Groovy](tools/lang/Groovy.groovy), up to Groovy 4.0.
	* [Haxe](tools/lang/Haxe.hx), up to Haxe 4.2
	* [HTML](tools/lang/html.html), up to [WHATWG](https://html.spec.whatwg.org/multipage/) April 2023. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#html)
		* ASP
		* ASP.NET
		* JSP
		* PHP
	* INI Configuration File
	* [Inno Setup](tools/lang/InnoSetup.iss), up to Inno Setup 6.2.
	* [Boost Jam](tools/lang/Jamfile.jam), up to Jam 4.4.
	* [Java](tools/lang/Java.java), up to Java 20. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#java)
		* [BeanShell](https://github.com/beanshell/beanshell)
	* [JavaScript](tools/lang/JavaScript.js), up to ECMAScript 2023 with [JSX](https://facebook.github.io/jsx/) and [Web APIs](https://developer.mozilla.org/en-US/docs/Web/API).
	* [JSON](https://www.json.org) and [JSON5](https://json5.org/) Document
	* [Julia Script](tools/lang/Julia.jl), up to Julia 1.2. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#julia)
	* [Kotlin](tools/lang/Kotlin.kt), up to Kotlin 1.7.
	* LaTeX
	* Lisp Script (Common Lisp, Clojure, Scheme, etc.)
	* [LLVM IR](tools/lang/LLVM.ll), up to LLVM 16.
	* [Lua Script](tools/lang/Lua.lua), up to Lua 5.4.
	* Makefile, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#makefile)
		* nmake
		* gmake
		* Borland bmake
		* qmake
		* ninja
	* Markdown, up to [CommonMark](https://spec.commonmark.org/) 0.30, with following extensions:
		* [GitHub Flavored Markdown](https://github.github.com/gfm/)
		* [GitLab Flavored Markdown](https://docs.gitlab.com/ee/user/markdown.html)
		* [Pandoc’s Markdown](https://pandoc.org/MANUAL.html#pandocs-markdown)
	* MATLAB, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#matlab-octave)
		* Octave
		* Scilab
		* Gnuplot
	* [Nullsoft Scriptable Install System](tools/lang/NSIS.nsi), up to NSIS 3.09.
	* Pascal, Delphi
	* [Perl](tools/lang/Perl.pl), up to Perl 5.36. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#perl)
	* [PowerShell](tools/lang/PowerShell.ps1), up to PowerShell 7.2.
	* [Python](tools/lang/Python.py), up to Python 3.11. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#python)
	* [PHP Script](tools/lang/PHP.php), up to PHP 8.2. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#php)
	* Windows Rescouce Script
	* [R](tools/lang/R.r), up to R 4.2.
	* [REBOL](tools/lang/Rebol.r) 3 and [Red](tools/lang/Red.red)
	* [Ruby](tools/lang/Ruby.rb), up to Ruby 3.2. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#ruby)
	* [Rust](tools/lang/Rust.rs), up to Rust 1.38. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#rust)
	* [Scala](tools/lang/Scala.scala), up to Scala 3.2.
	* Shell Script
		* [Bash](tools/lang/Bash.sh) likes (sh, bash, dash, ash, zsh, ksh), up to Bash 5.2. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#shell-script)
		* [C Shell](tools/lang/CShell.csh) (csh, tcsh), up to tcsh 6.24.
		* M4 Macro, limited support
	* SQL Query
		* [SQL Standard](tools/lang/SQL.sql), up to SQL:2016.
		* [MySQL](tools/lang/MySQL.sql), up to MySQL 8.0, MariaDB 10.9.
		* [SQLite3](tools/lang/SQLite3.sql), up to 3.41.
		* [PostgreSQL](tools/lang/PostgreSQL.sql), PostgreSQL 14.
		* [Transact-SQL](tools/lang/Transact-SQL.sql), SQL Server 2019.
		* [Oracle](tools/lang/Oracle.sql), Oracle 20, incomplete.
	* [Swift](tools/lang/Swift.swift), up to Swift 5.8.
	* TCL, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#tcl)
	* [Texinfo](tools/lang/Texinfo.texi), up to Texinfo 7.0.
	* [TOML File](https://toml.io/)
	* [TypeScript](tools/lang/TypeScript.ts), TypeScript 5.0.
	* Visual Basic, VB6, VB.NET
	* VBScript
	* [VHDL](tools/lang/VHDL.vhdl), up to VHDL 2019.
	* Verilog HDL, up to [Verilog](tools/lang/Verilog.v) 2005 and [SystemVerilog](tools/lang/SystemVerilog.sv) 2017.
	* [Vim Script](tools/lang/Vim.vim)
	* [WebAssembly](https://github.com/WebAssembly/wabt/blob/main/src/lexer-keywords.txt), up to wabt 1.0.
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
	* [YAML Document](https://yaml.org/)
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
* Support color font (Windows 8.1 and above) in Direct2D mode, useful to display color [Emoji](https://unicode.org/emoji/charts/full-emoji-list.html) 😊 and others
* Experimental support for RTL layout (GDI) and bidirectional (Direct2D), useful for Arabic and Hebrew
* Support using fractional font size
* Support full screen mode
* Extra support for drag and drop file from Visual Studio, Android Studio, IntelliJ IDEA, etc.
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
* Everyone else on [GitHub](https://github.com/zufuliu/notepad2/graphs/contributors) and on Notepad2-mod's [Contributor List](https://github.com/XhmikosR/notepad2-mod/graphs/contributors)

## More information:
* [Official Notepad2 website](https://www.flos-freeware.ch/notepad2.html)
* [Official Notepad2-mod website](https://xhmikosr.github.io/notepad2-mod/)
* [Scintilla's website](https://www.scintilla.org/)
* [Build Notepad2 and metapath](https://github.com/zufuliu/notepad2/wiki/Build-Notepad2)
* [Localization](https://github.com/zufuliu/notepad2/wiki/Localization)
	* [French (France)](https://github.com/VoilierBleu/notepad2) by [@VoilierBleu](https://github.com/VoilierBleu)
	* [Italiano](https://github.com/Matteo-Nigro/notepad2) by [@Matteo-Nigro](https://github.com/Matteo-Nigro)
	* [日本語](https://github.com/maboroshin/notepad2) by [@maboroshin](https://github.com/maboroshin)
	* [中文 (繁體)](https://github.com/zufuliu/notepad2/tree/main/locale/zh-Hant)
	* [中文 (简体)](https://github.com/zufuliu/notepad2/tree/main/locale/zh-Hans)
	* [한국어](https://github.com/VenusGirl/notepad2) by [@VenusGirl](https://github.com/VenusGirl)
	* [한국어](https://teus.me/category/IT/Notepad2) by [@bluenlive](https://github.com/bluenlive)
* [Notepad2 Wiki](https://github.com/zufuliu/notepad2/wiki)
