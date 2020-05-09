#!/usr/bin/env python3
#-*- coding: UTF-8 -*-
import sys
import os.path
import re
import shutil
import uuid

app = os.path.basename(__file__)
localeDir = '.'
notepad2_src = '../src/Notepad2.rc'
metapath_src = '../metapath/src/metapath.rc'

def get_available_locales():
	result = []
	with os.scandir(localeDir) as it:
		for entry in it:
			if entry.is_dir() and entry.name[0].islower():
				result.append(entry.name)

	result.sort()
	return result

def get_project_guid():
	return '{' + str(uuid.uuid4()).upper() + '}'

def patch_vc_project_file(path, src_lang, language):
	doc = open(path, encoding='utf-8', newline='\n').read()
	# change output folder
	doc = doc.replace(f'\\{src_lang}\\', f'\\{language}\\')

	# change project GUID
	guid = get_project_guid()
	doc = re.sub(r'(<ProjectGuid>)(.+)(</ProjectGuid>)', r'\1' + guid + r'\3', doc)

	with open(path, 'w', encoding='utf-8', newline='\n') as fd:
		fd.write(doc)

def update_resource_include_path(path, metapath):
	doc = open(path, encoding='utf-8', newline='\n').read()
	if metapath:
		# resource path
		doc = doc.replace(r'..\\res', r'..\\..\\metapath\\res')
		# include path
		doc = re.sub(r'^(#include\s+")(.+)(")', r'\1../../metapath/src/\2\3', doc, flags=re.MULTILINE)
	else:
		# resource path
		doc = doc.replace(r'..\\res', r'..\\..\\res')
		# include path
		doc = re.sub(r'^(#include\s+")(.+)(")', r'\1../../src/\2\3', doc, flags=re.MULTILINE)
		# string table for lexers and styles
		doc = re.sub(r'^//(#include\s+")(.+)(")', r'\1../../src/\2\3', doc, flags=re.MULTILINE)
		doc = re.sub(r'^#if\s+0\s*//\s*(NP2_ENABLE_LOCALIZE\w+)', r'#if \1', doc, flags=re.MULTILINE)

	with open(path, 'w', encoding='utf-8', newline='\n') as fd:
		fd.write(doc)

def make_new_localization(language):
	print(f'{app}: make new localization for {language}.')
	target = os.path.join(localeDir, language)
	if not os.path.exists(target):
		os.makedirs(target)

	metapath_dest = os.path.join(target, 'metapath.rc')
	notepad2_dest = os.path.join(target, 'Notepad2.rc')
	shutil.copy(metapath_src, metapath_dest)
	shutil.copy(notepad2_src, notepad2_dest)

	update_resource_include_path(metapath_dest, True)
	update_resource_include_path(notepad2_dest, False)

	src_lang = 'zh-Hans'
	src_folder = os.path.join(localeDir, src_lang)
	metapath_vcxproj_src = os.path.join(src_folder, f'metapath({src_lang}).vcxproj')
	notepad2_vcxproj_src = os.path.join(src_folder, f'Notepad2({src_lang}).vcxproj')
	metapath_vcxproj = os.path.join(target, f'metapath({language}).vcxproj')
	notepad2_vcxproj = os.path.join(target, f'Notepad2({language}).vcxproj')

	shutil.copy(metapath_vcxproj_src, metapath_vcxproj)
	shutil.copy(metapath_vcxproj_src + '.filters', metapath_vcxproj + '.filters')
	shutil.copy(notepad2_vcxproj_src, notepad2_vcxproj)
	shutil.copy(notepad2_vcxproj_src + '.filters', notepad2_vcxproj + '.filters')

	patch_vc_project_file(metapath_vcxproj, src_lang, language)
	patch_vc_project_file(notepad2_vcxproj, src_lang, language)

	metapath_dest = os.path.basename(metapath_dest)
	notepad2_dest = os.path.basename(notepad2_dest)
	metapath_vcxproj = os.path.basename(metapath_vcxproj)
	notepad2_vcxproj = os.path.basename(notepad2_vcxproj)
	print(f"""{app}: resources and projects added for {language}.
    Please manually update language tags in {metapath_dest} and {notepad2_dest},
    and open Locale.sln with Visual Studio to add project {metapath_vcxproj} and {notepad2_vcxproj}.""")


def restore_resource_include_path(path, metapath):
	doc = open(path, encoding='utf-8', newline='\n').read()
	if metapath:
		# include path
		doc = doc.replace('../../metapath/src/', '')
		# resource path
		doc = doc.replace(r'..\\metapath\\', '')
	else:
		# include path
		doc = doc.replace('../../src/', '')
		# resource path
		doc = doc.replace(r'..\\..\\res', r'..\\res')

	with open(path, 'w', encoding='utf-8', newline='\n') as fd:
		fd.write(doc)

def copy_back_localized_resources(language):
	print(f'{app}: copy back localized resources for {language}.')
	backupDir = os.path.join(localeDir, 'en')
	if os.path.exists(backupDir):
		if language != 'en':
			print(f"""{app}: please delete the backup folder {backupDir}
    on first run to ensure proper backup for English resources.""")
	else:
		os.makedirs(backupDir)
		shutil.copy(metapath_src, os.path.join(backupDir, 'metapath.rc'))
		shutil.copy(notepad2_src, os.path.join(backupDir, 'Notepad2.rc'))

	folder = os.path.join(localeDir, language)
	shutil.copy(os.path.join(folder, 'metapath.rc'), metapath_src)
	shutil.copy(os.path.join(folder, 'Notepad2.rc'), notepad2_src)

	restore_resource_include_path(metapath_src, True)
	restore_resource_include_path(notepad2_src, False)

	print(f"""{app}: resources for building standalone localized program for {language} are ready.
    you can copy English resources back by run: {app} back en""")


def show_help():
	print(f"""Usage: {app} action language
action:
    new     create a new localization for specific language.

    back    prepare building standalone localized program for specific language,
            copy back localized resources to overwrite English resources.
            English resources are copied into en folder when the folder does not exist.""")

def main():
	if len(sys.argv) < 3:
		show_help()
		return

	action = sys.argv[1]
	language = sys.argv[2]
	availableLocales = get_available_locales()
	if action == 'new':
		if language in availableLocales:
			print(f'{app}: language {language} already localized.');
			return
		make_new_localization(language)
	elif action == 'back':
		if language not in availableLocales:
			print(f'{app}: language {language} not localized [{", ".join(availableLocales)}].');
			return
		copy_back_localized_resources(language)
	else:
		show_help()

if __name__ == '__main__':
	main()
