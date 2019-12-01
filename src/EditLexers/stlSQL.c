#include "EditLexer.h"
#include "EditStyleX.h"

static KEYWORDLIST Keywords_SQL = {{
"abort accessible account action active add admin after against aggregate algorithm all allocate alter always analyse analyze and any array as asc ascii asensitive at attach audit authorization authors auto_increment autoextend_size autoincrement avg avg_row_length "
"backup before begin between binlog block both break btree buckets by "
"cache call cascade cascaded case cast catalog_name chain change changed channel charset check checksum cipher class_origin client clone close cluster coalesce code collate collation column columns column_format column_name comment commit committed compact completion component compressed compression concurrent condition conflict connect connection consistent constraint constraint_catalog constraint_name constraint_schema contains context continue contributors convert cpu create cross cube cume_dist current current_date current_path current_time current_timestamp current_user cursor cursor_name "
"data database databases datafile day_hour day_microsecond day_minute day_second deallocate declare default default_auth definer definition deferrable deferred delayed delay_key_write delete dense_rank delimiter desc describe description descriptor des_key_file detach deterministic diagnostics directory disable discard disk disconnect distinct distinctrow div do drop dual dumpfile duplicate dynamic "
"each else elseif empty enable enclosed encryption end ends enforced engine engines error errors escape escaped event events every except exception exchange exclude exclusive exec execute exists exit expansion expire explain export extended extent_size external "
"fail false fast faults fetch fields file file_block_size filter first first_value flush following follows for force foreign format found frac_second free from full fulltext function "
"general generated get get_format get_master_public_key glob global go goto grant grants group grouping groups group_replication "
"handler hash having help high_priority hold histogram history host hosts hour_microsecond hour_minute hour_second "
"identity identified if ignore ignore_server_ids immediate import in inactive index indexed indexes infile initial_size initially inner innobase innodb inout insensitive insert insert_method install instance instead intersect interval into invoker io io_after_gtids io_before_gtids io_thread ipc is isnull isolation issuer iterate "
"join json_table "
"key keys key_block_size kill "
"lag language last last_value lateral lead leading leave leaves left less level like limit linear lines list load local localtime localtimestamp locator lock locked locks logfile logs loop low_priority "
"master master_auto_position master_bind master_compression_algorithms master_connect_retry master_delay master_heartbeat_period master_host master_log_file master_log_pos master_password master_port master_public_key_path master_retry_count master_server_id master_ssl master_ssl_ca master_ssl_capath master_ssl_cert master_ssl_cipher master_ssl_crl master_ssl_crlpath master_ssl_key master_ssl_verify_server_cert master_tls_version master_user master_zstd_compression_level match maxvalue max_connections_per_hour max_queries_per_hour max_rows max_size max_statement_time max_updates_per_hour max_user_connections medium member memory merge message_text microsecond middleint migrate minute_microsecond minute_second min_rows mod mode modifies modify mutex mysql_errno "
"name names national natural ndb ndbcluster nested network_namespace never new next no nocase nodegroup nonblocking none not nothing notnull nowait no_wait no_write_to_binlog nth_value ntile null nullif nulls number "
"of offset oj old old_password on one one_shot only open optimize optimizer_costs option optional optionally options or order ordinality organization others out outer outfile over owner "
"pack_keys page parser parse_gcol_expr partial partition partitioning partitions password path percent_rank persist persist_only phase plan plugin plugins plugin_dir port precedes pragma preceding precision prepare preserve prev primary prior privileges procedure process processlist profile profiles proxy public purge "
"quarter query quick "
"raise range rank read reads read_only read_write rebuild recover recursive redofile redo_buffer_size redundant reference references regexp reindex relay relaylog relay_log_file relay_log_pos relay_thread release reload remote remove rename reorganize repair repeat repeatable replace replicate_do_db replicate_do_table replicate_ignore_db replicate_ignore_table replicate_rewrite_db replicate_wild_do_table replicate_wild_ignore_table replication require reset resignal resource respect restart restore restrict resume result return returned_sqlstate returning returns reuse reverse revoke right rlike role rollback rollup rotate routine row rowid rows row_count row_format row_number rtree rtrim "
"savepoint schedule schema schemas schema_name secondary secondary_engine secondary_load secondary_unload second_microsecond security select sensitive separator sequence serializable server session session_user set share show shutdown signal simple size skip slave slow snapshot socket some soname sounds source spatial specific sql sql_after_gtids sql_after_mts_gaps sql_before_gtids sql_big_result sql_buffer_result sql_cache sql_calc_found_rows sql_no_cache sql_small_result sql_thread sql_tsi_day sql_tsi_frac_second sql_tsi_hour sql_tsi_minute sql_tsi_month sql_tsi_quarter sql_tsi_second sql_tsi_week sql_tsi_year sqlexception sqlstate sqlwarning srid ssl stacked start starting starts static stats_auto_recalc stats_persistent stats_sample_pages status stop storage stored straight_join string subclass_origin subject subpartition subpartitions super suspend swaps switches synonym system system_user "
"table tables tablespace table_checksum table_name temp temporary temptable terminated than then thread_priority ties timestampadd timestampdiff to trailing transaction trigger triggers true truncate type types "
"unbounded uncommitted undefined undo undofile undo_buffer_size unicode uninstall union unique unknown unlock until update upgrade usage use user user_resources use_frm using utc_date utc_time utc_timestamp "
"vacuum validation value values variables vcpu view virtual visible "
"wait warnings week weight_string when whenever where while window with within without work wrapper write "
"x509 xa xid xml xor "
"year_month "
"zerofill "

, // 1 Type
"bigint binary bit blob bool boolean byte char character clob date datetime day dec decimal double enum fixed float float4 float8 hour int int1 int2 int3 int4 int8 integer json long mediumint minute month nchar native nvarchar numeric real second serial signed smallint text time timestamp tinyint unsigned varbinary varchar varcharacter varying year "
// MySQL
"tinyblob mediumblob longblob tinytext mediumtext longtext "
"geometry point linestring polygon multipoint multilinestring multipolygon geomcollection geometrycollection "

, // 2 Function
"abs() acos() adddate() addtime() aes_decrypt() aes_encrypt() any_value() ascii() asin() asymmetric_decrypt() asymmetric_derive() asymmetric_encrypt() asymmetric_sign() asymmetric_verify() atan() atan2() avg() "
"benchmark() bin() bin_to_uuid() binlog_gtid_pos() bit_and() bit_count() bit_length() bit_or() bit_xor() "
"char_length() character_length() charset() ceil() ceiling() coercibility() compress() concat() concat_ws() connection_id() count() conv() convert_tz() cos() cot() crc32() create_asymmetric_priv_key() create_asymmetric_pub_key() create_dh_parameters() create_digest() curdate() current_role() curtime() "
"column_add() column_check() column_create() column_delete() column_exists() column_get() column_json() column_list() "
"date_add() date_format() date_sub() datediff() dayname() dayofmonth() dayofweek() dayofyear() decode() decode_histogram() des_decrypt() des_encrypt() div() degrees() "
"elt() encode() encrypt() exp() export_set() extract() extractvalue() "
"field() find_in_set() floor() format() found_rows() from_base64() from_days() from_unixtime() "
"get_format() get_lock() greatest() group_concat() gtid_subset() gtid_subtract() "
"hex() "
"icu_version() ifnull() inet_aton() inet_ntoa() inet6_aton() inet6_ntoa() is_free_lock() is_ipv4() is_ipv4_compat() is_ipv4_mapped() is_ipv6() is_used_lock() is_uuid() "
"json_append() json_array() json_array_append() json_array_insert() json_arrayagg() json_contains() json_contains_path() json_depth() json_extract() json_insert() json_keys() json_length() json_merge() json_merge_patch() json_merge_preserve() json_object() json_objectagg() json_pretty() json_quote() json_remove() json_replace() json_search() json_set() json_storage_free() json_storage_size() json_type() json_unquote() json_valid() " "json_compact() json_detailed() json_exists() json_loose() json_query() json_value() "
"last_day() last_insert_id() lastval() lcase() least() length() ln() load_file() locate() log() log10() log2() lower() lpad() ltrim() "
"make_set() makedate() maketime() master_gtid_wait() master_pos_wait() max() md5() median() microsecond() mid() min() mod() monthname() "
"name_const() nextval() now() "
"oct() octet_length() old_password() ord() "
"password() percentile_cont() percentile_disc() period_add() period_diff() pi() position() pow() power() "
"quarter() "
"radians() rand() random() random_bytes() randomblob() regexp_instr() regexp_like() regexp_replace() regexp_substr() release_all_locks() release_lock() repeat() reverse() roles_graphml() round() row_count() rpad() rtrim() "
"sec_to_time() setval() sha() sha1() sha2() sign() sin() sleep() space() sql_thread_wait_after_gtids() sqrt() statement_digest() statement_digest_text() str_to_date() strftime() std() stddev() stddev_pop() stddev_samp() strcmp() subdate() substr() substring() substring_index() subtime() sum() sysdate() "
"tan() time_format() time_to_sec() timediff() timestampadd() timestampdiff() to_base64() to_days() to_seconds() total() trim() typeof() "
"ucase() uncompress() uncompressed_length() unhex() unix_timestamp() updatexml() upper() utc_date() utc_time() utc_timestamp() uuid() uuid_short() uuid_to_bin() "
"validate_password_strength() values() var_pop() var_samp() variance() version() "
"wait_for_executed_gtid_set() wait_until_sql_thread_after_gtids() week() weekday() weekofyear() weight_string() "
"xor() "
"yearweek() "
"zeroblob() "
// SQLite
"changes() fts3() fts4() instr() json_array_length() json_patch() json_group_array() json_group_object() json_each() json_tree() julianday() last_insert_rowid() likelihood() likely() load_extension() printf() quote() rtree() soundex() sqlite_compileoption_get() sqlite_compileoption_used() sqlite_source_id() sqlite_version() total_changes() unicode() unlikely() "

, NULL, NULL, NULL

, // 6 Upper Case Keyword
"ABORT ACCESSIBLE ACCOUNT ACTION ACTIVE ADD ADMIN AFTER AGAINST AGGREGATE ALGORITHM ALL ALLOCATE ALTER ALWAYS ANALYSE ANALYZE AND ANY ARRAY AS ASC ASCII ASENSITIVE AT ATTACH AUDIT AUTHORIZATION AUTHORS AUTO_INCREMENT AUTOEXTEND_SIZE AUTOINCREMENT AVG AVG_ROW_LENGTH "
"BACKUP BEFORE BEGIN BETWEEN BINLOG BLOCK BOTH BREAK BTREE BUCKETS BY "
"CACHE CALL CASCADE CASCADED CASE CAST CATALOG_NAME CHAIN CHANGE CHANGED CHANNEL CHARSET CHECK CHECKSUM CIPHER CLASS_ORIGIN CLIENT CLONE CLOSE CLUSTER COALESCE CODE COLLATE COLLATION COLUMN COLUMNS COLUMN_FORMAT COLUMN_NAME COMMENT COMMIT COMMITTED COMPACT COMPLETION COMPONENT COMPRESSED COMPRESSION CONCURRENT CONDITION CONFLICT CONNECT CONNECTION CONSISTENT CONSTRAINT CONSTRAINT_CATALOG CONSTRAINT_NAME CONSTRAINT_SCHEMA CONTAINS CONTEXT CONTINUE CONTRIBUTORS CONVERT CPU CREATE CROSS CUBE CUME_DIST CURRENT CURRENT_DATE CURRENT_PATH CURRENT_TIME CURRENT_TIMESTAMP CURRENT_USER CURSOR CURSOR_NAME "
"DATA DATABASE DATABASES DATAFILE DAY_HOUR DAY_MICROSECOND DAY_MINUTE DAY_SECOND DEALLOCATE DECLARE DEFAULT DEFAULT_AUTH DEFINER DEFINITION DEFERRABLE DEFERRED DELAYED DELAY_KEY_WRITE DELETE DENSE_RANK DELIMITER DESC DESCRIBE DESCRIPTION DESCRIPTOR DES_KEY_FILE DETACH DETERMINISTIC DIAGNOSTICS DIRECTORY DISABLE DISCARD DISK DISCONNECT DISTINCT DISTINCTROW DIV DO DROP DUAL DUMPFILE DUPLICATE DYNAMIC "
"EACH ELSE ELSEIF EMPTY ENABLE ENCLOSED ENCRYPTION END ENDS ENFORCED ENGINE ENGINES ERROR ERRORS ESCAPE ESCAPED EVENT EVENTS EVERY EXCEPT EXCEPTION EXCHANGE EXCLUDE EXCLUSIVE EXEC EXECUTE EXISTS EXIT EXPANSION EXPIRE EXPLAIN EXPORT EXTENDED EXTENT_SIZE EXTERNAL "
"FAIL FALSE FAST FAULTS FETCH FIELDS FILE FILE_BLOCK_SIZE FILTER FIRST FIRST_VALUE FLUSH FOLLOWING FOLLOWS FOR FORCE FOREIGN FORMAT FOUND FRAC_SECOND FREE FROM FULL FULLTEXT FUNCTION "
"GENERAL GENERATED GET GET_FORMAT GET_MASTER_PUBLIC_KEY GLOB GLOBAL GO GOTO GRANT GRANTS GROUP GROUPING GROUPS GROUP_REPLICATION "
"HANDLER HASH HAVING HELP HIGH_PRIORITY HOLD HISTOGRAM HISTORY HOST HOSTS HOUR_MICROSECOND HOUR_MINUTE HOUR_SECOND "
"IDENTITY IDENTIFIED IF IGNORE IGNORE_SERVER_IDS IMMEDIATE IMPORT IN INACTIVE INDEX INDEXED INDEXES INFILE INITIAL_SIZE INITIALLY INNER INNOBASE INNODB INOUT INSENSITIVE INSERT INSERT_METHOD INSTALL INSTANCE INSTEAD INTERSECT INTERVAL INTO INVOKER IO IO_AFTER_GTIDS IO_BEFORE_GTIDS IO_THREAD IPC IS ISNULL ISOLATION ISSUER ITERATE "
"JOIN JSON_TABLE "
"KEY KEYS KEY_BLOCK_SIZE KILL "
"LAG LANGUAGE LAST LAST_VALUE LATERAL LEAD LEADING LEAVE LEAVES LEFT LESS LEVEL LIKE LIMIT LINEAR LINES LIST LOAD LOCAL LOCALTIME LOCALTIMESTAMP LOCATOR LOCK LOCKED LOCKS LOGFILE LOGS LOOP LOW_PRIORITY "
"MASTER MASTER_AUTO_POSITION MASTER_BIND MASTER_COMPRESSION_ALGORITHMS MASTER_CONNECT_RETRY MASTER_DELAY MASTER_HEARTBEAT_PERIOD MASTER_HOST MASTER_LOG_FILE MASTER_LOG_POS MASTER_PASSWORD MASTER_PORT MASTER_PUBLIC_KEY_PATH MASTER_RETRY_COUNT MASTER_SERVER_ID MASTER_SSL MASTER_SSL_CA MASTER_SSL_CAPATH MASTER_SSL_CERT MASTER_SSL_CIPHER MASTER_SSL_CRL MASTER_SSL_CRLPATH MASTER_SSL_KEY MASTER_SSL_VERIFY_SERVER_CERT MASTER_TLS_VERSION MASTER_USER MASTER_ZSTD_COMPRESSION_LEVEL MATCH MAXVALUE MAX_CONNECTIONS_PER_HOUR MAX_QUERIES_PER_HOUR MAX_ROWS MAX_SIZE MAX_STATEMENT_TIME MAX_UPDATES_PER_HOUR MAX_USER_CONNECTIONS MEDIUM MEMBER MEMORY MERGE MESSAGE_TEXT MICROSECOND MIDDLEINT MIGRATE MINUTE_MICROSECOND MINUTE_SECOND MIN_ROWS MOD MODE MODIFIES MODIFY MUTEX MYSQL_ERRNO "
"NAME NAMES NATIONAL NATURAL NDB NDBCLUSTER NESTED NETWORK_NAMESPACE NEVER NEW NEXT NO NOCASE NODEGROUP NONBLOCKING NONE NOT NOTHING NOTNULL NOWAIT NO_WAIT NO_WRITE_TO_BINLOG NTH_VALUE NTILE NULL NULLIF NULLS NUMBER "
"OF OFFSET OJ OLD OLD_PASSWORD ON ONE ONE_SHOT ONLY OPEN OPTIMIZE OPTIMIZER_COSTS OPTION OPTIONAL OPTIONALLY OPTIONS OR ORDER ORDINALITY ORGANIZATION OTHERS OUT OUTER OUTFILE OVER OWNER "
"PACK_KEYS PAGE PARSER PARSE_GCOL_EXPR PARTIAL PARTITION PARTITIONING PARTITIONS PASSWORD PATH PERCENT_RANK PERSIST PERSIST_ONLY PHASE PLAN PLUGIN PLUGINS PLUGIN_DIR PORT PRECEDES PRAGMA PRECEDING PRECISION PREPARE PRESERVE PREV PRIMARY PRIOR PRIVILEGES PROCEDURE PROCESS PROCESSLIST PROFILE PROFILES PROXY PUBLIC PURGE "
"QUARTER QUERY QUICK "
"RAISE RANGE RANK READ READS READ_ONLY READ_WRITE REBUILD RECOVER RECURSIVE REDOFILE REDO_BUFFER_SIZE REDUNDANT REFERENCE REFERENCES REGEXP REINDEX RELAY RELAYLOG RELAY_LOG_FILE RELAY_LOG_POS RELAY_THREAD RELEASE RELOAD REMOTE REMOVE RENAME REORGANIZE REPAIR REPEAT REPEATABLE REPLACE REPLICATE_DO_DB REPLICATE_DO_TABLE REPLICATE_IGNORE_DB REPLICATE_IGNORE_TABLE REPLICATE_REWRITE_DB REPLICATE_WILD_DO_TABLE REPLICATE_WILD_IGNORE_TABLE REPLICATION REQUIRE RESET RESIGNAL RESOURCE RESPECT RESTART RESTORE RESTRICT RESUME RESULT RETURN RETURNED_SQLSTATE RETURNING RETURNS REVERSE REVOKE RIGHT RLIKE ROLLBACK ROLLUP ROTATE ROUTINE ROW ROWID ROWS ROW_COUNT ROW_FORMAT RTREE RTRIM "
"SAVEPOINT SCHEDULE SCHEMA SCHEMAS SCHEMA_NAME SECONDARY SECONDARY_ENGINE SECONDARY_LOAD SECONDARY_UNLOAD SECOND_MICROSECOND SECURITY SELECT SENSITIVE SEPARATOR SEQUENCE SERIALIZABLE SERVER SESSION SESSION_USER SET SHARE SHOW SHUTDOWN SIGNAL SIMPLE SIZE SKIP SLAVE SLOW SNAPSHOT SOCKET SOME SONAME SOUNDS SOURCE SPATIAL SPECIFIC SQL SQL_AFTER_GTIDS SQL_AFTER_MTS_GAPS SQL_BEFORE_GTIDS SQL_BIG_RESULT SQL_BUFFER_RESULT SQL_CACHE SQL_CALC_FOUND_ROWS SQL_NO_CACHE SQL_SMALL_RESULT SQL_THREAD SQL_TSI_DAY SQL_TSI_FRAC_SECOND SQL_TSI_HOUR SQL_TSI_MINUTE SQL_TSI_MONTH SQL_TSI_QUARTER SQL_TSI_SECOND SQL_TSI_WEEK SQL_TSI_YEAR SQLEXCEPTION SQLSTATE SQLWARNING SRID SSL STACKED START STARTING STARTS STATIC STATS_AUTO_RECALC STATS_PERSISTENT STATS_SAMPLE_PAGES STATUS STOP STORAGE STORED STRAIGHT_JOIN STRING SUBCLASS_ORIGIN SUBJECT SUBPARTITION SUBPARTITIONS SUPER SUSPEND SWAPS SWITCHES SYNONYM SYSTEM SYSTEM_USER "
"TABLE TABLES TABLESPACE TABLE_CHECKSUM TABLE_NAME TEMP TEMPORARY TEMPTABLE TERMINATED THAN THEN THREAD_PRIORITY TIES TIMESTAMPADD TIMESTAMPDIFF TO TRAILING TRANSACTION TRIGGER TRIGGERS TRUE TRUNCATE TYPE TYPES "
"UNBOUNDED UNCOMMITTED UNDEFINED UNDO UNDOFILE UNDO_BUFFER_SIZE UNICODE UNINSTALL UNION UNIQUE UNKNOWN UNLOCK UNTIL UPDATE UPGRADE USAGE USE USER USER_RESOURCES USE_FRM USING UTC_DATE UTC_TIME UTC_TIMESTAMP "
"VACUUM VALIDATION VALUE VALUES VARIABLES VCPU VIEW VIRTUAL VISIBLE "
"WAIT WARNINGS WEEK WEIGHT_STRING WHEN WHENEVER WHERE WHILE WINDOW WITH WITHIN WITHOUT WORK WRAPPER WRITE "
"X509 XA XID XML XOR "
"YEAR_MONTH "
"ZEROFILL "

, // 7 Upper Case Type
"BIGINT BINARY BIT BLOB BOOL BOOLEAN BYTE CHAR CHARACTER CLOB DATE DATETIME DAY DEC DECIMAL DOUBLE ENUM FIXED FLOAT FLOAT4 FLOAT8 HOUR INT INT1 INT2 INT3 INT4 INT8 INTEGER JSON LONG MEDIUMINT MINUTE MONTH NCHAR NATIVE NVARCHAR NUMERIC REAL SECOND SERIAL SIGNED SMALLINT TEXT TIME TIMESTAMP TINYINT UNSIGNED VARBINARY VARCHAR VARCHARACTER VARYING YEAR "
// MySQL
"TINYBLOB MEDIUMBLOB LONGBLOB TINYTEXT MEDIUMTEXT LONGTEXT "
"GEOMETRY POINT LINESTRING POLYGON MULTIPOINT MULTILINESTRING MULTIPOLYGON GEOMCOLLECTION GEOMETRYCOLLECTION "

, // 8 Upper Case Function
"ABS() ACOS() ADDDATE() ADDTIME() AES_DECRYPT() AES_ENCRYPT() ANY_VALUE() ASCII() ASIN() ASYMMETRIC_DECRYPT() ASYMMETRIC_DERIVE() ASYMMETRIC_ENCRYPT() ASYMMETRIC_SIGN() ASYMMETRIC_VERIFY() ATAN() ATAN2() AVG() "
"BENCHMARK() BIN() BIN_TO_UUID() BINLOG_GTID_POS() BIT_AND() BIT_COUNT() BIT_LENGTH() BIT_OR() BIT_XOR() "
"CHAR_LENGTH() CHARACTER_LENGTH() CHARSET() CEIL() CEILING() COERCIBILITY() COMPRESS() CONCAT() CONCAT_WS() CONNECTION_ID() COUNT() CONV() CONVERT_TZ() COS() COT() CRC32() CREATE_ASYMMETRIC_PRIV_KEY() CREATE_ASYMMETRIC_PUB_KEY() CREATE_DH_PARAMETERS() CREATE_DIGEST() CURDATE() CURRENT_ROLE() CURTIME() "
"COLUMN_ADD() COLUMN_CHECK() COLUMN_CREATE() COLUMN_DELETE() COLUMN_EXISTS() COLUMN_GET() COLUMN_JSON() COLUMN_LIST() "
"DATE_ADD() DATE_FORMAT() DATE_SUB() DATEDIFF() DAYNAME() DAYOFMONTH() DAYOFWEEK() DAYOFYEAR() DECODE() DECODE_HISTOGRAM() DES_DECRYPT() DES_ENCRYPT() DIV() DEGREES() "
"ELT() ENCODE() ENCRYPT() EXP() EXPORT_SET() EXTRACT() EXTRACTVALUE() "
"FIELD() FIND_IN_SET() FLOOR() FORMAT() FOUND_ROWS() FROM_BASE64() FROM_DAYS() FROM_UNIXTIME() "
"GET_FORMAT() GET_LOCK() GREATEST() GROUP_CONCAT() GTID_SUBSET() GTID_SUBTRACT() "
"HEX() "
"ICU_VERSION() IFNULL() INET_ATON() INET_NTOA() INET6_ATON() INET6_NTOA() IS_FREE_LOCK() IS_IPV4() IS_IPV4_COMPAT() IS_IPV4_MAPPED() IS_IPV6() IS_USED_LOCK() IS_UUID() "
"JSON_APPEND() JSON_ARRAY() JSON_ARRAY_APPEND() JSON_ARRAY_INSERT() JSON_ARRAYAGG() JSON_CONTAINS() JSON_CONTAINS_PATH() JSON_DEPTH() JSON_EXTRACT() JSON_INSERT() JSON_KEYS() JSON_LENGTH() JSON_MERGE() JSON_MERGE_PATCH() JSON_MERGE_PRESERVE() JSON_OBJECT() JSON_OBJECTAGG() JSON_PRETTY() JSON_QUOTE() JSON_REMOVE() JSON_REPLACE() JSON_SEARCH() JSON_SET() JSON_STORAGE_FREE() JSON_STORAGE_SIZE() JSON_TYPE() JSON_UNQUOTE() JSON_VALID() " "JSON_COMPACT() JSON_DETAILED() JSON_EXISTS() JSON_LOOSE() JSON_QUERY() JSON_VALUE() "
"LAST_DAY() LAST_INSERT_ID() LASTVAL() LCASE() LEAST() LENGTH() LN() LOAD_FILE() LOCATE() LOG() LOG10() LOG2() LOWER() LPAD() LTRIM() "
"MAKE_SET() MAKEDATE() MAKETIME() MASTER_GTID_WAIT() MASTER_POS_WAIT() MAX() MD5() MEDIAN() MICROSECOND() MID() MIN() MOD() MONTHNAME() "
"NAME_CONST() NEXTVAL() NOW() "
"OCT() OCTET_LENGTH() OLD_PASSWORD() ORD() "
"PASSWORD() PERCENTILE_CONT() PERCENTILE_DISC() PERIOD_ADD() PERIOD_DIFF() PI() POSITION() POW() POWER() "
"QUARTER() "
"RADIANS() RAND() RANDOM() RANDOM_BYTES() RANDOMBLOB() REGEXP_INSTR() REGEXP_LIKE() REGEXP_REPLACE() REGEXP_SUBSTR() RELEASE_ALL_LOCKS() RELEASE_LOCK() REPEAT() REVERSE() ROLES_GRAPHML() ROUND() ROW_COUNT() RPAD() RTRIM() "
"SEC_TO_TIME() SETVAL() SHA() SHA1() SHA2() SIGN() SIN() SLEEP() SPACE() SQL_THREAD_WAIT_AFTER_GTIDS() SQRT() STATEMENT_DIGEST() STATEMENT_DIGEST_TEXT() STR_TO_DATE() STRFTIME() STD() STRCMP() SUBDATE() SUBSTR() SUBSTRING() SUBSTRING_INDEX() SUBTIME() SUM() SYSDATE() "
"TAN() TIME_FORMAT() TIME_TO_SEC() TIMEDIFF() TIMESTAMPADD() TIMESTAMPDIFF() TO_BASE64() TO_DAYS() TO_SECONDS() TOTAL() TRIM() TYPEOF() "
"UCASE() UNCOMPRESS() UNCOMPRESSED_LENGTH() UNHEX() UNIX_TIMESTAMP() UpdateXml() UPDATEXML() UPPER() UTC_DATE() UTC_TIME() UTC_TIMESTAMP() UUID() UUID_SHORT() UUID_TO_BIN() "
"VALIDATE_PASSWORD_STRENGTH() VALUES() VAR_POP() VAR_SAMP() VARIANCE() VERSION() "
"WAIT_FOR_EXECUTED_GTID_SET() WAIT_UNTIL_SQL_THREAD_AFTER_GTIDS() WEEK() WEEKDAY() WEEKOFYEAR() WEIGHT_STRING() "
"XOR() "
"YEARWEEK() "
"ZEROBLOB() "
// SQLite
"CHANGES() FTS3() FTS4() INSTR() JSON_ARRAY_LENGTH() JSON_PATCH() JSON_GROUP_ARRAY() JSON_GROUP_OBJECT() JSON_EACH() JSON_TREE() JULIANDAY() LAST_INSERT_ROWID() LIKELIHOOD() LIKELY() LOAD_EXTENSION() PRINTF() QUOTE() RTREE() SOUNDEX() SQLITE_COMPILEOPTION_GET() SQLITE_COMPILEOPTION_USED() SQLITE_SOURCE_ID() SQLITE_VERSION() TOTAL_CHANGES() UNICODE() UNLIKELY() "

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_SQL[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_SQL_WORD, NP2StyleX_Keyword, L"bold; fore:#FF8040" },
	{ SCE_SQL_WORD2, NP2StyleX_TypeKeyword, L"bold; fore:#1E90FF" },
	{ SCE_SQL_USER1, NP2StyleX_BasicFunction, L"fore:#FF0080" },
	{ MULTI_STYLE(SCE_SQL_COMMENT, SCE_SQL_COMMENTLINE, SCE_SQL_COMMENTLINEDOC, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ MULTI_STYLE(SCE_SQL_STRING, SCE_SQL_CHARACTER, 0, 0), NP2StyleX_String, L"fore:#008000" },
	{ SCE_SQL_ESCAPECHAR, NP2StyleX_EscapeSequence, L"fore:#0080C0" },
	{ SCE_SQL_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ MULTI_STYLE(SCE_SQL_HEX, SCE_SQL_HEX2, 0, 0), NP2StyleX_BlobHex, L"fore:#C08000" },
	{ MULTI_STYLE(SCE_SQL_BIT, SCE_SQL_BIT2, 0, 0), NP2StyleX_BitField, L"fore:#C08000" },
	{ SCE_SQL_VARIABLE, NP2StyleX_Variable, L"fore:#9E4D2A" },
	{ MULTI_STYLE(SCE_SQL_OPERATOR, SCE_SQL_QOPERATOR, 0, 0), NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexSQL = {
	SCLEX_SQL, NP2LEX_SQL,
	EDITLEXER_HOLE(L"SQL Query", Styles_SQL),
	L"sql; mysql; hsql",
	&Keywords_SQL,
	Styles_SQL
};
