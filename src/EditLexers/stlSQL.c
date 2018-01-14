#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_SQL = {{
"abort accessible account action add after against aggregate algorithm all allocate alter always analyse analyze and any as asc ascii asensitive at attach audit authorization auto_increment autoextend_size autoincrement avg avg_row_length "
"backup before begin between binlog block both break btree by "
"cache call cascade cascaded case cast catalog_name chain change changed channel charset check checksum cipher class_origin client close cluster coalesce code collate collation column columns column_format column_name comment commit committed compact completion compressed compression concurrent condition conflict connect connection consistent constraint constraint_catalog constraint_name constraint_schema contains context continue convert cpu create cross cube current current_date current_path current_time current_timestamp current_user cursor cursor_name "
"data database databases datafile day_hour day_microsecond day_minute day_second deallocate declare default default_auth definer deferrable deferred delayed delay_key_write delete delimiter desc describe descriptor des_key_file detach deterministic diagnostics directory disable discard disk disconnect distinct distinctrow div do drop dual dumpfile duplicate dynamic "
"each else elseif enable enclosed encryption end ends engine engines error errors escape escaped event events every except exception exchange exclusive exec execute exists exit expansion expire explain export extended extent_size external "
"fail false fast faults fetch fields file file_block_size filter first flush follows for force foreign format found free from full fulltext function "
"general generated geometry geometrycollection get get_format glob global go goto grant grants group group_replication "
"handler hash having help high_priority hold host hosts hour_microsecond hour_minute hour_second "
"identity identified if ignore ignore_server_ids immediate import in index indexed indexes infile initial_size initially inner inout insensitive insert insert_method install instance instead intersect interval into invoker io io_after_gtids io_before_gtids io_thread ipc is isnull isolation issuer iterate "
"join json "
"key keys key_block_size kill "
"language last leading leave leaves left less level like limit linear lines linestring list load local localtime localtimestamp locator lock locks logfile logs loop low_priority "
"master master_auto_position master_bind master_connect_retry master_delay master_heartbeat_period master_host master_log_file master_log_pos master_password master_port master_retry_count master_server_id master_ssl master_ssl_ca master_ssl_capath master_ssl_cert master_ssl_cipher master_ssl_crl master_ssl_crlpath master_ssl_key master_ssl_verify_server_cert master_tls_version master_user match maxvalue max_connections_per_hour max_queries_per_hour max_rows max_size max_statement_time max_updates_per_hour max_user_connections medium memory merge message_text microsecond middleint migrate minute_microsecond minute_second min_rows mod mode modifies modify multilinestring multipoint multipolygon mutex mysql_errno "
"name names national natural ndb ndbcluster never new next no nocase nodegroup nonblocking none not notnull no_wait no_write_to_binlog null nullif number "
"of offset old old_password on one only open optimize optimizer_costs option optionally options or order out outer outfile over owner "
"pack_keys page parser parse_gcol_expr partial partition partitioning partitions password path phase plan plugin plugins plugin_dir point polygon port precedes pragma precision prepare preserve prev primary prior privileges procedure processlist profile profiles proxy public purge "
"quarter query quick "
"raise range read reads read_only read_write rebuild recover recursive redofile redo_buffer_size redundant references regexp reindex relay relaylog relay_log_file relay_log_pos relay_thread release reload remove rename reorganize repair repeat repeatable replace replicate_do_db replicate_do_table replicate_ignore_db replicate_ignore_table replicate_rewrite_db replicate_wild_do_table replicate_wild_ignore_table replication require reset resignal restore restrict resume result return returned_sqlstate returns reverse revoke right rlike rollback rollup rotate routine row rowid rows row_count row_format rtree rtrim "
"savepoint schedule schema schemas schema_name second_microsecond security select sensitive separator serializable server session session_user set share show shutdown signal simple size slave slow snapshot socket some soname sounds source spatial specific sql sql_after_gtids sql_after_mts_gaps sql_before_gtids sql_big_result sql_buffer_result sql_cache sql_calc_found_rows sql_no_cache sql_small_result sql_thread sql_tsi_day sql_tsi_hour sql_tsi_minute sql_tsi_month sql_tsi_quarter sql_tsi_second sql_tsi_week sql_tsi_year sqlexception sqlstate sqlwarning ssl stacked start starting starts static stats_auto_recalc stats_persistent stats_sample_pages status stop storage stored straight_join string subclass_origin subject subpartition subpartitions super suspend swaps switches synonym system system_user "
"table tables tablespace table_checksum table_name temp temporary temptable terminated than then timestampadd timestampdiff to trailing transaction trigger triggers true truncate type types "
"uncommitted undefined undo undofile undo_buffer_size unicode uninstall union unique unknown unlock until update upgrade usage use user user_resources use_frm using utc_date utc_time utc_timestamp "
"vacuum validation value values variables view virtual "
"wait warnings week weight_string when whenever where while with within without work wrapper write "
"x509 xa xid xml xor "
"year_month "
"zerofill "
, // type
"bigint binary bit blob bool boolean byte char character clob date datetime day dec decimal double enum fixed float float4 float8 hour int int1 int2 int3 int4 int8 integer long mediumint minute month nchar native nvarchar numeric real second serial signed smallint text time timestamp tinyint unsigned varbinary varchar varcharacter varying year "
// MySQL
"tinyblob mediumblob longblob tinytext mediumtext longtext "

