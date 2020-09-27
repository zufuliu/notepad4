//-*- coding: utf-8 -*-
// Scintilla source code edit control
/** @file LexSQL.cxx
 ** Lexer for SQL, including PL/SQL and SQL*Plus.
 ** Improved by Jérôme LAFORGE <jerome.laforge_AT_gmail_DOT_com> from 2010 to 2012.
 **/
// Copyright 1998-2012 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cassert>
#include <cstring>

#include <string>
#include <vector>
#include <map>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

using namespace Scintilla;

static constexpr bool IsSqlWordChar(int ch, bool sqlAllowDottedWord) noexcept {
	return IsAlphaNumeric(ch) || ch == '_' || (sqlAllowDottedWord && ch == '.');
}

static constexpr bool IsANumberChar(int ch, int chPrev) noexcept {
	return IsADigit(ch) || (ch == '.' && chPrev != '.')
		|| ((ch == '+' || ch == '-') && (chPrev == 'e' || chPrev == 'E'))
		|| ((ch == 'e' || ch == 'E') && IsADigit(chPrev));
}

/*static const char * const sqlWordListDesc[] = {
	"Keywords",
	"Database Objects",
	"PLDoc",
	"SQL*Plus",
	"User Keywords 1",
	"User Keywords 2",
	"User Keywords 3",
	"User Keywords 4",
	0
};*/

