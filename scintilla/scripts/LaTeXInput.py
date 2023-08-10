import sys
import os.path
import re
import string
import json
from statistics import variance
import time

from FileGenerator import Regenerate, MakeKeywordLines
import UnicodeData

header_path = '../include/LaTeXInput.h'
data_path = '../win32/LaTeXInputData.h'

source_info = {
	# data source at https://github.com/JuliaLang/julia/tree/master/stdlib/REPL/src
	'latex_link': 'https://docs.julialang.org/en/v1.10-dev/manual/unicode-input/',
	'emoji_link': 'https://github.com/iamcal/emoji-data/blob/master/emoji_pretty.json',
}

# strip out unused bytes (sequence[0] and the separator ' ') from input sequences,
# sequence[0] is already stored in `magic` field.
BuildDataForLookupOnly = False

def escape_c_char(ch):
	if ch in r'\'"':
		return '\\' + ch
	if ch < ' ' or ord(ch) >= 127:
		return f'\\x{ord(ch):02x}'
	return ch

def quote_c_char(ch):
	return "'" + escape_c_char(ch) + "'"

def make_char_equals(charset):
	lines = []
	for index in range(0, len(charset), 5):
		line = ' || '.join('ch == ' + quote_c_char(ch) for ch in charset[index:index+5])
		lines.append('\t\t|| ' + line)
	return '\n'.join(lines)

def find_word_contains_punctuation(items):
	punctuation = set(string.punctuation)
	punctuation.remove('_')
	result = []
	for item in items:
		if any(ch in punctuation for ch in item):
			result.append(item)
	result.sort()
	return result

def json_dump(obj):
	return json.dumps(obj, ensure_ascii=False, indent='\t')

def json_load(path):
	with open(path, encoding='utf-8', newline='\n') as fd:
		return json.loads(fd.read())

def djb2_hash(buf, multiplier):
	value = 0
	for ch in buf:
		value = value * multiplier + ch
	# masked to match C/C++ unsigned integer overflow wrap around
	return value & (2**32 - 1)

def fast_counter(items):
	# faster than defaultdict and Counter
	counter = {}
	for item in items:
		if item in counter:
			counter[item] += 1
		else:
			counter[item] = 1
	return counter


def prepare_input_data_hash(input_map, path):
	if not input_map:
		input_map = json_load(path)
	for key, info in input_map.items():
		buf = key.encode('utf-8')
		info['hash_key'] = buf
		info['magic'] = len(buf) | (buf[0] << 8)
	return input_map

def dump_hash_param(hash_param, multiplier_list, path):
	with open(path, 'w', encoding='utf-8', newline='\n') as fd:
		for multiplier, items in hash_param.items():
			if multiplier in multiplier_list:
				output = [str(multiplier) + '\n']
				items.sort()
				previous = items[0][0]
				for item in items:
					current = item[0]
					if current != previous:
						previous = current
						output.append('\n')
					output.append('\t' + str(item) + '\n')
				fd.write(''.join(output))

def find_hash_param(input_map, multiplier_list, max_hash_size):
	key_list = [info['hash_key'] for info in input_map.values()]
	magic_list = [info['magic'] for info in input_map.values()]

	hash_size = len(input_map) // 15
	hash_param = {}
	for multiplier in multiplier_list:
		raw_hash = [djb2_hash(key, multiplier) for key in key_list]
		for size in range(hash_size, max_hash_size + 1):
			hash_list = [hash_key % size for hash_key in raw_hash]
			distribution = [0] * size
			for hash_key in hash_list:
				distribution[hash_key] += 1

			collision = max(distribution)
			if collision < 16:
				hash_map = {}
				for index, magic in enumerate(magic_list):
					hash_key = (hash_list[index] << 16) | magic
					if hash_key in hash_map:
						hash_map[hash_key] += 1
					else:
						hash_map[hash_key] = 1

				comparison = sorted(hash_map.values(), reverse=True)
				max_comparison = comparison[0]
				if max_comparison < 4:
					extra_comparison = 0
					for value in comparison:
						if value == 1:
							break
						extra_comparison += value - 1
					used = sum(item != 0 for item in distribution)
					var = round(variance(distribution), 2)
					item = (max_comparison, size, used, collision, extra_comparison, var)
					if multiplier in hash_param:
						hash_param[multiplier].append(item)
					else:
						hash_param[multiplier] = [item]
	return hash_param

