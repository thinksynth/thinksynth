/* $Id: resonator.cpp,v 1.2 2004/03/26 09:50:33 joshk Exp $ */

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

char		*desc = "Allpass Filter";
thPluginState	mystate = thActive;





void module_cleanup (struct module *mod)
{
	printf("Resonator plugin unloading\n");
}

int module_init (thPlugin *plugin)
{
	printf("Resonator plugin loaded\n");

	plugin->SetDesc (desc);
	plugin->SetState (mystate);

	return 0;
}

int module_callback (thNode *node, thMod *mod, unsigned int windowlen)
{
	float *out;
	thArg *in_arg, *in_freq, *in_fb;
	thArg *out_arg;
	thArg *inout_last;
	unsigned int i;
	int index;
	float *last;
	float in, omega, a0, freq, fb;

	in_arg = mod->GetArg(node, "in");
	in_freq = mod->GetArg(node, "freq");
	in_fb = mod->GetArg(node, "fb"); /* Feedback */

	inout_last = mod->GetArg(node, "last"); /* IIR delay buffer for in and out */
	last = inout_last->Allocate(3);

	out_arg = mod->GetArg(node, "out");
	out = out_arg->Allocate(windowlen);

	for(i = 0; i < windowlen; i++) {
		in = (*in_arg)[i];
		freq = (*in_freq)[i];
		fb = (*in_fb)[i];

		omega = M_PI * freq / TH_SAMPLE;
		a0 = -(1.0 - omega) / (1.0 + omega);

		in -= (last[0] * fb);

		out[i] = a0 * (in - last[0]) + last[1];
		last[2] = last[0];
		last[0] = out[i];
		last[1] = in;

		/* allpass algorithm from krodokov on #musicdsp
		omega = pi * freq / samplerate;
		a0 = -(1.0 - omega) / (1.0 + omega);
		Out[0] = a0 * (In[0] - Out[1]) + In[1];
		Out[1] = Out[0]; In[1] = In[0];
		*/

		//printf("%f %f %f: %f \t%f\n", freq, (*in_fb)[i], a0, in, out[i]);
	}

	return 0;
}