, // function
"abs() acos() adddate() addtime() any_value() ascii() asin() atan() atan2() avg() "
"bin() bit_and() bit_count() bit_length() bit_or() bit_xor() "
"char_length() character_length() charset() ceil() ceiling() coercibility() compress() concat() concat_ws() connection_id() count() conv() convert_tz() cos() cot() crc32() curdate() curtime() "
"date_add() date_format() date_sub() datediff() dayname() dayofmonth() dayofweek() dayofyear() div() degrees() "
"elt() exp() export_set() extract() extractvalue() "
"field() find_in_set() floor() format() found_rows() from_base64() from_days() from_unixtime() "
"get_format() get_lock() greatest() group_concat() gtid_subset() gtid_subtract() "
"hex() "
"ifnull() inet_aton() inet_ntoa() inet6_aton() inet6_ntoa() is_free_lock() is_ipv4() is_ipv4_compat() is_ipv4_mapped() is_ipv6() is_used_lock() "
"json_append() json_array() json_array_append() json_array_insert() json_arrayagg() json_contains() json_contains_path() json_depth() json_extract() json_insert() json_keys() json_length() json_merge() json_merge_patch() json_merge_preserve() json_object() json_objectagg() json_pretty() json_quote() json_remove() json_replace() json_search() json_set() json_storage_size() json_type() json_unquote() json_valid() "
"last_day() last_insert_id() lcase() least() length() ln() load_file() locate() log() log10() log2() lower() lpad() ltrim() "
"make_set() makedate() maketime() master_pos_wait() max() microsecond() mid() min() mod() monthname() "
"name_const() now() "
"oct() octet_length() ord() "
"period_add() period_diff() pi() position() pow() power() "
"quarter() "
"radians() rand() random() randomblob() release_all_locks() release_lock() repeat() reverse() round() row_count() rpad() rtrim() "
"sec_to_time() sign() sin() sleep() space() sqrt() str_to_date() strftime() std() stddev() stddev_pop() stddev_samp() strcmp() subdate() substr() substring() substring_index() subtime() sum() sysdate() "
"tan() time_format() time_to_sec() timediff() timestampadd() timestampdiff() to_base64() to_days() to_seconds() total() trim() typeof() "
"ucase() upper() unhex() unix_timestamp() updatexml() utc_date() utc_time() utc_timestamp() uuid() uuid_short() "
"var_pop() var_samp() variance() version() "
"wait_for_executed_gtid_set() wait_until_sql_thread_after_gtids() week() weekday() weekofyear() weight_string() "
"xor() "
"yearweek() "
"zeroblob() "
// SQLite
"changes() fts3() fts4() instr() json_array_length() json_patch() json_group_array() json_group_object() json_each() json_tree() julianday() last_insert_rowid() likelihood() likely() load_extension() printf() quote() rtree() soundex() sqlite_compileoption_get() sqlite_compileoption_used() sqlite_source_id() sqlite_version() total_changes() unicode() unlikely() "
, "", "", "",
// upper case
"ABORT ACCESSIBLE ACCOUNT ACTION ADD AFTER AGAINST AGGREGATE ALGORITHM ALL ALLOCATE ALTER ALWAYS ANALYSE ANALYZE AND ANY AS ASC ASCII ASENSITIVE AT ATTACH AUDIT AUTHORIZATION AUTO_INCREMENT AUTOEXTEND_SIZE AUTOINCREMENT AVG AVG_ROW_LENGTH "
"BACKUP BEFORE BEGIN BETWEEN BINLOG BLOCK BOTH BREAK BTREE BY "
"CACHE CALL CASCADE CASCADED CASE CAST CATALOG_NAME CHAIN CHANGE CHANGED CHANNEL CHARSET CHECK CHECKSUM CIPHER CLASS_ORIGIN CLIENT CLOSE CLUSTER COALESCE CODE COLLATE COLLATION COLUMN COLUMNS COLUMN_FORMAT COLUMN_NAME COMMENT COMMIT COMMITTED COMPACT COMPLETION COMPRESSED COMPRESSION CONCURRENT CONDITION CONFLICT CONNECT CONNECTION CONSISTENT CONSTRAINT CONSTRAINT_CATALOG CONSTRAINT_NAME CONSTRAINT_SCHEMA CONTAINS CONTEXT CONTINUE CONVERT CPU CREATE CROSS CUBE CURRENT CURRENT_DATE CURRENT_PATH CURRENT_TIME CURRENT_TIMESTAMP CURRENT_USER CURSOR CURSOR_NAME "
"DATA DATABASE DATABASES DATAFILE DAY_HOUR DAY_MICROSECOND DAY_MINUTE DAY_SECOND DEALLOCATE DECLARE DEFAULT DEFAULT_AUTH DEFINER DEFERRABLE DEFERRED DELAYED DELAY_KEY_WRITE DELETE DELIMITER DESC DESCRIBE DESCRIPTOR DES_KEY_FILE DETACH DETERMINISTIC DIAGNOSTICS DIRECTORY DISABLE DISCARD DISK DISCONNECT DISTINCT DISTINCTROW DIV DO DROP DUAL DUMPFILE DUPLICATE DYNAMIC "
"EACH ELSE ELSEIF ENABLE ENCLOSED ENCRYPTION END ENDS ENGINE ENGINES ERROR ERRORS ESCAPE ESCAPED EVENT EVENTS EVERY EXCEPT EXCEPTION EXCHANGE EXCLUSIVE EXEC EXECUTE EXISTS EXIT EXPANSION EXPIRE EXPLAIN EXPORT EXTENDED EXTENT_SIZE EXTERNAL "
"FAIL FALSE FAST FAULTS FETCH FIELDS FILE FILE_BLOCK_SIZE FILTER FIRST FLUSH FOLLOWS FOR FORCE FOREIGN FORMAT FOUND FREE FROM FULL FULLTEXT FUNCTION "
"GENERAL GENERATED GEOMETRY GEOMETRYCOLLECTION GET GET_FORMAT GLOB GLOBAL GO GOTO GRANT GRANTS GROUP GROUP_REPLICATION "
"HANDLER HASH HAVING HELP HIGH_PRIORITY HOLD HOST HOSTS HOUR_MICROSECOND HOUR_MINUTE HOUR_SECOND "
"IDENTITY IDENTIFIED IF IGNORE IGNORE_SERVER_IDS IMMEDIATE IMPORT IN INDEX INDEXED INDEXES INFILE INITIAL_SIZE INITIALLY INNER INOUT INSENSITIVE INSERT INSERT_METHOD INSTALL INSTANCE INSTEAD INTERSECT INTERVAL INTO INVOKER IO IO_AFTER_GTIDS IO_BEFORE_GTIDS IO_THREAD IPC IS ISNULL ISOLATION ISSUER ITERATE "
"JOIN JSON "
"KEY KEYS KEY_BLOCK_SIZE KILL "
"LANGUAGE LAST LEADING LEAVE LEAVES LEFT LESS LEVEL LIKE LIMIT LINEAR LINES LINESTRING LIST LOAD LOCAL LOCALTIME LOCALTIMESTAMP LOCATOR LOCK LOCKS LOGFILE LOGS LOOP LOW_PRIORITY "
"MASTER MASTER_AUTO_POSITION MASTER_BIND MASTER_CONNECT_RETRY MASTER_DELAY MASTER_HEARTBEAT_PERIOD MASTER_HOST MASTER_LOG_FILE MASTER_LOG_POS MASTER_PASSWORD MASTER_PORT MASTER_RETRY_COUNT MASTER_SERVER_ID MASTER_SSL MASTER_SSL_CA MASTER_SSL_CAPATH MASTER_SSL_CERT MASTER_SSL_CIPHER MASTER_SSL_CRL MASTER_SSL_CRLPATH MASTER_SSL_KEY MASTER_SSL_VERIFY_SERVER_CERT MASTER_TLS_VERSION MASTER_USER MATCH MAXVALUE MAX_CONNECTIONS_PER_HOUR MAX_QUERIES_PER_HOUR MAX_ROWS MAX_SIZE MAX_STATEMENT_TIME MAX_UPDATES_PER_HOUR MAX_USER_CONNECTIONS MEDIUM MEMORY MERGE MESSAGE_TEXT MICROSECOND MIDDLEINT MIGRATE MINUTE_MICROSECOND MINUTE_SECOND MIN_ROWS MOD MODE MODIFIES MODIFY MULTILINESTRING MULTIPOINT MULTIPOLYGON MUTEX MYSQL_ERRNO "
"NAME NAMES NATIONAL NATURAL NDB NDBCLUSTER NEVER NEW NEXT NO NOCASE NODEGROUP NONBLOCKING NONE NOT NOTNULL NO_WAIT NO_WRITE_TO_BINLOG NULL NULLIF NUMBER "
"OF OFFSET OLD OLD_PASSWORD ON ONE ONLY OPEN OPTIMIZE OPTIMIZER_COSTS OPTION OPTIONALLY OPTIONS OR ORDER OUT OUTER OUTFILE OVER OWNER "
"PACK_KEYS PAGE PARSER PARSE_GCOL_EXPR PARTIAL PARTITION PARTITIONING PARTITIONS PASSWORD PATH PHASE PLAN PLUGIN PLUGINS PLUGIN_DIR POINT POLYGON PORT PRECEDES PRAGMA PRECISION PREPARE PRESERVE PREV PRIMARY PRIOR PRIVILEGES PROCEDURE PROCESSLIST PROFILE PROFILES PROXY PUBLIC PURGE "
"QUARTER QUERY QUICK "
"RAISE RANGE READ READS READ_ONLY READ_WRITE REBUILD RECOVER RECURSIVE REDOFILE REDO_BUFFER_SIZE REDUNDANT REFERENCES REGEXP REINDEX RELAY RELAYLOG RELAY_LOG_FILE RELAY_LOG_POS RELAY_THREAD RELEASE RELOAD REMOVE RENAME REORGANIZE REPAIR REPEAT REPEATABLE REPLACE REPLICATE_DO_DB REPLICATE_DO_TABLE REPLICATE_IGNORE_DB REPLICATE_IGNORE_TABLE REPLICATE_REWRITE_DB REPLICATE_WILD_DO_TABLE REPLICATE_WILD_IGNORE_TABLE REPLICATION REQUIRE RESET RESIGNAL RESTORE RESTRICT RESUME RESULT RETURN RETURNED_SQLSTATE RETURNS REVERSE REVOKE RIGHT RLIKE ROLLBACK ROLLUP ROTATE ROUTINE ROW ROWID ROWS ROW_COUNT ROW_FORMAT RTREE RTRIM "
"SAVEPOINT SCHEDULE SCHEMA SCHEMAS SCHEMA_NAME SECOND_MICROSECOND SECURITY SELECT SENSITIVE SEPARATOR SERIALIZABLE SERVER SESSION SESSION_USER SET SHARE SHOW SHUTDOWN SIGNAL SIMPLE SIZE SLAVE SLOW SNAPSHOT SOCKET SOME SONAME SOUNDS SOURCE SPATIAL SPECIFIC SQL SQL_AFTER_GTIDS SQL_AFTER_MTS_GAPS SQL_BEFORE_GTIDS SQL_BIG_RESULT SQL_BUFFER_RESULT SQL_CACHE SQL_CALC_FOUND_ROWS SQL_NO_CACHE SQL_SMALL_RESULT SQL_THREAD SQL_TSI_DAY SQL_TSI_HOUR SQL_TSI_MINUTE SQL_TSI_MONTH SQL_TSI_QUARTER SQL_TSI_SECOND SQL_TSI_WEEK SQL_TSI_YEAR SQLEXCEPTION SQLSTATE SQLWARNING SSL STACKED START STARTING STARTS STATIC STATS_AUTO_RECALC STATS_PERSISTENT STATS_SAMPLE_PAGES STATUS STOP STORAGE STORED STRAIGHT_JOIN STRING SUBCLASS_ORIGIN SUBJECT SUBPARTITION SUBPARTITIONS SUPER SUSPEND SWAPS SWITCHES SYNONYM SYSTEM SYSTEM_USER "
"TABLE TABLES TABLESPACE TABLE_CHECKSUM TABLE_NAME TEMP TEMPORARY TEMPTABLE TERMINATED THAN THEN TIMESTAMPADD TIMESTAMPDIFF TO TRAILING TRANSACTION TRIGGER TRIGGERS TRUE TRUNCATE TYPE TYPES "
"UNCOMMITTED UNDEFINED UNDO UNDOFILE UNDO_BUFFER_SIZE UNICODE UNINSTALL UNION UNIQUE UNKNOWN UNLOCK UNTIL UPDATE UPGRADE USAGE USE USER USER_RESOURCES USE_FRM USING UTC_DATE UTC_TIME UTC_TIMESTAMP "
"VACUUM VALIDATION VALUE VALUES VARIABLES VIEW VIRTUAL "
"WAIT WARNINGS WEEK WEIGHT_STRING WHEN WHENEVER WHERE WHILE WITH WITHIN WITHOUT WORK WRAPPER WRITE "
"X509 XA XID XML XOR "
"YEAR_MONTH "
"ZEROFILL "
,
"BIGINT BINARY BIT BLOB BOOL BOOLEAN BYTE CHAR CHARACTER CLOB DATE DATETIME DAY DEC DECIMAL DOUBLE ENUM FIXED FLOAT FLOAT4 FLOAT8 HOUR INT INT1 INT2 INT3 INT4 INT8 INTEGER LONG MEDIUMINT MINUTE MONTH NCHAR NATIVE NVARCHAR NUMERIC REAL SECOND SERIAL SIGNED SMALLINT TEXT TIME TIMESTAMP TINYINT UNSIGNED VARBINARY VARCHAR VARCHARACTER VARYING YEAR "
// MySQL
"TINYBLOB MEDIUMBLOB LONGBLOB TINYTEXT MEDIUMTEXT LONGTEXT "
,
"ABS() ACOS() ADDDATE() ADDTIME() ANY_VALUE() ASCII() ASIN() ATAN() ATAN2() AVG() "
"BIN() BIT_AND() BIT_COUNT() BIT_LENGTH() BIT_OR() BIT_XOR() "
"CHAR_LENGTH() CHARACTER_LENGTH() CHARSET() CEIL() CEILING() COERCIBILITY() COMPRESS() CONCAT() CONCAT_WS() CONNECTION_ID() COUNT() CONV() CONVERT_TZ() COS() COT() CRC32() CURDATE() CURTIME() "
"DATE_ADD() DATE_FORMAT() DATE_SUB() DATEDIFF() DAYNAME() DAYOFMONTH() DAYOFWEEK() DAYOFYEAR() DIV() DEGREES() "
"ELT() EXP() EXPORT_SET() EXTRACT() EXTRACTVALUE() "
"FIELD() FIND_IN_SET() FLOOR() FORMAT() FOUND_ROWS() FROM_BASE64() FROM_DAYS() FROM_UNIXTIME() "
"GET_FORMAT() GET_LOCK() GREATEST() GROUP_CONCAT() GTID_SUBSET() GTID_SUBTRACT() "
"HEX() "
"IFNULL() INET_ATON() INET_NTOA() INET6_ATON() INET6_NTOA() IS_FREE_LOCK() IS_IPV4() IS_IPV4_COMPAT() IS_IPV4_MAPPED() IS_IPV6() IS_USED_LOCK() "
"JSON_APPEND() JSON_ARRAY() JSON_ARRAY_APPEND() JSON_ARRAY_INSERT() JSON_ARRAYAGG() JSON_CONTAINS() JSON_CONTAINS_PATH() JSON_DEPTH() JSON_EXTRACT() JSON_INSERT() JSON_KEYS() JSON_LENGTH() JSON_MERGE() JSON_MERGE_PATCH() JSON_MERGE_PRESERVE() JSON_OBJECT() JSON_OBJECTAGG() JSON_PRETTY() JSON_QUOTE() JSON_REMOVE() JSON_REPLACE() JSON_SEARCH() JSON_SET() JSON_STORAGE_SIZE() JSON_TYPE() JSON_UNQUOTE() JSON_VALID() "
"LAST_DAY() LAST_INSERT_ID() LCASE() LEAST() LENGTH() LN() LOAD_FILE() LOCATE() LOG() LOG10() LOG2() LOWER() LPAD() LTRIM() "
"MAKE_SET() MAKEDATE() MAKETIME() MASTER_POS_WAIT() MAX() MICROSECOND() MID() MIN() MOD() MONTHNAME() "
"NAME_CONST() NOW() "
"OCT() OCTET_LENGTH() ORD() "
"PERIOD_ADD() PERIOD_DIFF() PI() POSITION() POW() POWER() "
"QUARTER() "
"RADIANS() RAND() RANDOM() RANDOMBLOB() RELEASE_ALL_LOCKS() RELEASE_LOCK() REPEAT() REVERSE() ROUND() ROW_COUNT() RPAD() RTRIM() "
"SEC_TO_TIME() SIGN() SIN() SLEEP() SPACE() SQRT() STR_TO_DATE() STRFTIME() STD() STRCMP() SUBDATE() SUBSTR() SUBSTRING() SUBSTRING_INDEX() SUBTIME() SUM() SYSDATE() "
"TAN() TIME_FORMAT() TIME_TO_SEC() TIMEDIFF() TIMESTAMPADD() TIMESTAMPDIFF() TO_BASE64() TO_DAYS() TO_SECONDS() TOTAL() TRIM() TYPEOF() "
"UCASE() UPPER() UNHEX() UNIX_TIMESTAMP() UPDATEXML() UTC_DATE() UTC_TIME() UTC_TIMESTAMP() UUID() UUID_SHORT() "
"VAR_POP() VAR_SAMP() VARIANCE() VERSION() "
"WAIT_FOR_EXECUTED_GTID_SET() WAIT_UNTIL_SQL_THREAD_AFTER_GTIDS() WEEK() WEEKDAY() WEEKOFYEAR() WEIGHT_STRING() "
"XOR() "
"YEARWEEK() "
"ZEROBLOB() "
// SQLite
"CHANGES() FTS3() FTS4() INSTR() JSON_ARRAY_LENGTH() JSON_PATCH() JSON_GROUP_ARRAY() JSON_GROUP_OBJECT() JSON_EACH() JSON_TREE() JULIANDAY() LAST_INSERT_ROWID() LIKELIHOOD() LIKELY() LOAD_EXTENSION() PRINTF() QUOTE() RTREE() SOUNDEX() SQLITE_COMPILEOPTION_GET() SQLITE_COMPILEOPTION_USED() SQLITE_SOURCE_ID() SQLITE_VERSION() TOTAL_CHANGES() UNICODE() UNLIKELY() "