static void ColouriseSqlDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords1 = *keywordLists[0];
	const WordList &keywords2 = *keywordLists[1];
	const WordList &kw_user1 = *keywordLists[2];
	//const WordList &kw_pldoc = *keywordLists[2];
	//const WordList &kw_sqlplus = *keywordLists[3];
	//const WordList &kw_user2 = *keywordLists[5];
	//const WordList &kw_user3 = *keywordLists[6];
	//const WordList &kw_user4 = *keywordLists[7];

	const bool sqlBackticksIdentifier = styler.GetPropertyInt("lexer.sql.backticks.identifier", 1) != 0;
	const bool sqlNumbersignComment = styler.GetPropertyInt("lexer.sql.numbersign.comment", 1) != 0;
	const bool sqlBackslashEscapes = styler.GetPropertyInt("lexer.sql.backslash.escapes", 1) != 0;
	const bool sqlAllowDottedWord = styler.GetPropertyInt("lexer.sql.allow.dotted.word", 0) != 0;

	StyleContext sc(startPos, length, initStyle, styler);
	//int styleBeforeDCKeyword = SCE_SQL_DEFAULT;

	while (sc.More()) {
		// Determine if the current state should terminate.
		switch (sc.state) {
		case SCE_SQL_OPERATOR:
			sc.SetState(SCE_SQL_DEFAULT);
			break;
		case SCE_SQL_HEX:
			if (!IsHexDigit(sc.ch)) {
				sc.SetState(SCE_SQL_DEFAULT);
			}
			break;
		case SCE_SQL_HEX2:
			if (sc.ch == '\"' || sc.ch == '\'') {
				sc.ForwardSetState(SCE_SQL_DEFAULT);
			}
			break;
		case SCE_SQL_BIT:
			if (!(sc.ch == '0' || sc.ch == '1')) {
				sc.SetState(SCE_SQL_DEFAULT);
			}
			break;
		case SCE_SQL_BIT2:
			if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_SQL_DEFAULT);
			}
			break;
		case SCE_SQL_NUMBER:
			// We stop the number definition on non-numerical non-dot non-eE non-sign char
			if (!IsANumberChar(sc.ch, sc.chPrev)) {
				sc.SetState(SCE_SQL_DEFAULT);
			}
			break;
		case SCE_SQL_VARIABLE:
			if (!(iswordchar(sc.ch) || sc.ch == '@')) {
				sc.SetState(SCE_SQL_DEFAULT);
			}
			break;
		case SCE_SQL_IDENTIFIER:
			if (!IsSqlWordChar(sc.ch, sqlAllowDottedWord)) {
				constexpr int nextState = SCE_SQL_DEFAULT;
				char s[128];
				const int chNext = sc.GetNextNSChar();
				sc.GetCurrentLowered(s, sizeof(s));
				if (keywords1.InList(s)) {
					if (chNext == '(' && strcmp(s, "repeat") == 0) {
						sc.ChangeState(SCE_SQL_USER1);
					} else {
						sc.ChangeState(SCE_SQL_WORD);
					}
				} else if (keywords2.InList(s)) {
					sc.ChangeState(SCE_SQL_WORD2);
				} else if (chNext == '(' && kw_user1.InListPrefixed(s, '(')) {
					sc.ChangeState(SCE_SQL_USER1);
				}
				//} else if (kw_sqlplus.InListAbbreviated(s, '~')) {
				//	sc.ChangeState(SCE_SQL_SQLPLUS);
				//	if (strncmp(s, "rem", 3) == 0) {
				//		nextState = SCE_SQL_SQLPLUS_COMMENT;
				//	} else if (strncmp(s, "pro", 3) == 0) {
				//		nextState = SCE_SQL_SQLPLUS_PROMPT;
				//	}
				//} else if (kw_user2.InList(s)) {
				//	sc.ChangeState(SCE_SQL_USER2);
				//} else if (kw_user3.InList(s)) {
				//	sc.ChangeState(SCE_SQL_USER3);
				//} else if (kw_user4.InList(s)) {
				//	sc.ChangeState(SCE_SQL_USER4);
				//}
				sc.SetState(nextState);
			}
			break;
		case SCE_SQL_QUOTEDIDENTIFIER:
			if (sc.ch == 0x60) {
				if (sc.chNext == 0x60) {
					sc.Forward();	// Ignore it
				} else {
					sc.ForwardSetState(SCE_SQL_DEFAULT);
				}
			}
			break;
		case SCE_SQL_COMMENT:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_SQL_DEFAULT);
			}
			break;
		case SCE_SQL_COMMENTLINE:
		case SCE_SQL_COMMENTLINEDOC:
			//case SCE_SQL_SQLPLUS_COMMENT:
			//case SCE_SQL_SQLPLUS_PROMPT:
			if (sc.atLineStart) {
				sc.SetState(SCE_SQL_DEFAULT);
			}
			break;
		case SCE_SQL_CHARACTER:
			if ((sqlBackslashEscapes && sc.ch == '\\') || (sc.ch == '\'' && sc.chNext == '\'')) {
				// Escape sequence
				sc.SetState(SCE_SQL_ESCAPECHAR);
				sc.Forward();
				sc.ForwardSetState(SCE_SQL_CHARACTER);
				continue;
			}
			if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_SQL_DEFAULT);
			}
			break;
		case SCE_SQL_STRING:
			if ((sqlBackslashEscapes && sc.ch == '\\') || (sc.ch == '\"' && sc.chNext == '\"')) {
				// Escape sequence
				sc.SetState(SCE_SQL_ESCAPECHAR);
				sc.Forward();
				sc.ForwardSetState(SCE_SQL_STRING);
				continue;
			}
			if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_SQL_DEFAULT);
			}
			break;
		case SCE_SQL_QOPERATOR: {
			// Locate the unique Q operator character
			sc.Complete();
			char qOperator = 0x00;
			for (Sci_PositionU styleStartPos = sc.currentPos; styleStartPos > 0; --styleStartPos) {
				if (styler.StyleAt(styleStartPos - 1) != SCE_SQL_QOPERATOR) {
					qOperator = styler.SafeGetCharAt(styleStartPos + 2);
					break;
				}
			}

			char qComplement = 0x00;

			if (qOperator == '<') {
				qComplement = '>';
			} else if (qOperator == '(') {
				qComplement = ')';
			} else if (qOperator == '{') {
				qComplement = '}';
			} else if (qOperator == '[') {
				qComplement = ']';
			} else {
				qComplement = qOperator;
			}

			if (sc.Match(qComplement, '\'')) {
				sc.Forward();
				sc.ForwardSetState(SCE_SQL_DEFAULT);
			}
		}
		break;
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_SQL_DEFAULT) {
			if (sc.Match('q', '\'') || sc.Match('Q', '\'')) {
				sc.SetState(SCE_SQL_QOPERATOR);
				sc.Forward();
			} else if (sc.ch == '0' && (sc.chNext == 'x' || sc.chNext == 'X')) {
				sc.SetState(SCE_SQL_HEX);
				sc.Forward();
			} else if ((sc.ch == 'x' || sc.ch == 'X') && (sc.chNext == '\"' || sc.chNext == '\'')) {
				sc.SetState(SCE_SQL_HEX2);
				sc.Forward();
			} else if (sc.ch == '0' && (sc.chNext == 'b' || sc.chNext == 'B')) {
				sc.SetState(SCE_SQL_BIT);
				sc.Forward();
			} else if ((sc.ch == 'b' || sc.ch == 'B') && (sc.chNext == '\'')) {
				sc.SetState(SCE_SQL_BIT2);
				sc.Forward();
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_SQL_NUMBER);
			} else if ((sc.ch == '@' && iswordstart(sc.chNext))) {
				sc.SetState(SCE_SQL_VARIABLE);
			} else if (iswordstart(sc.ch)) {
				sc.SetState(SCE_SQL_IDENTIFIER);
			} else if (sc.ch == 0x60 && sqlBackticksIdentifier) {
				sc.SetState(SCE_SQL_QUOTEDIDENTIFIER);
			} else if (sc.Match('/', '*')) {
				sc.SetState(SCE_SQL_COMMENT);
				sc.Forward();	// Eat the * so it isn't used for the end of the comment
			} else if (sc.Match('-', '-')) {
				// MySQL requires a space or control char after --
				// https://dev.mysql.com/doc/mysql/en/ansi-diff-comments.html
				// Perhaps we should enforce that with proper property:
				//~} else if (sc.Match('-', '-', ' ')) {
				sc.SetState(SCE_SQL_COMMENTLINE);
			} else if (sc.ch == '#' && sqlNumbersignComment) {
				sc.SetState(SCE_SQL_COMMENTLINEDOC);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_SQL_CHARACTER);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_SQL_STRING);
			} else if (isoperator(sc.ch)) {
				sc.SetState(SCE_SQL_OPERATOR);
			}
		}
		sc.Forward();
	}

	sc.Complete();
}

