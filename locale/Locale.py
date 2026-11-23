#!/usr/bin/env python3
import sys
import os.path
import re
import json
import shutil
import uuid
import subprocess

app = os.path.basename(__file__)
localeDir = os.getcwd()
notepad4_src = os.path.abspath('../src/Notepad4.rc')
matepath_src = os.path.abspath('../matepath/src/matepath.rc')

def get_available_locales():
	result = []
	with os.scandir(localeDir) as it:
		for entry in it:
			if entry.is_dir() and entry.name[:2].islower():
				result.append(entry.name)

	result.sort()
	return result

def get_project_guid():
	return '{' + str(uuid.uuid4()).upper() + '}'

def patch_vc_project_file(path, src_lang, language):
	with open(path, encoding='utf-8', newline='\n') as fd:
		doc = fd.read()
	# change output folder
	doc = doc.replace(f'\\{src_lang}\\', f'\\{language}\\')

	# change project GUID
	guid = get_project_guid()
	doc = re.sub(r'(<ProjectGuid>)(.+)(</ProjectGuid>)', r'\1' + guid + r'\3', doc)

	with open(path, 'w', encoding='utf-8', newline='\n') as fd:
		fd.write(doc)

def update_resource_include_path(path, matepath):
	with open(path, encoding='utf-8', newline='\n') as fd:
		doc = fd.read()
	if matepath:
		# resource path
		doc = doc.replace(r'..\\res', r'..\\..\\matepath\\res')
		# include path
		doc = re.sub(r'^(#include\s+")(.+)(")', r'\1../../matepath/src/\2\3', doc, flags=re.MULTILINE)
	else:
		# resource path
		doc = doc.replace(r'..\\res', r'..\\..\\res')
		# include path
		doc = re.sub(r'^(#include\s+")(.+)(")', r'\1../../src/\2\3', doc, flags=re.MULTILINE)
		# string table for lexers and styles
		doc = re.sub(r'^//(#include\s+")(.+)(")', r'\1../../src/\2\3', doc, flags=re.MULTILINE)
		doc = re.sub(r'^#if\s+0\s*//\s*(NP2_ENABLE_LOCALIZE\w+)', r'#if \1', doc, flags=re.MULTILINE)

	doc = re.sub(r'^//(#if\s+!NP2_ENABLE_APP_LOCALIZATION_DLL)', r'\1', doc, flags=re.MULTILINE)
	doc = re.sub(r'^//(#endif\s*//\s*NP2_ENABLE_APP_LOCALIZATION_DLL)', r'\1', doc, flags=re.MULTILINE)
	with open(path, 'w', encoding='utf-8', newline='\n') as fd:
		fd.write(doc)

def make_new_localization(language):
	print(f'{app}: make new localization for {language}.')
	target = os.path.join(localeDir, language)
	if not os.path.exists(target):
		os.makedirs(target)

	matepath_dest = os.path.join(target, 'matepath.rc')
	notepad4_dest = os.path.join(target, 'Notepad4.rc')
	shutil.copyfile(matepath_src, matepath_dest)
	shutil.copyfile(notepad4_src, notepad4_dest)

	update_resource_include_path(matepath_dest, True)
	update_resource_include_path(notepad4_dest, False)

	src_lang = 'zh-Hans'
	src_folder = os.path.join(localeDir, src_lang)
	matepath_vcxproj_src = os.path.join(src_folder, f'matepath({src_lang}).vcxproj')
	notepad4_vcxproj_src = os.path.join(src_folder, f'Notepad4({src_lang}).vcxproj')
	matepath_vcxproj = os.path.join(target, f'matepath({language}).vcxproj')
	notepad4_vcxproj = os.path.join(target, f'Notepad4({language}).vcxproj')

	shutil.copyfile(matepath_vcxproj_src, matepath_vcxproj)
	shutil.copyfile(matepath_vcxproj_src + '.filters', matepath_vcxproj + '.filters')
	shutil.copyfile(notepad4_vcxproj_src, notepad4_vcxproj)
	shutil.copyfile(notepad4_vcxproj_src + '.filters', notepad4_vcxproj + '.filters')

	patch_vc_project_file(matepath_vcxproj, src_lang, language)
	patch_vc_project_file(notepad4_vcxproj, src_lang, language)

	matepath_dest = os.path.basename(matepath_dest)
	notepad4_dest = os.path.basename(notepad4_dest)
	matepath_vcxproj = os.path.basename(matepath_vcxproj)
	notepad4_vcxproj = os.path.basename(notepad4_vcxproj)
	print(f"""{app}: resources and projects added for {language}.
    Please manually update language tags in {matepath_dest} and {notepad4_dest},
    and open Locale.sln with Visual Studio to add project {matepath_vcxproj} and {notepad4_vcxproj}.""")


