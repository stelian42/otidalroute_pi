///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////


#include "otidalrouteUIDialogBase.h"
#include "otidalroute_pi.h"
#include "otidalrouteUIDialog.h"

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

otidalrouteUIDialogBase::otidalrouteUIDialogBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	m_menubar3 = new wxMenuBar(0);
	m_menu3 = new wxMenu();
	wxMenuItem* m_mSummary;
	m_mSummary = new wxMenuItem(m_menu3, wxID_ANY, wxString(wxT("Summary")), wxEmptyString, wxITEM_NORMAL);
	m_menu3->Append(m_mSummary);

	wxMenuItem* m_mNewRoute;
	m_mNewRoute = new wxMenuItem(m_menu3, wxID_ANY, wxString(wxT("Route Tables")), wxEmptyString, wxITEM_NORMAL);
	m_menu3->Append(m_mNewRoute);

	wxMenuItem* m_mDeleteAllRoutes;
	m_mDeleteAllRoutes = new wxMenuItem(m_menu3, wxID_ANY, wxString(wxT("Delete All Routes")), wxEmptyString, wxITEM_NORMAL);
	m_menu3->Append(m_mDeleteAllRoutes);

	m_menubar3->Append(m_menu3, wxT("Routes"));

	m_mHelp = new wxMenu();
	wxMenuItem* m_mInformation;
	m_mInformation = new wxMenuItem(m_mHelp, wxID_ANY, wxString(wxT("Guide")), wxEmptyString, wxITEM_NORMAL);
	m_mHelp->Append(m_mInformation);

	wxMenuItem* m_mAbout;
	m_mAbout = new wxMenuItem(m_mHelp, wxID_ANY, wxString(wxT("About")), wxEmptyString, wxITEM_NORMAL);
	m_mHelp->Append(m_mAbout);

	m_menubar3->Append(m_mHelp, wxT("Help"));

	this->SetMenuBar(m_menubar3);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	m_staticText3 = new wxStaticText(this, wxID_ANY, wxT("Speed"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->Wrap(-1);
	bSizer2->Add(m_staticText3, 0, wxALL, 5);

	m_tSpeed = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_tSpeed, 0, wxALL | wxEXPAND, 5);

	m_staticText41 = new wxStaticText(this, wxID_ANY, wxT("Departure Date/Time"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText41->Wrap(-1);
	bSizer2->Add(m_staticText41, 0, wxALL, 5);

	m_textCtrl1 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);

	m_staticText5 = new wxStaticText(this, wxID_ANY, wxT("DR/EP Route"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText5->Wrap(-1);
	bSizer2->Add(m_staticText5, 0, wxALL, 5);

	m_staticText4 = new wxStaticText(this, wxID_ANY, wxT("Route Name"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText4->Wrap(-1);
	bSizer2->Add(m_staticText4, 0, wxALL, 5);

	m_tRouteName = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_tRouteName, 0, wxALL | wxEXPAND, 5);

	m_cbGPX = new wxCheckBox(this, wxID_ANY, wxT("Save as GPX file"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_cbGPX, 0, wxALL, 5);

	m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("Departure Times"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->Wrap(-1);
	bSizer2->Add(m_staticText2, 0, wxALL, 5);

	wxString m_choiceDepartureTimesChoices[] = { wxT("1"), wxT("2"), wxT("3"), wxT("4"), wxT("5"), wxT("6") };
	int m_choiceDepartureTimesNChoices = sizeof(m_choiceDepartureTimesChoices) / sizeof(wxString);
	m_choiceDepartureTimes = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDepartureTimesNChoices, m_choiceDepartureTimesChoices, 0);
	m_choiceDepartureTimes->SetSelection(0);
	bSizer2->Add(m_choiceDepartureTimes, 0, wxALL, 5);

	m_bCalcDR = new wxButton(this, wxID_ANY, wxT("Calculate DR"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_bCalcDR, 0, wxALL | wxEXPAND, 5);

	m_bCalcETA = new wxButton(this, wxID_ANY, wxT("Calculate EP"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_bCalcETA, 0, wxALL | wxEXPAND, 5);


	this->SetSizer(bSizer2);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(otidalrouteUIDialogBase::OnClose));
	this->Connect(wxEVT_SIZE, wxSizeEventHandler(otidalrouteUIDialogBase::OnSize));
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnSummary), this, m_mSummary->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnShowTables), this, m_mNewRoute->GetId());
	m_menu3->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnDeleteAllRoutes), this, m_mDeleteAllRoutes->GetId());
	m_mHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnInformation), this, m_mInformation->GetId());
	m_mHelp->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(otidalrouteUIDialogBase::OnAbout), this, m_mAbout->GetId());
	m_bCalcDR->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(otidalrouteUIDialogBase::DRCalculate), NULL, this);
	m_bCalcETA->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(otidalrouteUIDialogBase::ETACalculate), NULL, this);
}

