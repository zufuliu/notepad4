import os.path
import time

scriptFolder = os.getcwd()
notepad2ConfigPath = os.path.abspath('../src/config.h')
metapathConfigPath = os.path.abspath('../metapath/src/config.h')

activeLocaleList = ['i18n', 'en', 'it', 'ja', 'ko', 'zh-Hans', 'zh-Hant']
defaultConfig = {
	'NP2_ENABLE_CUSTOMIZE_TOOLBAR_LABELS': 0,
	'NP2_ENABLE_HIDPI_TOOLBAR_IMAGE': 1,

	'NP2_ENABLE_DOT_LOG_FEATURE': 0,

	'NP2_ENABLE_APP_LOCALIZATION_DLL': 1,
	'NP2_ENABLE_TEST_LOCALIZATION_LAYOUT': 0,
	'NP2_ENABLE_LOCALIZE_LEXER_NAME': 1,
	'NP2_ENABLE_LOCALIZE_STYLE_NAME': 1,
}

def get_locale_override_config(locale, hd):
	override = {}
	if not hd:
		override['NP2_ENABLE_HIDPI_TOOLBAR_IMAGE'] = 0
	if locale != 'i18n':
		override['NP2_ENABLE_APP_LOCALIZATION_DLL'] = 0
	if locale == 'en':
		override['NP2_ENABLE_LOCALIZE_LEXER_NAME'] = 0
		override['NP2_ENABLE_LOCALIZE_STYLE_NAME'] = 0
	return override

def update_raw_file(path, content):
	with open(path, 'rb') as fd:
		origin = fd.read()
		if origin == content:
			return
	print('update:', path)
	with open(path, 'wb') as fd:
		fd.write(content)

def update_config_file(override):
	config = defaultConfig.copy()
	config.update(override)
	output = ['#pragma once']
	output.append('')
	for key, value in config.items():
		output.append(f'#define {key}\t\t{value}')
	output.append('')
	content = '\n'.join(output).encode('utf-8')
	update_raw_file(notepad2ConfigPath, content)
	update_raw_file(metapathConfigPath, content)


def run_command_in_folder(command, folder):
	print(f'run: {command} @ {folder}')
	os.chdir(folder)
	os.system(command)

def copy_back_local_resource(locale):
	command = f'python Locale.py back {locale}'
	folder = os.path.normpath(os.path.join(scriptFolder, '../locale'))
	run_command_in_folder(command, folder)

def build_main_project(arch):
	command = f'call build.bat Build {arch} Release'
	folder = os.path.join(scriptFolder, 'VS2017')
	run_command_in_folder(command, folder)

def build_locale_project(arch):
	command = f'call build.bat Build {arch} Release'
	folder = os.path.normpath(os.path.join(scriptFolder, '../locale'))
	run_command_in_folder(command, folder)

def make_release_artifact(suffix, extra):
	command = f'call make_zip.bat {extra} {suffix}'
	run_command_in_folder(command, scriptFolder)

def delete_unused_artifact():
	unused = []
	total = 0
	with os.scandir(scriptFolder) as it:
		for entry in it:
			name = entry.name
			if entry.is_file() and name.endswith('.zip'):
				total += 1
				# only keep 32-bit ARM artifact for i18n and en
				if '_ARM_' in name and not ('_en_' in name or '_i18n_' in name):
					unused.append(entry.path)
	print('total artifact:', total - len(unused))
	for path in unused:
		print('delete:', path)
		os.remove(path)

def build_release_artifact(hd, suffix=''):
	for locale in activeLocaleList:
		print('build:', hd, locale)
		override = get_locale_override_config(locale, hd)
		update_config_file(override)
		if locale not in ('i18n', 'en'):
			copy_back_local_resource(locale)
		for arch in ['ARM', 'ARM64', 'AVX2', 'Win32', 'x64']:
			if arch != 'ARM' or locale in ('i18n', 'en'):
				# 32-bit ARM is only built for i18n and en
				build_main_project(arch)
		# build locale DLL
		extra = ''
		if locale == 'i18n':
			extra = 'Locale'
			for arch in ['ARM', 'ARM64', 'AVX2', 'Win32']:
				# x64 locale DLL already built after building AVX2
				build_locale_project(arch)
		make_release_artifact(suffix + locale, extra)
	copy_back_local_resource('en')

def build_all_release_artifact():
	print('script folder:', scriptFolder)
	startTime = time.perf_counter()
	build_release_artifact(True, 'HD_')
	build_release_artifact(False)
	delete_unused_artifact()
	endTime = time.perf_counter()
	duration = round((endTime - startTime)/60, 2)
	print('total build time:', duration)

build_all_release_artifact()
