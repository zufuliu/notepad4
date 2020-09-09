#!/usr/bin/env python3
import sys
import os.path
import re

def increase_style_resource_id_value(path, delta=100):
	doc = open(path, encoding='utf-8', newline='\n').read()
	updated = re.sub(r'\d{5}', lambda m: str(int(m.group(0)) + delta), doc)
	print('update:', path)
	with open(path, 'w', encoding='utf-8', newline='\n') as fp:
		fp.write(updated)

def check_encoding_list(path):
	def is_tag_char(ch):
		return (ch >= 'a' and ch <= 'z') or (ch >= '0' and ch <= '9')

	doc = open(path, encoding='utf-8', newline='\n').read()
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

#increase_style_resource_id_value('../src/EditLexers/EditStyle.h')
check_encoding_list('../src/EditEncoding.c')
