/* $Id: thMidiNote.cpp,v 1.23 2003/05/08 00:53:33 ink Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thList.h"
#include "thBSTree.h"
#include "thArg.h"
#include "thPlugin.h"
#include "thNode.h"
#include "thMod.h"
#include "thMidiNote.h"

thMidiNote::thMidiNote (thMod *mod, float note, float velocity)
{
	float *notep = new float, *velocityp = new float;

	//	args = new thBSTree(StringCompare);

	modnode = mod->Copy();
	modnode->BuildSynthTree();
	ionode = modnode->GetIONode();

	*notep = note, *velocityp = velocity;

	ionode->SetArg("note", notep, 1);
	ionode->SetArg("velocity", velocityp, 1);

	noteid = (int)note;
}

thMidiNote::thMidiNote (thMod *mod)
{
  //	args = new thBSTree(StringCompare);

	modnode = mod->Copy();
	modnode->BuildSynthTree();
	ionode = modnode->GetIONode();
}

thMidiNote::~thMidiNote ()
{
  	delete modnode;
}

void thMidiNote::Process (int length)
{
  modnode->SetActiveNodes();
  modnode->Process(length);
}
