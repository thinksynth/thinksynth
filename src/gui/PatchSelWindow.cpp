#include "config.h"
#include "think.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtkmm.h>

#include "thArg.h"
#include "thPlugin.h"
#include "thPluginManager.h"
#include "thNode.h"
#include "thMod.h"
#include "thMidiNote.h"
#include "thMidiChan.h"
#include "thSynth.h"

#include "PatchSelWindow.h"

extern Glib::Mutex *synthMutex;

PatchSelWindow::PatchSelWindow (thSynth *synth)
{
//		set_default_size(320, 240);
	set_default_size(600, 0);

	realSynth = synth;
	mySynth = NULL;
//	mySynth = new thSynth(realSynth);

	set_title("thinksynth - Patch Selector");
	
	add(vbox);
	
	vbox.pack_start(patchTable, Gtk::PACK_SHRINK);

	synthMutex->lock();
		
	std::map<int, string> *patchlist = realSynth->GetPatchlist();
	int channelcount = realSynth->GetChannelCount();

	synthMutex->unlock();

	for(int i = 0; i < channelcount; i++)
	{
		char label[11];

//		thMidiChan *chan = synth->GetChannel(i);
//		thMod *mod = chan->GetMod();

		sprintf(label, "Channel %d", i+1);
		Gtk::Label *chanLabel = new Gtk::Label(label);
		string filename = (*patchlist)[i];
		Gtk::Entry *chanEntry = new Gtk::Entry;
		chanEntry->set_text(Glib::ustring (filename.c_str()));

		int *channum = new int;
		*channum = i;

		Gtk::HScale *chanAmp = new Gtk::HScale(0, 20, .5);
		chanAmp->set_data("channel", channum);
		chanAmp->signal_value_changed().connect(
			SigC::bind<Gtk::HScale *, thSynth *> (SigC::slot(*this, &PatchSelWindow::SetChannelAmp), chanAmp, realSynth));

		patchTable.attach(*chanLabel, 0, 1, i, i+1);
		patchTable.attach(*chanEntry, 1, 2, i, i+1);
		patchTable.attach(*chanAmp, 2, 3, i, i+1);


		chanEntry->set_data("channel", channum);
		chanEntry->signal_activate().connect(
			SigC::bind<Gtk::Entry *, thSynth *> (SigC::slot(*this, &PatchSelWindow::LoadPatch), chanEntry, realSynth) );
	}

	show_all_children();
}

void PatchSelWindow::LoadPatch (Gtk::Entry *chanEntry, thSynth *synth)
{
	int *channum = (int *)chanEntry->get_data("channel");

	synthMutex->lock();

	synth->LoadMod(chanEntry->get_text().c_str(), *channum, (float)12.0);

	synthMutex->unlock();
}

void PatchSelWindow::SetChannelAmp (Gtk::HScale *scale, thSynth *synth)
{
	int *channum = (int *)scale->get_data("channel");
	float *val = new float;
	*val = (float)scale->get_value();
	thArg *arg = new thArg("amp", val, 1);

	synthMutex->lock();

	synth->SetChanArg(*channum, arg);

	synthMutex->unlock();
}
