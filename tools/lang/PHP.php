<?php
// 8.4 https://www.php.net/
// https://wiki.php.net/rfc
// https://php.watch/versions

//! keywords			=======================================================
// https://www.php.net/manual/en/reserved.keywords.php
__halt_compiler()
abstract and array() as
break
case catch class clone const continue
declare() default die() do
echo else elseif empty()
enddeclare endfor endforeach endif endswitch endwhile
eval() exit() extends
final finally fn for foreach function
global goto
if implements include include_once instanceof insteadof interface isset()
list()
match
namespace new
or
print private protected public
readonly require require_once return
static switch
throw trait try
unset() use
var
while
xor
yield from

enum
true false null

//! type				=======================================================
// https://www.php.net/manual/en/language.types.intro.php
// https://www.php.net/manual/en/reserved.other-reserved-words.php
bool int float double string array object callable iterable resource numeric
// https://www.php.net/manual/en/language.types.declarations.php
// Single types
self parent array callable bool float int string iterable object mixed
// Return only types
void never static
// https://www.php.net/manual/en/language.types.type-juggling.php
int integer bool boolean float double real string array object unset

//! attribute			=======================================================
// https://www.php.net/manual/en/reserved.attributes.php
#[Attribute]
#[\AllowDynamicProperties]
#[\Deprecated]
#[\Override]
#[\ReturnTypeWillChange]
#[\SensitiveParameter]

//! Predefined Variable	=======================================================
// https://www.php.net/manual/en/language.variables.basics.php
$this
// https://www.php.net/manual/en/reserved.variables.php
$GLOBALS $_SERVER $_GET $_POST $_FILES $_REQUEST $_SESSION $_ENV $_COOKIE
$http_response_header
$argc
$argv
// Indices for $_SERVER
{
PHP_SELF
argv
argc
GATEWAY_INTERFACE
SERVER_ADDR
SERVER_NAME
SERVER_SOFTWARE
SERVER_PROTOCOL
REQUEST_METHOD
REQUEST_TIME
REQUEST_TIME_FLOAT
QUERY_STRING
DOCUMENT_ROOT
HTTP_ACCEPT
HTTP_ACCEPT_CHARSET
HTTP_ACCEPT_ENCODING
HTTP_ACCEPT_LANGUAGE
HTTP_CONNECTION
HTTP_HOST
HTTP_REFERER
HTTP_USER_AGENT
HTTPS
REMOTE_ADDR
REMOTE_HOST
REMOTE_PORT
REMOTE_USER
REDIRECT_REMOTE_USER
SCRIPT_FILENAME
SERVER_ADMIN
SERVER_PORT
SERVER_SIGNATURE
PATH_TRANSLATED
SCRIPT_NAME
REQUEST_URI
PHP_AUTH_DIGEST
PHP_AUTH_USER
PHP_AUTH_PW
AUTH_TYPE
PATH_INFO
ORIG_PATH_INFO
}

//! api					=======================================================
{ // PHP Core
// Magic constant
// https://www.php.net/manual/en/language.constants.magic.php
__LINE__
__FILE__
__DIR__
__FUNCTION__
__CLASS__
__TRAIT__
__METHOD__
__PROPERTY__
__NAMESPACE__

// Predefined constant
// https://www.php.net/manual/en/reserved.constants.php
__COMPILER_HALT_OFFSET__
PHP_VERSION
PHP_MAJOR_VERSION
PHP_MINOR_VERSION
PHP_RELEASE_VERSION
PHP_VERSION_ID
PHP_EXTRA_VERSION
PHP_ZTS
PHP_DEBUG
PHP_MAXPATHLEN
PHP_OS
PHP_OS_FAMILY
PHP_SAPI
PHP_EOL
PHP_INT_MAX
PHP_INT_MIN
PHP_INT_SIZE
PHP_FLOAT_DIG
PHP_FLOAT_EPSILON
PHP_FLOAT_MIN
PHP_FLOAT_MAX
DEFAULT_INCLUDE_PATH
PEAR_INSTALL_DIR
PEAR_EXTENSION_DIR
PHP_EXTENSION_DIR
PHP_PREFIX
PHP_BINDIR
PHP_BINARY
PHP_MANDIR
PHP_LIBDIR
PHP_DATADIR
PHP_SYSCONFDIR
PHP_LOCALSTATEDIR
PHP_CONFIG_FILE_PATH
PHP_CONFIG_FILE_SCAN_DIR
PHP_SHLIB_SUFFIX
PHP_FD_SETSIZE
PHP_WINDOWS_EVENT_CTRL_C
PHP_WINDOWS_EVENT_CTRL_BREAK

// Magic Method
// https://www.php.net/manual/en/language.oop5.magic.php
__construct()
__destruct()
__call()
__callStatic()
__get()
__set()
__isset()
__unset()
__sleep()
__wakeup()
__serialize()
__unserialize()
__toString()
__invoke()
__set_state()
__clone()
__debugInfo()

// Predefined Interfaces and Classes
class stdClass {}
class __PHP_Incomplete_Class {}
// https://www.php.net/manual/en/reserved.interfaces.php
interface Traversable {}
interface Iterator extends Traversable {
	public current(): mixed
	public key(): mixed
	public next(): void
	public rewind(): void
	public valid(): bool
}
interface IteratorAggregate extends Traversable {
	public getIterator(): Traversable
}
interface Throwable extends Stringable {
	public getMessage(): string
	public getCode(): int
	public getFile(): string
	public getLine(): int
	public getTrace(): array
	public getTraceAsString(): string
	public getPrevious(): ?Throwable
	abstract public __toString(): string
}
interface ArrayAccess {
	public offsetExists(mixed $offset): bool
	public offsetGet(mixed $offset): mixed
	public offsetSet(mixed $offset, mixed $value): void
	public offsetUnset(mixed $offset): void
}
interface Serializable {
	public serialize(): ?string
	public unserialize(string $data): void
}
final class Closure {
	public static bind(Closure $closure, ?object $newThis, object|string|null $newScope = "static"): ?Closure
	public bindTo(?object $newThis, object|string|null $newScope = "static"): ?Closure
	public call(object $newThis, mixed ...$args): mixed
	public static fromCallable(callable $callback): Closure
}
final class Generator implements Iterator {
	public current(): mixed
	public getReturn(): mixed
	public key(): mixed
	public next(): void
	public rewind(): void
	public send(mixed $value): mixed
	public throw(Throwable $exception): mixed
	public valid(): bool
	public __wakeup(): void
}
final class Fiber {
	public __construct(callable $callback)
	public start(mixed ...$args): mixed
	public resume(mixed $value = null): mixed
	public throw(Throwable $exception): mixed
	public getReturn(): mixed
	public isStarted(): bool
	public isSuspended(): bool
	public isRunning(): bool
	public isTerminated(): bool
	public static suspend(mixed $value = null): mixed
	public static getCurrent(): ?Fiber
}
final class WeakReference {
	public static create(object $object): WeakReference
	public get(): ?object
}
final class WeakMap implements ArrayAccess, Countable, IteratorAggregate {
	public count(): int
	public getIterator(): Iterator
	public offsetExists(object $object): bool
	public offsetGet(object $object): mixed
	public offsetSet(object $object, mixed $value): void
	public offsetUnset(object $object): void
}
interface Stringable {
	public __toString(): string
}
interface UnitEnum {
	public static cases(): array
}
interface BackedEnum extends UnitEnum {
	public static from(int|string $value): static
	public static tryFrom(int|string $value): ?static
}

// Predefined Exceptions
// https://www.php.net/manual/en/reserved.exceptions.php
class Exception implements Throwable {}
	class ErrorException extends Exception {
		final public getSeverity(): int
	}
class Error implements Throwable {}
	class ArithmeticError extends Error {}
		class DivisionByZeroError extends ArithmeticError {}
	class AssertionError extends Error {}
	class CompileError extends Error {}
		class ParseError extends CompileError {}
	class TypeError extends Error {}
		class ArgumentCountError extends TypeError {}
	class ValueError extends Error {}
	class UnhandledMatchError extends Error {}
	final class FiberError extends Error {}
}

{ // Affecting PHP's Behaviour
{ // Error Handling and Logging
E_ERROR
E_WARNING
E_PARSE
E_NOTICE
E_CORE_ERROR
E_CORE_WARNING
E_COMPILE_ERROR
E_COMPILE_WARNING
E_USER_ERROR
E_USER_WARNING
E_USER_NOTICE
E_RECOVERABLE_ERROR
E_DEPRECATED
E_USER_DEPRECATED
E_ALL

DEBUG_BACKTRACE_PROVIDE_OBJECT
DEBUG_BACKTRACE_IGNORE_ARGS

debug_backtrace(int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT, int $limit = 0): array
debug_print_backtrace(int $options = 0, int $limit = 0): void
error_clear_last(): void
error_get_last(): ?array
error_log(string $message, int $message_type = 0, ?string $destination = null, ?string $additional_headers = null): bool
error_reporting(?int $error_level = null): int
restore_error_handler(): bool
restore_exception_handler(): bool
set_error_handler(?callable $callback, int $error_levels = E_ALL): ?callable
set_exception_handler(?callable $callback): ?callable
trigger_error(string $message, int $error_level = E_USER_NOTICE): bool
user_error()
}

{ // Output Buffering Control
PHP_OUTPUT_HANDLER_START
PHP_OUTPUT_HANDLER_WRITE
PHP_OUTPUT_HANDLER_FLUSH
PHP_OUTPUT_HANDLER_CLEAN
PHP_OUTPUT_HANDLER_FINAL
PHP_OUTPUT_HANDLER_CONT
PHP_OUTPUT_HANDLER_END
PHP_OUTPUT_HANDLER_CLEANABLE
PHP_OUTPUT_HANDLER_FLUSHABLE
PHP_OUTPUT_HANDLER_REMOVABLE
PHP_OUTPUT_HANDLER_STDFLAGS

flush(): void
ob_clean(): bool
ob_end_clean(): bool
ob_end_flush(): bool
ob_flush(): bool
ob_get_clean(): string|false
ob_get_contents(): string|false
ob_get_flush(): string|false
ob_get_length(): int|false
ob_get_level(): int
ob_get_status(bool $full_status = false): array
ob_gzhandler(string $data, int $flags): string|false
ob_implicit_flush(bool $enable = true): void
ob_list_handlers(): array
ob_start(callable $callback = null, int $chunk_size = 0, int $flags = PHP_OUTPUT_HANDLER_STDFLAGS): bool
output_add_rewrite_var(string $name, string $value): bool
output_reset_rewrite_vars(): bool
}

{ // PHP Options and Information
// phpcredits()
CREDITS_GROUP
CREDITS_GENERAL
CREDITS_SAPI
CREDITS_MODULES
CREDITS_DOCS
CREDITS_FULLPAGE
CREDITS_QA
CREDITS_ALL
// phpinfo()
INFO_GENERAL
INFO_CREDITS
INFO_CONFIGURATION
INFO_MODULES
INFO_ENVIRONMENT
INFO_VARIABLES
INFO_LICENSE
INFO_ALL
// INI
INI_USER
INI_PERDIR
INI_SYSTEM
INI_ALL
// assert()
ASSERT_ACTIVE
ASSERT_CALLBACK
ASSERT_BAIL
ASSERT_WARNING
ASSERT_QUIET_EVAL
// Windows
PHP_WINDOWS_VERSION_MAJOR
PHP_WINDOWS_VERSION_MINOR
PHP_WINDOWS_VERSION_BUILD
PHP_WINDOWS_VERSION_PLATFORM
PHP_WINDOWS_VERSION_SP_MAJOR
PHP_WINDOWS_VERSION_SP_MINOR
PHP_WINDOWS_VERSION_SUITEMASK
PHP_WINDOWS_VERSION_PRODUCTTYPE
PHP_WINDOWS_NT_DOMAIN_CONTROLLER
PHP_WINDOWS_NT_SERVER
PHP_WINDOWS_NT_WORKSTATION

assert_options(int $what, mixed $value = ?): mixed
assert(mixed $assertion, string $description = ?): bool
assert(mixed $assertion, Throwable $exception = ?): bool
cli_get_process_title(): ?string
cli_set_process_title(string $title): bool
extension_loaded(string $extension): bool
gc_collect_cycles(): int
gc_disable(): void
gc_enable(): void
gc_enabled(): bool
gc_mem_caches(): int
gc_status(): array
get_cfg_var(string $option): string|array|false
get_current_user(): string
get_defined_constants(bool $categorize = false): array
get_extension_funcs(string $extension): array|false
get_include_path(): string|false
get_included_files(): array
get_loaded_extensions(bool $zend_extensions = false): array
get_required_files()
get_resources(?string $type = null): array
getenv(string $varname, bool $local_only = false): string|false
getenv(): array
getlastmod(): int|false
getmygid(): int|false
getmyinode(): int|false
getmypid(): int|false
getmyuid(): int|false
getopt(string $short_options, array $long_options = [], int &$rest_index = null): array|false
getrusage(int $mode = 0): array|false
ini_alter()
ini_get_all(?string $extension = null, bool $details = true): array|false
ini_get(string $option): string|false
ini_restore(string $option): void
ini_parse_quantity(string $shorthand): int
ini_set(string $option, string|int|float|bool|null $value): string|false
memory_get_peak_usage(bool $real_usage = false): int
memory_get_usage(bool $real_usage = false): int
memory_reset_peak_usage(): void
php_ini_loaded_file(): string|false
php_ini_scanned_files(): string|false
php_sapi_name(): string|false
php_uname(string $mode = "a"): string
phpcredits(int $flags = CREDITS_ALL): bool
phpinfo(int $flags = INFO_ALL): bool
phpversion(?string $extension = null): string|false
putenv(string $assignment): bool
set_include_path(string $include_path): string|false
set_time_limit(int $seconds): bool
sys_get_temp_dir(): string
version_compare(string $version1, string $version2, ?string $operator = null): int|bool
zend_thread_id(): int
zend_version(): string
}
}

{ // Cryptography Extensions
{ // CSPRNG
random_bytes(int $length): string
random_int(int $min, int $max): int
}

{ // HASH Message Digest Framework
HASH_HMAC

final class HashContext {
	public __serialize(): array
	public __unserialize(array $data): void
}

hash_algos(): array
hash_copy(HashContext $context): HashContext
hash_equals(string $known_string, string $user_string): bool
hash_file(string $algo, string $filename, bool $binary = false, array $options = []): string|false
hash_final(HashContext $context, bool $binary = false): string
hash_hkdf(string $algo, string $key, int $length = 0, string $info = "", string $salt = ""): string
hash_hmac_algos(): array
hash_hmac_file(string $algo, string $filename, string $key, bool $binary = false): string|false
hash_hmac(string $algo, string $data, string $key, bool $binary = false): string
hash_init(string $algo, int $flags = 0, string $key = "", array $options = []): HashContext
hash_pbkdf2(string $algo, string $password, string $salt, int $iterations, int $length = 0, bool $binary = false): string
hash_update_file(HashContext $context, string $filename, ?resource $stream_context = null): bool
hash_update_stream(HashContext $context, resource $stream, int $length = -1): int
hash_update(HashContext $context, string $data): bool
hash(string $algo, string $data, bool $binary = false, array $options = []): string
}

{ // Password Hashing
PASSWORD_BCRYPT
PASSWORD_ARGON2I
PASSWORD_ARGON2ID
PASSWORD_ARGON2_DEFAULT_MEMORY_COST
PASSWORD_ARGON2_DEFAULT_TIME_COST
PASSWORD_ARGON2_DEFAULT_THREADS
PASSWORD_DEFAULT

password_algos(): array
password_get_info(string $hash): array
password_hash(string $password, string|int|null $algo, array $options = []): string
password_needs_rehash(string $hash, string|int|null $algo, array $options = []): bool
password_verify(string $password, string $hash): bool
}
}

