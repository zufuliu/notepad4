#-*- coding: UTF-8 -*-
from StringSwitch import *

def _used_string_switch():
	with open('switch.c', 'wb') as fd:
		word_list = ['if', 'for', 'try', 'using', 'while', 'elseif', 'switch', 'foreach', 'synchronized']
		stmt = build_switch_stmt_head(SwitchType_Equal, 'NeedSpaceAfterKeyword', 'word', word_list)
		fd.write(stmt.encode('utf-8'))

		word_list = ['base', 'br', 'hr', 'embed', 'img', 'input', 'link', 'meta']
		stmt = build_switch_stmt_head(SwitchType_Equal, 'IsHtmlVoidTag', 'word', word_list)
		fd.write(stmt.encode('utf-8'))

if __name__ == '__main__':
	_used_string_switch()
