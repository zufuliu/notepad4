from KeywordCore import *

def update_all_keyword():
	items = [
		('NP2LEX_ABAQUS', 'stlABAQUS.c', 'LexAPDL.cxx', 'ABAQUS.inp', 0, parse_apdl_api_file),
		('NP2LEX_ACTIONSCRIPT', 'stlActionScript.c', 'LexJavaScript.cxx', 'ActionScript.as', 1, parse_actionscript_api_file),
		('NP2LEX_AHK', 'stlAutoHotkey.c', 'LexAutoHotkey.cxx', ['AutoHotkey_L.ahk', 'AutoHotkey_H.ahk'], 0, parse_autohotkey_api_file),
		('NP2LEX_APDL', 'stlAPDL.c', 'LexAPDL.cxx', 'APDL.cdb', 0, parse_apdl_api_file),
		('NP2LEX_ASYMPTOTE', 'stlAsymptote.c', 'LexAsymptote.cxx', 'Asymptote.asy', 1, parse_asymptote_api_file),
		('NP2LEX_AVS', 'stlAviSynth.c', 'LexAviSynth.cxx', 'AviSynth.avs', 0, parse_avisynth_api_file),
		('NP2LEX_AWK', 'stlAwk.c', 'LexAwk.cxx', 'Awk.awk', 1, parse_awk_api_file),
		('NP2LEX_BATCH', 'stlBatch.c', 'LexBatch.cxx', 'Batch.bat', 0, parse_batch_api_file),
		('NP2LEX_CMAKE', 'stlCMake.c', 'LexCMake.cxx', 'CMake.cmake', 0, parse_cmake_api_file),
		('NP2LEX_CSHARP', 'stlCSharp.c', 'LexCSharp.cxx', 'CSharp.cs', 1, parse_csharp_api_file),
		('NP2LEX_D', 'stlD.c', 'LexD.cxx', 'D.d', 1, parse_dlang_api_file),
		('NP2LEX_DART', 'stlDart.c', 'LexDart.cxx', 'Dart.dart', 0, parse_dart_api_file),
		('NP2LEX_GN', 'stlGN.c', 'LexGN.cxx', 'GN.gn', 0, parse_gn_api_file),
		('NP2LEX_GRAPHVIZ', 'stlGraphViz.c', 'LexGraphViz.cxx', 'GraphViz.dot', 0, parse_graphviz_api_file),
			('NP2LEX_BLOCKDIAG', 'stlBlockdiag.c', 'LexGraphViz.cxx', 'blockdiag.diag', 0, parse_graphviz_api_file),
			('NP2LEX_CSS', 'stlCSS.c', 'LexCSS.cxx', ['CSS.css', 'SCSS.scss', 'Less.less', 'HSS.hss'], 0, parse_css_api_file),
		('NP2LEX_GO', 'stlGO.c', 'LexGo.cxx', 'Go.go', 0, parse_go_api_file),
		('NP2LEX_HAXE', 'stlHaxe.c', 'LexHaxe.cxx', 'Haxe.hx', 1, parse_haxe_api_file),
		('NP2LEX_INNO', 'stlInno.c', 'LexInno.cxx', 'InnoSetup.iss', 0, parse_inno_setup_api_file),
		('NP2LEX_JAM', 'stlJamfile.c', 'LexJam.cxx', 'Jamfile.jam', 0, parse_jam_api_file),
		('NP2LEX_JAVA', 'stlJava.c', 'LexJava.cxx', 'Java.java', 1, parse_java_api_file),
			('NP2LEX_GROOVY', 'stlGroovy.c', 'LexGroovy.cxx', 'Groovy.groovy', 1, parse_groovy_api_file),
				('NP2LEX_GRADLE', 'stlGradle.c', 'LexGroovy.cxx', 'Gradle.gradle', 1, parse_gradle_api_file),
			('NP2LEX_KOTLIN', 'stlKotlin.c', 'LexKotlin.cxx', 'Kotlin.kt', 0, parse_kotlin_api_file),
		('NP2LEX_JAVASCRIPT', 'stlJavaScript.c', 'LexJavaScript.cxx', 'JavaScript.js', 1, parse_javascript_api_file),
			('NP2LEX_COFFEESCRIPT', 'stlCoffeeScript.c', 'LexCoffeeScript.cxx', 'CoffeeScript.coffee', 0, parse_coffeescript_api_file),
			('NP2LEX_TYPESCRIPT', 'stlTypeScript.c', 'LexJavaScript.cxx', 'TypeScript.ts', 1, parse_typescript_api_file),
		('NP2LEX_JULIA', 'stlJulia.c', 'LexJulia.cxx', 'Julia.jl', 0, parse_julia_api_file),
		('NP2LEX_LLVM', 'stlLLVM.c', 'LexLLVM.cxx', 'LLVM.ll', 0, parse_llvm_api_file),
		('NP2LEX_LUA', 'stlLua.c', 'LexLua.cxx', 'Lua.lua', 0, parse_lua_api_file),
		('NP2LEX_MARKDOWN', 'stlDefault.c', 'LexMarkdown.cxx', '', 0, parse_markdown_api_file),
		('NP2LEX_NSIS', 'stlNsis.c', 'LexNSIS.cxx', 'NSIS.nsi', 0, parse_nsis_api_file),
		('NP2LEX_PHP', 'stlPHP.c', 'LexPHP.cxx', 'PHP.php', 1, parse_php_api_file),
		('NP2LEX_PYTHON', 'stlPython.c', 'LexPython.cxx', 'Python.py', 0, parse_python_api_file),
		('NP2LEX_R', 'stlR.c', 'LexR.cxx', 'R.r', 0, parse_r_api_file),
		('NP2LEX_REBOL', 'stlRebol.c', 'LexRebol.cxx', ['Rebol.r', 'Red.red'], 1, parse_rebol_api_file),
		('NP2LEX_RUBY', 'stlRuby.c', 'LexRuby.cxx', 'Ruby.rb', 0, parse_ruby_api_file),
		('NP2LEX_RUST', 'stlRust.c', 'LexRust.cxx', 'Rust.rs', 0, parse_rust_api_file),
		# TODO: SQL Dialect, https://github.com/zufuliu/notepad2/issues/31
		('NP2LEX_SQL', 'stlSQL.c', 'LexSQL.cxx', [
								'MySQL.sql',
								'Oracle.sql',
								'PostgreSQL.sql',
								'SQL.sql',
								'SQLite3.sql',
								'Transact-SQL.sql',
								], 0, parse_sql_api_files),
		('NP2LEX_SWIFT', 'stlSwift.c', 'LexSwift.cxx', 'Swift.swift', 0, parse_swift_api_file),
		('NP2LEX_VIM', 'stlVim.c', 'LexVim.cxx', 'Vim.vim', 0, parse_vim_api_file),
		# https://github.com/WebAssembly/wabt/blob/main/src/lexer-keywords.txt
		('NP2LEX_WASM', 'stlWASM.c', 'LexWASM.cxx', 'wasm-lexer-keywords.txt', 0, parse_wasm_lexer_keywords),
	]

	numKeyword = 16
	for rid, output, lexer, path, count, parse in items:
		assert rid.startswith('NP2LEX_'), rid
		assert output.startswith('stl') and output.endswith('.c'), output
		assert lexer.startswith('Lex') and lexer.endswith('.cxx'), lexer

		if isinstance(path, str):
			keywordList = parse('lang/' + path)
		else:
			pathList = ['lang/' + name for name in path]
			keywordList = parse(pathList)
		if keywordList:
			output = '../src/EditLexers/' + output
			if isinstance(count, tuple):
				num, suffix = count
			else:
				num = count
				suffix = ''
			UpdateKeywordFile(rid, output, lexer, keywordList, numKeyword - num, suffix)

	update_lexer_keyword_attr('../src/Styles.c', '../src/EditAutoC.c', '../scintilla/lexers/')

update_all_keyword()