{ // Database Extensions
{ // PHP Data Objects
class PDO {
	const int PARAM_BOOL;
	const int PARAM_NULL;
	const int PARAM_INT;
	const int PARAM_STR;
	const int PARAM_STR_NATL;
	const int PARAM_STR_CHAR;
	const int PARAM_LOB;
	const int PARAM_STMT;
	const int PARAM_INPUT_OUTPUT;

	const int FETCH_DEFAULT;
	const int FETCH_LAZY;
	const int FETCH_ASSOC;
	const int FETCH_NAMED;
	const int FETCH_NUM;
	const int FETCH_BOTH ;
	const int FETCH_OBJ;
	const int FETCH_BOUND;
	const int FETCH_COLUMN;
	const int FETCH_CLASS;
	const int FETCH_INTO;
	const int FETCH_FUNC;
	const int FETCH_GROUP;
	const int FETCH_UNIQUE;
	const int FETCH_KEY_PAIR;
	const int FETCH_CLASSTYPE;
	const int FETCH_SERIALIZE;
	const int FETCH_PROPS_LATE;

	const int ATTR_AUTOCOMMIT;
	const int ATTR_PREFETCH;
	const int ATTR_TIMEOUT;
	const int ATTR_ERRMODE;
	const int ATTR_SERVER_VERSION;
	const int ATTR_CLIENT_VERSION;
	const int ATTR_SERVER_INFO;
	const int ATTR_CONNECTION_STATUS;
	const int ATTR_CASE;
	const int ATTR_CURSOR_NAME;
	const int ATTR_CURSOR;
	const int ATTR_DRIVER_NAME;
	const int ATTR_ORACLE_NULLS;
	const int ATTR_PERSISTENT;
	const int ATTR_STATEMENT_CLASS;
	const int ATTR_FETCH_CATALOG_NAMES;
	const int ATTR_FETCH_TABLE_NAMES ;
	const int ATTR_STRINGIFY_FETCHES;
	const int ATTR_MAX_COLUMN_LEN;
	const int ATTR_DEFAULT_FETCH_MODE;
	const int ATTR_EMULATE_PREPARES;
	const int ATTR_DEFAULT_STR_PARAM;

	const int ERRMODE_SILENT;
	const int ERRMODE_WARNING;
	const int ERRMODE_EXCEPTION;
	const int CASE_NATURAL;
	const int CASE_UPPER;
	const int NULL_NATURAL;
	const int NULL_EMPTY_STRING;
	const int NULL_TO_STRING;
	const int FETCH_ORI_NEXT;
	const int FETCH_ORI_PRIOR;
	const int FETCH_ORI_FIRST;
	const int FETCH_ORI_LAST;
	const int FETCH_ORI_ABS;
	const int FETCH_ORI_REL;
	const int CURSOR_FWDONLY;
	const int CURSOR_SCROLL;
	const string ERR_NONE;

	const int PARAM_EVT_ALLOC;
	const int PARAM_EVT_FREE;
	const int PARAM_EVT_EXEC_PRE;
	const int PARAM_EVT_EXEC_POST;
	const int PARAM_EVT_FETCH_PRE;
	const int PARAM_EVT_FETCH_POST;
	const int PARAM_EVT_NORMALIZE;
	const int SQLITE_DETERMINISTIC;

	public __construct(string $dsn, ?string $username = null, ?string $password = null, ?array $options = null)
	public beginTransaction(): bool
	public commit(): bool
	public errorCode(): ?string
	public errorInfo(): array
	public exec(string $statement): int|false
	public getAttribute(int $attribute): mixed
	public static getAvailableDrivers(): array
	public inTransaction(): bool
	public lastInsertId(?string $name = null): string|false
	public prepare(string $query, array $options = []): PDOStatement|false
	public query(string $query, ?int $fetchMode = null): PDOStatement|false
	public query(string $query, ?int $fetchMode = PDO::FETCH_COLUMN, int $colno): PDOStatement|false
	public query(string $query, ?int $fetchMode = PDO::FETCH_CLASS, string $classname, array $constructorArgs): PDOStatement|false
	public query(string $query, ?int $fetchMode = PDO::FETCH_INTO, object $object): PDOStatement|false
	public quote(string $string, int $type = PDO::PARAM_STR): string|false
	public rollBack(): bool
	public setAttribute(int $attribute, mixed $value): bool
}
class PDOStatement implements IteratorAggregate {
	public string $queryString;
	public bindColumn(string|int $column, mixed &$var, int $type = PDO::PARAM_STR, int $maxLength = 0, mixed $driverOptions = null): bool
	public bindParam(string|int $param, mixed &$var, int $type = PDO::PARAM_STR, int $maxLength = 0, mixed $driverOptions = null): bool
	public bindValue(string|int $param, mixed $value, int $type = PDO::PARAM_STR): bool
	public closeCursor(): bool
	public columnCount(): int
	public debugDumpParams(): ?bool
	public errorCode(): ?string
	public errorInfo(): array
	public execute(?array $params = null): bool
	public fetch(int $mode = PDO::FETCH_DEFAULT, int $cursorOrientation = PDO::FETCH_ORI_NEXT, int $cursorOffset = 0): mixed
	public fetchAll(int $mode = PDO::FETCH_DEFAULT): array
	public fetchAll(int $mode = PDO::FETCH_COLUMN, int $column): array
	public fetchAll(int $mode = PDO::FETCH_CLASS, string $class, ?array $constructorArgs): array
	public fetchAll(int $mode = PDO::FETCH_FUNC, callable $callback): array
	public fetchColumn(int $column = 0): mixed
	public fetchObject(?string $class = "stdClass", array $constructorArgs = []): object|false
	public getAttribute(int $name): mixed
	public getColumnMeta(int $column): array|false
	public getIterator(): Iterator
	public nextRowset(): bool
	public rowCount(): int
	public setAttribute(int $attribute, mixed $value): bool
	public setFetchMode(int $mode): bool
	public setFetchMode(int $mode = PDO::FETCH_COLUMN, int $colno): bool
	public setFetchMode(int $mode = PDO::FETCH_CLASS, string $class, ?array $constructorArgs = null): bool
	public setFetchMode(int $mode = PDO::FETCH_INTO, object $object): bool
}
class PDOException extends RuntimeException {
	protected int|string $code;
	public ?array $errorInfo = null;
}
}

{ // MySQL Improved Extension
MYSQLI_READ_DEFAULT_GROUP
MYSQLI_READ_DEFAULT_FILE
MYSQLI_OPT_CONNECT_TIMEOUT
MYSQLI_OPT_READ_TIMEOUT
MYSQLI_OPT_LOCAL_INFILE
MYSQLI_OPT_INT_AND_FLOAT_NATIVE
MYSQLI_OPT_NET_CMD_BUFFER_SIZE
MYSQLI_OPT_NET_READ_BUFFER_SIZE
MYSQLI_OPT_SSL_VERIFY_SERVER_CERT
MYSQLI_INIT_COMMAND
MYSQLI_CLIENT_SSL
MYSQLI_CLIENT_COMPRESS
MYSQLI_CLIENT_INTERACTIVE
MYSQLI_CLIENT_IGNORE_SPACE
MYSQLI_CLIENT_NO_SCHEMA
MYSQLI_CLIENT_MULTI_QUERIES
MYSQLI_STORE_RESULT
MYSQLI_USE_RESULT
MYSQLI_ASSOC
MYSQLI_NUM
MYSQLI_BOTH
MYSQLI_NOT_NULL_FLAG
MYSQLI_PRI_KEY_FLAG
MYSQLI_UNIQUE_KEY_FLAG
MYSQLI_MULTIPLE_KEY_FLAG
MYSQLI_BLOB_FLAG
MYSQLI_UNSIGNED_FLAG
MYSQLI_ZEROFILL_FLAG
MYSQLI_AUTO_INCREMENT_FLAG
MYSQLI_TIMESTAMP_FLAG
MYSQLI_SET_FLAG
MYSQLI_NUM_FLAG
MYSQLI_PART_KEY_FLAG
MYSQLI_GROUP_FLAG
MYSQLI_TYPE_DECIMAL
MYSQLI_TYPE_NEWDECIMAL
MYSQLI_TYPE_BIT
MYSQLI_TYPE_TINY
MYSQLI_TYPE_SHORT
MYSQLI_TYPE_LONG
MYSQLI_TYPE_FLOAT
MYSQLI_TYPE_DOUBLE
MYSQLI_TYPE_NULL
MYSQLI_TYPE_TIMESTAMP
MYSQLI_TYPE_LONGLONG
MYSQLI_TYPE_INT24
MYSQLI_TYPE_DATE
MYSQLI_TYPE_TIME
MYSQLI_TYPE_DATETIME
MYSQLI_TYPE_YEAR
MYSQLI_TYPE_NEWDATE
MYSQLI_TYPE_INTERVAL
MYSQLI_TYPE_ENUM
MYSQLI_TYPE_SET
MYSQLI_TYPE_TINY_BLOB
MYSQLI_TYPE_MEDIUM_BLOB
MYSQLI_TYPE_LONG_BLOB
MYSQLI_TYPE_BLOB
MYSQLI_TYPE_VAR_STRING
MYSQLI_TYPE_STRING
MYSQLI_TYPE_CHAR
MYSQLI_TYPE_GEOMETRY
MYSQLI_TYPE_JSON
MYSQLI_NEED_DATA
MYSQLI_NO_DATA
MYSQLI_DATA_TRUNCATED
MYSQLI_ENUM_FLAG
MYSQLI_BINARY_FLAG
MYSQLI_CURSOR_TYPE_FOR_UPDATE
MYSQLI_CURSOR_TYPE_NO_CURSOR
MYSQLI_CURSOR_TYPE_READ_ONLY
MYSQLI_CURSOR_TYPE_SCROLLABLE
MYSQLI_STMT_ATTR_CURSOR_TYPE
MYSQLI_STMT_ATTR_PREFETCH_ROWS
MYSQLI_STMT_ATTR_UPDATE_MAX_LENGTH
MYSQLI_SET_CHARSET_NAME
MYSQLI_REPORT_INDEX
MYSQLI_REPORT_ERROR
MYSQLI_REPORT_STRICT
MYSQLI_REPORT_ALL
MYSQLI_REPORT_OFF
MYSQLI_DEBUG_TRACE_ENABLED
MYSQLI_SERVER_QUERY_NO_GOOD_INDEX_USED
MYSQLI_SERVER_QUERY_NO_INDEX_USED
MYSQLI_SERVER_PUBLIC_KEY
MYSQLI_REFRESH_GRANT
MYSQLI_REFRESH_LOG
MYSQLI_REFRESH_TABLES
MYSQLI_REFRESH_HOSTS
MYSQLI_REFRESH_REPLICA
MYSQLI_REFRESH_STATUS
MYSQLI_REFRESH_THREADS
MYSQLI_REFRESH_SLAVE
MYSQLI_REFRESH_MASTER
MYSQLI_TRANS_COR_AND_CHAIN
MYSQLI_TRANS_COR_AND_NO_CHAIN
MYSQLI_TRANS_COR_RELEASE
MYSQLI_TRANS_COR_NO_RELEASE
MYSQLI_TRANS_START_READ_ONLY
MYSQLI_TRANS_START_READ_WRITE
MYSQLI_TRANS_START_CONSISTENT_SNAPSHOT
MYSQLI_CLIENT_SSL_DONT_VERIFY_SERVER_CERT
MYSQLI_IS_MARIADB

class mysqli {
	public int|string $affected_rows;
	public string $client_info;
	public int $client_version;
	public int $connect_errno;
	public ?string $connect_error;
	public int $errno;
	public string $error;
	public array $error_list;
	public int $field_count;
	public string $host_info;
	public ?string $info;
	public int|string $insert_id;
	public string $server_info;
	public int $server_version;
	public string $sqlstate;
	public int $protocol_version;
	public int $thread_id;
	public int $warning_count;

	public __construct(string $hostname =, string $username =, string $password =, string $database = "", int $port =, string $socket =)
	public autocommit(bool $enable): bool
	public begin_transaction(int $flags = 0, ?string $name = null): bool
	public change_user(string $username, string $password, ?string $database): bool
	public character_set_name(): string
	public close(): bool
	public commit(int $flags = 0, ?string $name = null): bool
	public connect(string $hostname =, string $username =, string $password =, string $database = "", int $port =, string $socket =): void
	public debug(string $options): bool
	public dump_debug_info(): bool
	public execute_query(string $query, ?array $params = null): mysqli_result|bool
	public get_charset(): ?object
	public get_client_info(): string
	public get_connection_stats(): array
	public get_server_info(): string
	public get_warnings(): mysqli_warning|false
	public kill(int $process_id): bool
	public more_results(): bool
	public multi_query(string $query): bool
	public next_result(): bool
	public options(int $option, string|int $value): bool
	public ping(): bool
	public static poll(?array &$read, ?array &$error, array &$reject, int $seconds, int $microseconds = 0): int|false
	public prepare(string $query): mysqli_stmt|false
	public query(string $query, int $result_mode = MYSQLI_STORE_RESULT): mysqli_result|bool
	public real_connect(string $host = ?, string $username = ?, string $passwd = ?, string $dbname = ?, int $port = ?, string $socket = ?, int $flags = ?): bool
	public real_escape_string(string $string): string
	public real_query(string $query): bool
	public reap_async_query(): mysqli_result|bool
	public refresh(int $flags): bool
	public release_savepoint(string $name): bool
	public rollback(int $flags = 0, ?string $name = null): bool
	public savepoint(string $name): bool
	public select_db(string $database): bool
	public set_charset(string $charset): bool
	public ssl_set(?string $key, ?string $certificate, ?string $ca_certificate, ?string $ca_path, ?string $cipher_algos): bool
	public stat(): string|false
	public stmt_init(): mysqli_stmt|false
	public store_result(int $mode = 0): mysqli_result|false
	public thread_safe(): bool
	public use_result(): mysqli_result|false
}
class mysqli_stmt {
	public int|string $affected_rows;
	public int|string $insert_id;
	public int|string $num_rows;
	public int $param_count;
	public int $field_count;
	public int $errno;
	public string $error;
	public array $error_list;
	public string $sqlstate;
	public int $id;

	public __construct(mysqli $mysql, ?string $query = null)
	public attr_get(int $attribute): int
	public attr_set(int $attribute, int $value): bool
	public bind_param(string $types, mixed &$var, mixed &...$vars): bool
	public bind_result(mixed &$var, mixed &...$vars): bool
	public close(): bool
	public data_seek(int $offset): void
	public execute(?array $params = null): bool
	public fetch(): ?bool
	public free_result(): void
	public get_result(): mysqli_result|false
	public get_warnings(): mysqli_warning|false
	public more_results(): bool
	public next_result(): bool
	public num_rows(): int|string
	public prepare(string $query): bool
	public reset(): bool
	public result_metadata(): mysqli_result|false
	public send_long_data(int $param_num, string $data): bool
	public store_result(): bool
}
class mysqli_result implements IteratorAggregate {
	public int $current_field;
	public int $field_count;
	public ?array $lengths;
	public int|string $num_rows;
	public int $type;

	public __construct(mysqli $mysql, int $result_mode = MYSQLI_STORE_RESULT)
	public data_seek(int $offset): bool
	public fetch_all(int $mode = MYSQLI_NUM): array
	public fetch_array(int $mode = MYSQLI_BOTH): array|null|false
	public fetch_assoc(): array|null|false
	public fetch_column(int $column = 0): null|int|float|string|false
	public fetch_field_direct(int $index): object|false
	public fetch_field(): object|false
	public fetch_fields(): array
	public fetch_object(string $class = "stdClass", array $constructor_args = []): object|null|false
	public fetch_row(): array|null|false
	public field_seek(int $index): bool
	public free(): void
	public close(): void
	public free_result(): void
	public getIterator(): Iterator
}
final class mysqli_driver {
	public readonly string $client_info;
	public readonly int $client_version;
	public readonly int $driver_version;
	public readonly bool $embedded;
	public bool $reconnect = false;
	public int $report_mode;
}
final class mysqli_warning {
	public string $message;
	public string $sqlstate;
	public int $errno;

	public next(): bool
}
final class mysqli_sql_exception extends RuntimeException {
	protected string $sqlstate = "00000";
	public getSqlState(): string
}
}

{ // SQLite3
SQLITE3_ASSOC
SQLITE3_NUM
SQLITE3_BOTH
SQLITE3_INTEGER
SQLITE3_FLOAT
SQLITE3_TEXT
SQLITE3_BLOB
SQLITE3_NULL
SQLITE3_OPEN_READONLY
SQLITE3_OPEN_READWRITE
SQLITE3_OPEN_CREATE
SQLITE3_DETERMINISTIC

class SQLite3 {
	public __construct(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, string $encryptionKey = "")
	public backup(SQLite3 $destination, string $sourceDatabase = "main", string $destinationDatabase = "main"): bool
	public busyTimeout(int $milliseconds): bool
	public changes(): int
	public close(): bool
	public createAggregate(string $name, callable $stepCallback, callable $finalCallback, int $argCount = -1): bool
	public createCollation(string $name, callable $callback): bool
	public createFunction(string $name, callable $callback, int $argCount = -1, int $flags = 0): bool
	public enableExceptions(bool $enable = false): bool
	public static escapeString(string $string): string
	public exec(string $query): bool
	public lastErrorCode(): int
	public lastErrorMsg(): string
	public lastInsertRowID(): int
	public loadExtension(string $name): bool
	public open(string $filename, int $flags = SQLITE3_OPEN_READWRITE | SQLITE3_OPEN_CREATE, string $encryptionKey = ""): void
	public openBlob(string $table, string $column, int $rowid, string $database = "main", int $flags = SQLITE3_OPEN_READONLY): resource|false
	public prepare(string $query): SQLite3Stmt|false
	public query(string $query): SQLite3Result|false
	public querySingle(string $query, bool $entireRow = false): mixed
	public setAuthorizer(?callable $callback): bool
	public static version(): array
}
class SQLite3Stmt {
	private __construct(SQLite3 $sqlite3, string $query)
	public bindParam(string|int $param, mixed &$var, int $type = SQLITE3_TEXT): bool
	public bindValue(string|int $param, mixed $value, int $type = SQLITE3_TEXT): bool
	public clear(): bool
	public close(): bool
	public execute(): SQLite3Result|false
	public getSQL(bool $expand = false): string|false
	public paramCount(): int
	public readOnly(): bool
	public reset(): bool
}
class SQLite3Result {
	public columnName(int $column): string|false
	public columnType(int $column): int|false
	public fetchArray(int $mode = SQLITE3_BOTH): array|false
	public finalize(): bool
	public numColumns(): int
	public reset(): bool
}
}
}

