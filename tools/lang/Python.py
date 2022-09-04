# https://www.python.org/
# https://ironpython.net/
# https://www.jython.org/
# https://cython.org/

# http://cobra-language.com/
# https://boo-language.github.io/
# https://wiki.gnome.org/Projects/Genie

#! keywords			===========================================================
# https://docs.python.org/3/reference/lexical_analysis.html#identifiers
and as assert async await
break
class continue
def del
elif else except
finally for from
global
if import in is
lambda
nonlocal not
or
pass
raise return
try
while with
yield

case match
# https://docs.python.org/2.7/reference/lexical_analysis.html#identifiers
exec print

#! Built-in Constants	===========================================================
# https://docs.python.org/3/library/constants.html
False None True NotImplemented Ellipsis
__debug__ __main__
self

#! Built-in Functions	===========================================================
# https://docs.python.org/3/library/functions.html
__builtins__
builtins
	abs(x)
	aiter(async_iterable)
	all(iterable)
	anext(async_iterator[, default])
	any(iterable)
	ascii(object)
	bin(x)
	class bool([x])
	breakpoint(*args, **kws)
	class bytearray([source[, encoding[, errors]]]):
	class bytes([source[, encoding[, errors]]]):
	callable(object)
	chr(i)
	@classmethod
	classmethod()
	compile(source, filename, mode, flags=0, dont_inherit=False, optimize=-1)
	class complex([real[, imag]])
	delattr(object, name)
	class dict(**kwarg)
	class dict(mapping, **kwarg)
	class dict(iterable, **kwarg):
	dir([object])
	divmod(a, b)
	enumerate(iterable, start=0)
	eval(expression[, globals[, locals]])
	exec(object[, globals[, locals]])
	filter(function, iterable)
	class float([x]):
	format(value[, format_spec])
	class frozenset([iterable])
	getattr(object, name[, default])
	globals()
	hasattr(object, name)
	hash(object)
	help([object])
	hex(x)
	id(object)
	input([prompt])
	class int([x]):
	class int(x, base=10)
	isinstance(object, classinfo)
	issubclass(class, classinfo)
	iter(object[, sentinel])
	len(s)
	class list([iterable]):
	locals()
	map(function, iterable, ...)
	max(iterable, *[, key, default])
	max(arg1, arg2, *args[, key])
	class memoryview(object):
	min(iterable, *[, key, default])
	min(arg1, arg2, *args[, key])
	next(iterator[, default])
	class object
	oct(x)
	open(file, mode='r', buffering=-1, encoding=None, errors=None, newline=None, closefd=True, opener=None)
	ord(c)
	pow(base, exp[, mod])
	print(*objects, sep=' ', end='\n', file=sys.stdout, flush=False)
	class property(fget=None, fset=None, fdel=None, doc=None):
	@property
	class range(stop)
	class range(start, stop[, step])
	repr(object)
	reversed(seq)
	round(number[, ndigits])
	class set([iterable]):
	setattr(object, name, value)
	class slice(stop)
	class slice(start, stop[, step])
	sorted(iterable, *, key=None, reverse=False)
	@staticmethod
	staticmethod(function)
	class str(object='')
	class str(object=b'', encoding='utf-8', errors='strict'):
	sum(iterable, /, start=0)
	class super([type[, object-or-type]])
	class tuple([iterable])
	class type(object)
	class type(name, bases, dict, **kwds)
	vars([object])
	zip(*iterables, strict=False)
	__import__(name, globals=None, locals=None, fromlist=(), level=0)

	# https://docs.python.org/2.7/library/functions.html#built-in-functions
	class basestring()
	cmp(x, y)
	execfile(filename[, globals[, locals]])
	class file(name[, mode[, buffering]])
	class long(x=0)
	class long(x, base=10)
	raw_input([prompt])
	reduce(function, iterable[, initializer])
	reload(module)
	unichr(i)
	class unicode(object='')
	class unicode(object[, encoding[, errors]])
	class xrange(stop)
	class xrange(start, stop[, step])
	apply(function, args[, keywords])
	class buffer(object[, offset[, size]])
	coerce(x, y)
	intern(string)

# https://docs.python.org/3/library/constants.html#constants-added-by-the-site-module
site
	quit(code=None)
	exit(code=None)
	copyright
	license
	credits

#! attributes			===========================================================
# https://docs.python.org/3/reference/datamodel.html#the-standard-type-hierarchy
# Callable types
__doc__
__name__
__qualname__
__module__
__defaults__
__code__
__globals__
__dict__
__closure__
__annotations__
__kwdefaults__
__self__
__func__
# Modules
__name__
__doc__
__file__
__annotations__

__all__
__version__
# Custom classes
__name__
__module__
__dict__
__bases__
__doc__
__annotations__
# Special Attributes
# https://docs.python.org/3/library/stdtypes.html#special-attributes
__dict__
__class__
__bases__
__mro__
mro()
__subclasses__()
__name__
__qualname__

# module attributes
# https://docs.python.org/3/reference/import.html#import-related-module-attributes
__name__
__loader__
__package__
__spec__
__path__
__file__
__cached__

#! special method	===========================================================
# https://docs.python.org/3/reference/datamodel.html#special-method-names
object:
	# Basic customization
	__new__(cls[, ...])
	__init__(self[, ...])
	__del__(self)
	__repr__(self)
	__str__(self)
	__bytes__(self)
	__format__(self, format_spec)
	__lt__(self, other)
	__le__(self, other)
	__eq__(self, other)
	__ne__(self, other)
	__gt__(self, other)
	__ge__(self, other)
	__hash__(self)
	__bool__(self)
	# Customizing attribute access
	__getattr__(self, name)
	__getattribute__(self, name)
	__setattr__(self, name, value)
	__delattr__(self, name)
	__dir__(self)
	# Implementing Descriptors
	__get__(self, instance, owner=None)
	__set__(self, instance, value)
	__delete__(self, instance)
	__slots__
	__weakref__
	# Customizing class creation
	__init_subclass__(cls)
	__set_name__(self, owner, name)
	# Emulating generic types
	__class_getitem__(cls, key)
	# Emulating callable objects
	__call__(self[, args...])
	# Emulating container types
	__len__(self)
	__length_hint__(self)
	__getitem__(self, key)
	__setitem__(self, key, value)
	__delitem__(self, key)
	__missing__(self, key)
	__iter__(self)
	__reversed__(self)
	__contains__(self, item)
	# Emulating numeric types
	__add__(self, other)
	__sub__(self, other)
	__mul__(self, other)
	__matmul__(self, other)
	__truediv__(self, other)
	__floordiv__(self, other)
	__mod__(self, other)
	__divmod__(self, other)
	__pow__(self, other[, modulo])
	__lshift__(self, other)
	__rshift__(self, other)
	__and__(self, other)
	__xor__(self, other)
	__or__(self, other)

	__radd__(self, other)
	__rsub__(self, other)
	__rmul__(self, other)
	__rmatmul__(self, other)
	__rtruediv__(self, other)
	__rfloordiv__(self, other)
	__rmod__(self, other)
	__rdivmod__(self, other)
	__rpow__(self, other[, modulo])
	__rlshift__(self, other)
	__rrshift__(self, other)
	__rand__(self, other)
	__rxor__(self, other)
	__ror__(self, other)

	__iadd__(self, other)
	__isub__(self, other)
	__imul__(self, other)
	__imatmul__(self, other)
	__itruediv__(self, other)
	__ifloordiv__(self, other)
	__imod__(self, other)
	__ipow__(self, other[, modulo])
	__ilshift__(self, other)
	__irshift__(self, other)
	__iand__(self, other)
	__ixor__(self, other)
	__ior__(self, other)

	__neg__(self)
	__pos__(self)
	__abs__(self)
	__invert__(self)

	__complex__(self)
	__int__(self)
	__float__(self)

	__index__(self)

	__round__(self[, ndigits])
	__trunc__(self)
	__floor__(self)
	__ceil__(self)

	# With Statement Context Managers
	__enter__(self)
	__exit__(self, exc_type, exc_value, traceback)
	# Pattern Matching
	__match_args__
	# Coroutines
	__await__(self)
	# Asynchronous Iterators
	__aiter__(self)
	__anext__(self)
	# Asynchronous Context Managers
	__aenter__(self)
	__aexit__(self, exc_type, exc_value, traceback)

	__sizeof__(self)
	# Python 2.7
	__cmp__(self, other)
	__nonzero__(self)
	__unicode__(self)
	__div__(self, other)
	__rdiv__(self, other)
	__idiv__(self, other)
	__long__(self)
	__oct__(self)
	__hex__(self)
	__coerce__(self, other)

# Metaclasses
class:
	__metaclass__ 	# Python 2.7
	__prepare__(name, bases, **kwds)
	__instancecheck__(self, instance)
	__subclasscheck__(self, subclass)

#! exceptions		===========================================================
BaseException
	BaseExceptionGroup
	GeneratorExit
	KeyboardInterrupt
	SystemExit
	Exception
		ArithmeticError
			FloatingPointError
			OverflowError
			ZeroDivisionError
		AssertionError
		AttributeError
		BufferError
		EOFError
		ExceptionGroup
		ImportError
			ModuleNotFoundError
		LookupError
			IndexError
			KeyError
		MemoryError
		NameError
			UnboundLocalError
		OSError
			BlockingIOError
			ChildProcessError
			ConnectionError
				BrokenPipeError
				ConnectionAbortedError
				ConnectionRefusedError
				ConnectionResetError
			FileExistsError
			FileNotFoundError
			InterruptedError
			IsADirectoryError
			NotADirectoryError
			PermissionError
			ProcessLookupError
			TimeoutError
		ReferenceError
		RuntimeError
			NotImplementedError
			RecursionError
		StopAsyncIteration
		StopIteration
		SyntaxError
			IndentationError
				TabError
		SystemError
		TypeError
		ValueError
			UnicodeError
				UnicodeDecodeError
				UnicodeEncodeError
				UnicodeTranslateError
		Warning
			BytesWarning
			DeprecationWarning
			EncodingWarning
			FutureWarning
			ImportWarning
			PendingDeprecationWarning
			ResourceWarning
			RuntimeWarning
			SyntaxWarning
			UnicodeWarning
			UserWarning

