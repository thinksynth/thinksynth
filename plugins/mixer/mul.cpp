/* $Id: mul.cpp,v 1.12 2004/03/26 09:38:37 joshk Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "think.h"

#include "thArg.h"
#include "thPlugin.h"
#include "thPluginManager.h"
#include "thNode.h"
#include "thMod.h"
#include "thSynth.h"

char		*desc = "Multiplies two streams";
thPluginState	mystate = thPassive;

extern "C" {
  int	module_init (thPlugin *plugin);
  int	module_callback (thNode *node, thMod *mod, unsigned int windowlen);
  void module_cleanup (struct module *mod);
}

void module_cleanup (struct module *mod)
{
	printf("Multiplier plugin unloading\n");
}

int module_init (thPlugin *plugin)
{
	printf("Multiplier plugin loaded\n");

	plugin->SetDesc (desc);
	plugin->SetState (mystate);

	return 0;
}

int module_callback (thNode *node, thMod *mod, unsigned int windowlen)
{
	float *out;
	thArg *in_0, *in_1;
	thArg *out_arg;
	unsigned int i;

	in_0 = mod->GetArg(node, "in0");
	in_1 = mod->GetArg(node, "in1");

	out_arg = mod->GetArg(node, "out");
	out = out_arg->Allocate(windowlen);

	for(i=0;i<windowlen;i++) {
		out[i] = (*in_0)[i]*((*in_1)[i]/TH_MAX);
	}

	return 0;
}

