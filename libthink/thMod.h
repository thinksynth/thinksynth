#ifndef TH_MOD_H
#define TH_MOD_H 1

class thMod {
public:
	thMod(char *name);
	~thMod();

	thNode *FindNode(char *name);
	const thArgValue *GetArg (char *nodename, char *argname);
	void NewNode(thNode *node);
	const char *GetName(void);
	void SetName(char *name);
	void SetIONode(char *name);
	thNode *GetIONode(void);
	void PrintIONode(void);

	void Process (void);

	void SetActiveNodes(void);

private:
	void ProcessHelper (thNode *node);
	void SetActiveNodesHelper(thNode *node);

	thBSTree modnodes;
	thList *activelist;
	thNode *ionode;

	char *modname;
};

#endif /* TH_MOD_H */
