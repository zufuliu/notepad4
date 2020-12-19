#!/usr/bin/env python3
from KeywordCore import *

def update_all_keyword():
	items = [
		('NP2LEX_AVS', 'stlAviSynth.c', 'AviSynth.avs', 0, parse_avisynth_api_file),
		('NP2LEX_CMAKE', 'stlCMake.c', 'CMake.cmake', 0, parse_cmake_api_file),
		('NP2LEX_DART', 'stlDart.c', 'Dart.dart', 0, parse_dart_api_file),
		('NP2LEX_GN', 'stlGN.c', 'GN.gn', 0, parse_gn_api_file),
		('NP2LEX_GO', 'stlGO.c', 'Go.go', 0, parse_go_api_file),
		('NP2LEX_JS', 'stlJavaScript.c', 'JavaScript.js', 1, parse_javascript_api_file),
		('NP2LEX_JULIA', 'stlJulia.c', 'Julia.jl', 0, parse_julia_api_file),
		('NP2LEX_KOTLIN', 'stlKotlin.c', 'Kotlin.kt', 0, parse_kotlin_api_file),
		('NP2LEX_LLVM', 'stlLLVM.c', 'LLVM.ll', 0, parse_llvm_api_file),
		('NP2LEX_LUA', 'stlLua.c', 'Lua.lua', 0, parse_lua_api_file),
		('NP2LEX_R', 'stlR.c', 'R.r', 0, parse_r_api_file),
		('NP2LEX_RUBY', 'stlRuby.c', 'Ruby.rb', 0, parse_ruby_api_file),
		('NP2LEX_RUST', 'stlRust.c', 'Rust.rs', 0, parse_rust_api_file),
		# TODO: SQL Dialect, https://github.com/zufuliu/notepad2/issues/31
		('NP2LEX_SQL', 'stlSQL.c', [
								'MySQL.sql',
								'Oracle.sql',
								'PostgreSQL.sql',
								'SQL.sql',
								'SQLite3.sql',
								'Transact-SQL.sql',
								], 0, parse_sql_api_files),
		('NP2LEX_VIM', 'stlVim.c', 'Vim.vim', 0, parse_vim_api_file),
		# https://github.com/WebAssembly/wabt/blob/master/src/lexer-keywords.txt
		('NP2LEX_WASM', 'stlWASM.c', 'wasm-lexer-keywords.txt', 0, parse_wasm_lexer_keywords),
	]

	for rid, output, path, count, parse in items:
		if isinstance(path, str):
			keywordList = parse('lang/' + path)
		else:
			pathList = ['lang/' + name for name in path]
			keywordList = parse(pathList)
		if keywordList:
			output = '../src/EditLexers/' + output
			UpdateKeywordFile(rid, output, keywordList, 16 - count)

	update_lexer_keyword_attr('../src/Styles.c')

update_all_keyword()