enum {
	MASK_NESTED_CASES                         = 0x0001FF,
	MASK_INTO_SELECT_STATEMENT_OR_ASSIGNEMENT = 0x000200,
	MASK_CASE_MERGE_WITHOUT_WHEN_FOUND        = 0x000400,
	MASK_MERGE_STATEMENT                      = 0x000800,
	MASK_INTO_DECLARE                         = 0x001000,
	MASK_INTO_EXCEPTION                       = 0x002000,
	MASK_INTO_CONDITION                       = 0x004000,
	MASK_IGNORE_WHEN                          = 0x008000,
	MASK_INTO_CREATE                          = 0x010000,
	MASK_INTO_CREATE_VIEW                     = 0x020000,
	MASK_INTO_CREATE_VIEW_AS_STATEMENT        = 0x040000
};

typedef unsigned int sql_state_t;

class SQLStates {
public:
	SQLStates() = default;

	void Set(Sci_Position lineNumber, sql_state_t sqlStatesLine) {
		if (sqlStatesLine) {
			sqlStatement.resize(lineNumber + 1, 0);
			sqlStatement[lineNumber] = sqlStatesLine;
		}
	}

	static constexpr sql_state_t IgnoreWhen(sql_state_t sqlStatesLine, bool enable) noexcept {
		if (enable)
			sqlStatesLine |= MASK_IGNORE_WHEN;
		else
			sqlStatesLine &= ~MASK_IGNORE_WHEN;

		return sqlStatesLine;
	}

	static constexpr sql_state_t IntoCondition(sql_state_t sqlStatesLine, bool enable) noexcept {
		if (enable)
			sqlStatesLine |= MASK_INTO_CONDITION;
		else
			sqlStatesLine &= ~MASK_INTO_CONDITION;

		return sqlStatesLine;
	}

	static constexpr sql_state_t IntoExceptionBlock(sql_state_t sqlStatesLine, bool enable) noexcept {
		if (enable)
			sqlStatesLine |= MASK_INTO_EXCEPTION;
		else
			sqlStatesLine &= ~MASK_INTO_EXCEPTION;

		return sqlStatesLine;
	}

	static constexpr sql_state_t IntoDeclareBlock(sql_state_t sqlStatesLine, bool enable) noexcept {
		if (enable)
			sqlStatesLine |= MASK_INTO_DECLARE;
		else
			sqlStatesLine &= ~MASK_INTO_DECLARE;

		return sqlStatesLine;
	}