#! API				===========================================================
# https://docs.python.org/3/library/index.html
# Built-in Types
# https://docs.python.org/3/library/stdtypes.html
int:
	bit_length()
	bit_count()
	to_bytes(length, byteorder, *, signed=False)
	from_bytes(bytes, byteorder, *, signed=False)
	as_integer_ratio()
float:
	as_integer_ratio()
	is_integer()
	hex()
	fromhex(s)
list:
	min(s)
	max(s)
	index(x[, i[, j]])
	count(x)
	append(x)
	clear()
	copy()
	extend(t)
	insert(i, x)
	pop([i])
	remove(x)
	reverse()
	sort(*, key=None, reverse=False)
str:
	capitalize()
	casefold()
	center(width[, fillchar])
	count(sub[, start[, end]])
	encode(encoding="utf-8", errors="strict | ignore | replace | xmlcharrefreplace | backslashreplace")
	endswith(suffix[, start[, end]])
	expandtabs(tabsize=8)
	find(sub[, start[, end]])
	format(*args, **kwargs)
	format_map(mapping)
	index(sub[, start[, end]])
	isalnum()
	isalpha()
	isascii()
	isdecimal()
	isdigit()
	isidentifier()
	islower()
	isnumeric()
	isprintable()
	isspace()
	istitle()
	isupper()
	join(iterable)
	ljust(width[, fillchar])
	lower()
	lstrip([chars])
	maketrans(x[, y[, z]])
	partition(sep)
	removeprefix(prefix, /)
	removesuffix(suffix, /)
	replace(old, new[, count])
	rfind(sub[, start[, end]])
	rindex(sub[, start[, end]])
	rjust(width[, fillchar])
	rpartition(sep)
	rsplit(sep=None, maxsplit=-1)
	rstrip([chars])
	split(sep=None, maxsplit=-1)
	splitlines([keepends])
	startswith(prefix[, start[, end]])
	strip([chars])
	swapcase()
	title()
	translate(table)
	upper()
	zfill(width)
bytearray:
	fromhex(string)
	maketrans(from, to)
bytes:
	fromhex(string)
	hex([sep[, bytes_per_sep]])
	count(sub[, start[, end]])
	removeprefix(prefix, /)
	removesuffix(suffix, /)
	decode(encoding="utf-8", errors="strict | ignore | replace")
	endswith(suffix[, start[, end]])
	find(sub[, start[, end]])
	index(sub[, start[, end]])
	join(iterable)
	maketrans(from, to)
	partition(sep)
	replace(old, new[, count])
	rfind(sub[, start[, end]])
	rindex(sub[, start[, end]])
	rpartition(sep)
	startswith(prefix[, start[, end]])
	translate(table, /, delete=b'')
	center(width[, fillbyte])
	ljust(width[, fillbyte])
	lstrip([chars])
	rjust(width[, fillbyte])
	rsplit(sep=None, maxsplit=-1)
	rstrip([chars])
	split(sep=None, maxsplit=-1)
	strip([chars])
	capitalize()
	expandtabs(tabsize=8)
	isalnum()
	isalpha()
	isascii()
	isdigit()
	islower()
	isspace()
	istitle()
	isupper()
	lower()
	splitlines(keepends=False)
	swapcase()
	title()
	upper()
	zfill(width)
memoryview:
	__eq__(exporter)
	tobytes(order=None)
	hex([sep[, bytes_per_sep]])
	tolist()
	toreadonly()
	release()
	cast(format[, shape])
	obj
	nbytes
	readonly
	format
	itemsize
	ndim
	shape
	strides
	suboffsets
	c_contiguous
	f_contiguous
	contiguous
set:
	isdisjoint(other)
	issubset(other)
	issuperset(other)
	union(*others)
	intersection(*others)
	difference(*others)
	symmetric_difference(other)
	copy()

	update(*others)
	intersection_update(*others)
	difference_update(*others)
	symmetric_difference_update(other)
	add(elem)
	remove(elem)
	discard(elem)
	pop()
	clear()
dict:
	clear()
	copy()
	fromkeys(iterable[, value])
	get(key[, default])
	items()
	keys()
	pop(key[, default])
	popitem()
	reversed(d)
	setdefault(key[, default])
	update([other])
	values()
property:
	getter
	setter
	deleter
slice:
	indices(self, length)

exception BaseException:
	__traceback__
	args
	with_traceback(tb)
	add_note(note)
	__notes__
exception OSError([arg])
exception OSError(errno, strerror[, filename[, winerror[, filename2]]]):
	errno
	winerror
	strerror
	filename
	filename2
exception SyntaxError(message, details):
	filename
	lineno
	offset
	text
	end_lineno
	end_offset
exception SystemExit:
	code
exception UnicodeError:
	encoding
	reason
	object
	start
	end
exception BlockingIOError:
	characters_written
exception BaseExceptionGroup(msg, excs):
	message
	exceptions
	subgroup(condition)
	split(condition)
	derive(excs)

# Text Processing Services
# https://docs.python.org/3/library/text.html
string
	ascii_letters
	ascii_lowercase
	ascii_uppercase
	digits
	hexdigits
	octdigits
	punctuation
	printable
	whitespace
	class Formatter:
		format(format_string, /, *args, **kwargs)
		vformat(format_string, args, kwargs)
		parse(format_string)
		get_field(field_name, args, kwargs)
		get_value(key, args, kwargs)
		check_unused_args(used_args, args, kwargs)
		format_field(value, format_spec)
		convert_field(value, conversion)
	class Template(template):
		substitute(mapping={}, /, **kwds)
		safe_substitute(mapping={}, /, **kwds)
		is_valid()
		get_identifiers()
		template
	capwords(s, sep=None)
re
	class RegexFlag
	ASCII
	DEBUG
	IGNORECASE
	LOCALE
	MULTILINE
	DOTALL
	VERBOSE
	compile(pattern, flags=0)
	search(pattern, string, flags=0)
	match(pattern, string, flags=0)
	fullmatch(pattern, string, flags=0)
	split(pattern, string, maxsplit=0, flags=0)
	findall(pattern, string, flags=0)
	finditer(pattern, string, flags=0)
	sub(pattern, repl, string, count=0, flags=0)
	subn(pattern, repl, string, count=0, flags=0)
	escape(pattern)
	purge()
	exception error(msg, pattern=None, pos=None):
		msg
		pattern
		pos
		lineno
		colno
	class Pattern:
		search(string[, pos[, endpos]])
		match(string[, pos[, endpos]])
		fullmatch(string[, pos[, endpos]])
		split(string, maxsplit=0)
		findall(string[, pos[, endpos]])
		finditer(string[, pos[, endpos]])
		sub(repl, string, count=0)
		subn(repl, string, count=0)
		flags
		groups
		groupindex
		pattern
	class Match:
		expand(template)
		group([group1, ...])
		__getitem__(g)
		groups(default=None)
		groupdict(default=None)
		start([group])
		end([group])
		span([group])
		pos
		endpos
		lastindex
		lastgroup
		string
textwrap
	wrap(text, width=70, **kwargs)
	fill(text, width=70, **kwargs)
	shorten(text, width, **kwargs)
	dedent(text)
	indent(text, prefix, predicate=None)
	class TextWrapper(**kwargs):
		width
		expand_tabs
		tabsize
		replace_whitespace
		drop_whitespace
		initial_indent
		subsequent_indent
		fix_sentence_endings
		break_long_words
		break_on_hyphens
		max_lines
		placeholder
		wrap(text)
		fill(text)
unicodedata
	lookup(name)
	name(chr[, default])
	decimal(chr[, default])
	digit(chr[, default])
	numeric(chr[, default])
	category(chr)
	bidirectional(chr)
	combining(chr)
	east_asian_width(chr)
	mirrored(chr)
	decomposition(chr)
	normalize(form, unistr)
	is_normalized(form, unistr)
	unidata_version
	ucd_3_2_0

# Binary Data Services
# https://docs.python.org/3/library/binary.html
struct
	exception error(msg)
	pack(format, v1, v2, ...)
	pack_into(format, buffer, offset, v1, v2, ...)
	unpack(format, buffer)
	unpack_from(format, /, buffer, offset=0)
	iter_unpack(format, buffer)
	calcsize(format)
	class Struct(format):
		pack(v1, v2, ...)
		pack_into(buffer, offset, v1, v2, ...)
		unpack(buffer)
		unpack_from(buffer, offset=0)
		iter_unpack(buffer)
		format
		size
codecs
	encode(obj, encoding='utf-8', errors='strict | ignore | replace | xmlcharrefreplace | backslashreplace | namereplace | surrogateescape | surrogatepass')
	decode(obj, encoding='utf-8', errors='strict')
	lookup(encoding)
	class CodecInfo(encode, decode, streamreader=None, streamwriter=None, incrementalencoder=None, incrementaldecoder=None, name=None)
	getencoder(encoding)
	getdecoder(encoding)
	getincrementalencoder(encoding)
	getincrementaldecoder(encoding)
	getreader(encoding)
	getwriter(encoding)
	register(search_function)
	unregister(search_function)
	open(filename, mode='r', encoding=None, errors='strict', buffering=- 1)
	EncodedFile(file, data_encoding, file_encoding=None, errors='strict')
	iterencode(iterator, encoding, errors='strict', **kwargs)
	iterdecode(iterator, encoding, errors='strict', **kwargs)
	BOM_UTF8
	BOM_UTF16
	BOM_UTF16_BE
	BOM_UTF16_LE
	BOM_UTF32
	BOM_UTF32_BE
	BOM_UTF32_LE
	register_error(name, error_handler)
	lookup_error(name)
	strict_errors(exception)
	replace_errors(exception)
	ignore_errors(exception)
	xmlcharrefreplace_errors(exception)
	backslashreplace_errors(exception)
	namereplace_errors(exception)
	class Codec:
		encode(input[, errors])
		decode(input[, errors])
	class IncrementalEncoder(errors='strict'):
		encode(object[, final])
		reset()
		getstate()
		setstate(state)
	class IncrementalDecoder(errors='strict'):
		decode(object[, final])
		reset()
		getstate()
		setstate(state)
	class StreamWriter(stream, errors='strict'):
		write(object)
		writelines(list)
		reset()
	class StreamReader(stream, errors='strict'):
		read([size[, chars[, firstline]]])
		readline([size[, keepends]])
		readlines([sizehint[, keepends]])
		reset()
	class StreamReaderWriter(stream, Reader, Writer, errors='strict')
	class StreamRecoder(stream, encode, decode, Reader, Writer, errors='strict')

