/* $Id: ui.cpp,v 1.9 2004/04/02 11:33:11 misha Exp $ */

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <gtkmm.h>

#include "think.h"

#include "thArg.h"
#include "thPlugin.h"
#include "thPluginManager.h"
#include "thNode.h"
#include "thMod.h"
#include "thMidiNote.h"
#include "thMidiChan.h"
#include "thSynth.h"

#include "Keyboard.h"
#include "KeyboardWindow.h"
#include "PatchSelWindow.h"
#include "MainSynthWindow.h"

extern thSynth Synth;

void ui_thread (void)
{
	MainSynthWindow synthWindow(&Synth);

	Gtk::Main::run(synthWindow);
}
