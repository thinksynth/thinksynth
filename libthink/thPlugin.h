/* $Id: thPlugin.h,v 1.14 2003/04/27 07:43:01 joshk Exp $ */

#ifndef TH_PLUGIN_H
#define TH_PLUGIN_H 1

#define MODULE_IFACE_VER 3

enum thPluginState { thActive, thPassive, thNotLoaded };

class thNode;
class thMod;

class thPlugin {
	public:
		thPlugin(const char *path);
		~thPlugin ();

		inline char *GetPath (void) const { return plugPath; };
		inline char *GetDesc (void) const { return plugDesc; };
		thPluginState GetState (void) const { return plugState; };

		void MakePath (void);

		void SetDesc(const char *desc);
		void SetState(thPluginState state) { plugState = state; };

		int Fire (thNode *node, thMod *mod, unsigned int windowlen);

	private:
		char *plugPath;
		thPluginState plugState;
		void *plugHandle;
		char *plugDesc;

		void (*plugCallback)(thNode *, thMod *, unsigned int);

		int ModuleLoad (void);
		void ModuleUnload (void);
};

#endif /* TH_PLUGIN_H */