# Data Types
# https://docs.python.org/3/library/datatypes.html
datetime
	MINYEAR
	MAXYEAR
	UTC
	class timedelta(days=0, seconds=0, microseconds=0, milliseconds=0, minutes=0, hours=0, weeks=0):
		min
		max
		resolution
		total_seconds()
	class date(year, month, day):
		today()
		fromtimestamp(timestamp)
		fromordinal(ordinal)
		fromisoformat(date_string)
		fromisocalendar(year, week, day)
		min
		max
		year
		month
		day
		replace(year=self.year, month=self.month, day=self.day)
		timetuple()
		toordinal()
		weekday()
		isoweekday()
		isocalendar()
		isoformat()
		ctime()
		strftime(format)
	class datetime(year, month, day, hour=0, minute=0, second=0, microsecond=0, tzinfo=None, *, fold=0):
		today()
		now(tz=None)
		utcnow()
		fromtimestamp(timestamp, tz=None)
		utcfromtimestamp(timestamp)
		fromordinal(ordinal)
		combine(date, time, tzinfo=self.tzinfo)
		fromisoformat(date_string)
		fromisocalendar(year, week, day)
		strptime(date_string, format)
		min
		max
		resolution
		year
		month
		day
		hour
		minute
		second
		microsecond
		tzinfo
		fold
		date()
		time()
		timetz()
		replace(year=self.year, month=self.month, day=self.day, hour=self.hour, minute=self.minute, second=self.second, microsecond=self.microsecond, tzinfo=self.tzinfo, *, fold=0)
		astimezone(tz=None)
		utcoffset()
		dst()
		tzname()
		timetuple()
		utctimetuple()
		toordinal()
		timestamp()
		weekday()
		isoweekday()
		isocalendar()
		isoformat(sep='T', timespec='auto')
		ctime()
		strftime(format)
	class time(hour=0, minute=0, second=0, microsecond=0, tzinfo=None, *, fold=0):
		min
		max
		resolution
		hour
		minute
		second
		microsecond
		tzinfo
		fold
		fromisoformat(time_string)
		replace(hour=self.hour, minute=self.minute, second=self.second, microsecond=self.microsecond, tzinfo=self.tzinfo, *, fold=0)
		isoformat(timespec='auto')
		strftime(format)
		utcoffset()
		dst()
		tzname()
	class tzinfo:
		utcoffset(dt)
		dst(dt)
		tzname(dt)
		fromutc(dt)
	class timezone(offset, name=None):
		utcoffset(dt)
		tzname(dt)
		dst(dt)
		fromutc(dt)
		utc
collections
	class ChainMap(*maps):
		maps
		new_child(m=None, **kwargs)
		parents
	class Counter([iterable-or-mapping]):
		elements()
		most_common([n])
		subtract([iterable-or-mapping])
		total()
		fromkeys(iterable)
		update([iterable-or-mapping])
	class deque([iterable[, maxlen]]):
		append(x)
		appendleft(x)
		clear()
		copy()
		count(x)
		extend(iterable)
		extendleft(iterable)
		index(x[, start[, stop]])
		insert(i, x)
		pop()
		popleft()
		remove(value)
		reverse()
		rotate(n=1)
		maxlen
	class defaultdict(default_factory=None, /[, ...]):
		__missing__(key)
		default_factory
	namedtuple(typename, field_names, *, rename=False, defaults=None, module=None):
		_make(iterable)
		_asdict()
		_replace(**kwargs)
		_fields
		_field_defaults
	class OrderedDict([items]):
		popitem(last=True)
		move_to_end(key, last=True)
	class UserDict([initialdata]):
		data
	class UserList([list]):
		data
	class UserString(seq)
		data
collections.abc
	class Container:
		__contains__()
	class Hashable:
		__hash__()
	class Sized:
		__len__()
	class Callable:
		__call__()
	class Iterable
		__iter__()
	class Collection
	class Iterator:
		__iter__()
		__next__()
	class Reversible
	# https://docs.python.org/3/reference/expressions.html#generator-iterator-methods
	class Generator:
		__next__()
		send(value)
		throw(type[, value[, traceback]])
		close()
	class Sequence
	class MutableSequence
	class ByteString
	class Set
	class MutableSet
	class Mapping
	class MutableMapping
	class MappingView
	class ItemsView
	class KeysView
	class ValuesView
	# https://docs.python.org/3/reference/datamodel.html#coroutine-objects
	class Awaitable:
		__await__()
		send(value)
		throw(type[, value[, traceback]])
		close()
	class Coroutine
	class AsyncIterable:
		 __aiter__()
	class AsyncIterator:
		 __aiter__()
		__anext__()
	# https://docs.python.org/3/reference/expressions.html#asynchronous-generator-iterator-methods
	class AsyncGenerator:
		__anext__()
		asend(value)
		athrow(type[, value[, traceback]])
		aclose()
heapq
	heappush(heap, item)
	heappop(heap)
	heappushpop(heap, item)
	heapify(x)
	heapreplace(heap, item)
	merge(*iterables, key=None, reverse=False)
	nlargest(n, iterable, key=None)
	nsmallest(n, iterable, key=None)
bisect
	bisect_left(a, x, lo=0, hi=len(a), *, key=None)
	bisect_right(a, x, lo=0, hi=len(a), *, key=None)
	bisect(a, x, lo=0, hi=len(a))
	insort_left(a, x, lo=0, hi=len(a), *, key=None)
	insort_right(a, x, lo=0, hi=len(a), *, key=None)
	insort(a, x, lo=0, hi=len(a))
array
	class array(typecode[, initializer])
	typecodes
	typecode
	itemsize
	append(x)
	buffer_info()
	byteswap()
	count(x)
	extend(iterable)
	frombytes(s)
	fromfile(f, n)
	fromlist(list)
	fromunicode(s)
	index(x[, start[, stop]])
	insert(i, x)
	pop([i])
	remove(x)
	reverse()
	tobytes()
	tofile(f)
	tolist()
	tounicode()
types
	class GenericAlias(t_origin, t_args):
		__origin__
		__args__
		__parameters__
		__unpacked__
copy
	__copy__()
	__deepcopy__()
	copy(x)
	deepcopy(x[, memo])
	exception Error
enum
	class Enum:
		__members__
	class IntEnum
	class StrEnum
	class Flag
	class IntFlag
	auto()
	@unique
	@property
	@verify
	@member
	@nonmember

# Numeric and Mathematical Modules
# https://docs.python.org/3/library/numeric.html
numbers
	class Number
	class Complex
		real
		imag
		conjugate()
	class Real
	class Rational
		numerator
		denominator
	class Integral
math
	# Number-theoretic and representation functions
	ceil(x)
	comb(n, k)
	copysign(x, y)
	fabs(x)
	factorial(x)
	floor(x)
	fmod(x, y)
	frexp(x)
	fsum(iterable)
	gcd(*integers)
	isclose(a, b, *, rel_tol=1e-09, abs_tol=0.0)
	isfinite(x)
	isinf(x)
	isnan(x)
	isqrt(n)
	lcm(*integers)
	ldexp(x, i)
	modf(x)
	nextafter(x, y)
	perm(n, k=None)
	prod(iterable, *, start=1)
	remainder(x, y)
	trunc(x)
	ulp(x)
	# Power and logarithmic functions
	cbrt(x)
	exp(x)
	exp2(x)
	expm1(x)
	log(x[, base])
	log1p(x)
	log2(x)
	log10(x)
	pow(x, y)
	sqrt(x)
	# Trigonometric functions
	acos(x)
	asin(x)
	atan(x)
	atan2(y, x)
	cos(x)
	dist(p, q)
	hypot(*coordinates)
	sin(x)
	tan(x)
	# Angular conversion
	degrees(x)
	radians(x)
	# Hyperbolic functions
	acosh(x)
	asinh(x)
	atanh(x)
	cosh(x)
	sinh(x)
	tanh(x)
	# Special functions
	erf(x)
	erfc(x)
	gamma(x)
	lgamma(x)
	# Constants
	pi
	tau
	inf
	nan
cmath
	phase(x)
	polar(x)
	rect(r, phi)
	exp(x)
	log(x[, base])
	log10(x)
	sqrt(x)
	acos(x)
	asin(x)
	atan(x)
	cos(x)
	sin(x)
	tan(x)
	acosh(x)
	asinh(x)
	atanh(x)
	cosh(x)
	sinh(x)
	tanh(x)
	isfinite(x)
	isinf(x)
	isnan(x)
	isclose(a, b, *, rel_tol=1e-09, abs_tol=0.0)
	pi
	tau
	inf
	infj
	nan
	nanj