def update_latex_input_data_hash(input_name, input_map, multiplier, hash_size):
	hash_map = {}
	for info in input_map.values():
		hash_key = djb2_hash(info['hash_key'], multiplier) % hash_size
		if hash_key in hash_map:
			hash_map[hash_key].append(info)
		else:
			hash_map[hash_key] = [info]

	hash_table = [0] * hash_size
	input_list = []
	content = []
	string_offset = 0
	max_collision = 0
	max_comparison = 0
	for hash_key in range(hash_size):
		if hash_key in hash_map:
			items = hash_map[hash_key]
			items.sort(key=lambda m: (m['magic'], m['hash_key']))
			collision = len(items)
			if collision > max_collision:
				max_collision = collision
			counter = fast_counter(info['magic'] for info in items)
			comparison = max(counter.values())
			if comparison > max_comparison:
				max_comparison = comparison

			if BuildDataForLookupOnly:
				key_list = [info['sequence'][1:] for info in items]
				for index, sequence in enumerate(key_list):
					items[index]['offset'] = string_offset
					string_offset += len(sequence)
				content.append('"' + ''.join(key_list) + '"')
			else:
				key_list = [info['sequence'] for info in items]
				for index, sequence in enumerate(key_list):
					items[index]['offset'] = string_offset + 1
					string_offset += len(sequence) + 1
				content.append('"' + ' '.join(key_list) + ' "')

			value = (len(input_list) << 4) | len(items)
			assert len(items) < 16, (input_name, hash_key, len(items))
			assert value <= 0xffff, (input_name, hash_key, len(items))
			hash_table[hash_key] = value
			input_list.extend(items)

	output = []
	output.append(f'static constexpr uint32_t {input_name}HashMultiplier = {multiplier};')
	output.append('')
	output.append(f'static const uint16_t {input_name}HashTable[] = {{')
	output.extend(f'0x{value:04x},' for value in hash_table)
	output.append('};')
	Regenerate(data_path, f'//{input_name} hash', output)

	prefix = '\\'
	suffix = ''
	if input_name == 'Emoji':
		prefix = '\\:'
		suffix = ':'
	# see https://www.unicode.org/faq/utf_bom.html
	LEAD_OFFSET = 0xD800 - (0x10000 >> 10)
	output= []
	for info in input_list:
		character = info['character']
		if len(character) == 1:
			ch = ord(character)
			if ch <= 0xffff:
				code = f'0x{ch:04X}'
			else:
				character = f'U+{ch:X}, {character}'
				# convert to UTF-16
				lead = LEAD_OFFSET + (ch >> 10)
				trail = 0xDC00 + (ch & 0x3FF)
				code = f"0x{trail:04X}'{lead:04X}"
		else:
			code = f"0x{ord(character[1]):04X}'{ord(character[0]):04X}"
		magic = info['magic']
		offset = info['offset']
		sequence = prefix + info['sequence'] + suffix
		name = info['name']
		line = f'{{0x{magic:04x}, 0x{offset:04x}, {code}}}, // {character}, {sequence}, {name}'
		output.append(line)
	Regenerate(data_path, f'//{input_name} list', output)

	content[-1] += ';'
	Regenerate(data_path, f'//{input_name} string', content)

	string_size, hash_size, list_size = string_offset + 1, hash_size*2, len(input_map)*8
	size = string_size + hash_size + list_size
	print(input_name, 'count:', len(input_map), 'content:', string_size,
		'map:', (hash_size, list_size, max_collision, max_comparison), 'total:', (size, size/1024))

