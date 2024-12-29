from KeywordCore import *

lexerList = [
	('NP2LEX_TEXTFILE', 'stlDefault.cpp', 'LexNull.cxx', '', (0, 'Text'), None),
	('NP2LEX_2NDTEXTFILE', 'stlDefault.cpp', 'LexNull.cxx', '', (0, '2nd Text'), None),
	('NP2LEX_ANSI', 'stlDefault.cpp', 'LexNull.cxx', '', (0, 'ANSI'), None),

	('NP2LEX_ABAQUS', 'stlABAQUS.cpp', 'LexAPDL.cxx', 'ABAQUS.inp', 0, parse_apdl_api_file),
	('NP2LEX_ACTIONSCRIPT', 'stlActionScript.cpp', 'LexJavaScript.cxx', 'ActionScript.as', 1, parse_actionscript_api_file),
	('NP2LEX_APDL', 'stlAPDL.cpp', 'LexAPDL.cxx', 'APDL.cdb', 0, parse_apdl_api_file),
	('NP2LEX_ASM', 'stlAsm.cpp', 'LexAsm.cxx', '', 0, None),
	('NP2LEX_ASYMPTOTE', 'stlAsymptote.cpp', 'LexAsymptote.cxx', 'Asymptote.asy', 1, parse_asymptote_api_file),
	('NP2LEX_AUTOHOTKEY', 'stlAutoHotkey.cpp', 'LexAutoHotkey.cxx', ['AutoHotkey_L.ahk', 'AutoHotkey_H.ahk'], 0, parse_autohotkey_api_file),
	('NP2LEX_AUTOIT3', 'stlAutoIt3.cpp', 'LexAutoIt3.cxx', 'AutoIt3.au3', 0, parse_autoit3_api_file),
	('NP2LEX_AVISYNTH', 'stlAviSynth.cpp', 'LexAviSynth.cxx', 'AviSynth.avs', 0, parse_avisynth_api_file),
	('NP2LEX_AWK', 'stlAwk.cpp', 'LexAwk.cxx', 'Awk.awk', 1, parse_awk_api_file),

	('NP2LEX_BASH', 'stlBash.cpp', 'LexBash.cxx', ['Bash.sh', 'CShell.csh'], 0, parse_bash_api_file),
	('NP2LEX_BATCH', 'stlBatch.cpp', 'LexBatch.cxx', 'Batch.bat', 0, parse_batch_api_file),

	('NP2LEX_CANGJIE', 'stlCangjie.cpp', 'LexCangjie.cxx', 'Cangjie.cj', 0, parse_cangjie_api_file),
	('NP2LEX_CIL', 'stlCIL.cpp', 'LexCIL.cxx', '', 0, None),
	('NP2LEX_CMAKE', 'stlCMake.cpp', 'LexCMake.cxx', 'CMake.cmake', 0, parse_cmake_api_file),
	('NP2LEX_CONFIG', 'stlDefault.cpp', 'LexConfig.cxx', '', (0, 'Config'), None),
	('NP2LEX_CPP', 'stlCPP.cpp', 'LexCPP.cxx', ['CPP.cpp', 'C.c', 'POSIX.c'], 1, parse_cpp_api_file),
	('NP2LEX_CSHARP', 'stlCSharp.cpp', 'LexCSharp.cxx', 'CSharp.cs', 1, parse_csharp_api_file),
		('NP2LEX_FSHARP', 'stlFSharp.cpp', 'LexFSharp.cxx', 'FSharp.fs', 0, parse_fsharp_api_file),
	('NP2LEX_CSV', 'stlDefault.cpp', 'LexCSV.cxx', '', (0, 'CSV'), None),

	('NP2LEX_DLANG', 'stlD.cpp', 'LexD.cxx', 'D.d', 1, parse_dlang_api_file),
	('NP2LEX_DART', 'stlDart.cpp', 'LexDart.cxx', 'Dart.dart', 0, parse_dart_api_file),
	('NP2LEX_DIFF', 'stlDefault.cpp', 'LexDiff.cxx', '', (0, 'Diff'), None),

	('NP2LEX_FORTRAN', 'stlFortran.cpp', 'LexFortran.cxx', 'Fortran.f', 0, parse_fortran_api_file),

	('NP2LEX_GN', 'stlGN.cpp', 'LexGN.cxx', 'GN.gn', 0, parse_gn_api_file),
	('NP2LEX_GRAPHVIZ', 'stlGraphViz.cpp', 'LexGraphViz.cxx', 'GraphViz.dot', 0, parse_graphviz_api_file),
		('NP2LEX_BLOCKDIAG', 'stlBlockdiag.cpp', 'LexGraphViz.cxx', 'blockdiag.diag', 0, parse_graphviz_api_file),
		('NP2LEX_CSS', 'stlCSS.cpp', 'LexCSS.cxx', ['CSS.css', 'SCSS.scss', 'Less.less', 'HSS.hss'], 0, parse_css_api_file),
	('NP2LEX_GO', 'stlGo.cpp', 'LexGo.cxx', 'Go.go', 0, parse_go_api_file),

	('NP2LEX_HASKELL', 'stlHaskell.cpp', 'LexHaskell.cxx', 'Haskell.hs', 0, parse_haskell_api_file),
	('NP2LEX_HAXE', 'stlHaxe.cpp', 'LexHaxe.cxx', 'Haxe.hx', 1, parse_haxe_api_file),

	('NP2LEX_INI', 'stlDefault.cpp', 'LexProps.cxx', '', (0, 'Ini'), None),
	('NP2LEX_INNOSETUP', 'stlInno.cpp', 'LexInno.cxx', 'InnoSetup.iss', 0, parse_inno_setup_api_file),

	('NP2LEX_JAMFILE', 'stlJamfile.cpp', 'LexJam.cxx', 'Jamfile.jam', 0, parse_jam_api_file),
	('NP2LEX_JAVA', 'stlJava.cpp', 'LexJava.cxx', 'Java.java', 1, parse_java_api_file),
		('NP2LEX_GROOVY', 'stlGroovy.cpp', 'LexGroovy.cxx', 'Groovy.groovy', 1, parse_groovy_api_file),
			('NP2LEX_GRADLE', 'stlGradle.cpp', 'LexGroovy.cxx', 'Gradle.gradle', 1, parse_gradle_api_file),
		('NP2LEX_KOTLIN', 'stlKotlin.cpp', 'LexKotlin.cxx', 'Kotlin.kt', 0, parse_kotlin_api_file),
	('NP2LEX_JAVASCRIPT', 'stlJavaScript.cpp', 'LexJavaScript.cxx', 'JavaScript.js', 1, parse_javascript_api_file),
		('NP2LEX_COFFEESCRIPT', 'stlCoffeeScript.cpp', 'LexCoffeeScript.cxx', 'CoffeeScript.coffee', 0, parse_coffeescript_api_file),
		('NP2LEX_TYPESCRIPT', 'stlTypeScript.cpp', 'LexJavaScript.cxx', 'TypeScript.ts', 1, parse_typescript_api_file),
	('NP2LEX_JSON', 'stlJSON.cpp', 'LexJSON.cxx', '', 0, parse_json_api_file),
	('NP2LEX_JULIA', 'stlJulia.cpp', 'LexJulia.cxx', 'Julia.jl', 0, parse_julia_api_file),

	('NP2LEX_LATEX', 'stlLaTeX.cpp', 'LexLaTeX.cxx', '', 0, None),
	('NP2LEX_LISP', 'stlLisp.cpp', 'LexLisp.cxx', '', 0, None),
	('NP2LEX_LLVM', 'stlLLVM.cpp', 'LexLLVM.cxx', 'LLVM.ll', 0, parse_llvm_api_file),
	('NP2LEX_LUA', 'stlLua.cpp', 'LexLua.cxx', 'Lua.lua', 0, parse_lua_api_file),

	('NP2LEX_MAKEFILE', 'stlMake.cpp', 'LexMakefile.cxx', '', 0, None),
	('NP2LEX_MARKDOWN', 'stlMarkdown.cpp', 'LexMarkdown.cxx', '', 0, parse_markdown_api_file),
	('NP2LEX_MATHEMATICA', 'stlMathematica.cpp', 'LexMathematica.cxx', 'Mathematica.nb', 0, parse_mathematica_api_file),
	('NP2LEX_MATLAB', 'stlMatlab.cpp', 'LexMatlab.cxx', '', 0, None),

	('NP2LEX_NIM', 'stlNim.cpp', 'LexNim.cxx', 'Nim.nim', 0, parse_nim_api_file),
	('NP2LEX_NSIS', 'stlNsis.cpp', 'LexNSIS.cxx', 'NSIS.nsi', 0, parse_nsis_api_file),

	('NP2LEX_OCAML', 'stlOCaml.cpp', 'LexOCaml.cxx', 'OCaml.ml', 0, parse_ocaml_api_file),

	('NP2LEX_PASCAL', 'stlPascal.cpp', 'LexPascal.cxx', '', 0, None),
	('NP2LEX_PERL', 'stlPerl.cpp', 'LexPerl.cxx', 'Perl.pl', 0, parse_perl_api_file),
	('NP2LEX_PHP', 'stlPHP.cpp', 'LexPHP.cxx', 'PHP.php', 1, parse_php_api_file),
	('NP2LEX_POWERSHELL', 'stlPowerShell.cpp', 'LexPowerShell.cxx', 'PowerShell.ps1', 0, parse_powershell_api_file),
	('NP2LEX_PYTHON', 'stlPython.cpp', 'LexPython.cxx', 'Python.py', 0, parse_python_api_file),

	('NP2LEX_RLANG', 'stlR.cpp', 'LexR.cxx', 'R.r', 0, parse_rlang_api_file),
	('NP2LEX_REBOL', 'stlRebol.cpp', 'LexRebol.cxx', ['Rebol.r', 'Red.red'], 1, parse_rebol_api_file),
	('NP2LEX_RESOURCESCRIPT', 'stlResource.cpp', 'LexCPP.cxx', '', 0, parse_resource_script_api_file),
	('NP2LEX_RUBY', 'stlRuby.cpp', 'LexRuby.cxx', 'Ruby.rb', 0, parse_ruby_api_file),
	('NP2LEX_RUST', 'stlRust.cpp', 'LexRust.cxx', 'Rust.rs', 0, parse_rust_api_file),

	('NP2LEX_SAS', 'stlSAS.cpp', 'LexSAS.cxx', 'SAS.sas', 0, parse_sas_api_file),
	('NP2LEX_SCALA', 'stlScala.cpp', 'LexScala.cxx', 'Scala.scala', 1, parse_scala_api_file),
	('NP2LEX_SMALI', 'stlSmali.cpp', 'LexSmali.cxx', '', 0, parse_smali_api_file),
	# TODO: SQL Dialect, https://github.com/zufuliu/notepad4/issues/31
	('NP2LEX_SQL', 'stlSQL.cpp', 'LexSQL.cxx', [
							'MySQL.sql',
							'Oracle.sql',
							'PostgreSQL.sql',
							'SQL.sql',
							'SQLite3.sql',
							'Transact-SQL.sql',
							], 0, parse_sql_api_file),
	('NP2LEX_SWIFT', 'stlSwift.cpp', 'LexSwift.cxx', 'Swift.swift', 0, parse_swift_api_file),

	('NP2LEX_TCL', 'stlTcl.cpp', 'LexTCL.cxx', '', 0, None),
	('NP2LEX_TEXINFO', 'stlTexinfo.cpp', 'LexTexinfo.cxx', 'Texinfo.texi', 0, parse_texinfo_api_file),
	('NP2LEX_TOML', 'stlTOML.cpp', 'LexTOML.cxx', '', 0, parse_toml_api_file),

	('NP2LEX_VBSCRIPT', 'stlVBS.cpp', 'LexVB.cxx', ['VBScript.vbs'], 0, parse_visual_basic_api_file),
	('NP2LEX_VERILOG', 'stlVerilog.cpp', 'LexVerilog.cxx', ['Verilog.v', 'SystemVerilog.sv'], 0, parse_verilog_api_file),
	('NP2LEX_VHDL', 'stlVHDL.cpp', 'LexVHDL.cxx', 'VHDL.vhdl', 0, parse_vhdl_api_file),
	('NP2LEX_VIM', 'stlVim.cpp', 'LexVim.cxx', 'Vim.vim', 0, parse_vim_api_file),
	('NP2LEX_VISUALBASIC', 'stlVB.cpp', 'LexVB.cxx', ['VB.NET.vb', 'VBA.bas'], 0, parse_visual_basic_api_file),

	# https://github.com/WebAssembly/wabt/blob/main/src/lexer-keywords.txt
	('NP2LEX_WASM', 'stlWASM.cpp', 'LexWASM.cxx', 'wasm-lexer-keywords.txt', 0, parse_wasm_lexer_keywords),
	('NP2LEX_WINHEX', 'stlWinHex.cpp', 'LexWinHex.cxx', 'WinHex.whs', 0, parse_winhex_api_file),
	('NP2LEX_XML', 'stlXML.cpp', 'LexHTML.cxx', 'XML.xml', 0, parse_xml_api_file),
		('NP2LEX_HTML', 'stlHTML.cpp', 'LexHTML.cxx', 'html.html', 0, parse_html_api_file),
	('NP2LEX_YAML', 'stlYAML.cpp', 'LexYAML.cxx', '', 0, parse_yaml_api_file),
	('NP2LEX_ZIG', 'stlZig.cpp', 'LexZig.cxx', 'Zig.zig', 0, parse_zig_api_file),
]