decimal
	class Decimal(value='0', context=None):
		adjusted()
		as_integer_ratio()
		as_tuple()
		canonical()
		compare(other, context=None)
		compare_signal(other, context=None)
		compare_total(other, context=None)
		compare_total_mag(other, context=None)
		conjugate()
		copy_abs()
		copy_negate()
		copy_sign(other, context=None)
		exp(context=None)
		from_float(f)
		fma(other, third, context=None)
		is_canonical()
		is_finite()
		is_infinite()
		is_nan()
		is_normal(context=None)
		is_qnan()
		is_signed()
		is_snan()
		is_subnormal(context=None)
		is_zero()
		ln(context=None)
		log10(context=None)
		logb(context=None)
		logical_and(other, context=None)
		logical_invert(context=None)
		logical_or(other, context=None)
		logical_xor(other, context=None)
		max(other, context=None)
		max_mag(other, context=None)
		min(other, context=None)
		min_mag(other, context=None)
		next_minus(context=None)
		next_plus(context=None)
		next_toward(other, context=None)
		normalize(context=None)
		number_class(context=None)
		quantize(exp, rounding=None, context=None)
		radix()
		remainder_near(other, context=None)
		rotate(other, context=None)
		same_quantum(other, context=None)
		scaleb(other, context=None)
		shift(other, context=None)
		sqrt(context=None)
		to_eng_string(context=None)
		to_integral(rounding=None, context=None)
		to_integral_exact(rounding=None, context=None)
		to_integral_value(rounding=None, context=None)
	getcontext()
	setcontext(c)
	localcontext(ctx=None)
	class BasicContext
	class ExtendedContext
	class DefaultContext
	class Context(prec=None, rounding=None, Emin=None, Emax=None, capitals=None, clamp=None, flags=None, traps=None):
		clear_flags()
		clear_traps()
		copy()
		copy_decimal(num)
		create_decimal(num)
		create_decimal_from_float(f)
		Etiny()
		Etop()
		abs(x)
		add(x, y)
		canonical(x)
		compare(x, y)
		compare_signal(x, y)
		compare_total(x, y)
		compare_total_mag(x, y)
		copy_abs(x)
		copy_negate(x)
		copy_sign(x, y)
		divide(x, y)
		divide_int(x, y)
		divmod(x, y)
		exp(x)
		fma(x, y, z)
		is_canonical(x)
		is_finite(x)
		is_infinite(x)
		is_nan(x)
		is_normal(x)
		is_qnan(x)
		is_signed(x)
		is_snan(x)
		is_subnormal(x)
		is_zero(x)
		ln(x)
		log10(x)
		logb(x)
		logical_and(x, y)
		logical_invert(x)
		logical_or(x, y)
		logical_xor(x, y)
		max(x, y)
		max_mag(x, y)
		min(x, y)
		min_mag(x, y)
		minus(x)
		multiply(x, y)
		next_minus(x)
		next_plus(x)
		next_toward(x, y)
		normalize(x)
		number_class(x)
		plus(x)
		power(x, y, modulo=None)
		quantize(x, y)
		radix()
		remainder(x, y)
		remainder_near(x, y)
		rotate(x, y)
		same_quantum(x, y)
		scaleb(x, y)
		shift(x, y)
		sqrt(x)
		subtract(x, y)
		to_eng_string(x)
		to_integral_exact(x)
		to_sci_string(x)
	# Constants
	MAX_PREC
	MAX_EMAX
	MIN_EMIN
	MIN_ETINY
	HAVE_CONTEXTVAR
	ROUND_CEILING
	ROUND_DOWN
	ROUND_FLOOR
	ROUND_HALF_DOWN
	ROUND_HALF_EVEN
	ROUND_HALF_UP
	ROUND_UP
	ROUND_05UP
	class Clamped
	class DecimalException
	class DivisionByZero
	class Inexact
	class InvalidOperation
	class Overflow
	class Rounded
	class Subnormal
	class Underflow
	class FloatOperation

	Infinity
	Inf
	NaN
	sNaN
fractions
	class Fraction(numerator=0, denominator=1)
	class Fraction(other_fraction)
	class Fraction(float)
	class Fraction(decimal)
	class Fraction(string):
		numerator
		denominator
		as_integer_ratio()
		from_float(flt)
		from_decimal(dec)
		limit_denominator(max_denominator=1000000)
		__floor__()
		__ceil__()
		__round__()
		__round__(ndigits)
random
	seed(a=None, version=2)
	getstate()
	setstate(state)
	randbytes(n)
	randrange(stop)
	randrange(start, stop[, step])
	randint(a, b)
	getrandbits(k)
	choice(seq)
	choices(population, weights=None, *, cum_weights=None, k=1)
	shuffle(x[, random])
	sample(population, k, *, counts=None)
	random()
	uniform(a, b)
	triangular(low, high, mode)
	betavariate(alpha, beta)
	expovariate(lambd)
	gammavariate(alpha, beta)
	gauss(mu, sigma)
	lognormvariate(mu, sigma)
	normalvariate(mu, sigma)
	vonmisesvariate(mu, kappa)
	paretovariate(alpha)
	weibullvariate(alpha, beta)
	class Random([seed])
	class SystemRandom([seed])
statistics
	mean(data)
	fmean(data)
	geometric_mean(data)
	harmonic_mean(data, weights=None)
	median(data)
	median_low(data)
	median_high(data)
	median_grouped(data, interval=1)
	mode(data)
	multimode(data)
	pstdev(data, mu=None)
	pvariance(data, mu=None)
	stdev(data, xbar=None)
	variance(data, xbar=None)
	quantiles(data, *, n=4, method='exclusive')
	covariance(x, y, /)
	correlation(x, y, /)
	linear_regression(x, y, /)
	exception StatisticsError
	class NormalDist(mu=0.0, sigma=1.0):
		mean
		median
		mode
		stdev
		variance
		from_samples(data)
		samples(n, *, seed=None)
		pdf(x)
		cdf(x)
		inv_cdf(p)
		overlap(other)
		quantiles(n=4)
		zscore(x)

# Functional Programming Modules
# https://docs.python.org/3/library/functional.html
itertools
	accumulate(iterable[, func, *, initial=None])
	chain(*iterables)
	from_iterable(iterable)
	combinations(iterable, r)
	combinations_with_replacement(iterable, r)
	compress(data, selectors)
	count(start=0, step=1)
	cycle(iterable)
	dropwhile(predicate, iterable)
	filterfalse(predicate, iterable)
	groupby(iterable, key=None)
	islice(iterable, stop)
	islice(iterable, start, stop[, step])
	pairwise(iterable)
	permutations(iterable, r=None)
	product(*iterables, repeat=1)
	repeat(object[, times])
	starmap(function, iterable)
	takewhile(predicate, iterable)
	tee(iterable, n=2)
	zip_longest(*iterables, fillvalue=None)
functools
	@cache(user_function)
	@cached_property(func)
	cmp_to_key(func)
	@lru_cache(user_function)
	@lru_cache(maxsize=128, typed=False)
	@total_ordering
	partial(func, /, *args, **keywords)
	class partialmethod(func, /, *args, **keywords)
	reduce(function, iterable[, initializer])
	@singledispatch
	class singledispatchmethod(func)
	update_wrapper(wrapper, wrapped, assigned=WRAPPER_ASSIGNMENTS, updated=WRAPPER_UPDATES)
	@wraps(wrapped, assigned=WRAPPER_ASSIGNMENTS, updated=WRAPPER_UPDATES)
	partial:
		func
		args
		keywords

# File and Directory Access
# https://docs.python.org/3/library/filesys.html
pathlib
	class PurePath(*pathsegments):
		parts
		drive
		root
		anchor
		parents
		parent
		name
		suffix
		suffixes
		stem
		as_posix()
		as_uri()
		is_absolute()
		is_relative_to(*other)
		is_reserved()
		joinpath(*other)
		match(pattern)
		relative_to(*other)
		with_name(name)
		with_stem(stem)
		with_suffix(suffix)
	class PurePosixPath(*pathsegments)
	class PureWindowsPath(*pathsegments)
	class Path(*pathsegments):
		cwd()
		home()
		stat(*, follow_symlinks=True)
		chmod(mode, *, follow_symlinks=True)
		exists()
		expanduser()
		glob(pattern)
		group()
		is_dir()
		is_file()
		is_mount()
		is_symlink()
		is_socket()
		is_fifo()
		is_block_device()
		is_char_device()
		iterdir()
		lchmod(mode)
		lstat()
		mkdir(mode=511, parents=False, exist_ok=False)
		open(mode='r', buffering=- 1, encoding=None, errors=None, newline=None)
		owner()
		read_bytes()
		read_text(encoding=None, errors=None)
		readlink()
		rename(target)
		replace(target)
		resolve(strict=False)
		rglob(pattern)
		rmdir()
		samefile(other_path)
		symlink_to(target, target_is_directory=False)
		hardlink_to(target)
		link_to(target)
		touch(mode=438, exist_ok=True)
		unlink(missing_ok=False)
		write_bytes(data)
		write_text(data, encoding=None, errors=None, newline=None)
	class PosixPath(*pathsegments)
	class WindowsPath(*pathsegments)
os.path
	abspath(path)
	basename(path)
	commonpath(paths)
	commonprefix(list)
	dirname(path)
	exists(path)
	lexists(path)
	expanduser(path)
	expandvars(path)
	getatime(path)
	getmtime(path)
	getctime(path)
	getsize(path)
	isabs(path)
	isfile(path)
	isdir(path)
	islink(path)
	ismount(path)
	join(path, *paths)
	normcase(path)
	normpath(path)
	realpath(path, *, strict=False)
	relpath(path, start=os.curdir)
	samefile(path1, path2)
	sameopenfile(fp1, fp2)
	samestat(stat1, stat2)
	split(path)
	splitdrive(path)
	splitext(path)
	supports_unicode_filenames
glob
	glob(pathname, *, root_dir=None, dir_fd=None, recursive=False)
	iglob(pathname, *, root_dir=None, dir_fd=None, recursive=False)
	escape(pathname)
fnmatch
	fnmatch(filename, pattern)
	fnmatchcase(filename, pattern)
	filter(names, pattern)
	translate(pattern)
