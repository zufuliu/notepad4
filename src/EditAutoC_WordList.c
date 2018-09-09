#define NP2_AUTOC_USE_BUF	1
#define NP2_AUTOC_USE_NODE_CACHE	1
#define NP2_AUTOC_USE_STRING_ORDER	1
// scintilla/src/AutoComplete.h AutoComplete::maxItemLen
#define NP2_AUTOC_MAX_WORD_LENGTH	(1024 - 3 - 1 - 16)	// SP + '(' + ')' + '\0'
#define NP2_AUTOC_INIT_BUF_SIZE		(4096)
#define NP2_AUTOC_MAX_BUF_COUNT		12
#define NP2_AUTOC_INIT_CACHE_SIZE	128
#define NP2_AUTOC_MAX_CACHE_COUNT	12

#define DefaultAlignment		16
static inline unsigned int align_up(unsigned int value) {
	return (value + DefaultAlignment - 1) & (~(DefaultAlignment - 1));
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
	int (*WL_OrderFunc)(const void *, unsigned int);
#endif
	struct WordNode *pListHead;
	LPCSTR pWordStart;
#if NP2_AUTOC_USE_BUF
	char *bufferList[NP2_AUTOC_MAX_BUF_COUNT];
	char *buffer;
	int bufferCount;
	int offset;
	int capacity;
#endif
	int nWordCount;
	int nTotalLen;
	int orderStart;
	int iStartLen;
	int iMaxLength;
#if NP2_AUTOC_USE_NODE_CACHE
	struct WordNode *nodeCacheList[NP2_AUTOC_MAX_CACHE_COUNT];
#if !NP2_AUTOC_USE_BUF
	int cacheIndexList[NP2_AUTOC_MAX_CACHE_COUNT];
#endif
	struct WordNode *nodeCache;
	int cacheCount;
	int cacheIndex;
	int cacheCapacity;
#endif
};

#if NP2_AUTOC_USE_STRING_ORDER
#define NP2_AUTOC_ORDER_LENGTH	4

int WordList_Order(const void *pWord, unsigned int len) {
	unsigned int high = *(const unsigned int *)pWord;
	high &= (len < NP2_AUTOC_ORDER_LENGTH) ? ((1U << len * 8) - 1) : UINT_MAX;
	high = bswap32(high);
	return high;
}

