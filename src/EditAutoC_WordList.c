#define NP2_AUTOC_USE_LIST	0
#define NP2_AUTOC_USE_BUF	1
#define NP2_AUTOC_USE_NODE_CACHE	1
// scintilla/src/AutoComplete.h AutoComplete::maxItemLen
#define NP2_AUTOC_MAX_WORD_LENGTH	(1024 - 3 - 1)	// SP + '(' + ')' + '\0'
#define NP2_AUTOC_INIT_BUF_SIZE		(4096)
#define NP2_AUTOC_MAX_BUF_COUNT		12
#define NP2_AUTOC_INIT_CACHE_SIZE	128
#define NP2_AUTOC_MAX_CACHE_COUNT	12

#if NP2_AUTOC_USE_BUF
#define DefaultAlignment		16
__forceinline unsigned int align_up(unsigned int value) {
	return (value + DefaultAlignment - 1) & (~(DefaultAlignment - 1));
}
#endif

struct WordNode;
struct WordList {
	int (WINAPI *WL_StrCmpA)(LPCSTR, LPCSTR);
	int (WINAPI *WL_StrCmpNA)(LPCSTR, LPCSTR, int);
	struct WordNode *pListHead;
	LPCSTR pWordStart;
#if NP2_AUTOC_USE_BUF
	char* bufferList[NP2_AUTOC_MAX_BUF_COUNT];
	char* buffer;
	int bufferCount;
	int offset;
	int capacity;
#endif
	int nWordCount;
	int nTotalLen;
	int iStartLen;
	int iMaxLength;
#if NP2_AUTOC_USE_NODE_CACHE
	struct WordNode* nodeCacheList[NP2_AUTOC_MAX_CACHE_COUNT];
#if !NP2_AUTOC_USE_BUF
	int cacheIndexList[NP2_AUTOC_MAX_CACHE_COUNT];
#endif
	struct WordNode* nodeCache;
	int cacheCount;
	int cacheIndex;
	int cacheCapacity;
#endif
};

#if NP2_AUTOC_USE_LIST
// Linked List
struct WordNode {
	struct WordNode *next;
	char *word;
	int len;
};

void WordList_AddWord(struct WordList *pWList, LPCSTR pWord, int len)
{
	struct WordNode* *pListHead = &(pWList->pListHead);
	struct WordNode *head = *pListHead;
	struct WordNode *prev = NULL;
	int diff = 1;

	while (head) {
		diff = pWList->WL_StrCmpA(pWord, head->word);
		if (diff <= 0) {
			break;
		}
		prev = head;
		head = head->next;
	}

	if (diff) {
		struct WordNode *node;
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
		node->len = len;
		node->next = head;
		if (prev) {
			prev->next = node;
		} else {
			*pListHead = node;
		}

		pWList->nWordCount++;
		pWList->nTotalLen += len + 1;
#if NP2_AUTOC_USE_BUF
		pWList->offset += align_up(len + 1);
#endif
		if (len > pWList->iMaxLength) {
			pWList->iMaxLength = len;
		}
	}
}

void WordList_Free(struct WordList *pWList)
{
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
#endif
#if NP2_AUTOC_USE_BUF
	for (i = 0; i < pWList->bufferCount; i++) {
		NP2HeapFree(pWList->bufferList[i]);
	}
#endif
}

void WordList_GetList(struct WordList *pWList, char* *pList)
{
	struct WordNode *head = pWList->pListHead;
	struct WordNode *prev;
	char* buf;
#if NP2_AUTOC_USE_NODE_CACHE || NP2_AUTOC_USE_BUF
	int i;
#endif
	*pList = NP2HeapAlloc(pWList->nTotalLen + 1);// additional separator
	buf = *pList;

	while (head) {
		CopyMemory(buf, head->word, head->len);
		buf += head->len;
		*buf++ = '\n'; // the separator char
#if !NP2_AUTOC_USE_BUF
		NP2HeapFree(head->word);
#endif
		prev = head;
		head = head->next;
#if !NP2_AUTOC_USE_NODE_CACHE
		NP2HeapFree(prev);
#endif
	}
#if NP2_AUTOC_USE_NODE_CACHE
	for (i = 0; i < pWList->cacheCount; i++) {
		NP2HeapFree(pWList->nodeCacheList[i]);
	}
#endif
#if NP2_AUTOC_USE_BUF
	for (i = 0; i < pWList->bufferCount; i++) {
		NP2HeapFree(pWList->bufferList[i]);
	}
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
	char *word;
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

void WordList_AddWord(struct WordList *pWList, LPCSTR pWord, int len)
{
	struct WordNode *root = pWList->pListHead;
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
		node->len = len;
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
			dir = pWList->WL_StrCmpA(iter->word, pWord);
			if (dir == 0)
				return;
			dir = dir < 0;
			if (iter->link[dir] == NULL)
				break;
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
		node->len = len;
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
#if NP2_AUTOC_USE_BUF
	pWList->offset += align_up(len + 1);
#endif
	if (len > pWList->iMaxLength) {
		pWList->iMaxLength = len;
	}
}

void WordList_Free(struct WordList *pWList)
{
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
	pWList->bufferCount = 1;
	pWList->buffer = (char *)NP2HeapAlloc(NP2_AUTOC_INIT_BUF_SIZE);
	pWList->bufferList[0] = pWList->buffer;
#endif
	if (bIgnoreCase) {
		pWList->WL_StrCmpA = StrCmpIA;
		pWList->WL_StrCmpNA = StrCmpNIA;
	} else {
		pWList->WL_StrCmpA = StrCmpA;
		pWList->WL_StrCmpNA = StrCmpNA;
	}
#if NP2_AUTOC_USE_NODE_CACHE
	pWList->cacheCapacity = NP2_AUTOC_INIT_CACHE_SIZE;
	pWList->cacheCount = 1;
	pWList->nodeCache = (struct WordNode *)NP2HeapAlloc(pWList->cacheCapacity * sizeof(struct WordNode));
	pWList->nodeCacheList[0] = pWList->nodeCache;
#endif
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
