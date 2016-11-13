#define NP2_AUTOC_USE_LIST	0
#define NP2_AUTOC_USE_BUF	1
// scintilla/src/AutoComplete.h AutoComplete::maxItemLen
#define NP2_AUTOC_MAX_WORD_LENGTH	(1024 - 3 - 1)	// SP + '(' + ')' + '\0'
#define NP2_AUTOC_INIT_BUF_SIZE		(4096)

struct WordNode;
struct WordList {
	int (WINAPI *WL_StrCmpA)(LPCSTR, LPCSTR);
	int (WINAPI *WL_StrCmpNA)(LPCSTR, LPCSTR, int);
	struct WordNode *pListHead;
	LPCSTR pWordStart;
#if NP2_AUTOC_USE_BUF
	char* buffer;
	int capacity;
#endif
	int nWordCount;
	int nTotalLen;
	int iStartLen;
	int iMaxLength;
};

#if NP2_AUTOC_USE_LIST
// Linked List
struct WordNode {
	struct WordNode *next;
#if NP2_AUTOC_USE_BUF
	int offset;
#else
	char *word;
#endif
	int len;
};

void WordList_AddWord(struct WordList *pWList, LPCSTR pWord, int len)
{
	struct WordNode* *pListHead = &(pWList->pListHead);
	struct WordNode *head = *pListHead;
	struct WordNode *prev = NULL;
	int diff = 1;

	while (head) {
#if NP2_AUTOC_USE_BUF
		diff = pWList->WL_StrCmpA(pWord, &pWList->buffer[head->offset]);
#else
		diff = pWList->WL_StrCmpA(pWord, head->word);
#endif
		if (diff <= 0) {
			break;
		}
		prev = head;
		head = head->next;
	}

	if (diff) {
		struct WordNode *node;
		node = (struct WordNode *)NP2HeapAlloc(sizeof(struct WordNode));
		node->len = len;
#if NP2_AUTOC_USE_BUF
		if (pWList->capacity < pWList->nTotalLen + len + 1) {
			pWList->capacity <<= 1;
			pWList->buffer = NP2HeapReAlloc(pWList->buffer, pWList->capacity);
		}
		node->offset = pWList->nTotalLen;
		CopyMemory(&pWList->buffer[node->offset], pWord, len);
		pWList->buffer[node->offset + len] = '\0';
#else
		node->word = NP2HeapAlloc(len + 1);
		CopyMemory(node->word, pWord, len);
#endif
		node->next = head;
		if (prev) {
			prev->next = node;
		} else {
			*pListHead = node;
		}

		pWList->nWordCount++;
		pWList->nTotalLen += len + 1;
		if (len > pWList->iMaxLength) {
			pWList->iMaxLength = len;
		}
	}
}

void WordList_Free(struct WordList *pWList)
{
	struct WordNode *head = pWList->pListHead;
	struct WordNode *prev;

	while (head) {
#if !NP2_AUTOC_USE_BUF
		NP2HeapFree(head->word);
#endif
		prev = head;
		head = head->next;
		NP2HeapFree(prev);
	}
#if NP2_AUTOC_USE_BUF
	NP2HeapFree(pWList->buffer);
#endif
}

void WordList_GetList(struct WordList *pWList, char* *pList)
{
	struct WordNode *head = pWList->pListHead;
	struct WordNode *prev;
	char* buf;
	*pList = NP2HeapAlloc(pWList->nTotalLen + 1);// additional separator
	buf = *pList;

	while (head) {
#if NP2_AUTOC_USE_BUF
		CopyMemory(buf, &pWList->buffer[head->offset], head->len);
#else
		CopyMemory(buf, head->word, head->len);
#endif
		buf += head->len;
		*buf++ = '\n'; // the separator char
#if !NP2_AUTOC_USE_BUF
		NP2HeapFree(head->word);
#endif
		prev = head;
		head = head->next;
		NP2HeapFree(prev);
	}
#if NP2_AUTOC_USE_BUF
	NP2HeapFree(pWList->buffer);
#endif
	// trim last separator char
	if (buf && buf != *pList) {
		*(--buf) = 0;
	}
}

