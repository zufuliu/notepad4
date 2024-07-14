# generate test to find valid variable in for loop
# https://github.com/zufuliu/notepad4/pull/348

def generate_for_var_test(path):
	with open(path, 'w', encoding='utf-8', newline='\r\n') as fd:
		tokens = ','.join(map(str, range(1, 31)))
		items = ' '.join(map(str, range(1, 35)))
		fd.write(f"""@echo off
	SetLocal EnableExtensions EnableDelayedExpansion

	for /F "tokens={tokens},*" %%! ^
	in ("{items}") ^
	do (
	""")

		escape_map = {
			'%': '%',

			')': '^)',
			'"': '^"',
			'&': '^&',
			'|': '^|',
			'<': '^<',
			'>': '^>',
		}

		lines = []
		for code in range(33, 127):
			ch = chr(code)
			esc = escape_map.get(ch, '')
			if esc:
				if esc[0] == '^':
					lines.append(f'echo {esc}: %%{esc}')
				else:
					lines.append(f':: invalid {esc}')
			elif ch == '!':
				lines.append('echo ^^!: %%!')
			elif ch == '^':
				lines.append('echo ^^: %%^^')
			else:
				lines.append(f'echo {ch}: %%{ch}')

		fd.write('\t')
		fd.write('\n\t'.join(lines))
		fd.write("\n)\n")

generate_for_var_test('test-for-var.bat')
