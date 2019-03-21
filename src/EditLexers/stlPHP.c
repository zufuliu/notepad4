#include "EditLexer.h"
#include "EditStyle.h"

// https://php.net/

static KEYWORDLIST Keywords_PHP = {{
"abstract and as break callable case catch class clone const continue declare "
"default do else elseif enddeclare endfor endforeach endif endswitch endwhile "
"extends final finally for foreach function global goto if implements instanceof "
"insteadof interface namespace new or parent private protected public return self "
"static switch throw trait try use var while xor yield "
"null true false NULL TRUE FALSE "
"define defined die echo empty eval exit isset list print unset "
"include include_once require require_once __halt_compiler "
"array bool boolean real double float int integer object string "

, // 1 Type Keyword, Magic Method
"__autoload __call __callStatic __clone __construct __destruct __get __invoke "
"__isset __set __set_state __sleep __toString __unset __wakeup "

, // 2 #preprocessor
NULL
, // 3 @Directive, Predefined Variable
NULL

, // 4 Attribute, Magic Constant
"__CLASS__ __DIR__ __FILE__ __FUNCTION__ __LINE__ __METHOD__ __NAMESPACE__ __TRAIT__ "

, // 5 Class
"stdClass __PHP_Incomplete_Class Exception ErrorException Directory php_user_filter "
"DateTime DateTimeZone DateInterval DatePeriod DateTimeInterface DateTimeImmutable "
// Reflection
"Reflection ReflectionException ReflectionClass ReflectionZendExtension ReflectionExtension ReflectionFunction ReflectionFunctionAbstract ReflectionMethod ReflectionObject ReflectionParameter ReflectionProperty "
// SPL
"SplFileInfo SplFileObject SplTempFileObject ArrayObject SplDoublyLinkedList SplStack SplQueue SplHeap SplMaxHeap SplMinHeap SplPriorityQueue SplFixedArray SplObjectStorage "
"IteratorIterator FilterIterator RegexIterator DirectoryIterator FilesystemIterator CallbackFilterIterator ArrayIterator CachingIterator "
"RecursiveIteratorIterator RecursiveFilterIterator RecursiveRegexIterator RecursiveDirectoryIterator RecursiveCallbackFilterIterator RecursiveArrayIterator RecursiveCachingIterator "
"AppendIterator EmptyIterator GlobIterator InfiniteIterator LimitIterator MultipleIterator NoRewindIterator ParentIterator RecursiveTreeIterator "
"RuntimeException LogicException BadFunctionCallException BadMethodCallException DomainException InvalidArgumentException LengthException OutOfBoundsException OutOfRangeException OverflowException RangeException UnderflowException UnexpectedValueException "
// XML
"SimpleXMLElement SimpleXMLIterator "
"SessionHandler "
"PDO PDOStatement PDOException "
"mysqli mysqli_stmt mysqli_result mysqli_driver mysqli_warning mysqli_sql_exception "
"SQLite3 SQLite3Stmt SQLite3Result "

, // 6 Interface
"Traversable Iterator IteratorAggregate ArrayAccess Serializable Closure Generator "
"Reflector JsonSerializable SessionHandlerInterface "
// SPL
"Countable OuterIterator RecursiveIterator SeekableIterator SplObserver SplSubject"

, // 7 Enumeration
"$this $GLOBALS $_SERVER $_GET $_POST $_FILES $_COOKIE $_SESSION $_REQUEST $_ENV "
"$php_errormsg $HTTP_RAW_POST_DATA $http_response_header $argc $argv "

, // 8 Constant
// Core Predefined Constants
"PHP_VERSION PHP_MAJOR_VERSION PHP_MINOR_VERSION PHP_RELEASE_VERSION PHP_VERSION_ID PHP_EXTRA_VERSION PHP_ZTS PHP_DEBUG PHP_MAXPATHLEN PHP_OS PHP_SAPI PHP_EOL PHP_INT_MAX PHP_INT_SIZE DEFAULT_INCLUDE_PATH PEAR_INSTALL_DIR PEAR_EXTENSION_DIR PHP_EXTENSION_DIR PHP_PREFIX PHP_BINDIR PHP_BINARY PHP_MANDIR PHP_LIBDIR PHP_DATADIR PHP_SYSCONFDIR PHP_LOCALSTATEDIR PHP_CONFIG_FILE_PATH PHP_CONFIG_FILE_SCAN_DIR PHP_SHLIB_SUFFIX "
// Error Handling
"E_ERROR E_WARNING E_PARSE E_NOTICE E_CORE_ERROR E_CORE_WARNING E_COMPILE_ERROR E_COMPILE_WARNING E_USER_ERROR E_USER_WARNING E_USER_NOTICE E_STRICT E_RECOVERABLE_ERROR E_DEPRECATED E_USER_DEPRECATED E_ALL "
// Output Control
"PHP_OUTPUT_HANDLER_START PHP_OUTPUT_HANDLER_WRITE PHP_OUTPUT_HANDLER_FLUSH PHP_OUTPUT_HANDLER_CLEAN PHP_OUTPUT_HANDLER_FINAL PHP_OUTPUT_HANDLER_CONT PHP_OUTPUT_HANDLER_END "
// PHP Options/Info
// Pre-defined phpcredits constants
"CREDITS_GROUP CREDITS_GENERAL CREDITS_SAPI CREDITS_MODULES CREDITS_DOCS CREDITS_FULLPAGE CREDITS_QA CREDITS_ALL "
// phpinfo constants
"INFO_GENERAL INFO_CREDITS INFO_CONFIGURATION INFO_MODULES INFO_ENVIRONMENT INFO_VARIABLES INFO_LICENSE INFO_ALL "
// INI constants
"INI_USER INI_PERDIR INI_SYSTEM INI_ALL "
// assert constants
"ASSERT_ACTIVE ASSERT_CALLBACK ASSERT_BAIL ASSERT_WARNING ASSERT_QUIET_EVAL "
// Windows specific constants
"PHP_WINDOWS_VERSION_MAJOR PHP_WINDOWS_VERSION_MINOR PHP_WINDOWS_VERSION_BUILD PHP_WINDOWS_VERSION_PLATFORM PHP_WINDOWS_VERSION_SP_MAJOR PHP_WINDOWS_VERSION_SP_MINOR PHP_WINDOWS_VERSION_SUITEMASK PHP_WINDOWS_VERSION_PRODUCTTYPE PHP_WINDOWS_NT_DOMAIN_CONTROLLER PHP_WINDOWS_NT_SERVER PHP_WINDOWS_NT_WORKSTATION "
// Date and Time, Hash, Password Hashing
"SUNFUNCS_RET_TIMESTAMP SUNFUNCS_RET_STRING SUNFUNCS_RET_DOUBLE " "HASH_HMAC " "PASSWORD_BCRYPT PASSWORD_DEFAULT "
"DIRECTORY_SEPARATOR PATH_SEPARATOR SCANDIR_SORT_ASCENDING SCANDIR_SORT_DESCENDING SCANDIR_SORT_NONE "
// Directory, Filesystem
"SEEK_SET SEEK_CUR SEEK_END LOCK_SH LOCK_EX LOCK_UN LOCK_NB GLOB_BRACE GLOB_ONLYDIR GLOB_MARK GLOB_NOSORT GLOB_NOCHECK GLOB_NOESCAPE GLOB_AVAILABLE_FLAGS PATHINFO_DIRNAME PATHINFO_BASENAME PATHINFO_EXTENSION PATHINFO_FILENAME FILE_USE_INCLUDE_PATH FILE_NO_DEFAULT_CONTEXT FILE_APPEND FILE_IGNORE_NEW_LINES FILE_SKIP_EMPTY_LINES FILE_BINARY FILE_TEXT INI_SCANNER_NORMAL INI_SCANNER_RAW FNM_NOESCAPE FNM_PATHNAME FNM_PERIOD FNM_CASEFOLD "
// Math
"M_PI M_E M_LOG2E M_LOG10E M_LN2 M_LN10 M_PI_2 M_PI_4 M_1_PI M_2_PI M_SQRTPI M_2_SQRTPI M_SQRT2 M_SQRT3 M_SQRT1_2 M_LNPI M_EULER PHP_ROUND_HALF_UP PHP_ROUND_HALF_DOWN PHP_ROUND_HALF_EVEN PHP_ROUND_HALF_ODD NAN INF "
// Misc
"CONNECTION_ABORTED CONNECTION_NORMAL CONNECTION_TIMEOUT __COMPILER_HALT_OFFSET__ "
// URLs
"PHP_URL_SCHEME PHP_URL_HOST PHP_URL_PORT PHP_URL_USER PHP_URL_PASS PHP_URL_PATH PHP_URL_QUERY PHP_URL_FRAGMENT PHP_QUERY_RFC1738 PHP_QUERY_RFC3986 "
// Network
// openlog Options
"LOG_CONS LOG_NDELAY LOG_ODELAY LOG_NOWAIT LOG_PERROR LOG_PID "
// openlog Facilities
"LOG_AUTH LOG_AUTHPRIV LOG_CRON LOG_DAEMON LOG_KERN LOG_LPR LOG_MAIL LOG_NEWS USENET LOG_SYSLOG LOG_USER LOG_UUCP "
// syslog Priorities (in descending order)
"LOG_EMERG LOG_ALERT LOG_CRIT LOG_ERR LOG_WARNING LOG_NOTICE LOG_INFO LOG_DEBUG "
// dns_get_record Options
"DNS_A DNS_MX DNS_CNAME DNS_NS DNS_PTR DNS_HINFO DNS_SOA DNS_TXT DNS_ANY DNS_AAAA DNS_ALL "
// String
"CRYPT_SALT_LENGTH integer CRYPT_STD_DES integer CRYPT_EXT_DES integer CRYPT_MD5 integer CRYPT_BLOWFISH integer HTML_SPECIALCHARS HTML_ENTITIES ENT_COMPAT ENT_QUOTES ENT_NOQUOTES ENT_IGNORE ENT_SUBSTITUTE ENT_DISALLOWED ENT_HTML401 ENT_XML1 ENT_XHTML ENT_HTML5 CHAR_MAX LC_CTYPE LC_NUMERIC LC_TIME LC_COLLATE LC_MONETARY LC_ALL LC_MESSAGES STR_PAD_LEFT STR_PAD_RIGHT STR_PAD_BOTH "
// Arrays
"CASE_LOWER CASE_UPPER SORT_ASC SORT_DESC SORT_REGULAR SORT_NUMERIC SORT_STRING SORT_LOCALE_STRING SORT_NATURAL SORT_FLAG_CASE COUNT_NORMAL COUNT_RECURSIVE EXTR_OVERWRITE EXTR_SKIP EXTR_PREFIX_SAME EXTR_PREFIX_ALL EXTR_PREFIX_INVALID EXTR_PREFIX_IF_EXISTS EXTR_IF_EXISTS EXTR_REFS "
// PCRE
"PREG_PATTERN_ORDER PREG_SET_ORDER PREG_OFFSET_CAPTURE PREG_SPLIT_NO_EMPTY PREG_SPLIT_DELIM_CAPTURE PREG_SPLIT_OFFSET_CAPTURE PREG_NO_ERROR PREG_INTERNAL_ERROR PREG_BACKTRACK_LIMIT_ERROR PREG_RECURSION_LIMIT_ERROR PREG_BAD_UTF8_ERROR PREG_BAD_UTF8_OFFSET_ERROR PCRE_VERSION "
// JSON
"JSON_ERROR_NONE JSON_ERROR_DEPTH JSON_ERROR_STATE_MISMATCH JSON_ERROR_CTRL_CHAR JSON_ERROR_SYNTAX JSON_ERROR_UTF8 JSON_HEX_TAG JSON_HEX_AMP JSON_HEX_APOS JSON_HEX_QUOT JSON_FORCE_OBJECT JSON_NUMERIC_CHECK JSON_BIGINT_AS_STRING JSON_PRETTY_PRINT JSON_UNESCAPED_SLASHES JSON_UNESCAPED_UNICODE "
// Session Handling
"SID PHP_SESSION_DISABLED PHP_SESSION_NONE PHP_SESSION_ACTIVE "
// Multibyte String
"MB_OVERLOAD_MAIL MB_OVERLOAD_STRING MB_OVERLOAD_REGEX MB_CASE_UPPER MB_CASE_LOWER MB_CASE_TITLE "
// GMP
"GMP_ROUND_ZERO GMP_ROUND_PLUSINF GMP_ROUND_MINUSINF GMP_VERSION "
// MySQL client constants
"MYSQL_CLIENT_COMPRESS MYSQL_CLIENT_IGNORE_SPACE MYSQL_CLIENT_INTERACTIVE MYSQL_CLIENT_SSL "
// MySQL fetch constants
"MYSQL_ASSOC MYSQL_BOTH MYSQL_NUM "
// MySQLi
"MYSQLI_READ_DEFAULT_GROUP MYSQLI_READ_DEFAULT_FILE MYSQLI_OPT_CONNECT_TIMEOUT MYSQLI_OPT_LOCAL_INFILE MYSQLI_INIT_COMMAND MYSQLI_SERVER_PUBLIC_KEY MYSQLI_CLIENT_SSL MYSQLI_CLIENT_COMPRESS MYSQLI_CLIENT_INTERACTIVE MYSQLI_CLIENT_IGNORE_SPACE MYSQLI_CLIENT_NO_SCHEMA MYSQLI_CLIENT_MULTI_QUERIES MYSQLI_STORE_RESULT MYSQLI_USE_RESULT MYSQLI_ASSOC MYSQLI_NUM MYSQLI_BOTH MYSQLI_NOT_NULL_FLAG MYSQLI_PRI_KEY_FLAG MYSQLI_UNIQUE_KEY_FLAG MYSQLI_MULTIPLE_KEY_FLAG MYSQLI_BLOB_FLAG MYSQLI_UNSIGNED_FLAG MYSQLI_ZEROFILL_FLAG MYSQLI_AUTO_INCREMENT_FLAG MYSQLI_TIMESTAMP_FLAG MYSQLI_SET_FLAG MYSQLI_NUM_FLAG MYSQLI_PART_KEY_FLAG MYSQLI_GROUP_FLAG MYSQLI_TYPE_DECIMAL MYSQLI_TYPE_NEWDECIMAL MYSQLI_TYPE_BIT MYSQLI_TYPE_TINY MYSQLI_TYPE_SHORT MYSQLI_TYPE_LONG MYSQLI_TYPE_FLOAT MYSQLI_TYPE_DOUBLE MYSQLI_TYPE_NULL MYSQLI_TYPE_TIMESTAMP MYSQLI_TYPE_LONGLONG MYSQLI_TYPE_INT24 MYSQLI_TYPE_DATE MYSQLI_TYPE_TIME MYSQLI_TYPE_DATETIME MYSQLI_TYPE_YEAR MYSQLI_TYPE_NEWDATE MYSQLI_TYPE_INTERVAL MYSQLI_TYPE_ENUM MYSQLI_TYPE_SET MYSQLI_TYPE_TINY_BLOB MYSQLI_TYPE_MEDIUM_BLOB MYSQLI_TYPE_LONG_BLOB MYSQLI_TYPE_BLOB MYSQLI_TYPE_VAR_STRING MYSQLI_TYPE_STRING MYSQLI_TYPE_CHAR MYSQLI_TYPE_GEOMETRY MYSQLI_NEED_DATA MYSQLI_NO_DATA MYSQLI_DATA_TRUNCATED MYSQLI_ENUM_FLAG MYSQLI_CURSOR_TYPE_FOR_UPDATE MYSQLI_CURSOR_TYPE_NO_CURSOR MYSQLI_CURSOR_TYPE_READ_ONLY MYSQLI_CURSOR_TYPE_SCROLLABLE MYSQLI_STMT_ATTR_CURSOR_TYPE MYSQLI_STMT_ATTR_PREFETCH_ROWS MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH MYSQLI_SET_CHARSET_NAME MYSQLI_REPORT_INDEX MYSQLI_REPORT_ERROR MYSQLI_REPORT_STRICT MYSQLI_REPORT_ALL MYSQLI_REPORT_OFF MYSQLI_DEBUG_TRACE_ENABLED MYSQLI_SERVER_QUERY_NO_GOOD_INDEX_USED MYSQLI_SERVER_QUERY_NO_INDEX_USED MYSQLI_REFRESH_GRANT MYSQLI_REFRESH_LOG MYSQLI_REFRESH_TABLES MYSQLI_REFRESH_HOSTS MYSQLI_REFRESH_STATUS MYSQLI_REFRESH_THREADS MYSQLI_REFRESH_SLAVE MYSQLI_REFRESH_MASTER "
// SQLite3
"SQLITE3_ASSOC SQLITE3_NUM SQLITE3_BOTH SQLITE3_INTEGER SQLITE3_FLOAT SQLITE3_TEXT SQLITE3_BLOB SQLITE3_NULL SQLITE3_OPEN_READONLY SQLITE3_OPEN_READWRITE SQLITE3_OPEN_CREATE "
// PDO
"PARAM_BOOL PARAM_NULL PARAM_INT PARAM_STR PARAM_LOB PARAM_STMT PARAM_INPUT_OUTPUT FETCH_LAZY FETCH_ASSOC FETCH_NAMED FETCH_NUM FETCH_BOTH FETCH_OBJ FETCH_BOUND FETCH_COLUMN FETCH_CLASS FETCH_INTO FETCH_FUNC FETCH_GROUP FETCH_UNIQUE FETCH_KEY_PAIR FETCH_CLASSTYPE FETCH_SERIALIZE FETCH_PROPS_LATE ATTR_AUTOCOMMIT ATTR_PREFETCH ATTR_TIMEOUT ATTR_ERRMODE ATTR_SERVER_VERSION ATTR_CLIENT_VERSION ATTR_SERVER_INFO ATTR_CONNECTION_STATUS ATTR_CASE ATTR_CURSOR_NAME ATTR_CURSOR ATTR_DRIVER_NAME ATTR_ORACLE_NULLS ATTR_PERSISTENT ATTR_STATEMENT_CLASS ATTR_FETCH_CATALOG_NAMES ATTR_FETCH_TABLE_NAMES ATTR_STRINGIFY_FETCHES ATTR_MAX_COLUMN_LEN ATTR_DEFAULT_FETCH_MODE ATTR_EMULATE_PREPARES ERRMODE_SILENT ERRMODE_WARNING ERRMODE_EXCEPTION CASE_NATURAL CASE_LOWER CASE_UPPER NULL_NATURAL NULL_EMPTY_STRING NULL_TO_STRING FETCH_ORI_NEXT FETCH_ORI_PRIOR FETCH_ORI_FIRST FETCH_ORI_LAST FETCH_ORI_ABS FETCH_ORI_REL CURSOR_FWDONLY CURSOR_SCROLL ERR_NONE PARAM_EVT_ALLOC PARAM_EVT_FREE PARAM_EVT_EXEC_PRE PARAM_EVT_EXEC_POST PARAM_EVT_FETCH_PRE PARAM_EVT_FETCH_POST PARAM_EVT_NORMALIZE "
// cURL

, // 9 Function
// Error Handling
"debug_backtrace() debug_print_backtrace() error_get_last() error_log() error_reporting() restore_error_handler() restore_exception_handler() set_error_handler() set_exception_handler() trigger_error() user_error() "
// Output Control
"flush() ob_clean() ob_end_clean() ob_end_flush() ob_flush() ob_get_clean() ob_get_contents() ob_get_flush() ob_get_length() ob_get_level() ob_get_status() ob_gzhandler() ob_implicit_flush() ob_list_handlers() ob_start() output_add_rewrite_var() output_reset_rewrite_vars() "
// PHP Options/Info
"assert_options() assert() dl() extension_loaded() gc_collect_cycles() gc_disable() gc_enable() gc_enabled() get_cfg_var() get_current_user() get_defined_constants() get_extension_funcs() get_include_path() get_included_files() get_loaded_extensions() get_magic_quotes_gpc() get_magic_quotes_runtime() get_required_files() getenv() getlastmod() getmygid() getmyinode() getmypid() getmyuid() getopt() getrusage() ini_alter() ini_get_all() ini_get() ini_restore() ini_set() magic_quotes_runtime() main() memory_get_peak_usage() memory_get_usage() php_ini_loaded_file() php_ini_scanned_files() php_logo_guid() php_sapi_name() php_uname() phpcredits() phpinfo() phpversion() putenv() restore_include_path() set_include_path() set_magic_quotes_runtime() set_time_limit() sys_get_temp_dir() version_compare() zend_logo_guid() zend_thread_id() zend_version() "
// Date and Time
"checkdate() date_add() date_create_from_format() date_create() date_date_set() date_default_timezone_get() date_default_timezone_set() date_diff() date_format() date_get_last_errors() date_interval_create_from_date_string() date_interval_format() date_isodate_set() date_modify() date_offset_get() date_parse_from_format() date_parse() date_sub() date_sun_info() date_sunrise() date_sunset() date_time_set() date_timestamp_get() date_timestamp_set() date_timezone_get() date_timezone_set() date() getdate() gettimeofday() gmdate() gmmktime() gmstrftime() idate() localtime() microtime() mktime() strftime() strptime() strtotime() time() timezone_abbreviations_list() timezone_identifiers_list() timezone_location_get() timezone_name_from_abbr() timezone_name_get() timezone_offset_get() timezone_open() timezone_transitions_get() timezone_version_get() "
// Hash, Password Hashing
"hash_algos() hash_copy() hash_file() hash_final() hash_hmac_file() hash_hmac() hash_init() hash_pbkdf2() hash_update_file() hash_update_stream() hash_update() hash() "
"password_get_info() password_hash() password_needs_rehash() password_verify() "
// Directory, FileSystem
"chdir() chroot() closedir() dir() getcwd() opendir() readdir() rewinddir() scandir() "
"basename() chgrp() chmod() chown() clearstatcache() copy() delete() dirname() disk_free_space() disk_total_space() diskfreespace() fclose() feof() fflush() fgetc() fgetcsv() fgets() fgetss() file_exists() file_get_contents() file_put_contents() file() fileatime() filectime() filegroup() fileinode() filemtime() fileowner() fileperms() filesize() filetype() flock() fnmatch() fopen() fpassthru() fputcsv() fputs() fread() fscanf() fseek() fstat() ftell() ftruncate() fwrite() glob() is_dir() is_executable() is_file() is_link() is_readable() is_uploaded_file() is_writable() is_writeable() lchgrp() lchown() link() linkinfo() lstat() mkdir() move_uploaded_file() parse_ini_file() parse_ini_string() pathinfo() pclose() popen() readfile() readlink() realpath_cache_get() realpath_cache_size() realpath() rename() rewind() rmdir() set_file_buffer() stat() symlink() tempnam() tmpfile() touch() umask() unlink() "
"ezmlm_hash() mail() " // Mail
// Math
"abs() acos() acosh() asin() asinh() atan2() atan() atanh() base_convert() bindec() ceil() cos() cosh() decbin() dechex() decoct() deg2rad() exp() expm1() floor() fmod() getrandmax() hexdec() hypot() is_finite() is_infinite() is_nan() lcg_value() log10() log1p() log() max() min() mt_getrandmax() mt_rand() mt_srand() octdec() pi() pow() rad2deg() rand() round() sin() sinh() sqrt() srand() tan() tanh() "
// System program execution
"escapeshellarg() escapeshellcmd() exec() passthru() proc_close() proc_get_status() proc_nice() proc_open() proc_terminate() shell_exec() system() "
// Misc
"connection_aborted() connection_status() connection_timeout() constant() get_browser() __halt_compiler() highlight_file() highlight_string() ignore_user_abort() pack() php_check_syntax() php_strip_whitespace() show_source() sleep() sys_getloadavg() time_nanosleep() time_sleep_until() uniqid() unpack() usleep() "
// URLs
"base64_decode() base64_encode() get_headers() get_meta_tags() http_build_query() parse_url() rawurldecode() rawurlencode() urldecode() urlencode() "
// Network
"checkdnsrr() closelog() define_syslog_variables() dns_check_record() dns_get_mx() dns_get_record() fsockopen() gethostbyaddr() gethostbyname() gethostbynamel() gethostname() getmxrr() getprotobyname() getprotobynumber() getservbyname() getservbyport() header_register_callback() header_remove() header() headers_list() headers_sent() http_response_code() inet_ntop() inet_pton() ip2long() long2ip() openlog() pfsockopen() setcookie() setrawcookie() socket_get_status() socket_set_blocking() socket_set_timeout() syslog() "
// String
"addcslashes() addslashes() bin2hex() chop() chr() chunk_split() convert_cyr_string() convert_uudecode() convert_uuencode() count_chars() crc32() crypt() echo() explode() fprintf() get_html_translation_table() hebrev() hebrevc() hex2bin() html_entity_decode() htmlentities() htmlspecialchars_decode() htmlspecialchars() implode() join() lcfirst() levenshtein() localeconv() ltrim() md5_file() md5() metaphone() money_format() nl_langinfo() nl2br() number_format() ord() parse_str() print() printf() quoted_printable_decode() quoted_printable_encode() quotemeta() rtrim() setlocale() sha1_file() sha1() similar_text() soundex() sprintf() sscanf() str_getcsv() str_ireplace() str_pad() str_repeat() str_replace() str_rot13() str_shuffle() str_split() str_word_count() strcasecmp() strchr() strcmp() strcoll() strcspn() strip_tags() stripcslashes() stripos() stripslashes() stristr() strlen() strnatcasecmp() strnatcmp() strncasecmp() strncmp() strpbrk() strpos() strrchr() strrev() strripos() strrpos() strspn() strstr() strtok() strtolower() strtoupper() strtr() substr_compare() substr_count() substr_replace() substr() trim() ucfirst() ucwords() vfprintf() vprintf() vsprintf() wordwrap() "
// Arrays
"array_change_key_case() array_chunk() array_column() array_combine() array_count_values() array_diff_assoc() array_diff_key() array_diff_uassoc() array_diff_ukey() array_diff() array_fill_keys() array_fill() array_filter() array_flip() array_intersect_assoc() array_intersect_key() array_intersect_uassoc() array_intersect_ukey() array_intersect() array_key_exists() array_keys() array_map() array_merge_recursive() array_merge() array_multisort() array_pad() array_pop() array_product() array_push() array_rand() array_reduce() array_replace_recursive() array_replace() array_reverse() array_search() array_shift() array_slice() array_splice() array_sum() array_udiff_assoc() array_udiff_uassoc() array_udiff() array_uintersect_assoc() array_uintersect_uassoc() array_uintersect() array_unique() array_unshift() array_values() array_walk_recursive() array_walk() array() arsort() asort() compact() count() current() each() end() extract() in_array() key() krsort() ksort() list() natcasesort() natsort() next() pos() prev() range() reset() rsort() shuffle() sizeof() sort() uasort() uksort() usort() "
// Class/Object Information
"call_user_method_array() call_user_method() class_alias() class_exists() get_called_class() get_class_methods() get_class_vars() get_class() get_declared_classes() get_declared_interfaces() get_declared_traits() get_object_vars() get_parent_class() interface_exists() is_a() is_subclass_of() method_exists() property_exists() trait_exists() "
// Function Handling
"call_user_func_array() call_user_func() create_function() forward_static_call_array() forward_static_call() func_get_arg() func_get_args() func_num_args() function_exists() get_defined_functions() register_shutdown_function() register_tick_function() unregister_tick_function() "
// Variable handling
"boolval() debug_zval_dump() doubleval() floatval() get_defined_vars() get_resource_type() gettype() import_request_variables() intval() is_array() is_bool() is_callable() is_double() is_float() is_int() is_integer() is_long() is_null() is_numeric() is_object() is_real() is_resource() is_scalar() is_string() print_r() serialize() settype() strval() unserialize() var_dump() var_export() "
// PCRE
"preg_filter() preg_grep() preg_last_error() preg_match_all() preg_match() preg_quote() preg_replace_callback() preg_replace() preg_split() "
// POXSIX-Regex, JSON
"ereg_replace() ereg() eregi_replace() eregi() split() spliti() sql_regcase() " "json_decode() json_encode() json_last_error() json_last_error_msg() "
// Character type checking
"ctype_alnum() ctype_alpha() ctype_cntrl() ctype_digit() ctype_graph() ctype_lower() ctype_print() ctype_punct() ctype_space() ctype_upper() ctype_xdigit() "
// Session Handling
"session_cache_expire() session_cache_limiter() session_commit() session_decode() session_destroy() session_encode() session_get_cookie_params() session_id() session_is_registered() session_module_name() session_name() session_regenerate_id() session_register_shutdown() session_register() session_save_path() session_set_cookie_params() session_set_save_handler() session_start() session_status() session_unregister() session_unset() session_write_close() "
// Multibyte String
"mb_check_encoding() mb_convert_case() mb_convert_encoding() mb_convert_kana() mb_convert_variables() mb_decode_mimeheader() mb_decode_numericentity() mb_detect_encoding() mb_detect_order() mb_encode_mimeheader() mb_encode_numericentity() mb_encoding_aliases() mb_ereg_match() mb_ereg_replace_callback() mb_ereg_replace() mb_ereg_search_getpos() mb_ereg_search_getregs() mb_ereg_search_init() mb_ereg_search_pos() mb_ereg_search_regs() mb_ereg_search_setpos() mb_ereg_search() mb_ereg() mb_eregi_replace() mb_eregi() mb_get_info() mb_http_input() mb_http_output() mb_internal_encoding() mb_language() mb_list_encodings() mb_output_handler() mb_parse_str() mb_preferred_mime_name() mb_regex_encoding() mb_regex_set_options() mb_send_mail() mb_split() mb_strcut() mb_strimwidth() mb_stripos() mb_stristr() mb_strlen() mb_strpos() mb_strrchr() mb_strrichr() mb_strripos() mb_strrpos() mb_strstr() mb_strtolower() mb_strtoupper() mb_strwidth() mb_substitute_character() mb_substr_count() mb_substr() "
// BCMath
"bcadd() bccomp() bcdiv() bcmod() bcmul() bcpow() bcpowmod() bcscale() bcsqrt() bcsub() "
// GMP
"gmp_abs() gmp_add() gmp_and() gmp_clrbit() gmp_cmp() gmp_com() gmp_div_q() gmp_div_qr() gmp_div_r() gmp_div() gmp_divexact() gmp_fact() gmp_gcd() gmp_gcdext() gmp_hamdist() gmp_init() gmp_intval() gmp_invert() gmp_jacobi() gmp_legendre() gmp_mod() gmp_mul() gmp_neg() gmp_nextprime() gmp_or() gmp_perfect_square() gmp_popcount() gmp_pow() gmp_powm() gmp_prob_prime() gmp_random() gmp_scan0() gmp_scan1() gmp_setbit() gmp_sign() gmp_sqrt() gmp_sqrtrem() gmp_strval() gmp_sub() gmp_testbit() gmp_xor() "
// MySQL
"mysql_affected_rows() mysql_client_encoding() mysql_close() mysql_connect() mysql_create_db() mysql_data_seek() mysql_db_name() mysql_db_query() mysql_drop_db() mysql_errno() mysql_error() mysql_escape_string() mysql_fetch_array() mysql_fetch_assoc() mysql_fetch_field() mysql_fetch_lengths() mysql_fetch_object() mysql_fetch_row() mysql_field_flags() mysql_field_len() mysql_field_name() mysql_field_seek() mysql_field_table() mysql_field_type() mysql_free_result() mysql_get_client_info() mysql_get_host_info() mysql_get_proto_info() mysql_get_server_info() mysql_info() mysql_insert_id() mysql_list_dbs() mysql_list_fields() mysql_list_processes() mysql_list_tables() mysql_num_fields() mysql_num_rows() mysql_pconnect() mysql_ping() mysql_query() mysql_real_escape_string() mysql_result() mysql_select_db() mysql_set_charset() mysql_stat() mysql_tablename() mysql_thread_id() mysql_unbuffered_query() "
// MySQLi
"mysqli_affected_rows() mysqli_begin_transaction() mysqli_get_client_info() mysqli_get_client_version() mysqli_connect_errno() mysqli_connect_error() mysqli_errno() mysqli_error() mysqli_field_count() mysqli_get_host_info() mysqli_get_proto_info() mysqli_get_server_info() mysqli_get_server_version() mysqli_info() mysqli_insert_id() mysqli_sqlstate() mysqli_warning_count() mysqli_autocommit() mysqli_change_user() mysqli_character_set_name() mysqli_client_encoding() mysqli_close() mysqli_commit() mysqli_connect() mysqli_debug() mysqli_dump_debug_info() mysqli_get_charset() mysqli_get_connection_stats() mysqli_get_client_info() mysqli_get_client_stats() mysqli_get_cache_stats() mysqli_get_server_info() mysqli_get_warnings() mysqli_init() mysqli_kill() mysqli_more_results() mysqli_multi_query() mysqli_next_result() mysqli_options() mysqli_ping() mysqli_prepare() mysqli_query() mysqli_real_connect() mysqli_real_escape_string() mysqli_real_query() mysqli_refresh() mysqli_release_savepoint() mysqli_rollback() mysqli_savepoint() mysqli_select_db() mysqli_set_charset() mysqli_set_local_infile_default() mysqli_set_local_infile_handler() mysqli_ssl_set() mysqli_stat() mysqli_stmt_init() mysqli_store_result() mysqli_thread_id() mysqli_thread_safe() mysqli_use_result() "
"mysqli_stmt_affected_rows() mysqli_stmt_errno() mysqli_stmt_error() mysqli_stmt_field_count() mysqli_stmt_insert_id() mysqli_stmt_num_rows() mysqli_stmt_param_count() mysqli_stmt_sqlstate() mysqli_stmt_attr_get() mysqli_stmt_attr_set() mysqli_stmt_bind_param() mysqli_stmt_bind_result() mysqli_stmt_close() mysqli_stmt_data_seek() mysqli_stmt_execute() mysqli_stmt_fetch() mysqli_stmt_free_result() mysqli_stmt_get_result() mysqli_stmt_get_warnings() mysqli_stmt_more_results() mysqli_stmt_next_result() mysqli_stmt_num_rows() mysqli_stmt_prepare() mysqli_stmt_reset() mysqli_stmt_result_metadata() mysqli_stmt_send_long_data() mysqli_stmt_store_result() mysqli_field_tell() mysqli_num_fields() mysqli_fetch_lengths() mysqli_num_rows() mysqli_data_seek() mysqli_fetch_all() mysqli_fetch_array() mysqli_fetch_assoc() mysqli_fetch_field_direct() mysqli_fetch_field() mysqli_fetch_fields() mysqli_fetch_object() mysqli_fetch_row() mysqli_field_seek() mysqli_free_result() mysqli_embedded_server_end() mysqli_embedded_server_start() mysqli_bind_param() mysqli_bind_result() mysqli_client_encoding() mysqli_connect() mysqli::disable_reads_from_master() mysqli_disable_rpl_parse() mysqli_enable_reads_from_master() mysqli_enable_rpl_parse() mysqli_escape_string() mysqli_execute() mysqli_fetch() mysqli_get_cache_stats() mysqli_get_metadata() mysqli_master_query() mysqli_param_count() mysqli_report() mysqli_rpl_parse_enabled() mysqli_rpl_probe() mysqli_send_long_data() mysqli_set_opt() mysqli_slave_query() "
// SPL
"class_implements() class_parents() class_uses() iterator_apply() iterator_count() iterator_to_array() spl_autoload_call() spl_autoload_extensions() spl_autoload_functions() spl_autoload_register() spl_autoload_unregister() spl_autoload() spl_classes() spl_object_hash() "
// SimpleXML
"simplexml_import_dom() simplexml_load_file() simplexml_load_string() "
// cURL
"curl_close() curl_copy_handle() curl_errno() curl_error() curl_escape() curl_exec() curl_file_create() curl_getinfo() curl_init() curl_multi_add_handle() curl_multi_close() curl_multi_exec() curl_multi_getcontent() curl_multi_info_read() curl_multi_init() curl_multi_remove_handle() curl_multi_select() curl_multi_setopt() curl_multi_strerror() curl_pause() curl_reset() curl_setopt_array() curl_setopt() curl_share_close() curl_share_init() curl_share_setopt() curl_strerror() curl_unescape() curl_version() "

, // 10 Field
// Exception/ErrorException
"message code file line NULLseverity "
// Directory
"path handle "
// php_user_filter
"filtername params "
// MySQLi
"affected_rows client_info client_version connect_errno connect_error errno error_list field_count client_version host_info protocol_version server_info info insert_id warning_count "
"num_rows param_count sqlstate " "current_field lengths " "embedded reconnect report_mode "

, // 11 Method
// Iterator/Generator/IteratorAggregate/ArrayAccess
"current() key() next() rewind() valid() " " send() throw() valid() " " getIterator() " "offsetExists() offsetGet() offsetSet() offsetUnset() "
// Serializable/Closure
"serialize() unserialize() " " bind() bindTo() "
// Exception/ErrorException
"getMessage() getPrevious() getCode() getFile() getLine() getTrace() getTraceAsString() " " getSeverity() "
// Directory
"close() read() rewind() "
// php_user_filter
"filter() onClose() onCreate() "
// MySQLi
"autocommit() begin_transaction() change_user() character_set_name() close() commit() debug() dump_debug_info() get_charset() get_client_info() get_connection_stats() get_warnings() init() kill() more_results() multi_query() next_result() options() ping() poll() prepare() query() real_connect() escape_string() real_query() reap_async_query() refresh() release_savepoint() rollback() rpl_query_type() savepoint() select_db() send_query() set_charset() set_charset() set_local_infile_handler() ssl_set() stat() stmt_init() store_result() use_result() "
// mysqli_stmt
"attr_get() attr_set() bind_param() bind_result() close() data_seek() execute() fetch() free_result() get_result() get_warnings() prepare() reset() result_metadata() send_long_data() store_result() "
// mysqli_result
"data_seek() fetch_all() fetch_array() fetch_assoc() fetch_field_direct() fetch_field() fetch_fields() fetch_object() fetch_row() field_seek() free() "
"embedded_server_end() embedded_server_start() "
// SQLite3
"busyTimeout() changes() close() createAggregate() createFunction() escapeString() exec() lastErrorCode() lastErrorMsg() lastInsertRowID() loadExtension() open() prepare() query() querySingle() version() "
// SQLite3Stmt
"bindParam() bindValue() clear() close() execute() paramCount() reset() "
// SQLite3Result
"columnName() columnType() fetchArray() finalize() numColumns() reset() "
// PDO
"beginTransaction() commit() errorCode() errorInfo() exec() getAttribute() getAvailableDrivers() inTransaction() lastInsertId() prepare() query() quote() rollBack() setAttribute() "
// PDOStatement
"bindColumn() bindParam() bindValue() closeCursor() columnCount() debugDumpParams() errorCode() errorInfo() execute() fetch() fetchAll() fetchColumn() fetchObject() getAttribute() getColumnMeta() nextRowset() rowCount() setAttribute() setFetchMode() "
// SimpleXML
"addAttribute() addChild() asXML() attributes() children() count() getDocNamespaces() getName() getNamespaces() registerXPathNamespace() xpath() " "getChildren() hasChildren() "

, // 12 Tag
"php"

, // 13 String Constant
// $_SERVER
"PHP_SELF argv argc GATEWAY_INTERFACE SERVER_ADDR SERVER_NAME SERVER_SOFTWARE SERVER_PROTOCOL REQUEST_METHOD REQUEST_TIME REQUEST_TIME_FLOAT QUERY_STRING DOCUMENT_ROOT HTTP_ACCEPT HTTP_ACCEPT_CHARSET HTTP_ACCEPT_ENCODING HTTP_ACCEPT_LANGUAGE HTTP_CONNECTION HTTP_HOST HTTP_REFERER HTTP_USER_AGENT HTTPS REMOTE_ADDR REMOTE_HOST REMOTE_PORT REMOTE_USER REDIRECT_REMOTE_USER SCRIPT_FILENAME SERVER_ADMIN SERVER_PORT SERVER_SIGNATURE PATH_TRANSLATED SCRIPT_NAME REQUEST_URI PHP_AUTH_DIGEST PHP_AUTH_USER PHP_AUTH_PW AUTH_TYPE PATH_INFO ORIG_PATH_INFO "

, // 14
NULL

, // 15 Code Snippet
"for^() if^() switch^() while^() elseif^() else^{} foreach^() catch^() "
"declare() define() defined() die() echo() empty() eval() exit() isset() list() unset() require() require_once() __halt_compiler() "
"__autoload() __call() __callStatic() __clone() __construct() __destruct() __get() __invoke() __isset() __set() __set_state() __sleep() __toString() __unset() __wakeup() "
}};