def update_all_keyword():
	numKeyword = 16
	handled = set()
	for rid, output, lexer, path, count, parse in lexerList:
		assert rid.startswith('NP2LEX_'), rid
		assert rid not in handled, rid
		assert output.startswith('stl') and output.endswith('.cpp'), output
		assert lexer.startswith('Lex') and lexer.endswith('.cxx'), lexer
		handled.add(rid)

		if not parse:
			keywordList = []
		elif isinstance(path, str):
			keywordList = parse('lang/' + path)
		else:
			pathList = ['lang/' + name for name in path]
			keywordList = parse(pathList)

		output = '../src/EditLexers/' + output
		if isinstance(count, tuple):
			num, suffix = count
		else:
			num = count
			suffix = ''
		UpdateKeywordFile(rid, output, lexer, keywordList, numKeyword - num, suffix)

	UpdateKeywordFile('', '../src/EditLexers/EditStyleX.h', '', [], numKeyword, 'Scheme Default')
	config = set(LexerConfigMap.keys())
	if missing := config - handled:
		print('missing lexer config:', ', '.join(sorted(missing)))
	if missing := handled - config:
		print('default lexer config:', ', '.join(sorted(missing)))
	UpdateLexerKeywordAttr('../src/EditAutoC.cpp', '../scintilla/lexers/')

update_all_keyword()
UpdateLexerEnumFile('../src/EditLexer.h', '../scintilla/lexlib/WordList.h')
UpdateAutoCompletionCache('../src/EditAutoC.cpp')