{ // Date and Time Related Extensions
{ // Date and Time
class DateTime implements DateTimeInterface {
	public __construct(string $datetime = "now", ?DateTimeZone $timezone = null)
	public add(DateInterval $interval): DateTime
	public static createFromFormat(string $format, string $datetime, ?DateTimeZone $timezone = null): DateTime|false
	public static createFromImmutable(DateTimeImmutable $object): DateTime
	public static createFromInterface(DateTimeInterface $object): DateTime
	public static getLastErrors(): array|false
	public modify(string $modifier): DateTime|false
	public static __set_state(array $array): DateTime
	public setDate(int $year, int $month, int $day): DateTime
	public setISODate(int $year, int $week, int $dayOfWeek = 1): DateTime
	public setTime(int $hour, int $minute, int $second = 0, int $microsecond = 0): DateTime
	public setTimestamp(int $timestamp): DateTime
	public setTimezone(DateTimeZone $timezone): DateTime
	public sub(DateInterval $interval): DateTime
	public diff(DateTimeInterface $targetObject, bool $absolute = false): DateInterval
	public format(string $format): string
	public getOffset(): int
	public getTimestamp(): int
	public getTimezone(): DateTimeZone|false
	public __wakeup(): void
}
class DateTimeImmutable implements DateTimeInterface {
	public __construct(string $datetime = "now", ?DateTimeZone $timezone = null)
	public add(DateInterval $interval): DateTimeImmutable
	public static createFromFormat(string $format, string $datetime, ?DateTimeZone $timezone = null): DateTimeImmutable|false
	public static createFromInterface(DateTimeInterface $object): DateTimeImmutable
	public static createFromMutable(DateTime $object): DateTimeImmutable
	public static getLastErrors(): array|false
	public modify(string $modifier): DateTimeImmutable|false
	public static __set_state(array $array): DateTimeImmutable
	public setDate(int $year, int $month, int $day): DateTimeImmutable
	public setISODate(int $year, int $week, int $dayOfWeek = 1): DateTimeImmutable
	public setTime(int $hour, int $minute, int $second = 0, int $microsecond = 0): DateTimeImmutable
	public setTimestamp(int $timestamp): DateTimeImmutable
	public setTimezone(DateTimeZone $timezone): DateTimeImmutable
	public sub(DateInterval $interval): DateTimeImmutable
	public diff(DateTimeInterface $targetObject, bool $absolute = false): DateInterval
	public format(string $format): string
	public getOffset(): int
	public getTimestamp(): int
	public getTimezone(): DateTimeZone|false
	public __wakeup(): void
}
interface DateTimeInterface {
	const string ATOM;
	const string COOKIE;
	const string ISO8601;
	const string RSS;
	const string W3C;

	public diff(DateTimeInterface $targetObject, bool $absolute = false): DateInterval
	public format(string $format): string
	public getOffset(): int
	public getTimestamp(): int
	public getTimezone(): DateTimeZone|false
	public __wakeup(): void
}
class DateTimeZone {
	const int AFRICA;
	const int AMERICA;
	const int ANTARCTICA;
	const int ARCTIC;
	const int ASIA;
	const int ATLANTIC;
	const int AUSTRALIA;
	const int EUROPE;
	const int INDIAN;
	const int PACIFIC;
	const int UTC;
	const int ALL;
	const int ALL_WITH_BC;
	const int PER_COUNTRY;

	public __construct(string $timezone)
	public getLocation(): array|false
	public getName(): string
	public getOffset(DateTimeInterface $datetime): int
	public getTransitions(int $timestampBegin = PHP_INT_MIN, int $timestampEnd = PHP_INT_MAX): array|false
	public static listAbbreviations(): array
	public static listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null): array
}
class DateInterval {
	public int $invert;
	public mixed $days;

	public __construct(string $duration)
	public static createFromDateString(string $datetime): DateInterval|false
	public format(string $format): string
}
class DatePeriod implements IteratorAggregate {
const int EXCLUDE_START_DATE = 1;
public int $recurrences;
public bool $include_start_date;
public DateTimeInterface $start;
public DateTimeInterface $current;
public DateTimeInterface $end;
public DateInterval $interval;

public __construct(DateTimeInterface $start, DateInterval $interval, int $recurrences, int $options = 0)
public __construct(DateTimeInterface $start, DateInterval $interval, DateTimeInterface $end, int $options = 0)
public __construct(string $isostr, int $options = 0)
public getDateInterval(): DateInterval
public getEndDate(): ?DateTimeInterface
public getRecurrences(): ?int
public getStartDate(): DateTimeInterface
}

checkdate(int $month, int $day, int $year): bool
date_add()
date_create_from_format()
date_create_immutable_from_format()
date_create_immutable()
date_create()
date_date_set()
date_default_timezone_get(): string
date_default_timezone_set(string $timezoneId): bool
date_diff()
date_format()
date_get_last_errors()
date_interval_create_from_date_string()
date_interval_format()
date_isodate_set()
date_modify()
date_offset_get()
date_parse_from_format(string $format, string $datetime): array
date_parse(string $datetime): array
date_sub()
date_sun_info(int $timestamp, float $latitude, float $longitude): array
date_time_set()
date_timestamp_get()
date_timestamp_set()
date_timezone_get()
date_timezone_set()
date(string $format, ?int $timestamp = null): string
getdate(?int $timestamp = null): array
gettimeofday(bool $as_float = false): array|float
gmdate(string $format, ?int $timestamp = null): string
gmmktime(int $hour, ?int $minute = null, ?int $second = null, ?int $month = null, ?int $day = null, ?int $year = null): int|false
idate(string $format, ?int $timestamp = null): int|false
localtime(?int $timestamp = null, bool $associative = false): array
microtime(bool $as_float = false): string|float
mktime(int $hour, ?int $minute = null, ?int $second = null, ?int $month = null, ?int $day = null, ?int $year = null): int|false
strtotime(string $datetime, ?int $baseTimestamp = null): int|false
time(): int
timezone_abbreviations_list()
timezone_identifiers_list()
timezone_location_get()
timezone_name_from_abbr(string $abbr, int $utcOffset = -1, int $isDST = -1): string|false
timezone_name_get()
timezone_offset_get()
timezone_open()
timezone_transitions_get()
timezone_version_get(): string
}
}

{ // File System Related Extensions
{ // Directories
DIRECTORY_SEPARATOR
PATH_SEPARATOR
SCANDIR_SORT_ASCENDING
SCANDIR_SORT_DESCENDING
SCANDIR_SORT_NONE

class Directory {
	public readonly string $path;
	public readonly resource $handle;
	public close(): void
	public read(): string|false
	public rewind(): void
}

chdir(string $directory): bool
chroot(string $directory): bool
closedir(?resource $dir_handle = null): void
dir(string $directory, ?resource $context = null): Directory|false
getcwd(): string|false
opendir(string $directory, ?resource $context = null): resource|false
readdir(?resource $dir_handle = null): string|false
rewinddir(?resource $dir_handle = null): void
scandir(string $directory, int $sorting_order = SCANDIR_SORT_ASCENDING, ?resource $context = null): array|false
}

{ // Filesystem
SEEK_SET
SEEK_CUR
SEEK_END
LOCK_SH
LOCK_EX
LOCK_UN
LOCK_NB
GLOB_BRACE
GLOB_ONLYDIR
GLOB_MARK
GLOB_NOSORT
GLOB_NOCHECK
GLOB_NOESCAPE
GLOB_AVAILABLE_FLAGS
PATHINFO_DIRNAME
PATHINFO_BASENAME
PATHINFO_EXTENSION
PATHINFO_FILENAME
FILE_USE_INCLUDE_PATH
FILE_NO_DEFAULT_CONTEXT
FILE_APPEND
FILE_IGNORE_NEW_LINES
FILE_SKIP_EMPTY_LINES
FILE_BINARY
FILE_TEXT
INI_SCANNER_NORMAL
INI_SCANNER_RAW
INI_SCANNER_TYPED
FNM_NOESCAPE
FNM_PATHNAME
FNM_PERIOD
FNM_CASEFOLD

basename(string $path, string $suffix = ""): string
chgrp(string $filename, string|int $group): bool
chmod(string $filename, int $permissions): bool
chown(string $filename, string|int $user): bool
clearstatcache(bool $clear_realpath_cache = false, string $filename = ""): void
copy(string $from, string $to, ?resource $context = null): bool
dirname(string $path, int $levels = 1): string
disk_free_space(string $directory): float|false
disk_total_space(string $directory): float|false
diskfreespace()
fclose(resource $stream): bool
fdatasync(resource $stream): bool
feof(resource $stream): bool
fflush(resource $stream): bool
fgetc(resource $stream): string|false
fgetcsv(resource $stream, ?int $length = null, string $separator = ",", string $enclosure = "\"", string $escape = "\\"): array|false
fgets(resource $stream, ?int $length = null): string|false
file_exists(string $filename): bool
file_get_contents(string $filename, bool $use_include_path = false, ?resource $context = null, int $offset = 0, ?int $length = null): string|false
file_put_contents(string $filename, mixed $data, int $flags = 0, ?resource $context = null): int|false
file(string $filename, int $flags = 0, ?resource $context = null): array|false
fileatime(string $filename): int|false
filectime(string $filename): int|false
filegroup(string $filename): int|false
fileinode(string $filename): int|false
filemtime(string $filename): int|false
fileowner(string $filename): int|false
fileperms(string $filename): int|false
filesize(string $filename): int|false
filetype(string $filename): string|false
flock(resource $stream, int $operation, int &$would_block = null): bool
fnmatch(string $pattern, string $filename, int $flags = 0): bool
fopen(string $filename, string $mode, bool $use_include_path = false, ?resource $context = null): resource|false
fpassthru(resource $stream): int
fputcsv(resource $stream, array $fields, string $separator = ",", string $enclosure = "\"", string $escape = "\\", string $eol = "\n"): int|false
fputs()
fread(resource $stream, int $length): string|false
fscanf(resource $stream, string $format, mixed &...$vars): array|int|false|null
fseek(resource $stream, int $offset, int $whence = SEEK_SET): int
fstat(resource $stream): array|false
fsync(resource $stream): bool
ftell(resource $stream): int|false
ftruncate(resource $stream, int $size): bool
fwrite(resource $stream, string $data, ?int $length = null): int|false
glob(string $pattern, int $flags = 0): array|false
is_dir(string $filename): bool
is_executable(string $filename): bool
is_file(string $filename): bool
is_link(string $filename): bool
is_readable(string $filename): bool
is_uploaded_file(string $filename): bool
is_writable(string $filename): bool
is_writeable()
lchgrp(string $filename, string|int $group): bool
lchown(string $filename, string|int $user): bool
link(string $target, string $link): bool
linkinfo(string $path): int|false
lstat(string $filename): array|false
mkdir(string $directory, int $permissions = 0777, bool $recursive = false, ?resource $context = null): bool
move_uploaded_file(string $from, string $to): bool
parse_ini_file(string $filename, bool $process_sections = false, int $scanner_mode = INI_SCANNER_NORMAL): array|false
parse_ini_string(string $ini_string, bool $process_sections = false, int $scanner_mode = INI_SCANNER_NORMAL): array|false
pathinfo(string $path, int $flags = PATHINFO_ALL): array|string
pclose(resource $handle): int
popen(string $command, string $mode): resource|false
readfile(string $filename, bool $use_include_path = false, ?resource $context = null): int|false
readlink(string $path): string|false
realpath_cache_get(): array
realpath_cache_size(): int
realpath(string $path): string|false
rename(string $from, string $to, ?resource $context = null): bool
rewind(resource $stream): bool
rmdir(string $directory, ?resource $context = null): bool
set_file_buffer()
stat(string $filename): array|false
symlink(string $target, string $link): bool
tempnam(string $directory, string $prefix): string|false
tmpfile(): resource|false
touch(string $filename, ?int $mtime = null, ?int $atime = null): bool
umask(?int $mask = null): int
unlink(string $filename, ?resource $context = null): bool
}
}