shutil
	copyfileobj(fsrc, fdst[, length])
	copyfile(src, dst, *, follow_symlinks=True)
	exception SameFileError
	copymode(src, dst, *, follow_symlinks=True)
	copystat(src, dst, *, follow_symlinks=True)
	copy(src, dst, *, follow_symlinks=True)
	copy2(src, dst, *, follow_symlinks=True)
	ignore_patterns(*patterns)
	copytree(src, dst, symlinks=False, ignore=None, copy_function=copy2, ignore_dangling_symlinks=False, dirs_exist_ok=False)
	rmtree(path, ignore_errors=False, onerror=None)
	avoids_symlink_attacks
	move(src, dst, copy_function=copy2)
	disk_usage(path)
	chown(path, user=None, group=None)
	which(cmd, mode=os.F_OK | os.X_OK, path=None)
	exception Error
	make_archive(base_name, format[, root_dir[, base_dir[, verbose[, dry_run[, owner[, group[, logger]]]]]]])
	get_archive_formats()
	register_archive_format(name, function[, extra_args[, description]])
	unregister_archive_format(name)
	unpack_archive(filename[, extract_dir[, format]])
	register_unpack_format(name, extensions, function[, extra_args[, description]])
	unregister_unpack_format(name)
	get_unpack_formats()
	get_terminal_size(fallback=columns, lines)

# Data Persistence
# https://docs.python.org/3/library/persistence.html
pickle:
	dump(obj, file, protocol=None, *, fix_imports=True, buffer_callback=None)
	dumps(obj, protocol=None, *, fix_imports=True, buffer_callback=None)
	load(file, *, fix_imports=True, encoding='ASCII', errors='strict', buffers=None)
	loads(data, /, *, fix_imports=True, encoding="ASCII", errors="strict", buffers=None)
	# Pickling Class Instances
	object:
		__getinitargs__()
		__getnewargs_ex__()
		__getnewargs__()
		__getstate__()
		__setstate__(state)
		__reduce__()
		__reduce_ex__(protocol)
sqlite3
	PARSE_DECLTYPES
	PARSE_COLNAMES
	SQLITE_OK
	SQLITE_DENY
	SQLITE_IGNORE
	apilevel
	paramstyle
	sqlite_version
	sqlite_version_info
	threadsafety
	version
	version_info
	connect(database[, timeout, detect_types, isolation_level, check_same_thread, factory, cached_statements, uri])
	complete_statement(sql)
	enable_callback_tracebacks(flag)
	register_converter(typename, callable)
	register_adapter(type, callable)
	class Connection:
		isolation_level
		in_transaction
		row_factory
		text_factory
		total_changes
		cursor(factory=Cursor)
		blobopen(table, column, row, /, \*, readonly=False, name="main")
		commit()
		rollback()
		close()
		execute(sql[, parameters])
		executemany(sql[, parameters])
		executescript(sql_script)
		create_function(name, num_params, func, *, deterministic=False)
		create_window_function(name, num_params, aggregate_class, /)
		create_aggregate(name, num_params, aggregate_class)
		create_collation(name, callable)
		interrupt()
		set_authorizer(authorizer_callback)
		set_progress_handler(handler, n)
		set_trace_callback(trace_callback)
		enable_load_extension(enabled)
		load_extension(path)
		iterdump()
		backup(target, *, pages=- 1, progress=None, name='main', sleep=0.25)
		getlimit(category, /)
		setlimit(category, limit, /)
		serialize(\*, name="main")
		deserialize(data, /, \*, name="main")
	class Cursor:
		execute(sql[, parameters])
		executemany(sql, seq_of_parameters)
		executescript(sql_script)
		fetchone()
		fetchmany(size=cursor.arraysize)
		fetchall()
		close()
		setinputsizes(sizes, /)
		setoutputsize(size, column=None, /)
		rowcount
		lastrowid
		arraysize
		description
		connection
	class Row
		keys()
	class Blob
		close()
		read(length=- 1, /)
		write(data, /)
		tell()
		seek(offset, origin=os.SEEK_SET, /)
	class PrepareProtocol
	exception Warning
	exception Error:
		sqlite_errorcode
		sqlite_errorname
	exception InterfaceError
	exception DatabaseError
	exception DataError
	exception OperationalError
	exception IntegrityError
	exception InternalError
	exception ProgrammingError
	exception NotSupportedError