def update_all_latex_input_data_hash(latex_map=None, emoji_map=None):
	latex_map = prepare_input_data_hash(latex_map, 'latex_map.json')
	emoji_map = prepare_input_data_hash(emoji_map, 'emoji_map.json')

	if True:
		multiplier_list = [33]
		if False:
			for value in range(1, 8):
				multiplier_list.append((1 << value) - 1)
				multiplier_list.append((1 << value))
				multiplier_list.append((1 << value) + 1)
			multiplier_list = sorted(set(multiplier_list))

		start_time = time.perf_counter_ns()
		latex_hash = find_hash_param(latex_map, multiplier_list, 512)
		emoji_hash = find_hash_param(emoji_map, multiplier_list, 256)
		end_time = time.perf_counter_ns()
		duration = (end_time - start_time)/1e6
		print('hash param time:', duration)

		multiplier_list = sorted(set(latex_hash.keys()) & set(emoji_hash.keys()))
		print('hash multiplier:', multiplier_list)
		dump_hash_param(latex_hash, multiplier_list, 'latex_hash.log')
		dump_hash_param(emoji_hash, multiplier_list, 'emoji_hash.log')
		assert any(item[1] == 290 for item in latex_hash[33])
		assert any(item[1] == 160 for item in emoji_hash[33])

	update_latex_input_data_hash('LaTeX', latex_map, 33, 290)
	update_latex_input_data_hash('Emoji', emoji_map, 33, 160)


def update_latex_input_data_linear(input_name, input_map, path, param):
	if not input_map:
		input_map = json_load(path)
	input_list = sorted(input_map.items())
	key_list = [item[0] for item in input_list]

	singleChar = [item[1]['character'] for item in input_list if len(item[1]['character']) == 1]
	doubleChar = sorted(item[1]['character'] for item in input_list if len(item[1]['character']) > 1)
	minChar = min(singleChar)
	maxChar = max(singleChar)
	charBit = ord(maxChar).bit_length()

	minKeyLen = min(len(key) for key in key_list)
	maxKeyLen = max(len(key) for key in key_list)
	keyLenBit = maxKeyLen.bit_length()
	if 'keyLenBit' in param:
		keyLenBit = max(keyLenBit, param['keyLenBit'])
	hashBit = 32 - charBit
	hashMask = (1 << hashBit) - 1

	firstList = set(key[-1] for key in key_list if len(key) > 1)
	if 'minFirst' in param:
		minFirst = param['minFirst']
	else:
		minFirst = min(firstList)
	maxFirst = max(firstList)
	firstBit = (ord(maxFirst) - ord(minFirst)).bit_length() - 1
	firstMask = (1 << firstBit) - 1

	secondList = set(key[-2] for key in key_list if len(key) > 2)
	if 'minSecond' in param:
		minSecond = param['minSecond']
	else:
		minSecond = min(secondList)
	maxSecond = max(secondList)
	secondBit = hashBit - keyLenBit - firstBit
	secondMask = (1 << secondBit) - 1

	prefix = '\\'
	suffix = ''
	if input_name == 'Emoji':
		prefix = '\\:'
		suffix = ':'
	group = {}
	hashStat = {}
	start = len(doubleChar)
	doubleList = ['']*start
	totalKeyLen = 0
	output = []
	for key, info in input_list:
		initial = ord(key[0])
		if initial in group:
			group[initial]['count'] += 1
		else:
			group[initial] = {'start': start, 'count': 1, 'offset': totalKeyLen}

		start += 1
		keyLen = len(key)
		totalKeyLen += keyLen + 1

		magic = ((ord(key[-1]) - ord(minFirst)) & firstMask) << keyLenBit
		if keyLen > 2:
			second = (ord(key[-2]) - ord(minSecond)) & secondMask
			magic = magic | (second << (keyLenBit + firstBit))

		hash_key = magic | (keyLen + 1)
		character = info['character']
		sequence = prefix + info['sequence'] + suffix
		name = info['name']
		if len(character) > 1:
			code = doubleChar.index(character)
			info['index'] = code
			line = f"0x{ord(character[1]):04X}'{ord(character[0]):04X}, // {character}, {sequence}, {name}"
			doubleList[code] = line
		else:
			code = ord(character)
		magic = (code << hashBit) | hash_key
		line = f'0x{magic:08x}, // U+{code:05X} {character}, {sequence}, {name}'
		output.append(line)

		hash_key = (initial, hash_key)
		if hash_key in hashStat:
			hashStat[hash_key] += 1
		else:
			hashStat[hash_key] = 1

	minInitial = min(group.keys())
	maxInitial = max(group.keys())
	groupCount = maxInitial - minInitial + 1
	maxInitialCount = max(item['count'] for item in group.values())
	collision = max(hashStat.values()), round(len(input_map)/len(hashStat), 4)
	totalSize = (totalKeyLen + 2*groupCount + 4*(groupCount + len(input_map) + len(doubleChar)))/1024
	print(f'''{input_name} {len(input_map)} {hex(ord(minChar)), hex(ord(maxChar))}, bit: {charBit}, double: {len(doubleChar)}, total: {totalSize}
    group table: {chr(minInitial), chr(maxInitial)}, size: {groupCount}, max: {maxInitialCount}
     key length: {minKeyLen, maxKeyLen}, bit: {maxKeyLen.bit_length()} / {keyLenBit}, total: {totalKeyLen/1024}
       hash bit: {hashBit, hashBit - keyLenBit, firstBit, secondBit}, collision: {collision}
     hash first: {min(firstList), maxFirst}: {minFirst}, bit: {firstBit}
    hash second: {min(secondList), maxSecond}: {minSecond}, bit: {secondBit}''')

	output = doubleList + output
	Regenerate(data_path, f'//{input_name} list', output)

	offsetList = [0]*groupCount
	groupList = [0]*groupCount
	for key, item in group.items():
		key -= minInitial
		start = item['start']
		offsetList[key] = item['offset']
		groupList[key] = start | ((start + item['count']) << 16)

	output = []
	output.extend(f"""#define {input_name}MinInitialCharacter\t{quote_c_char(chr(minInitial))}
#define {input_name}MaxInitialCharacter\t{quote_c_char(chr(maxInitial))}
""".splitlines())
	if input_name == 'LaTeX':
		output.extend(f"""#define {input_name}DoubleCharacterCount\t{len(doubleChar)}
#define {input_name}HashBit\t\t{hashBit}
#define {input_name}HashMask\t\t{hex(hashMask)}
#define {input_name}LengthBit\t\t{keyLenBit}
#define {input_name}LengthMask\t\t{(1 << keyLenBit) - 1}
#define {input_name}FirstSubCharacter\t{quote_c_char(minFirst)}
#define {input_name}FirstSubBit\t\t{firstBit}
#define {input_name}FirstSubMask\t\t{firstMask}
#define {input_name}SecondSubCharacter\t{quote_c_char(minSecond)}
#define {input_name}SecondSubBit\t\t{secondBit}
#define {input_name}SecondSubMask\t\t{secondMask}
""".splitlines())
	output.append('')
	output.append(f"static const uint32_t {input_name}IndexTable[] = {{")
	for i in range(0, groupCount, 10):
		line = ', '.join(f'0x{value:08x}' for value in groupList[i:i+10])
		output.append(line + ',')
	output.append(f"}};")
	output.append('')
	output.append(f"static const uint16_t {input_name}OffsetTable[] = {{")
	for i in range(0, groupCount, 10):
		line = ', '.join(f'0x{value:04x}' for value in offsetList[i:i+10])
		output.append(line + ',')
	output.append(f"}};")
	Regenerate(data_path, f'//{input_name} hash', output)

	lines = MakeKeywordLines(key_list)
	output = [f'"{line} "' for line in lines]
	output[-1] += ';'
	Regenerate(data_path, f'//{input_name} string', output)