{ // Human Language and Character Encoding Support
{ // iconv
ICONV_IMPL
ICONV_VERSION
ICONV_MIME_DECODE_STRICT
ICONV_MIME_DECODE_CONTINUE_ON_ERROR

iconv_get_encoding(string $type = "all"): array|string|false
iconv_mime_decode_headers(string $headers, int $mode = 0, ?string $encoding = null): array|false
iconv_mime_decode(string $string, int $mode = 0, ?string $encoding = null): string|false
iconv_mime_encode(string $field_name, string $field_value, array $options = []): string|false
iconv_set_encoding(string $type, string $encoding): bool
iconv_strlen(string $string, ?string $encoding = null): int|false
iconv_strpos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false
iconv_strrpos(string $haystack, string $needle, ?string $encoding = null): int|false
iconv_substr(string $string, int $offset, ?int $length = null, ?string $encoding = null): string|false
iconv(string $from_encoding, string $to_encoding, string $string): string|false
ob_iconv_handler(string $contents, int $status): string
}

{ // Multibyte String
MB_CASE_UPPER
MB_CASE_LOWER
MB_CASE_TITLE
MB_CASE_FOLD
MB_CASE_LOWER_SIMPLE
MB_CASE_UPPER_SIMPLE
MB_CASE_TITLE_SIMPLE
MB_CASE_FOLD_SIMPLE
MB_ONIGURUMA_VERSION

mb_check_encoding(array|string|null $value = null, ?string $encoding = null): bool
mb_chr(int $codepoint, ?string $encoding = null): string|false
mb_convert_case(string $string, int $mode, ?string $encoding = null): string
mb_convert_encoding(array|string $string, string $to_encoding, array|string|null $from_encoding = null): array|string|false
mb_convert_kana(string $string, string $mode = "KV", ?string $encoding = null): string
mb_convert_variables(string $to_encoding, array|string $from_encoding, mixed &$var, mixed &...$vars): string|false
mb_decode_mimeheader(string $string): string
mb_decode_numericentity(string $string, array $map, ?string $encoding = null): string
mb_detect_encoding(string $string, array|string|null $encodings = null, bool $strict = false): string|false
mb_detect_order(array|string|null $encoding = null): array|bool
mb_encode_mimeheader(string $string, ?string $charset = null, ?string $transfer_encoding = null, string $newline = "\r\n", int $indent = 0): string
mb_encode_numericentity(string $string, array $map, ?string $encoding = null, bool $hex = false): string
mb_encoding_aliases(string $encoding): array
mb_ereg_match(string $pattern, string $string, ?string $options = null): bool
mb_ereg_replace_callback(string $pattern, callable $callback, string $string, ?string $options = null): string|false|null
mb_ereg_replace(string $pattern, string $replacement, string $string, ?string $options = null): string|false|null
mb_ereg_search_getpos(): int
mb_ereg_search_getregs(): array|false
mb_ereg_search_init(string $string, ?string $pattern = null, ?string $options = null): bool
mb_ereg_search_pos(?string $pattern = null, ?string $options = null): array|false
mb_ereg_search_regs(?string $pattern = null, ?string $options = null): array|false
mb_ereg_search_setpos(int $offset): bool
mb_ereg_search(?string $pattern = null, ?string $options = null): bool
mb_ereg(string $pattern, string $string, array &$matches = null): bool
mb_eregi_replace(string $pattern, string $replacement, string $string, ?string $options = null): string|false|null
mb_eregi(string $pattern, string $string, array &$matches = null): bool
mb_get_info(string $type = "all"): array|string|int|false
mb_http_input(?string $type = null): array|string|false
mb_http_output(?string $encoding = null): string|bool
mb_internal_encoding(?string $encoding = null): string|bool
mb_language(?string $language = null): string|bool
mb_lcfirst(string $string, ?string $encoding = null): string
mb_list_encodings(): array
mb_ltrim(string $string, ?string $characters = null, ?string $encoding = null): string
mb_ord(string $string, ?string $encoding = null): int|false
mb_output_handler(string $string, int $status): string
mb_parse_str(string $string, array &$result): bool
mb_preferred_mime_name(string $encoding): string|false
mb_regex_encoding(?string $encoding = null): string|bool
mb_regex_set_options(?string $options = null): string
mb_rtrim(string $string, ?string $characters = null, ?string $encoding = null): string
mb_send_mail(string $to, string $subject, string $message, array|string $additional_headers = [], ?string $additional_params = null): bool
mb_split(string $pattern, string $string, int $limit = -1): array|false
mb_str_pad(string $string, int $length, string $pad_string = " ", int $pad_type = STR_PAD_RIGHT, ?string $encoding = null): string
mb_str_split(string $string, int $length = 1, ?string $encoding = null): array
mb_strcut(string $string, int $start, ?int $length = null, ?string $encoding = null): string
mb_strimwidth(string $string, int $start, int $width, string $trim_marker = "", ?string $encoding = null): string
mb_stripos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false
mb_stristr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false
mb_strlen(string $string, ?string $encoding = null): int
mb_strpos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false
mb_strrchr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false
mb_strrichr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false
mb_strripos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false
mb_strrpos(string $haystack, string $needle, int $offset = 0, ?string $encoding = null): int|false
mb_strstr(string $haystack, string $needle, bool $before_needle = false, ?string $encoding = null): string|false
mb_strtolower(string $string, ?string $encoding = null): string
mb_strtoupper(string $string, ?string $encoding = null): string
mb_strwidth(string $string, ?string $encoding = null): int
mb_substitute_character(string|int|null $substitute_character = null): string|int|bool
mb_substr_count(string $haystack, string $needle, ?string $encoding = null): int
mb_substr(string $string, int $start, ?int $length = null, ?string $encoding = null): string
mb_trim(string $string, ?string $characters = null, ?string $encoding = null): string
mb_ucfirst(string $string, ?string $encoding = null): string
}
}

{ // Mail Related Extensions
// Mail
mail(string $to, string $subject, string $message, array|string $additional_headers = [], string $additional_params = ""): bool
}

{ // Mathematical Extensions
{ // BCMath Arbitrary Precision Mathematics
bcadd(string $num1, string $num2, ?int $scale = null): string
bccomp(string $num1, string $num2, ?int $scale = null): int
bcdiv(string $num1, string $num2, ?int $scale = null): string
bcdivmod(string $num1, string $num2, ?int $scale = null): string
bcmod(string $num1, string $num2, ?int $scale = null): string
bcmul(string $num1, string $num2, ?int $scale = null): string
bcpow(string $num, string $exponent, ?int $scale = null): string
bcpowmod(string $num, string $exponent, string $modulus, ?int $scale = null): string
bcscale(int $scale): int
bcscale(null $scale = null): int
bcsqrt(string $num, ?int $scale = null): string
bcsub(string $num1, string $num2, ?int $scale = null): string
}

{ // GNU Multiple Precision
GMP_ROUND_ZERO
GMP_ROUND_PLUSINF
GMP_ROUND_MINUSINF
GMP_MSW_FIRST
GMP_LSW_FIRST
GMP_LITTLE_ENDIAN
GMP_BIG_ENDIAN
GMP_NATIVE_ENDIAN
GMP_VERSION

gmp_abs(GMP|int|string $num): GMP
gmp_add(GMP|int|string $num1, GMP|int|string $num2): GMP
gmp_and(GMP|int|string $num1, GMP|int|string $num2): GMP
gmp_binomial(GMP|int|string $n, int $k): GMP
gmp_clrbit(GMP $num, int $index): void
gmp_cmp(GMP|int|string $num1, GMP|int|string $num2): int
gmp_com(GMP|int|string $num): GMP
gmp_div_q(GMP|int|string $num1, GMP|int|string $num2, int $rounding_mode = GMP_ROUND_ZERO): GMP
gmp_div_qr(GMP|int|string $num1, GMP|int|string $num2, int $rounding_mode = GMP_ROUND_ZERO): array
gmp_div_r(GMP|int|string $num1, GMP|int|string $num2, int $rounding_mode = GMP_ROUND_ZERO): GMP
gmp_div()
gmp_divexact(GMP|int|string $num1, GMP|int|string $num2): GMP
gmp_export(GMP|int|string $num, int $word_size = 1, int $flags = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN): string
gmp_fact(GMP|int|string $num): GMP
gmp_gcd(GMP|int|string $num1, GMP|int|string $num2): GMP
gmp_gcdext(GMP|int|string $num1, GMP|int|string $num2): array
gmp_hamdist(GMP|int|string $num1, GMP|int|string $num2): int
gmp_import(string $data, int $word_size = 1, int $flags = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN): GMP
gmp_init(int|string $num, int $base = 0): GMP
gmp_intval(GMP|int|string $num): int
gmp_invert(GMP|int|string $num1, GMP|int|string $num2): GMP|false
gmp_jacobi(GMP|int|string $num1, GMP|int|string $num2): int
gmp_kronecker(GMP|int|string $num1, GMP|int|string $num2): int
gmp_lcm(GMP|int|string $num1, GMP|int|string $num2): GMP
gmp_legendre(GMP|int|string $num1, GMP|int|string $num2): int
gmp_mod(GMP|int|string $num1, GMP|int|string $num2): GMP
gmp_mul(GMP|int|string $num1, GMP|int|string $num2): GMP
gmp_neg(GMP|int|string $num): GMP
gmp_nextprime(GMP|int|string $num): GMP
gmp_or(GMP|int|string $num1, GMP|int|string $num2): GMP
gmp_perfect_power(GMP|int|string $num): bool
gmp_perfect_square(GMP|int|string $num): bool
gmp_popcount(GMP|int|string $num): int
gmp_pow(GMP|int|string $num, int $exponent): GMP
gmp_powm(GMP|int|string $num, GMP|int|string $exponent, GMP|int|string $modulus): GMP
gmp_prob_prime(GMP|int|string $num, int $repetitions = 10): int
gmp_random_bits(int $bits): GMP
gmp_random_range(GMP|int|string $min, GMP|int|string $max): GMP
gmp_random_seed(GMP|int|string $seed): void
gmp_root(GMP|int|string $num, int $nth): GMP
gmp_rootrem(GMP|int|string $num, int $nth): array
gmp_scan0(GMP|int|string $num1, int $start): int
gmp_scan1(GMP|int|string $num1, int $start): int
gmp_setbit(GMP $num, int $index, bool $value = true): void
gmp_sign(GMP|int|string $num): int
gmp_sqrt(GMP|int|string $num): GMP
gmp_sqrtrem(GMP|int|string $num): array
gmp_strval(GMP|int|string $num, int $base = 10): string
gmp_sub(GMP|int|string $num1, GMP|int|string $num2): GMP
gmp_testbit(GMP|int|string $num, int $index): bool
gmp_xor(GMP|int|string $num1, GMP|int|string $num2): GMP

class GMP {
	public __serialize(): array
	public __unserialize(array $data): void
}
}

{ // Mathematical Functions
M_PI
M_E
M_LOG2E
M_LOG10E
M_LN2
M_LN10
M_PI_2
M_PI_4
M_1_PI
M_2_PI
M_SQRTPI
M_2_SQRTPI
M_SQRT2
M_SQRT3
M_SQRT1_2
M_LNPI
M_EULER
PHP_ROUND_HALF_UP
PHP_ROUND_HALF_DOWN
PHP_ROUND_HALF_EVEN
PHP_ROUND_HALF_ODD
NAN
INF

abs(int|float $num): int|float
acos(float $num): float
acosh(float $num): float
asin(float $num): float
asinh(float $num): float
atan2(float $y, float $x): float
atan(float $num): float
atanh(float $num): float
base_convert(string $num, int $from_base, int $to_base): string
bindec(string $binary_string): int|float
ceil(int|float $num): float
cos(float $num): float
cosh(float $num): float
decbin(int $num): string
dechex(int $num): string
decoct(int $num): string
deg2rad(float $num): float
exp(float $num): float
expm1(float $num): float
fdiv(float $num1, float $num2): float
floor(int|float $num): float
fmod(float $num1, float $num2): float
getrandmax(): int
hexdec(string $hex_string): int|float
hypot(float $x, float $y): float
intdiv(int $num1, int $num2): int
is_finite(float $num): bool
is_infinite(float $num): bool
is_nan(float $num): bool
lcg_value(): float
log10(float $num): float
log1p(float $num): float
log(float $num, float $base = M_E): float
max(mixed $value, mixed ...$values): mixed
max(array $value_array): mixed
min(mixed $value, mixed ...$values): mixed
min(array $value_array): mixed
mt_getrandmax(): int
mt_rand(): int
mt_rand(int $min, int $max): int
mt_srand(int $seed = 0, int $mode = MT_RAND_MT19937): void
octdec(string $octal_string): int|float
pi(): float
pow(mixed $num, mixed $exponent): int|float|object
rad2deg(float $num): float
rand(): int
rand(int $min, int $max): int
round(int|float $num, int $precision = 0, int $mode = PHP_ROUND_HALF_UP): float
sin(float $num): float
sinh(float $num): float
sqrt(float $num): float
srand(int $seed = 0, int $mode = MT_RAND_MT19937): void
tan(float $num): float
tanh(float $num): float
}
}

{ // Process Control Extensions
{ // System program execution
escapeshellarg(string $arg): string
escapeshellcmd(string $command): string
exec(string $command, array &$output = null, int &$result_code = null): string|false
passthru(string $command, int &$result_code = null): ?bool
proc_close(resource $process): int
proc_get_status(resource $process): array
proc_nice(int $priority): bool
proc_open(array|string $command, array $descriptor_spec, array &$pipes, ?string $cwd = null, ?array $env_vars = null, ?array $options = null): resource|false
proc_terminate(resource $process, int $signal = 15): bool
shell_exec(string $command): string|false|null
system(string $command, int &$result_code = null): string|false
}
}