def restore_resource_include_path(path, matepath):
	with open(path, encoding='utf-8', newline='\n') as fd:
		doc = fd.read()
	if matepath:
		# include path
		doc = doc.replace('../../matepath/src/', '')
		# resource path
		doc = doc.replace(r'..\\matepath\\', '')
	else:
		# include path
		doc = doc.replace('../../src/', '')
		# resource path
		doc = doc.replace(r'..\\..\\res', r'..\\res')

	with open(path, 'w', encoding='utf-8', newline='\n') as fd:
		fd.write(doc)

def copy_back_localized_resources(language):
	print(f'{app}: copy back localized resources for {language}.')
	backupDir = os.path.join(localeDir, 'en')
	if os.path.exists(backupDir):
		if language != 'en':
			print(f"""{app}: please delete the backup folder {backupDir}
    on first run to ensure proper backup for English resources.""")
	else:
		os.makedirs(backupDir)
		shutil.copyfile(matepath_src, os.path.join(backupDir, 'matepath.rc'))
		shutil.copyfile(notepad4_src, os.path.join(backupDir, 'Notepad4.rc'))

	folder = os.path.join(localeDir, language)
	shutil.copyfile(os.path.join(folder, 'matepath.rc'), matepath_src)
	shutil.copyfile(os.path.join(folder, 'Notepad4.rc'), notepad4_src)

	restore_resource_include_path(matepath_src, True)
	restore_resource_include_path(notepad4_src, False)

	print(f"""{app}: resources for building standalone localized program for {language} are ready.
    you can copy English resources back by run: {app} back en""")


