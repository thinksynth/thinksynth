#ifndef TH_PLUGIN_H
#define TH_PLUGIN_H 1

#define MODULE_IFACE_VER 3

/* XXX */
#define PLUGPREFIX "plugins/"
#define PLUGPOSTFIX ".so"

#define NUM_SIGNALS 128

class thPlugin {
public:
	thPlugin(const char *name, int id, bool state);
	~thPlugin ();

	const char *GetName (void);
	const char *GetDesc (void);
	bool GetState (void);

	void MakePath (void);

	void SetDesc(const char *desc);
	void SetState(bool state);

	int Fire (void);
private:
	char *plugName;
	int plugId;
	bool plugState;
	void *plugHandle;
	char *plugDesc;
	char *plugPath;

	int ModuleLoad (void);
	void ModuleUnload (void);
};

#endif /* TH_PLUGIN_H */
