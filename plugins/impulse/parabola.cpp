/* $Id: parabola.cpp,v 1.3 2003/05/24 02:13:28 ink Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "think.h"

#include "thArg.h"
#include "thList.h"
#include "thBSTree.h"
#include "thPlugin.h"
#include "thPluginManager.h"
#include "thNode.h"
#include "thMod.h"
#include "thSynth.h"

#define SQR(x) ((x)*(x))

char		*desc = "Generates a small parabola";
thPluginState	mystate = thPassive;

extern "C" int	module_init (thPlugin *plugin);
extern "C" int	module_callback (thNode *node, thMod *mod, unsigned int windowlen);
extern "C" void module_cleanup (struct module *mod);

void module_cleanup (struct module *mod)
{
	printf("Parabola plugin unloading\n");
}

int module_init (thPlugin *plugin)
{
	printf("Parabola plugin loaded\n");

	plugin->SetDesc (desc);
	plugin->SetState (mystate);

	return 0;
}

int module_callback (thNode *node, thMod *mod, unsigned int windowlen)
{
	float *out;
	thArgValue *in_len, *in_max, *in_percent;
	thArgValue *out_arg;
	float max, percent, parabolalen, offset, half;
	unsigned int i, len;

	in_len = (thArgValue *)mod->GetArg(node, "len"); /* Length of output */
	in_max = (thArgValue *)mod->GetArg(node, "max"); /* Parabola peak */
	in_percent = (thArgValue *)mod->GetArg(node, "percent"); /* How much of the output is actually parabola */
	len = (int)(*in_len)[0];
	max = (*in_max)[0];
	percent = (*in_percent)[0];
	half = len/2;

	out_arg = (thArgValue *)mod->GetArg(node, "out");
	out = out_arg->allocate(len);

	if(percent == 0) {
		percent = 1;
	}
	parabolalen = percent * len;
	offset = (len-parabolalen)/2;
	for(i=0;i<offset;i++) {
		out[i] = 0;
	}
	for(;i<(parabolalen/2)+offset;i++) {
		out[i] = max*(1-SQR((i/((float)len-offset))*2-1));
	}
	for(;i<len;i++) {
		out[i] = out[(int)(half-(i-half))];
	}

	return 0;
}