	static constexpr sql_state_t IntoMergeStatement(sql_state_t sqlStatesLine, bool enable) noexcept {
		if (enable)
			sqlStatesLine |= MASK_MERGE_STATEMENT;
		else
			sqlStatesLine &= ~MASK_MERGE_STATEMENT;

		return sqlStatesLine;
	}

	static constexpr sql_state_t CaseMergeWithoutWhenFound(sql_state_t sqlStatesLine, bool found) noexcept {
		if (found)
			sqlStatesLine |= MASK_CASE_MERGE_WITHOUT_WHEN_FOUND;
		else
			sqlStatesLine &= ~MASK_CASE_MERGE_WITHOUT_WHEN_FOUND;

		return sqlStatesLine;
	}

	static constexpr sql_state_t IntoSelectStatementOrAssignment(sql_state_t sqlStatesLine, bool found) noexcept {
		if (found)
			sqlStatesLine |= MASK_INTO_SELECT_STATEMENT_OR_ASSIGNEMENT;
		else
			sqlStatesLine &= ~MASK_INTO_SELECT_STATEMENT_OR_ASSIGNEMENT;

		return sqlStatesLine;
	}

	static constexpr sql_state_t BeginCaseBlock(sql_state_t sqlStatesLine) noexcept {
		if ((sqlStatesLine & MASK_NESTED_CASES) < MASK_NESTED_CASES) {
			sqlStatesLine++;
		}
		return sqlStatesLine;
	}

	static constexpr sql_state_t EndCaseBlock(sql_state_t sqlStatesLine) noexcept {
		if ((sqlStatesLine & MASK_NESTED_CASES) > 0) {
			sqlStatesLine--;
		}
		return sqlStatesLine;
	}

	static constexpr sql_state_t IntoCreateStatement(sql_state_t sqlStatesLine, bool enable) noexcept {
		if (enable)
			sqlStatesLine |= MASK_INTO_CREATE;
		else
			sqlStatesLine &= ~MASK_INTO_CREATE;

		return sqlStatesLine;
	}

	static constexpr sql_state_t IntoCreateViewStatement(sql_state_t sqlStatesLine, bool enable) noexcept {
		if (enable)
			sqlStatesLine |= MASK_INTO_CREATE_VIEW;
		else
			sqlStatesLine &= ~MASK_INTO_CREATE_VIEW;

		return sqlStatesLine;
	}

	static constexpr sql_state_t IntoCreateViewAsStatement(sql_state_t sqlStatesLine, bool enable) noexcept {
		if (enable)
			sqlStatesLine |= MASK_INTO_CREATE_VIEW_AS_STATEMENT;
		else
			sqlStatesLine &= ~MASK_INTO_CREATE_VIEW_AS_STATEMENT;

		return sqlStatesLine;
	}

	static constexpr bool IsIgnoreWhen(sql_state_t sqlStatesLine) noexcept {
		return (sqlStatesLine & MASK_IGNORE_WHEN) != 0;
	}

	static constexpr bool IsIntoCondition(sql_state_t sqlStatesLine) noexcept {
		return (sqlStatesLine & MASK_INTO_CONDITION) != 0;
	}

	static constexpr bool IsIntoCaseBlock(sql_state_t sqlStatesLine) noexcept {
		return (sqlStatesLine & MASK_NESTED_CASES) != 0;
	}

	static constexpr bool IsIntoExceptionBlock(sql_state_t sqlStatesLine) noexcept {
		return (sqlStatesLine & MASK_INTO_EXCEPTION) != 0;
	}

	static constexpr bool IsIntoSelectStatementOrAssignment(sql_state_t sqlStatesLine) noexcept {
		return (sqlStatesLine & MASK_INTO_SELECT_STATEMENT_OR_ASSIGNEMENT) != 0;
	}

	static constexpr bool IsCaseMergeWithoutWhenFound(sql_state_t sqlStatesLine) noexcept {
		return (sqlStatesLine & MASK_CASE_MERGE_WITHOUT_WHEN_FOUND) != 0;
	}

	static constexpr bool IsIntoDeclareBlock(sql_state_t sqlStatesLine) noexcept {
		return (sqlStatesLine & MASK_INTO_DECLARE) != 0;
	}