{ // Other Basic Extensions
{ // JavaScript Object Notation
JSON_ERROR_NONE
JSON_ERROR_DEPTH
JSON_ERROR_STATE_MISMATCH
JSON_ERROR_CTRL_CHAR
JSON_ERROR_SYNTAX
JSON_ERROR_UTF8
JSON_ERROR_RECURSION
JSON_ERROR_INF_OR_NAN
JSON_ERROR_UNSUPPORTED_TYPE
JSON_ERROR_INVALID_PROPERTY_NAME
JSON_ERROR_UTF16
JSON_BIGINT_AS_STRING
JSON_OBJECT_AS_ARRAY

JSON_HEX_TAG
JSON_HEX_AMP
JSON_HEX_APOS
JSON_HEX_QUOT
JSON_FORCE_OBJECT
JSON_NUMERIC_CHECK
JSON_PRETTY_PRINT
JSON_UNESCAPED_SLASHES
JSON_UNESCAPED_UNICODE
JSON_PARTIAL_OUTPUT_ON_ERROR
JSON_PRESERVE_ZERO_FRACTION
JSON_UNESCAPED_LINE_TERMINATORS

JSON_INVALID_UTF8_IGNORE
JSON_INVALID_UTF8_SUBSTITUTE
JSON_THROW_ON_ERROR

class JsonException extends Exception {}
interface JsonSerializable {
	public jsonSerialize(): mixed
}

json_decode(string $json, ?bool $associative = null, int $depth = 512, int $flags = 0): mixed
json_encode(mixed $value, int $flags = 0, int $depth = 512): string|false
json_last_error_msg(): string
json_last_error(): int
json_validate(string $json, int $depth = 512, int $flags = 0): bool
}

{ // Miscellaneous Functions
CONNECTION_ABORTED
CONNECTION_NORMAL
CONNECTION_TIMEOUT
__COMPILER_HALT_OFFSET__

connection_aborted(): int
connection_status(): int
constant(string $name): mixed
define(string $constant_name, mixed $value, bool $case_insensitive = false): bool
defined(string $constant_name): bool
die()
eval(string $code): mixed
exit(string $status = ?): void
exit(int $status): void
get_browser(?string $user_agent = null, bool $return_array = false): object|array|false
__halt_compiler(): void
highlight_file(string $filename, bool $return = false): string|bool
highlight_string(string $string, bool $return = false): string|bool
hrtime(bool $as_number = false): array|int|float|false
ignore_user_abort(?bool $enable = null): int
pack(string $format, mixed ...$values): string
php_strip_whitespace(string $filename): string
show_source()
sleep(int $seconds): int
sys_getloadavg(): array|false
time_nanosleep(int $seconds, int $nanoseconds): array|bool
time_sleep_until(float $timestamp): bool
uniqid(string $prefix = "", bool $more_entropy = false): string
unpack(string $format, string $string, int $offset = 0): array|false
usleep(int $microseconds): void
}

{ // Standard PHP Library (SPL)
class SplDoublyLinkedList implements Iterator, Countable, ArrayAccess, Serializable {
	const int IT_MODE_LIFO = 2;
	const int IT_MODE_FIFO = 0;
	const int IT_MODE_DELETE = 1;
	const int IT_MODE_KEEP = 0;

	public add(int $index, mixed $value): void
	public bottom(): mixed
	public count(): int
	public current(): mixed
	public getIteratorMode(): int
	public isEmpty(): bool
	public key(): int
	public next(): void
	public offsetExists(int $index): bool
	public offsetGet(int $index): mixed
	public offsetSet(?int $index, mixed $value): void
	public offsetUnset(int $index): void
	public pop(): mixed
	public prev(): void
	public push(mixed $value): void
	public rewind(): void
	public serialize(): string
	public setIteratorMode(int $mode): int
	public shift(): mixed
	public top(): mixed
	public unserialize(string $data): void
	public unshift(mixed $value): void
	public valid(): bool
}
class SplStack extends SplDoublyLinkedList {
	public setIteratorMode(int $mode): void
}
class SplQueue extends SplDoublyLinkedList {
	public dequeue(): mixed
	public enqueue(mixed $value): void
	public setIteratorMode(int $mode): void
}
abstract class SplHeap implements Iterator, Countable {
	protected compare(mixed $value1, mixed $value2): int
	public count(): int
	public current(): mixed
	public extract(): mixed
	public insert(mixed $value): bool
	public isCorrupted(): bool
	public isEmpty(): bool
	public key(): int
	public next(): void
	public recoverFromCorruption(): bool
	public rewind(): void
	public top(): mixed
	public valid(): bool
}
class SplMaxHeap extends SplHeap {
	protected compare(mixed $value1, mixed $value2): int
}
class SplMinHeap extends SplHeap {
	protected compare(mixed $value1, mixed $value2): int
}
class SplPriorityQueue implements Iterator, Countable {
	public compare(mixed $priority1, mixed $priority2): int
	public count(): int
	public current(): mixed
	public extract(): mixed
	public getExtractFlags(): int
	public insert(mixed $value, mixed $priority): bool
	public isCorrupted(): bool
	public isEmpty(): bool
	public key(): int
	public next(): void
	public recoverFromCorruption(): bool
	public rewind(): void
	public setExtractFlags(int $flags): int
	public top(): mixed
	public valid(): bool
}
class SplFixedArray implements IteratorAggregate, ArrayAccess, Countable, JsonSerializable {
	public __construct(int $size = 0)
	public count(): int
	public current(): mixed
	public static fromArray(array $array, bool $preserveKeys = true): SplFixedArray
	public getSize(): int
	public key(): int
	public next(): void
	public offsetExists(int $index): bool
	public offsetGet(int $index): mixed
	public offsetSet(int $index, mixed $value): void
	public offsetUnset(int $index): void
	public rewind(): void
	public setSize(int $size): bool
	public toArray(): array
	public valid(): bool
	public __wakeup(): void
}
class SplObjectStorage implements Countable, Iterator, Serializable, ArrayAccess {
	public addAll(SplObjectStorage $storage): int
	public attach(object $object, mixed $info = null): void
	public contains(object $object): bool
	public count(int $mode = COUNT_NORMAL): int
	public current(): object
	public detach(object $object): void
	public getHash(object $object): string
	public getInfo(): mixed
	public key(): int
	public next(): void
	public offsetExists(object $object): bool
	public offsetGet(object $object): mixed
	public offsetSet(object $object, mixed $info = null): void
	public offsetUnset(object $object): void
	public removeAll(SplObjectStorage $storage): int
	public removeAllExcept(SplObjectStorage $storage): int
	public rewind(): void
	public serialize(): string
	public setInfo(mixed $info): void
	public unserialize(string $data): void
	public valid(): bool
}
// Iterators
class AppendIterator extends IteratorIterator {
	public append(Iterator $iterator): void
	public current(): mixed
	public getArrayIterator(): ArrayIterator
	public getInnerIterator(): Iterator
	public getIteratorIndex(): ?int
	public key(): scalar
	public next(): void
	public rewind(): void
	public valid(): bool
}
class ArrayIterator implements SeekableIterator, ArrayAccess, Serializable, Countable {
	const int STD_PROP_LIST = 1;
	const int ARRAY_AS_PROPS = 2;

	public __construct(array|object $array = [], int $flags = 0)
	public append(mixed $value): void
	public asort(int $flags = SORT_REGULAR): bool
	public count(): int
	public current(): mixed
	public getArrayCopy(): array
	public getFlags(): int
	public key(): string|int|null
	public ksort(int $flags = SORT_REGULAR): bool
	public natcasesort(): bool
	public natsort(): bool
	public next(): void
	public offsetExists(mixed $key): bool
	public offsetGet(mixed $key): mixed
	public offsetSet(mixed $key, mixed $value): void
	public offsetUnset(mixed $key): void
	public rewind(): void
	public seek(int $offset): void
	public serialize(): string
	public setFlags(int $flags): void
	public uasort(callable $callback): bool
	public uksort(callable $callback): bool
	public unserialize(string $data): void
	public valid(): bool
}
class CachingIterator extends IteratorIterator implements ArrayAccess, Countable, Stringable {
	const int CALL_TOSTRING = 1;
	const int CATCH_GET_CHILD = 16;
	const int TOSTRING_USE_KEY = 2;
	const int TOSTRING_USE_CURRENT = 4;
	const int TOSTRING_USE_INNER = 8;
	const int FULL_CACHE = 256;

	public __construct(Iterator $iterator, int $flags = CachingIterator::CALL_TOSTRING)
	public count(): int
	public current(): mixed
	public getCache(): array
	public getFlags(): int
	public getInnerIterator(): Iterator
	public hasNext(): bool
	public key(): scalar
	public next(): void
	public offsetExists(string $key): bool
	public offsetGet(string $key): mixed
	public offsetSet(string $key, mixed $value): void
	public offsetUnset(string $key): void
	public rewind(): void
	public setFlags(int $flags): void
	public __toString(): string
	public valid(): bool
}
class CallbackFilterIterator extends FilterIterator {
	public __construct(Iterator $iterator, callable $callback)
	public accept(): bool
}
class DirectoryIterator extends SplFileInfo implements SeekableIterator {
	public __construct(string $directory)
	public current(): DirectoryIterator
	public getATime(): int
	public getBasename(string $suffix = ""): string
	public getCTime(): int
	public getExtension(): string
	public getFilename(): string
	public getGroup(): int
	public getInode(): int
	public getMTime(): int
	public getOwner(): int
	public getPath(): string
	public getPathname(): string
	public getPerms(): int
	public getSize(): int
	public getType(): string
	public isDir(): bool
	public isDot(): bool
	public isExecutable(): bool
	public isFile(): bool
	public isLink(): bool
	public isReadable(): bool
	public isWritable(): bool
	public key(): int|false
	public next(): void
	public rewind(): void
	public seek(int $offset): void
	public __toString(): string
	public valid(): bool
}
class EmptyIterator implements Iterator {
	public current(): never
	public key(): never
	public next(): void
	public rewind(): void
	public valid(): bool
}
class FilesystemIterator extends DirectoryIterator {
	const int CURRENT_AS_PATHNAME;
	const int CURRENT_AS_FILEINFO;
	const int CURRENT_AS_SELF;
	const int CURRENT_MODE_MASK;
	const int KEY_AS_PATHNAME;
	const int KEY_AS_FILENAME;
	const int FOLLOW_SYMLINKS;
	const int KEY_MODE_MASK;
	const int NEW_CURRENT_AND_KEY;
	const int SKIP_DOTS;
	const int UNIX_PATHS;

	public __construct(string $directory, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO | FilesystemIterator::SKIP_DOTS)
	public current(): string|SplFileInfo|FilesystemIterator
	public getFlags(): int
	public key(): string
	public next(): void
	public rewind(): void
	public setFlags(int $flags): void
}
abstract class FilterIterator extends IteratorIterator {
	public __construct(Iterator $iterator)
	public accept(): bool
	public current(): mixed
	public getInnerIterator(): Iterator
	public key(): mixed
	public next(): void
	public rewind(): void
	public valid(): bool
}
class GlobIterator extends FilesystemIterator implements Countable {
	public __construct(string $pattern, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO)
	public count(): int
}
class InfiniteIterator extends IteratorIterator {
	public __construct(Iterator $iterator)
	public next(): void
}
class IteratorIterator implements OuterIterator {
	public __construct(Traversable $iterator, ?string $class = null)
	public current(): mixed
	public getInnerIterator(): ?Iterator
	public key(): mixed
	public next(): void
	public rewind(): void
	public valid(): bool
}
class LimitIterator extends IteratorIterator {
	public __construct(Iterator $iterator, int $offset = 0, int $limit = -1)
	public current(): mixed
	public getInnerIterator(): Iterator
	public getPosition(): int
	public key(): mixed
	public next(): void
	public rewind(): void
	public seek(int $offset): int
	public valid(): bool
}
class MultipleIterator implements Iterator {
	const int MIT_NEED_ANY = 0;
	const int MIT_NEED_ALL = 1;
	const int MIT_KEYS_NUMERIC = 0;
	const int MIT_KEYS_ASSOC = 2;

	public __construct(int $flags = MultipleIterator::MIT_NEED_ALL | MultipleIterator::MIT_KEYS_NUMERIC)
	public attachIterator(Iterator $iterator, string|int|null $info = null): void
	public containsIterator(Iterator $iterator): bool
	public countIterators(): int
	public current(): array
	public detachIterator(Iterator $iterator): void
	public getFlags(): int
	public key(): array
	public next(): void
	public rewind(): void
	public setFlags(int $flags): void
	public valid(): bool
}
class NoRewindIterator extends IteratorIterator {
	public __construct(Iterator $iterator)
	public current(): mixed
	public getInnerIterator(): iterator
	public key(): mixed
	public next(): void
	public rewind(): void
	public valid(): bool
}
class ParentIterator extends RecursiveFilterIterator {
	public __construct(RecursiveIterator $iterator)
	public accept(): bool
	public getChildren(): ParentIterator
	public hasChildren(): bool
	public next(): void
	public rewind(): void
}
class RecursiveArrayIterator extends ArrayIterator implements RecursiveIterator {
	const int CHILD_ARRAYS_ONLY = 4;

	public getChildren(): ?RecursiveArrayIterator
	public hasChildren(): bool
}
class RecursiveCachingIterator extends CachingIterator implements RecursiveIterator {
	public __construct(Iterator $iterator, int $flags = RecursiveCachingIterator::CALL_TOSTRING)
	public getChildren(): ?RecursiveCachingIterator
	public hasChildren(): bool
}
class RecursiveCallbackFilterIterator extends CallbackFilterIterator implements RecursiveIterator {
	public __construct(RecursiveIterator $iterator, callable $callback)
	public getChildren(): RecursiveCallbackFilterIterator
	public hasChildren(): bool
}
class RecursiveDirectoryIterator extends FilesystemIterator implements RecursiveIterator {
	public __construct(string $directory, int $flags = FilesystemIterator::KEY_AS_PATHNAME | FilesystemIterator::CURRENT_AS_FILEINFO)
	public getChildren(): RecursiveDirectoryIterator
	public getSubPath(): string
	public getSubPathname(): string
	public hasChildren(bool $allowLinks = false): bool
	public key(): string
	public next(): void
	public rewind(): void
}
abstract class RecursiveFilterIterator extends FilterIterator implements RecursiveIterator {
	public __construct(RecursiveIterator $iterator)
	public getChildren(): ?RecursiveFilterIterator
	public hasChildren(): bool
}
class RecursiveIteratorIterator implements OuterIterator {
	const int LEAVES_ONLY = 0;
	const int SELF_FIRST = 1;
	const int CHILD_FIRST = 2;
	const int CATCH_GET_CHILD = 16;

	public __construct(Traversable $iterator, int $mode = RecursiveIteratorIterator::LEAVES_ONLY, int $flags = 0)
	public beginChildren(): void
	public beginIteration(): void
	public callGetChildren(): ?RecursiveIterator
	public callHasChildren(): bool
	public current(): mixed
	public endChildren(): void
	public endIteration(): void
	public getDepth(): int
	public getInnerIterator(): RecursiveIterator
	public getMaxDepth(): int|false
	public getSubIterator(?int $level = null): ?RecursiveIterator
	public key(): mixed
	public next(): void
	public nextElement(): void
	public rewind(): void
	public setMaxDepth(int $maxDepth = -1): void
	public valid(): bool
}
class RecursiveRegexIterator extends RegexIterator implements RecursiveIterator {
	public __construct(RecursiveIterator $iterator, string $pattern, int $mode = RecursiveRegexIterator::MATCH, int $flags = 0, int $pregFlags = 0)
	public getChildren(): RecursiveRegexIterator
	public hasChildren(): bool
}
class RecursiveTreeIterator extends RecursiveIteratorIterator {
	const int BYPASS_CURRENT = 4;
	const int BYPASS_KEY = 8;
	const int PREFIX_LEFT = 0;
	const int PREFIX_MID_HAS_NEXT = 1;
	const int PREFIX_MID_LAST = 2;
	const int PREFIX_END_HAS_NEXT = 3;
	const int PREFIX_END_LAST = 4;
	const int PREFIX_RIGHT = 5;

	public __construct(RecursiveIterator|IteratorAggregate $iterator, int $flags = RecursiveTreeIterator::BYPASS_KEY, int $cachingIteratorFlags = CachingIterator::CATCH_GET_CHILD, int $mode = RecursiveTreeIterator::SELF_FIRST)
	public beginChildren(): void
	public beginIteration(): RecursiveIterator
	public callGetChildren(): RecursiveIterator
	public callHasChildren(): bool
	public current(): mixed
	public endChildren(): void
	public endIteration(): void
	public getEntry(): string
	public getPostfix(): string
	public getPrefix(): string
	public key(): mixed
	public next(): void
	public nextElement(): void
	public rewind(): void
	public setPostfix(string $postfix): void
	public setPrefixPart(int $part, string $value): void
	public valid(): bool
}
class RegexIterator extends FilterIterator {
	const int MATCH = 0;
	const int GET_MATCH = 1;
	const int ALL_MATCHES = 2;
	const int SPLIT = 3;
	const int REPLACE = 4;
	const int USE_KEY = 1;
	public ?string $replacement = null;

	public __construct(Iterator $iterator, string $pattern, int $mode = RegexIterator::MATCH, int $flags = 0, int $pregFlags = 0)
	public accept(): bool
	public getFlags(): int
	public getMode(): int
	public getPregFlags(): int
	public getRegex(): string
	public setFlags(int $flags): void
	public setMode(int $mode): void
	public setPregFlags(int $pregFlags): void
}
// Interfaces
interface Countable {
	public count(): int
}
interface OuterIterator extends Iterator {
	public getInnerIterator(): ?Iterator
}
interface RecursiveIterator extends Iterator {
	public getChildren(): ?RecursiveIterator
	public hasChildren(): bool
}
interface SeekableIterator extends Iterator {
	public seek(int $offset): void
}
// Exceptions
class LogicException extends Exception {}
	class BadFunctionCallException extends LogicException {}
		class BadMethodCallException extends BadFunctionCallException {}
	class DomainException extends LogicException {}
	class InvalidArgumentException extends LogicException {}
	class LengthException extends LogicException {}
	class OutOfRangeException extends LogicException {}
class RuntimeException extends Exception {}
	class OutOfBoundsException extends RuntimeException {}
	class OverflowException extends RuntimeException {}
	class RangeException extends RuntimeException {}
	class UnderflowException extends RuntimeException {}
	class UnexpectedValueException extends RuntimeException {}
// SPL Functions
class_implements(object|string $object_or_class, bool $autoload = true): array|false
class_parents(object|string $object_or_class, bool $autoload = true): array|false
class_uses(object|string $object_or_class, bool $autoload = true): array|false
iterator_apply(Traversable $iterator, callable $callback, ?array $args = null): int
iterator_count(Traversable $iterator): int
iterator_to_array(Traversable $iterator, bool $preserve_keys = true): array
spl_autoload_call(string $class): void
spl_autoload_extensions(?string $file_extensions = null): string
spl_autoload_functions(): array
spl_autoload_register(?callable $callback = null, bool $throw = true, bool $prepend = false): bool
spl_autoload_unregister(callable $callback): bool
spl_autoload(string $class, ?string $file_extensions = null): void
spl_classes(): array
spl_object_hash(object $object): string
spl_object_id(object $object): int
// File Handling
class SplFileInfo implements Stringable {
	public __construct(string $filename)
	public getATime(): int|false
	public getBasename(string $suffix = ""): string
	public getCTime(): int|false
	public getExtension(): string
	public getFileInfo(?string $class = null): SplFileInfo
	public getFilename(): string
	public getGroup(): int|false
	public getInode(): int|false
	public getLinkTarget(): string|false
	public getMTime(): int|false
	public getOwner(): int|false
	public getPath(): string
	public getPathInfo(?string $class = null): ?SplFileInfo
	public getPathname(): string
	public getPerms(): int|false
	public getRealPath(): string|false
	public getSize(): int|false
	public getType(): string|false
	public isDir(): bool
	public isExecutable(): bool
	public isFile(): bool
	public isLink(): bool
	public isReadable(): bool
	public isWritable(): bool
	public openFile(string $mode = "r", bool $useIncludePath = false, ?resource $context = null): SplFileObject
	public setFileClass(string $class = SplFileObject::class): void
	public setInfoClass(string $class = SplFileInfo::class): void
	public __toString(): string
}
class SplFileObject extends SplFileInfo implements RecursiveIterator, SeekableIterator {
	const int DROP_NEW_LINE = 1;
	const int READ_AHEAD = 2;
	const int SKIP_EMPTY = 4;
	const int READ_CSV = 8;

	public __construct(string $filename, string $mode = "r", bool $useIncludePath = false, ?resource $context = null)
	public current(): string|array|false
	public eof(): bool
	public fflush(): bool
	public fgetc(): string|false
	public fgetcsv(string $separator = ",", string $enclosure = "\"", string $escape = "\\"): array|false
	public fgets(): string
	public flock(int $operation, int &$wouldBlock = null): bool
	public fpassthru(): int
	public fputcsv(array $fields, string $separator = ",", string $enclosure = "\"", string $escape = "\\", string $eol = "\n"): int|false
	public fread(int $length): string|false
	public fscanf(string $format, mixed &...$vars): array|int|null
	public fseek(int $offset, int $whence = SEEK_SET): int
	public fstat(): array
	public ftell(): int|false
	public ftruncate(int $size): bool
	public fwrite(string $data, int $length = 0): int|false
	public getChildren(): ?RecursiveIterator
	public getCsvControl(): array
	public getFlags(): int
	public getMaxLineLen(): int
	public hasChildren(): bool
	public key(): int
	public next(): void
	public rewind(): void
	public seek(int $line): void
	public setCsvControl(string $separator = ",", string $enclosure = "\"", string $escape = "\\"): void
	public setFlags(int $flags): void
	public setMaxLineLen(int $maxLength): void
	public valid(): bool
}
class SplTempFileObject extends SplFileObject {
	public __construct(int $maxMemory = 2 * 1024 * 1024)
}
// Miscellaneous
class ArrayObject implements IteratorAggregate, ArrayAccess, Serializable, Countable {
	const int STD_PROP_LIST = 1;
	const int ARRAY_AS_PROPS = 2;

	public __construct(array|object $array = [], int $flags = 0, string $iteratorClass = ArrayIterator::class)
	public append(mixed $value): void
	public asort(int $flags = SORT_REGULAR): bool
	public count(): int
	public exchangeArray(array|object $array): array
	public getArrayCopy(): array
	public getFlags(): int
	public getIterator(): Iterator
	public getIteratorClass(): string
	public ksort(int $flags = SORT_REGULAR): bool
	public natcasesort(): bool
	public natsort(): bool
	public offsetExists(mixed $key): bool
	public offsetGet(mixed $key): mixed
	public offsetSet(mixed $key, mixed $value): void
	public offsetUnset(mixed $key): void
	public serialize(): string
	public setFlags(int $flags): void
	public setIteratorClass(string $iteratorClass): void
	public uasort(callable $callback): bool
	public uksort(callable $callback): bool
	public unserialize(string $data): void
}
interface SplObserver {
	public update(SplSubject $subject): void
}
interface SplSubject {
	public attach(SplObserver $observer): void
	public detach(SplObserver $observer): void
	public notify(): void
}
}

{ // Streams
STREAM_FILTER_READ
STREAM_FILTER_WRITE
STREAM_FILTER_ALL
PSFS_PASS_ON
PSFS_FEED_ME
PSFS_ERR_FATAL
PSFS_FLAG_NORMAL
PSFS_FLAG_FLUSH_INC
PSFS_FLAG_FLUSH_CLOSE
STREAM_USE_PATH
STREAM_CLIENT_ASYNC_CONNECT
STREAM_CLIENT_CONNECT
STREAM_CLIENT_PERSISTENT
STREAM_SERVER_BIND
STREAM_SERVER_LISTEN
STREAM_NOTIFY_RESOLVE
STREAM_NOTIFY_CONNECT
STREAM_NOTIFY_AUTH_REQUIRED
STREAM_NOTIFY_MIME_TYPE_IS
STREAM_NOTIFY_FILE_SIZE_IS
STREAM_NOTIFY_REDIRECTED
STREAM_NOTIFY_PROGRESS
STREAM_NOTIFY_COMPLETED
STREAM_NOTIFY_FAILURE
STREAM_NOTIFY_AUTH_RESULT
STREAM_NOTIFY_SEVERITY_INFO
STREAM_NOTIFY_SEVERITY_WARN
STREAM_NOTIFY_SEVERITY_ERR
STREAM_IPPROTO_ICMP
STREAM_IPPROTO_IP
STREAM_IPPROTO_RAW
STREAM_IPPROTO_TCP
STREAM_IPPROTO_UDP
STREAM_PF_INET
STREAM_PF_INET6
STREAM_PF_UNIX
STREAM_SOCK_DGRAM
STREAM_SOCK_RAW
STREAM_SOCK_RDM
STREAM_SOCK_SEQPACKET
STREAM_SOCK_STREAM
STREAM_SHUT_RD
STREAM_SHUT_WR
STREAM_SHUT_RDWR
STREAM_CAST_FOR_SELECT
STREAM_CAST_AS_STREAM
STREAM_META_TOUCH
STREAM_META_OWNER
STREAM_META_OWNER_NAME
STREAM_META_GROUP
STREAM_META_GROUP_NAME
STREAM_META_ACCESS
STREAM_BUFFER_NONE
STREAM_BUFFER_LINE
STREAM_BUFFER_FULL

class php_user_filter {
	public string $filtername = "";
	public mixed $params = "";
	public ?resource $stream = null;

	public filter(resource $in, resource $out, int &$consumed, bool $closing): int
	public onClose(): void
	public onCreate(): bool
}

class /*streamWrapper*/ {
	public resource $context;
	public dir_closedir(): bool
	public dir_opendir(string $path, int $options): bool
	public dir_readdir(): string
	public dir_rewinddir(): bool
	public mkdir(string $path, int $mode, int $options): bool
	public rename(string $path_from, string $path_to): bool
	public rmdir(string $path, int $options): bool
	public stream_cast(int $cast_as): resource
	public stream_close(): void
	public stream_eof(): bool
	public stream_flush(): bool
	public stream_lock(int $operation): bool
	public stream_metadata(string $path, int $option, mixed $value): bool
	public stream_open(string $path, string $mode, int $options, ?string &$opened_path): bool
	public stream_read(int $count): string|false
	public stream_seek(int $offset, int $whence  = SEEK_SET): bool
	public stream_set_option(int $option, int $arg1, int $arg2): bool
	public stream_stat(): array|false
	public stream_tell(): int
	public stream_truncate(int $new_size): bool
	public stream_write(string $data): int
	public unlink(string $path): bool
	public url_stat(string $path, int $flags): array|false
}

stream_bucket_append(resource $brigade, object $bucket): void
stream_bucket_make_writeable(resource $brigade): ?object
stream_bucket_new(resource $stream, string $buffer): object
stream_bucket_prepend(resource $brigade, object $bucket): void
stream_context_create(?array $options = null, ?array $params = null): resource
stream_context_get_default(?array $options = null): resource
stream_context_get_options(resource $stream_or_context): array
stream_context_get_params(resource $context): array
stream_context_set_default(array $options): resource
stream_context_set_option(resource $stream_or_context, string $wrapper, string $option, mixed $value): bool
stream_context_set_option(resource $stream_or_context, array $options): bool
stream_context_set_params(resource $context, array $params): bool
stream_copy_to_stream(resource $from, resource $to, ?int $length = null, int $offset = 0): int|false
stream_filter_append(resource $stream, string $filtername, int $read_write = ?, mixed $params = ?): resource
stream_filter_prepend(resource $stream, string $filtername, int $read_write = ?, mixed $params = ?): resource
stream_filter_register(string $filter_name, string $class): bool
stream_filter_remove(resource $stream_filter): bool
stream_get_contents(resource $stream, ?int $length = null, int $offset = -1): string|false
stream_get_filters(): array
stream_get_line(resource $stream, int $length, string $ending = ""): string|false
stream_get_meta_data(resource $stream): array
stream_get_transports(): array
stream_get_wrappers(): array
stream_is_local(resource|string $stream): bool
stream_isatty(resource $stream): bool
stream_notification_callback(int $notification_code, int $severity, string $message, int $message_code, int $bytes_transferred, int $bytes_max): void
stream_register_wrapper()
stream_resolve_include_path(string $filename): string|false
stream_select(?array &$read, ?array &$write, ?array &$except, ?int $seconds, ?int $microseconds = null): int|false
stream_set_blocking(resource $stream, bool $enable): bool
stream_set_chunk_size(resource $stream, int $size): int
stream_set_read_buffer(resource $stream, int $size): int
stream_set_timeout(resource $stream, int $seconds, int $microseconds = 0): bool
stream_set_write_buffer(resource $stream, int $size): int
stream_socket_accept(resource $socket, ?float $timeout = null, string &$peer_name = null): resource|false
stream_socket_client(string $address, int &$error_code = null, string &$error_message = null, ?float $timeout = null, int $flags = STREAM_CLIENT_CONNECT, ?resource $context = null): resource|false
stream_socket_enable_crypto(resource $stream, bool $enable, ?int $crypto_method = null, ?resource $session_stream = null): int|bool
stream_socket_get_name(resource $socket, bool $remote): string|false
stream_socket_pair(int $domain, int $type, int $protocol): array|false
stream_socket_recvfrom(resource $socket, int $length, int $flags = 0, ?string &$address = null): string|false
stream_socket_sendto(resource $socket, string $data, int $flags = 0, string $address = ""): int|false
stream_socket_server(string $address, int &$error_code = null, string &$error_message = null, int $flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, ?resource $context = null): resource|false
stream_socket_shutdown(resource $stream, int $mode): bool
stream_supports_lock(resource $stream): bool
stream_wrapper_register(string $protocol, string $class, int $flags = 0): bool
stream_wrapper_restore(string $protocol): bool
stream_wrapper_unregister(string $protocol): bool
}

{ // URLs
PHP_URL_SCHEME
PHP_URL_HOST
PHP_URL_PORT
PHP_URL_USER
PHP_URL_PASS
PHP_URL_PATH
PHP_URL_QUERY
PHP_URL_FRAGMENT
PHP_QUERY_RFC1738
PHP_QUERY_RFC3986

base64_decode(string $string, bool $strict = false): string|false
base64_encode(string $string): string
get_headers(string $url, bool $associative = false, ?resource $context = null): array|false
get_meta_tags(string $filename, bool $use_include_path = false): array|false
http_build_query(array|object $data, string $numeric_prefix = "", ?string $arg_separator = null, int $encoding_type = PHP_QUERY_RFC1738): string
parse_url(string $url, int $component = -1): int|string|array|null|false
rawurldecode(string $string): string
rawurlencode(string $string): string
urldecode(string $string): string
urlencode(string $string): string
}
}

