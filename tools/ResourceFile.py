import glob
import re
from functools import wraps

def file_updater(func):
	@wraps(func)
	def wrapper(*args):
		path = args[0]
		with open(path, encoding='utf-8', newline='\n') as fd:
			doc = fd.read()
		updated = func(doc, *args[1:])
		if updated != doc:
			print('update:', path)
			with open(path, 'w', encoding='utf-8', newline='\n') as fd:
				fd.write(updated)
	return wrapper

@file_updater
def increase_style_resource_id_value(doc, delta=100):
	return re.sub(r'\d{5}', lambda m: str(int(m.group(0)) + delta), doc)

@file_updater
def generate_lexer_menu_resource_id(doc):
	dummy = {'id': 40700}
	def get_value():
		result = str(dummy['id'])
		dummy['id'] += 1
		return result

	start = doc.index('#define IDM_LEXER_TEXTFILE')
	end = doc.index('\n', doc.index('#define IDM_LEXER_LEXER_COUNT'))
	menu = doc[start:end]
	updated = re.sub(r'\d{5}', lambda m: get_value(), menu)
	return doc[:start] + updated + doc[end:]

def add_style(match, style):
	text = match.group(0)
	return text if style in text else text + style

@file_updater
def remove_old_style(doc):
	return doc.replace(' | DS_NOFAILCREATE', '')

@file_updater
def add_scrollbar_style(doc):
	style = ',SBS_SIZEGRIP | WS_CLIPSIBLINGS'
	return re.sub(r'(SCROLLBAR\s+IDC_RESIZEGRIP[^\r\n]+)', lambda m: add_style(m, style), doc)

@file_updater
def add_thick_frame_style(doc):
	parts = doc.split(' DIALOGEX ')
	style = ' | WS_THICKFRAME'
	for index, item in enumerate(parts):
		if re.search(r'SCROLLBAR\s+IDC_RESIZEGRIP', item):
			item = re.sub(r'(STYLE DS_SETFONT[^\r\n]+)', lambda m: add_style(m, style), item)
			parts[index] = item
	return ' DIALOGEX '.join(parts)

def update_all_resource_file(func):
	func('../src/Notepad4.rc')
	func('../matepath/src/matepath.rc')
	for path in glob.glob('../locale/*/*.rc'):
		func(path)

# increase_style_resource_id_value('../src/EditLexers/EditStyle.h')
# generate_lexer_menu_resource_id('../src/resource.h')

# update_all_resource_file(remove_old_style)
# update_all_resource_file(add_scrollbar_style)
# update_all_resource_file(add_thick_frame_style)
