/* $Id: res1pole.cpp,v 1.2 2003/05/30 00:55:41 aaronl Exp $ */

/* Written by Leif Ames <ink@bespni.org>
   Algorithm taken from musicdsp.org posted by Paul Kellett */

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

char		*desc = "Resonant 1-pole LPF";
thPluginState	mystate = thActive;

extern "C" int	module_init (thPlugin *plugin);
extern "C" int	module_callback (thNode *node, thMod *mod, unsigned int windowlen);
extern "C" void module_cleanup (struct module *mod);

void module_cleanup (struct module *mod)
{
	printf("ResLPF plugin unloading\n");
}

int module_init (thPlugin *plugin)
{
	printf("ResLPF plugin loaded\n");

	plugin->SetDesc (desc);
	plugin->SetState (mystate);

	return 0;
}

int module_callback (thNode *node, thMod *mod, unsigned int windowlen)
{
	float *out, *buffer;
	thArg *in_arg, *in_cutoff, *in_res;
	thArg *out_arg;
	thArg *inout_buffer;
	float buf0, buf1;
	float fb, f, q;  /* feedback, cutoff, resonance */
	unsigned int i;

	out_arg = mod->GetArg(node, "out");
	out = out_arg->allocate(windowlen);

	inout_buffer = mod->GetArg(node, "buffer");
	buf0 = (*inout_buffer)[0];
	buf1 = (*inout_buffer)[1];
	buffer = inout_buffer->allocate(2);

	in_arg = mod->GetArg(node, "in");
	in_cutoff = mod->GetArg(node, "cutoff");
	in_res = mod->GetArg(node, "res");

	for(i=0;i<windowlen;i++) {
		f = (*in_cutoff)[i];
		q = (*in_res)[i];
		fb = q + q/(1.0 - f);

		buf0 = buf0 + f * ((*in_arg)[i] - buf0 + fb * (buf0 - buf1));
		buf1 = buf1 + f * (buf0 - buf1);
		out[i] = buf1;
	}

	buffer[0] = buf0;
	buffer[1] = buf1;
	return 0;
}

