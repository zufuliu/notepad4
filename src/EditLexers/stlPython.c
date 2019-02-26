#include "EditLexer.h"
#include "EditStyle.h"

// https://www.python.org/
// http://ironpython.net/
// http://cobra-language.com/
// http://boo-lang.org/
// http://www.jython.org/

static KEYWORDLIST Keywords_Python = {{
"async await " // python 3.7
"and as assert break class continue def del elif else except finally for from "
"global if import in is lambda nonlocal not or pass raise return try while with yield "
// python 2
"exec print "

, // 1 Type
"int long float bool complex list tuple range str bytes bytearray memoryview set frozenset dict contextmanager "
"type object unicode buffer file xrange slice "

, // 2 Built-in Constant
"self None True False NotImplemented Ellipsis __debug__ __main__ __builtin__ __future__ "

, // 3 @Decorator
"classmethod staticmethod cached_method abstractmethod property contextmanager "

, // 4 Built-in Function
"__import__() abs() all() any() ascii() bin() bool() breakpoint() bytearray() bytes() callable() chr() classmethod() "
"compile() complex() delattr() dict() dir() divmod() enumerate() eval() exec() filter() float() format() "
"frozenset() getattr() globals() hasattr() hash() help() hex() id() input() int() isinstance() issubclass() "
"iter() len() list() locals() map() max() memoryview() min() next() object() oct() open() ord() pow() print() "
"property() range() repr() reversed() round() set() setattr() slice() sorted() staticmethod() str() sum() "
"super() tuple() type() vars() zip() "
"quit() exit() mro() abstractmethod() "
// python 2
"basestring() cmp() execfile() file() long() raw_input() reduce() reload() unichr() unicode() xrange() "
"apply() buffer() coerce() intern() "

, // 5 Attribute
"__doc__ __name__ __qualname__ __module__ __defaults__ __code__ __globals__ "
"__dict__ __closure__ __annotations__ __kwdefaults__ __self__ __func__ "
"__class__ __bases__ __metaclass__ __subclasses__ __loader__ __package__ "
"__path__ __file__ __all__ __version__ __slots__ "
// other
"__docformat__ __about__ __author__ __date__ __traceback__ __cause__ __context__ __suppress_context__ __weakref__ "

, // 6 Object Method
"__new__ __init__ __del__ __repr__ __str__ __bytes__ __format__ __lt__ __le__ "
"__eq__ __ne__ __gt__ __ge__ __hash__ __bool__ __getattr__ __getattribute__ "
"__setattr__ __delattr__ __dir__ __get__ __set__ __delete__ __prepare__ "
"__instancecheck__ __subclasscheck__ __call__ __len__ __length_hint__ __getitem__ "
"__setitem__ __delitem__ __iter__ __reversed__ __contains__ __add__ __sub__ __mul__ "
"__truediv__ __floordiv__ __mod__ __divmod__ __pow__ __lshift__ __rshift__ __and__ "
"__xor__ __or__ __radd__ __rsub__ __rmul__ __rtruediv__ __rfloordiv__ __rmod__ "
"__rdivmod__ __rpow__ __rlshift__ __rrshift__ __rand__ __rxor__ __ror__ __iadd__ "
"__isub__ __imul__ __itruediv__ __ifloordiv__ __imod__ __ipow__ __ilshift__ "
"__irshift__ __iand__ __ixor__ __ior__ __neg__ __pos__ __abs__ __invert__ __complex__ "
"__int__ __float__ __round__ __index__ __enter__ __exit__ "
"__next__ __subclasshook__ __mro__ "
// python 2
"__cmp__ __rcmp__ __nonzero__ __unicode__ __getslice__ __setslice__ __delslice__ "
"__div__ __rdiv__ __idiv__ __long__ __oct__ __hex__ __coerce__ __reduce__ "
// other
"__trunc__ __getstate__ __setstate__ __copy__ __deepcopy__ __getinitargs__ "

, // 7 Class
"Complex Real Rational Integral " "Fraction " "Decimal BasicContext ExtendedContext DefaultContext Context Clamped DecimalException DivisionByZero Inexact InvalidOperation Overflow Rounded Subnormal Underflow FloatOperation " "SystemRandom "
// exception
"BaseException SystemExit KeyboardInterrupt GeneratorExit Exception StopIteration ArithmeticError FloatingPointError OverflowError ZeroDivisionError AssertionError AttributeError BufferError EOFError ImportError LookupError IndexError KeyError MemoryError NameError UnboundLocalError OSError BlockingIOError ChildProcessError ConnectionError BrokenPipeError ConnectionAbortedError ConnectionRefusedError ConnectionResetError FileExistsError FileNotFoundError InterruptedError IsADirectoryError NotADirectoryError PermissionError ProcessLookupError TimeoutError ReferenceError RuntimeError NotImplementedError SyntaxError IndentationError TabError SystemError TypeError ValueError UnicodeError UnicodeDecodeError UnicodeEncodeError UnicodeTranslateError Warning DeprecationWarning PendingDeprecationWarning RuntimeWarning SyntaxWarning UserWarning FutureWarning ImportWarning UnicodeWarning BytesWarning ResourceWarning "
"terminal_size sched_param "
"BlockingIOError UnsupportedOperation IOBase RawIOBase BufferedIOBase FileIO BytesIO BufferedReader BufferedWriter BufferedRandom BufferedRWPair TextIOBase TextIOWrapper StringIO IncrementalNewlineDecoder "
"Formatter Template " "TextWrapper "
"Struct " "Codec IncrementalEncoder IncrementalDecoder StreamWriter StreamReader StreamReaderWriter StreamRecoder "
// collections
"ChainMap Counter deque defaultdict OrderedDict UserDict UserList UserString "
"struct_time " "date time datetime timedelta tzinfo timezone "
"array " "Enum IntEnum " "catch_warnings "
"JSONDecoder JSONEncoder " "UUID "
// urllib
"URLError HTTPError OpenerDirector BaseHandler HTTPCookieProcessor HTTPPasswordMgr "
// httplib
"HTTPConnection HTTPSConnection HTTPResponse HTTPMessage HTTPException "
// cookielib
"CookieJar FileCookieJar MozillaCookieJar CookiePolicy DefaultCookiePolicy Cookie "
// logging
"Logger Handler Filter LogRecord LoggerAdapter "
// argparse
"ArgumentParser "
// sqlite3
"Connection Cursor "
// mysql.connector
"MySQLConnection CursorBase MySQLCursor MySQLCursorRaw MySQLCursorBuffered MySQLCursorBufferedRaw "
// gzip
"GzipFile "

, // 8 Site
"copyright license credits __copyright__ __license__ __credits__ "

, // 9 Module
"__builtin__ builtins __future__ "
// future
"nested_scopes generators division absolute_import with_statement print_function unicode_literals generator_stop "
// modules
"numbers math cmath fractions decimal random "
"sys os io os.path glob shutil time calendar logging argparse traceback gc "
"string re textwrap unicodedata "
"struct codecs collections datetime array copy enum itertools operator warnings "
"json base64 email.utils "
"urllib urllib2 urlparse uuid httplib.client cookielib "
"wave sqlite3 mysql.connector gzip "

, // 10 Method
// int/float
"bit_length() to_bytes() from_bytes() " "as_integer_ratio() is_integer() hex() fromhex() "
// list
"index() count() append() clear() copy() extend() insert() pop() remove() reverse() sort() "
// bytes/bytearray/memoryview
"decode() fromhex() translate() maketrans() " "tobytes() tolist() release() cast() "
// dict
"iter() clear() copy() fromkeys() get() items() keys() pop() popitem() setdefault() update() values() "
// str
"capitalize() casefold() center() count() encode() endswith() expandtabs() find() format() format_map() index() isalnum() isalpha() isdecimal() isdigit() isidentifier() islower() isnumeric() isprintable() isspace() istitle() isupper() join() ljust() lower() lstrip() maketrans() partition() replace() rfind() rindex() rjust() rpartition() rsplit() rstrip() split() splitlines() startswith() strip() swapcase() title() translate() upper() zfill() "

// math/cmath
"ceil() copysign() fabs() factorial() floor() fmod() frexp() fsum() gcd() isclose() isfinite() isinf() isnan() ldexp() modf() trunc() exp() expm1() log() log1p() log2() log10() pow() sqrt() acos() asin() atan() atan2() cos() hypot() sin() tan() degrees() radians() acosh() asinh() atanh() cosh() sinh() tanh() erf() erfc() gamma() lgamma() " "conjugate() phase() polar() rect() "
// Fraction
"from_float() from_decimal() limit_denominator() "
// random
"seed() getstate() setstate() getrandbits() randrange() randint() choice() choices() shuffle() sample() random() uniform() triangular() betavariate() expovariate() gammavariate() gauss() lognormvariate() normalvariate() vonmisesvariate() paretovariate() weibullvariate() "
// decimal
"getcontext() setcontext() localcontext() " "adjusted() as_tuple() as_integer_ratio() canonical() compare() compare_signal() compare_total() compare_total_mag() conjugate() copy_abs() copy_negate() copy_sign() rom_float() fma() is_canonical() is_finite() is_infinite() is_nan() is_normal() is_qnan() is_signed() is_snan() is_subnormal() is_zero() ln() logb() logical_and() logical_invert() logical_or() logical_xor() max_mag() min_mag() next_minus() next_plus() next_toward() normalize() number_class() quantize() radix() remainder_near() rotate() same_quantum() scaleb() shift() to_eng_string() to_integral() to_integral_exact() to_integral_value() " "clear_flags() clear_traps() copy_decimal() create_decimal() create_decimal_from_float() Etiny() Etop() add() divide() divide_int() minus() multiply() plus() power() remainder() "
// sys
"_clear_type_cache() _current_frames() _debugmallocstats() excepthook() exc_info() getallocatedblocks() getcheckinterval() getdefaultencoding() getdlopenflags() getfilesystemencoding() getrefcount() getrecursionlimit() getsizeof() getswitchinterval() _getframe() getprofile() gettrace() getwindowsversion() setcheckinterval() setdlopenflags() setprofile() setrecursionlimit() setswitchinterval() settrace() settscdump() "
// os
"ctermid() chdir() fchdir() getcwd() fsencode() fsdecode() getenv() getenvb() get_exec_path() getegid() geteuid() geteuid() getgrouplist() getgroups() getlogin() getpgid() getpgrp() getpid() getppid() getpriority() getresuid() getresgid() getuid() initgroups() putenv() setegid() seteuid() setgid() setgroups() setpgrp() setpgid() setpriority() setregid() setresgid() setresuid() setreuid() getsid() setsid() setuid() strerror() umask() uname() unsetenv() fdopen() close() closerange() device_encoding() dup() dup2() fchmod() fchown() fdatasync() fpathconf() fstat() fstatvfs() fsync() ftruncate() isatty() lockf() lseek() openpty() pipe() pipe2() posix_fallocate() posix_fadvise() pread() pwrite() read() sendfile() readv() tcgetpgrp() tcsetpgrp() ttyname() write() writev() get_terminal_size() access() chdir() chflags() chmod() chown() chroot() fchdir() getcwd() getcwdb() lchflags() lchmod() lchown() link() listdir() lstat() mkdir() makedirs() mkfifo() mknod() major() minor() makedev() pathconf() readlink() remove() removedirs() rename() renames() replace() rmdir() stat() stat_float_times() statvfs() symlink() sync() truncate() unlink() utime() walk() fwalk() getxattr() listxattr() removexattr() setxattr() abort() execl() execle() execlp() execlpe() execv() execve() execvp() execvpe() _exit() fork() forkpty() kill() killpg() nice() plock() popen() spawnl() spawnle() spawnlp() spawnlpe() spawnv() spawnve() spawnvp() spawnvpe() startfile() system() times() wait() waitid() waitpid() wait3() wait4() WCOREDUMP() WIFCONTINUED() WIFSTOPPED() WIFSIGNALED() WIFEXITED() WEXITSTATUS() WSTOPSIG() WTERMSIG() sched_get_priority_min() sched_get_priority_max() sched_setscheduler() sched_getscheduler() sched_setparam() sched_getparam() sched_rr_get_interval() sched_yield() sched_setaffinity() sched_getaffinity() confstr() cpu_count() getloadavg() sysconf() urandom() "
"scandir() "
// io
"close() fileno() flush() isatty() readable() readline() readlines() seek() seekable() tell() truncate() writable() writelines() read() readall() readinto() write() detach() read1() getbuffer() getvalue() peek() "
// os.path/glob
"abspath() basename() commonprefix() dirname() exists() lexists() expanduser() expandvars() getatime() getmtime() getctime() getsize() isabs() isfile() isdir() islink() ismount() join() normcase() normpath() realpath() relpath() samefile() ameopenfile() samestat() split() splitdrive() splitext() splitunc() " "glob() iglob() "
// shutil
"copy() move() "
// time
"asctime() clock() clock_getres() clock_gettime() clock_settime() ctime() get_clock_info() gmtime() localtime() mktime() monotonic() perf_counter() process_time() sleep() strftime() strptime() time() tzset() "
// calendar
"timegm() "
// datetime
"today() fromtimestamp() fromordinal() replace() timetuple() toordinal() weekday() isoweekday() isocalendar() isoformat() ctime() strftime() utcoffset() dst() tzname() now() utcnow() utcfromtimestamp() combine() strptime() date() timetz() astimezone() utctimetuple() timestamp() soweekday() total_seconds() fromutc() "
// string
"capwords() " "format() vformat() parse() get_field() get_value() check_unused_args() format_field() convert_field() " "substitute() safe_substitute() "
// re
"search() match() split() findall() finditer() sub() subn() escape() purge() " "expand() group() groups() groupdict() start() end() span() "
// textwrap
"wrap() fill() dedent() indent() "
// unicodedata
"lookup() name() decimal() digit() numeric() category() bidirectional() combining() east_asian_width() mirrored() decomposition() normalize() "
// struct
"pack() pack_into() unpack() unpack_from() iter_unpack() calcsize() "
// codecs
"register() lookup() getencoder() getdecoder() getincrementalencoder() getincrementaldecoder() getreader() getwriter() register_error() lookup_error() strict_errors() replace_errors() ignore_errors() xmlcharrefreplace_errors() backslashreplace_errors() open() EncodedFile() iterencode() iterdecode() " "encode() decode() reset() getstate() setstate() write() writelines() read() readline() readlines() "
// collections
"namedtuple() new_child() elements() most_common() subtract() fromkeys() update() append() appendleft() clear() count() extend() extendleft() pop() popleft() remove() reverse() rotate() __missing__() popitem() move_to_end() "

// array
"append() buffer_info() byteswap() count() extend() frombytes() fromfile() fromlist() fromstring() fromunicode() index() insert() pop() remove() reverse() tobytes() tofile() tolist() tostring() tounicode() "
// copy
"copy() deepcopy() "
// itertools
"accumulate() chain() from_iterable() combinations() combinations_with_replacement() compress() count() cycle() dropwhile() filterfalse() groupby() islice() islice() permutations() product() repeat() starmap() takewhile() tee() zip_longest() "
// operator
"attrgetter() itemgetter() "
// warnings
"warn() warn_explicit() showwarning() formatwarning() filterwarnings() simplefilter() resetwarnings() "
// logging
"getLogger() getLoggerClass() getLogRecordFactory() debug() info() warning() error() critical() exception() log() disable() addLevelName() getLevelName() makeLogRecord() basicConfig() shutdown() setLoggerClass() setLogRecordFactory() captureWarnings() "
"setLevel() isEnabledFor() getEffectiveLevel() getChild() addFilter() removeFilter() addHandler() removeHandler() findCaller() handle() makeRecord() hasHandlers() "
"createLock() acquire() release() setFormatter() handleError() emit() "
"formatTime() formatException() formatStack() " "getMessage() " "process() "
// traceback
"format_exc() print_exc() "
// gc
"enable() disable() collect() "
// json
"dumps(ensure_ascii,sort_keys,indent,separators,encoding) load() "
"dump() loads() " "decode() raw_decode() " "default() encode() iterencode() "
// base64
"b64encode() b64decode() standard_b64encode() standard_b64decode() urlsafe_b64encode() urlsafe_b64decode() b32encode() b32decode() b16encode() b16decode() decode() decodebytes() decodestring() encode() encodebytes() encodestring() "
// urllib/urllib2
"quote() quote_plus() unquote() unquote_plus() urlencode() urlopen() info() getcode() install_opener() build_opener() "
"add_data() get_method() has_data() get_data() add_header() add_unredirected_header() has_header() get_full_url() get_type() get_host() get_selector() get_header() header_items() set_proxy() get_origin_req_host() is_unverifiable() "
"add_parent() default_open() protocol_open() unknown_open() http_error_default() http_error_nnn() protocol_request() protocol_response() "
// urlparse
"urlparse() parse_qs() parse_qsl() urlsplit() urljoin() geturl() urlunparse() urlunsplit() urldefrag() "
// httplib
"request() getresponse() set_debuglevel() set_tunnel() connect() putrequest() putheader() endheaders() send() "
"getheader() getheaders() fileno() "
// cookielib
"add_cookie_header() extract_cookies() set_policy() make_cookies() set_cookie_if_ok() set_cookie() clear() clear_session_cookies() "
"save() load() revert() set_ok() return_ok() domain_return_ok() path_return_ok() blocked_domains() set_blocked_domains() is_blocked() allowed_domains() set_allowed_domains() is_not_allowed() "
"blocked_domains() set_blocked_domains() is_blocked() allowed_domains() set_allowed_domains() is_not_allowed() has_nonstandard_attr() get_nonstandard_attr() set_nonstandard_attr() is_expired() "
// uuid
"getnode() uuid1() uuid3() uuid4() uuid5() "
// wave
"openfp() close() getnchannels() getsampwidth() getframerate() getnframes() getcomptype() getcompname() getparams() readframes() rewind() getmarkers() getmark() setpos() tell() setnchannels() setsampwidth() setframerate() etnframes() setcomptype() setparams() writeframesraw() writeframes() "
// argparse
"add_argument(action,const,default,help) parse_args() "
// sqlite3
"connect() register_converter() register_adapter() complete_statement() enable_callback_tracebacks() "
"cursor() commit() rollback() execute() executemany() executescript() create_function() create_aggregate() create_collation() interrupt() set_authorizer() set_progress_handler() set_trace_callback() enable_load_extension() load_extension() "
"fetchone() fetchmany() fetchall() "
// mysql.connector
"config() disconnect() get_rows() get_row() is_connected() reconnect() ping() set_converter_class() get_server_version() get_server_info() set_login() set_unicode() set_charset_collation() set_client_flags() isset_client_flag() set_database() get_database() set_time_zone() get_time_zone() set_sql_mode() get_sql_mode() set_autocommit() get_autocommit() "
"callproc() nextset() setinputsizes() setoutputsize() reset() stored_results() getlastrowid() fetchwarnings() "

, // 11 Constant
// math/decimal
"pi " "inf nan NaN Infinity " "MAX_PREC MAX_EMAX MIN_EMIN MIN_ETINY HAVE_THREADS ROUND_CEILING ROUND_DOWN ROUND_FLOOR ROUND_HALF_DOWN ROUND_HALF_EVEN ROUND_HALF_UP ROUND_UP ROUND_05UP "
// os
"PRIO_PROCESS PRIO_PGRP PRIO_USER F_LOCK F_TLOCK F_ULOCK F_TEST SEEK_SET SEEK_CUR SEEK_END O_RDONLY O_WRONLY O_RDWR O_APPEND O_CREAT O_EXCL O_TRUNC O_DSYNC O_RSYNC O_SYNC O_NDELAY O_NONBLOCK O_NOCTTY O_SHLOCK O_EXLOCK O_CLOEXEC O_BINARY O_NOINHERIT O_SHORT_LIVED O_TEMPORARY O_RANDOM O_SEQUENTIAL O_TEXT O_ASYNC O_DIRECT O_DIRECTORY O_NOFOLLOW O_NOATIME O_PATH POSIX_FADV_NORMAL POSIX_FADV_SEQUENTIAL POSIX_FADV_RANDOM POSIX_FADV_NOREUSE POSIX_FADV_WILLNEED POSIX_FADV_DONTNEED SF_NODISKIO SF_MNOWAIT SF_SYNC F_OK R_OK W_OK X_OK XATTR_SIZE_MAX XATTR_CREATE XATTR_REPLACE EX_OK EX_USAGE EX_DATAERR EX_NOINPUT EX_NOUSER EX_NOHOST EX_UNAVAILABLE EX_SOFTWARE EX_OSERR EX_OSFILE EX_CANTCREAT EX_IOERR EX_TEMPFAIL EX_PROTOCOL EX_NOPERM EX_CONFIG EX_NOTFOUND P_NOWAIT P_NOWAITO P_WAIT P_DETACH P_OVERLAY P_PID P_PGID P_ALL WEXITED WSTOPPED WNOWAIT CLD_EXITED CLD_DUMPED CLD_TRAPPED CLD_CONTINUED WNOHANG WCONTINUED WUNTRACED SCHED_OTHER SCHED_BATCH SCHED_IDLE SCHED_SPORADIC SCHED_FIFO SCHED_RR SCHED_RESET_ON_FORK RTLD_LAZY RTLD_NOW RTLD_GLOBAL RTLD_LOCAL RTLD_NODELETE RTLD_NOLOAD RTLD_DEEPBIND "
// io
"DEFAULT_BUFFER_SIZE "
// time
"CLOCK_HIGHRES CLOCK_MONOTONIC CLOCK_MONOTONIC_RAW CLOCK_PROCESS_CPUTIME_ID CLOCK_REALTIME CLOCK_THREAD_CPUTIME_ID "
// datetime
"MINYEAR MAXYEAR "
// string
"ascii_letters ascii_lowercase ascii_uppercase digits hexdigits octdigits punctuation printable whitespace "
// re
"ASCII DEBUG re.I IGNORECASE LOCALE MULTILINE DOTALL VERBOSE "
// codecs
"BOM BOM_BE BOM_LE BOM_UTF8 BOM_UTF16 BOM_UTF16_BE BOM_UTF16_LE BOM_UTF32 BOM_UTF32_BE BOM_UTF32_LE "
// uuid
"NAMESPACE_DNS NAMESPACE_URL NAMESPACE_OID NAMESPACE_X500 RESERVED_NCS RFC_4122 RESERVED_MICROSOFT RESERVED_FUTURE "
// sqlite3
"PARSE_DECLTYPES PARSE_COLNAMES "

, // 12 Attribute
// builtins

// numbers
"real imag " "numerator denominator "
// memoryview
"obj nbytes readonly format itemsize ndim strides suboffsets c_contiguous f_contiguous contiguous "
// sys
"abiflags sys.argv base_exec_prefix base_prefix byteorder builtin_module_names call_tracing copyright dllhandle displayhook dont_write_bytecode __displayhook__ __excepthook__ exec_prefix executable flags float_info float_repr_style hash_info hexversion implementation int_info __interactivehook__ last_type last_value last_traceback maxsize maxunicode meta_path modules path path_hooks path_importer_cache platform prefix ps1 ps2 stdin stdout stderr __stdin__ __stdout__ __stderr__ thread_info tracebacklimit version api_version version_info warnoptions winver _xoptions "
// os
"columns lines sched_priority "
"error name environ environb supports_bytes_environ pathconf_names supports_dir_fd supports_effective_ids supports_fd supports_follow_symlinks confstr_names sysconf_names curdir pardir sep altsep extsep pathsep defpath linesep devnull "
// io
"closed raw mode name encoding errors newlines buffer line_buffering "
// time
"altzone daylight timezone tzname "
// logging
"lastResort propagate "
// datetime
"min max resolution year month day hour minute second microsecond tzinfo utc "
// string
"template "
// re
"error " "flags groups groupindex pattern " "pos endpos lastindex lastgroup re string "
// textwrap
"width expand_tabs tabsize replace_whitespace drop_whitespace initial_indent subsequent_indent fix_sentence_endings break_long_words break_on_hyphens "
// unicodedata
"unidata_version ucd_3_2_0 "
// Struct
"error " " format size "
// collections
"maps parents default_factory data "
// array
"typecodes typecode itemsize "
// uuid
"bytes bytes_le fields hex int urn variant "
// urllib
"code reason parent cookiejar "
// httplib
"msg status "
// cookielib
"filename delayload netscape rfc2965 hide_cookie2 name value port path secure expires discard comment comment_url rfc2109 port_specified domain_specified domain_initial_dot rfc2109_as_netscape strict_domain strict_rfc2965_unverifiable strict_ns_unverifiable strict_ns_domain strict_ns_set_initial_dollar strict_ns_set_path "
// urlparse
"scheme netloc path query fragment username password hostname port "
// argparse

// sqlite3
"version_info sqlite_version sqlite_version_info " "isolation_level in_transaction row_factory text_factory total_changes iterdump " " rowcount lastrowid description "
// mysql.connector
"connection_id charset collation user server_host server_port unix_socket unread_result database time_zone sql_mode autocommit get_warnings raise_on_warnings "
"column_names statement with_rows "
"password port use_unicode connection_timeout client_flags buffered "
// gzip
"filename mode compresslevel fileobj "

, NULL, NULL, NULL
}};

