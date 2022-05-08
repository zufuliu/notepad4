# return true if equal
SwitchType_Equal = 0
# do something if equal
SwitchType_IfMatch = 1
# return literal string if equal
SwitchType_IfCached = 2
# classify string in group list, return int or enum
SwitchType_Classify = 3

# when length bigger than this value, string is compared by using memcmp() or strncmp()
MaxSmallStringLength = 4

SwitchOption_HeadAndLength = 0
SwitchOption_OnlyHead = 1
SwitchOption_OnlyLength = 2
SwitchOption_HashAndLength = 3
SwitchOption_OnlyHash = 4

IndentChar = '\t'
#IndentChar = ' ' * 4
IndentLevel1 = IndentChar
IndentLevel2 = IndentChar * 2
IndentLevel3 = IndentChar * 3

# encoding for char* in C/C++
CStringEncoding = 'utf-8'

# https://en.wikipedia.org/wiki/Escape_sequences_in_C
_EscapeCharMap = {
	0x00: r'\0',
	0x07: r'\a',
	0x08: r'\b',
	0x0C: r'\f',
	0x0A: r'\n',
	0x0D: r'\r',
	0x09: r'\t',
	0x0B: r'\v',
	0x5C: r'\\',
	0x27: r'\'',
	0x22: r'\"',
}

def cstr_escape(buf):
	items = []
	for ch in buf:
		if ch in _EscapeCharMap:
			items.append(_EscapeCharMap[ch])
		elif ch < 32 or ch > 127:
			items.append(f'\\x{ch:02x}')
		else:
			items.append(chr(ch))
	return ''.join(items)

#define make_switch_key(length, ch)	(((length) << 8) | (ch))
def make_switch_key(length, ch, switch_option):
	if switch_option == SwitchOption_HeadAndLength:
		return f"make_switch_key({length}, '{cstr_escape(ch)}')"
	if switch_option == SwitchOption_OnlyHead:
		return f"'{cstr_escape(ch)}'"
	if switch_option == SwitchOption_OnlyLength:
		return str(length)
	return None

def small_string_hash(buf, hash_size, switch_option):
	value = 0
	mask = (1 << hash_size) - 1
	for ch in buf:
		value = value * 3 + ch
		value = value & mask
	if switch_option == SwitchOption_HashAndLength:
		value |= (len(buf) << hash_size)
	return f'0x{value:X}U'

def _make_small_string_cmp(var_name, buf, length, index):
	expr_list = []
	while index < length:
		expr = f"{var_name}[{index}] == '{cstr_escape(buf[index:index+1])}'"
		expr_list.append(expr)
		index += 1
	if len(expr_list) > 1:
		return '(' + ' && '.join(expr_list) + ')'
	return expr_list[0]

def _get_switch_func_header(switch_type, func_name, var_name, option_set):
	ret = ''
	if switch_type == SwitchType_Equal:
		ret = 'bool'
	elif switch_type == SwitchType_IfMatch:
		ret = 'void'
	elif switch_type == SwitchType_IfCached:
		ret = 'const char*'
	elif switch_type == SwitchType_Classify:
		ret = option_set['return_type']
	return f'{ret} {func_name}(const char *{var_name}, int length) {{\n'

def _get_switch_func_tail(switch_type, option_set):
	ret = ''
	if switch_type == SwitchType_Equal:
		ret = 'false'
	elif switch_type == SwitchType_IfCached:
		ret = 'NULL'
	elif switch_type == SwitchType_Classify:
		ret = option_set['default']
	if ret:
		ret = IndentLevel1 + f'return {ret};\n'
	return ret

def _get_switch_default_return(switch_type, option_set):
	ret = _get_switch_func_tail(switch_type, option_set)
	if not ret:
		ret = IndentLevel1 + 'return;\n'
	return ret

