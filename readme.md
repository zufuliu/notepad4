# Notepad2 and metapath

> *Notepad2* is a modified version (fork) of [Notepad2-mod](https://xhmikosr.github.io/notepad2-mod/). *metapath* is a bug-fixed version of original [metapath](http://www.flos-freeware.ch/archive.html).

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](../master/License.txt)

## Binaries
Latest release: [![Latest release](https://img.shields.io/github/release/zufuliu/notepad2.svg)](https://github.com/zufuliu/notepad2/releases)

Latest development build [![Build status](https://ci.appveyor.com/api/projects/status/github/zufuliu/notepad2?branch=master&svg=true)](https://ci.appveyor.com/project/zufuliu/notepad2) is available on [AppVeyor]( https://ci.appveyor.com/project/zufuliu/notepad2) (Find artifacts in Release configuration for each compiler and platform).

Or build from source, see the [Wiki](https://github.com/zufuliu/notepad2/wiki/Build-Notepad2).

## [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots)
![v4.19.06](https://raw.githubusercontent.com/wiki/zufuliu/notepad2/screenshots/v4.19.06.png)

## Changes compared to the official [Notepad2](http://www.flos-freeware.ch/notepad2.html) or [Notepad2-mod](https://xhmikosr.github.io/notepad2-mod/):

* Code folding
* Support for bookmarks
* Option to mark all occurrences of a word or selection, and show matched count
* [Enhanced auto-completion for words and functions](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2)
* [Context-based auto-completion for some words](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#context-based-auto-completion)
* [Enhanced auto indentation](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#auto-indent-text)
* [Auto completion for braces, brackets and quotes](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#auto-insert-text)
* Syntax highlighting and code folding for (see built-in [file extension list](../master/doc/FileExt.txt)):
	* Plain Text
	* ActionScript
	* Assembler, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#assembler)
		* MASM
		* GNU Assembler
		* NASM
	* Asymptote Vector Graphics Language
	* AutoIt Script
	* AWK
		* GNU AWK
	* Windows Batch Script, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#batch)
	* .NET IL
	* CMake, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#cmake)
	* C/C++, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#cc)
		* Objective-C/C++, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#objective-cc)
	* C#, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#c)
		* C# Doc
		* Vala
	* CSS Style Sheet
		* Sassy CSS
		* Less CSS
		* HSS
	* Configuration / Properties File
		* Apache Configuration File
	* D
	* Diff/Patch File
	* Doxygen
	* Fortran
		* GNU Fortran
	* F#
	* Go
	* Gradle, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#gradle)
	* GraphViz Dot, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#graphviz)
	* Groovy
	* Haxe
	* [HTML](../master/tools/lang/html.html), up to 5.2. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#html)
		* ASP
		* ASP.NET
		* JSP
		* PHP
	* INI Configuration File
	* Inno Setup
	* Boost Jam
	* Java, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#java)
		* JavaDoc
	* [JavaScript](../master/tools/lang/JavaScript.js), up to ECMAScript 2018, 9ed.
	* JSON and JSON5 Document
	* [Julia Script](https://julialang.org/), up to 1.0. [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#julia)
	* LaTeX
	* Lisp Script
		* Common Lisp
	* LLVM IR
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
	* Pascal
		* Delphi
	* Perl, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#perl)
	* PowerShell
	* Python, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#python)
		* IronPython
		* Cobra
		* Jython
		* Boo
	* PHP Script, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#php)
		* PHPDoc
	* Windows Rescouce Script
	* Ruby, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#ruby)
	* Scala Script
	* Shell Script
		* Bash likes (sh, bash, dash, ash, zsh, ksh), [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#shell-script)
		* C Shell (csh, tcsh)
		* M4 Macro, limited support
	* Android Dalvik Smali, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#android-smali)
		* javap
		* Jasmin
	* SQL Query
		* [MySQL](../master/tools/lang/MySQL.sql), up to MySQL 8.0, MariaDB 10.3.
		* [SQLite3](../master/tools/lang/SQLite3.sql)
	* TCL, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#tcl)
	* Texinfo
	* Visual Basic
		* VB6
		* VB.NET
	* VBScript
	* VHDL
	* Verilog HDL
	* Vim
	* XML Document, [Screenshots](https://github.com/zufuliu/notepad2/wiki/Screenshots#xml)
		* XML Schema
		* XML Stylesheet
		* XML DTD SGML
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
	* ANSI Art
	* Many Others
* API list for:
	* C Standard Library, up to C11.
	* C++ STL
	* MFC Classes
	* Cocoa Classes
	* AutoIt Script
	* CMake
	* C# (class list only)
	* Java (class list only)
	* [JavaScript](../master/tools/lang/JavaScript.js), up to ECMAScript 2018, 9ed.
	* Inno Setup
	* [Julia Script](https://julialang.org/)
	* Lua Script
	* Python Script
	* PHP Script
	* SQL Query
		* [MySQL](../master/tools/lang/MySQL.sql), up to MySQL 8.0, MariaDB 10.3.
		* [SQLite3](../master/tools/lang/SQLite3.sql)
* Syntax highlighting for function and type for C/C++ family languages
* Syntax highlighting for annotation for Java like JVM based languages
* Syntax highlighting for attribute for C# like languages
* Syntax highlighting for decorator for Python like languages
* Folding for plain text
* Toggle fold for specific level, shortcuts: <kbd>Alt</kbd>+<kbd>Number</kbd>
* Toggle fold for current block without scrolling back to the block start line, shortcut: <kbd>Alt</kbd>+<kbd>C</kbd>, or context menu "Toggle Fold"
* Insert new GUID and current timestamps
* Number conversion between decimal, hexadecimal, octal and binary
* Display caret location and length of selected text in count of characters
* Improved support for NFO ANSI art
* Support replacing Windows Notepad using a clean, unintuitive registry-based method
* Support [adding Notepad2 to Windows explorer context menu](https://github.com/zufuliu/notepad2/wiki/Open-with-Notepad2)
* Per-Monitor DPI-Aware, improved high DPI display
* Support rendering using Direct2D DirectWrite, and switch between GDI and Direct2D
* Support font ligature with font like [FiraCode](https://github.com/tonsky/FiraCode) in Direct2D mode
* Support color font (Windows 8.1 and above) in Direct2D mode, useful to display color [Emoji](http://unicode.org/emoji/charts/full-emoji-list.html) 😊 and others
* Experimental support for RTL layout (GDI) and bidirectional (Direct2D), useful for Arabic and Hebrew
* Support using fractional font size
* Support Inline IME
* Support full screen mode
* Extra support for drag and drop file from Visual Studio, Android Studio, IntelliJ IDEA, etc.
* Cut, copy and paste binary content.
* Other various major or minor changes and tweaks

## Supported Operating Systems:
* Windows XP, Vista, 7, 8, 8.1 and 10, both 32-bit and 64-bit
* Windows Server 2003, 2008, 2012 and 2016, both 32-bit and 64-bit
* Windows 10 on ARM64
* Unix-like Operating Systems (Linux, macOS, BSD, etc.) using [Wine](https://www.winehq.org/)
* [ReactOS](https://www.reactos.org/)

## Notes:
* The default *Win32* build require [SSE2](https://en.wikipedia.org/wiki/SSE2) (Pentium 4), which can be turned off if you [build Notepad2 and metapath](https://github.com/zufuliu/notepad2/wiki/Build-Notepad2) yourself.
* If you find any bugs or have any suggestions feel free to **write proposals/issues** or **provide patches/pull requests**.

## Contributors:
* [XhmikosR](https://xhmikosr.github.io/notepad2-mod/)
* [Kai Liu](http://code.kliu.org/misc/notepad2/)
* [RL Vision](http://www.rlvision.com/notepad2/about.asp)
* Aleksandar Lekov
* Bruno Barbieri
* Homam
* Everyone else on [GitHub](https://github.com/zufuliu/notepad2/graphs/contributors) and on Notepad2-mod's [Contributor List](https://github.com/XhmikosR/notepad2-mod/graphs/contributors)

## More information:
* [Official Notepad2 website](http://www.flos-freeware.ch/notepad2.html)
* [Official Notepad2-mod website](https://xhmikosr.github.io/notepad2-mod/)
* [Scintilla's website](https://www.scintilla.org/)
* [Build Notepad2 and metapath](https://github.com/zufuliu/notepad2/wiki/Build-Notepad2)
* [Notepad2 Wiki](https://github.com/zufuliu/notepad2/wiki)

### Other projects based on original Notepad2 or Notepad2-mod:
* [Notepad3](https://github.com/rizonesoft/Notepad3)
* [Notepad2e](https://github.com/ProgerXP/Notepad2e)