static EDITSTYLE Styles_Python[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_PY_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"bold; fore:#FF8000" },
	{ SCE_PY_WORD2, NP2STYLE_Type, EDITSTYLE_HOLE(L"Type"), L"fore:#0080FF" },
	{ SCE_PY_CLASSNAME, NP2STYLE_Class, EDITSTYLE_HOLE(L"Class"), L"bold; fore:#007F7F" },
	{ SCE_PY_DEFNAME, NP2STYLE_FunctionDef, EDITSTYLE_HOLE(L"Function Define"), L"bold; fore:#0080C0" },
	{ SCE_PY_FUNCTION, NP2STYLE_Function, EDITSTYLE_HOLE(L"Function"), L"fore:#A46000" },
	{ SCE_PY_ATTR, NP2STYLE_Attribute, EDITSTYLE_HOLE(L"Attribute"), L"fore:#FF8000" },
	{ SCE_PY_DECORATOR, 63465, EDITSTYLE_HOLE(L"Decorator"), L"fore:#C65D09" },
	{ SCE_PY_BUILDIN_CONST, 63468, EDITSTYLE_HOLE(L"Build-in Constant"), L"bold; fore:#008080" },
	{ SCE_PY_BUILDIN_FUNC, 63466, EDITSTYLE_HOLE(L"Build-in Function"), L"fore:#0080C0" },
	{ SCE_PY_OBJ_FUNC, 63467, EDITSTYLE_HOLE(L"object Method"), L"fore:#0080C0" },
	{ MULTI_STYLE(SCE_PY_COMMENTLINE, SCE_PY_COMMENTBLOCK, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ MULTI_STYLE(SCE_PY_STRING1, SCE_PY_STRING2, 0, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ MULTI_STYLE(SCE_PY_RAW_STRING1, SCE_PY_RAW_STRING2, 0, 0), NP2STYLE_RawString, EDITSTYLE_HOLE(L"Raw String"), L"fore:#008080" },
	{ MULTI_STYLE(SCE_PY_TRIPLE_STRING1, SCE_PY_TRIPLE_STRING2, 0, 0), NP2STYLE_TripleString, EDITSTYLE_HOLE(L"Triple Quoted String"), L"fore:#F08000" },
	{ MULTI_STYLE(SCE_PY_BYTES1, SCE_PY_BYTES2, 0, 0), 63461, EDITSTYLE_HOLE(L"Bytes"), L"fore:#C08000" },
	{ MULTI_STYLE(SCE_PY_RAW_BYTES1, SCE_PY_RAW_BYTES2, 0, 0), 63462, EDITSTYLE_HOLE(L"Raw Bytes"), L"fore:#648000" },
	{ MULTI_STYLE(SCE_PY_TRIPLE_BYTES1, SCE_PY_TRIPLE_BYTES2, 0, 0), 63463, EDITSTYLE_HOLE(L"Triple Quoted Bytes"), L"fore:#A46000" },
	{ MULTI_STYLE(SCE_PY_FMT_STRING1, SCE_PY_FMT_STRING2, 0, 0), 63469, EDITSTYLE_HOLE(L"Formatted String"), L"fore:#008080" },
	{ MULTI_STYLE(SCE_PY_TRIPLE_FMT_STRING1, SCE_PY_TRIPLE_FMT_STRING2, 0, 0), 63470, EDITSTYLE_HOLE(L"Triple Quoted Formatted String"), L"fore:#F08000" },
	{ SCE_PY_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ SCE_PY_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
};

EDITLEXER lexPython = {
	SCLEX_PYTHON, NP2LEX_PYTHON,
	EDITLEXER_HOLE(L"Python Script", Styles_Python),
	L"py; pyw; pyx; boo; empy; cobra",
	&Keywords_Python,
	Styles_Python
};

