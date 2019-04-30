#define NP2_AUTOC_USE_STRING_ORDER	1
// scintilla/src/AutoComplete.h AutoComplete::maxItemLen
#define NP2_AUTOC_MAX_WORD_LENGTH	(1024 - 3 - 1 - 16)	// SP + '(' + ')' + '\0'
#define NP2_AUTOC_INIT_BUF_SIZE		(4096)
#define NP2_AUTOC_MAX_BUF_COUNT		16
#define NP2_AUTOC_INIT_CACHE_BYTES	(4096)
#define NP2_AUTOC_MAX_CACHE_COUNT	16
/*
word buffer:
(2**16 - 1)*4096 => 256 MiB

node cache:
a = [4096*2**i for i in range(16)] => 256 MiB
x64: sum(i//40 for i in a) => 6710776 nodes
x86: sum(i//24 for i in a) => 11184632 nodes
*/

// required for SSE2
#define DefaultAlignment		16
static inline unsigned int align_up(unsigned int value) {
	return (value + DefaultAlignment - 1) & (~(DefaultAlignment - 1));
}

static inline void* align_ptr(void *ptr) {
	return (void *)(((uintptr_t)(ptr) + DefaultAlignment - 1) & (~(DefaultAlignment - 1)));
}

static inline unsigned int bswap32(unsigned int x) {
	return (x << 24) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | (x >> 24);
}

struct WordNode;
struct WordList {
	char wordBuf[1024];
	int (*WL_strcmp)(LPCSTR, LPCSTR);
	int (*WL_strncmp)(LPCSTR, LPCSTR, size_t);
#if NP2_AUTOC_USE_STRING_ORDER
	UINT (*WL_OrderFunc)(const void *, unsigned int);
#endif
	struct WordNode *pListHead;
	LPCSTR pWordStart;

	char *bufferList[NP2_AUTOC_MAX_BUF_COUNT];
	char *buffer;
	int bufferCount;
	int offset;
	int capacity;

	int nWordCount;
	int nTotalLen;
	UINT orderStart;
	int iStartLen;
	int iMaxLength;

	struct WordNode *nodeCacheList[NP2_AUTOC_MAX_CACHE_COUNT];
	struct WordNode *nodeCache;
	int cacheCount;
	int cacheIndex;
	int cacheCapacity;
	int cacheBytes;
};

// TODO: replace _stricmp() and _strnicmp() with other functions
// which correctly case insensitively compares UTF-8 string and ANSI string.

#if NP2_AUTOC_USE_STRING_ORDER
#define NP2_AUTOC_ORDER_LENGTH	4
#define NP2_AUTOC_MAX_ORDER_LENGTH	4

UINT WordList_Order(const void *pWord, unsigned int len) {
#if 0
	unsigned int high = 0;
	const unsigned char *ptr = (const unsigned char *)pWord;
	len = min_u(len, 4);
	while (len) {
		high = (high << 8) | *ptr++;
		--len;
	}
#else
	unsigned int high = *((const unsigned int *)pWord);
	if (len < NP2_AUTOC_ORDER_LENGTH) {
		high &= ((1U << len * 8) - 1);
	}
	high = bswap32(high);
#endif
	return high;
}

UINT WordList_OrderCase(const void *pWord, unsigned int len) {
	unsigned int high = 0;
	const unsigned char *ptr = (const unsigned char *)pWord;
	len = min_u(len, 4);
	while (len) {
		unsigned char ch = *ptr++;
		if (ch >= 'A' && ch <= 'Z') {
			ch = ch + 'a' - 'A';
		}
		high = (high << 8) | ch;
		--len;
	}
	return high;
}
#endif

// Tree
struct WordNode {
	union {
		struct WordNode *link[2];
		struct {
			struct WordNode *left;
			struct WordNode *right;
		};
	};
	char *word;
#if NP2_AUTOC_USE_STRING_ORDER
	UINT order;
#endif
	int len;
	int level;
};

#define NP2_TREE_HEIGHT_LIMIT	32
// TODO: since the tree is sorted, nodes greater than some level can be deleted to reduce total words.
// or only limit word count in WordList_GetList().

// Andersson Tree, source from http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_andersson.aspx
// see also https://en.wikipedia.org/wiki/AA_tree
#define aa_tree_skew(t) \
	if ((t)->level && (t)->left && (t)->level == (t)->left->level) {\
		struct WordNode *save = (t)->left;					\
		(t)->left = save->right;							\
		save->right = (t);									\
		(t) = save;											\
	}
#define aa_tree_split(t) \
	if ((t)->level && (t)->right && (t)->right->right && (t)->level == (t)->right->right->level) {\
		struct WordNode *save = (t)->right;					\
		(t)->right = save->left;							\
		save->left = (t);									\
		(t) = save;											\
		++(t)->level;										\
	}