#else

#pragma warning(push)
#pragma warning(disable: 4201) // C4201: nonstandard extension used : nameless struct/union

// Tree
struct WordNode {
	union {
		struct WordNode* link[2];
		struct {
			struct WordNode *left;
			struct WordNode *right;
		};
	};
	int level;
#if NP2_AUTOC_USE_BUF
	int offset;
#else
	char *word;
#endif
	int len;
};

#define NP2_TREE_HEIGHT_LIMIT	16

// Andersson Tree, source from http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_andersson.aspx
#define aa_tree_skew(t) \
if (t->level && t->left && t->level == t->left->level) {\
	struct WordNode *save = t->left;					\
	t->left = save->right;								\
	save->right = t;									\
	t = save;											\
}
#define aa_tree_split(t) \
if (t->level && t->right && t->right->right && t->level == t->right->right->level) {\
	struct WordNode *save = t->right;					\
	t->right = save->left;								\
	save->left = t;										\
	t = save;											\
	++t->level;											\
}

void WordList_AddWord(struct WordList *pWList, LPCSTR pWord, int len)
{
	struct WordNode *root = pWList->pListHead;
	if (root == NULL) {
		struct WordNode *node;
		node = (struct WordNode *)NP2HeapAlloc(sizeof(struct WordNode));
		node->len = len;
#if NP2_AUTOC_USE_BUF
		node->offset = 0;
		CopyMemory(pWList->buffer, pWord, len);
		pWList->buffer[len] = '\0';
#else
		node->word = NP2HeapAlloc(len + 1);
		CopyMemory(node->word, pWord, len);
#endif
		node->level = 1;
		root = node;
	} else {
		struct WordNode *iter = root;
		struct WordNode* path[NP2_TREE_HEIGHT_LIMIT] = {NULL};
		struct WordNode *node = NULL;
		int top = 0, dir;

		// find a spot and save the path
		for (;;) {
			path[top++] = iter;
#if NP2_AUTOC_USE_BUF
			dir = pWList->WL_StrCmpA(&pWList->buffer[iter->offset], pWord);
#else
			dir = pWList->WL_StrCmpA(iter->word, pWord);
#endif
			if (dir == 0)
				return;
			dir = dir < 0;
			if (iter->link[dir] == NULL)
				break;
			iter = iter->link[dir];
		}

		node = (struct WordNode *)NP2HeapAlloc(sizeof(struct WordNode));
		node->len = len;
#if NP2_AUTOC_USE_BUF
		if (pWList->capacity < pWList->nTotalLen + len + 1) {
			pWList->capacity <<= 1;
			pWList->buffer = NP2HeapReAlloc(pWList->buffer, pWList->capacity);
		}
		node->offset = pWList->nTotalLen;
		CopyMemory(&pWList->buffer[node->offset], pWord, len);
		pWList->buffer[node->offset + len] = '\0';
#else
		node->word = NP2HeapAlloc(len + 1);
		CopyMemory(node->word, pWord, len);
#endif
		node->level = 1;
		iter->link[dir] = node;

		// walk back and rebalance
		while (--top >= 0) {
			// which child?
			if (top != 0)
				dir = path[top - 1]->right == path[top];
			aa_tree_skew(path[top]);
			aa_tree_split(path[top]);
			// fix the parent
			if (top != 0)
				path[top - 1]->link[dir] = path[top];
			else
				root = path[top];
		}
	}

	pWList->pListHead = root;
	pWList->nWordCount++;
	pWList->nTotalLen += len + 1;
	if (len > pWList->iMaxLength) {
		pWList->iMaxLength = len;
	}
}