int WordList_OrderCase(const void *pWord, unsigned int len) {
	unsigned int high = *(const unsigned int *)pWord;
	high |= 0x20202020;
	high &= (len < NP2_AUTOC_ORDER_LENGTH) ? ((1U << len * 8) - 1) : UINT_MAX;
	high = bswap32(high);
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
	int order;
#endif
	int len;
	int level;
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

void WordList_AddWord(struct WordList *pWList, LPCSTR pWord, int len) {
	struct WordNode *root = pWList->pListHead;
#if NP2_AUTOC_USE_STRING_ORDER
	int order = (pWList->iStartLen > NP2_AUTOC_ORDER_LENGTH) ? 0 : pWList->WL_OrderFunc(pWord, len);
#endif
	if (root == NULL) {
		struct WordNode *node;
#if NP2_AUTOC_USE_NODE_CACHE
		node = pWList->nodeCache + pWList->cacheIndex++;
#else
		node = (struct WordNode *)NP2HeapAlloc(sizeof(struct WordNode));
#endif

#if NP2_AUTOC_USE_BUF
		node->word = pWList->buffer + pWList->offset;
#else
		node->word = NP2HeapAlloc(len + 1);
#endif

		CopyMemory(node->word, pWord, len);
#if NP2_AUTOC_USE_STRING_ORDER
		node->order = order;
#endif
		node->len = len;
		node->level = 1;
		root = node;
	} else {
		struct WordNode *iter = root;
		struct WordNode *path[NP2_TREE_HEIGHT_LIMIT] = {NULL};
		struct WordNode *node = NULL;
		int top = 0, dir;

		// find a spot and save the path
		for (;;) {
			path[top++] = iter;
#if NP2_AUTOC_USE_STRING_ORDER
			dir = iter->order - order;
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

#if NP2_AUTOC_USE_NODE_CACHE
		if (pWList->cacheIndex + 1 > pWList->cacheCapacity) {
#if !NP2_AUTOC_USE_BUF
			pWList->cacheIndexList[pWList->cacheCount - 1] = pWList->cacheIndex;
#endif
			pWList->cacheCapacity <<= 1;
			pWList->cacheIndex = 0;
			pWList->nodeCache = (struct WordNode *)NP2HeapAlloc(pWList->cacheCapacity * sizeof(struct WordNode));
			pWList->nodeCacheList[pWList->cacheCount++] = pWList->nodeCache;
		}
		node = pWList->nodeCache + pWList->cacheIndex++;
#else
		node = (struct WordNode *)NP2HeapAlloc(sizeof(struct WordNode));
#endif

#if NP2_AUTOC_USE_BUF
		if (pWList->capacity < pWList->offset + len + 1) {
			pWList->capacity <<= 1;
			pWList->offset = 0;
			pWList->buffer = (char *)NP2HeapAlloc(pWList->capacity);
			pWList->bufferList[pWList->bufferCount++] = pWList->buffer;
		}
		node->word = pWList->buffer + pWList->offset;
#else
		node->word = NP2HeapAlloc(len + 1);
#endif

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
#if NP2_AUTOC_USE_BUF
	pWList->offset += align_up(len + 1);
#endif
	if (len > pWList->iMaxLength) {
		pWList->iMaxLength = len;
	}
}

void WordList_Free(struct WordList *pWList) {
#if NP2_AUTOC_USE_NODE_CACHE || NP2_AUTOC_USE_BUF
	int i;
#endif
#if NP2_AUTOC_USE_NODE_CACHE
#if !NP2_AUTOC_USE_BUF
	pWList->cacheIndexList[pWList->cacheCount - 1] = pWList->cacheIndex;
#endif
	for (i = 0; i < pWList->cacheCount; i++) {
#if !NP2_AUTOC_USE_BUF
		int j, cacheIndex = pWList->cacheIndexList[i];
		struct WordNode *nodeCache = pWList->nodeCacheList[i];
		for (j = 0; j < cacheIndex; j++) {
			struct WordNode *iter = &(nodeCache[j]);
			NP2HeapFree(iter->word);
		}
#endif
		NP2HeapFree(pWList->nodeCacheList[i]);
	}
#else
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
#endif
#if NP2_AUTOC_USE_BUF
	for (i = 0; i < pWList->bufferCount; i++) {
		NP2HeapFree(pWList->bufferList[i]);
	}
#endif
}

void WordList_GetList(struct WordList *pWList, char * *pList) {
	struct WordNode *root = pWList->pListHead;
	struct WordNode *path[NP2_TREE_HEIGHT_LIMIT] = {NULL};
	int top = 0;
	char *buf;
	*pList = NP2HeapAlloc(pWList->nTotalLen + 1);// additional separator
	buf = *pList;

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
	WordList_Free(pWList);
}

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
	pWList->bufferCount = 1;
	pWList->buffer = (char *)NP2HeapAlloc(NP2_AUTOC_INIT_BUF_SIZE);
	pWList->bufferList[0] = pWList->buffer;
#endif
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
#if NP2_AUTOC_USE_NODE_CACHE
	pWList->cacheCapacity = NP2_AUTOC_INIT_CACHE_SIZE;
	pWList->cacheCount = 1;
	pWList->nodeCache = (struct WordNode *)NP2HeapAlloc(pWList->cacheCapacity * sizeof(struct WordNode));
	pWList->nodeCacheList[0] = pWList->nodeCache;
#endif
	return pWList;
}

static inline BOOL WordList_StartsWith(struct WordList *pWList, LPCSTR pWord) {
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

void WordList_AddList(struct WordList *pWList, LPCSTR pList) {
	char *word = pWList->wordBuf;
	int iStartLen = pWList->iStartLen;
	int len = 0;
	int ok = 0;
	while (*pList) {
		char *sub = StrPBrkA(pList, " \t.,();^\n\r");
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
	}
}