static inline void WordList_AddBuffer(struct WordList *pWList) {
	char *buffer = (char *)NP2HeapAlloc(pWList->capacity);
	char *align = (char *)align_ptr(buffer);
	pWList->bufferList[pWList->bufferCount] = buffer;
	pWList->buffer = buffer;
	pWList->bufferCount++;
	pWList->offset = (int)(align - buffer);
}

static inline void WordList_AddCache(struct WordList *pWList) {
	struct WordNode *node = (struct WordNode *)NP2HeapAlloc(pWList->cacheBytes);
	pWList->nodeCacheList[pWList->cacheCount] = node;
	pWList->nodeCache = node;
	pWList->cacheCount++;
	pWList->cacheIndex = 0;
	pWList->cacheCapacity = pWList->cacheBytes / ((int)sizeof(struct WordNode));
}

void WordList_AddWord(struct WordList *pWList, LPCSTR pWord, int len) {
	struct WordNode *root = pWList->pListHead;
#if NP2_AUTOC_USE_STRING_ORDER
	const UINT order = (pWList->iStartLen > NP2_AUTOC_MAX_ORDER_LENGTH) ? 0 : pWList->WL_OrderFunc(pWord, len);
#endif
	if (root == NULL) {
		struct WordNode *node;
		node = pWList->nodeCache + pWList->cacheIndex++;
		node->word = pWList->buffer + pWList->offset;

		CopyMemory(node->word, pWord, len);
#if NP2_AUTOC_USE_STRING_ORDER
		node->order = order;
#endif
		node->len = len;
		node->level = 1;
		root = node;
	} else {
		struct WordNode *iter = root;
		struct WordNode *path[NP2_TREE_HEIGHT_LIMIT] = { NULL };
		int top = 0;
		int dir;

		// find a spot and save the path
		for (;;) {
			path[top++] = iter;
#if NP2_AUTOC_USE_STRING_ORDER
			dir = (int)(iter->order - order);
			if (dir == 0 && (len > NP2_AUTOC_ORDER_LENGTH || iter->len > NP2_AUTOC_ORDER_LENGTH)) {
				dir = pWList->WL_strcmp(iter->word, pWord);
			}
#else
			dir = pWList->WL_strcmp(iter->word, pWord);
#endif
			if (dir == 0) {
				return;
			}
			dir = dir < 0;
			if (iter->link[dir] == NULL) {
				break;
			}
			iter = iter->link[dir];
		}

		if (pWList->cacheIndex + 1 > pWList->cacheCapacity) {
			pWList->cacheBytes <<= 1;
			WordList_AddCache(pWList);
		}
		if (pWList->capacity < pWList->offset + len + 1) {
			pWList->capacity <<= 1;
			WordList_AddBuffer(pWList);
		}

		struct WordNode *node = pWList->nodeCache + pWList->cacheIndex++;
		node->word = pWList->buffer + pWList->offset;

		CopyMemory(node->word, pWord, len);
#if NP2_AUTOC_USE_STRING_ORDER
		node->order = order;
#endif
		node->len = len;
		node->level = 1;
		iter->link[dir] = node;

		// walk back and rebalance
		while (--top >= 0) {
			// which child?
			if (top != 0) {
				dir = path[top - 1]->right == path[top];
			}
			aa_tree_skew(path[top]);
			aa_tree_split(path[top]);
			// fix the parent
			if (top != 0) {
				path[top - 1]->link[dir] = path[top];
			} else {
				root = path[top];
			}
		}
	}

	pWList->pListHead = root;
	pWList->nWordCount++;
	pWList->nTotalLen += len + 1;
	pWList->offset += (int)align_up(len + 1);
	if (len > pWList->iMaxLength) {
		pWList->iMaxLength = len;
	}
}

void WordList_Free(struct WordList *pWList) {
	for (int i = 0; i < pWList->cacheCount; i++) {
		NP2HeapFree(pWList->nodeCacheList[i]);
	}
	for (int i = 0; i < pWList->bufferCount; i++) {
		NP2HeapFree(pWList->bufferList[i]);
	}
}

void WordList_GetList(struct WordList *pWList, char* *pList) {
	struct WordNode *root = pWList->pListHead;
	struct WordNode *path[NP2_TREE_HEIGHT_LIMIT] = { NULL };
	int top = 0;
	char *buf = (char *)NP2HeapAlloc(pWList->nTotalLen + 1);// additional separator
	*pList = buf;

	while (root || top > 0) {
		if (root) {
			path[top++] = root;
			root = root->left;
		} else {
			root = path[--top];
			CopyMemory(buf, root->word, root->len);
			buf += root->len;
			*buf++ = '\n'; // the separator char
			root = root->right;
		}
	}
	// trim last separator char
	if (buf && buf != *pList) {
		*(--buf) = 0;
	}
}