	static constexpr bool IsIntoMergeStatement(sql_state_t sqlStatesLine) noexcept {
		return (sqlStatesLine & MASK_MERGE_STATEMENT) != 0;
	}

	static constexpr bool IsIntoCreateStatement(sql_state_t sqlStatesLine) noexcept {
		return (sqlStatesLine & MASK_INTO_CREATE) != 0;
	}

	static constexpr bool IsIntoCreateViewStatement(sql_state_t sqlStatesLine) noexcept {
		return (sqlStatesLine & MASK_INTO_CREATE_VIEW) != 0;
	}

	static constexpr bool IsIntoCreateViewAsStatement(sql_state_t sqlStatesLine) noexcept {
		return (sqlStatesLine & MASK_INTO_CREATE_VIEW_AS_STATEMENT) != 0;
	}

	sql_state_t ForLine(Sci_Position lineNumber) const noexcept {
		if ((lineNumber > 0) && (sqlStatement.size() > static_cast<size_t>(lineNumber))) {
			return sqlStatement[lineNumber];
		}
		return 0;
	}

private:
	std::vector<sql_state_t> sqlStatement;
};

static constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_SQL_COMMENT;
}

static constexpr bool IsCommentStyle (int style) noexcept {
	return style == SCE_SQL_COMMENT || style == SCE_SQL_COMMENTLINE || style == SCE_SQL_COMMENTLINEDOC;
}

#define IsCommentLine(line)			IsLexCommentLine(line, styler, MultiStyle(SCE_SQL_COMMENTLINE, SCE_SQL_COMMENTLINEDOC))

