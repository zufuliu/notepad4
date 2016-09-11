#-*- coding: UTF-8 -*-
from __future__ import print_function
import re

def findHoles(asc):
	holes = []
	prev = asc[0]
	for val in asc:
		diff = val - prev
		if diff > 1:
			holes.extend(range(prev+1, val))
		prev = val
	return holes

kReFun = re.compile(r'\s*(fun|get|set)\s(\w+)\s(\w+)=(\d+)')
ifaceDoc = open('../include/Scintilla.iface').read()
result = kReFun.findall(ifaceDoc)

valList = {}
for item in result:
	name = item[2]
	val = int(item[3])
	if val in valList:
		print('duplicate value: %d %s %s' % (val, name, str(valList[val])))
		valList[val].append(name)
	else:
		valList[val] = [name]

allVals = sorted(valList.keys())
print('all values:', allVals)
allVals = [item for item in allVals if item < 3000]
print('min, max and holes:', allVals[0], allVals[-1], findHoles(allVals))
