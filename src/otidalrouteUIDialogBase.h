///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __otidalrouteUIDIALOGBASE_H__
#define __otidalrouteUIDIALOGBASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <wx/clrpicker.h>
#include <wx/choice.h>
#include <wx/colordlg.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/splitter.h>
#include <wx/listbox.h>

class ConfigurationDialog;
class otidalroute_pi;
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class oTidalRoute
///////////////////////////////////////////////////////////////////////////////
class otidalrouteUIDialogBase : public wxFrame
{
private:

protected:
	
	wxSplitterWindow* m_splitter1;
	wxPanel* m_panel1;
	wxListCtrl* m_lPositions;
	wxPanel* m_panel2;
	wxButton* m_bHourlyCTS;
	wxButton* m_bSingleCTS;
	wxTextCtrl* m_tSpeed;
	wxMenuBar* m_menubar3;
	wxMenu* m_menu2;
	wxMenu* m_menu3;
	wxMenu* m_menu4;
	wxMenu* m_mHelp;
	wxStaticText* m_staticText2;
	
	wxStaticText* m_staticText3;
	wxStaticText* m_staticText4;
	

	// Virtual event handlers, overide them in your derived class
	virtual void OnClose(wxCloseEvent& event) { event.Skip(); }
	virtual void OnSize(wxSizeEvent& event) { event.Skip(); }
	virtual void HourlyCTS(wxCommandEvent& event) { event.Skip(); }
	virtual void SingleCTS(wxCommandEvent& event) { event.Skip(); }
	virtual void OnNewPosition(wxCommandEvent& event) { event.Skip(); }
	virtual void OnUpdateBoat(wxCommandEvent& event) { event.Skip(); }
	virtual void OnDeletePosition(wxCommandEvent& event) { event.Skip(); }
	virtual void OnDeleteAllPositions(wxCommandEvent& event) { event.Skip(); }

	virtual void OnSummary(wxCommandEvent& event) { event.Skip(); }
	virtual void OnNewRoute(wxCommandEvent& event) { event.Skip(); }
	virtual void OnDeleteRoute(wxCommandEvent& event) { event.Skip(); }
	virtual void OnDeleteAllRoutes(wxCommandEvent& event) { event.Skip(); }
	virtual void OnInformation(wxCommandEvent& event) { event.Skip(); }
	virtual void OnAbout(wxCommandEvent& event) { event.Skip(); }

public:
	wxTextCtrl* m_textCtrl1;
	wxTextCtrl* m_textCtrl3;
	wxTextCtrl* m_textCtrl4;

	wxTextCtrl* m_tRouteName;

	void otidalrouteOnContextMenu(wxMouseEvent &event)
	{
		this->PopupMenu(m_menu2, event.GetPosition());
	}

	otidalrouteUIDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 364), long style = wxCAPTION | wxCLOSE_BOX | wxFRAME_FLOAT_ON_PARENT | wxSYSTEM_MENU | wxTAB_TRAVERSAL);

	~otidalrouteUIDialogBase();

};


///////////////////////////////////////////////////////////////////////////////
/// Class otidalroutePreferencesDialogBase
///////////////////////////////////////////////////////////////////////////////
class otidalroutePreferencesDialogBase : public wxDialog
{
private:

protected:

	// Virtual event handlers, overide them in your derived class

public:


	wxCheckBox* m_cbUseRate;
	wxCheckBox* m_cbUseDirection;
	wxCheckBox* m_cbFillColour;

	wxStaticText* m_staticText4;
	wxStaticText* m_staticText41;
	wxStaticText* m_staticText42;
	wxStaticText* m_staticText43;
	wxStaticText* m_staticText44;


	wxButton* m_button111;
	wxButton* m_button2;
	wxButton* m_button3;
	wxButton* m_button4;
	wxButton* m_button5;

	wxColourPickerCtrl* myColourPicker0;
	wxColourPickerCtrl* myColourPicker1;
	wxColourPickerCtrl* myColourPicker2;
	wxColourPickerCtrl* myColourPicker3;
	wxColourPickerCtrl* myColourPicker4;

	wxCheckBox* m_cbCopyFirstCumulativeRecord;
	wxCheckBox* m_cbCopyMissingWaveRecord;
	wxRadioBox* m_rbLoadOptions;
	wxRadioBox* m_rbStartOptions;
	wxRadioBox* m_rbTimeFormat;

	otidalroutePreferencesDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	~otidalroutePreferencesDialogBase();

};


///////////////////////////////////////////////////////////////////////////////
/// Class ConfigurationDialog
///////////////////////////////////////////////////////////////////////////////
class ConfigurationDialog : public wxDialog
{
private:
	
	

protected:

	wxComboBox* m_cStart;
	wxStaticText* m_staticText28;
	//wxStaticText* m_staticText29;
	wxButton* m_bGribTime;
	wxStaticText* m_staticText30;
	wxTextCtrl* m_tRouteName;
	//wxTextCtrl* m_tCTS;
	wxButton* m_bRouteName;
	wxComboBox* m_cEnd;
	
	wxButton* m_bDelete;
	wxButton* m_bSelect;
	wxButton* m_bGenerate;
	wxButton* m_bClose;

	// Virtual event handlers, overide them in your derived class
	virtual void OnUpdateDate(wxDateEvent& event) { event.Skip(); }
	virtual void OnGribTime(wxCommandEvent& event) { event.Skip(); }
	virtual void OnUpdate(wxCommandEvent& event) { event.Skip(); }

	virtual void OnCurrentTime(wxCommandEvent& event) { event.Skip(); }
	void OnDelete(wxCommandEvent& event);
	void OnInformation(wxCommandEvent& event);
	void OnGenerate(wxCommandEvent& event) ; 
	void OnClose(wxCommandEvent& event);

public:
	otidalroute_pi *pPlugIn;
	wxDatePickerCtrl* m_dpStartDate;
	wxListBox* m_lRoutes;
	void AddSource(wxString name);
	void RemoveSource(wxString name);
	void ClearSources();


	ConfigurationDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Tidal Route Configuration"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
	~ConfigurationDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class NewPositionDialog
///////////////////////////////////////////////////////////////////////////////
class NewPositionDialog : public wxDialog
{
private:

protected:
	wxStaticText* m_staticText140;
	wxStaticText* m_staticText142;
	wxStaticText* m_staticText143;
	wxStaticText* m_staticText144;
	
	wxStaticText* m_staticText145;
	wxStaticText* m_staticText146;
	wxStaticText* m_staticText147;
	
	wxStdDialogButtonSizer* m_sdbSizer4;
	wxButton* m_sdbSizer4OK;
	wxButton* m_sdbSizer4Cancel;

public:
	wxTextCtrl* m_tName;
	wxTextCtrl* m_tLatitudeDegrees;
	wxTextCtrl* m_tLatitudeMinutes;
	wxTextCtrl* m_tLongitudeDegrees;
	wxTextCtrl* m_tLongitudeMinutes;
	wxChoice* m_cNS;
	wxChoice* m_cEW;
	NewPositionDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("New Tidal Route Position"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(471, 170), long style = wxDEFAULT_DIALOG_STYLE);
	~NewPositionDialog();

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


#endif //__otidalrouteUIDIALOGBASE_H__
