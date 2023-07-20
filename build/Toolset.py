import os.path
import glob
import json

toolset_msvc = """
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='15.0'">v141</PlatformToolset>
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='16.0'">v142</PlatformToolset>
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='17.0'">v143</PlatformToolset>
""".strip('\r\n').splitlines()

toolset_llvm = """
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='15.0'">LLVM_v141</PlatformToolset>
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='16.0'">LLVM_v142</PlatformToolset>
    <PlatformToolset Condition="'$(VisualStudioVersion)'=='17.0'">LLVM_v143</PlatformToolset>
""".strip('\r\n').splitlines()

def update_project_toolset(projectPath):
	with open(projectPath, encoding='utf-8') as fd:
		doc = fd.read()
	lines = []
	previous = False
	for line in doc.splitlines():
		current = '<PlatformToolset Condition=' in line
		if current:
			if not previous:
				if 'LLVM' in line:
					lines.extend(toolset_llvm)
				else:
					lines.extend(toolset_msvc)
		else:
			lines.append(line)
		previous = current

	updated = '\n'.join(lines)
	if updated != doc:
		print('update:', projectPath)
		with open(projectPath, 'w', encoding='utf-8') as fd:
			fd.write(updated)

def update_all_project_toolset():
	for path in glob.glob('VS2017/*.vcxproj'):
		update_project_toolset(path)
	for path in glob.glob('../locale/*/*.vcxproj'):
		update_project_toolset(path)

def dump_static_linked_function(path):
	result = {}
	with open(path, encoding='utf-8') as fd:
		for line in fd.readlines():
			if line.count(':') > 1:
				items = line.split()
				func = items[1]
				obj = items[-1]
				if items[3] == 'f' and '<lambda_' not in func and items[4] != 'i':
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

def quote_path(path):
	return f'"{path}"' if ' ' in path else path

def build_compile_commands(commands, folder, cflags, cxxflags, includes, cxx=False):
	# https://clang.llvm.org/docs/JSONCompilationDatabase.html
	folder = os.path.abspath(folder)
	with os.scandir(folder) as it:
		for entry in it:
			if not entry.is_file():
				continue
			ext = os.path.splitext(entry.name)[1]
			if ext in ('.c', '.cpp', '.cxx'):
				path = entry.path
				arguments = cxxflags[:] if cxx or ext != '.c' else cflags[:]
				arguments.extend(includes)
				arguments.append(quote_path(path))
				commands.append({
					'directory': folder,
					#'arguments': arguments,
					'command': ' '.join(arguments),
					'file': path,
				})

def generate_compile_commands(target, avx2=False, cxx=False):
	cflags = []
	cxxflags = []
	# flags to run clang-tidy via vscode-clangd plugin, see https://clangd.llvm.org/
	defines = ['NDEBUG', '_WINDOWS', 'NOMINMAX', 'WIN32_LEAN_AND_MEAN', 'STRICT_TYPED_ITEMIDS',
		'UNICODE', '_UNICODE', '_CRT_SECURE_NO_WARNINGS', '_SCL_SECURE_NO_WARNINGS']
	warnings = ['-Wextra', '-Wshadow', '-Wimplicit-fallthrough', '-Wformat=2', '-Wundef', '-Wcomma']

	target_flag = '--target=' + target
	msvc = 'msvc' in target
	if msvc:
		cflags.extend(['clang-cl.exe', target_flag, '/c', '/std:c17', '/O2'])
		cxxflags.extend(['clang-cl.exe', target_flag, '/c', '/std:c++20', '/O2', '/EHsc', '/GR-'])
		warnings.insert(0, '/W4')
	else:
		cflags.extend(['clang.exe', target_flag, '-municode', '-c', '-std=gnu17', '-O2'])
		cxxflags.extend(['clang++.exe', target_flag, '-municode', '-c', '-std=gnu++20', '-O2', '-fno-rtti'])
		warnings.insert(0, '-Wall')
	if cxx:
		cxxflags.insert(2, '/TP' if msvc else 'x c++')

	arch = target[:target.index('-')]
	if arch == 'x86_64':
		defines.append('_WIN64')
		if avx2:
			cflags.append('-march=x86-64-v3')
			cxxflags.append('-march=x86-64-v3')
			defines.extend(['_WIN32_WINNT=0x0601', 'WINVER=0x0601'])	# 7
		else:
			defines.extend(['_WIN32_WINNT=0x0600', 'WINVER=0x0600'])	# Vista
	elif arch == 'i686':
		defines.extend(['WIN32', '_WIN32_WINNT=0x0501', 'WINVER=0x0501'])	# XP
	elif arch in ('aarch64', 'arm64'):
		defines.extend(['_WIN64', '_WIN32_WINNT=0x0A00', 'WINVER=0x0A00'])	# 10
	elif arch.startswith('arm'):
		defines.extend(['WIN32', '_WIN32_WINNT=0x0602', 'WINVER=0x0602'])	# 8

	defines = ['-D' + item for item in defines]
	cflags.extend(defines)
	cxxflags.extend(defines)
	cflags.extend(warnings)
	cxxflags.extend(warnings)

	config = [
		('../src', ['../scintilla/include']),
		('../scintilla/lexers', ['../include', '../lexlib']),
		('../scintilla/lexlib', ['../include']),
		('../scintilla/src', ['../include', '../lexlib']),
		('../scintilla/win32', ['../include', '../src']),
		('../metapath/src', []),
	]

	def include_path(folder, path):
		path = os.path.abspath(os.path.join(folder, path))
		return quote_path(path)

	commands = []
	for folder, includes in config:
		includes = ['-I' + include_path(folder, path) for path in includes]
		build_compile_commands(commands, folder, cflags, cxxflags, includes, cxx)

	path = '../compile_commands.json'
	print('write:', path)
	with open(path, 'w', encoding='utf-8', newline='\n') as fd:
		fd.write(json.dumps(commands, indent='\t', ensure_ascii=False))

#update_all_project_toolset()
#dump_static_linked_function('bin/Release/x64/metapath.map')
#dump_static_linked_function('bin/Release/x64/Notepad2.map')
generate_compile_commands('x86_64-pc-windows-msvc', cxx=True)
#generate_compile_commands('x86_64-w64-windows-gnu')
#run-clang-tidy --quiet -j4 1>tidy.log
