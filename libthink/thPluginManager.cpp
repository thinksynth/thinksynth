/* $Id: thPluginManager.cpp,v 1.47 2004/08/16 09:34:48 misha Exp $ */
/*
 * Copyright (C) 2004 Metaphonic Labs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General
 * Public License along with this program; if not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "config.h"

#include <stdio.h>
#include <unistd.h>

#ifdef HAVE_DLFCN_H
# include <dlfcn.h>
#else
# ifdef USING_DARWIN
#  include "nsmodule_dl.h"
# else
#  error Need a dl implementation!
# endif
#endif

#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "think.h"

thPluginManager::thPluginManager (const string &path)
{
	plugin_path = path;
}

thPluginManager::~thPluginManager ()
{
	UnloadPlugins();
}

const string thPluginManager::GetPath (const string &name)
{
	string path;
	struct stat dummy;

	/* Use the default path first */
	path = plugin_path + name + SHARED_SUFFIX;

	/* Check for existence in the expected place */
	if (stat (path.c_str(), &dummy) == -1) { /* File existeth not */
#ifdef USE_DEBUG
		fprintf (stderr, "thPluginManager: %s: %s\n", path.c_str(), strerror(errno));
#endif
		path = "plugins/" + name + SHARED_SUFFIX;
		if(stat(path.c_str(), &dummy) == -1) {
#ifdef USE_DEBUG
			fprintf(stderr, "thPluginManager: %s: %s\n", path.c_str(), strerror(errno));
#endif
			return ""; /* Empty string */ 
		}
	}

	return path;
}

int thPluginManager::LoadPlugin (const string &name)
{
	thPlugin *plugin;
	const string path = GetPath(name);

	if (path.empty()) { /* Not found at all */
		fprintf (stderr, "Could not find the plugin anywhere!\n");
		return 1;
	}

	plugin = new thPlugin (path);

	if (plugin->GetState() == thNotLoaded) {	/* something messed up */
		delete plugin;
		return 1;
	}
	
	plugins[name] = plugin;

	return 0;
}


void thPluginManager::UnloadPlugin(const string &name)
{
	map<string, thPlugin*>::iterator i = plugins.find(name);

	if(i == plugins.end()) {
		fprintf(stderr, "thPluginManager::UnloadPlugin: No such plugin '%s'\n",
				name.c_str());
		return;
	}

	thPlugin *plugin = i->second;

	plugins.erase(i);

	delete plugin;
}

void thPluginManager::UnloadPlugins (void)
{
	DestroyMap(plugins);
}
