/* $Id: PatchSelWindow.h,v 1.12 2004/08/07 18:54:49 misha Exp $ */

#ifndef PATCHSEL_WINDOW_H
#define PATCHSEL_WINDOW_H

class PatchSelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
	PatchSelColumns (void)
	{
		add (chanNum);
		add (dspName);
		add (amp);
	}

	Gtk::TreeModelColumn <unsigned int> chanNum;
	Gtk::TreeModelColumn <Glib::ustring> dspName;
	Gtk::TreeModelColumn <float> amp;
};

class PatchSelWindow : public Gtk::Window
{
public:
	PatchSelWindow (thSynth *);
	~PatchSelWindow (void);

	/* signals */
	

protected:
	bool LoadPatch (void);
	void SetChannelAmp (void);
	void BrowsePatch (void);
	void CursorChanged (void);

	void patchSelected (GdkEventButton *);
	void fileEntryActivate (void);

	Gtk::VBox vbox;
	Gtk::Table controlTable;

	Gtk::HScale dspAmp;
	Gtk::Button setButton;
	Gtk::Button browseButton;
	Gtk::Label ampLabel;

	Gtk::Label fileLabel;
	Gtk::Entry fileEntry;

	Gtk::ScrolledWindow patchScroll;
	Gtk::TreeView patchView;
	Glib::RefPtr<Gtk::ListStore> patchModel;
	PatchSelColumns patchViewCols;

private:
	thSynth *synth;
	char *prevDir;
};

#endif /* PATCHSEL_WINDOW_H */