otidalrouteUIDialogBase::~otidalrouteUIDialogBase()
{
	// Disconnect Events
	this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(otidalrouteUIDialogBase::OnClose));
	this->Disconnect(wxEVT_SIZE, wxSizeEventHandler(otidalrouteUIDialogBase::OnSize));
	m_bCalcDR->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(otidalrouteUIDialogBase::DRCalculate), NULL, this);
	m_bCalcETA->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(otidalrouteUIDialogBase::ETACalculate), NULL, this);

}



///////////////////////////////////////////////////////////////////////////

ConfigurationDialog::ConfigurationDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxFlexGridSizer* fgSizer95;
	fgSizer95 = new wxFlexGridSizer(3, 1, 0, 0);
	fgSizer95->AddGrowableCol(0);
	fgSizer95->SetFlexibleDirection(wxBOTH);
	fgSizer95->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticBoxSizer* sbSizer29;
	sbSizer29 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Routes")), wxVERTICAL);

	m_lRoutes = new wxListBox(sbSizer29->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	sbSizer29->Add(m_lRoutes, 1, wxALL | wxEXPAND, 5);


	fgSizer95->Add(sbSizer29, 1, wxEXPAND, 5);

	wxFlexGridSizer* fgSizer78;
	fgSizer78 = new wxFlexGridSizer(1, 0, 0, 0);
	fgSizer78->SetFlexibleDirection(wxBOTH);
	fgSizer78->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_bDelete = new wxButton(this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer78->Add(m_bDelete, 0, wxALL, 5);

	m_bSelect = new wxButton(this, wxID_ANY, _("Route Table"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer78->Add(m_bSelect, 0, wxALL, 5);

	m_bGenerate = new wxButton(this, wxID_ANY, _("Chart Route"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer78->Add(m_bGenerate, 0, wxALL, 5);

	m_bClose = new wxButton(this, wxID_ANY, _(" Close"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer78->Add(m_bClose, 0, wxALIGN_RIGHT | wxALL, 5);


	fgSizer95->Add(fgSizer78, 1, wxEXPAND, 5);


	this->SetSizer(fgSizer95);
	this->Layout();
	fgSizer95->Fit(this);

	this->Centre(wxBOTH);

	// Connect Events
	m_bDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnDelete), NULL, this);
	m_bSelect->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnInformation), NULL, this);
	m_bGenerate->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnGenerate), NULL, this);
	m_bClose->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnClose), NULL, this);
}

ConfigurationDialog::~ConfigurationDialog()
{
	// Disconnect Events
	m_bDelete->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnDelete), NULL, this);
	m_bSelect->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnInformation), NULL, this);
	m_bGenerate->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnGenerate), NULL, this);
	m_bClose->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ConfigurationDialog::OnClose), NULL, this);

}