static EDITSTYLE Styles_PHP[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2STYLE_Keyword, EDITSTYLE_HOLE(L"Keyword"), L"bold; fore:#FF8000" },
	{ SCE_C_CLASS, NP2STYLE_Class, EDITSTYLE_HOLE(L"Class"), L"bold; fore:#007F7F" },
	{ SCE_C_INTERFACE, NP2STYLE_Interface, EDITSTYLE_HOLE(L"Interface"), L"bold; fore:#1E90FF" },
	{ SCE_C_TRAIT, NP2STYLE_Trait, EDITSTYLE_HOLE(L"Trait"), L"bold; fore:#007F7F" },
	{ SCE_C_FUNCTION, NP2STYLE_Function, EDITSTYLE_HOLE(L"Function"), L"fore:#A46000" },
	{ SCE_C_WORD2, 63561, EDITSTYLE_HOLE(L"Magic Method"), L"fore:#0080C0" },
	{ SCE_C_ATTRIBUTE, 63562, EDITSTYLE_HOLE(L"Magic Constant"), L"fore:#FF8000" },
	{ SCE_C_ENUMERATION, NP2STYLE_PredefinedVariable, EDITSTYLE_HOLE(L"Predefined Variable"), L"bold; italic; fore:#0080C0" },
	{ SCE_C_CONSTANT, NP2STYLE_Constant, EDITSTYLE_HOLE(L"Constant"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, 0, 0), NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ SCE_C_COMMENTDOC_TAG, NP2STYLE_DocCommentTag, EDITSTYLE_HOLE(L"Doc Comment Tag"), L"fore:#408080" },
	{ MULTI_STYLE(SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG_XML, 0), NP2STYLE_DocComment, EDITSTYLE_HOLE(L"Doc Comment"), L"fore:#408040" },
	{ MULTI_STYLE(SCE_C_STRING, SCE_C_CHARACTER, SCE_C_STRINGEOL, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_C_HEREDOC, 63564, EDITSTYLE_HOLE(L"Heredoc String"), L"fore:#648000" },
	{ SCE_C_NOWDOC, 63565, EDITSTYLE_HOLE(L"Nowdoc String"), L"fore:#A46000" },
	{ SCE_C_LABEL, NP2STYLE_Label, EDITSTYLE_HOLE(L"Label"), L"fore:#000000; back:#FFC040" },
	{ SCE_C_NUMBER, NP2STYLE_Number, EDITSTYLE_HOLE(L"Number"), L"fore:#FF0000" },
	{ MULTI_STYLE(SCE_C_VARIABLE, SCE_C_VARIABLE2, 0, 0), NP2STYLE_Variable, EDITSTYLE_HOLE(L"Variable"), L"italic; fore:#003CE6" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ SCE_C_XML_TAG, 63566, EDITSTYLE_HOLE(L"PHP Tag"), L"bold; fore:#8B008B" },
};

EDITLEXER lexPHP = {
	SCLEX_CPP, NP2LEX_PHP,
	EDITLEXER_HOLE(L"PHP Script", Styles_PHP),
	L"php; phpt; phtml; php_cs; eyecode",
	&Keywords_PHP,
	Styles_PHP
};

