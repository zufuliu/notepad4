import sys
import os.path
import re
import codecs
import ctypes

# count color on exported scheme file
kReColorHex = re.compile(r'(fore|back)?\s*:?\s*(#[0-9A-Fa-f]{6})')
CP_ACP = f'cp{ctypes.windll.kernel32.GetACP()}'

def read_file(path):
	with open(path, 'rb') as fd:
		doc = fd.read()
	if doc.startswith(codecs.BOM_UTF8):
		return doc[len(codecs.BOM_UTF8):].decode('utf-8')
	if doc.startswith(codecs.BOM_UTF16_LE):
		return doc[len(codecs.BOM_UTF16_LE):].decode('utf_16_le')
	try:
		return doc.decode('utf-8')
	except UnicodeDecodeError:
		return doc.decode(CP_ACP)

def parse_key_value(line):
	items = line.split('=', 2)
	if not items or len(items) != 2:
		return None

	items[0] = items[0].strip()
	items[1] = items[1].strip()
	if not items[0] or not items[1]:
		return None

	return items

def find_color_in_file(path, color_map):
	lines = read_file(path).splitlines()
	scheme = ''
	for line in lines:
		line = line.strip()
		if not line or line[0] in ';#':
			continue
		if line[0] == '[':
			scheme = line[1:-1]
			continue
		items = parse_key_value(line)
		if not items:
			continue

		colors = kReColorHex.findall(items[1])
		if not colors:
			continue

		key = items[0]
		for color in colors:
			fore = color[0]
			color = color[1].upper()
			if color in color_map:
				color_stat = color_map[color]
				color_stat['total_count'] += 1
				color_stat[fore] += 1
				if key not in color_stat['usage']:
					color_stat['usage'][key] = [scheme]
				else:
					color_stat['usage'][key].append(scheme)
			else:
				color_stat = {
					'total_count': 1,
					'fore': 0,
					'back': 0,
					'usage': {
						key: [scheme],
					},
				}
				color_stat[fore] = 1
				color_map[color] = color_stat

def print_color_count(color_map):
	for color, color_stat in color_map.items():
		lines = []
		if fore := color_stat['fore']:
			lines.append(f'fore: {fore}')
		if back := color_stat['back']:
			lines.append(f'back: {back}')
		print(f"{color}\t{color_stat['total_count']} {' '.join(lines)}")
		usage = color_stat['usage']
		for key, items in usage.items():
			count = len(items)
			scheme = ', '.join(sorted(set(items)))
			print(f'\t{count}\t{key}\t[{scheme}]')

def count_color(path):
	# { color : { total_count: 0, fore: 0, back: 0, usage: { key: [scheme]}}}
	color_map = {}
	find_color_in_file(path, color_map)

	colors = sorted(color_map.items(), key=lambda m: m[0])
	colors = sorted(colors, key=lambda m: m[1]['total_count'], reverse=True)
	color_map = dict(colors)
	for color_stat in color_map.values():
		usage = color_stat['usage']
		usage = sorted(usage.items(), key=lambda m: m[0])
		usage = sorted(usage, key=lambda m: len(m[1]), reverse=True)
		color_stat['usage'] = dict(usage)

	print_color_count(color_map)

if __name__ == '__main__':
	if len(sys.argv) > 1 and os.path.isfile(sys.argv[1]):
		count_color(sys.argv[1])
	else:
		print(f"""Usage: {sys.argv[0]} path""")