#if NUMKEYWORD == 16
, "", "", "", "", "", "", ""
#endif
}};

EDITLEXER lexSQL = { SCLEX_SQL, NP2LEX_SQL, L"SQL Query", L"sql; mysql", L"", &Keywords_SQL,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	//{ SCE_SQL_DEFAULT, L"Default", L"", L"" },
	{ SCE_SQL_WORD, NP2STYLE_Keyword, L"Keyword", L"bold; fore:#FF8040", L"" },
	{ SCE_SQL_WORD2, NP2STYLE_TypeKeyword, L"Type Keyword", L"bold; fore:#1E90FF", L"" },
	{ SCE_SQL_USER1, NP2STYLE_BasicFunction, L"Basic Function", L"fore:#FF0080", L"" },
	{ MULTI_STYLE(SCE_SQL_COMMENT, SCE_SQL_COMMENTLINE, SCE_SQL_COMMENTLINEDOC, 0), NP2STYLE_Comment, L"Comment", L"fore:#008000", L"" },
	{ MULTI_STYLE(SCE_SQL_STRING, SCE_SQL_CHARACTER, 0, 0), NP2STYLE_String, L"String", L"fore:#008000; back:#FFF1A8", L"" },
	//{ SCE_SQL_IDENTIFIER, NP2STYLE_Identifier, L"Identifier", L"fore:#800080", L"" },
	//{ SCE_SQL_QUOTEDIDENTIFIER, 63551, L"Quoted Identifier", L"fore:#800080; back:#FFCCFF", L"" },
	{ SCE_SQL_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ MULTI_STYLE(SCE_SQL_HEX, SCE_SQL_HEX2, 0, 0), NP2STYLE_BlobHex, L"Blob Hex", L"fore:#C08000", L""},
	{ MULTI_STYLE(SCE_SQL_BIT, SCE_SQL_BIT2, 0, 0), NP2STYLE_BitField, L"Bit Field ", L"fore:#C08000", L""},
	{ SCE_SQL_VARIABLE, NP2STYLE_Variable, L"Variable", L"fore:#CC3300", L"" },
	{ MULTI_STYLE(SCE_SQL_OPERATOR, SCE_SQL_QOPERATOR, 0, 0), NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	{ -1, 00000, L"", L"", L"" }
}
};