class StringExtractor:
	def reset(self, path, reversion):
		self.path = path
		self.reversion = reversion
		self.changed_lines = set()
		if reversion:
			self.find_changed_lines(reversion)
			if not self.changed_lines:
				return False

		with open(path, encoding='utf-8', newline='\n') as fd:
			doc = fd.read()
		self.lines = doc.splitlines()
		return len(self.lines)

	def find_changed_lines(self, reversion):
		cmd = ['git', 'diff', '--no-color', '--unified=0', '--text', reversion, self.path]
		result = subprocess.run(cmd, capture_output=True, check=True, encoding='utf-8')
		if result.stderr:
			print(result.stderr, file=sys.stderr)

		items = re.findall(r'^@@\s+\-\d+(,\d+)?\s+\+(\d+)(,\d+)?\s+@@', result.stdout, re.MULTILINE)
		if not items:
			return

		for item in items:
			_, line, count = item
			line = int(line)
			count = int(count[1:]) if count else 0
			if count:
				self.changed_lines.update(range(line, line + count))
			else:
				self.changed_lines.add(line)

		# reversion time
		cmd =['git', 'show', '--no-patch', '--no-notes', "--pretty='%ci'", reversion]
		result = subprocess.run(cmd, capture_output=True, check=True, encoding='utf-8')
		if result.stderr:
			print(result.stderr, file=sys.stderr)
		items = result.stdout.replace("'", '').split()[:2]
		self.reversion = f"{reversion} {' '.join(items)}".strip()

	def is_line_changed(self, start, end):
		if not self.changed_lines:
			return True

		while start <= end:
			if start in self.changed_lines:
				return True
			start += 1
		return False

	def match_line(self, line, word):
		if line.startswith(word):
			ch = ' ' if len(line) == len(word) else line[len(word)]
			return ch.isspace() or ch == '/'
		return False

	def scan_string(self, line, escape_sequence, format_specifier, access_key, start):
		index = 0
		if start:
			# identifier "string"
			index = line.find('"')
			if index <= 0:
				return '', 0, 0, False

		length = len(line)
		begin = index
		if start:
			index += 1
		stop = False

		while index < length:
			ch = line[index]
			index += 1
			if ch == '\\':
				ch = line[index] if index < length else ''
				end = index + 1
				if ch and ch in 'abfnrtvxu':
					if ch == 'x':
						end += 2
					elif ch == 'u':
						end += 4
					sequence = line[index - 1:end]
					escape_sequence.add(sequence)
				index = end
			elif ch == '&':
				ch = line[index]
				if ch == '&':
					index += 1
				elif not ch.isspace():
					access_key.append(ch.upper())
			elif ch == '%':
				ch = line[index]
				# we only use '%s' in resource files
				if ch == 's':
					format_specifier.add('%' + ch)
			elif ch == '"':
				if index < length and line[index] == ch:
					index += 1
				else:
					stop = True
					break

		value = line[begin:index]
		return value, begin, index, stop

	def build_hint(self, item, escape_sequence, format_specifier, access_key, width=''):
		hint = [item['comment']]
		if access_key:
			access_key = ' '.join(access_key)
			item['access_key'] = access_key
			hint.append(', access key: ')
			hint.append(access_key)
		if escape_sequence:
			escape_sequence = ''.join(sorted(escape_sequence))
			item['escape_sequence'] = escape_sequence
			hint.append(', escape sequence: ')
			hint.append(escape_sequence)
		if format_specifier:
			format_specifier = ' '.join(sorted(format_specifier))
			item['format_specifier'] = format_specifier
			hint.append(', format specifier: ')
			hint.append(format_specifier)
		hint.append(width)
		item['comment'] = ''.join(hint)

	def parse_resource_item(self, lineno, line, block_items):
		if not self.is_line_changed(lineno, lineno):
			return None

		escape_sequence = set()
		format_specifier = set()
		access_key = []
		value, begin, index, _ = self.scan_string(line, escape_sequence, format_specifier, access_key, True)
		if not value or len(value) <= 2:
			return None

		# type "string", id
		word = line[:begin].strip()
		if word == 'POPUP' and value == '"+"':
			return None
		if not word.isidentifier():
			if not line.startswith('FONT '):
				print(f'ignore {lineno}', line)
			return None

		rcid = ''
		width = ''
		begin = line.find(',', index)
		if begin > 0:
			index = line.find(',', begin + 1)
			if index > 0:
				rcid = line[begin + 1:index].strip()
				match = re.search(r',\s*\d+\s*,\s*\d+\s*,(\d+)\s*,\s*\d+', line)
				width = int(match.group(1))
				minCount = width >> 2
				maxCount = minCount + (minCount >> 1)
				width = f', width: {width}, character: {minCount} {maxCount}'
			else:
				rcid = line[begin + 1:].strip()
			assert rcid.isidentifier()

		if word == 'CAPTION':
			return value
		if word == 'MENUITEM' and not rcid:
			print(f'no menu id {lineno} {line}')
		if len(access_key) > 1:
			print(f'multiple access keys {lineno}', access_key, line)

		comment = f'// {lineno} {word} {rcid}'.strip()
		item = {'id': rcid, 'type': word, 'value': value, 'comment': comment, }
		self.build_hint(item, escape_sequence, format_specifier, access_key, width)
		block_items.append(item)
		return None

	def parse_string_table_item(self, lineno, line, block_items):
		# id "multiline string"
		escape_sequence = set()
		format_specifier = set()
		access_key = []
		value, begin, index, stop = self.scan_string(line, escape_sequence, format_specifier, access_key, True)

		rcid = line[:begin].strip()
		assert rcid.isidentifier()

		result = [value]
		start = lineno
		while not stop:
			line = self.lines[lineno]
			lineno += 1
			value, begin, index, stop = self.scan_string(line, escape_sequence, format_specifier, access_key, False)
			result.append(value)

		if not self.is_line_changed(start, lineno):
			return lineno

		value = '\n'.join(result)
		if not any(ch.isalpha() for ch in value):
			return lineno
		if len(access_key) > 1:
			print(f'multiple access keys {start} {rcid}', access_key)

		comment = f'// {start} {rcid}'
		item = {'id': rcid, 'value': value, 'comment': comment, }
		self.build_hint(item, escape_sequence, format_specifier, access_key)
		block_items.append(item)
		return lineno

	def extract(self, path, reversion, out_path=None):
		if not self.reset(path, reversion):
			return

		Block_None = 0
		Block_Menu = 1
		Block_DialogEx = 2
		Block_StringTable = 3
		Block_Ignore = 4

		block_type = Block_None
		block_name = ''
		block_caption = ''
		begin = 0
		block_begin = 0
		block_items = []

		lineno = 0
		line_count = len(self.lines)
		string_list = []

		while lineno < line_count:
			line = self.lines[lineno]
			line = line.strip()
			lineno += 1
			if not line or line.startswith('//') or line.startswith('#'):
				continue

			if block_type == Block_None:
				begin = 0
				if self.match_line(line, 'STRINGTABLE'):
					block_type = Block_StringTable
					block_name = 'STRINGTABLE'
				else:
					items = line.split()
					if len(items) >= 2:
						word = items[1]
						if word == 'MENU':
							block_type = Block_Menu
							block_name = ' '.join(items[:2])
						elif word == 'DIALOGEX':
							block_type = Block_DialogEx
							block_name = ' '.join(items[:2])
						elif word in ('ACCELERATORS', 'DESIGNINFO', 'TEXTINCLUDE'):
							block_type = Block_Ignore
				if block_type != Block_None:
					block_begin = lineno
					block_items = []
					block_caption = ''
			elif self.match_line(line, 'BEGIN'):
				begin += 1
			elif self.match_line(line, 'END'):
				begin -= 1
				if begin <= 0:
					block_type = Block_None
					if block_items:
						string_list.append({
							'name': block_name,
							'comment': f'// line {block_begin} - {lineno}',
							'caption': block_caption,
							'items': block_items
						})
						block_items = []
			elif block_type != Block_Ignore:
				try:
					if block_type in (Block_Menu, Block_DialogEx):
						caption = self.parse_resource_item(lineno, line, block_items)
						if caption:
							block_caption = caption
					elif block_type == Block_StringTable:
						lineno = self.parse_string_table_item(lineno, line, block_items)
				except Exception:
					print(f'parse {block_type} {block_name} fail at {lineno} for {self.path}')
					raise

		if string_list:
			self.save(string_list, out_path)

	def save_string_list(self, path, string_list):
		print('save:', path)
		with open(path, 'w', encoding='utf-8') as fd:
			fd.write("//! Ignore line starts with //, it's a comment line.\n")
			fd.write("//! Please don't translate escape sequence or format specifiers.\n")
			if self.reversion:
				fd.write("//! Updated strings since: " + self.reversion + '\n')
			fd.write('\n')

			for block in string_list:
				fd.write(block['comment'] + '\n')
				fd.write(block['name'] + '\n')
				caption = block['caption']
				if caption:
					fd.write(f'CAPTION {caption}\n')

				fd.write('BEGIN\n')
				for item in block['items']:
					fd.write(f"\t{item['comment']}\n")
					fd.write(f"\t{item['value']}\n")
				fd.write('END\n\n')

	def save(self, string_list, out_path=None):
		if not out_path:
			path, ext = os.path.splitext(self.path)
			out_path = path + '-string' + ext

		self.save_string_list(out_path, string_list)
		out_path = os.path.splitext(out_path)[0] + '.json'
		print('save:', out_path)
		with open(out_path, 'w', encoding='utf-8') as fd:
			fd.write(json.dumps(string_list, indent='\t', ensure_ascii=False))


	def init_translate(self, language):
		self.reversion = ''
		self.language = language
		language = language[:2]
		self.is_cjk = language in ('ja', 'ko', 'zh')
		self.is_latin = language not in ('ja', 'ko', 'zh', 'ru')
		self.ignore_ok = language not in ('ko', 'zh')
		self.single_word = set("""Notepad4 matepath Zufu Liu Florian Balmer XhmikosR Scintilla
Apache
Base64 Big5 Big5-HKSCS Bing Boost
ClearType
Direct2D Direct3D
Esc
GitHub GitLab Google
JavaScript Johab JScript
LaTeX Less
Mac macOS Markdown Mathematica
Octave
Pandoc Perl
RegExp
Scilab Shebang Shell Shift-JIS
TypeScript
Unicode Unix US-Canada
VBScript
Windows
7-bit abc a-z ms ns us uHHHH xHH
""".split())
		self.dual_word = set([
			('ANSI', 'Art'),
			('Direct2D', 'Retain'),
			('EBCDIC', 'Greek'), ('EBCDIC', 'Intl'),
			('Property', 'List'),
			('US-Canada', 'Euro'),
			('Notepad4', 'ini'), ('matepath', 'ini'),
			('Visual', 'Basic'), ('Visual', 'Studio'),
		])
		self.dual_start = set(item[0] for item in self.dual_word)

	def check_word(self, text, match, word, previous, rctype):
		if not any(ch.islower() for ch in word):
			if word == 'OK' and not self.ignore_ok:
				return True
			return False # all upper case or digits
		if word in self.single_word or re.fullmatch(r'Windows-\d+', word) or re.fullmatch(r'Latin-\d+', word):
			return False # encoding name
		start, end = match.span()
		if len(word) == 3 and word.islower() and text[start - 2:start] == '*.':
			return False # file extension
		before = text[start - 1]
		after = text[end]
		if after == '+' and before in '(+\t' and word in ('Ctrl', 'Alt', 'Shift'):
			return False # key name
		if before == '+' and word in ('Back', 'Del', 'Tab', 'Space', 'Up', 'Down') and previous in ('Ctrl', 'Alt', 'Shift'):
			return False # key name
		if before == '\t' and after == '"' and rctype == 'MENUITEM' and word in ('Delete', 'Del', 'Tab'):
			return False # key name
		if (previous, word) in self.dual_word:
			return 3
		if word in self.dual_start:
			return 2
		return True

	def find_word(self, text, rctype, iterator=None):
		if not iterator:
			iterator = re.finditer(r'[\w\-]{2,}', text)
		items = []
		previous = ''
		dual = False
		for match in iterator:
			word = match.group(0)
			result = self.check_word(text, match, word, previous, rctype)
			if dual and result != 3:
				items.append(previous.lower())
			if result == 1:
				items.append(word.lower())
			dual = result == 2
			previous = word
		if dual:
			items.append(previous.lower())
		return items

	def check_translate(self, origin, target, rctype=''):
		iterator = None if self.is_latin else re.finditer(r'[a-zA-Z0-9\u0080-\u036F\-]{2,}', target)
		missing = self.find_word(target, rctype, iterator)
		if not missing:
			return False
		if not self.is_latin:
			return True
		source = set(self.find_word(origin, rctype))
		missing = set(missing)
		same = source & missing
		if same:
			return True
		return False

	def get_item_string(self, item):
		value = item['value']
		if 'access_key' in item:
			value = value.replace('&', '')
		if 'escape_sequence' in item:
			escape_sequence = item['escape_sequence'].split()
			for escape in escape_sequence:
				value = value.replace(escape, '\t')
		if 'format_specifier' in item:
			format_specifier = item['format_specifier'].split()
			for specifier in format_specifier:
				value = value.replace(specifier, ' ')
		value = re.sub(r'(https?://[\w/.:?]+)', ' ', value)
		return value

	def find_untranslated_string(self, src, path):
		filename = os.path.basename(path)
		print('read:', src)
		with open(src, encoding='utf-8') as fd:
			source_list = json.load(fd)
		print('read:', path)
		with open(path, encoding='utf-8') as fd:
			string_list = json.load(fd)
		if len(source_list) != len(string_list):
			print(f'block count diff: {len(source_list)} => {len(string_list)} {self.language} {filename}')

		changed_lines = []
		total = 0
		for block_index, src_block in enumerate(source_list):
			block = string_list[block_index]
			block_name = src_block['name']
			source = src_block['items']
			items = block['items']
			if len(source) != len(items):
				print(f"items count diff: {len(source)} => {len(items)} {block_name} {block['comment']}")

			updated = []
			for index, src_item in enumerate(source):
				item = items[index]
				rcid = src_item['id']
				if rcid != item['id']:
					print(f"id diff: {rcid} => {item['id']} {item['comment']}")
				rctype = src_item.get('type', '')
				word = item.get('type', '')
				if rctype != word:
					print(f"type diff: {rctype} => {word} {item['comment']}")

				origin = self.get_item_string(src_item)
				value = self.get_item_string(item)
				if self.check_translate(origin, value, rctype):
					total += 1
					updated.append(item)
				if self.is_cjk:
					origin = src_item.get('access_key', '')
					value = item.get('access_key', '')
					if origin != value:
						print(f"access key diff {rcid}: [{origin}] => [{value}] {item['comment']}")

			caption = src_block['caption']
			change = False
			if caption:
				title = block['caption']
				if self.check_translate(caption, title):
					change = True
					total += 1
			if change or updated:
				block['items'] = updated
				changed_lines.append(block)

		print('untranslated:', len(changed_lines), total, filename)
		if changed_lines:
			path = path.replace('-string.json', '-new.rc')
			self.save_string_list(path, changed_lines)

