/* $Id: thPluginManager.cpp,v 1.26 2003/04/29 08:33:53 joshk Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <errno.h>

#include "thBSTree.h"
#include "thPlugin.h"
#include "thPluginManager.h"

thPluginManager::thPluginManager ()
{
	plugins = new thBSTree(StringCompare);
}

thPluginManager::~thPluginManager ()
{
	UnloadPlugins();
}

/* Caller must free!!! */
char *thPluginManager::GetPath (char *name)
{
	char *path = new char[strlen(name) + strlen(PLUGIN_PATH) + 
						  strlen(SHARED_SUFFIX) + 1];
	struct stat *dummy = (struct stat*)malloc (sizeof(struct stat));
	
	/* Use the default path first */
	sprintf(path, "%s%s%s", PLUGIN_PATH, name, SHARED_SUFFIX);
	
	/* Check for existence in the expected place */
	
	if (stat (path, dummy) == -1) { /* File existeth not */
		fprintf (stderr, "thPluginManager: %s: %s/\n", path, strerror(errno));
		
		delete path;
		path = new char[strlen("plugins/") + strlen(name) + 
					strlen(SHARED_SUFFIX) + 1];
		
		sprintf (path, "plugins/%s%s", name, SHARED_SUFFIX);
		if(stat(path, dummy) == -1) {
			fprintf(stderr, "thPluginManager: %s: %s\n", path, strerror(errno));
			delete path;
			return NULL;
		}
	}

	free (dummy);
	return path;
}

/* TODO: Return values.. should we care about them? consider making void? */
int thPluginManager::LoadPlugin (char *name)
{
	thPlugin *plugin;
	char *path;

	path = GetPath(name);

	if (path == NULL) { /* Not found at all */
		fprintf (stderr, "Could not find the plugin anywhere!\n");
		return 1;
	}
	
	plugin = new thPlugin (path);
	delete[] path;

	if (plugin->GetState() == thNotLoaded) {	/* something messed up */
		delete plugin;
		return 1;
	}
	
	plugins->Insert((void *)name, (void *)plugin);

	return 0;
}


void thPluginManager::UnloadPlugin(char *name)
{
	thPlugin *plugin = (thPlugin *)plugins->GetData((void *)name);

	if(!plugin) {
		fprintf(stderr, "thPluginManager::UnloadPlugin: No such plugin '%s'\n", name);
		return;
	}

	plugins->Remove((void *)name);

	delete plugin;
}

thPlugin *thPluginManager::GetPlugin (char *name)
{
	thPlugin *plugin = (thPlugin *)plugins->GetData((void *)name);
	
	return plugin;
}

void thPluginManager::UnloadPlugins (void)
{
	/* XXX: modify thBSTree to take an option to delete data;
	   by freeing each plugin it will unload each */
/*	thListNode *node;

	for(node = plugins->GetHead(); node; node = node->prev) {
		thPlugin *plugin = (thPlugin *)node->data;

		delete plugin;
		} */
}
