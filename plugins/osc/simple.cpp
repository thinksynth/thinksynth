/* $Id: simple.cpp,v 1.44 2004/04/08 13:33:30 ink Exp $ */

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

enum {IN_FREQ, IN_AMP, IN_PW, IN_WAVEFORM, IN_FM, IN_FMAMT, IN_RESET, IN_MUL, OUT_ARG, OUT_SYNC, INOUT_LAST};

int args[INOUT_LAST + 1];

char		*desc = "Basic Oscillator";
thPluginState	mystate = thActive;

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

	args[IN_FREQ] = plugin->RegArg("freq");
	args[IN_AMP] = plugin->RegArg("amp");
	args[IN_PW] = plugin->RegArg("pw");
	args[IN_WAVEFORM] = plugin->RegArg("waveform");
	args[IN_FM] = plugin->RegArg("fm");
	args[IN_FMAMT] = plugin->RegArg("fmamt");
	args[IN_RESET] = plugin->RegArg("reset");
	args[IN_MUL] = plugin->RegArg("mul");

	args[OUT_ARG] = plugin->RegArg("out");
	args[OUT_SYNC] = plugin->RegArg("sync");

	args[INOUT_LAST] = plugin->RegArg("last");

	return 0;
}

int module_callback (thNode *node, thMod *mod, unsigned int windowlen)
{
	int i;
	float *out;
	float *out_last, *sync;
	float halfwave, ratio;
	float position, fmpos;
	double wavelength, freq;
	float amp_max, amp_min, amp_range;
	float mul;
	float pw; /* Make pw cooler! */
	float fmamt;
	thArg *in_freq, *in_amp, *in_pw, *in_waveform, *in_fm, *in_fmamt, *in_reset, *in_mul;
	thArg *out_arg, *out_sync;
	thArg *inout_last;

	out_arg = mod->GetArg(node, args[OUT_ARG]);
	out_sync = mod->GetArg(node, args[OUT_SYNC]); /* Output a 1 when the wave begins 
											 its cycle */
	inout_last = mod->GetArg(node, args[INOUT_LAST]);
	position = (*inout_last)[0];
	out_last = inout_last->Allocate(1);
	sync = out_sync->Allocate(windowlen);

	out = out_arg->Allocate(windowlen);

	in_freq = mod->GetArg(node, args[IN_FREQ]);
	in_amp = mod->GetArg(node, args[IN_AMP]);
	in_pw = mod->GetArg(node, args[IN_PW]);
	in_waveform = mod->GetArg(node, args[IN_WAVEFORM]);
	in_fm = mod->GetArg(node, args[IN_FM]); /* FM Input */
	in_fmamt = mod->GetArg(node, args[IN_FMAMT]); /* Modulation amount */
	in_reset = mod->GetArg(node, args[IN_RESET]); /* Reset position to 0 when this 
											  goes to 1 */
	in_mul = mod->GetArg(node, args[IN_MUL]);  /* Multiply the wavelength by this */

	for(i=0; i < (int)windowlen; i++) {
		//wavelength = TH_SAMPLE/(*in_freq)[i];
		freq = (*in_freq)[i];
		amp_max = (*in_amp)[i];
		if(amp_max == 0) {
		  amp_max = TH_MAX;
		}
		amp_min = -amp_max;
		amp_range = amp_max-amp_min;

		wavelength = TH_SAMPLE/freq;

		mul = (*in_mul)[i];
		if(mul) {
			wavelength *= 1/mul;  /* floats are inpresice, if you do freq/2 you
								   get the beating effect.  grr */
		}

		position += 1 + (((*in_fm)[i] / TH_MAX) * (*in_fmamt)[i]);
		//printf("%f\n", position);
		/* worked FM into the position counter */

		if(position > wavelength || (*in_reset)[i] == 1) {
			position -= wavelength;
			sync[i] = 1;
		} else if (position < 0) {
			position += wavelength;
			sync[i] = 1;
		} else {
			sync[i] = 0;
		}

//	fmamt = (*in_fmamt)[i]; /* If FM is being used, apply it! */
//	fmpos = (int)(position + (((*in_fm)[i] / TH_MAX) * wavelength * fmamt)) % (int)wavelength;
		fmpos = position;

		pw = (*in_pw)[i];  /* Pulse Width */
		if(pw == 0) {
			pw = 0.5;
		}

		halfwave = wavelength * pw;
		if(fmpos < halfwave) {
			ratio = fmpos/(2*halfwave);
		} else {
			ratio = ((fmpos - halfwave) / (wavelength - halfwave)) / 2 + 0.5;
		}

		switch((int)(*in_waveform)[i]) {
			/* 0 = sine, 1 = sawtooth, 2 = square, 3 = tri, 4 = half-circle,
			   5 = parabola */
			case 0:    /* SINE WAVE */
				out[i] = amp_max*sin(ratio*2*M_PI); /* This will fuck up if 
													   TH_MIX is not the 
													   negative of TH_MIN */
				break;
			case 1:    /* SAWTOOTH WAVE */
				out[i] = amp_range*ratio+amp_min;
				break;
			case 2:    /* SQUARE WAVE */
				if(ratio < 0.5) {
					out[i] = amp_min;
				} else {
					out[i] = amp_max;
				}
				break;
			case 3:    /* TRIANGLE WAVE */
				ratio *= 2;
				if(ratio < 1) {
					out[i] = amp_range*ratio+amp_min;
				} else {
					out[i] = (-amp_range)*(ratio-1)+amp_max;
				}
				break;
			case 4:    /* HALF-CIRCLE WAVE */
				if(ratio < 0.5) {
					out[i] = 2*sqrt(2)*sqrt((wavelength-(2*position))*position/
											(wavelength*wavelength))*amp_max;
				} else {
					out[i] = 2*sqrt(2)*sqrt((wavelength-(2*(position-halfwave)))*(position-halfwave)/(wavelength*wavelength))*amp_min;
				}
				break;
			case 5:    /* PARABOLA WAVE */
				if(ratio < 0.5) {
					out[i] = amp_max*(1-SQR(ratio*4-1));
				} else {
					out[i] = amp_max*(SQR((ratio-0.5)*4-1)-1);
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