struct WordList *WordList_Alloc(LPCSTR pRoot, int iRootLen, BOOL bIgnoreCase) {
	struct WordList *pWList = (struct WordList *)NP2HeapAlloc(sizeof(struct WordList));
	pWList->pListHead =  NULL;
	pWList->pWordStart = pRoot;
	pWList->nWordCount = 0;
	pWList->nTotalLen = 0;
	pWList->iStartLen = iRootLen;
	pWList->iMaxLength = iRootLen;

	if (bIgnoreCase) {
		pWList->WL_strcmp = _stricmp;
		pWList->WL_strncmp = _strnicmp;
#if NP2_AUTOC_USE_STRING_ORDER
		pWList->WL_OrderFunc = WordList_OrderCase;
#endif
	} else {
		pWList->WL_strcmp = strcmp;
		pWList->WL_strncmp = strncmp;
#if NP2_AUTOC_USE_STRING_ORDER
		pWList->WL_OrderFunc = WordList_Order;
#endif
	}
#if NP2_AUTOC_USE_STRING_ORDER
	pWList->orderStart = pWList->WL_OrderFunc(pRoot, iRootLen);
#endif

	pWList->capacity = NP2_AUTOC_INIT_BUF_SIZE;
	WordList_AddBuffer(pWList);
	pWList->cacheBytes = NP2_AUTOC_INIT_CACHE_BYTES;
	WordList_AddCache(pWList);
	return pWList;
}

static inline void WordList_UpdateRoot(struct WordList *pWList, LPCSTR pRoot, int iRootLen) {
	pWList->pWordStart = pRoot;
	pWList->iStartLen = iRootLen;
	pWList->iMaxLength = (pWList->nWordCount == 0) ? iRootLen : max_i(iRootLen, pWList->iMaxLength);
#if NP2_AUTOC_USE_STRING_ORDER
	pWList->orderStart = pWList->WL_OrderFunc(pRoot, iRootLen);
#endif
}

static inline BOOL WordList_StartsWith(const struct WordList *pWList, LPCSTR pWord) {
#if NP2_AUTOC_USE_STRING_ORDER
	if (pWList->iStartLen > NP2_AUTOC_ORDER_LENGTH) {
		return pWList->WL_strncmp(pWList->pWordStart, pWord, pWList->iStartLen) == 0;
	}
	if (pWList->orderStart != pWList->WL_OrderFunc(pWord, pWList->iStartLen)) {
		return FALSE;
	}
	return TRUE;
#else
	return pWList->WL_strncmp(pWList->pWordStart, pWord, pWList->iStartLen) == 0;
#endif
}

void WordList_AddListEx(struct WordList *pWList, LPCSTR pList) {
	char *word = pWList->wordBuf;
	const int iStartLen = pWList->iStartLen;
	int len = 0;
	BOOL ok = FALSE;
	do {
		const char *sub = strpbrk(pList, " \t.,();^\n\r");
		if (sub) {
			int lenSub = (int)(sub - pList);
			lenSub = min_i(NP2_AUTOC_MAX_WORD_LENGTH - len, lenSub);
			memcpy(word + len, pList, lenSub);
			len += lenSub;
			if (len >= iStartLen) {
				if (*sub == '(') {
					word[len++] = '(';
					word[len++] = ')';
				}
				word[len] = 0;
				if (ok || WordList_StartsWith(pWList, word)) {
					WordList_AddWord(pWList, word, len);
					ok = *sub == '.';
				}
			}
			if (*sub == '^') {
				word[len++] = ' ';
			} else if (!ok && *sub != '.') {
				len = 0;
			} else {
				word[len++] = '.';
			}
			pList = ++sub;
		} else {
			int lenSub = lstrlenA(pList);
			lenSub = min_i(NP2_AUTOC_MAX_WORD_LENGTH - len, lenSub);
			if (len) {
				memcpy(word + len, pList, lenSub);
				len += lenSub;
				word[len] = '\0';
				pList = word;
			} else {
				len = lenSub;
			}
			if (len >= iStartLen) {
				if (ok || WordList_StartsWith(pWList, pList)) {
					WordList_AddWord(pWList, pList, len);
				}
			}
			break;
		}
	} while (*pList);
}

static inline void WordList_AddList(struct WordList *pWList, LPCSTR pList) {
	if (StrNotEmptyA(pList)) {
		WordList_AddListEx(pWList, pList);
	}
}