def update_all_latex_input_data_linear(latex_map=None, emoji_map=None):
	param = {
		'keyLenBit': 6,
		'minFirst': '!',
		'minSecond': '-',
	}
	update_latex_input_data_linear('LaTeX', latex_map, 'latex_map.json', param)
	update_latex_input_data_linear('Emoji', emoji_map, 'emoji_map.json', param)


def get_input_map_size_info(input_name, input_map):
	items = [len(key) for key in input_map.keys()]
	min_len = min(items)
	max_len = max(items)
	print(input_name, 'count:', len(items), 'content:', sum(items) + len(items), 'length:', (min_len, max_len))
	return min_len, max_len

def build_charset_function(latex_charset, emoji_charset, output):
	letters = set(string.ascii_letters + string.digits)
	punctuation = set(string.punctuation)
	latex_charset -= letters
	emoji_charset -= letters

	diff = latex_charset - punctuation
	if diff:
		diff = sorted(diff)
		print('Invalid LaTeX character:', diff, [ord(ch) for ch in diff])
	diff = emoji_charset - punctuation
	if diff:
		diff = sorted(diff)
		print('Invalid Emoji character:', diff, [ord(ch) for ch in diff])

	print('LaTeX punctuation:', latex_charset)
	print('LaTeX punctuation:', [ord(ch) for ch in latex_charset])
	print('Emoji punctuation:', emoji_charset)
	print('Emoji punctuation:', [ord(ch) for ch in emoji_charset])

	latex_punctuation = sorted(latex_charset)
	emoji_punctuation = sorted(emoji_charset - latex_charset)
	latex_charset = make_char_equals(latex_punctuation)
	emoji_charset = make_char_equals(emoji_punctuation)
	output.extend(f"""
static inline bool IsLaTeXInputSequenceChar(char ch) {{
	return (ch >= 'a' && ch <= 'z')
		|| (ch >= 'A' && ch <= 'Z')
		|| (ch >= '0' && ch <= '9')
{latex_charset}
#if NP2_ENABLE_LATEX_LIKE_EMOJI_INPUT
{emoji_charset}
#endif
	;
}}
""".splitlines())