{ // Other Services
{ // Client URL Library
curl_close(CurlHandle $handle): void
curl_copy_handle(CurlHandle $handle): CurlHandle|false
curl_errno(CurlHandle $handle): int
curl_error(CurlHandle $handle): string
curl_escape(CurlHandle $handle, string $string): string|false
curl_exec(CurlHandle $handle): string|bool
curl_file_create()
curl_getinfo(CurlHandle $handle, ?int $option = null): mixed
curl_init(?string $url = null): CurlHandle|false
curl_multi_add_handle(CurlMultiHandle $multi_handle, CurlHandle $handle): int
curl_multi_close(CurlMultiHandle $multi_handle): void
curl_multi_errno(CurlMultiHandle $multi_handle): int
curl_multi_exec(CurlMultiHandle $multi_handle, int &$still_running): int
curl_multi_getcontent(CurlHandle $handle): ?string
curl_multi_info_read(CurlMultiHandle $multi_handle, int &$queued_messages = null): array|false
curl_multi_init(): CurlMultiHandle
curl_multi_remove_handle(CurlMultiHandle $multi_handle, CurlHandle $handle): int
curl_multi_select(CurlMultiHandle $multi_handle, float $timeout = 1.0): int
curl_multi_setopt(CurlMultiHandle $multi_handle, int $option, mixed $value): bool
curl_multi_strerror(int $error_code): ?string
curl_pause(CurlHandle $handle, int $flags): int
curl_reset(CurlHandle $handle): void
curl_setopt_array(CurlHandle $handle, array $options): bool
curl_setopt(CurlHandle $handle, int $option, mixed $value): bool
curl_share_close(CurlShareHandle $share_handle): void
curl_share_errno(CurlShareHandle $share_handle): int
curl_share_init(): CurlShareHandle
curl_share_setopt(CurlShareHandle $share_handle, int $option, mixed $value): bool
curl_share_strerror(int $error_code): ?string
curl_strerror(int $error_code): ?string
curl_unescape(CurlHandle $handle, string $string): string|false
curl_version(): array|false

final class CurlHandle {}
final class CurlMultiHandle {}
final class CurlShareHandle {}
class CURLFile {
	public string $name = "";
	public string $mime = "";
	public string $postname = "";

	public __construct(string $filename, ?string $mime_type = null, ?string $posted_filename = null)
	public getFilename(): string
	public getMimeType(): string
	public getPostFilename(): string
	public setMimeType(string $mime_type): void
	public setPostFilename(string $posted_filename): void
}
class CURLStringFile {
	public string $data;
	public string $postname;
	public string $mime;

	public __construct(string $data, string $postname, string $mime = "application/octet-stream")
}
}

{ // Network
// openlog() Options
LOG_CONS
LOG_NDELAY
LOG_ODELAY
LOG_NOWAIT
LOG_PERROR
LOG_PID
// openlog() Facilities
LOG_AUTH
LOG_AUTHPRIV
LOG_CRON
LOG_DAEMON
LOG_KERN
LOG_LOCAL0
LOG_LOCAL1
LOG_LOCAL2
LOG_LOCAL3
LOG_LOCAL4
LOG_LOCAL5
LOG_LOCAL6
LOG_LOCAL7
LOG_LPR
LOG_MAIL
LOG_NEWS
LOG_SYSLOG
LOG_USER
LOG_UUCP
// syslog() Priorities
LOG_EMERG
LOG_ALERT
LOG_CRIT
LOG_ERR
LOG_WARNING
LOG_NOTICE
LOG_INFO
LOG_DEBUG
// dns_get_record() Options
DNS_A
DNS_CAA
DNS_MX
DNS_CNAME
DNS_NS
DNS_PTR
DNS_HINFO
DNS_SOA
DNS_TXT
DNS_ANY
DNS_AAAA
DNS_ALL

checkdnsrr(string $hostname, string $type = "MX"): bool
closelog(): bool
dns_check_record()
dns_get_mx()
dns_get_record(string $hostname, int $type = DNS_ANY, array &$authoritative_name_servers = null, array &$additional_records = null, bool $raw = false): array|false
fsockopen(string $hostname, int $port = -1, int &$error_code = null, string &$error_message = null, ?float $timeout = null): resource|false
gethostbyaddr(string $ip): string|false
gethostbyname(string $hostname): string
gethostbynamel(string $hostname): array|false
gethostname(): string|false
getmxrr(string $hostname, array &$hosts, array &$weights = null): bool
getprotobyname(string $protocol): int|false
getprotobynumber(int $protocol): string|false
getservbyname(string $service, string $protocol): int|false
getservbyport(int $port, string $protocol): string|false
header_register_callback(callable $callback): bool
header_remove(?string $name = null): void
header(string $header, bool $replace = true, int $response_code = 0): void
headers_list(): array
headers_sent(string &$filename = null, int &$line = null): bool
http_clear_last_response_headers(): void
http_get_last_response_headers(): ?array
http_response_code(int $response_code = 0): int|bool
inet_ntop(string $ip): string|false
inet_pton(string $ip): string|false
ip2long(string $ip): int|false
long2ip(int $ip): string|false
net_get_interfaces(): array|false
openlog(string $prefix, int $flags, int $facility): bool
pfsockopen(string $hostname, int $port = -1, int &$error_code = null, string &$error_message = null, ?float $timeout = null): resource|false
request_parse_body(?array $options = null): array
setcookie(string $name, string $value = "", int $expires_or_options = 0, string $path = "", string $domain = "", bool $secure = false, bool $httponly = false): bool
setrawcookie(string $name, string $value = ?, int $expires_or_options = 0, string $path = ?, string $domain = ?, bool $secure = false, bool $httponly = false): bool
socket_get_status()
socket_set_blocking()
socket_set_timeout()
syslog(int $priority, string $message): bool
}
}