def extract_resource_string(language, reversion):
	print(f'{app}: extract updated string for {language} since {reversion}.')

	extractor = StringExtractor()
	if language == 'en':
		extractor.extract(matepath_src, reversion)
		extractor.extract(notepad4_src, reversion)
	else:
		folder = os.path.join(localeDir, language)
		path = os.path.join(folder, 'matepath.rc')
		extractor.extract(path, reversion)
		path = os.path.join(folder, 'Notepad4.rc')
		extractor.extract(path, reversion)

def find_untranslated_string(language):
	extractor = StringExtractor()
	extractor.init_translate(language)
	folder = os.path.join(localeDir, language)
	matepath_dest = os.path.join(folder, 'matepath-string.json')
	path = matepath_src.replace('.rc', '-string.json')
	extractor.find_untranslated_string(path, matepath_dest)
	path = notepad4_src.replace('.rc', '-string.json')
	notepad4_dest = os.path.join(folder, 'Notepad4-string.json')
	extractor.find_untranslated_string(path, notepad4_dest)


def show_help():
	print(f"""Usage: {app} action language [reversion]
action:
    new     create a new localization for specific language.

    back    prepare building standalone localized program for specific language,
            copy back localized resources to overwrite English resources.
            English resources are copied into en folder when the folder does not exist.

    string  extract all resource string or updated strings since specific reversion.
""")

def main(argv):
	if len(argv) < 3:
		show_help()
		return

	action = argv[1]
	language = argv[2]
	availableLocales = get_available_locales()
	if action == 'new':
		if language in availableLocales:
			print(f'{app}: language {language} already localized.')
			return
		make_new_localization(language)
	elif action == 'back':
		if language not in availableLocales:
			print(f'{app}: language {language} not localized [{", ".join(availableLocales)}].')
			return
		copy_back_localized_resources(language)
	elif action == 'string':
		if language != 'en' and language not in availableLocales:
			print(f'{app}: language {language} not localized [{", ".join(availableLocales)}].')
			return
		reversion = argv[3] if len(argv) > 3 else ''
		extract_resource_string(language, reversion)
	elif action == 'trans' and language != 'en':
		find_untranslated_string(language)
	else:
		show_help()

if __name__ == '__main__':
	main(sys.argv)
