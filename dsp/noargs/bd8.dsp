# $Id$
name "test";

node ionode {
	out0 = filt->out;
	out1 = filt->out;
	channels = 2;
	play = aenv1->play;

	fmin = 20;
	fmax = 170;
	bandmin = 0;
	bandmax = 200;
	toned = 2000;
	tonemid = 0.25;
	ampd = 5000;
	ampmid = 0.9;
	release = 12000;

	cutoff = 0.1;
	res = 0.92;
	shaper = 10;

	pw = 0.1;

	tonemul = 1.15; # How much higher than the last
	tonemuladd = 0.015; # Linear addition to the multiplication variable
	dmul = 0.8; # How much shorter than the last

	fade1 = 0.35; # Osc 1 and 2
	fade2 = 0.35; # Osc 3 and 4
	fade3 = 0.35; # Osc 4 and 6
	fade4 = 0.35; # Fader 2 and 3
	fade5 = 0.35; # Fader 1 and 4
};

node mixer1 mixer::mul {
	in0 = osc1->out;
	in1 = aenv1->out;
};

node mixer2 mixer::mul {
	in0 = osc2->out;
	in1 = aenv2->out;
};

node mixer3 mixer::mul {
	in0 = osc3->out;
	in1 = aenv3->out;
};

node mixer4 mixer::mul {
	in0 = osc4->out;
	in1 = aenv4->out;
};

node mixer5 mixer::mul {
	in0 = osc5->out;
	in1 = aenv5->out;
};

node mixer6 mixer::mul {
	in0 = osc6->out;
	in1 = aenv6->out;
};

node tonerelease math::add {  # tone envelope should be longer than amp
	in0 = ionode->release;
	in1 = ionode->ampd;
};

node dmul1 math::mul {
	in0 = ionode->ampd;
	in1 = ionode->dmul;
};

node dmul2 math::mul {
	in0 = dmul1->out;
	in1 = ionode->dmul;
};

node dmul3 math::mul {
	in0 = dmul2->out;
	in1 = ionode->dmul;
};

node dmul4 math::mul {
	in0 = dmul3->out;
	in1 = ionode->dmul;
};

node dmul5 math::mul {
	in0 = dmul4->out;
	in1 = ionode->dmul;
};

node tenv env::adsr {
	a = 5;
	d = ionode->toned;
	s = ionode->tonemid;
	r = tonerelease->out;
	trigger = 0;
};

node aenv1 env::adsr {
        a = 0;
        d = ionode->ampd;
        s = ionode->ampmid;
        r = ionode->release;
        trigger = 0;
};

node aenv2 env::adsr {
        a = 0;
        d = dmul1->out;
        s = ionode->ampmid;
        r = ionode->release;
        trigger = 0;
};

node aenv3 env::adsr {
        a = 0;
        d = dmul2->out;
        s = ionode->ampmid;
        r = ionode->release;
        trigger = 0;
};

node aenv4 env::adsr {
        a = 0;
        d = dmul3->out;
        s = ionode->ampmid;
        r = ionode->release;
        trigger = 0;
};

node aenv5 env::adsr {
        a = 0;
        d = dmul4->out;
        s = ionode->ampmid;
        r = ionode->release;
        trigger = 0;
};

node aenv6 env::adsr {
        a = 0;
        d = dmul5->out;
        s = ionode->ampmid;
        r = ionode->release;
        trigger = 0;
};

node map1 env::map {
	in = tenv->out;
	inmin = 0;
	inmax = th_max;
	outmin = ionode->fmin;
	outmax = ionode->fmax;
};

node map2 env::map {
	in = tenv->out;
	inmin = 0;
	inmax = th_max;
	outmin = ionode->bandmin;
	outmax = ionode->bandmax;
};

node tonemulcalc1 math::add {
	in0 = ionode->tonemul;
	in1 = ionode->tonemuladd;
};

node tonemulcalc2 math::add {
	in0 = tonemulcalc1->out;
	in1 = ionode->tonemuladd;
};

node tonemulcalc3 math::add {
	in0 = tonemulcalc2->out;
	in1 = ionode->tonemuladd;
};

node tonemulcalc4 math::add {
	in0 = tonemulcalc3->out;
	in1 = ionode->tonemuladd;
};

node pitchcalc1 math::mul {
	in0 = map1->out;
	in1 = ionode->tonemul;
};

node pitchcalc2 math::mul {
	in0 = pitchcalc1->out;
	in1 = tonemulcalc1->out;
};

node pitchcalc3 math::mul {
	in0 = pitchcalc2->out;
	in1 = tonemulcalc2->out;
};

node pitchcalc4 math::mul {
	in0 = pitchcalc3->out;
	in1 = tonemulcalc3->out;
};

node pitchcalc5 math::mul {
	in0 = pitchcalc4->out;
	in1 = tonemulcalc4->out;
};

node osc1 osc::softsqr {
	freq = map1->out;
	sfreq = map2->out;
	pw = ionode->pw;
};

node osc2 osc::softsqr {
	freq = pitchcalc1->out;
	sfreq = map2->out;
	pw = ionode->pw;
};

node osc3 osc::softsqr {
	freq = pitchcalc2->out;
	sfreq = map2->out;
	pw = ionode->pw;
};

node osc4 osc::softsqr {
	freq = pitchcalc3->out;
	sfreq = map2->out;
	pw = ionode->pw;
};

node osc5 osc::softsqr {
	freq = pitchcalc4->out;
	sfreq = map2->out;
	pw = ionode->pw;
};

node osc6 osc::softsqr {
	freq = pitchcalc5->out;
	sfreq = map2->out;
	pw = ionode->pw;
};

node fade1 mixer::fade {
	in0 = mixer1->out;
	in1 = mixer2->out;
	fade = ionode->fade1;
};

node fade2 mixer::fade {
	in0 = mixer3->out;
	in1 = mixer4->out;
	fade = ionode->fade2;
};

node fade3 mixer::fade {
	in0 = mixer5->out;
	in1 = mixer6->out;
	fade = ionode->fade3;
};

node fade4 mixer::fade {
	in0 = fade2->out;
	in1 = fade3->out;
	fade = ionode->fade4;
};

node fade5 mixer::fade {
	in0 = fade1->out;
	in1 = fade4->out;
	fade = ionode->fade5;
};

node filt filt::inkshape {
	in = fade5->out;
	cutoff = ionode->cutoff;
	res = ionode->res;
	shaper = ionode->shaper;
};

io ionode;