void ConfigurationDialog::OnDelete(wxCommandEvent& event)
{
	wxString rn;
	int s;
	s = m_lRoutes->GetSelection();
	rn = m_lRoutes->GetString(s);
	
	for (std::list<TidalRoute>::iterator it = pPlugIn->m_potidalrouteDialog->m_TidalRoutes.begin();
		it != pPlugIn->m_potidalrouteDialog->m_TidalRoutes.end(); it++) {
		if ((*it).Name == rn ) {
			pPlugIn->m_potidalrouteDialog->m_TidalRoutes.erase(it);
			m_lRoutes->Delete(s);

			pPlugIn->m_potidalrouteDialog->SaveXML(pPlugIn->m_potidalrouteDialog->m_default_configuration_path);
		}
	}
}
void ConfigurationDialog::OnInformation(wxCommandEvent& event)
{
	wxString rn;
	int s;
	s = m_lRoutes->GetSelection();

	if (s == -1){ 		
		wxMessageBox(_("Please select a route"));
		return; 
	}
	
	rn = m_lRoutes->GetString(s);
	/*
	
	if (m_lRoutes->IsEmpty()){
		wxMessageBox(_("Please select positions and generate a route"));
		return;
	}
	*/
	pPlugIn->m_potidalrouteDialog->GetTable(rn);
	return; //
}


void ConfigurationDialog::OnGenerate(wxCommandEvent& event)
{
	wxString rn;
	int s;
	s = m_lRoutes->GetSelection();

	if (s == -1) {
		wxMessageBox(_("Please select a route"));
		return;
	}

	rn = m_lRoutes->GetString(s);

	pPlugIn->m_potidalrouteDialog->AddChartRoute(rn);
}


void ConfigurationDialog::OnClose(wxCommandEvent& event)
{
	Hide();
}


///////////////////////////////////////////////////////////////////////////

#include "otidalrouteUIDialogBase.h"

///////////////////////////////////////////////////////////////////////////

AboutDialogBase::AboutDialogBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxFlexGridSizer* fgSizer90;
	fgSizer90 = new wxFlexGridSizer(0, 1, 0, 0);
	fgSizer90->SetFlexibleDirection(wxBOTH);
	fgSizer90->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxFlexGridSizer* fgSizer109;
	fgSizer109 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer109->SetFlexibleDirection(wxBOTH);
	fgSizer109->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText135 = new wxStaticText(this, wxID_ANY, _("oTidalRoute Plugin Version"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText135->Wrap(-1);
	fgSizer109->Add(m_staticText135, 0, wxALL, 5);

	m_stVersion = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_stVersion->Wrap(-1);
	fgSizer109->Add(m_stVersion, 0, wxALL, 5);


	fgSizer90->Add(fgSizer109, 1, wxEXPAND, 5);

	m_staticText110 = new wxStaticText(this, wxID_ANY, _("The oTidalRoute plugin for opencpn is intended to calculate routes based on computerized tidal data.\n\nLicense: GPLv3+\n\nSource Code:\nhttps://github.com/rasbats/oTidalRoute_pi\n\nAuthor:\nMike Rossiter\n\nMany thanks to all translators and testers."), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText110->Wrap(400);
	fgSizer90->Add(m_staticText110, 0, wxALL, 5);

	wxFlexGridSizer* fgSizer91;
	fgSizer91 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer91->SetFlexibleDirection(wxBOTH);
	fgSizer91->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_bAboutAuthor = new wxButton(this, wxID_ANY, _("About Author"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer91->Add(m_bAboutAuthor, 0, wxALL, 5);

	m_bClose = new wxButton(this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer91->Add(m_bClose, 0, wxALL, 5);


	fgSizer90->Add(fgSizer91, 1, wxEXPAND, 5);


	this->SetSizer(fgSizer90);
	this->Layout();
	fgSizer90->Fit(this);

	this->Centre(wxBOTH);

	// Connect Events
	m_bAboutAuthor->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialogBase::OnAboutAuthor), NULL, this);
	m_bClose->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialogBase::OnClose), NULL, this);
}

AboutDialogBase::~AboutDialogBase()
{
	// Disconnect Events
	m_bAboutAuthor->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialogBase::OnAboutAuthor), NULL, this);
	m_bClose->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialogBase::OnClose), NULL, this);

}
