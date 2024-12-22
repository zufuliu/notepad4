import sys
import os.path
import glob
import re

def dump_static_linked_function(path, dumpAll=True):
	result = {}
	with open(path, encoding='utf-8') as fd:
		for line in fd.readlines():
			if line.count(':') > 1:
				items = line.split()
				func = items[1]
				obj = items[-1]
				if dumpAll or (items[3] == 'f' and '<lambda_' not in func and items[4] != 'i'):
					if obj in result:
						result[obj].append(func)
					else:
						result[obj] = [func]

	path, ext = os.path.splitext(path)
	path = f'{path}-crt{ext}'
	print('write:', path)
	with open(path, 'w', encoding='utf-8') as fd:
		for obj, items in sorted(result.items()):
			fd.write(obj + '\n')
			fd.write(''.join(f'\t{func}\n' for func in sorted(items)))

text_segment = re.compile(r'_TEXT\s+SEGMENT')
proc_comdat = re.compile(r'(\w+\s+)?PROC\s*;(.+?)COMDAT')
sub_rsp = re.compile(r'sub\s+(e|r)sp\s*,\s*(\d+)')
add_rsp = re.compile(r'add\s+(e|r)sp\s*,\s*(\d+)')
mov_eax = re.compile(r'mov\s+eax\s*,\s*(\d+)')
call_chkstk = re.compile(r'call\s+_?_?chkstk')

def get_stack_size(path, result_map, threshold):
	with open(path, 'r', encoding='cp1252') as fd:
		doc = fd.read()
	segmentList = text_segment.split(doc)
	path = os.path.basename(path)
	for segment in segmentList:
		items = sub_rsp.findall(segment) + add_rsp.findall(segment)
		items = [item[1] for item in items]
		chkstk = call_chkstk.search(segment)
		if chkstk:
			items.extend(mov_eax.findall(segment[:chkstk.start(0)]))
		if items:
			stack_size = max(int(value) for value in items)
			if stack_size >= threshold:
				proc = proc_comdat.search(segment)
				name = proc.group(2).strip(' \t,;')
				if not name:
					name = proc.group(1).strip()
				if stack_size in result_map:
					result = result_map[stack_size]
					if path in result:
						result[path].append(name)
					else:
						result[path] = [name]
				else:
					result_map[stack_size] = {path: [name]}

def dump_stack_size():
	if len(sys.argv) < 3:
		print(f'Usgae: {sys.argv[0]} threshold <path or folder>')
		return

	threshold = int(sys.argv[1])
	result_map = {}
	for arg in sys.argv[2:]:
		if os.path.isfile(arg):
			get_stack_size(arg, result_map, threshold)
		elif os.path.isdir(arg):
			for path in glob.glob(os.path.join(arg, '*.cod')):
				get_stack_size(path, result_map, threshold)

	if not result_map:
		print(f'No result for threshold: {threshold}')
		return

	output = []
	for stack_size in sorted(result_map.keys(), reverse=True):
		output.append(f'{stack_size}:\n')
		result = result_map[stack_size]
		for path in sorted(result.keys()):
			output.append(f'\t{path}:\n\t')
			output.append('\n\t'.join(sorted(result[path])))
			output.append('\n\n')
	path = f'StackSize{threshold}.log'
	print(f'write: {path}')
	with open(path, 'w', encoding='utf-8') as fd:
		fd.write(''.join(output))

#dump_static_linked_function('bin/Release/x64/matepath.map')
#dump_static_linked_function('bin/Release/Win32/matepath.map')
#dump_static_linked_function('bin/Release/x64/Notepad4.map')
#dump_static_linked_function('bin/Release/Win32/Notepad4.map')
dump_stack_size()
