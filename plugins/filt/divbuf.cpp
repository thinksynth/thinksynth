/* $Id: divbuf.cpp,v 1.4 2003/09/16 01:02:28 misha Exp $ */

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

#define SQR(a) (a*a)

char		*desc = "Cheap IIR-ish Filter";
thPluginState	mystate = thActive;

extern "C" int	module_init (thPlugin *plugin);
extern "C" int	module_callback (thNode *node, thMod *mod, unsigned int windowlen);
extern "C" void module_cleanup (struct module *mod);

void module_cleanup (struct module *mod)
{
	printf("Divbuf plugin unloading\n");
}

int module_init (thPlugin *plugin)
{
	printf("Divbuf plugin loaded\n");

	plugin->SetDesc (desc);
	plugin->SetState (mystate);

	return 0;
}

int module_callback (thNode *node, thMod *mod, unsigned int windowlen)
{
	float *out;
	float *out_buf;
	thArg *in_arg, *in_factor;
	thArg *out_arg;
	thArg *inout_buffer;
	
	unsigned int i;
	float factor, buffer;

	out_arg = mod->GetArg(node, "out");
	inout_buffer = mod->GetArg(node, "buffer");
	buffer = (*inout_buffer)[0];
	out = out_arg->Allocate(windowlen);
	out_buf = inout_buffer->Allocate(1);

	in_arg = mod->GetArg(node, "in");
	in_factor = mod->GetArg(node, "factor");

	for(i=0;i<windowlen;i++) {
	  factor = SQR(SQR((*in_factor)[i]));

	  buffer = ((*in_arg)[i] * factor) + (buffer * (1-factor));

	  out[i] = buffer;
	}

	out_buf[0] = buffer;

/*	node->SetArg("out", out, windowlen);
	node->SetArg("buffer", out_buffer, 1); */

	return 0;
}

