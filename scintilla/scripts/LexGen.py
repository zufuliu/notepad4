#!/usr/bin/env python3
import os.path
import glob
import re

from FileGenerator import Regenerate

def FindModules(lexersPath, projectPath):
	doc = open(projectPath, encoding='utf-8').read()
	files = re.findall(r'(Lex\w+\.cxx)', doc)

	lexers = {}
	for path in glob.glob(lexersPath + '/Lex*.cxx'):
		path = os.path.normpath(path)
		name = os.path.basename(path)
		if name not in files:
			print('Ignore lexer file:', path)
			continue

		doc = open(path, encoding='utf-8').read()
		items = re.findall(r'^\s*LexerModule\s+(\w+)\s*\(\s*(\w+)', doc, re.MULTILINE)
		if not items:
			print('No lexer in file:', path)
		else:
			for module, lexer in items:
				lexers[lexer] = module
	return lexers

def RegenerateAll():
	lexers = FindModules('../lexers', '../../build/VS2017/Notepad2.vcxproj')

	modules = list(lexers.values())
	modules.sort(key=lambda m: m.lower())
	Regenerate('../lexlib/LexerModule.cxx', '//', modules)

RegenerateAll()