# Generic Operating System Services
# https://docs.python.org/3/library/allos.html
os
	exception error
	name
	# Process Parameters
	ctermid()
	environ
	environb
	chdir(path)
	fchdir(fd)
	getcwd()
	fsencode(filename)
	fsdecode(filename)
	fspath(path)
	class PathLike:
		__fspath__()
	getenv(key, default=None)
	getenvb(key, default=None)
	get_exec_path(env=None)
	getegid()
	geteuid()
	getgid()
	getgrouplist(user, group)
	getgroups()
	getlogin()
	getpgid(pid)
	getpgrp()
	getpid()
	getppid()
	getpriority(which, who)
	PRIO_PROCESS
	PRIO_PGRP
	PRIO_USER
	getresuid()
	getresgid()
	getuid()
	initgroups(username, gid)
	putenv(key, value)
	setegid(egid)
	seteuid(euid)
	setgid(gid)
	setgroups(groups)
	setpgrp()
	setpgid(pid, pgrp)
	setpriority(which, who, priority)
	setregid(rgid, egid)
	setresgid(rgid, egid, sgid)
	setresuid(ruid, euid, suid)
	setreuid(ruid, euid)
	getsid(pid)
	setsid()
	setuid(uid)
	strerror(code)
	supports_bytes_environ
	umask(mask)
	uname()
	unsetenv(key)
	# File Object Creation
	fdopen(fd, *args, **kwargs)
	# File Descriptor Operations
	close(fd)
	closerange(fd_low, fd_high)
	copy_file_range(src, dst, count, offset_src=None, offset_dst=None)
	device_encoding(fd)
	dup(fd)
	dup2(fd, fd2, inheritable=True)
	fchmod(fd, mode)
	fchown(fd, uid, gid)
	fdatasync(fd)
	fpathconf(fd, name)
	fstat(fd)
	fstatvfs(fd)
	fsync(fd)
	ftruncate(fd, length)
	get_blocking(fd)
	isatty(fd)
	lockf(fd, cmd, len)
	F_LOCK
	F_TLOCK
	F_ULOCK
	F_TEST
	lseek(fd, pos, how)
	SEEK_SET
	SEEK_CUR
	SEEK_END
	open(path, flags, mode=511, *, dir_fd=None)
	O_RDONLY
	O_WRONLY
	O_RDWR
	O_APPEND
	O_CREAT
	O_EXCL
	O_TRUNC
	O_DSYNC
	O_RSYNC
	O_SYNC
	O_NDELAY
	O_NONBLOCK
	O_NOCTTY
	O_CLOEXEC
	O_BINARY
	O_NOINHERIT
	O_SHORT_LIVED
	O_TEMPORARY
	O_RANDOM
	O_SEQUENTIAL
	O_TEXT
	O_EVTONLY
	O_FSYNC
	O_SYMLINK
	O_NOFOLLOW_ANY
	O_ASYNC
	O_DIRECT
	O_DIRECTORY
	O_NOFOLLOW
	O_NOATIME
	O_PATH
	O_TMPFILE
	O_SHLOCK
	O_EXLOCK
	openpty()
	pipe()
	pipe2(flags)
	posix_fallocate(fd, offset, len)
	posix_fadvise(fd, offset, len, advice)
	POSIX_FADV_NORMAL
	POSIX_FADV_SEQUENTIAL
	POSIX_FADV_RANDOM
	POSIX_FADV_NOREUSE
	POSIX_FADV_WILLNEED
	POSIX_FADV_DONTNEED
	pread(fd, n, offset)
	preadv(fd, buffers, offset, flags=0)
	RWF_NOWAIT
	RWF_HIPRI
	pwrite(fd, str, offset)
	pwritev(fd, buffers, offset, flags=0)
	RWF_DSYNC
	RWF_SYNC
	RWF_APPEND
	read(fd, n)
	sendfile(out_fd, in_fd, offset, count)
	sendfile(out_fd, in_fd, offset, count, headers=(), trailers=(), flags=0)
	set_blocking(fd, blocking)
	SF_NODISKIO
	SF_MNOWAIT
	SF_SYNC
	splice(src, dst, count, offset_src=None, offset_dst=None)
	SPLICE_F_MOVE
	SPLICE_F_NONBLOCK
	SPLICE_F_MORE
	readv(fd, buffers)
	tcgetpgrp(fd)
	tcsetpgrp(fd, pg)
	ttyname(fd)
	write(fd, str)
	writev(fd, buffers)
	# Querying the size of a terminal
	get_terminal_size(fd=STDOUT_FILENO)
	class terminal_size::
		columns
		lines
	# Inheritance of File Descriptors
	get_inheritable(fd)
	set_inheritable(fd, inheritable)
	get_handle_inheritable(handle)
	set_handle_inheritable(handle, inheritable)
	# Files and Directories
	access(path, mode, *, dir_fd=None, effective_ids=False, follow_symlinks=True)
	F_OK
	R_OK
	W_OK
	X_OK
	chdir(path)
	chflags(path, flags, *, follow_symlinks=True)
	chmod(path, mode, *, dir_fd=None, follow_symlinks=True)
	chown(path, uid, gid, *, dir_fd=None, follow_symlinks=True)
	chroot(path)
	fchdir(fd)
	getcwd()
	getcwdb()
	lchflags(path, flags)
	lchmod(path, mode)
	lchown(path, uid, gid)
	link(src, dst, *, src_dir_fd=None, dst_dir_fd=None, follow_symlinks=True)
	listdir(path='.')
	lstat(path, *, dir_fd=None)
	mkdir(path, mode=511, *, dir_fd=None)
	makedirs(name, mode=511, exist_ok=False)
	mkfifo(path, mode=438, *, dir_fd=None)
	mknod(path, mode=384, device=0, *, dir_fd=None)
	major(device)
	minor(device)
	makedev(major, minor)
	pathconf(path, name)
	pathconf_names
	readlink(path, *, dir_fd=None)
	remove(path, *, dir_fd=None)
	removedirs(name)
	rename(src, dst, *, src_dir_fd=None, dst_dir_fd=None)
	renames(old, new)
	replace(src, dst, *, src_dir_fd=None, dst_dir_fd=None)
	rmdir(path, *, dir_fd=None)
	scandir(path='.')
	class DirEntry:
		name
		path
		inode()
		is_dir(*, follow_symlinks=True)
		is_file(*, follow_symlinks=True)
		is_symlink()
		stat(*, follow_symlinks=True)
	stat(path, *, dir_fd=None, follow_symlinks=True)
	class stat_result:
		st_mode
		st_ino
		st_dev
		st_nlink
		st_uid
		st_gid
		st_size
		st_atime
		st_mtime
		st_ctime
		st_atime_ns
		st_mtime_ns
		st_ctime_ns
		st_blocks
		st_blksize
		st_rdev
		st_flags
		st_gen
		st_birthtime
		st_fstype
		st_rsize
		st_creator
		st_type
		st_file_attributes
		st_reparse_tag
	statvfs(path)
	supports_dir_fd
	supports_effective_ids
	supports_fd
	supports_follow_symlinks
	symlink(src, dst, target_is_directory=False, *, dir_fd=None)
	sync()
	truncate(path, length)
	unlink(path, *, dir_fd=None)
	utime(path, times=None, *, [ns, ]dir_fd=None, follow_symlinks=True)
	walk(top, topdown=True, onerror=None, followlinks=False)
	fwalk(top='.', topdown=True, onerror=None, *, follow_symlinks=False, dir_fd=None)
	memfd_create(name[, flags=os.MFD_CLOEXEC])
	MFD_CLOEXEC
	MFD_ALLOW_SEALING
	MFD_HUGETLB
	MFD_HUGE_SHIFT
	MFD_HUGE_MASK
	MFD_HUGE_64KB
	MFD_HUGE_512KB
	MFD_HUGE_1MB
	MFD_HUGE_2MB
	MFD_HUGE_8MB
	MFD_HUGE_16MB
	MFD_HUGE_32MB
	MFD_HUGE_256MB
	MFD_HUGE_512MB
	MFD_HUGE_1GB
	MFD_HUGE_2GB
	MFD_HUGE_16GB
	eventfd(initval[, flags=os.EFD_CLOEXEC])
	eventfd_read(fd)
	eventfd_write(fd, value)
	EFD_CLOEXEC
	EFD_NONBLOCK
	EFD_SEMAPHORE
	# Linux extended attributes
	getxattr(path, attribute, *, follow_symlinks=True)
	listxattr(path=None, *, follow_symlinks=True)
	removexattr(path, attribute, *, follow_symlinks=True)
	setxattr(path, attribute, value, flags=0, *, follow_symlinks=True)
	XATTR_SIZE_MAX
	XATTR_CREATE
	XATTR_REPLACE
	# Process Management
	abort()
	add_dll_directory(path)
	execl(path, arg0, arg1, ...)
	execle(path, arg0, arg1, ..., env)
	execlp(file, arg0, arg1, ...)
	execlpe(file, arg0, arg1, ..., env)
	execv(path, args)
	execve(path, args, env)
	execvp(file, args)
	execvpe(file, args, env)
	_exit(n)
	EX_OK
	EX_USAGE
	EX_DATAERR
	EX_NOINPUT
	EX_NOUSER
	EX_NOHOST
	EX_UNAVAILABLE
	EX_SOFTWARE
	EX_OSERR
	EX_OSFILE
	EX_CANTCREAT
	EX_IOERR
	EX_TEMPFAIL
	EX_PROTOCOL
	EX_NOPERM
	EX_CONFIG
	EX_NOTFOUND
	fork()
	forkpty()
	kill(pid, sig)
	killpg(pgid, sig)
	nice(increment)
	pidfd_open(pid, flags=0)
	plock(op)
	popen(cmd, mode='r', buffering=- 1)
	posix_spawn(path, argv, env, *, file_actions=None, setpgroup=None, resetids=False, setsid=False, setsigmask=(), setsigdef=(), scheduler=None)
	POSIX_SPAWN_OPEN
	POSIX_SPAWN_CLOSE
	POSIX_SPAWN_DUP2
	posix_spawnp(path, argv, env, *, file_actions=None, setpgroup=None, resetids=False, setsid=False, setsigmask=(), setsigdef=(), scheduler=None)
	register_at_fork(*, before=None, after_in_parent=None, after_in_child=None)
	spawnl(mode, path, ...)
	spawnle(mode, path, ..., env)
	spawnlp(mode, file, ...)
	spawnlpe(mode, file, ..., env)
	spawnv(mode, path, args)
	spawnve(mode, path, args, env)
	spawnvp(mode, file, args)
	spawnvpe(mode, file, args, env)
	P_NOWAIT
	P_NOWAITO
	P_WAIT
	P_DETACH
	P_OVERLAY
	startfile(path[, operation][, arguments][, cwd][, show_cmd])
	system(command)
	times():
		user
		system
		children_user
		children_system
		elapsed
	wait()
	waitid(idtype, id, options)
	P_PID
	P_PGID
	P_ALL
	P_PIDFD
	WEXITED
	WSTOPPED
	WNOWAIT
	CLD_EXITED
	CLD_KILLED
	CLD_DUMPED
	CLD_TRAPPED
	CLD_STOPPED
	CLD_CONTINUED
	waitpid(pid, options)
	wait3(options)
	wait4(pid, options)
	waitstatus_to_exitcode(status)
	WNOHANG
	WCONTINUED
	WUNTRACED
	WCOREDUMP(status)
	WIFCONTINUED(status)
	WIFSTOPPED(status)
	WIFSIGNALED(status)
	WIFEXITED(status)
	WEXITSTATUS(status)
	WSTOPSIG(status)
	WTERMSIG(status)
	# Interface to the scheduler
	SCHED_OTHER
	SCHED_BATCH
	SCHED_IDLE
	SCHED_SPORADIC
	SCHED_FIFO
	SCHED_RR
	SCHED_RESET_ON_FORK
	class sched_param(sched_priority)
	sched_get_priority_min(policy)
	sched_get_priority_max(policy)
	sched_setscheduler(pid, policy, param)
	sched_getscheduler(pid)
	sched_setparam(pid, param)
	sched_getparam(pid)
	sched_rr_get_interval(pid)
	sched_yield()
	sched_setaffinity(pid, mask)
	sched_getaffinity(pid)
	# Miscellaneous System Information
	confstr(name)
	confstr_names
	cpu_count()
	getloadavg()
	sysconf(name)
	sysconf_names
	curdir
	pardir
	sep
	altsep
	pathsep
	defpath
	linesep
	devnull
	RTLD_LAZY
	RTLD_NOW
	RTLD_GLOBAL
	RTLD_LOCAL
	RTLD_NODELETE
	RTLD_NOLOAD
	RTLD_DEEPBIND
	# Random numbers
	getrandom(size, flags=0)
	urandom(size)
	GRND_NONBLOCK
	GRND_RANDOM
io
	DEFAULT_BUFFER_SIZE
	open(file, mode='r', buffering=- 1, encoding=None, errors=None, newline=None, closefd=True, opener=None)
	open_code(path)
	text_encoding(encoding, stacklevel=2)
	exception BlockingIOError
	exception UnsupportedOperation
	class IOBase:
		close()
		closed
		fileno()
		flush()
		isatty()
		readable()
		readline(size=- 1)
		readlines(hint=- 1)
		seek(offset, whence=SEEK_SET)
		seekable()
		tell()
		truncate(size=None)
		writable()
		writelines(lines)
	class RawIOBase:
		read(size=- 1)
		readall()
		readinto(b)
		write(b)
	class BufferedIOBase:
		raw
		detach()
		read(size=- 1)
		read1([size])
		readinto(b)
		readinto1(b)
		write(b)
	class FileIO(name, mode='r', closefd=True, opener=None):
		mode
		name
	class BytesIO([initial_bytes]):
		getbuffer()
		getvalue()
		read1([size])
		readinto1(b)
	class BufferedReader(raw, buffer_size=DEFAULT_BUFFER_SIZE):
		peek([size])
		read([size])
		read1([size])
	class BufferedWriter(raw, buffer_size=DEFAULT_BUFFER_SIZE):
		flush()
		write(b)
	class BufferedRandom(raw, buffer_size=DEFAULT_BUFFER_SIZE)
	class BufferedRWPair(reader, writer, buffer_size=DEFAULT_BUFFER_SIZE)
	class TextIOBase:
		encoding
		errors
		newlines
		buffer
		detach()
		read(size=- 1)
		readline(size=- 1)
		seek(offset, whence=SEEK_SET)
		tell()
		write(s)
	class TextIOWrapper(buffer, encoding=None, errors=None, newline=None, line_buffering=False, write_through=False):
		line_buffering
		write_through
		reconfigure(*[, encoding][, errors][, newline][, line_buffering][, write_through])
	class StringIO(initial_value='', newline='\n'):
	 	getvalue()
	class IncrementalNewlineDecoder
time
	asctime([t])
	pthread_getcpuclockid(thread_id)
	clock_getres(clk_id)
	clock_gettime(clk_id)
	clock_gettime_ns(clk_id)
	clock_settime(clk_id, time: float)
	clock_settime_ns(clk_id, time: int)
	ctime([secs])
	get_clock_info(name)
	gmtime([secs])
	localtime([secs])
	mktime(t)
	monotonic()
	monotonic_ns()
	perf_counter()
	perf_counter_ns()
	process_time()
	process_time_ns()
	sleep(secs)
	strftime(format[, t])
	strptime(string[, format])
	class struct_time:
		tm_year
		tm_mon
		tm_mday
		tm_hour
		tm_min
		tm_sec
		tm_wday
		tm_yday
		tm_isdst
		tm_zone
		tm_gmtoff
	time()
	time_ns()
	thread_time()
	thread_time_ns()
	tzset()
	CLOCK_BOOTTIME
	CLOCK_HIGHRES
	CLOCK_MONOTONIC
	CLOCK_MONOTONIC_RAW
	CLOCK_PROCESS_CPUTIME_ID
	CLOCK_PROF
	CLOCK_TAI
	CLOCK_THREAD_CPUTIME_ID
	CLOCK_UPTIME
	CLOCK_UPTIME_RAW
	CLOCK_REALTIME
	altzone
	daylight
	timezone
	tzname