{ // Server Specific Extensions
{ // FastCGI Process Manager
fastcgi_finish_request(): bool
fpm_get_status(): array
}
}

{ // Session Extensions
SID
PHP_SESSION_DISABLED
PHP_SESSION_NONE
PHP_SESSION_ACTIVE

session_abort(): bool
session_cache_expire(?int $value = null): int|false
session_cache_limiter(?string $value = null): string|false
session_commit()
session_create_id(string $prefix = ""): string|false
session_decode(string $data): bool
session_destroy(): bool
session_encode(): string|false
session_gc(): int|false
session_get_cookie_params(): array
session_id(?string $id = null): string|false
session_module_name(?string $module = null): string|false
session_name(?string $name = null): string|false
session_regenerate_id(bool $delete_old_session = false): bool
session_register_shutdown(): void
session_reset(): bool
session_save_path(?string $path = null): string|false
session_set_cookie_params(int $lifetime_or_options, ?string $path = null, ?string $domain = null, ?bool $secure = null, ?bool $httponly = null): bool
session_set_save_handler(callable $open, callable $close, callable $read, callable $write, callable $destroy, callable $gc, callable $create_sid = ?, callable $validate_sid = ?, callable $update_timestamp = ?): bool
session_start(array $options = []): bool
session_status(): int
session_unset(): bool
session_write_close(): bool

class SessionHandler implements SessionHandlerInterface, SessionIdInterface {
	public close(): bool
	public create_sid(): string
	public destroy(string $id): bool
	public gc(int $max_lifetime): int|false
	public open(string $path, string $name): bool
	public read(string $id): string|false
	public write(string $id, string $data): bool
}
interface SessionHandlerInterface {
	public close(): bool
	public destroy(string $id): bool
	public gc(int $max_lifetime): int|false
	public open(string $path, string $name): bool
	public read(string $id): string|false
	public write(string $id, string $data): bool
}
interface SessionIdInterface {
	public create_sid(): string
}
interface SessionUpdateTimestampHandlerInterface {
	public updateTimestamp(string $id, string $data): bool
	public validateId(string $id): bool
}
}

{ // Text Processing
{ // Regular Expressions (Perl-Compatible)
PREG_PATTERN_ORDER
PREG_SET_ORDER
PREG_OFFSET_CAPTURE
PREG_SPLIT_NO_EMPTY
PREG_SPLIT_DELIM_CAPTURE
PREG_SPLIT_OFFSET_CAPTURE
PREG_UNMATCHED_AS_NULL
PREG_NO_ERROR
PREG_INTERNAL_ERROR
PREG_BACKTRACK_LIMIT_ERROR
PREG_RECURSION_LIMIT_ERROR
PREG_BAD_UTF8_ERROR
PREG_BAD_UTF8_OFFSET_ERROR
PREG_JIT_STACKLIMIT_ERROR
PCRE_VERSION

preg_filter(string|array $pattern, string|array $replacement, string|array $subject, int $limit = -1, int &$count = null): string|array|null
preg_grep(string $pattern, array $array, int $flags = 0): array|false
preg_last_error_msg(): string
preg_last_error(): int
preg_match_all(string $pattern, string $subject, array &$matches = null, int $flags = 0, int $offset = 0): int|false
preg_match(string $pattern, string $subject, array &$matches = null, int $flags = 0, int $offset = 0): int|false
preg_quote(string $str, ?string $delimiter = null): string
preg_replace_callback_array(array $pattern, string|array $subject, int $limit = -1, int &$count = null, int $flags = 0): string|array|null
preg_replace_callback(string|array $pattern, callable $callback, string|array $subject, int $limit = -1, int &$count = null, int $flags = 0): string|array|null
preg_replace(string|array $pattern, string|array $replacement, string|array $subject, int $limit = -1, int &$count = null): string|array|null
preg_split(string $pattern, string $subject, int $limit = -1, int $flags = 0): array|false
}

{ // Strings
CRYPT_SALT_LENGTH
CRYPT_STD_DES
CRYPT_EXT_DES
CRYPT_MD5
CRYPT_BLOWFISH
CRYPT_SHA256
CRYPT_SHA512

HTML_SPECIALCHARS
HTML_ENTITIES
ENT_COMPAT
ENT_QUOTES
ENT_NOQUOTES
ENT_IGNORE
ENT_SUBSTITUTE
ENT_DISALLOWED
ENT_HTML401
ENT_XML1
ENT_XHTML
ENT_HTML5

CHAR_MAX
LC_CTYPE
LC_NUMERIC
LC_TIME
LC_COLLATE
LC_MONETARY
LC_ALL
LC_MESSAGES

STR_PAD_LEFT
STR_PAD_RIGHT
STR_PAD_BOTH

addcslashes(string $string, string $characters): string
addslashes(string $string): string
bin2hex(string $string): string
chop()
chr(int $codepoint): string
chunk_split(string $string, int $length = 76, string $separator = "\r\n"): string
convert_uudecode(string $string): string|false
convert_uuencode(string $string): string
count_chars(string $string, int $mode = 0): array|string
crc32(string $string): int
crypt(string $string, string $salt): string
echo(string ...$expressions): void
explode(string $separator, string $string, int $limit = PHP_INT_MAX): array
fprintf(resource $stream, string $format, mixed ...$values): int
get_html_translation_table(int $table = HTML_SPECIALCHARS, int $flags = ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401, string $encoding = "UTF-8"): array
hebrev(string $string, int $max_chars_per_line = 0): string
hex2bin(string $string): string|false
html_entity_decode(string $string, int $flags = ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401, ?string $encoding = null): string
htmlentities(string $string, int $flags = ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401, ?string $encoding = null, bool $double_encode = true): string
htmlspecialchars_decode(string $string, int $flags = ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401): string
htmlspecialchars(string $string, int $flags = ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401, ?string $encoding = null, bool $double_encode = true): string
implode(string $separator, array $array): string
implode(array $array): string
join()
lcfirst(string $string): string
levenshtein(string $string1, string $string2, int $insertion_cost = 1, int $replacement_cost = 1, int $deletion_cost = 1): int
localeconv(): array
ltrim(string $string, string $characters = " \n\r\t\v\x00"): string
md5_file(string $filename, bool $binary = false): string|false
md5(string $string, bool $binary = false): string
metaphone(string $string, int $max_phonemes = 0): string
nl_langinfo(int $item): string|false
nl2br(string $string, bool $use_xhtml = true): string
number_format(float $num, int $decimals = 0, ?string $decimal_separator = ".", ?string $thousands_separator = ","): string
ord(string $character): int
parse_str(string $string, array &$result): void
print(string $expression): int
printf(string $format, mixed ...$values): int
quoted_printable_decode(string $string): string
quoted_printable_encode(string $string): string
quotemeta(string $string): string
rtrim(string $string, string $characters = " \n\r\t\v\x00"): string
setlocale(int $category, string $locales, string ...$rest): string|false
setlocale(int $category, array $locale_array): string|false
sha1_file(string $filename, bool $binary = false): string|false
sha1(string $string, bool $binary = false): string
similar_text(string $string1, string $string2, float &$percent = null): int
soundex(string $string): string
sprintf(string $format, mixed ...$values): string
sscanf(string $string, string $format, mixed &...$vars): array|int|null
str_contains(string $haystack, string $needle): bool
str_ends_with(string $haystack, string $needle): bool
str_getcsv(string $string, string $separator = ",", string $enclosure = "\"", string $escape = "\\"): array
str_ireplace(array|string $search, array|string $replace, string|array $subject, int &$count = null): string|array
str_pad(string $string, int $length, string $pad_string = " ", int $pad_type = STR_PAD_RIGHT): string
str_repeat(string $string, int $times): string
str_replace(array|string $search, array|string $replace, string|array $subject, int &$count = null): string|array
str_rot13(string $string): string
str_shuffle(string $string): string
str_split(string $string, int $length = 1): array
str_starts_with(string $haystack, string $needle): bool
str_word_count(string $string, int $format = 0, ?string $characters = null): array|int
strcasecmp(string $string1, string $string2): int
strchr()
strcmp(string $string1, string $string2): int
strcoll(string $string1, string $string2): int
strcspn(string $string, string $characters, int $offset = 0, ?int $length = null): int
strip_tags(string $string, array|string|null $allowed_tags = null): string
stripcslashes(string $string): string
stripos(string $haystack, string $needle, int $offset = 0): int|false
stripslashes(string $string): string
stristr(string $haystack, string $needle, bool $before_needle = false): string|false
strlen(string $string): int
strnatcasecmp(string $string1, string $string2): int
strnatcmp(string $string1, string $string2): int
strncasecmp(string $string1, string $string2, int $length): int
strncmp(string $string1, string $string2, int $length): int
strpbrk(string $string, string $characters): string|false
strpos(string $haystack, string $needle, int $offset = 0): int|false
strrchr(string $haystack, string $needle): string|false
strrev(string $string): string
strripos(string $haystack, string $needle, int $offset = 0): int|false
strrpos(string $haystack, string $needle, int $offset = 0): int|false
strspn(string $string, string $characters, int $offset = 0, ?int $length = null): int
strstr(string $haystack, string $needle, bool $before_needle = false): string|false
strtok(string $string, string $token): string|false
strtok(string $token): string|false
strtolower(string $string): string
strtoupper(string $string): string
strtr(string $string, string $from, string $to): string
strtr(string $string, array $replace_pairs): string
substr_compare(string $haystack, string $needle, int $offset, ?int $length = null, bool $case_insensitive = false): int
substr_count(string $haystack, string $needle, int $offset = 0, ?int $length = null): int
substr_replace(array|string $string, array|string $replace, array|int $offset, array|int|null $length = null): string|array
substr(string $string, int $offset, ?int $length = null): string
trim(string $string, string $characters = " \n\r\t\v\x00"): string
ucfirst(string $string): string
ucwords(string $string, string $separators = " \t\r\n\f\v"): string
vfprintf(resource $stream, string $format, array $values): int
vprintf(string $format, array $values): int
vsprintf(string $format, array $values): string
wordwrap(string $string, int $width = 75, string $break = "\n", bool $cut_long_words = false): string
}
}

