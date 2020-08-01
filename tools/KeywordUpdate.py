#!/usr/bin/env python3
from KeywordCore import *

def update_all_keyword():
	items = [
		('NP2LEX_CMAKE', 'stlCMake.c', 'CMake.cmake', 16, parse_cmake_api_file),
		('NP2LEX_GN', 'stlGN.c', 'GN.gn', 16, parse_gn_api_file),
		('NP2LEX_GO', 'stlGO.c', 'Go.go', 16, parse_go_api_file),
		('NP2LEX_JS', 'stlJavaScript.c', 'JavaScript.js', 15, parse_javascript_api_file),
		('NP2LEX_JULIA', 'stlJulia.c', 'Julia.jl', 16, parse_julia_api_file),
		('NP2LEX_KOTLIN', 'stlKotlin.c', 'Kotlin.kt', 16, parse_kotlin_api_file),
		('NP2LEX_LLVM', 'stlLLVM.c', 'LLVM.ll', 16, parse_llvm_api_file),
		('NP2LEX_RUBY', 'stlRuby.c', 'Ruby.rb', 16, parse_ruby_api_file),
		('NP2LEX_RUST', 'stlRust.c', 'Rust.rs', 16, parse_rust_api_file),
		('NP2LEX_VIM', 'stlVim.c', 'Vim.vim', 16, parse_vim_api_file),
		# https://github.com/WebAssembly/wabt/blob/master/src/lexer-keywords.txt
		('NP2LEX_WASM', 'stlWASM.c', 'wasm-lexer-keywords.txt', 16, parse_wasm_lexer_keywords),
	]

	for rid, output, path, count, parse in items:
		keywordList = parse('lang/' + path)
		if keywordList:
			output = '../src/EditLexers/' + output
			UpdateKeywordFile(rid, output, keywordList, count)

	update_lexer_keyword_attr('../src/Styles.c')

update_all_keyword()
