import os.path
import glob
import re

import FileGenerator

def FindModules(lexersPath, projectPath):
	with open(projectPath, encoding='utf-8') as fd:
		doc = fd.read()
	files = re.findall(r'(Lex\w+\.cxx)', doc)

	lexers = {}
	for path in glob.glob(lexersPath + '/Lex*.cxx'):
		path = os.path.normpath(path)
		name = os.path.basename(path)
		if name not in files:
			print('Ignore lexer file:', path)
			continue

		with open(path, encoding='utf-8') as fd:
			doc = fd.read()
		items = re.findall(r'^\s*LexerModule\s+(\w+)\s*\(\s*(\w+)', doc, re.MULTILINE)
		if not items:
			print('No lexer in file:', path)
		else:
			for module, lexer in items:
				lexers[lexer] = module
	return lexers

def RegenerateAll():
	lexers = FindModules('../lexers', '../../build/VS2017/Notepad2.vcxproj')

	modules = sorted(lexers.values(), key=lambda m: m.lower())
	FileGenerator.Regenerate('../lexlib/LexerModule.cxx', '//', modules)

RegenerateAll()