def build_switch_stmt(switch_type, func_name, var_name, word_list, int_arg=0, switch_option=0):
	option_set = None
	sorted_list = []
	if switch_type == SwitchType_Classify:
		option_set = word_list['option']
		groups = word_list['groups']
		for key, items in groups.items():
			for item in items:
				buf = item.encode(CStringEncoding)
				sorted_list.append((len(buf), buf, key))
	else:
		for item in word_list:
			buf = item.encode(CStringEncoding)
			sorted_list.append((len(buf), buf))

	cond_map = {}
	sorted_list.sort(key=lambda m: m[0]) # sorted by string length
	for item in sorted_list:
		length = item[0]
		buf = item[1]
		if switch_option in (SwitchOption_HashAndLength, SwitchOption_OnlyHash):
			key = small_string_hash(buf, int_arg, switch_option)
		else:
			key = make_switch_key(length, buf[int_arg:int_arg+1], switch_option)
		if key in cond_map:
			cond_map[key].append(item)
		else:
			cond_map[key] = [item]

	stmt_list = []
	stmt = _get_switch_func_header(switch_type, func_name, var_name, option_set)
	stmt_list.append(stmt)

	if switch_option != SwitchOption_OnlyLength:
		stmt = IndentLevel1 + _get_switch_default_return(switch_type, option_set)
		stmt = f"if (length < {sorted_list[0][0]} || length > {sorted_list[-1][0]}) {{\n{stmt}{IndentLevel1}}}\n"
		stmt_list.append(IndentLevel1 + stmt)

	if switch_option == SwitchOption_HeadAndLength:
		stmt = f'switch (make_switch_key(length, {var_name}[{int_arg}])) {{\n'
	elif switch_option == SwitchOption_OnlyHead:
		stmt = f'switch ({var_name}[{int_arg}]) {{\n'
	elif switch_option == SwitchOption_OnlyLength:
		stmt = 'switch (length) {\n'
	elif switch_option in (SwitchOption_HashAndLength, SwitchOption_OnlyHash):
		stmt = f'switch (small_string_hash({var_name}, length)) {{\n'
		int_arg = 0
	stmt_list.append(IndentLevel1 + stmt)

	for key, items in cond_map.items():
		stmt = f'{IndentLevel1}case {key}:'
		stmt_list.append(stmt)
		expr_list = []
		items.sort(key=lambda m: m[1]) # sorted by string value
		for item in items:
			length = item[0]
			buf = item[1]
			expr = None
			if switch_option not in (SwitchOption_OnlyHead, SwitchOption_OnlyHash):
				if int_arg or length > MaxSmallStringLength:
					expr = f'memcmp({var_name}, "{cstr_escape(buf)}", {length}) == 0'
				elif length > 1:
					expr = _make_small_string_cmp(var_name, buf, length, 1)
			else:
				expr = f'strncmp({var_name}, "{cstr_escape(buf)}", {length}) == 0'
			if expr:
				expr_list.append(expr)

		if expr_list:
			stmt_list.append('\n')
			if switch_type == SwitchType_Equal:
				stmt = f"return {' || '.join(expr_list)};\n"
				stmt_list.append(IndentLevel2 + stmt)
			elif switch_type == SwitchType_IfMatch:
				for expr in expr_list:
					stmt = f'if ({expr}) {{\n{IndentLevel2}}}\n'
					stmt_list.append(IndentLevel2 + stmt)
			elif switch_type == SwitchType_IfCached:
				for index, expr in enumerate(expr_list):
					word = cstr_escape(items[index][1])
					stmt = f'if ({expr})\n{IndentLevel3}return "{word}";\n'
					stmt_list.append(IndentLevel2 + stmt)
			elif switch_type == SwitchType_Classify:
				for index, expr in enumerate(expr_list):
					stmt = f'if ({expr})\n{IndentLevel3}return {items[index][2]};\n'
					stmt_list.append(IndentLevel2 + stmt)
			if switch_type != SwitchType_Equal:
				stmt_list.append(IndentLevel2 + "break;\n")
		else:
			if switch_type != SwitchType_IfMatch:
				stmt_list.append('\n')
			if switch_type == SwitchType_Equal:
				stmt = 'return true;\n'
				stmt_list.append(IndentLevel2 + stmt)
			elif switch_type == SwitchType_IfMatch:
				stmt = f' {{\n{IndentLevel1}}} break;\n'
				stmt_list.append(stmt)
			elif switch_type == SwitchType_IfCached:
				word = cstr_escape(items[0][1])
				stmt = f'return "{word}";\n'
				stmt_list.append(IndentLevel2 + stmt)
			elif switch_type == SwitchType_Classify:
				stmt = f'return {items[0][2]};\n'
				stmt_list.append(IndentLevel2 + stmt)

	stmt_list.append(IndentLevel1 + '}\n')
	stmt = _get_switch_func_tail(switch_type, option_set)
	stmt_list.append(stmt + '}\n\n')
	return ''.join(stmt_list)

def build_switch_stmt_head(switch_type, func_name, var_name, word_list, ch_index=0, switch_option=SwitchOption_HeadAndLength):
	return build_switch_stmt(switch_type, func_name, var_name, word_list, ch_index, switch_option)

def build_switch_stmt_hash(switch_type, func_name, var_name, word_list, hash_size=16, switch_option=SwitchOption_HashAndLength):
	return build_switch_stmt(switch_type, func_name, var_name, word_list, hash_size, switch_option)
