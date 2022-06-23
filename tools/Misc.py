import sys
import os.path
import re
import json
import unicodedata

def increase_style_resource_id_value(path, delta=100):
	with open(path, encoding='utf-8', newline='\n') as fd:
		doc = fd.read()
	updated = re.sub(r'\d{5}', lambda m: str(int(m.group(0)) + delta), doc)
	print('update:', path)
	with open(path, 'w', encoding='utf-8', newline='\n') as fp:
		fp.write(updated)

def check_encoding_list(path):
	def is_tag_char(ch):
		return (ch >= 'a' and ch <= 'z') or (ch >= '0' and ch <= '9')

	with open(path, encoding='utf-8', newline='\n') as fd:
		doc = fd.read()
	lines = doc.splitlines()
	started = False
	name_map = {}
	tag_map = {}
	page_map = {}

	for index, line in enumerate(lines):
		if not started:
			started = line.startswith('NP2ENCODING mEncoding[] = {')
		elif line.startswith('};'):
			break
		else:
			start = line.find('"')
			if start < 0:
				continue
			start += 1
			end = line.index('"', start)
			tag = line[start:end]
			if not tag:
				continue

			start = line.index(',') + 1
			end = line.index(',', start)
			page = line[start:end].strip()

			lineno = index + 1
			if tag[-1] != ',':
				print('missing trailing comma at line:', lineno, page, tag)
			s = ''.join(tag.split())
			if s != tag:
				print('space in encoding at line:', lineno, page, tag)

			items = tag[:-1].split(',')
			name = items[0].lower()
			items = items[1:]
			s = ''.join(items)
			if any(not is_tag_char(ch) for ch in s):
				print('tag not normalized at line:', lineno, page, tag)

			if name not in name_map:
				name_map[name] = [lineno]
			else:
				name_map[name].append(lineno)

			for item in items:
				if item not in tag_map:
					tag_map[item] = [lineno]
				else:
					tag_map[item].append(lineno)

			if page not in page_map:
				page_map[page] = [lineno]
			else:
				page_map[page].append(lineno)

	for name, lines in name_map.items():
		if len(lines) > 1:
			print('same encoding name:', name, lines)
	for tag, lines in tag_map.items():
		if len(lines) > 1:
			print('same encoding tag:', tag, lines)
	for page, lines in page_map.items():
		if len(lines) > 1:
			print('same code page:', page, lines)

def diff_iso_encoding(path):
	# https://encoding.spec.whatwg.org/#names-and-labels
	encodingList = [
		('cp874', 'iso-8859-11'),
		('cp1252', 'iso-8859-1'),
		('cp1254', 'iso-8859-9'),
	]

	diffList = {}
	for ansi, encoding in encodingList:
		diff = []
		for code in range(256):
			src = bytes([code])
			try:
				ch1 = src.decode(ansi)
			except UnicodeDecodeError:
				ch1 = ''
			try:
				ch2 = src.decode(encoding)
			except UnicodeDecodeError:
				ch2 = ''
			if ch1 != ch2:
				diff.append((code, ch1, ch2))
		diffList[(ansi, encoding)] = diff

	with open(path, 'w', encoding='utf-8', newline='\n') as fd:
		for key, diff in diffList.items():
			fd.write(f'{key[0]} {key[1]}\n')
			for code, ch1, ch2 in diff:
				cat1 = unicodedata.category(ch1) if ch1 else ''
				cat2 = unicodedata.category(ch2) if ch2 else ''
				fd.write(f'\t{code:02X} {ch1}/{cat1}\t\t\t{ch2}/{cat2}\n')

def dump_all_css_properties(path, keyName, keyModule, keyUrl):
	with open(path, encoding='utf-8') as fd:
		properties = json.load(fd)
	properties.sort(key=lambda m: (m[keyName], m[keyModule]))
	groups = {}
	for item in properties:
		status = item['status']
		if status in groups:
			groups[status]['count'] += 1
		else:
			groups[status] = {
				'count': 1,
				'module': {}
			}

		title = item[keyModule]
		module = groups[status]['module']
		if title in module:
			module[title].append(item)
		else:
			module[title] = [item]

	status_name = {
		'ED': "Editor's Draft",
		'FPWD': 'First Public Working Draft',
		'WD': 'Working Draft',
		'LC': 'Last Call Working Draft',
		'CR': 'Candidate Recommendation',
		'PR ': 'Proposed Recommendation',
		'CRD': 'Candidate Recommendation Draft',
		'REC': 'Recommendation',
		'NOTE': 'Working Group Note',
	}

	output = []
	stat = {}
	for status, group in groups.items():
		stat[status] = group['count']
		output.append(f'{status} {{ /* {status_name[status]} */')
		for title, module in group['module'].items():
			output.append(f'/* {title} */')
			for item in module:
				output.append(f'{item[keyName]}:; /* {item[keyUrl]} */')
			output.append('')
		output.append('}')
		output.append('')

	path = os.path.basename(path)
	print(path, 'status:', stat)
	path = os.path.splitext(path)[0] + '.css'
	with open(path, 'w', encoding='utf-8', newline='\n') as fd:
		fd.write('\n'.join(output))

def group_powershell_commands(path):
	# group Get-Command output by module
	commands = {}
	with open(path, encoding='utf-8') as fd:
		for line in fd.read().splitlines():
			items = line.split()
			if len(items) == 4:
				category, name, _, module = items
				item = category, name
				if module in commands:
					commands[module].append(item)
				else:
					commands[module] = [item]

	path = os.path.splitext(path)[0] + '-sort.ps1'
	with open(path, 'w', encoding='utf-8', newline='\n') as fd:
		for module, items in sorted(commands.items()):
			fd.write(f'{{ # {module}\n')
			for category, name in sorted(items):
				fd.write(f'\t{category}\t\t{name}\n')
			fd.write('}\n')

#increase_style_resource_id_value('../src/EditLexers/EditStyle.h')
check_encoding_list('../src/EditEncoding.c')
#diff_iso_encoding('iso-8859.log')

# https://www.w3.org/Style/CSS/all-properties.en.json
#dump_all_css_properties('all-properties.en.json', 'property', 'title', 'url')
# https://www.w3.org/Style/CSS/all-descriptors.en.json
#dump_all_css_properties('all-descriptors.en.json', 'descriptor', 'specification', 'URL')