{ // Variable and Type Related Extensions
{ // Arrays
CASE_LOWER
CASE_UPPER

SORT_ASC
SORT_DESC
SORT_REGULAR
SORT_NUMERIC
SORT_STRING
SORT_LOCALE_STRING
SORT_NATURAL
SORT_FLAG_CASE

ARRAY_FILTER_USE_KEY
ARRAY_FILTER_USE_BOTH

COUNT_NORMAL
COUNT_RECURSIVE

EXTR_OVERWRITE
EXTR_SKIP
EXTR_PREFIX_SAME
EXTR_PREFIX_SAME
EXTR_PREFIX_ALL
EXTR_PREFIX_INVALID
EXTR_PREFIX_IF_EXISTS
EXTR_IF_EXISTS
EXTR_REFS

array_all(array $array, callable $callback): bool
array_any(array $array, callable $callback): bool
array_change_key_case(array $array, int $case = CASE_LOWER): array
array_chunk(array $array, int $length, bool $preserve_keys = false): array
array_column(array $array, int|string|null $column_key, int|string|null $index_key = null): array
array_combine(array $keys, array $values): array
array_count_values(array $array): array
array_diff_assoc(array $array, array ...$arrays): array
array_diff_key(array $array, array ...$arrays): array
array_diff_uassoc(array $array, array ...$arrays, callable $key_compare_func): array
array_diff_ukey(array $array, array ...$arrays, callable $key_compare_func): array
array_diff(array $array, array ...$arrays): array
array_fill_keys(array $keys, mixed $value): array
array_fill(int $start_index, int $count, mixed $value): array
array_filter(array $array, ?callable $callback = null, int $mode = 0): array
array_find(array $array, callable $callback): mixed
array_find_key(array $array, callable $callback): mixed
array_flip(array $array): array
array_intersect_assoc(array $array, array ...$arrays): array
array_intersect_key(array $array, array ...$arrays): array
array_intersect_uassoc(array $array, array ...$arrays, callable $key_compare_func): array
array_intersect_ukey(array $array, array ...$arrays, callable $key_compare_func): array
array_intersect(array $array, array ...$arrays): array
array_is_list(array $array): bool
array_key_exists(string|int $key, array $array): bool
array_key_first(array $array): int|string|null
array_key_last(array $array): int|string|null
array_keys(array $array): array
array_keys(array $array, mixed $search_value, bool $strict = false): array
array_map(?callable $callback, array $array, array ...$arrays): array
array_merge_recursive(array ...$arrays): array
array_merge(array ...$arrays): array
array_multisort(array &$array1, mixed $array1_sort_order = SORT_ASC, mixed $array1_sort_flags = SORT_REGULAR, mixed ...$rest): bool
array_pad(array $array, int $length, mixed $value): array
array_pop(array &$array): mixed
array_product(array $array): int|float
array_push(array &$array, mixed ...$values): int
array_rand(array $array, int $num = 1): int|string|array
array_reduce(array $array, callable $callback, mixed $initial = null): mixed
array_replace_recursive(array $array, array ...$replacements): array
array_replace(array $array, array ...$replacements): array
array_reverse(array $array, bool $preserve_keys = false): array
array_search(mixed $needle, array $haystack, bool $strict = false): int|string|false
array_shift(array &$array): mixed
array_slice(array $array, int $offset, ?int $length = null, bool $preserve_keys = false): array
array_splice(array &$array, int $offset, ?int $length = null, mixed $replacement = []): array
array_sum(array $array): int|float
array_udiff_assoc(array $array, array ...$arrays, callable $value_compare_func): array
array_udiff_uassoc(array $array, array ...$arrays, callable $value_compare_func, callable $key_compare_func): array
array_udiff(array $array, array ...$arrays, callable $value_compare_func): array
array_uintersect_assoc(array $array, array ...$arrays, callable $value_compare_func): array
array_uintersect_uassoc(array $array1, array ...$arrays, callable $value_compare_func, callable $key_compare_func): array
array_uintersect(array $array, array ...$arrays, callable $value_compare_func): array
array_unique(array $array, int $flags = SORT_STRING): array
array_unshift(array &$array, mixed ...$values): int
array_values(array $array): array
array_walk_recursive(array|object &$array, callable $callback, mixed $arg = null): bool
array_walk(array|object &$array, callable $callback, mixed $arg = null): bool
array(mixed ...$values): array
arsort(array &$array, int $flags = SORT_REGULAR): bool
asort(array &$array, int $flags = SORT_REGULAR): bool
compact(array|string $var_name, array|string ...$var_names): array
count(Countable|array $value, int $mode = COUNT_NORMAL): int
current(array|object $array): mixed
end(array|object &$array): mixed
extract(array &$array, int $flags = EXTR_OVERWRITE, string $prefix = ""): int
in_array(mixed $needle, array $haystack, bool $strict = false): bool
key_exists()
key(array|object $array): int|string|null
krsort(array &$array, int $flags = SORT_REGULAR): bool
ksort(array &$array, int $flags = SORT_REGULAR): bool
list(mixed $var, mixed ...$vars = ?): array
natcasesort(array &$array): bool
natsort(array &$array): bool
next(array|object &$array): mixed
pos()
prev(array|object &$array): mixed
range(string|int|float $start, string|int|float $end, int|float $step = 1): array
reset(array|object &$array): mixed
rsort(array &$array, int $flags = SORT_REGULAR): bool
shuffle(array &$array): bool
sizeof()
sort(array &$array, int $flags = SORT_REGULAR): bool
uasort(array &$array, callable $callback): bool
uksort(array &$array, callable $callback): bool
usort(array &$array, callable $callback): bool
}

{ // Class/Object Information
class_alias(string $class, string $alias, bool $autoload = true): bool
class_exists(string $class, bool $autoload = true): bool
enum_exists(string $enum, bool $autoload = true): bool
get_called_class(): string
get_class_methods(object|string $object_or_class): array
get_class_vars(string $class): array
get_class(object $object = ?): string
get_declared_classes(): array
get_declared_interfaces(): array
get_declared_traits(): array
get_mangled_object_vars(object $object): array
get_object_vars(object $object): array
get_parent_class(object|string $object_or_class = ?): string
interface_exists(string $interface, bool $autoload = true): bool
is_a(mixed $object_or_class, string $class, bool $allow_string = false): bool
is_subclass_of(mixed $object_or_class, string $class, bool $allow_string = true): bool
method_exists(object|string $object_or_class, string $method): bool
property_exists(object|string $object_or_class, string $property): bool
trait_exists(string $trait, bool $autoload = true): bool
}

{ // Function Handling
call_user_func_array(callable $callback, array $args): mixed
call_user_func(callable $callback, mixed ...$args): mixed
forward_static_call_array(callable $callback, array $args): mixed
forward_static_call(callable $callback, mixed ...$args): mixed
func_get_arg(int $position): mixed
func_get_args(): array
func_num_args(): int
function_exists(string $function): bool
get_defined_functions(bool $exclude_disabled = true): array
register_shutdown_function(callable $callback, mixed ...$args): ?bool
register_tick_function(callable $callback, mixed ...$args): bool
unregister_tick_function(callable $callback): void
}

{ // Reflection
class Reflection {
	public static getModifierNames(int $modifiers): array
}
class ReflectionClass implements Reflector {
	const int IS_IMPLICIT_ABSTRACT = 16;
	const int IS_EXPLICIT_ABSTRACT = 32;
	const int IS_FINAL = 64;
	public string $name;

	public __construct(object|string $objectOrClass)
	public getAttributes(?string $name = null, int $flags = 0): array
	public getConstant(string $name): mixed
	public getConstants(?int $filter = null): array
	public getConstructor(): ?ReflectionMethod
	public getDefaultProperties(): array
	public getDocComment(): string|false
	public getEndLine(): int|false
	public getExtension(): ?ReflectionExtension
	public getExtensionName(): string|false
	public getFileName(): string|false
	public getInterfaceNames(): array
	public getInterfaces(): array
	public getMethod(string $name): ReflectionMethod
	public getMethods(?int $filter = null): array
	public getModifiers(): int
	public getName(): string
	public getNamespaceName(): string
	public getParentClass(): ReflectionClass|false
	public getProperties(?int $filter = null): array
	public getProperty(string $name): ReflectionProperty
	public getReflectionConstant(string $name): ReflectionClassConstant|false
	public getReflectionConstants(?int $filter = null): array
	public getShortName(): string
	public getStartLine(): int|false
	public getStaticProperties(): ?array
	public getStaticPropertyValue(string $name, mixed &$def_value = ?): mixed
	public getTraitAliases(): array
	public getTraitNames(): array
	public getTraits(): array
	public hasConstant(string $name): bool
	public hasMethod(string $name): bool
	public hasProperty(string $name): bool
	public implementsInterface(ReflectionClass|string $interface): bool
	public inNamespace(): bool
	public isAbstract(): bool
	public isAnonymous(): bool
	public isCloneable(): bool
	public isEnum(): bool
	public isFinal(): bool
	public isInstance(object $object): bool
	public isInstantiable(): bool
	public isInterface(): bool
	public isInternal(): bool
	public isIterable(): bool
	public isSubclassOf(ReflectionClass|string $class): bool
	public isTrait(): bool
	public isUserDefined(): bool
	public newInstance(mixed ...$args): object
	public newInstanceArgs(array $args = []): ?object
	public newInstanceWithoutConstructor(): object
	public setStaticPropertyValue(string $name, mixed $value): void
	public __toString(): string
}
class ReflectionClassConstant implements Reflector {
	const int IS_PUBLIC = 1;
	const int IS_PROTECTED = 2;
	const int IS_PRIVATE = 4;
	public string $name;
	public string $class;

	public __construct(object|string $class, string $constant)
	public getAttributes(?string $name = null, int $flags = 0): array
	public getDeclaringClass(): ReflectionClass
	public getDocComment(): string|false
	public getModifiers(): int
	public getName(): string
	public getValue(): mixed
	public isFinal(): bool
	public isPrivate(): bool
	public isProtected(): bool
	public isPublic(): bool
	public __toString(): string
}
class ReflectionEnum extends ReflectionClass {
	public __construct(object|string $objectOrClass)
	public getBackingType(): ?ReflectionType
	public getCase(string $name): ReflectionEnumUnitCase
	public getCases(): array
	public hasCase(string $name): bool
	public isBacked(): bool
}
class ReflectionEnumUnitCase extends ReflectionClassConstant {
	public __construct(object|string $class, string $constant)
	public getEnum(): ReflectionEnum
	public getValue(): UnitEnum
}
class ReflectionEnumBackedCase extends ReflectionEnumUnitCase {
	public string $name;
	public string $class;

	public __construct(object|string $class, string $constant)
	public getBackingValue(): int|string
}
class ReflectionZendExtension implements Reflector {
	public string $name;

	public __construct(string $name)
	private __clone(): void
	public getAuthor(): string
	public getCopyright(): string
	public getName(): string
	public getURL(): string
	public getVersion(): string
	public __toString(): string
}
class ReflectionExtension implements Reflector {
	public string $name;

	public __construct(string $name)
	private __clone(): void
	public getClasses(): array
	public getClassNames(): array
	public getConstants(): array
	public getDependencies(): array
	public getFunctions(): array
	public getINIEntries(): array
	public getName(): string
	public getVersion(): ?string
	public info(): void
	public isPersistent(): bool
	public isTemporary(): bool
	public __toString(): string
}
class ReflectionFunction extends ReflectionFunctionAbstract {
	const int IS_DEPRECATED;

	public __construct(Closure|string $function)
	public static export(string $name, string $return = ?): string
	public getClosure(): Closure
	public invoke(mixed ...$args): mixed
	public invokeArgs(array $args): mixed
	public __toString(): string
}
abstract class ReflectionFunctionAbstract implements Reflector {
	public string $name;

	private __clone(): void
	public getAttributes(?string $name = null, int $flags = 0): array
	public getClosureScopeClass(): ?ReflectionClass
	public getClosureThis(): ?object
	public getDocComment(): string|false
	public getEndLine(): int|false
	public getExtension(): ?ReflectionExtension
	public getExtensionName(): string|false
	public getFileName(): string|false
	public getName(): string
	public getNamespaceName(): string
	public getNumberOfParameters(): int
	public getNumberOfRequiredParameters(): int
	public getParameters(): array
	public getReturnType(): ?ReflectionType
	public getShortName(): string
	public getStartLine(): int|false
	public getStaticVariables(): array
	public hasReturnType(): bool
	public inNamespace(): bool
	public isClosure(): bool
	public isDeprecated(): bool
	public isGenerator(): bool
	public isInternal(): bool
	public isUserDefined(): bool
	public isVariadic(): bool
	public returnsReference(): bool
	abstract public __toString(): void
}
class ReflectionMethod extends ReflectionFunctionAbstract {
	const int IS_STATIC = 16;
	const int IS_PUBLIC = 1;
	const int IS_PROTECTED = 2;
	const int IS_PRIVATE = 4;
	const int IS_ABSTRACT = 64;
	const int IS_FINAL = 32;
	public string $class;

	public __construct(object|string $objectOrMethod, string $method)
	public __construct(string $classMethod)
	public static export(string $class, string $name, bool $return = false): string
	public getClosure(?object $object = null): Closure
	public getDeclaringClass(): ReflectionClass
	public getModifiers(): int
	public getPrototype(): ReflectionMethod
	public invoke(?object $object, mixed ...$args): mixed
	public invokeArgs(?object $object, array $args): mixed
	public isAbstract(): bool
	public isConstructor(): bool
	public isDestructor(): bool
	public isFinal(): bool
	public isPrivate(): bool
	public isProtected(): bool
	public isPublic(): bool
	public isStatic(): bool
	public setAccessible(bool $accessible): void
	public __toString(): string
}
class ReflectionNamedType extends ReflectionType {
	public getName(): string
	public isBuiltin(): bool
}
class ReflectionObject extends ReflectionClass {
	public __construct(object $object)
}
class ReflectionParameter implements Reflector {
	public string $name;

	public __construct(string|array|object $function, int|string $param)
	public allowsNull(): bool
	public canBePassedByValue(): bool
	private __clone(): void
	public getAttributes(?string $name = null, int $flags = 0): array
	public getDeclaringClass(): ?ReflectionClass
	public getDeclaringFunction(): ReflectionFunctionAbstract
	public getDefaultValue(): mixed
	public getDefaultValueConstantName(): ?string
	public getName(): string
	public getPosition(): int
	public getType(): ?ReflectionType
	public hasType(): bool
	public isDefaultValueAvailable(): bool
	public isDefaultValueConstant(): bool
	public isOptional(): bool
	public isPassedByReference(): bool
	public isVariadic(): bool
	public __toString(): string
}
class ReflectionProperty implements Reflector {
	const int IS_STATIC = 16;
	const int IS_PUBLIC = 1;
	const int IS_PROTECTED = 2;
	const int IS_PRIVATE = 4;
	public string $name;
	public string $class;

	public __construct(object|string $class, string $property)
	private __clone(): void
	public getAttributes(?string $name = null, int $flags = 0): array
	public getDeclaringClass(): ReflectionClass
	public getDefaultValue(): mixed
	public getDocComment(): string|false
	public getModifiers(): int
	public getName(): string
	public getType(): ?ReflectionType
	public getValue(?object $object = null): mixed
	public hasDefaultValue(): bool
	public hasType(): bool
	public isDefault(): bool
	public isInitialized(?object $object = null): bool
	public isPrivate(): bool
	public isPromoted(): bool
	public isProtected(): bool
	public isPublic(): bool
	public isReadOnly(): bool
	public isStatic(): bool
	public setAccessible(bool $accessible): void
	public setValue(object $object, mixed $value): void
	public setValue(mixed $value): void
	public __toString(): string
}
abstract class ReflectionType implements Stringable {
	public allowsNull(): bool
}
class ReflectionUnionType extends ReflectionType {
	public getTypes(): array
}
final class ReflectionGenerator {
	public __construct(Generator $generator)
	public getExecutingFile(): string
	public getExecutingGenerator(): Generator
	public getExecutingLine(): int
	public getFunction(): ReflectionFunctionAbstract
	public getThis(): ?object
	public getTrace(int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT): array
}
final class ReflectionFiber {
	public __construct(Fiber $fiber)
	public getCallable(): callable
	public getExecutingFile(): string
	public getExecutingLine(): int
	public getFiber(): Fiber
	public getTrace(int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT): array
}
class ReflectionIntersectionType extends ReflectionType {
	public getTypes(): array
}
final class ReflectionReference {
	public static fromArrayElement(array $array, int|string $key): ?ReflectionReference
	public getId(): string
}
class ReflectionAttribute implements Reflector {
	public getArguments(): array
	public getName(): string
	public getTarget(): int
	public isRepeated(): bool
	public newInstance(): object
}
interface Reflector extends Stringable {
	public static export(): string
	public __toString(): string
}
class ReflectionException extends Exception {}
}

{ // Variable handling
boolval(mixed $value): bool
debug_zval_dump(mixed $value, mixed ...$values): void
doubleval()
empty(mixed $var): bool
floatval(mixed $value): float
get_debug_type(mixed $value): string
get_defined_vars(): array
get_resource_id(resource $resource): int
get_resource_type(resource $resource): string
gettype(mixed $value): string
intval(mixed $value, int $base = 10): int
is_array(mixed $value): bool
is_bool(mixed $value): bool
is_callable(mixed $value, bool $syntax_only = false, string &$callable_name = null): bool
is_countable(mixed $value): bool
is_double()
is_float(mixed $value): bool
is_int(mixed $value): bool
is_integer()
is_iterable(mixed $value): bool
is_long()
is_null(mixed $value): bool
is_numeric(mixed $value): bool
is_object(mixed $value): bool
is_resource(mixed $value): bool
is_scalar(mixed $value): bool
is_string(mixed $value): bool
isset(mixed $var, mixed ...$vars): bool
print_r(mixed $value, bool $return = false): string|bool
serialize(mixed $value): string
settype(mixed &$var, string $type): bool
strval(mixed $value): string
unserialize(string $data, array $options = []): mixed
unset(mixed $var, mixed ...$vars): void
var_dump(mixed $value, mixed ...$values): void
var_export(mixed $value, bool $return = false): ?string
}
}

{ // XML Manipulation
{ // SimpleXML
class SimpleXMLElement implements Stringable, Countable, RecursiveIterator {
	public __construct(string $data, int $options = 0, bool $dataIsURL = false, string $namespaceOrPrefix = "", bool $isPrefix = false)
	public addAttribute(string $qualifiedName, string $value, ?string $namespace = null): void
	public addChild(string $qualifiedName, ?string $value = null, ?string $namespace = null): ?SimpleXMLElement
	public asXML(?string $filename = null): string|bool
	public attributes(?string $namespaceOrPrefix = null, bool $isPrefix = false): ?SimpleXMLElement
	public children(?string $namespaceOrPrefix = null, bool $isPrefix = false): ?SimpleXMLElement
	public count(): int
	public getDocNamespaces(bool $recursive = false, bool $fromRoot = true): array|false
	public getName(): string
	public getNamespaces(bool $recursive = false): array
	public registerXPathNamespace(string $prefix, string $namespace): bool
	public __toString(): string
	public xpath(string $expression): array|null|false
}
class SimpleXMLIterator extends SimpleXMLElement {
	public current(): mixed
	public getChildren(): SimpleXMLIterator
	public hasChildren(): bool
	public key(): mixed
	public next(): void
	public rewind(): void
	public valid(): bool
}

simplexml_import_dom(SimpleXMLElement|DOMNode $node, ?string $class_name = SimpleXMLElement::class): ?SimpleXMLElement
simplexml_load_file(string $filename, ?string $class_name = SimpleXMLElement::class, int $options = 0, string $namespace_or_prefix = "", bool $is_prefix = false): SimpleXMLElement|false
simplexml_load_string(string $data, ?string $class_name = SimpleXMLElement::class, int $options = 0, string $namespace_or_prefix = "", bool $is_prefix = false): SimpleXMLElement|false
}
}

//! phpdoc				=======================================================
// https://docs.phpdoc.org/guide/references/phpdoc/tags/index.html#tag-reference
/**
 * @api
 * @author [name] [<email address>]
 * @category [description]
 * @copyright [description]
 * @deprecated [<Semantic Version>] [<description>]
 * @example [location] [<start-line> [<number-of-lines>] ] [<description>]
 * {@example [location] [<start-line> [<number-of-lines>] ] [<description>]}
 * @filesource
 * @final [description]
 * @global [Type] [name] @global [Type] [description]
 * @ignore [<description>]
 * @internal [description]
 * {@internal [description]}
 * @license [<url>] [name]
 * @link [URI] [<description>]
 * {@link [URI] [<description>]}
 * @method [[static] return type] [name]([[type] [parameter]<, ...>]) [<description>]
 * @package [level 1]\\[level 2]\\[etc.]
 * @param [<Type>] [name] [<description>]
 * @property [Type] [name] [<description>]
 * @property-read [Type] [name] [<description>]
 * @property-write [Type] [name] [<description>]
 * @return [Type] [<description>]
 * @see [URI | FQSEN] [<description>]
 * {@see [URI | FQSEN] [<description>]}
 * @since [<version>] [<description>]
 * @source [<start-line> [<number-of-lines>] ] [<description>]
 * @subpackage [name]
 * @throws [Type] [<description>]
 * @todo [description]
 * @uses [FQSEN] [<description>]
 * @used-by [FQSEN] [<description>]
 * @var ["Type"] [element_name] [<description>]
 * @version [<Semantic Version>] [<description>]
 */
?>
