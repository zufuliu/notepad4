import os
import multiprocessing
import subprocess

# generate test for nested backquoted command substitution
# https://github.com/ScintillaOrg/lexilla/issues/194

def generate_backtick_test(level, what, n, m, preamble, backtick=True):
	backslash = '\\'*((1 << level) + n + m)
	prefix = [preamble, 'echo ']
	suffix = []
	inner = what.format(backslash[:n], backslash[:m])
	stack = [f'{">"*(level + 1)} {n},{m} echo {inner}']
	for i in range(level):
		if backtick:
			count = (1 << i) - 1
			escape = backslash[:count]
			prefix.append(f'{escape}`echo ')
			suffix.append(f' {escape}`')
		else:
			prefix.append('$(echo ')
			suffix.append(')')
		n = n >> 1
		m = m >> 1
		outer = what.format(backslash[:n], backslash[:m])
		stack.append(f'{">"*(level - i)} {n},{m} echo {outer}')
	prefix.append(inner)
	prefix.extend(reversed(suffix))
	stack.extend(['', ''])
	return ''.join(prefix), '\n'.join(stack)

def execute_backtick_test(level, what, n, m, preamble):
	code, stack = generate_backtick_test(level, what, n, m, preamble)
	path = f'x{level}-{n}-{m}.sh'
	with open(path, 'wb') as fd:
		fd.write(code.encode('utf-8'))
	proc = subprocess.run(['bash', '-x', path], check=False, capture_output=True)
	try:
		os.remove(path)
	except Exception:
		pass
	return code, proc.stdout, stack.encode('utf-8') + proc.stderr

def run_backtick_test(level, what, n, m, preamble):
	code, stdout, stderr = execute_backtick_test(level, what, n, m, preamble)
	prefix = f'\t{n:<3},{m:<3}\t{code}\t'
	status, suffix = 'OK', ' '
	fail = any(word in stderr for word in [b'unexpected', b'error', b'match'])
	if fail:
		status, suffix = 'FAIL', '\n\t\t# '
	else:
		code = f"{code} # {stdout.decode('utf-8').strip()} ({n},{m})"
	lines = (stdout + stderr).decode('utf-8').strip().splitlines()
	output = '\n\t\t# '.join(lines)
	return fail, code, f'{prefix}# {status}:{suffix}{output}'

def parallel_test_backtick(pool, all_test, level, max_num):
	result = []
	valid = []
	invalid = []
	for (what, preamble) in all_test:
		label = '# ' + what
		result.append(label)
		valid.append(label)
		invalid.append(label)
		second = max_num if '{1}' in what else 1
		args = [(level, what, n, m, preamble) for n in range(max_num) for m in range(second)]
		for fail, code, output in pool.starmap(run_backtick_test, args):
			result.append(output)
			if fail:
				invalid.append(code)
			else:
				valid.append(code)
		result.append('\n')
		valid.append('\n')
		invalid.append('\n')
	path = f'backtick-{level}-{max_num}'
	with open(path + '.log', 'wb') as fd:
		fd.write('\n'.join(result).encode('utf-8'))
	with open(path + '.sh', 'wb') as fd:
		fd.write('\n'.join(valid).encode('utf-8'))
	with open(path + '-bad.sh', 'wb') as fd:
		fd.write('\n'.join(invalid).encode('utf-8'))

def test_backtick_main():
	all_test = [
		('{0}$foo', 'foo=A; '),
		('{0}$[1]', ''),
		('{0}$((1))', ''),
		('{0}$(echo $[1])', ''),
		('$(echo {0}$[1])', ''),
		('{0}$(echo {1}$[1])', ''),
		(r"{0}$'A\'CB'", ''),

		('{0}"', ''),
		('{0}"A"', ''),
		('{0}"A{1}"', ''),

		('{0}`', ''),
		('{0}`echo $[1]{1}`', ''),
		('$(echo {0}`echo $[1]{1}`)', ''),
	]
	with multiprocessing.Pool(multiprocessing.cpu_count()) as pool:
		parallel_test_backtick(pool, all_test, 1, 16)
		parallel_test_backtick(pool, all_test, 2, 32)
		parallel_test_backtick(pool, all_test, 3, 64)
		parallel_test_backtick(pool, all_test, 4, 128)
		parallel_test_backtick(pool, all_test, 5, 256)

if __name__ == '__main__':
	#! MUST run inside __main__ block
	test_backtick_main()