argparse:
	class ArgumentParser(prog=None, usage=None, description=None, epilog=None, parents=[], formatter_class=argparse.HelpFormatter, prefix_chars='-', fromfile_prefix_chars=None, argument_default=None, conflict_handler='error', add_help=True, allow_abbrev=True, exit_on_error=True):
		add_argument(name or flags...[, action][, nargs][, const][, default][, type][, choices][, required][, help][, metavar][, dest])
		parse_args(args=None, namespace=None)
		add_subparsers([title][, description][, prog][, parser_class][, action][, option_string][, dest][, required][, help][, metavar])
		add_argument_group(title=None, description=None)
		add_mutually_exclusive_group(required=False)
		set_defaults(**kwargs)
		get_default(dest)
		print_usage(file=None)
		print_help(file=None)
		format_usage()
		format_help()
		parse_known_args(args=None, namespace=None)
		convert_arg_line_to_args(arg_line)
		exit(status=0, message=None)
		error(message)
		parse_intermixed_args(args=None, namespace=None)
		parse_known_intermixed_args(args=None, namespace=None)
	class Action(option_strings, dest, nargs=None, const=None, default=None, type=None, choices=None, required=False, help=None, metavar=None)
	class FileType(mode='r', bufsize=- 1, encoding=None, errors=None)
logging
	class Logger:
		propagate
		setLevel(level)
		isEnabledFor(level)
		getEffectiveLevel()
		getChild(suffix)
		debug(msg, *args, **kwargs)
		info(msg, *args, **kwargs)
		warning(msg, *args, **kwargs)
		error(msg, *args, **kwargs)
		critical(msg, *args, **kwargs)
		log(level, msg, *args, **kwargs)
		exception(msg, *args, **kwargs)
		addFilter(filter)
		removeFilter(filter)
		filter(record)
		addHandler(hdlr)
		removeHandler(hdlr)
		findCaller(stack_info=False, stacklevel=1)
		handle(record)
		makeRecord(name, level, fn, lno, msg, args, exc_info, func=None, extra=None, sinfo=None)
		hasHandlers()
	CRITICAL
	ERROR
	WARNING
	INFO
	DEBUG
	NOTSET
	class Handler:
		__init__(level=NOTSET)
		createLock()
		acquire()
		release()
		setLevel(level)
		setFormatter(fmt)
		addFilter(filter)
		removeFilter(filter)
		filter(record)
		flush()
		close()
		handle(record)
		handleError(record)
		format(record)
		emit(record)
	class Formatter(fmt=None, datefmt=None, style='%', validate=True, *, defaults=None):
		format(record)
		formatTime(record, datefmt=None)
		formatException(exc_info)
		formatStack(stack_info)
	class Filter(name=''):
		filter(record)
	class LogRecord(name, level, pathname, lineno, msg, args, exc_info, func=None, sinfo=None):
		getMessage()
	class LoggerAdapter(logger, extra)
		process(msg, kwargs)
	getLogger(name=None)
	getLoggerClass()
	getLogRecordFactory()
	debug(msg, *args, **kwargs)
	info(msg, *args, **kwargs)
	warning(msg, *args, **kwargs)
	error(msg, *args, **kwargs)
	critical(msg, *args, **kwargs)
	exception(msg, *args, **kwargs)
	log(level, msg, *args, **kwargs)
	disable(level=CRITICAL)
	addLevelName(level, levelName)
	getLevelName(level)
	makeLogRecord(attrdict)
	basicConfig(**kwargs)
		filename
		filemode
		format
		datefmt
		style
		level
		stream
		force
		errors
	shutdown()
	setLoggerClass(klass)
	setLogRecordFactory(factory)
	lastResort
	captureWarnings(capture)

# Concurrent Execution
# https://docs.python.org/3/library/concurrency.html
subprocess
	run(args, *, stdin=None, input=None, stdout=None, stderr=None, capture_output=False, shell=False, cwd=None, timeout=None, check=False, encoding=None, errors=None, text=None, env=None, universal_newlines=None, **other_popen_kwargs)
	class CompletedProcess:
		args
		returncode
		stdout
		stderr
		check_returncode()
	DEVNULL
	PIPE
	STDOUT
	exception SubprocessError
	exception TimeoutExpired:
		cmd
		timeout
		output
		stdout
		stderr
	exception CalledProcessError
		returncode
		cmd
		timeout
		output
		stdout
		stderr
	class Popen(args, bufsize=- 1, executable=None, stdin=None, stdout=None, stderr=None, preexec_fn=None, close_fds=True, shell=False, cwd=None, env=None, universal_newlines=None, startupinfo=None, creationflags=0, restore_signals=True, start_new_session=False, pass_fds=(), *, group=None, extra_groups=None, user=None, umask=- 1, encoding=None, errors=None, text=None, pipesize=- 1):
		poll()
		wait(timeout=None)
		communicate(input=None, timeout=None)
		send_signal(signal)
		terminate()
		kill()
		args
		stdin
		stdout
		stderr
		pid
		returncode
contextvars
	 class ContextVar(name[, *, default]):
	 	name
	 	get([default])
	 	set(value)
	 	reset(token)
	class Token:
		var
	 	old_value
	 	MISSING
	copy_context()
	class Context:
	 	run(callable, *args, **kwargs)
	 	copy()
	 	get(var[, default])

# Networking and Interprocess Communication
# https://docs.python.org/3/library/ipc.html
asyncio
	# Runners
	run(coro, *, debug=False)
	class Runner(*, debug=None, loop_factory=None):
		run(coro, *, context=None)
		close()
		get_loop()
	# Coroutines and Tasks
	create_task(coro, *, name=None)
	class TaskGroup:
		create_task(coro, *, name=None, context=None)
	sleep(delay, result=None)
	timeout(delay)
	class Timeout:
		when()
		reschedule(when: float | None)
		expired()
	timeout_at(when)
	wait_for(aw, timeout)
	wait(aws, *, timeout=None, return_when=ALL_COMPLETED)
	as_completed(aws, *, timeout=None)
	to_thread(func, /, *args, **kwargs)
	run_coroutine_threadsafe(coro, loop)
	current_task(loop=None)
	all_tasks(loop=None)
	class Task(coro, *, loop=None, name=None):
		cancel(msg=None)
		cancelled()
		done()
		result()
		exception()
		add_done_callback(callback, *, context=None)
		remove_done_callback(callback)
		get_stack(*, limit=None)
		print_stack(*, limit=None, file=None)
		get_coro()
		get_name()
		set_name(value)
	# Streams
	open_connection(host=None, port=None, *, limit=None, ssl=None, family=0, proto=0, flags=0, sock=None, local_addr=None, server_hostname=None, ssl_handshake_timeout=None)
	start_server(client_connected_cb, host=None, port=None, *, limit=None, family=socket.AF_UNSPEC, flags=socket.AI_PASSIVE, sock=None, backlog=100, ssl=None, reuse_address=None, reuse_port=None, ssl_handshake_timeout=None, start_serving=True)
	open_unix_connection(path=None, *, limit=None, ssl=None, sock=None, server_hostname=None, ssl_handshake_timeout=None)
	start_unix_server(client_connected_cb, path=None, *, limit=None, sock=None, backlog=100, ssl=None, ssl_handshake_timeout=None, start_serving=True)
	class StreamReader:
		read(n=- 1)
		readline()
		readexactly(n)
		readuntil(separator=b'\n')
		at_eof()
	class StreamWriter
		write(data)
		writelines(data)
		close()
		can_write_eof()
		write_eof()
		transport
		get_extra_info(name, default=None)
		drain()
		start_tls(sslcontext, \*, server_hostname=None, ssl_handshake_timeout=None)
		is_closing()
		wait_closed()
	# Synchronization Primitives
	class Lock:
		acquire()
		release()
		locked()
	class Event:
		wait()
		set()
		clear()
		is_set()
	class Condition(lock=None):
		acquire()
		notify(n=1)
		locked()
		notify_all()
		release()
		wait()
		wait_for(predicate)
	class Semaphore(value=1):
		acquire()
		locked()
		release()
	class BoundedSemaphore(value=1)
	class Barrier(parties):
		wait()
		reset()
		abort()
		parties
		n_waiting
		broken
	exception BrokenBarrierError
	# Subprocesses
	create_subprocess_exec(program, *args, stdin=None, stdout=None, stderr=None, limit=None, **kwds)
	create_subprocess_shell(cmd, stdin=None, stdout=None, stderr=None, limit=None, **kwds)
	PIPE
	STDOUT
	DEVNULL
	class Process:
		wait()
		communicate(input=None)
		send_signal(signal)
		terminate()
		kill()
		stdin
		stdout
		stderr
		pid
		returncode
	# Queues
	class Queue(maxsize=0):
		maxsize
		empty()
		full()
		get()
		get_nowait()
		join()
		put(item)
		put_nowait(item)
		qsize()
		task_done()
	class PriorityQueue
	class LifoQueue
	exception QueueEmpty
	exception QueueFull
	# Exceptions
	exception TimeoutError
	exception CancelledError
	exception InvalidStateError
	exception SendfileNotAvailableError
	exception IncompleteReadError:
		expected
		partial
	exception LimitOverrunError:
		consumed
	# Event Loop
	# Futures
	# Transports and Protocols
	# Policies

