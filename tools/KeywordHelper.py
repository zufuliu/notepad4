import os.path
import re
import json
import subprocess
import ctypes

CP_ACP = 'cp' + str(ctypes.windll.kernel32.GetACP())

def decode_stdout(doc):
	if not doc:
		return ''
	try:
		return doc.decode('utf-8')
	except UnicodeDecodeError:
		return doc.decode(CP_ACP)

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

def find_new_css_properties(properties, descriptors, lang, draft=True):
	with open(properties, encoding='utf-8') as fd:
		properties = json.load(fd)
	properties = [item['property'] for item in properties if draft or item['status'] != 'ED']
	with open(descriptors, encoding='utf-8') as fd:
		descriptors = json.load(fd)
		properties += [item['descriptor'] for item in descriptors if draft or item['status'] != 'ED']
	properties = set(properties)
	with open(lang, encoding='utf-8') as fd:
		doc = fd.read()
	existing = set(re.findall(r'$\s*([\w\-]+):', doc, re.MULTILINE))
	existing -= set('''grid-column-gap grid-gap grid-row-gap
	max-aspect-ratio max-device-aspect-ratio max-device-height max-device-width max-resolution
	min-aspect-ratio min-device-aspect-ratio min-device-height min-device-width min-resolution
	'''.split())
	print('new css properties:', ', '.join(sorted(properties - existing)))
	print('unknown css properties:', ', '.join(sorted(existing - properties)))

def find_updated_css_drafts(since, path):
	os.chdir(path)
	# show commit date and dir stat
	with subprocess.Popen(['git.exe', 'log', f'--since={since}', '--format=%cs', '--dirstat=files,0,cumulative'], stdout=subprocess.PIPE) as proc:
		doc = proc.stdout.read()
		lines = decode_stdout(doc).splitlines()
		drafts = {}
		date = ''
		length = 0
		for line in lines:
			if m := re.match(r'(\d{4}-\d{2}-\d{2})', line):
				date = m.groups()[0]
			elif m := re.match(r'\s+[\d\.]+%\s+([\w\-]+)/', line):
				module = m.groups()[0]
				if module not in drafts or drafts[module] < date:
					drafts[module] = date
					length = max(length, len(module))
		output = []
		for module, date in sorted(drafts.items()):
			output.append(f'{' '*4}https://drafts.csswg.org/{module}/{' '*(4 + length - len(module))}{date}')
		print(f'{len(drafts)} updated CSS drafts since {since}:')
		print('\n'.join(output))

def dump_html_elements(path):
	from bs4 import BeautifulSoup

	with open(path, encoding='utf-8', newline='\n') as fd:
		doc = fd.read()
	soup = BeautifulSoup(doc, 'html5lib')

	elements = []
	table = soup.body.find('table')
	assert table.find('caption').get_text() == 'List of elements'
	tbody = table.find('tbody')
	for row in tbody.find_all('tr'):
		name = row.find('th').get_text().strip()
		if not name.isalnum():
			print('ignore elements:', name)
		else:
			elements.append(name)

	attributes = []
	table = soup.body.find('table', {'id': 'attributes-1'})
	tbody = table.find('tbody')
	for row in tbody.find_all('tr'):
		name = row.find('th').get_text().strip()
		attributes.append(name)

	handlers = []
	table = soup.body.find('table', {'id': 'ix-event-handlers'})
	tbody = table.find('tbody')
	for row in tbody.find_all('tr'):
		name = row.find('th').get_text().strip()
		handlers.append(name)

	elements = sorted(set(elements))
	attributes = sorted(set(attributes))
	handlers = sorted(set(handlers))
	print(f'total elements {len(elements)}, attributes: {len(attributes)}, event handlers: {len(handlers)}')
	with open('html-dump.html', 'w', encoding='utf-8', newline='\n') as fd:
		fd.write('\n'.join(f'<{name}/>' for name in elements))
		fd.write('\n<a\n')
		fd.write('\n'.join(attributes))
		fd.write('\n/>\n')
		fd.write('\n<a\n')
		fd.write('\n'.join(handlers))
		fd.write('\n/>\n')

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

def find_new_texinfo_commands(path, lang):
	with open(lang, encoding='utf-8') as fd:
		doc = fd.read()
	existing = set(re.findall(r'@(\w+)', doc))
	with open(path, encoding='utf-8') as fd:
		doc = fd.read()

	doc = re.sub(r'@c\W.+', '', doc)
	doc = re.sub(r'@comment\W.+', '', doc)
	macros = re.findall(r'@r?macro\s+(\w+)', doc)
	macros.extend(re.findall(r'@alias\s+(\w+)', doc))
	# find inside "Command List" section
	commands = re.findall(r'@itemx?\s+@@(\w+)', doc)
	commands += ['c', 'comment', 'rmacro']
	commands = set(commands) - set(macros)
	with open('texinfo-new.texi', 'w', encoding='utf-8') as fd:
		doc = '\n@'.join(sorted(commands - existing))
		fd.write('new commands:\n')
		fd.write(f'@{doc}\n')
		fd.write('\nunknown commands:\n')
		doc = '\n@'.join(sorted(existing - commands))
		fd.write(f'@{doc}\n')

# https://github.com/w3c/csswg-drafts/
#find_updated_css_drafts('2025-09-12', r'H:\Libs\csswg-drafts')
# https://www.w3.org/Style/CSS/all-properties.en.json
#dump_all_css_properties('all-properties.en.json', 'property', 'title', 'url')
# https://www.w3.org/Style/CSS/all-descriptors.en.json
#dump_all_css_properties('all-descriptors.en.json', 'descriptor', 'specification', 'URL')
#find_new_css_properties('all-properties.en.json', 'all-descriptors.en.json', 'lang/CSS.css')

# https://html.spec.whatwg.org/multipage/indices.html
#dump_html_elements('indices.html')

#group_powershell_commands('command.ps1')

#find_new_texinfo_commands(r'texinfo.texi', 'lang/Texinfo.texi')