void WordList_Free(struct WordList *pWList)
{
	struct WordNode *iter = pWList->pListHead;
	struct WordNode *save;

	// destruction by rotation
	while (iter) {
		if (iter->left == NULL) {
			save = iter->right;
#if !NP2_AUTOC_USE_BUF
			NP2HeapFree(iter->word);
#endif
			NP2HeapFree(iter);
		} else {
			// rotate right
			save = iter->left;
			iter->left = save->right;
			save->right = iter;
		}
		iter = save;
	}
#if NP2_AUTOC_USE_BUF
	NP2HeapFree(pWList->buffer);
#endif
}

void WordList_GetList(struct WordList *pWList, char* *pList)
{
	struct WordNode *root = pWList->pListHead;
	struct WordNode* path[NP2_TREE_HEIGHT_LIMIT] = {NULL};
	int top = 0;
	char* buf;
	*pList = NP2HeapAlloc(pWList->nTotalLen + 1);// additional separator
	buf = *pList;

	while (root || top > 0) {
		if (root) {
			path[top++] = root;
			root = root->left;
		} else {
			root = path[--top];
#if NP2_AUTOC_USE_BUF
			CopyMemory(buf, &pWList->buffer[root->offset], root->len);
#else
			CopyMemory(buf, root->word, root->len);
#endif
			buf += root->len;
			*buf++ = '\n'; // the separator char
			root = root->right;
		}
	}
	// trim last separator char
	if (buf && buf != *pList) {
		*(--buf) = 0;
	}
	WordList_Free(pWList);
}

#pragma warning(pop) // C4201
#endif

struct WordList *WordList_Alloc(LPCSTR pRoot, int iRootLen, BOOL bIgnoreCase) {
	struct WordList *pWList = (struct WordList *)NP2HeapAlloc(sizeof(struct WordList));
	pWList->pListHead =  NULL;
	pWList->pWordStart = pRoot;
	pWList->nWordCount = 0;
	pWList->nTotalLen = 0;
	pWList->iStartLen = iRootLen;
	pWList->iMaxLength = iRootLen;
#if NP2_AUTOC_USE_BUF
	pWList->capacity = NP2_AUTOC_INIT_BUF_SIZE;
	pWList->buffer = (char *)NP2HeapAlloc(NP2_AUTOC_INIT_BUF_SIZE);
#endif
	if (bIgnoreCase) {
		pWList->WL_StrCmpA = StrCmpIA;
		pWList->WL_StrCmpNA = StrCmpNIA;
	} else {
		pWList->WL_StrCmpA = StrCmpA;
		pWList->WL_StrCmpNA = StrCmpNA;
	}

	return pWList;
}

#define NP2_MAX_KEYWORD_LENGTH		(255)
void WordList_AddList(struct WordList *pWList, LPCSTR pList)
{
	char word[NP2_MAX_KEYWORD_LENGTH + 1] = "";
	int len = 0;
	int ok = 0;
	while (*pList) {
		switch (*pList) {
		case ' ': case '\t': case '.': case ',': case '(':  case ')': case ';': case '\n': case '\r':
			if (len >= pWList->iStartLen) {
				if (*pList == '(' && len < NP2_MAX_KEYWORD_LENGTH - 2) {
					word[len++] = '(';
					word[len++] = ')';
				}
				word[len] = 0;
				if (ok || (pWList->iStartLen && !pWList->WL_StrCmpNA(pWList->pWordStart, word, pWList->iStartLen))) {
					WordList_AddWord(pWList, word, len);
					ok = *pList == '.';
				}
			}
			if (!ok && *pList != '.') {
				len = 0;
			} else if (len < NP2_MAX_KEYWORD_LENGTH) {
				word[len++] = '.';
			}
			break;
		case '^':
			if (len < NP2_MAX_KEYWORD_LENGTH) {
				word[len++] = ' ';
			}
			break;
		default:
			if (len < NP2_MAX_KEYWORD_LENGTH) {
				word[len++] = *pList;
			}
		}
		pList++;
	}
	if (len >= pWList->iStartLen) {
		word[len] = 0;
		if (ok || (pWList->iStartLen && !pWList->WL_StrCmpNA(pWList->pWordStart, word, pWList->iStartLen))) {
			WordList_AddWord(pWList, word, len);
		}
	}
}