static void FoldSqlDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	const bool foldOnlyBegin = styler.GetPropertyInt("fold.sql.only.begin", 0) != 0;
	const bool foldComment = styler.GetPropertyInt("fold.comment", 1) != 0;
	const bool foldAtElse = styler.GetPropertyInt("fold.sql.at.else", 0) != 0;

	SQLStates sqlStates;
	Sci_PositionU endPos = startPos + length;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		// Backtrack to previous line in case need to fix its fold status for folding block of single-line comments (i.e. '--').
		Sci_Position lastNLPos = -1;
		// And keep going back until we find an operator ';' followed
		// by white-space and/or comments. This will improve folding.
		while (--startPos > 0) {
			const char ch = styler[startPos];
			if (ch == '\n' || (ch == '\r' && styler[startPos + 1] != '\n')) {
				lastNLPos = startPos;
			} else if (ch == ';' && styler.StyleAt(startPos) == SCE_SQL_OPERATOR) {
				bool isAllClear = true;
				for (Sci_Position tempPos = startPos + 1;
					tempPos < lastNLPos;
					++tempPos) {
					const int tempStyle = styler.StyleAt(tempPos);
					if (!IsCommentStyle(tempStyle)
						&& tempStyle != SCE_SQL_DEFAULT) {
						isAllClear = false;
						break;
					}
				}
				if (isAllClear) {
					startPos = lastNLPos + 1;
					break;
				}
			}
		}
		lineCurrent = styler.GetLine(startPos);
		if (lineCurrent > 0)
			levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	}
	// And because folding ends at ';', keep going until we find one
	// Otherwise if create ... view ... as is split over multiple
	// lines the folding won't always update immediately.
	const Sci_PositionU docLength = styler.Length();
	for (; endPos < docLength; ++endPos) {
		if (styler.SafeGetCharAt(endPos) == ';') {
			break;
		}
	}

	int levelNext = levelCurrent;
	char chNext = styler[startPos];
	int style = initStyle;
	int styleNext = styler.StyleAt(startPos);
	bool endFound = false;
	bool isUnfoldingIgnored = false;
	// this statementFound flag avoids to fold when the statement is on only one line by ignoring ELSE or ELSIF
	// eg. "IF condition1 THEN ... ELSIF condition2 THEN ... ELSE ... END IF;"
	bool statementFound = false;
	sql_state_t sqlStatesCurrentLine = 0;
	if (foldOnlyBegin) {
		sqlStatesCurrentLine = sqlStates.ForLine(lineCurrent);
	}

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		const int stylePrev = style;
		chNext = styler.SafeGetCharAt(i + 1);
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (atEOL || (!IsCommentStyle(style) && ch == ';')) {
			if (endFound) {
				//Maybe this is the end of "EXCEPTION" BLOCK (eg. "BEGIN ... EXCEPTION ... END;")
				sqlStatesCurrentLine = SQLStates::IntoExceptionBlock(sqlStatesCurrentLine, false);
			}
			// set endFound and isUnfoldingIgnored to false if EOL is reached or ';' is found
			endFound = false;
			isUnfoldingIgnored = false;
		}
		if ((!IsCommentStyle(style) && ch == ';')) {
			if (SQLStates::IsIntoMergeStatement(sqlStatesCurrentLine)) {
				// This is the end of "MERGE" statement.
				if (!SQLStates::IsCaseMergeWithoutWhenFound(sqlStatesCurrentLine))
					levelNext--;
				sqlStatesCurrentLine = SQLStates::IntoMergeStatement(sqlStatesCurrentLine, false);
				//sqlStatesCurrentLine = SQLStates::WhenThenFound(sqlStatesCurrentLine, false);
				levelNext--;
			}
			if (SQLStates::IsIntoSelectStatementOrAssignment(sqlStatesCurrentLine))
				sqlStatesCurrentLine = SQLStates::IntoSelectStatementOrAssignment(sqlStatesCurrentLine, false);
			if (SQLStates::IsIntoCreateStatement(sqlStatesCurrentLine)) {
				if (SQLStates::IsIntoCreateViewStatement(sqlStatesCurrentLine)) {
					if (SQLStates::IsIntoCreateViewAsStatement(sqlStatesCurrentLine)) {
						levelNext--;
						sqlStatesCurrentLine = SQLStates::IntoCreateViewAsStatement(sqlStatesCurrentLine, false);
					}
					sqlStatesCurrentLine = SQLStates::IntoCreateViewStatement(sqlStatesCurrentLine, false);
				}
				sqlStatesCurrentLine = SQLStates::IntoCreateStatement(sqlStatesCurrentLine, false);
			}
		}
		if (ch == ':' && chNext == '=' && !IsCommentStyle(style))
			sqlStatesCurrentLine = SQLStates::IntoSelectStatementOrAssignment(sqlStatesCurrentLine, true);

		if (foldComment && IsStreamCommentStyle(style)) {
			if (!IsStreamCommentStyle(stylePrev)) {
				levelNext++;
			} else if (!IsStreamCommentStyle(styleNext) && !atEOL) {
				// Comments don't end at end of line and the next character may be unstyled.
				levelNext--;
			}
		}
		// Disable explicit folding; it can often cause problems with non-aware code
		// MySQL needs -- comments to be followed by space or control char
		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			levelNext += IsCommentLine(lineCurrent + 1) - IsCommentLine(lineCurrent - 1);
		}
		if (style == SCE_SQL_OPERATOR) {
			if (ch == '(') {
				if (levelCurrent > levelNext)
					levelCurrent--;
				levelNext++;
			} else if (ch == ')') {
				levelNext--;
			} else if ((foldOnlyBegin) && ch == ';') {
				sqlStatesCurrentLine = SQLStates::IgnoreWhen(sqlStatesCurrentLine, false);
			}
		}
		// If new keyword (cannot trigger on elseif or nullif, does less tests)
		if (style == SCE_SQL_WORD && stylePrev != SCE_SQL_WORD) {
			constexpr int MAX_KW_LEN = 9;	// Maximum length of folding keywords
			char s[MAX_KW_LEN + 2];
			unsigned int j = 0;
			for (; j < MAX_KW_LEN + 1; j++) {
				if (!iswordchar(styler[i + j])) {
					break;
				}
				s[j] = MakeLowerCase(styler[i + j]);
			}
			if (j == MAX_KW_LEN + 1) {
				// Keyword too long, don't test it
				s[0] = '\0';
			} else {
				s[j] = '\0';
			}

			if (!foldOnlyBegin && strcmp(s, "select") == 0) {
				sqlStatesCurrentLine = SQLStates::IntoSelectStatementOrAssignment(sqlStatesCurrentLine, true);
			} else if (strcmp(s, "if") == 0) {
				if (endFound) {
					endFound = false;
					if (foldOnlyBegin && !isUnfoldingIgnored) {
						// this end isn't for begin block, but for if block ("end if;")
						// so ignore previous "end" by increment levelNext.
						levelNext++;
					}
				} else {
					if (!foldOnlyBegin)
						sqlStatesCurrentLine = SQLStates::IntoCondition(sqlStatesCurrentLine, true);
					if (levelCurrent > levelNext) {
						// doesn't include this line into the folding block
						// because doesn't hide IF (eg "END; IF")
						levelCurrent = levelNext;
					}
				}
			} else if (!foldOnlyBegin && strcmp(s, "then") == 0 &&
				SQLStates::IsIntoCondition(sqlStatesCurrentLine)) {
				sqlStatesCurrentLine = SQLStates::IntoCondition(sqlStatesCurrentLine, false);
				if (!foldOnlyBegin) {
					if (levelCurrent > levelNext) {
						levelCurrent = levelNext;
					}
					if (!statementFound)
						levelNext++;
					statementFound = true;
				} else if (levelCurrent > levelNext) {
					// doesn't include this line into the folding block
					// because doesn't hide LOOP or CASE (eg "END; LOOP" or "END; CASE")
					levelCurrent = levelNext;
				}
			} else if (strcmp(s, "loop") == 0 || strcmp(s, "case") == 0 || strcmp(s, "while") == 0 || strcmp(s, "repeat") == 0) {
				if (endFound) {
					endFound = false;
					if (foldOnlyBegin && !isUnfoldingIgnored) {
						// this end isn't for begin block, but for loop block ("end loop;") or case block ("end case;")
						// so ignore previous "end" by increment levelNext.
						levelNext++;
					}
					if ((!foldOnlyBegin) && strcmp(s, "case") == 0) {
						sqlStatesCurrentLine = SQLStates::EndCaseBlock(sqlStatesCurrentLine);
						if (!SQLStates::IsCaseMergeWithoutWhenFound(sqlStatesCurrentLine))
							levelNext--; //again for the "end case;" and block when
					}
				} else if (!foldOnlyBegin) {
					if (strcmp(s, "case") == 0) {
						sqlStatesCurrentLine = SQLStates::BeginCaseBlock(sqlStatesCurrentLine);
						sqlStatesCurrentLine = SQLStates::CaseMergeWithoutWhenFound(sqlStatesCurrentLine, true);
					}

					if (levelCurrent > levelNext)
						levelCurrent = levelNext;

					if (!statementFound)
						levelNext++;

					statementFound = true;
				} else if (levelCurrent > levelNext) {
					// doesn't include this line into the folding block
					// because doesn't hide LOOP or CASE (eg "END; LOOP" or "END; CASE")
					levelCurrent = levelNext;
				}
			} else if ((!foldOnlyBegin) && (foldAtElse && !statementFound) && strcmp(s, "elsif") == 0) {
				// folding for ELSE and ELSIF block only if foldAtElse is set
				// and IF or CASE aren't on only one line with ELSE or ELSIF (with flag statementFound)
				sqlStatesCurrentLine = SQLStates::IntoCondition(sqlStatesCurrentLine, true);
				levelCurrent--;
				levelNext--;
			} else if ((!foldOnlyBegin) && (foldAtElse && !statementFound) && strcmp(s, "else") == 0) {
				// folding for ELSE and ELSIF block only if foldAtElse is set
				// and IF or CASE aren't on only one line with ELSE or ELSIF (with flag statementFound)
				// prevent also ELSE is on the same line (eg. "ELSE ... END IF;")
				statementFound = true;
				if (SQLStates::IsIntoCaseBlock(sqlStatesCurrentLine) && SQLStates::IsCaseMergeWithoutWhenFound(sqlStatesCurrentLine)) {
					sqlStatesCurrentLine = SQLStates::CaseMergeWithoutWhenFound(sqlStatesCurrentLine, false);
					levelNext++;
				} else {
					// we are in same case "} ELSE {" in C language
					levelCurrent--;
				}
			} else if (strcmp(s, "begin") == 0 || strcmp(s, "start") == 0) {
				levelNext++;
				sqlStatesCurrentLine = SQLStates::IntoDeclareBlock(sqlStatesCurrentLine, false);
			} else if ((strcmp(s, "end") == 0) || (strcmp(s, "endif") == 0)) {
				// SQL Anywhere permits IF ... ELSE ... ENDIF
				// will only be active if "endif" appears in the
				// keyword list.
				endFound = true;
				levelNext--;
				if (SQLStates::IsIntoSelectStatementOrAssignment(sqlStatesCurrentLine) && !SQLStates::IsCaseMergeWithoutWhenFound(sqlStatesCurrentLine))
					levelNext--;
				if (levelNext < SC_FOLDLEVELBASE) {
					levelNext = SC_FOLDLEVELBASE;
					isUnfoldingIgnored = true;
				}
			} else if ((!foldOnlyBegin) && strcmp(s, "when") == 0 &&
				!SQLStates::IsIgnoreWhen(sqlStatesCurrentLine) &&
				!SQLStates::IsIntoExceptionBlock(sqlStatesCurrentLine) && (
					SQLStates::IsIntoCaseBlock(sqlStatesCurrentLine) ||
					SQLStates::IsIntoMergeStatement(sqlStatesCurrentLine)
					)
				) {
				sqlStatesCurrentLine = SQLStates::IntoCondition(sqlStatesCurrentLine, true);
				// Don't foldind when CASE and WHEN are on the same line (with flag statementFound) (eg. "CASE selector WHEN expression1 THEN sequence_of_statements1;\n")
				// and same way for MERGE statement.
				if (!statementFound) {
					if (!SQLStates::IsCaseMergeWithoutWhenFound(sqlStatesCurrentLine)) {
						levelCurrent--;
						levelNext--;
					}
					sqlStatesCurrentLine = SQLStates::CaseMergeWithoutWhenFound(sqlStatesCurrentLine, false);
				}
			} else if ((!foldOnlyBegin) && strcmp(s, "exit") == 0) {
				sqlStatesCurrentLine = SQLStates::IgnoreWhen(sqlStatesCurrentLine, true);
			} else if ((!foldOnlyBegin) && !SQLStates::IsIntoDeclareBlock(sqlStatesCurrentLine) && strcmp(s, "exception") == 0) {
				sqlStatesCurrentLine = SQLStates::IntoExceptionBlock(sqlStatesCurrentLine, true);
			} else if ((!foldOnlyBegin) &&
				(strcmp(s, "declare") == 0 || strcmp(s, "function") == 0 ||
					strcmp(s, "procedure") == 0 || strcmp(s, "package") == 0)) {
				sqlStatesCurrentLine = SQLStates::IntoDeclareBlock(sqlStatesCurrentLine, true);
			} else if ((!foldOnlyBegin) && strcmp(s, "merge") == 0) {
				sqlStatesCurrentLine = SQLStates::IntoMergeStatement(sqlStatesCurrentLine, true);
				sqlStatesCurrentLine = SQLStates::CaseMergeWithoutWhenFound(sqlStatesCurrentLine, true);
				levelNext++;
				statementFound = true;
			} else if ((!foldOnlyBegin) &&
				strcmp(s, "create") == 0) {
				sqlStatesCurrentLine = SQLStates::IntoCreateStatement(sqlStatesCurrentLine, true);
			} else if ((!foldOnlyBegin) &&
				strcmp(s, "view") == 0 &&
				SQLStates::IsIntoCreateStatement(sqlStatesCurrentLine)) {
				sqlStatesCurrentLine = SQLStates::IntoCreateViewStatement(sqlStatesCurrentLine, true);
			} else if ((!foldOnlyBegin) &&
				strcmp(s, "as") == 0 &&
				SQLStates::IsIntoCreateViewStatement(sqlStatesCurrentLine) &&
				!SQLStates::IsIntoCreateViewAsStatement(sqlStatesCurrentLine)) {
				sqlStatesCurrentLine = SQLStates::IntoCreateViewAsStatement(sqlStatesCurrentLine, true);
				levelNext++;
			}
		}
		if (atEOL || (i == endPos - 1)) {
			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			levelCurrent = levelNext;
			statementFound = false;
			if (!foldOnlyBegin)
				sqlStates.Set(lineCurrent, sqlStatesCurrentLine);
		}
	}
}

LexerModule lmSQL(SCLEX_SQL, ColouriseSqlDoc, "sql", FoldSqlDoc);
