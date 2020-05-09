#!/usr/bin/env python3
#-*- coding: UTF-8 -*-
import sys
import os.path
import re

def increase_style_resource_id_value(path, delta=100):
	doc = open(path, encoding='utf-8', newline='\n').read()
	updated = re.sub(r'\d{5}', lambda m: str(int(m.group(0)) + delta), doc)
	print('update:', path)
	with open(path, 'w', encoding='utf-8', newline='\n') as fp:
		fp.write(updated)

#increase_style_resource_id_value('../src/EditLexers/EditStyle.h')