# Internet Data Handling
# https://docs.python.org/3/library/netdata.html
json
	dump(obj, fp, *, skipkeys=False, ensure_ascii=True, check_circular=True, allow_nan=True, cls=None, indent=None, separators=None, default=None, sort_keys=False, **kw)
	dumps(obj, *, skipkeys=False, ensure_ascii=True, check_circular=True, allow_nan=True, cls=None, indent=None, separators=None, default=None, sort_keys=False, **kw)
	load(fp, *, cls=None, object_hook=None, parse_float=None, parse_int=None, parse_constant=None, object_pairs_hook=None, **kw)
	loads(s, *, cls=None, object_hook=None, parse_float=None, parse_int=None, parse_constant=None, object_pairs_hook=None, **kw)
	class JSONDecoder(*, object_hook=None, parse_float=None, parse_int=None, parse_constant=None, strict=True, object_pairs_hook=None):
		decode(s)
		raw_decode(s)
	class JSONEncoder(*, skipkeys=False, ensure_ascii=True, check_circular=True, allow_nan=True, sort_keys=False, indent=None, separators=None, default=None):
		default(o)
		encode(o)
		iterencode(o)
	exception JSONDecodeError(msg, doc, pos):
		msg
		doc
		pos
		lineno
		colno
base64
	b64encode(s, altchars=None)
	b64decode(s, altchars=None, validate=False)
	standard_b64encode(s)
	standard_b64decode(s)
	urlsafe_b64encode(s)
	urlsafe_b64decode(s)
	b32encode(s)
	b32decode(s, casefold=False, map01=None)
	b32hexencode(s)
	b32hexdecode(s, casefold=False)
	b16encode(s)
	b16decode(s, casefold=False)
	a85encode(b, *, foldspaces=False, wrapcol=0, pad=False, adobe=False)
	a85decode(b, *, foldspaces=False, adobe=False, ignorechars=b' \t\n\r\x0b')
	b85encode(b, pad=False)
	b85decode(b)

# Internet Protocols and Support
# https://docs.python.org/3/library/internet.html
urllib.parse
	urlparse(urlstring, scheme='', allow_fragments=True)
		scheme
		netloc
		path
		params
		query
		fragment
		username
		password
		hostname
		port
	parse_qs(qs, keep_blank_values=False, strict_parsing=False, encoding='utf-8', errors='replace', max_num_fields=None, separator='&')
	parse_qsl(qs, keep_blank_values=False, strict_parsing=False, encoding='utf-8', errors='replace', max_num_fields=None, separator='&')
	urlunparse(parts)
	urlsplit(urlstring, scheme='', allow_fragments=True)
	urlunsplit(parts)
	urljoin(base, url, allow_fragments=True)
	urldefrag(url)
	unwrap(url)
	class DefragResult(url, fragment)
	class ParseResult(scheme, netloc, path, params, query, fragment)
	class SplitResult(scheme, netloc, path, query, fragment):
		geturl()
	class DefragResultBytes(url, fragment)
	class ParseResultBytes(scheme, netloc, path, params, query, fragment)
	class SplitResultBytes(scheme, netloc, path, query, fragment)
	quote(string, safe='/', encoding=None, errors=None)
	quote_plus(string, safe='', encoding=None, errors=None)
	quote_from_bytes(bytes, safe='/')
	unquote(string, encoding='utf-8', errors='replace')
	unquote_plus(string, encoding='utf-8', errors='replace')
	unquote_to_bytes(string)
	urlencode(query, doseq=False, safe='', encoding=None, errors=None, quote_via=quote_plus)
urllib.error
	exception URLError
		reason
	exception HTTPError
		code
		reason
		headers
	exception ContentTooShortError(msg, content)
uuid
	class SafeUUID:
		safe
		unsafe
		unknown
	class UUID(hex=None, bytes=None, bytes_le=None, fields=None, int=None, version=None, *, is_safe=SafeUUID.unknown):
		bytes
		bytes_le
		fields
		hex
		int
		urn
		variant
		version
		is_safe
	getnode()
	uuid1(node=None, clock_seq=None)
	uuid3(namespace, name)
	uuid4()
	uuid5(namespace, name)
	NAMESPACE_DNS
	NAMESPACE_URL
	NAMESPACE_OID
	NAMESPACE_X500

# Python Runtime Services
# https://docs.python.org/3/library/python.html
sys
	abiflags
	addaudithook(hook)
	argv
	audit(event, *args)
	base_exec_prefix
	base_prefix
	byteorder
	builtin_module_names
	call_tracing(func, args)
	copyright
	_clear_type_cache()
	_current_frames()
	_current_exceptions()
	breakpointhook()
	_debugmallocstats()
	dllhandle
	displayhook(value)
	dont_write_bytecode
	pycache_prefix
	excepthook(type, value, traceback)
	__breakpointhook__
	__displayhook__
	__excepthook__
	__unraisablehook__
	exc_info()
	exec_prefix
	executable
	exit([arg])
	flags
	float_info
	float_repr_style
	getallocatedblocks()
	getandroidapilevel()
	getdefaultencoding()
	getdlopenflags()
	getfilesystemencoding()
	getfilesystemencodeerrors()
	get_int_max_str_digits()
	getrefcount(object)
	getrecursionlimit()
	getsizeof(object[, default])
	getswitchinterval()
	_getframe([depth])
	getprofile()
	gettrace()
	getwindowsversion()
	get_asyncgen_hooks()
	get_coroutine_origin_tracking_depth()
	hash_info
	hexversion
	implementation
	int_info
	__interactivehook__
	intern(string)
	is_finalizing()
	last_type
	last_value
	last_traceback
	maxsize
	maxunicode
	meta_path
	modules
	orig_argv
	path
	path_hooks
	path_importer_cache
	platform
	platlibdir
	prefix
	ps1
	ps2
	setdlopenflags(n)
	set_int_max_str_digits(n)
	setprofile(profilefunc)
	setrecursionlimit(limit)
	setswitchinterval(interval)
	settrace(tracefunc)
	set_asyncgen_hooks(firstiter, finalizer)
	set_coroutine_origin_tracking_depth(depth)
	_enablelegacywindowsfsencoding()
	stdin
	stdout
	stderr
	__stdin__
	__stdout__
	__stderr__
	stdlib_module_names
	thread_info
	tracebacklimit
	unraisablehook(unraisable, /)
	version
	api_version
	version_info
	warnoptions
	winver
	_xoptions
warnings
	warn(message, category=None, stacklevel=1, source=None)
	warn_explicit(message, category, filename, lineno, module=None, registry=None, module_globals=None, source=None)
	showwarning(message, category, filename, lineno, file=None, line=None)
	formatwarning(message, category, filename, lineno, line=None)
	filterwarnings(action, message='', category=Warning, module='', lineno=0, append=False)
	simplefilter(action, category=Warning, lineno=0, append=False)
	resetwarnings()
	class catch_warnings(*, record=False, module=None, action=None, category=Warning, lineno=0, append=False)
dataclasses
	@dataclassdataclass(*, init=True, repr=True, eq=True, order=False, unsafe_hash=False, frozen=False, match_args=True, kw_only=False, slots=False, weakref_slot=False)
	field(*, default=MISSING, default_factory=MISSING, init=True, repr=True, hash=None, compare=True, metadata=None, kw_only=MISSING)
	class Field
	fields(class_or_instance)
	asdict(instance, *, dict_factory=dict)
	astuple(instance, *, tuple_factory=tuple)
	make_dataclass(cls_name, fields, *, bases=(), namespace=None, init=True, repr=True, eq=True, order=False, unsafe_hash=False, frozen=False, match_args=True, kw_only=False, slots=False, weakref_slot=False)
	replace(instance, /, **changes)
	is_dataclass(class_or_instance)
	MISSING
	KW_ONLY
	exception FrozenInstanceError
contextlib
	class AbstractContextManager
	class AbstractAsyncContextManager
	@contextmanager:
		__enter__(self)
		__exit__(self, exc_type, exc_value, traceback)
	@asynccontextmanager
	closing(thing)
	aclosing(thing)
	nullcontext(enter_result=None)
	suppress(*exceptions)
	redirect_stdout(new_target)
	redirect_stderr(new_target)
	chdir(path)
	class ContextDecorator
	class AsyncContextDecorator
	class ExitStack:
		enter_context(cm)
		push(exit)
		callback(callback, /, *args, **kwds)
		pop_all()
		close()
	class AsyncExitStack:
		enter_async_context(cm)
		push_async_exit(exit)
		push_async_callback(callback, /, *args, **kwds)
		aclose()
abc
	class ABC
	class ABCMeta
		register(subclass)
		__subclasshook__(subclass)
	@abstractmethod
	@abstractclassmethod
	@abstractstaticmethod
	@abstractproperty
	get_cache_token()
	update_abstractmethods(cls)
atexit
	@register
	register(func, *args, **kwargs)
	unregister(func)
traceback
	print_tb(tb, limit=None, file=None)
	print_exception(exc, /, [value, tb, ]limit=None, file=None, chain=True)
	print_exc(limit=None, file=None, chain=True)
	print_last(limit=None, file=None, chain=True)
	print_stack(f=None, limit=None, file=None)
	extract_tb(tb, limit=None)
	extract_stack(f=None, limit=None)
	format_list(extracted_list)
	format_exception_only(exc, /[, value])
	format_exception(exc, /, [value, tb, ]limit=None, chain=True)
	format_exc(limit=None, chain=True)
	format_tb(tb, limit=None)
	format_stack(f=None, limit=None)
	clear_frames(tb)
	walk_stack(f)
	walk_tb(tb)
	class TracebackException(exc_type, exc_value, exc_traceback, *, limit=None, lookup_lines=True, capture_locals=False, compact=False):
		__cause__
		__context__
		__suppress_context__
		__notes__
		stack
		exc_type
		filename
		lineno
		text
		offset
		msg
		from_exception(exc, *, limit=None, lookup_lines=True, capture_locals=False)
		format(*, chain=True)
		format_exception_only()
	class StackSummary:
		extract(frame_gen, *, limit=None, lookup_lines=True, capture_locals=False)
		from_list(a_list)
		format()
	class FrameSummary(filename, lineno, name, lookup_line=True, locals=None, line=None)
__future__
	nested_scopes
	generators
	division
	absolute_import
	with_statement
	print_function
	unicode_literals
	generator_stop
	annotations

#! comment			===========================================================
# https://devtut.github.io/python/comments-and-documentation.html#syntax-conventions
# https://realpython.com/documenting-python-code/#docstring-formats
"""
:param name:
:type name:
:return:
:returns:
:rtype:
"""
