/* $Id: simple.cpp,v 1.29 2003/05/30 00:55:41 aaronl Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "think.h"

#include "thArg.h"
#include "thPlugin.h"
#include "thPluginManager.h"
#include "thNode.h"
#include "thMod.h"
#include "thSynth.h"

#define SQR(x) ((x)*(x))

char		*desc = "Basic Oscillator";
thPluginState	mystate = thActive;

extern "C" int	module_init (thPlugin *plugin);
extern "C" int	module_callback (thNode *node, thMod *mod, unsigned int windowlen);
extern "C" void module_cleanup (struct module *mod);

void module_cleanup (struct module *mod)
{
	printf("Oscillator module unloading\n");
}

/* ModuleLoad() invokes this function with a pointer to the plugin
 * instance. */
int module_init (thPlugin *plugin)
{
	printf("Simple Oscillator plugin loaded\n");
	plugin->SetDesc (desc);
	plugin->SetState (mystate);

	return 0;
}

int module_callback (thNode *node, thMod *mod, unsigned int windowlen)
{
	int i;
	float *out;
	float *out_last, *sync;
	float halfwave, ratio;
	float position, wavelength;
	float pw; /* Make pw cooler! */
	float fmamt;
	thArg *in_freq, *in_pw, *in_waveform, *in_fm, *in_fmamt, *in_reset;
	thArg *out_arg, *out_sync;
	thArg *inout_last;

	out_arg = mod->GetArg(node, "out");
	out_sync = mod->GetArg(node, "sync"); /* Output a 1 when the wave begins its cycle */
	inout_last = mod->GetArg(node, "last");
	position = (*inout_last)[0];
	out_last = inout_last->allocate(1);
	sync = out_sync->allocate(windowlen);

	out = out_arg->allocate(windowlen);

	in_freq = mod->GetArg(node, "freq");
	in_pw = mod->GetArg(node, "pw");
	in_waveform = mod->GetArg(node, "waveform");
	in_fm = mod->GetArg(node, "fm"); /* FM Input */
	in_fmamt = mod->GetArg(node, "fmamt"); /* Modulation amount */
	in_reset = mod->GetArg(node, "reset"); /* Reset position to 0 when this goes to 1 */

	for(i=0; i < (int)windowlen; i++) {
		wavelength = TH_SAMPLE/(*in_freq)[i];
		position++;

		fmamt = (*in_fmamt)[i]; /* If FM is being used, apply it! */
		if(fmamt) {
			wavelength += (wavelength * fmamt) * ((*in_fm)[i] / TH_MAX);
		}
		if(position > wavelength || (*in_reset)[i] == 1) {
			position = 0;
			sync[i] = 1;
		} else {
			sync[i] = 0;
		}

		pw = (*in_pw)[i];  /* Pulse Width */
		if(pw == 0) {
			pw = 0.5;
		}

		halfwave = wavelength * pw;
		if(position < halfwave) {
			ratio = position/(2*halfwave);
		} else {
			ratio = (((position-halfwave)/(wavelength-halfwave))/2)+0.5;
		}
		
		switch((int)(*in_waveform)[i]) {
			/* 0 = sine, 1 = sawtooth, 2 = square, 3 = tri, 4 = half-circle, 5 = parabola */
			case 0:    /* SINE WAVE */
				out[i] = TH_MAX*sin(ratio*2*M_PI); /* This will fuck up if TH_MIX is not the negative of TH_MIN */
				break;
			case 1:    /* SAWTOOTH WAVE */
				out[i] = TH_RANGE*ratio+TH_MIN;
				break;
			case 2:    /* SQUARE WAVE */
				if(ratio < 0.5) {
					out[i] = TH_MIN;
				} else {
					out[i] = TH_MAX;
				}
				break;
			case 3:    /* TRIANGLE WAVE */
				ratio *= 2;
				if(ratio < 1) {
					out[i] = TH_RANGE*ratio+TH_MIN;
				} else {
					out[i] = (-TH_RANGE)*(ratio-1)+TH_MAX;
				}
				break;
			case 4:    /* HALF-CIRCLE WAVE */
				if(ratio < 0.5) {
					out[i] = 2*sqrt(2)*sqrt((wavelength-(2*position))*position/(wavelength*wavelength))*TH_MAX;
				} else {
					out[i] = 2*sqrt(2)*sqrt((wavelength-(2*(position-halfwave)))*(position-halfwave)/(wavelength*wavelength))*TH_MIN;
				}
				break;
			case 5:    /* PARABOLA WAVE */
				if(ratio < 0.5) {
					out[i] = TH_MAX*(1-SQR(ratio*4-1));
				} else {
					out[i] = TH_MAX*(SQR((ratio-0.5)*4-1)-1)       ;
				}	
		}
	}
	
	out_last[0] = position;
/*	node->SetArg("out", out, windowlen);
	last[0] = position;
	node->SetArg("last", (float*)last, 1);
*/	
	return 0;
}
