from KeywordCore import *

lexerList = [
	('NP2LEX_TEXTFILE', 'stlDefault.c', 'LexNull.cxx', '', (0, 'Text'), None),
	('NP2LEX_2NDTEXTFILE', 'stlDefault.c', 'LexNull.cxx', '', (0, '2nd Text'), None),
	('NP2LEX_ANSI', 'stlDefault.c', 'LexNull.cxx', '', (0, 'ANSI'), None),

	('NP2LEX_ABAQUS', 'stlABAQUS.c', 'LexAPDL.cxx', 'ABAQUS.inp', 0, parse_apdl_api_file),
	('NP2LEX_ACTIONSCRIPT', 'stlActionScript.c', 'LexJavaScript.cxx', 'ActionScript.as', 1, parse_actionscript_api_file),
	('NP2LEX_APDL', 'stlAPDL.c', 'LexAPDL.cxx', 'APDL.cdb', 0, parse_apdl_api_file),
	('NP2LEX_ASM', 'stlAsm.c', 'LexAsm.cxx', '', 0, None),
	('NP2LEX_ASYMPTOTE', 'stlAsymptote.c', 'LexAsymptote.cxx', 'Asymptote.asy', 1, parse_asymptote_api_file),
	('NP2LEX_AUTOHOTKEY', 'stlAutoHotkey.c', 'LexAutoHotkey.cxx', ['AutoHotkey_L.ahk', 'AutoHotkey_H.ahk'], 0, parse_autohotkey_api_file),
	('NP2LEX_AUTOIT3', 'stlAutoIt3.c', 'LexAutoIt3.cxx', 'AutoIt3.au3', 0, parse_autoit3_api_file),
	('NP2LEX_AVISYNTH', 'stlAviSynth.c', 'LexAviSynth.cxx', 'AviSynth.avs', 0, parse_avisynth_api_file),
	('NP2LEX_AWK', 'stlAwk.c', 'LexAwk.cxx', 'Awk.awk', 1, parse_awk_api_file),

	('NP2LEX_BASH', 'stlBash.c', 'LexBash.cxx', ['Bash.sh', 'CShell.csh'], 0, parse_bash_api_file),
	('NP2LEX_BATCH', 'stlBatch.c', 'LexBatch.cxx', 'Batch.bat', 0, parse_batch_api_file),

	('NP2LEX_CIL', 'stlCIL.c', 'LexCIL.cxx', '', 0, None),
	('NP2LEX_CMAKE', 'stlCMake.c', 'LexCMake.cxx', 'CMake.cmake', 0, parse_cmake_api_file),
	('NP2LEX_CONFIG', 'stlDefault.c', 'LexConfig.cxx', '', (0, 'Config'), None),
	('NP2LEX_CPP', 'stlCPP.c', 'LexCPP.cxx', ['CPP.cpp', 'C.c', 'POSIX.c'], 1, parse_cpp_api_file),
	('NP2LEX_CSHARP', 'stlCSharp.c', 'LexCSharp.cxx', 'CSharp.cs', 1, parse_csharp_api_file),
	('NP2LEX_CSV', 'stlDefault.c', 'LexCSV.cxx', '', (0, 'CSV'), None),

	('NP2LEX_DLANG', 'stlD.c', 'LexD.cxx', 'D.d', 1, parse_dlang_api_file),
	('NP2LEX_DART', 'stlDart.c', 'LexDart.cxx', 'Dart.dart', 0, parse_dart_api_file),
	('NP2LEX_DIFF', 'stlDefault.c', 'LexDiff.cxx', '', (0, 'Diff'), None),

	('NP2LEX_FORTRAN', 'stlFortran.c', 'LexFortran.cxx', 'Fortran.f', 0, parse_fortran_api_file),
	('NP2LEX_FSHARP', 'stlFSharp.c', 'LexFSharp.cxx', '', 0, None),

	('NP2LEX_GN', 'stlGN.c', 'LexGN.cxx', 'GN.gn', 0, parse_gn_api_file),
	('NP2LEX_GRAPHVIZ', 'stlGraphViz.c', 'LexGraphViz.cxx', 'GraphViz.dot', 0, parse_graphviz_api_file),
		('NP2LEX_BLOCKDIAG', 'stlBlockdiag.c', 'LexGraphViz.cxx', 'blockdiag.diag', 0, parse_graphviz_api_file),
		('NP2LEX_CSS', 'stlCSS.c', 'LexCSS.cxx', ['CSS.css', 'SCSS.scss', 'Less.less', 'HSS.hss'], 0, parse_css_api_file),
	('NP2LEX_GO', 'stlGo.c', 'LexGo.cxx', 'Go.go', 0, parse_go_api_file),

	('NP2LEX_HAXE', 'stlHaxe.c', 'LexHaxe.cxx', 'Haxe.hx', 1, parse_haxe_api_file),
	('NP2LEX_HTML', 'stlHTML.c', 'LexHTML.cxx', 'html.html', 1, parse_html_api_file),

	('NP2LEX_INI', 'stlDefault.c', 'LexProps.cxx', '', (0, 'Ini'), None),
	('NP2LEX_INNOSETUP', 'stlInno.c', 'LexInno.cxx', 'InnoSetup.iss', 0, parse_inno_setup_api_file),

	('NP2LEX_JAMFILE', 'stlJamfile.c', 'LexJam.cxx', 'Jamfile.jam', 0, parse_jam_api_file),
	('NP2LEX_JAVA', 'stlJava.c', 'LexJava.cxx', 'Java.java', 1, parse_java_api_file),
		('NP2LEX_GROOVY', 'stlGroovy.c', 'LexGroovy.cxx', 'Groovy.groovy', 1, parse_groovy_api_file),
			('NP2LEX_GRADLE', 'stlGradle.c', 'LexGroovy.cxx', 'Gradle.gradle', 1, parse_gradle_api_file),
		('NP2LEX_KOTLIN', 'stlKotlin.c', 'LexKotlin.cxx', 'Kotlin.kt', 0, parse_kotlin_api_file),
	('NP2LEX_JAVASCRIPT', 'stlJavaScript.c', 'LexJavaScript.cxx', 'JavaScript.js', 1, parse_javascript_api_file),
		('NP2LEX_COFFEESCRIPT', 'stlCoffeeScript.c', 'LexCoffeeScript.cxx', 'CoffeeScript.coffee', 0, parse_coffeescript_api_file),
		('NP2LEX_TYPESCRIPT', 'stlTypeScript.c', 'LexJavaScript.cxx', 'TypeScript.ts', 1, parse_typescript_api_file),
	('NP2LEX_JSON', 'stlJSON.c', 'LexJSON.cxx', '', 0, parse_json_api_file),
	('NP2LEX_JULIA', 'stlJulia.c', 'LexJulia.cxx', 'Julia.jl', 0, parse_julia_api_file),

	('NP2LEX_LATEX', 'stlLaTeX.c', 'LexLaTeX.cxx', '', 0, None),
	('NP2LEX_LISP', 'stlLisp.c', 'LexLisp.cxx', '', 0, None),
	('NP2LEX_LLVM', 'stlLLVM.c', 'LexLLVM.cxx', 'LLVM.ll', 0, parse_llvm_api_file),
	('NP2LEX_LUA', 'stlLua.c', 'LexLua.cxx', 'Lua.lua', 0, parse_lua_api_file),

	('NP2LEX_MAKEFILE', 'stlMake.c', 'LexMakefile.cxx', '', 0, None),
	('NP2LEX_MATLAB', 'stlMatlab.c', 'LexMatlab.cxx', '', 0, None),
	('NP2LEX_MARKDOWN', 'stlMarkdown.c', 'LexMarkdown.cxx', '', 0, parse_markdown_api_file),

	('NP2LEX_NSIS', 'stlNsis.c', 'LexNSIS.cxx', 'NSIS.nsi', 0, parse_nsis_api_file),

	('NP2LEX_PASCAL', 'stlPascal.c', 'LexPascal.cxx', '', 0, None),
	('NP2LEX_PERL', 'stlPerl.c', 'LexPerl.cxx', 'Perl.pl', 0, parse_perl_api_file),
	('NP2LEX_PHP', 'stlPHP.c', 'LexPHP.cxx', 'PHP.php', 1, parse_php_api_file),
	('NP2LEX_POWERSHELL', 'stlPowerShell.c', 'LexPowerShell.cxx', 'PowerShell.ps1', 0, parse_powershell_api_file),
	('NP2LEX_PYTHON', 'stlPython.c', 'LexPython.cxx', 'Python.py', 0, parse_python_api_file),

	('NP2LEX_RLANG', 'stlR.c', 'LexR.cxx', 'R.r', 0, parse_rlang_api_file),
	('NP2LEX_REBOL', 'stlRebol.c', 'LexRebol.cxx', ['Rebol.r', 'Red.red'], 1, parse_rebol_api_file),
	('NP2LEX_RESOURCESCRIPT', 'stlResource.c', 'LexCPP.cxx', '', 0, parse_resource_script_api_file),
	('NP2LEX_RUBY', 'stlRuby.c', 'LexRuby.cxx', 'Ruby.rb', 0, parse_ruby_api_file),
	('NP2LEX_RUST', 'stlRust.c', 'LexRust.cxx', 'Rust.rs', 0, parse_rust_api_file),

	('NP2LEX_SCALA', 'stlScala.c', 'LexScala.cxx', 'Scala.scala', 1, parse_scala_api_file),
	('NP2LEX_SMALI', 'stlSmali.c', 'LexSmali.cxx', '', 0, parse_smali_api_file),
	# TODO: SQL Dialect, https://github.com/zufuliu/notepad2/issues/31
	('NP2LEX_SQL', 'stlSQL.c', 'LexSQL.cxx', [
							'MySQL.sql',
							'Oracle.sql',
							'PostgreSQL.sql',
							'SQL.sql',
							'SQLite3.sql',
							'Transact-SQL.sql',
							], 0, parse_sql_api_file),
	('NP2LEX_SWIFT', 'stlSwift.c', 'LexSwift.cxx', 'Swift.swift', 0, parse_swift_api_file),

	('NP2LEX_TCL', 'stlTcl.c', 'LexTCL.cxx', '', 0, None),
	('NP2LEX_TEXINFO', 'stlTexinfo.c', 'LexTexinfo.cxx', 'Texinfo.texi', 0, parse_texinfo_api_file),
	('NP2LEX_TOML', 'stlTOML.c', 'LexTOML.cxx', '', 0, parse_toml_api_file),

	('NP2LEX_VBSCRIPT', 'stlVBS.c', 'LexVB.cxx', '', 0, parse_visual_basic_api_file),
	('NP2LEX_VERILOG', 'stlVerilog.c', 'LexVerilog.cxx', ['Verilog.v', 'SystemVerilog.sv'], 0, parse_verilog_api_file),
	('NP2LEX_VHDL', 'stlVHDL.c', 'LexVHDL.cxx', 'VHDL.vhdl', 0, parse_vhdl_api_file),
	('NP2LEX_VIM', 'stlVim.c', 'LexVim.cxx', 'Vim.vim', 0, parse_vim_api_file),
	('NP2LEX_VISUALBASIC', 'stlVB.c', 'LexVB.cxx', '', 0, parse_visual_basic_api_file),

	# https://github.com/WebAssembly/wabt/blob/main/src/lexer-keywords.txt
	('NP2LEX_WASM', 'stlWASM.c', 'LexWASM.cxx', 'wasm-lexer-keywords.txt', 0, parse_wasm_lexer_keywords),
	('NP2LEX_XML', 'stlXML.c', 'LexHTML.cxx', '', 0, parse_xml_api_file),
	('NP2LEX_YAML', 'stlYAML.c', 'LexYAML.cxx', '', 0, parse_yaml_api_file),
]

def update_all_keyword():
	numKeyword = 16
	handled = set()
	for rid, output, lexer, path, count, parse in lexerList:
		assert rid.startswith('NP2LEX_'), rid
		assert rid not in handled, rid
		assert output.startswith('stl') and output.endswith('.c'), output
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
	UpdateLexerKeywordAttr('../src/EditAutoC.c', '../scintilla/lexers/')

update_all_keyword()
UpdateLexerEnumFile('../src/EditLexer.h', '../scintilla/lexlib/WordList.h')
UpdateAutoCompletionCache('../src/EditAutoC.c')
