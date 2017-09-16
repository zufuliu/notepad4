//@file HereDoc.cxx

#include "HereDoc.h"

using namespace Scintilla;

static int opposite(int ch) {
	if (ch == '(') return ')';
	if (ch == '[') return ']';
	if (ch == '{') return '}';
	if (ch == '<') return '>';
	return ch;
}


HereDocCls::HereDocCls() {
	State = 0;
	Quote = 0;
	Quoted = false;
	Indented = false;
	DelimiterLength = 0;
	Delimiter[0] = '\0';
}
void HereDocCls::Append(int ch) {
	Delimiter[DelimiterLength++] = static_cast<char>(ch);
	Delimiter[DelimiterLength] = '\0';
}


void QuoteCls::New(int r) {
	Rep   = r;
	Count = 0;
	Up = '\0';
	Down = '\0';
}
void QuoteCls::Open(int u) {
	Count++;
	Up = u;
	Down = opposite(Up);
}
void QuoteCls::Start(int u) {
	Count = 0;
	Open(u);
}
QuoteCls::QuoteCls(const QuoteCls& q) { // copy constructor -- use this for copying in
	Rep = q.Rep;
	Count = q.Count;
	Up = q.Up;
	Down = q.Down;
}
QuoteCls& QuoteCls::operator=(const QuoteCls& q) { // assignment constructor
	if (this != &q) {
		Rep = q.Rep;
		Count = q.Count;
		Up = q.Up;
		Down = q.Down;
	}
	return *this;
}


QuoteStackCls::QuoteStackCls() {
	Count = 0;
	Up    = '\0';
	Down  = '\0';
	Style = 0;
	Depth = 0;
}
void QuoteStackCls::Start(int u, int s) {
	Count = 1;
	Up    = u;
	Down  = opposite(Up);
	Style = s;
}
void QuoteStackCls::Push(int u, int s) {
	if (Depth >= QUOTE_DELIM_STACK_MAX)
		return;
	CountStack[Depth] = Count;
	UpStack   [Depth] = Up;
	StyleStack[Depth] = Style;
	Depth++;
	Count = 1;
	Up    = u;
	Down  = opposite(Up);
	Style = s;
}
void QuoteStackCls::Pop(void) {
	if (Depth <= 0)
		return;
	Depth--;
	Count = CountStack[Depth];
	Up    = UpStack   [Depth];
	Style = StyleStack[Depth];
	Down  = opposite(Up);
}
