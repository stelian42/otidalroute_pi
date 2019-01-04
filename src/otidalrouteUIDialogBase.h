///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/listbox.h>

///////////////////////////////////////////////////////////////////////////

class ConfigurationDialog;
class otidalroute_pi;

///////////////////////////////////////////////////////////////////////////////
/// Class otidalrouteUIDialogBase
///////////////////////////////////////////////////////////////////////////////
class otidalrouteUIDialogBase : public wxFrame
{
private:

protected:
	wxMenuBar* m_menubar3;
	wxMenu* m_menu3;
	wxMenu* m_mHelp;
	wxStaticText* m_staticText3;
	wxStaticText* m_staticText41;
	wxStaticText* m_staticText5;
	wxStaticText* m_staticText4;
	wxCheckBox* m_cbGPX;
	wxStaticText* m_staticText2;
	wxButton* m_bCalcDR;
	wxButton* m_bCalcETA;

	// Virtual event handlers, overide them in your derived class
	virtual void OnClose(wxCloseEvent& event) { event.Skip(); }
	virtual void OnSize(wxSizeEvent& event) { event.Skip(); }
	virtual void OnSummary(wxCommandEvent& event) { event.Skip(); }
	virtual void OnShowTables(wxCommandEvent& event) { event.Skip(); }
	virtual void OnDeleteAllRoutes(wxCommandEvent& event) { event.Skip(); }
	virtual void OnInformation(wxCommandEvent& event) { event.Skip(); }
	virtual void OnAbout(wxCommandEvent& event) { event.Skip(); }
	virtual void DRCalculate(wxCommandEvent& event) { event.Skip(); }
	virtual void ETACalculate(wxCommandEvent& event) { event.Skip(); }


public:
	wxTextCtrl* m_tSpeed;
	wxTextCtrl* m_textCtrl1;
	wxTextCtrl* m_tRouteName;
	wxChoice* m_choiceDepartureTimes;

	otidalrouteUIDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(236, 425), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	~otidalrouteUIDialogBase();

};



///////////////////////////////////////////////////////////////////////////////
/// Class ConfigurationDialog
///////////////////////////////////////////////////////////////////////////////
class ConfigurationDialog : public wxDialog
{
private:
	
	

protected:

	wxButton* m_bDelete;
	wxButton* m_bSelect;
	wxButton* m_bGenerate;
	wxButton* m_bClose;

	// Virtual event handlers, overide them in your derived class
	void OnDelete(wxCommandEvent& event);
	void OnInformation(wxCommandEvent& event);
	void OnGenerate(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);

public:
	otidalroute_pi* pPlugIn;
	wxListBox* m_lRoutes;

	ConfigurationDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Tidal Routes"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	~ConfigurationDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class AboutDialogBase
///////////////////////////////////////////////////////////////////////////////
class AboutDialogBase : public wxDialog
{
private:

protected:
	wxStaticText* m_staticText135;
	wxStaticText* m_stVersion;
	wxStaticText* m_staticText110;
	wxButton* m_bAboutAuthor;
	wxButton* m_bClose;

	// Virtual event handlers, overide them in your derived class
	virtual void OnAboutAuthor(wxCommandEvent& event) { event.Skip(); }
	virtual void OnClose(wxCommandEvent& event) { event.Skip(); }


public:

	AboutDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("About Weather Routing"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	~AboutDialogBase();

};

