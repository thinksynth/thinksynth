/* $Id: thBSTree.h,v 1.13 2003/04/26 04:44:02 misha Exp $ */

#ifndef TH_BSTREE_H
#define TH_BSTREE_H 1

class thBSTree {
public:
	thBSTree (int (*fn)(void *, void *));
	thBSTree (int (*fn)(void *, void *), void *id, void *data);
	~thBSTree (void);

	void Insert (void *id, void *data);
	void Insert (thBSTree *node);

	bool Remove (void *id);
	bool Remove (thBSTree *node);

	void *GetData (void *id); /* get data from a specific node id */

	inline void *GetData (void) { /* get data from this node */
		return bsData;
	}

	inline void *GetId (void) { /* get this node's id */
		return bsId;
	}

	inline thBSTree *GetLeft (void) {
		return bsLeft;
	}

	inline thBSTree *GetRight (void) {
		return bsRight;
	}

	inline bool IsEmpty (void) {
		return !bsId;
	}

	inline bool IsLeaf (void) {
		return !bsLeft && !bsRight;
	}


	thBSTree *Find (void *id);
private:
	thBSTree *bsLeft, *bsRight;
	void *bsId, *bsData;

	int (*bsCompare)(void *, void *);
};

/* inline string comparison function */
inline int StringCompare(const char *str1, const char *str2) {
	/* error */
	if(!str1 || !str2) {
		return -2;
	}
	
	while(*str1 && *str2) {
		if(*str1 < *str2) {
			/* str1 is less than str2 */
			return -1;
		}
		else if(*str1 > *str2) {
			/* str1 is greater than str2 */
			return 1;
		}
		
		str1++; str2++;
	}
	
	/* these strings might be equal up to a point, but one might be longer,
	   so we must handle this case */
	int len1, len2;
	
	if((len1 = strlen(str1)) == (len2 = strlen(str2))) {
		/* str1 == str2 */
		return 0;
	}
	else if (len1 < len2) {
		/* str1 < str2 */
		return -1;
	}
	
	/* str1 > str2 */
	return 1;
}


#endif /* TH_BSTREE_H */
