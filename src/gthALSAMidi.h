/* $Id: gthALSAMidi.h,v 1.7 2004/09/19 08:43:38 joshk Exp $ */
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

#ifndef GTH_ALSAMIDI_H
#define GTH_ALSAMIDI_H

#define ALSA_DEFAULT_MIDI_DEVICE "default"

typedef sigc::signal1<int, snd_seq_t *> sigMidiEvent_t;

class gthALSAMidi
{
public:
	gthALSAMidi (const char *argname)
		throw(thIOException);
	~gthALSAMidi (void);

	sigMidiEvent_t signal_midi_event (void);

	bool ProcessEvents (void);
	bool pollMidiEvent (Glib::IOCondition);
	inline bool seq_opened(void) const { return _seq_opened; }

protected:
	string name, device;

	bool open_seq (void);
	bool _seq_opened;

	snd_seq_t *seq_handle;
	int seq_nfds;
	struct pollfd *pfds;

	sigMidiEvent_t m_sigMidiEvent;
};

#endif /* GTH_ALSAMIDI_H */
