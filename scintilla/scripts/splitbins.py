# splitbins() is based on Python source
# https://github.com/python/cpython/blob/main/Tools/unicode/makeunicodedata.py
# Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010,
# 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019 Python Software Foundation;
# All Rights Reserved
# see Python-LICENSE.txt or https://www.python.org/psf/license/ for license details.

import sys
import math
from collections.abc import Iterable

def getsize(data):
	# return smallest possible integer size for the given array
	if isinstance(data, Iterable):
		maxdata = max(data)
	else:
		maxdata = data
	if maxdata < 256:
		return 1
	elif maxdata < 65536:
		return 2
	else:
		return 4

def splitbins(t, second=False):
	"""t -> (t1, t2, shift).  Split a table to save space.

	t is a sequence of ints.  This function can be useful to save space if
	many of the ints are the same.	t1 and t2 are lists of ints, and shift
	is an int, chosen to minimize the combined size of t1 and t2 (in C
	code), and where for each i in range(len(t)),
		t[i] == t2[(t1[i >> shift] << shift) | (i & mask)]
	where mask is a bitmask isolating the last "shift" bits.
	"""

	# the most we can shift n and still have something left
	maxshift = math.floor(math.log2(len(t)))

	total = sys.maxsize	 # smallest total size so far
	t = tuple(t)	# so slices can be dict keys
	for shift in range(maxshift + 1):
		t1 = []
		t2 = []
		size = 2**shift
		bincache = {}
		for i in range(0, len(t), size):
			part = t[i:i+size]
			index = bincache.get(part)
			if index is None:
				index = len(t2)
				bincache[part] = index
				t2.extend(part)
			t1.append(index >> shift)
		# determine memory size
		b = len(t1)*getsize(t1)
		if second:
			t3, t4, shift2 = splitbins(t2, False)
			b += len(t3)*getsize(t3) + len(t4)*getsize(t4)
		else:
			b += len(t2)*getsize(t2)
		if b < total:
			if second:
				best = t1, t3, t4, shift, shift2
			else:
				best = t1, t2, shift
			total = b
	return best

def preshift(index, shift):
	m = max(index)
	size = getsize(m)
	n = (2**(size * 8) - 1) // m
	n = math.floor(math.log2(n))
	if n > 0:
		n = min(n, shift)
		for i in range(len(index)):
			index[i] = index[i] << n
		return shift - n
	return shift
