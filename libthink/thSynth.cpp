/* $Id: thSynth.cpp,v 1.58 2003/05/11 09:05:29 aaronl Exp $ */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "thArg.h"
#include "thList.h"
#include "thBSTree.h"
#include "thPlugin.h"
#include "thPluginManager.h"
#include "thNode.h"
#include "thMod.h"
#include "thMidiNote.h"
#include "thMidiChan.h"
#include "thSynth.h"

#include "parser.h"

thSynth::thSynth (void)
{
	thWindowlen = 1024;
	thChans = 2;  /* mono / stereo / etc */

	modlist = new thBSTree(StringCompare);
	channels = new thBSTree(StringCompare);

	thOutput = new float[thChans*thWindowlen];  /* We should make a function to
												   allocate this, so we can 
												   easily change thChans and
												   thWindowlen */
}

thSynth::~thSynth (void)
{
	delete modlist;
	delete channels;
	delete [] thOutput;
}

void thSynth::LoadMod(const char *filename)
{
	if ((yyin = fopen(filename, "r")) == NULL) { /* ENOENT or smth */
		fprintf (stderr, "couldn't open %s: %s\n", filename, strerror(errno));
		exit(1);
	}

	parsemod = new thMod("newmod");     /* these are used by the parser */
	parsenode = new thNode("newnode", NULL);
	
	yyparse();

	fclose(yyin);

	delete parsenode;

	parsemod->BuildSynthTree();
	modlist->Insert((void *)strdup(parsemod->GetName()), (void *)parsemod);
}

thMod *thSynth::FindMod(const char *modname)
{
	return (thMod *)modlist->GetData((void *)modname);
}

/* Make these voids return something and add error checking everywhere! */
void thSynth::ListMods(void)
{
	ListMods(modlist);
}

void thSynth::ListMods(thBSTree *node)
{
	if(!node) {
		return;
	}

	ListMods(node->GetLeft());
	printf("%s\n", (char *)node->GetId());
	ListMods(node->GetRight());
}

thPluginManager *thSynth::GetPluginManager(void)
{
	return &pluginmanager;
}

void thSynth::AddChannel(char *channame, char *modname, float amp)
{
	thMidiChan *newchan = new thMidiChan(FindMod(modname), amp, thWindowlen);
	channels->Insert((void *)channame, (void *)newchan);
}

thMidiNote *thSynth::AddNote(char *channame, float note, float velocity)
{
	thMidiChan *chan = (thMidiChan *)channels->GetData((void *)channame);
	thMidiNote *newnote = chan->AddNote(note, velocity);

	return newnote;
}

void thSynth::Process()
{
	memset(thOutput, 0, thChans*thWindowlen*sizeof(float));
	ProcessHelper(channels);
}

void thSynth::ProcessHelper(thBSTree *node)
{
	int i, j, mixchannels, notechannels;
	thMidiChan *chan;
	float *chanoutput;

	if(!node) {
		return;
	}

	ProcessHelper(node->GetLeft());

	chan = (thMidiChan *)node->GetData();

	notechannels = chan->GetChannels();
	mixchannels = notechannels;

	if(mixchannels > thChans) {
		mixchannels = thChans;
	}

	chan->Process();
	chanoutput = chan->GetOutput();

	for(i = 0; i < mixchannels; i++) {
		for(j = 0 ;j < thWindowlen; j++) {
			thOutput[i+(j*thChans)] += chanoutput[i+(j*notechannels)];
		}
	}

	ProcessHelper(node->GetRight());
}

void thSynth::PrintChan(int chan)
{
	int i;

	for(i = 0; i < thWindowlen; i++) {
		printf("-=- %f\n", thOutput[(i*thChans)+chan]);
	}
}
