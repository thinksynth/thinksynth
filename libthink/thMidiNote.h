/* $Id: thMidiNote.h,v 1.14 2003/05/08 00:53:33 ink Exp $ */

#ifndef TH_MIDINOTE_H
#define TH_MIDINOTE_H 1

class thMidiNote {
public:
	thMidiNote(thMod *mod, float note, float velocity);
	thMidiNote(thMod *mod);
	~thMidiNote();
	
	/* takes the arg name, and a pointer to a list of values */
	//	void SetArg (const char *name, float *value, int num);
	
	/* returns a pointer to a list of values */
	//thArgValue *GetArg(const char *name);
	
	thMod *GetMod(void) { return modnode; }
	int GetID(void) { return noteid; }

	void Process (int length);
	
private:
	//		thBSTree *args;
	thMod *modnode;
	thNode *ionode;
	int noteid;
};

#endif /* TH_MIDINOTE_H */
