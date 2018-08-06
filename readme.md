# Notepad2 and metapath

> *Notepad2 is a modified version (fork) of [Notepad2-mod](https://xhmikosr.github.io/notepad2-mod/)
> *metapath is a bug-fixed version of original [metapath](http://www.flos-freeware.ch/archive.html)

## Changes compared to the official [Notepad2](http://www.flos-freeware.ch/notepad2.html) or [Notepad2-mod](https://xhmikosr.github.io/notepad2-mod/):

* Code folding
* Support for bookmarks
* Option to mark all occurrences of a word or selection, and show matched count
* [Enhanced auto-completion for words and functions](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2)
* [Context-based auto-completion for some words](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#context-based-auto-completion)
* [Enhanced auto indentation](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#auto-indent-text)
* [Auto completion for braces, brackets, quotes](https://github.com/zufuliu/notepad2/wiki/Auto-Completion-in-Notepad2#auto-close-bracesquotes)
* Syntax highlighting and auto-completion support for:
	* Plain Text
	* ActionScript
	* Assembler
		* MASM
		* GNU Assembler
		* NASM
	* Asymptote Vector Graphics Language
	* AutoIt Script
	* AWK
		* GNU AWK
	* Bash Script
	* Windows Batch Script
	* .NET IL
	* CMake
	* C/C++
		* Objective-C/C++
	* C#
		* C# Doc
		* Vala
	* CSS
	* Configuration / Properties File
		* Apache Configuration File
	* D
	* Diff/Patch File
	* Doxygen
	* Fortran
		* GNU Fortran
	* F#
	* Go
	* Gradle
	* GraphViz Dot
	* Groovy
	* Haxe
	* [HTML](../master/tools/lang/html.html), up to 5.2.
		* ASP
		* ASP.NET
		* JSP
		* PHP
	* INI Configuration File
	* Inno Setup
	* Boost Jam
	* Java
		* JavaDoc
	* [JavaScript](../master/tools/lang/JavaScript.js), up to ECMAScript 2018, 9ed.
	* JSON Document
	* [Julia Script](https://julialang.org/)
	* LaTeX
	* Lisp Script
		* Common Lisp
	* LLVM IR
	* Lua Script
	* Makefile
		* nmake
		* gmake
		* Borland bmake
		* qmake (syntax highlighting only)
	* Matlab
		* Octave
		* Scilab
		* Gnuplot
	* NSIS
	* Pascal
		* Delphi
	* Perl
	* PowerShell
	* Python
		* IronPython
		* Cobra
		* Jython
		* Boo
	* PHP Script
		* PHPDoc
	* Windows Rescouce Script
	* Ruby
	* Scala Script
	* Android Dalvik Smali
		* javap
		* Jasmin
	* SQL Query
		* [MySQL](../master/tools/lang/MySQL.sql), up to MySQL 8.0, MariaDB 10.3.
		* [SQLite3](../master/tools/lang/SQLite3.sql)
	* TCL
	* Texinfo
	* Visual Basic
		* VB6
		* VB.NET
	* VBScript
	* VHDL
	* Verilog HDL
	* Vim
	* XML Document
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
	* C Standard Library
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
* Toggle folding for specific level or current block
* Insert new GUID and current timestamps
* Number conversion between decimal, hexadecimal, octal and binary
* Display caret location and length of selected text in count of characters
* Improved support for NFO ANSI art
* Support replacing Windows Notepad using a clean, unintuitive registry-based method
* Support [adding Notepad2 to Windows explorer context menu](https://github.com/zufuliu/notepad2/wiki/Open-with-Notepad2)
* Per-Monitor DPI-Aware, improved high DPI display
* Support rendering using Direct2D DirectWrite, and switch between GDI and Direct2D
* Support bidirectional, useful for Arabic and Hebrew
* Support using fractional font size
* Other various major or minor changes and tweaks

## Supported Operating Systems:
* Windows XP, Vista, 7, 8, 8.1 and 10, both 32-bit and 64-bit
* Windows Server 2003, 2008, 2012 and 2016, both 32-bit and 64-bit
* Unix-like Operating Systems (Linux, macOS, BSD, etc.) using [Wine](https://www.winehq.org/)
* [ReactOS](https://www.reactos.org/)

## [Screenshots]
TBD.

## Download
Prebuild binaries for each release can be downloaded from [release list](https://github.com/zufuliu/notepad2/releases).

## Notes:
* The default *Win32* build require [SSE2](https://en.wikipedia.org/wiki/SSE2)(Pentium 4), which can be turned off if you [build Notepad2 and metapath](https://github.com/zufuliu/notepad2/wiki/Build-Notepad2) yourself.
* If you find any bugs or have any suggestions feel free to **write proposals/issues** and/or **provide patches/pull requests**.

## Contributors:
* [XhmikosR](https://xhmikosr.github.io/notepad2-mod/)
* [Kai Liu](http://code.kliu.org/misc/notepad2/)
* [RL Vision](http://www.rlvision.com/notepad2/about.asp)
* Aleksandar Lekov
* Bruno Barbieri
* Everyone else on [GitHub](https://github.com/zufuliu/notepad2/graphs/contributors)

## More information:
* [Official Notepad2 website](http://www.flos-freeware.ch/notepad2.html)
* [Official Notepad2-mod website](https://xhmikosr.github.io/notepad2-mod/)
* [Scintilla's website](http://www.scintilla.org/)
* [Build Notepad2 and metapath](https://github.com/zufuliu/notepad2/wiki/Build-Notepad2)

## Changed keyboard shortcuts compared to Notepad2 and/or Notepad2-mod:
Almost every shortcut is available in the menu.
