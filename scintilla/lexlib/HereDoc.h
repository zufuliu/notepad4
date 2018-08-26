//@file HereDoc.h

#ifndef HEREDOC_H
#define HEREDOC_H

namespace Scintilla {

#define HERE_DELIM_MAX 256		// maximum length of HERE doc delimiter
#define QUOTE_DELIM_STACK_MAX	7

// Class to manage HERE doc sequence
class HereDocCls {
public:
	int State;
	// 0: '<<' encountered
	// 1: collect the delimiter
	// 1b: text between the end of the delimiter and the EOL
	// 2: here doc text (lines after the delimiter)
	int Quote;		// the char after '<<'
	bool Quoted;	// true if Quote in ('\'','"','`')
	bool Indented;	// indented delimiter (for <<-)
	int DelimiterLength;	// strlen(Delimiter)
	char Delimiter[HERE_DELIM_MAX];	// the Delimiter

	HereDocCls() noexcept;
	void Append(int ch) noexcept;
	~HereDocCls() = default;
};

// Class to manage quote pairs
class QuoteCls {
public:
	int Rep;
	int	Count;
	int Up, Down;

	QuoteCls() noexcept {
		this->New();
	}
	void New(int r = 1) noexcept;
	void Open(int u) noexcept;
	void Start(int u) noexcept;
	QuoteCls(const QuoteCls &q) noexcept;
	QuoteCls& operator=(const QuoteCls &q) noexcept;
};

// Class to manage quote pairs that nest
class QuoteStackCls {
public:
	int Count;
	int Up, Down;
	int Style;
	int Depth;			// levels pushed
	int CountStack[QUOTE_DELIM_STACK_MAX];
	int UpStack[QUOTE_DELIM_STACK_MAX];
	int StyleStack[QUOTE_DELIM_STACK_MAX];

	QuoteStackCls() noexcept;
	void Start(int u, int s) noexcept;
	void Push(int u, int s) noexcept;
	void Pop() noexcept;
	~QuoteStackCls() = default;
};


}

#endif