def update_latex_input_header(latex_map, emoji_map):
	output = ['// LaTeX input sequences based on ' + source_info['latex_version']]
	output.append('// documented at ' + source_info['latex_link'] + '.')
	output.append('// Emoji input sequences based on ' + source_info['emoji_link'] + ',')
	output.append('// downloaded on ' + source_info['emoji_version'] + '.')
	output.append('')

	output.append('enum {')
	min_latex_len, max_latex_len = get_input_map_size_info('LaTeX', latex_map)
	output.append(f'\tMinLaTeXInputSequenceLength = {min_latex_len},')
	output.append(f'\tMaxLaTeXInputSequenceLength = {max_latex_len},')
	output.append('')

	min_emoji_len, max_emoji_len = get_input_map_size_info('Emoji', emoji_map)
	prefix = ':'
	suffix = ':'
	output.append('#if NP2_ENABLE_LATEX_LIKE_EMOJI_INPUT')
	output.append(f'\tEmojiInputSequencePrefixLength = {len(prefix)},')
	output.append(f'\tEmojiInputSequenceSuffixLength = {len(suffix)},')
	output.append(f'\tMinEmojiInputSequenceLength = {min_emoji_len} + EmojiInputSequencePrefixLength, // suffix is optional')
	output.append(f'\tMaxEmojiInputSequenceLength = {max_emoji_len} + EmojiInputSequencePrefixLength + EmojiInputSequenceSuffixLength,')

	output.append('')
	if max_latex_len >= max_emoji_len + len(prefix) + len(suffix):
		output.append('\tMaxLaTeXInputBufferLength = 1 + MaxLaTeXInputSequenceLength + 1,')
	else:
		output.append('\tMaxLaTeXInputBufferLength = 1 + MaxEmojiInputSequenceLength + 1,')
	output.append('#else')
	output.append('\tMaxLaTeXInputBufferLength = 1 + MaxLaTeXInputSequenceLength + 1,')
	output.append('#endif')
	output.append('};')

	latex_punctuation = find_word_contains_punctuation(latex_map.keys())
	emoji_punctuation = find_word_contains_punctuation(emoji_map.keys())
	print('LaTeX punctuation:', latex_punctuation)
	print('Emoji punctuation:', emoji_punctuation)

	latex_charset = set(''.join(latex_map.keys()))
	emoji_charset = set(''.join(emoji_map.keys()))
	emoji_charset.update(prefix + suffix)
	build_charset_function(latex_charset, emoji_charset, output)
	Regenerate(header_path, '//', output)

def fix_character_and_code(character, code):
	items = re.findall(r'U\+(\w+)', code)
	ch = ''.join(chr(int(item, 16)) for item in items)
	code = ' + '.join('U+' + item for item in items)
	if len(character) != len(ch):
		# add back stripped spaces
		return character in ch, ch, code
	return character == ch, ch, code

