/* $Id: pitch.cpp,v 1.2 2004/03/26 09:38:37 joshk Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "think.h"

#include "thArg.h"
#include "thPlugin.h"
#include "thPluginManager.h"
#include "thNode.h"
#include "thMod.h"
#include "thSynth.h"

char		*desc = "Follows the pitch of the input";
thPluginState	mystate = thActive;

extern "C" {
  int	module_init (thPlugin *plugin);
  int	module_callback (thNode *node, thMod *mod, unsigned int windowlen);
  void module_cleanup (struct module *mod);
}

void module_cleanup (struct module *mod)
{
	printf("Avg Pitch Follower plugin unloading\n");
}

int module_init (thPlugin *plugin)
{
	printf("Avg Pitch Follower plugin loaded\n");

	plugin->SetDesc (desc);
	plugin->SetState (mystate);

	return 0;
}

int module_callback (thNode *node, thMod *mod, unsigned int windowlen)
{
	float *out, *out_last;
	thArg *in_arg, *in_falloff;
	thArg *out_arg;
	thArg *inout_last;
	unsigned int i;
	float input, last, falloff;
	float freq;
	int sign, wavelength;

	out_arg = mod->GetArg(node, "out");
	inout_last = mod->GetArg(node, "last");

	last = (*inout_last)[0];
	freq = (*inout_last)[1];
	wavelength = (int)(*inout_last)[2];

	out_last = inout_last->Allocate(3);

	out = out_arg->Allocate(windowlen);

	in_arg = mod->GetArg(node, "in");
	in_falloff = mod->GetArg(node, "falloff");


	for(i = 0; i < windowlen; i++)
	{
		falloff = pow(0.1, (*in_falloff)[i]);

		input = (*in_arg)[i];

		if(last > 0)
			sign = 1;
		else
			sign = 0;
		
		if(sign == 0 && input > 0) /* trigger on the rising edge */
		{
			freq = TH_SAMPLE / wavelength;
			wavelength = 0;
		}
		else
		{
			wavelength++;
		}

		out[i] = freq;

		last = input;
	}

	out_last[0] = last;
	out_last[1] = freq;
	out_last[2] = wavelength;
	

	return 0;
}

