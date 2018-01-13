#include "EditLexer.h"
#include "EditStyle.h"

static KEYWORDLIST Keywords_SQL = {{
"abort action add after all allocate alter analyze and any as asc asensitive at attach audit authorization auto_increment autoincrement before begin between both break by call cascade cascaded case cast change check close cluster coalesce collate collation column comment commit condition conflict connect connection "
"constraint continue convert create cross current current_date current_path current_time current_timestamp current_user "
"cursor database deallocate dec declare default deferrable deferred delete desc describe descriptor detach deterministic disconnect distinct do drop dynamic each else elseif end escape except exception exclusive exec execute exists "
"exit explain external fail false fetch file first for foreign free from full function get glob global go goto grant group handler having hold identity if "
"ignore immediate in index indexed initially inner inout insensitive insert instead intersect interval into is isnull iterate join key kill language last leading leave left level like limit load local localtime localtimestamp locator lock "
"loop match merge modifies national natural next no none not notnull null nullif of offset old on open optimize option or order out outer over partition path plan pragma precision prepare primary prior privileges procedure public query raise range read reads recursive "
"references regexp reindex release rename repeat replace resignal restrict result return returns revoke right rollback row rowid rows rtrim savepoint schema select sensitive "
"session session_user set signal size some specific sql sqlexception sqlstate sqlwarning start static synonym system system_user table temp temporary then to trailing "
"transaction trigger truncate true undo union unique update usage use user using vacuum values view virtual when whenever where while with within without write "
"nocase zerofill "

"delimiter until "
, // type
"bigint binary bit blob boolean char character clob date datetime day decimal double enum float hour int int2 int8 integer long mediumint minute month nchar native nvarchar numeric real second smallint text time timestamp tinyint unsigned varbinary varchar varcharacter varying year "
// MySQL
"tinyblob mediumblob longblob tinytext mediumtext longtext "

, // function
"abs() avg() bit_length() char_length() character_length() compress() concat() count() div() group_concat() hex() ifnull() length() lower() ltrim() max() min() mod() octet_length() position() random() randomblob() round() rtrim() strftime() std() substr() substring() sum() space() total() trim() typeof() upper() xor() zeroblob() "
// SQLite
"changes() instr() last_insert_rowid() likelihood() likely() load_extension() printf() quote() soundex() sqlite_compileoption_get() sqlite_compileoption_used() sqlite_source_id() sqlite_version() total_changes() unicode() unlikely() julianday() rtree() fts3() fts4() "
, "", "", "",
// upper case
"ABORT ACTION ADD AFTER ALL ALLOCATE ALTER ANALYZE AND ANY AS ASC ASENSITIVE AT ATTACH AUDIT AUTHORIZATION AUTO_INCREMENT AUTOINCREMENT BEFORE BEGIN BETWEEN BOTH BREAK BY CALL CASCADE CASCADED CASE CAST CHANGE CHECK CLOSE CLUSTER COALESCE COLLATE COLLATION COLUMN COMMENT COMMIT CONDITION CONFLICT CONNECT CONNECTION "
"CONSTRAINT CONTINUE CONVERT CREATE CROSS CURRENT CURRENT_DATE CURRENT_PATH CURRENT_TIME CURRENT_TIMESTAMP CURRENT_USER "
"CURSOR DATABASE DEALLOCATE DEC DECLARE DEFAULT DEFERRABLE DEFERRED DELETE DESC DESCRIBE DESCRIPTOR DETACH DETERMINISTIC DISCONNECT DISTINCT DO DROP DYNAMIC EACH ELSE ELSEIF END ESCAPE EXCEPT EXCEPTION EXCLUSIVE EXEC EXECUTE EXISTS "
"EXIT EXPLAIN EXTERNAL FAIL FALSE FETCH FILE FIRST FOR FOREIGN FREE FROM FULL FUNCTION GET GLOB GLOBAL GO GOTO GRANT GROUP HANDLER HAVING HOLD IDENTITY IF "
"IGNORE IMMEDIATE IN INDEX INDEXED INITIALLY INNER INOUT INSENSITIVE INSERT INSTEAD INTERSECT INTERVAL INTO IS ISNULL ITERATE JOIN KEY KILL LANGUAGE LAST LEADING LEAVE LEFT LEVEL LIKE LIMIT LOAD LOCAL LOCALTIME LOCALTIMESTAMP LOCATOR LOCK "
"LOOP MATCH MERGE MODIFIES NATIONAL NATURAL NEXT NO NONE NOT NOTNULL NULL NULLIF OF OFFSET OLD ON OPEN OPTIMIZE OPTION OR ORDER OUT OUTER OVER PARTITION PATH PLAN PRAGMA PRECISION PREPARE PRIMARY PRIOR PRIVILEGES PROCEDURE PUBLIC QUERY RAISE RANGE READ READS RECURSIVE "
"REFERENCES REGEXP REINDEX RELEASE RENAME REPEAT REPLACE RESIGNAL RESTRICT RESULT RETURN RETURNS REVOKE RIGHT ROLLBACK ROW ROWID ROWS RTRIM SAVEPOINT SCHEMA SELECT SENSITIVE "
"SESSION SESSION_USER SET SIGNAL SIZE SOME SPECIFIC SQL SQLEXCEPTION SQLSTATE SQLWARNING START STATIC SYNONYM SYSTEM SYSTEM_USER TABLE TEMP TEMPORARY THEN TO TRAILING "
"TRANSACTION TRIGGER TRUNCATE TRUE UNDO UNION UNIQUE UPDATE USAGE USE USER USING VACUUM VALUES VIEW VIRTUAL WHEN WHENEVER WHERE WHILE WITH WITHIN WITHOUT WRITE "
"NOCASE ZEROFILL "

"DELIMITER UNTIL "
,
"BIGINT BINARY BIT BLOB BOOLEAN CHAR CHARACTER CLOB DATE DATETIME DAY DECIMAL DOUBLE ENUM FLOAT HOUR INT INT2 INT8 INTEGER LONG MEDIUMINT MINUTE MONTH NCHAR NATIVE NVARCHAR NUMERIC REAL SECOND SMALLINT TEXT TIME TIMESTAMP TINYINT UNSIGNED VARBINARY VARCHAR VARCHARACTER VARYING YEAR "
// MySQL
"TINYBLOB MEDIUMBLOB LONGBLOB TINYTEXT MEDIUMTEXT LONGTEXT "
,
"ABS() AVG() BIT_LENGTH() CHAR_LENGTH() CHARACTER_LENGTH() COMPRESS() CONCAT() COUNT() DIV() GROUP_CONCAT() HEX() IFNULL() LENGTH() LOWER() LTRIM() MAX() MIN() MOD() OCTET_LENGTH() POSITION() RANDOM() RANDOMBLOB() ROUND() RTRIM() STRFTIME() STD() SUBSTR() SUBSTRING() SUM() SPACE() TOTAL() TRIM() TYPEOF() UPPER() XOR() ZEROBLOB() "
// SQLite
"CHANGES() INSTR() LAST_INSERT_ROWID() LIKELIHOOD() LIKELY() LOAD_EXTENSION() PRINTF() QUOTE() SOUNDEX() SQLITE_COMPILEOPTION_GET() SQLITE_COMPILEOPTION_USED() SQLITE_SOURCE_ID() SQLITE_VERSION() TOTAL_CHANGES() UNICODE() UNLIKELY() JULIANDAY() RTREE() FTS3() FTS4() "

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