def parse_julia_unicode_input_html(path):
	from bs4 import BeautifulSoup

	with open(path, encoding='utf-8', newline='\n') as fd:
		doc = fd.read()
	soup = BeautifulSoup(doc, 'html5lib')
	documenter = soup.body.find(id='documenter')
	page = documenter.find(id='documenter-page')

	link = soup.head.find('link', {'rel': 'canonical'})
	link = link['href']
	print(link)

	settings = documenter.find(id='documenter-settings')
	colophon = settings.find('span', {'class': 'colophon-date'})
	version = colophon.parent.get_text().strip()
	version = version[version.index('Julia version'):]
	version_date = colophon.get_text().strip()
	version = f"{version.rstrip('.')} ({version_date}),"
	print(version)
	source_info['latex_version'] = version

	table = page.find('table').find('tbody')
	high = UnicodeData.MaxCharacter >> 16
	latex_map = {}
	emoji_map = {}
	for row in table.find_all('tr'):
		items = []
		for column in row.find_all('td'):
			items.append(column.get_text().strip())
		if not items: # table header: th
			continue

		row_text = row.get_text().strip()
		assert len(items) == 4, (items, row_text)

		code, character, sequence, name = items
		assert code.startswith('U+'), row_text
		assert sequence.startswith('\\'), row_text

		ok, character, code = fix_character_and_code(character, code)
		assert ok and 1 <= len(character) <= 2, (character, row_text)
		if len(character) == 2:
			assert ord(character[0]) <= 0xffff and high < ord(character[1]) <= 0xffff, (character, row_text)
		else:
			assert ord(character) > 0x80, (character, row_text)

		items = [item.strip() for item in sequence.split(',')]
		for sequence in items:
			assert len(sequence) > 1 and sequence.startswith('\\'), (sequence, row_text)
			sequence = sequence[1:]
			if sequence[0] == ':':
				sequence = sequence[1:-1]
				assert len(sequence) >= 1, (sequence, row_text)
				emoji_map[sequence] = {
					'code': code,
					'character': character,
					'sequence': sequence,
					'name': name
				}
			else:
				latex_map[sequence] = {
					'code': code,
					'character': character,
					'sequence': sequence,
					'name': name
				}

	return latex_map, emoji_map

def parse_iamcal_emoji_data_json(path):
	modification = time.gmtime(os.path.getmtime(path))
	emoji_version = time.strftime('%A %d %B %Y', modification)
	print(emoji_version, modification)
	source_info['emoji_version'] = emoji_version

	input_list = json_load(path)
	emoji_map = {}
	non_qualified = {}
	input_map = {}
	for info in input_list:
		code = info['unified']
		unified = True
		if '-' in code:
			code = info['non_qualified']
			if not code or '-' in code:
				continue
			unified = False

		ch = int(code, 16)
		assert ch > 0x80, info
		character = chr(ch)
		code = 'U+' + code
		name = info['name'].title().strip()
		if not name:
			name = UnicodeData.getCharacterName(character)
		short_name = info['short_name']
		short_names = info['short_names']
		assert short_name in short_names
		for sequence in short_names:
			if sequence in input_map:
				assert character == input_map[sequence], (sequence, info)
			else:
				input_map[sequence] = character
			if unified:
				emoji_map[sequence] = {
					'code': code,
					'character': character,
					'sequence': sequence,
					'name': name
				}
			else:
				non_qualified[sequence] = {
					'code': code,
					'character': character,
					'sequence': sequence,
					'name': name
				}

	return emoji_map, non_qualified

def parse_all_data_source():
	latex_map, emoji_map = parse_julia_unicode_input_html('Unicode Input.html')
	with open('latex_map.json', 'w', encoding='utf-8', newline='\n') as fd:
		fd.write(json_dump(latex_map))

	unified, non_qualified = parse_iamcal_emoji_data_json('emoji_pretty.json')
	diff = set(emoji_map.keys()) - set(unified.keys()) - set(non_qualified.keys())
	if diff:
		print('missing emoji:', sorted(diff))
	emoji_map.update(non_qualified)
	emoji_map.update(unified)
	with open('emoji_map.json', 'w', encoding='utf-8', newline='\n') as fd:
		fd.write(json_dump(emoji_map))

	update_latex_input_header(latex_map, emoji_map)

#parse_all_data_source()
#update_all_latex_input_data_hash()
update_all_latex_input_data_linear()
