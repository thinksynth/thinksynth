/* $Id: gthALSAAudio.h,v 1.3 2004/08/16 09:34:48 misha Exp $ */
/*
 * Copyright (C) 2004 Metaphonic Labs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General
 * Public License along with this program; if not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef GTH_ALSAAUDIO_H
#define GTH_ALSAAUDIO_H

#include "thException.h"
#include <alsa/asoundlib.h>

//#define ALSA_BUFSIZE 512

#define ALSA_DEFAULT_DEVICE "hw:0"

/* additional arguments are usually bound to the callbacks of this signal */
typedef SigC::Signal0<void> sigReadyWrite_t;

class gthALSAAudio : public gthAudio, public SigC::Object
{
public:
	gthALSAAudio (thSynth *argsynth)
		throw(thIOException);
	gthALSAAudio (thSynth *argsynth, const char *device)
		throw(thIOException);

	virtual ~gthALSAAudio ();

	int Write (float *, int len);
	int Read (void *, int len);
	const gthAudioFmt *GetFormat (void) { return &ofmt; };
	void SetFormat (thSynth *argsynth);
	void SetFormat (const gthAudioFmt *afmt);

	sigReadyWrite_t signal_ready_write (void);

	bool ProcessEvents (void);

	snd_pcm_t *play_handle;
	int nfds;
	struct pollfd *pfds;

	bool pollAudioEvent (Glib::IOCondition);
protected:
	thSynth *synth;
	sigReadyWrite_t m_sigReadyWrite;

	void main (void);
private:
	Glib::Thread *thread;
	gthAudioFmt ofmt, ifmt;
	void *outbuf;
};

#endif /* GTH_ALSAAUDIO_H */
