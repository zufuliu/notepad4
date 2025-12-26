import unicodedata

def check_encoding_list(path):
	def is_tag_char(ch):
		return ('a' <= ch <= 'z') or ('0' <= ch <= '9')

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

#check_encoding_list('../src/EditEncoding.cpp')
#diff_iso_encoding('iso-8859.log')
