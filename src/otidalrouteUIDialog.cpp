/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  otidalroute Object
 * Author:   Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2016 by Mike Rossiter  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 *
 */
#include <wx/intl.h>
#include "wx/wx.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/debug.h>
#include <wx/graphics.h>
#include <wx/stdpaths.h>

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "otidalroute_pi.h"
#include "folder.xpm"
#include "icons.h"
#include <wx/arrimpl.cpp>

#ifdef __WXMSW__
#include <windows.h>
#endif
#include <memory.h> 

#include <wx/colordlg.h>
#include <wx/event.h>
#include "AboutDialog.h"




class GribRecordSet;
class TidalRoute;
class ConfigurationDialog;
class RouteProp;
class AboutDialog;

using namespace std;


#define FAIL(X) do { error = X; goto failed; } while(0)

//date/time in the desired time zone format
static wxString TToString( const wxDateTime date_time, const int time_zone )
{
    wxDateTime t( date_time );
    t.MakeFromTimezone( wxDateTime::UTC );
    if( t.IsDST() ) t.Subtract( wxTimeSpan( 1, 0, 0, 0 ) );
    switch( time_zone ) {
        case 0: return t.Format( _T(" %a %d-%b-%Y  %H:%M LOC"), wxDateTime::Local );
        case 1:
        default: return t.Format( _T(" %a %d-%b-%Y %H:%M  UTC"), wxDateTime::UTC );
    }
}


static double deg2rad(double degrees)
{
	return M_PI * degrees / 180.0;
}

static double rad2deg(double radians)
{
	return 180.0 * radians / M_PI;
}

static void CTSWithCurrent(double BG, double &VBG, double C, double VC, double &BC, double VBC)
{
	if (VC == 0) { // short-cut if no current
		BC = BG, VBG = VBC;
		return;
	}
	
	// Thanks to Geoff Sargent at "tidalstreams.net"

	double B5 = VC / VBC;
	double C1 = deg2rad(BG);
	double C2 = deg2rad(C);

	double C6 = asin(B5 * sin(C1 - C2));
	double B6 = rad2deg(C6);
	if ((BG + B6) > 360){
		BC = BG + B6 - 360;
	}
	else {
		BC = BG + B6;
	}
	VBG = (VBC * cos(C6)) + (VC * cos(C1 - C2));

}

static void CMGWithCurrent(double &BG, double &VBG, double C, double VC, double BC, double VBC)
{
	if (VC == 0) { // short-cut if no current
		BG = BC, VBG = VBC;
		return;
	}

	// Thanks to Geoff Sargent at "tidalstreams.net"
	// BUT this function has not been tested !!!

	double B5 = VC / VBC;
	double C1 = deg2rad(BC);
	double C2 = deg2rad(C);

	double B3 = VC;
	double B4 = VBC;

	double C5 = sqr(B3) + sqr(B4) + 2 * B3*B4*cos(C1 - C2);
	double D5 = C5;
	double E5 = sqrt(C5);
	double E6 = B3*sin(C2 - C1) / E5;
	double E7 = asin(E6);
	double E8 = rad2deg(E7);
	if ((BC + E8) > 360){
		BG = BC + E8 - 360;
	}
	else {
		BG = BC + E8;
	}
	VBG = E5;
}

#if !wxCHECK_VERSION(2,9,4) /* to work with wx 2.8 */
#define SetBitmap SetBitmapLabel
#endif


otidalrouteUIDialog::otidalrouteUIDialog(wxWindow *parent, otidalroute_pi *ppi)
	: otidalrouteUIDialogBase(parent), m_ConfigurationDialog(this, wxID_ANY, _("Tidal Route Configuration"),
	wxDefaultPosition, wxSize(-1, -1), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	pParent = parent;
    pPlugIn = ppi;

    wxFileConfig *pConf = GetOCPNConfigObject();

    if(pConf) {
        pConf->SetPath ( _T ( "/Settings/otidalroute" ) );

		pConf->Read ( _T ( "otidalrouteUseRate" ), &m_bUseRate );
        pConf->Read ( _T ( "otidalrouteUseDirection" ), &m_bUseDirection);
		pConf->Read ( _T ( "otidalrouteUseFillColour" ), &m_bUseFillColour);

		pConf->Read( _T("VColour0"), &myVColour[0], myVColour[0] );
		pConf->Read( _T("VColour1"), &myVColour[1], myVColour[1] );
		pConf->Read( _T("VColour2"), &myVColour[2], myVColour[2] );
		pConf->Read( _T("VColour3"), &myVColour[3], myVColour[3] );
		pConf->Read( _T("VColour4"), &myVColour[4], myVColour[4] );
		
		myUseColour[0] = myVColour[0];
		myUseColour[1] = myVColour[1];
		myUseColour[2] = myVColour[2];
		myUseColour[3] = myVColour[3];
		myUseColour[4] = myVColour[4];

    }

	m_lPositions->InsertColumn(POSITION_NAME, _("Name"),0,25);
	m_lPositions->InsertColumn(POSITION_LAT, _("Lat"));
	m_lPositions->InsertColumn(POSITION_LON, _("Lon"));

	UpdateColumns(); 

	m_default_configuration_path = ppi->StandardPath()
		+ _T("otidalroute_config.xml");
	
	if (!OpenXML(m_default_configuration_path, false)) {
		// create directory for plugin files if it doesn't already exist 
		wxFileName fn(m_default_configuration_path);
		wxFileName fn2 = fn.GetPath();
		if (!fn.DirExists()) {
			fn2.Mkdir();
			fn.Mkdir();
		}
	}
	
	m_ConfigurationDialog.pPlugIn = ppi;   

    this->Connect( wxEVT_MOVE, wxMoveEventHandler( otidalrouteUIDialog::OnMove ) );

	m_tSpeed->SetValue(_T("5"));
	m_dtNow = wxDateTime::Now(); 

	b_showTidalArrow = false;

	DimeWindow( this );

    Fit();
    SetMinSize( GetBestSize() );
	
}

otidalrouteUIDialog::~otidalrouteUIDialog()
{
    wxFileConfig *pConf = GetOCPNConfigObject();;

    if(pConf) {

        pConf->SetPath ( _T ( "/Settings/otidalroute" ) );

		pConf->Write ( _T ( "otidalrouteUseRate" ), m_bUseRate );
		pConf->Write ( _T ( "otidalrouteUseDirection" ), m_bUseDirection );
		pConf->Write ( _T ( "otidalrouteUseFillColour" ), m_bUseFillColour );

		pConf->Write( _T("VColour0"), myVColour[0] );
		pConf->Write( _T("VColour1"), myVColour[1] );
		pConf->Write( _T("VColour2"), myVColour[2] );
		pConf->Write( _T("VColour3"), myVColour[3] );
		pConf->Write( _T("VColour4"), myVColour[4] );
		
    }
	SaveXML(m_default_configuration_path);
	
}

void otidalrouteUIDialog::SetCursorLatLon( double lat, double lon )
{
    m_cursor_lon = lon;
    m_cursor_lat = lat;
}

void otidalrouteUIDialog::SetViewPort( PlugIn_ViewPort *vp )
{
    if(m_vp == vp)  return;

    m_vp = new PlugIn_ViewPort(*vp);
}

void otidalrouteUIDialog::OnClose( wxCloseEvent& event )
{
    pPlugIn->OnotidalrouteDialogClose();
}


void otidalrouteUIDialog::OnNewRoute(wxCommandEvent& event)
{
	b_showTidalArrow = false;
	GetParent()->Refresh();
	m_ConfigurationDialog.Show();
}

void otidalrouteUIDialog::OnDeleteRoute(wxCommandEvent& event)
{
	if (m_TidalRoutes.empty()){
		wxMessageBox(_("No routes have been calculated"));
		return;
	}
	else {
		m_ConfigurationDialog.Show();
	}

	GetParent()->Refresh();
}
/*
void otidalrouteUIDialog::OnDelete()
{
	wxString rn;
	
	rn = m_tRouteName->GetValue();

	for (std::list<TidalRoute>::iterator it = pPlugIn->m_potidalrouteDialog->m_TidalRoutes.begin();
		it != pPlugIn->m_potidalrouteDialog->m_TidalRoutes.end(); it++) {
		if ((*it).Name == rn) {
			pPlugIn->m_potidalrouteDialog->m_TidalRoutes.erase(it);
						
		}
	}
}
*/
void otidalrouteUIDialog::OnDeleteAllRoutes(wxCommandEvent& event)
{
	if (m_TidalRoutes.empty()){
		wxMessageBox(_("No routes have been calculated"));
		return;
	}
	wxMessageDialog mdlg(this, _("Delete all routes?\n"),
		_("Delete All Routes"), wxYES | wxNO | wxICON_WARNING);
	if (mdlg.ShowModal() == wxID_YES) {
		m_TidalRoutes.clear();
		m_ConfigurationDialog.m_lRoutes->Clear();
	}

	GetParent()->Refresh();

}
void otidalrouteUIDialog::OnMove( wxMoveEvent& event )
{
    //    Record the dialog position
    wxPoint p = GetPosition();
    pPlugIn->SetotidalrouteDialogX( p.x );
    pPlugIn->SetotidalrouteDialogY( p.y );

    event.Skip();
}

void otidalrouteUIDialog::OnSize( wxSizeEvent& event )
{
    //    Record the dialog size
    wxSize p = event.GetSize();
    pPlugIn->SetotidalrouteDialogSizeX( p.x );
    pPlugIn->SetotidalrouteDialogSizeY( p.y );

    event.Skip();
}

void otidalrouteUIDialog::OpenFile(bool newestFile)
{
	m_bUseRate = pPlugIn->GetCopyRate();
	m_bUseDirection = pPlugIn->GetCopyDirection();
	m_bUseFillColour = pPlugIn->GetCopyColour();

	m_FolderSelected = pPlugIn->GetFolderSelected();
	m_IntervalSelected = pPlugIn->GetIntervalSelected();
}

wxString otidalrouteUIDialog::MakeDateTimeLabel(wxDateTime myDateTime)
{			
		wxDateTime dt = myDateTime;

		wxString s2 = dt.Format ( _T( "%Y-%m-%d"));
		wxString s = dt.Format(_T("%H:%M")); 
		wxString dateLabel = s2 + _T(" ") + s;	

		m_textCtrl1->SetValue(dateLabel);				

		return dateLabel;	
}

void otidalrouteUIDialog::OnInformation(wxCommandEvent& event)
{
	
	wxString infolocation = *GetpSharedDataLocation()
		+ _T("plugins/otidalroute_pi/data/") + _("TidalRoutingInformation.html");
	wxLaunchDefaultBrowser(_T("file:///") + infolocation);
	
}

void otidalrouteUIDialog::OnAbout(wxCommandEvent& event)
{
	AboutDialog dlg(GetParent());
	dlg.ShowModal();
}

void otidalrouteUIDialog::OnSummary(wxCommandEvent& event)
{
	TableRoutes* tableroutes = new TableRoutes(this, 7000, _T(" Route Summary"), wxPoint(200, 200), wxSize(650, 200), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	
	wxString RouteName;
	wxString From;
	wxString Towards;
	wxString StartTime;
	wxString EndTime;
	wxString Duration;
	wxString Distance;
	wxString Type;

	if (m_TidalRoutes.empty()){

		wxMessageBox(_("No routes found. Please make a route"));
		return;
	}		

	int in = 0;
	
	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {
		

		RouteName = (*it).Name;
		From = (*it).Start;
		Towards = (*it).End;
		StartTime = (*it).StartTime;
		EndTime = (*it).EndTime;
		Duration = (*it).Time;
		Distance = (*it).Distance;
		Type = (*it).Type;

		tableroutes->m_wpList->InsertItem(in, _T(""), -1);
		tableroutes->m_wpList->SetItem(in, 0, RouteName);
		tableroutes->m_wpList->SetItem(in, 1, From);
		tableroutes->m_wpList->SetItem(in, 2, Towards);
		tableroutes->m_wpList->SetItem(in, 3, StartTime);
		tableroutes->m_wpList->SetItem(in, 4, EndTime);
		tableroutes->m_wpList->SetItem(in, 5, Duration);
		tableroutes->m_wpList->SetItem(in, 6, Distance);
		tableroutes->m_wpList->SetItem(in, 7, Type);

		in++;	
		
	}

	tableroutes->Show();

	GetParent()->Refresh();
		
}


void otidalrouteUIDialog::ShowTable(){
	//
	// this shown from right-click context menu
	//
	wxString name;
	if (m_tRouteName->GetValue() == _("")){
		wxMessageBox(_("Please select or generate a route"));
		return;
	}

	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {

		if (!m_TidalRoutes.empty()){
			name = (*it).Name;
			break;
		}
		else {
			wxMessageBox(_("Please select or generate a route"));
			return;
		}
	}

	RouteProp* routetable = new RouteProp(this, 7000, _T("Tidal Route Table"), wxPoint(200, 200), wxSize(650, 800), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	int in = 0;

	wxString lat;
	wxString lon;
	wxString etd;
	wxString cts;
	wxString dis;
	wxString set;
	wxString rat;

	routetable->m_PlanSpeedCtl->SetValue(pPlugIn->m_potidalrouteDialog->m_tSpeed->GetValue());

	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {
		name = (*it).Name;
		if (m_tRouteName->GetValue() == name){

			routetable->m_RouteNameCtl->SetValue(name);
			routetable->m_RouteStartCtl->SetValue((*it).Start);
			routetable->m_RouteDestCtl->SetValue((*it).End);

			routetable->m_TotalDistCtl->SetValue((*it).Distance);
			routetable->m_TimeEnrouteCtl->SetValue((*it).Time);
			routetable->m_StartTimeCtl->SetValue((*it).StartTime);
			wxString t;
			if ((*it).Type == _T("h")){
				t = _T("Adjust Each Hour");
			}
			else if ((*it).Type == _T("s")){
				t = _T("Maintain Single Course");
			}
			routetable->m_TypeRouteCtl->SetValue(t);

			for (std::list<Position>::iterator itp = (*it).m_positionslist.begin();
				itp != (*it).m_positionslist.end(); itp++) {

				name = (*itp).name;
				lat = (*itp).lat;
				lon = (*itp).lon;
				etd = (*itp).time;
				cts = (*itp).CTS;
				dis = (*itp).distTo;
				set = (*itp).set;
				rat = (*itp).rate;

				routetable->m_wpList->InsertItem(in, _T(""), -1);
				routetable->m_wpList->SetItem(in, 1, name);
				routetable->m_wpList->SetItem(in, 2, dis);
				routetable->m_wpList->SetItem(in, 4, lat);
				routetable->m_wpList->SetItem(in, 5, lon);
				routetable->m_wpList->SetItem(in, 6, etd);
				routetable->m_wpList->SetItem(in, 8, cts);
				routetable->m_wpList->SetItem(in, 9, set);
				routetable->m_wpList->SetItem(in, 10, rat);

				in++;
			}
		}
	}

	routetable->Show();

}

void otidalrouteUIDialog::GetTable(wxString myRoute){

	wxString name;

	if (m_TidalRoutes.empty()){
		wxMessageBox(_("Please select or generate a route"));
		return;
	}	

	RouteProp* routetable = new RouteProp(this, 7000, _T("Tidal Route Table"), wxPoint(200, 200), wxSize(650, 800), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	int in = 0;

	wxString lat;
	wxString lon;
	wxString etd;
	wxString cts;
	wxString dis;
	wxString set;
	wxString rat;

	routetable->m_PlanSpeedCtl->SetValue(pPlugIn->m_potidalrouteDialog->m_tSpeed->GetValue());

	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {
		name = (*it).Name;
		if (myRoute == name){

			routetable->m_RouteNameCtl->SetValue(name);
			routetable->m_RouteStartCtl->SetValue((*it).Start);
			routetable->m_RouteDestCtl->SetValue((*it).End);

			routetable->m_TotalDistCtl->SetValue((*it).Distance);
			routetable->m_TimeEnrouteCtl->SetValue((*it).Time);
			routetable->m_StartTimeCtl->SetValue((*it).StartTime);
			wxString t;
			if ((*it).Type == _T("h")){
				t = _T("Adjust Each Hour");
			}
			else if ((*it).Type == _T("s")){
				t = _T("Maintain Single Course");
			}
			routetable->m_TypeRouteCtl->SetValue(t);

			for (std::list<Position>::iterator itp = (*it).m_positionslist.begin();
				itp != (*it).m_positionslist.end(); itp++) {

				name = (*itp).name;
				lat = (*itp).lat;
				lon = (*itp).lon;
				etd = (*itp).time;
				cts = (*itp).CTS;
				dis = (*itp).distTo;
				set = (*itp).set;
				rat = (*itp).rate;

				routetable->m_wpList->InsertItem(in, _T(""), -1);
				routetable->m_wpList->SetItem(in, 1, name);
				routetable->m_wpList->SetItem(in, 2, dis);
				routetable->m_wpList->SetItem(in, 4, lat);
				routetable->m_wpList->SetItem(in, 5, lon);
				routetable->m_wpList->SetItem(in, 6, etd);
				routetable->m_wpList->SetItem(in, 8, cts);
				routetable->m_wpList->SetItem(in, 9, set);
				routetable->m_wpList->SetItem(in, 10, rat);

				in++;
			}
		}
	}

	routetable->Show();

}

void otidalrouteUIDialog::AddPosition(double lat, double lon)
{
	wxTextEntryDialog pd(this, _("Enter Name"), _("New Position"));
	if (pd.ShowModal() == wxID_OK)
		AddPosition(lat, lon, pd.GetValue());
}

void otidalrouteUIDialog::AddPosition(double lat, double lon, wxString name)
{
	for (std::vector<RouteMapPosition>::iterator it = Positions.begin();
		it != Positions.end(); it++) {
		if ((*it).Name == name) {
			wxMessageDialog mdlg(this, _("This name already exists, replace?\n"),
				_("Tidal Route"), wxYES | wxNO | wxICON_WARNING);
			if (mdlg.ShowModal() == wxID_YES) {
				m_ConfigurationDialog.RemoveSource(name);

				long index = m_lPositions->FindItem(0, name);
				(*it).lat = lat;
				(*it).lon = lon;
				m_lPositions->SetItem(index, POSITION_LAT, wxString::Format(_T("%.5f"), lat));
				m_lPositions->SetItem(index, POSITION_LON, wxString::Format(_T("%.5f"), lon));

				m_ConfigurationDialog.AddSource(name);
				return;
			}			
		}
	}

	Positions.push_back(RouteMapPosition(name, lat, lon));

	wxListItem item;
	long index = m_lPositions->InsertItem(m_lPositions->GetItemCount(), item);
	m_lPositions->SetItem(index, POSITION_NAME, name);
	m_lPositions->SetColumnWidth(POSITION_NAME, wxLIST_AUTOSIZE);

	m_lPositions->SetItem(index, POSITION_LAT, wxString::Format(_T("%.5f"), lat));
	m_lPositions->SetItem(index, POSITION_LON, wxString::Format(_T("%.5f"), lon));

	m_ConfigurationDialog.AddSource(name);
	
}

#if 0 /* wx widgets only allows users
to edit the first column, so this doesn't work ????? */
void otidalrouteUIDialog::OnListLabelEdit(wxListEvent& event)
{
	long index = event.GetIndex();
	int col = event.GetColumn();

	long i = 0;
	for (std::list<RouteMapPosition>::iterator it = RouteMap::Positions.begin();
		it != RouteMap::Positions.end(); it++, i++)
		if (i == index) {
			if (col == POSITION_NAME) {
				(*it).Name = event.GetText();
			}
			else {
				double value;
				event.GetText().ToDouble(&value);
				if (col == POSITION_LAT)
					(*it).lat = value;
				else if (col == POSITION_LON)
					(*it).lon = value;

				m_lPositions->SetItem(index, col, wxString::Format(_T("%.5f"), value));
				UpdateConfigurations();
			}
		}
}
#endif


void otidalrouteUIDialog::OnNewPosition(wxCommandEvent& event)
{
	NewPositionDialog dlg(this);
	if (dlg.ShowModal() == wxID_OK) {
		double lat = 0, lon = 0, lat_minutes = 0, lon_minutes = 0;

		wxString latitude_degrees = dlg.m_tLatitudeDegrees->GetValue();
		wxString latitude_minutes = dlg.m_tLatitudeMinutes->GetValue();
		latitude_degrees.ToDouble(&lat);
		latitude_minutes.ToDouble(&lat_minutes);
		lat += lat_minutes / 60;
		if (dlg.m_cNS->GetSelection() == 1){
			lat = -lat;
		}

		wxString longitude_degrees = dlg.m_tLongitudeDegrees->GetValue();
		wxString longitude_minutes = dlg.m_tLongitudeMinutes->GetValue();
		longitude_degrees.ToDouble(&lon);
		longitude_minutes.ToDouble(&lon_minutes);
		lon += lon_minutes / 60;

		if (dlg.m_cEW->GetSelection() == 1){
			lon = -lon;
		}

		AddPosition(lat, lon, dlg.m_tName->GetValue());
	}
}

void otidalrouteUIDialog::AddConfiguration(RouteMapConfiguration configuration)
{/*
	TidalRoute tidalroute; 
	
	tidalroute.Start = configuration.Start;
	tidalroute.End = configuration.End;
	wxMessageBox(tidalroute.Start);
	int c = m_lTidalRoutes->GetItemCount();

	wxListItem item;
	long index = m_lTidalRoutes->InsertItem(c, item);
	m_lTidalRoutes->SetItem(index, START, tidalroute.Start);
	m_lTidalRoutes->SetColumnWidth(START, wxLIST_AUTOSIZE);

	//m_mDeleteAll->Enable();
	//m_mComputeAll->Enable();
	//m_mExportAll->Enable();*/
}

void otidalrouteUIDialog::AddTidalRoute(TidalRoute tr)
{
	m_TidalRoutes.push_back(tr);
	wxString it = tr.Name;
	m_ConfigurationDialog.m_lRoutes->Append(it);

}
void otidalrouteUIDialog::Export()
{

	PlugIn_Track* newTrack = new PlugIn_Track;
	newTrack->m_NameString = _("TidalRoute");

	for (std::vector<RouteMapPosition>::iterator it = Positions.begin(); it != Positions.end(); it++) {
		PlugIn_Waypoint*  newPoint = new PlugIn_Waypoint
			((*it).lat, (*it).lon, _T("circle"), _("Tidal Route Point"));

		newPoint->m_CreateTime = wxDateTime::Now();
		newTrack->pWaypointList->Append(newPoint);
	}

	AddPlugInTrack(newTrack);

	GetParent()->Refresh();
}


void otidalrouteUIDialog::RequestGrib(wxDateTime time)


{	
	wxJSONValue v;
	time = time.FromUTC();

	v[_T("Day")] = time.GetDay();
	v[_T("Month")] = time.GetMonth();
	v[_T("Year")] = time.GetYear();
	v[_T("Hour")] = time.GetHour();
	v[_T("Minute")] = time.GetMinute();
	v[_T("Second")] = time.GetSecond();

	wxJSONWriter w;
	wxString out;
	w.Write(v, out);

	SendPluginMessage(wxString(_T("GRIB_TIMELINE_RECORD_REQUEST")), out);

	Lock();
	m_bNeedsGrib = false;
	Unlock();
}

void otidalrouteUIDialog::SingleCTS(wxCommandEvent& event)
{
	//
	// Calculates single course to maintain to reach final ep before destination
	//

	TidalRoute tr;
	Position ptr ;

	if (m_TidalRoutes.empty()){
		tr.Name = m_tRouteName->GetValue();
		tr.Type = _("s");
	}
	else {
		for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
			it != m_TidalRoutes.end(); it++) {
			if ((*it).Name == m_tRouteName->GetValue()){
				wxMessageBox(_("Route name already exists, please edit the name"));
				return;
			}
			else {
				tr.m_positionslist.clear();
				tr.Name = m_tRouteName->GetValue();
				tr.Type = _("s");
			}
		}
	}

	PlugIn_Route* trk = new PlugIn_Route;
	PlugIn_Route* newRoute = new PlugIn_Route;
	newRoute->m_NameString = tr.Name;
	
	m_arrowList.clear();
	Arrow m_arrow;

	double speed;

	wxString s = m_tSpeed->GetValue();
	s.ToDouble(&speed);

	double VBG, BC;
	double toDist, toBrng;
	double latR, lonR;
	double lati, loni, latEP, lonEP;
	double latN[2], lonN[2];
	double cts; 

	for (std::vector<RouteMapPosition>::iterator it = Positions.begin(); it != Positions.end(); it++){
		
		if (m_textCtrl3->GetValue() == _T("") || m_textCtrl4->GetValue() == _T("")){
			wxMessageBox(_("No route generated"));
			return;
		}
		
		if ((*it).Name == m_textCtrl3->GetValue()){
             
			latN[0] = (*it).lat;
			lonN[0] = (*it).lon;
			tr.Start = (*it).Name;			
		}

		if ((*it).Name == m_textCtrl4->GetValue()){

			latN[1] = (*it).lat;
			lonN[1] = (*it).lon;
			tr.End = (*it).Name;		
		}

	}	
	//
	// Get the single course to steer to make the destination
	//
	GetCTS_DMG(cts); // this generates the currents list - m_cList
	//
	if (m_cList.size() == 0){
		wxMessageBox(_T("No route created"));
		return;
	}
	//
	//
	// 
	// cts is the single course needed up until the final ep
	//
	double tdist = 0; // For accumulating the total distance by adding the distance of each leg
	double myDist, myBrng;

	DistanceBearingMercator(latN[1], lonN[1], latN[0], lonN[0], &myDist, &myBrng);

	wxDateTime dt;
	wxString sdt;
	wxString ddt;
	wxDateTime dtStart, dtEnd;
	wxTimeSpan trTime;
	wxTimeSpan HourSpan, MinuteSpan;
	double trTimeHours;

	HourSpan = wxTimeSpan::Hours(1);
	MinuteSpan = wxTimeSpan::Minutes(30);

	sdt = m_textCtrl1->GetValue();
	dt.ParseDateTime(sdt);
	
	sdt = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));
	tr.StartTime = sdt;
	dtStart = dt;

	PlugIn_Waypoint*  newPoint = new PlugIn_Waypoint
		(latN[0], lonN[0], _T("circle"),tr.Start);
	newPoint->m_MarkDescription = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));
	newPoint->m_GUID = wxString::Format(_T("%i"), (int) GetRandomNumber(1, 4000000));	
	newRoute->pWaypointList->Append(newPoint);

	ptr.name = tr.Start; // _T("Start");
	ptr.time = sdt;
	ptr.lat = wxString::Format(_T("%8.4f"), latN[0]);
	ptr.lon = wxString::Format(_T("%8.4f"), lonN[0]);
	ptr.guid = newPoint->m_GUID;
	ptr.CTS = wxString::Format(_T("%03.0f"), cts);
	ptr.distTo = _T("----");
	ptr.set = _T("----");
	ptr.rate = _T("----");
	tr.m_positionslist.push_back(ptr);

	//
	// initialise start positions
	//
	latEP = latN[0];
	lonEP = lonN[0];	

	latR = latN[0];
	lonR = lonN[0];

	int sz = m_cList.size();
	int n = 1;
	int epCounter = 0;
	wxString epStringNumber;

	for (std::list<Arrow>::iterator it = m_cList.begin(); it != m_cList.end(); it++){

		destLoxodrome(latR, lonR, (*it).m_dir, (*it).m_force, &lati, &loni);
		destLoxodrome(lati, loni, cts, speed, &latEP, &lonEP);

		DistanceBearingMercator(latR, lonR, latEP, lonEP, &toDist, &toBrng);

		m_arrow.m_dt = dt;
		m_arrow.m_lat = (*it).m_lat;
		m_arrow.m_lon = (*it).m_lon;
		m_arrow.m_dir = (*it).m_dir;
		m_arrow.m_force = (*it).m_force;
		m_arrowList.push_back(m_arrow);
		

		if ((it != m_cList.end()) && (next(it) == m_cList.end()))
		{
			//
			// point at the last element of the list
			//
		   
			DistanceBearingMercator(latN[1], lonN[1], latR, lonR, &toDist, &toBrng); //

			PlugIn_Waypoint*  endPoint = new PlugIn_Waypoint
				(latN[1], lonN[1], _T("circle"), tr.End);			
			
			CTSWithCurrent(myBrng, VBG, (*it).m_dir, (*it).m_force, BC, speed);
			
			int minutesToEnd;		
			minutesToEnd = (toDist / VBG) * 60;
		
			dt.Add(wxTimeSpan::Minutes(minutesToEnd));
			ddt = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));

			endPoint->m_MarkDescription = ddt;
			endPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
			newRoute->pWaypointList->Append(endPoint);

			tr.EndTime = ddt;
			dtEnd = dt;
			trTime = dtEnd - dtStart;
			trTimeHours = (double)trTime.GetMinutes() / 60;
			tr.Time = wxString::Format(_("%.1f"), trTimeHours);

			tdist += toDist;
			tr.Distance = wxString::Format(_("%.1f"), tdist);

			ptr.name = tr.End;
			ptr.lat = wxString::Format(_T("%8.4f"), latN[1]);
			ptr.lon = wxString::Format(_T("%8.4f"), lonN[1]);
			ptr.time = ddt;
			ptr.guid = endPoint->m_GUID;
			ptr.set = wxString::Format(_T("%03.0f"), (*it).m_dir);
			ptr.rate = wxString::Format(_T("%5.1f"), (*it).m_force);
			ptr.CTS = _T("----");
			ptr.distTo = wxString::Format(_T("%.1f"), toDist);
			tr.m_positionslist.push_back(ptr);
			m_TidalRoutes.push_back(tr);

			AddPlugInRoute(newRoute);
			b_showTidalArrow = true;
			GetParent()->Refresh();

			SaveXML(m_default_configuration_path);
			m_ConfigurationDialog.m_lRoutes->Append(tr.Name);
			m_ConfigurationDialog.Refresh();

			return;

		}
	
		
		else {

			epCounter++;
			epStringNumber = _("EP") + wxString::Format(_T("%i"), epCounter);

			PlugIn_Waypoint*  epPoint = new PlugIn_Waypoint
				(latEP, lonEP, _T("triangle"), epStringNumber);
			epPoint->m_MarkDescription = dt.Format(_T("%a %d-%b-%Y  %H:%M"));
			epPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
			newRoute->pWaypointList->Append(epPoint);			
			
			tdist += toDist;

			dt.Add(HourSpan);
			ddt = dt.Format(_T("%a %d-%b-%Y  %H:%M"));

			ptr.name = epStringNumber;
			ptr.lat = wxString::Format(_T("%8.4f"), latEP);
			ptr.lon = wxString::Format(_T("%8.4f"), lonEP);
			ptr.set = wxString::Format(_T("%03.0f"), (*it).m_dir);
			ptr.rate = wxString::Format(_T("%5.1f"),(*it).m_force);
			ptr.time = ddt;
			ptr.guid = epPoint->m_GUID;
			if (n == (sz - 1)){
				// we want the last current data from the list
				// to display final cts against the last ep 
				Arrow cArrow = m_cList.back();
				double cs = cArrow.m_cts;
				ptr.CTS = wxString::Format(_T("%03.0f"), cs);;
			}
			else {
				ptr.CTS = wxString::Format(_T("%03.0f"), cts);;
			}
			ptr.distTo = wxString::Format(_T("%.1f"), toDist);
			tr.m_positionslist.push_back(ptr);		
			
			latR = latEP;
			lonR = lonEP;
			n++;
		}

	
	}

	
}

list<Arrow> otidalrouteUIDialog::GetHourlyCurrent(){

	m_cList.clear();
	Arrow cArrow;

	m_totaltideList.clear();

	double latN[2], lonN[2];
	double speed; // boat speed through the water

	wxString s = m_tSpeed->GetValue();
	s.ToDouble(&speed);

	int n = 0;
	double lati, loni;



	for (std::vector<RouteMapPosition>::iterator it = Positions.begin(); it != Positions.end(); it++){

		if (m_textCtrl3->GetValue() == _T("") || m_textCtrl4->GetValue() == _T("")){

			wxMessageBox(_("No route chosen"));
			return m_cList;
		}

		if ((*it).Name == m_textCtrl3->GetValue()){

			latN[0] = (*it).lat;
			lonN[0] = (*it).lon;
		}

		if ((*it).Name == m_textCtrl4->GetValue()){

			latN[1] = (*it).lat;
			lonN[1] = (*it).lon;
		}
	}

	wxDateTime dt;
	wxString sdt;
	wxTimeSpan HourSpan, MinuteSpan;
	HourSpan = wxTimeSpan::Hours(1);
	MinuteSpan = wxTimeSpan::Minutes(30);

	sdt = m_textCtrl1->GetValue(); // date/time route starts
	dt.ParseDateTime(sdt);

	double myBrng;
	double spd; // current
	double dir; // current

	double VBG;
	double BC;
	double latC, lonC;
	double latR, lonR;

	latC = latN[0];
	lonC = lonN[0];
	latR = latN[0];
	lonR = lonN[0];
	double DTG; //distance to go
	VBG = speed; // initial estimate of speed over ground

	double distToGrib;
	int minutesToGrib;

	do {

		DistanceBearingMercator(latN[1], lonN[1], latR, lonR, &DTG, &myBrng);

		distToGrib = VBG / 2;
		minutesToGrib = (int)((distToGrib / VBG) * 60);

		dt.Add(wxTimeSpan::Minutes(minutesToGrib));

		destLoxodrome(latR, lonR, myBrng, distToGrib, &lati, &loni); // To get lat/lon for Grib

		bool m_bGrib = GetGribSpdDir(dt, lati, loni, spd, dir);
		if (!m_bGrib){
			wxMessageBox(_("Route start date is not compatible with this Grib"));
			return m_cList;
		}

		CTSWithCurrent(myBrng, VBG, dir, spd, BC, speed);

		if (VBG > DTG){ break; } // go to make the last arrow

		cArrow.m_dir = dir;
		cArrow.m_force = spd;
		cArrow.m_lat = lati;
		cArrow.m_lon = loni;
		cArrow.m_cts = BC;
		m_cList.push_back(cArrow); // first current arrow

		destLoxodrome(latR, lonR, myBrng, VBG, &latC, &lonC);

		latR = latC; // R is on the rhumb line
		lonR = lonC;

		dt.Subtract(wxTimeSpan::Minutes(minutesToGrib));
		dt.Add(HourSpan);			

	} while (DTG > VBG);

	   
	distToGrib = DTG / 2;
	minutesToGrib = (int)((distToGrib / VBG) * 60);

	dt.Add(wxTimeSpan::Minutes(minutesToGrib));

	destLoxodrome(latR, lonR, myBrng, distToGrib, &lati, &loni); // To get lat/lon for Grib

	bool m_bGrib = GetGribSpdDir(dt, lati, loni, spd, dir);
	if (!m_bGrib){
		wxMessageBox(_("Route start date is not compatible with this Grib"));
		return m_cList;
	}

	cArrow.m_dir = dir;
	cArrow.m_force = spd;
	cArrow.m_lat = lati;
	cArrow.m_lon = loni;
	cArrow.m_cts = BC;
	m_cList.push_back(cArrow); // single current arrow

	return m_cList;	
	
}


void otidalrouteUIDialog::HourlyCTS(wxCommandEvent& event)
{
	//
	// Calculates hourly course change to reach final destination, staying on the rhumb line
	//

	TidalRoute tr;
	Position ptr;

	if (m_TidalRoutes.empty()){
		tr.Name = m_tRouteName->GetValue();
		tr.Type = _("h");
	}
	else {
		for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
			it != m_TidalRoutes.end(); it++) {
			if ((*it).Name == m_tRouteName->GetValue()){
				wxMessageBox(_("Route name already exists, please edit the name"));
				return;
			}
			else {
				tr.m_positionslist.clear();
				tr.Name = m_tRouteName->GetValue();
				tr.Type = _("h");
			}
		}
	}

	PlugIn_Route* trk = new PlugIn_Route;
	PlugIn_Route* newRoute = new PlugIn_Route;
	newRoute->m_NameString = tr.Name;

	m_arrowList.clear();
	Arrow m_arrow;

	double speed;

	wxString s = m_tSpeed->GetValue();
	s.ToDouble(&speed);

	double VBG, BC;
	double toDist, toBrng;
	double latR, lonR;
	double latEP, lonEP, latF, lonF;
	double latN[2], lonN[2];

	for (std::vector<RouteMapPosition>::iterator it = Positions.begin(); it != Positions.end(); it++){

		if (m_textCtrl3->GetValue() == _T("") || m_textCtrl4->GetValue() == _T("")){
			wxMessageBox(_("No route generated"));
			return;
		}

		if ((*it).Name == m_textCtrl3->GetValue()){

			latN[0] = (*it).lat;
			lonN[0] = (*it).lon;
			tr.Start = (*it).Name;
		}

		if ((*it).Name == m_textCtrl4->GetValue()){

			latN[1] = (*it).lat;
			lonN[1] = (*it).lon;
			tr.End = (*it).Name;
		}

	}
	//
	// 
	//
	GetHourlyCurrent(); // this generates the currents list - m_cList
	//
	if (m_cList.size() == 0){
		wxMessageBox(_T("No route created"));
		return;
	}
	//
	//
	//
	double tdist = 0; // For accumulating the total distance by adding the distance of each leg
	double myDist, myBrng;

	DistanceBearingMercator(latN[1], lonN[1], latN[0], lonN[0], &myDist, &myBrng);

	wxDateTime dt;
	wxString sdt;
	wxString ddt;
	wxDateTime dtStart, dtEnd;
	wxTimeSpan trTime;
	wxTimeSpan HourSpan, MinuteSpan;
	double trTimeHours;

	HourSpan = wxTimeSpan::Hours(1);
	MinuteSpan = wxTimeSpan::Minutes(30);

	sdt = m_textCtrl1->GetValue();
	dt.ParseDateTime(sdt);

	sdt = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));
	tr.StartTime = sdt;
	dtStart = dt;

	//
	// initialise start positions
	//
	latEP = latN[0];
	lonEP = lonN[0];

	latR = latN[0];
	lonR = lonN[0];

	int sz = m_cList.size();

	int minutesToEnd;

	double dir, force;
	int epNumber = 0;
	wxString epName;



	for (std::list<Arrow>::iterator it = m_cList.begin(); it != m_cList.end(); it++){

		if (it == m_cList.begin()){
			PlugIn_Waypoint*  newPoint = new PlugIn_Waypoint
				(latN[0], lonN[0], _T("circle"), tr.Start);
			newPoint->m_MarkDescription = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));
			newPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
			newRoute->pWaypointList->Append(newPoint);

			m_arrow.m_dt = dt;
			m_arrow.m_lat = (*it).m_lat;
			m_arrow.m_lon = (*it).m_lon;
			m_arrow.m_dir = (*it).m_dir;
			m_arrow.m_force = (*it).m_force;
			m_arrowList.push_back(m_arrow);
			

			ptr.name = tr.Start; // _T("Start");
			ptr.time = sdt;
			ptr.lat = wxString::Format(_T("%8.4f"), latN[0]);
			ptr.lon = wxString::Format(_T("%8.4f"), lonN[0]);
			ptr.guid = newPoint->m_GUID;
			ptr.CTS = wxString::Format(_T("%03.0f"), (*it).m_cts);
			ptr.distTo = _T("----");
			ptr.set = _T("----");
			ptr.rate = _T("----");
			tr.m_positionslist.push_back(ptr);

			ptr.set = wxString::Format(_T("%03.0f"), (*it).m_dir);
			ptr.rate = wxString::Format(_T("%3.1f"), (*it).m_force);
			
			dir = (*it).m_dir;
			force = (*it).m_force;

			CTSWithCurrent(myBrng, VBG, dir, force, BC, speed);
			destLoxodrome(latR, lonR, myBrng, VBG, &latEP, &lonEP);
            
			//wxMessageBox(wxString::Format(wxT("%i"), i));

			if (sz == 1){

				PlugIn_Waypoint*  endPoint = new PlugIn_Waypoint
					(latN[1], lonN[1], _T("circle"), tr.End);								

				minutesToEnd = (myDist / VBG) * 60;
				//wxMessageBox(wxString::Format(_T("%i"), minutesToEnd));

				dt.Add(wxTimeSpan::Minutes(minutesToEnd));
				ddt = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));
				tr.EndTime = ddt;

				endPoint->m_MarkDescription = ddt;
				endPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
				newRoute->pWaypointList->Append(endPoint);
				
				trTimeHours = (double) minutesToEnd / 60;
				tr.Time = wxString::Format(_("%.1f"), trTimeHours);
				//wxMessageBox(tr.Time);


				tdist = myDist;
				tr.Distance = wxString::Format(_("%.1f"), tdist);

				ptr.name = tr.End;
				ptr.lat = wxString::Format(_T("%8.4f"), latN[1]);
				ptr.lon = wxString::Format(_T("%8.4f"), lonN[1]);
				ptr.time = ddt;
				ptr.guid = endPoint->m_GUID;
				ptr.CTS = _T("----");
				//ptr.set = wxString::Format(_T("%3.0f"),tempSet);
				//ptr.rate = wxString::Format(_T("%3.0f"), tempRate);
				ptr.distTo = wxString::Format(_T("%.1f"), myDist);
				tr.m_positionslist.push_back(ptr);

				m_TidalRoutes.push_back(tr);

				AddPlugInRoute(newRoute);
				b_showTidalArrow = true;
				GetParent()->Refresh();

				SaveXML(m_default_configuration_path);
				m_ConfigurationDialog.m_lRoutes->Append(tr.Name);
				m_ConfigurationDialog.Refresh();

				return;

			}


		}
			
		else {

			dt.Add(HourSpan);

			epNumber++;
			epName = wxString::Format(wxT("%i"), epNumber);
			PlugIn_Waypoint*  epPoint = new PlugIn_Waypoint
				(latEP, lonEP, _T("triangle"), (_T("EP") + epName));
			epPoint->m_MarkDescription = dt.Format(_T("%a %d-%b-%Y  %H:%M"));
			epPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
			newRoute->pWaypointList->Append(epPoint);

			m_arrow.m_dt = dt;
			m_arrow.m_lat = (*it).m_lat;
			m_arrow.m_lon = (*it).m_lon;
			m_arrow.m_dir = (*it).m_dir;
			m_arrow.m_force = (*it).m_force;
			m_arrowList.push_back(m_arrow);


			tdist += VBG;

			ddt = dt.Format(_T("%a %d-%b-%Y  %H:%M"));

			ptr.name = _T("EP") + epName;
			ptr.lat = wxString::Format(_T("%8.4f"), latEP);
			ptr.lon = wxString::Format(_T("%8.4f"), lonEP);
			ptr.time = ddt;
			ptr.guid = epPoint->m_GUID;
			ptr.distTo = wxString::Format(_T("%.1f"), VBG);
			ptr.CTS = wxString::Format(_T("%03.0f"), (*it).m_cts);
			

			tr.m_positionslist.push_back(ptr);

			ptr.set = wxString::Format(_T("%03.0f"), (*it).m_dir);
			ptr.rate = wxString::Format(_T("%5.1f"), (*it).m_force);

			dir = (*it).m_dir;
			force = (*it).m_force;

			CTSWithCurrent(myBrng, VBG, dir, force, BC, speed);
			destLoxodrome(latEP, lonEP, myBrng, VBG, &latR, &lonR);

			latF = latEP;
			lonF = lonEP;
			
			latEP = latR;
			lonEP = lonR;
				
			}
		
	}

	DistanceBearingMercator(latN[1], lonN[1], latF, lonF, &toDist, &toBrng); //

	PlugIn_Waypoint*  endPoint = new PlugIn_Waypoint
		(latN[1], lonN[1], _T("circle"), tr.End);

	Arrow a = m_cList.back();
	double d = a.m_dir;
	double f = a.m_force;

	CTSWithCurrent(myBrng, VBG, d, f, BC, speed);

	minutesToEnd = (toDist / VBG) * 60;

	dt.Add(wxTimeSpan::Minutes(minutesToEnd));
	ddt = dt.Format(_T(" %a %d-%b-%Y  %H:%M"));

	endPoint->m_MarkDescription = ddt;
	endPoint->m_GUID = wxString::Format(_T("%i"), (int)GetRandomNumber(1, 4000000));
	newRoute->pWaypointList->Append(endPoint);

	Arrow AList = m_cList.back();

	m_arrow.m_dt = dt;
	m_arrow.m_lat = AList.m_lat;
	m_arrow.m_lon = AList.m_lon;
	//m_arrow.m_dir = AList.m_dir;
	//m_arrow.m_force = AList.m_force;
	m_arrowList.push_back(m_arrow);

	tr.EndTime = ddt;
	dtEnd = dt;
	trTime = dtEnd - dtStart;
	trTimeHours = (double)trTime.GetMinutes() / 60;
	tr.Time = wxString::Format(_("%.1f"), trTimeHours);

	tdist += toDist;
	tr.Distance = wxString::Format(_("%.1f"), tdist);

	ptr.name = tr.End;
	ptr.lat = wxString::Format(_T("%8.4f"), latN[1]);
	ptr.lon = wxString::Format(_T("%8.4f"), lonN[1]);
	ptr.time = ddt;
	ptr.guid = endPoint->m_GUID;
	ptr.CTS = _T("----");
	//ptr.set = wxString::Format(_T("%3.0f"), AList.m_dir);
	//ptr.rate = wxString::Format(_T("%5.1f"), AList.m_force);
	ptr.distTo = wxString::Format(_T("%.1f"), toDist);

	tr.m_positionslist.push_back(ptr);

	m_TidalRoutes.push_back(tr);

	AddPlugInRoute(newRoute);
	b_showTidalArrow = true;
	GetParent()->Refresh();

	SaveXML(m_default_configuration_path);
	m_ConfigurationDialog.m_lRoutes->Append(tr.Name);
	m_ConfigurationDialog.Refresh();
	
}

list<Arrow> otidalrouteUIDialog::GetCTS_DMG(double &CTS)
{
	m_cList.clear();
	Arrow cArrow;

	m_totaltideList.clear();

	double latN[2], lonN[2];
	double speed;

	wxString s = m_tSpeed->GetValue();
	s.ToDouble(&speed);

	int n = 0;
	double lati, loni;

	for (std::vector<RouteMapPosition>::iterator it = Positions.begin(); it != Positions.end(); it++){

		if (m_textCtrl3->GetValue() == _T("") || m_textCtrl4->GetValue() == _T("")){

			wxMessageBox(_("No route chosen"));
			return m_cList;
		}

		if ((*it).Name == m_textCtrl3->GetValue()){

			latN[0] = (*it).lat;
			lonN[0] = (*it).lon;
		}

		if ((*it).Name == m_textCtrl4->GetValue()){

			latN[1] = (*it).lat;
			lonN[1] = (*it).lon;
		}
	}

	wxDateTime dt, dtStart, dtEnd;
	wxString sdt;
	wxTimeSpan HourSpan, MinuteSpan;
	HourSpan = wxTimeSpan::Hours(1);
	MinuteSpan = wxTimeSpan::Minutes(30);

	sdt = m_textCtrl1->GetValue();
	dt.ParseDateTime(sdt);

	dtStart = dt;

	double myDist, myBrng;
	DistanceBearingMercator(latN[1], lonN[1], latN[0], lonN[0], &myDist, &myBrng);

	double spd;
	double dir;
	double DTG;
	double VBG;
	double DMG;
	double BC, TBC;
	double cDist, cBrng;
	double latC, lonC;	 // tracks end of current arrow
	double latTC, lonTC;  // temporary position of current arrow
	double latV, lonV; // tracks positions on the rhumb line
	latC = latN[0];
	lonC = lonN[0];
	latTC = latN[0]; 
	lonTC = lonN[0];
	latV = latN[0];
	lonV = lonN[0];
	VBG = speed; // initial estimate of VBG

	double distToGrib;
	int minutesToGrib;

	double fDist, fBrng;
	double iCourse, iDist;

	int myLegCounter = 0;

	do {
		myLegCounter++;

		DistanceBearingMercator(latN[1], lonN[1], latV, lonV, &DTG, &myBrng);
	
		distToGrib = VBG / 2;
		minutesToGrib = (int)((distToGrib / VBG) * 60);

		dt.Add(wxTimeSpan::Minutes(minutesToGrib));

		destLoxodrome(latV, lonV, myBrng, distToGrib, &lati, &loni); // To get lat/lon for Grib

		bool m_bGrib = GetGribSpdDir(dt, lati, loni, spd, dir);
		if (!m_bGrib){
			wxMessageBox(_("Route start date is not compatible with this Grib"));

			return m_cList;
		}
		
		destLoxodrome(latC, lonC, dir, spd, &latTC, &lonTC); // To get lat/lon for current arrow end
		DistanceBearingMercator(latTC, lonTC, latN[0], lonN[0], &cDist, &cBrng); //total current so far
		CTSWithCurrent(myBrng, DMG, cBrng, cDist, BC, (speed * myLegCounter));	

		destLoxodrome(latN[0], lonN[0], myBrng, DMG, &latV, &lonV);
		DistanceBearingMercator(latV, lonV, latTC, lonTC,  &iDist, &iCourse);
		CTSWithCurrent(iCourse, VBG, dir, spd, TBC, speed); // for better interim estimate of SMG

		latC = latTC; // end position current arrow 
		lonC = lonTC;
		
		dt.Subtract(wxTimeSpan::Minutes(minutesToGrib));
        dt.Add(HourSpan);  // to position 1 hour later

		if (DMG > myDist){
			break;
		}

		cArrow.m_dir = dir;
		cArrow.m_force = spd;
		cArrow.m_lat = lati;
		cArrow.m_lon = loni;
		cArrow.m_cts = TBC;
		m_cList.push_back(cArrow);

		
				
	} while (DMG < myDist);

	dt.Subtract(HourSpan);  // to position 1 hour earlier
	myLegCounter--;

	destLoxodrome(latC, lonC, (dir + 180), spd, &latTC, &lonTC); // retrace current to previous current position
	DistanceBearingMercator(latTC, lonTC, latN[0], lonN[0], &cDist, &cBrng); // calculate final total current
	CTSWithCurrent(myBrng, DMG, cBrng, cDist, BC, (speed * myLegCounter));

	destLoxodrome(latN[0], lonN[0], myBrng, DMG, &latV, &lonV);
	DistanceBearingMercator(latN[1], lonN[1], latV, lonV, &fDist, &fBrng); // final DTG
	//fDist = myDist - DMG;

	if (myLegCounter == 0){

		DistanceBearingMercator(latN[1], lonN[1], latN[0], lonN[0], &fDist, &fBrng);
		VBG = speed;
		latV = latN[0];
		lonV = lonN[0];
	}

	distToGrib = fDist / 2;
	minutesToGrib = (int)((distToGrib / VBG) * 60);

	//wxMessageBox(wxString::Format(wxT("%5.2f"), fDist));

	dt.Add(wxTimeSpan::Minutes(minutesToGrib));

	destLoxodrome(latV, lonV, myBrng, distToGrib, &lati, &loni); // To get lat/lon for Grib

	bool m_bGrib = GetGribSpdDir(dt, lati, loni, spd, dir);
	if (!m_bGrib){
		wxMessageBox(_("Route start date is not compatible with this Grib"));
		return m_cList;
	}

	CTSWithCurrent(myBrng, VBG, dir, spd, TBC, speed);  // back on the line for last leg
	
	cArrow.m_dir = dir;
	cArrow.m_force = spd;
	cArrow.m_lat = lati;
	cArrow.m_lon = loni;
	cArrow.m_cts = TBC;
	m_cList.push_back(cArrow); // single current arrow

	CTS = BC;
	return m_cList;

}

bool otidalrouteUIDialog::GetGribSpdDir(wxDateTime dt, double lat, double lon, double &spd, double &dir)
{	
	wxDateTime dtime = dt;
	
	pPlugIn->m_grib_lat = lat;
	pPlugIn->m_grib_lon = lon;
	RequestGrib(dtime);
	if (pPlugIn->m_bGribValid){
		spd = pPlugIn->m_tr_spd;
		dir = pPlugIn->m_tr_dir;
		return true;
	}
	else {		
		return false;
	}

}


int otidalrouteUIDialog::GetRandomNumber(int range_min, int range_max)
{
	long u = (long)wxRound(((double)rand() / ((double)(RAND_MAX)+1) * (range_max - range_min)) + range_min);
	return (int)u;
}


/* C   - Sea Current Direction over ground
VC  - Velocity of Current

provisions to compute boat movement over ground

BG  - boat direction over ground
BGV - boat speed over ground (gps velocity)  
*/


void otidalrouteUIDialog::OverGround(double B, double VB, double C, double VC, double &BG, double &VBG)
{
	if (VC == 0) { // short-cut if no currents
		BG = B, VBG = VB;
		return;
	}

	double Cx = VC * cos(deg2rad(C));
	double Cy = VC * sin(deg2rad(C));
	double BGx = VB * cos(deg2rad(B)) + Cx;
	double BGy = VB * sin(deg2rad(B)) + Cy;
	BG = rad2deg(atan2(BGy, BGx));
	VBG = distance(BGx, BGy);
}

double otidalrouteUIDialog::AttributeDouble(TiXmlElement *e, const char *name, double def)
{
	const char *attr = e->Attribute(name);
	if (!attr)
		return def;
	char *end;
	double d = strtod(attr, &end);
	if (end == attr)
		return def;
	return d;
}

bool otidalrouteUIDialog::OpenXML(wxString filename, bool reportfailure)
{
	Position pos;
	list<Position> m_pos;

	TiXmlDocument doc;
	wxString error;

	wxFileName fn(filename);

	SetTitle(_("oTidalRoute"));

	wxProgressDialog *progressdialog = NULL;
	wxDateTime start = wxDateTime::UNow();

	if (!doc.LoadFile(filename.mb_str()))
		FAIL(_("Failed to load file."));
	else {
		TiXmlHandle root(doc.RootElement());

		if (strcmp(root.Element()->Value(), "OpenCPNotidalrouteConfiguration"))
			FAIL(_("Invalid xml file"));

		Positions.clear();

		int count = 0;
		for (TiXmlElement* e = root.FirstChild().Element(); e; e = e->NextSiblingElement())
			count++;

		int i = 0;
		for (TiXmlElement* e = root.FirstChild().Element(); e; e = e->NextSiblingElement(), i++) {
			if (progressdialog) {
				if (!progressdialog->Update(i))
					return true;
			}
			else {
				wxDateTime now = wxDateTime::UNow();
				/* if it's going to take more than a half second, show a progress dialog */
				if ((now - start).GetMilliseconds() > 250 && i < count / 2) {
					progressdialog = new wxProgressDialog(
						_("Load"), _("otidalroute"), count, this,
						wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_REMAINING_TIME);
				}
			}

			if (!strcmp(e->Value(), "Position")) {
				wxString name = wxString::FromUTF8(e->Attribute("Name"));
				double lat = AttributeDouble(e, "Latitude", NAN);
				double lon = AttributeDouble(e, "Longitude", NAN);

				for (std::vector<RouteMapPosition>::iterator it = Positions.begin();
					it != Positions.end(); it++) {
					if ((*it).Name == name) {
						static bool warnonce = true;
						if (warnonce) {
							warnonce = false;
							wxMessageDialog mdlg(this, _("File contains duplicate position name, discarding\n"),
								_("otidalroute"), wxOK | wxICON_WARNING);
							mdlg.ShowModal();
						}

						goto skipadd;
					}
				}
				AddPosition(lat, lon, name);

			skipadd:;

			}

			else				

				if (!strcmp(e->Value(), "TidalRoute")) {
					TidalRoute tr;
					m_pos.clear();
					wxString nm = wxString::FromUTF8(e->Attribute("Name"));
					wxString tp = wxString::FromUTF8(e->Attribute("Type"));
					wxString st = wxString::FromUTF8(e->Attribute("Start"));
					wxString en = wxString::FromUTF8(e->Attribute("End"));
					wxString tm = wxString::FromUTF8(e->Attribute("Time"));
					wxString tms = wxString::FromUTF8(e->Attribute("StartTime"));
					wxString tme = wxString::FromUTF8(e->Attribute("EndTime"));
					wxString dn = wxString::FromUTF8(e->Attribute("Distance"));
					tr.Name = nm;
					tr.Type = tp;
					tr.Start = st;
					tr.End = en;
					tr.Time = tm;
					tr.StartTime = tms;
					tr.EndTime = tme;
					tr.Distance = dn;

					for (TiXmlElement* f = e->FirstChildElement(); f; f = f->NextSiblingElement()) {
						
						if (!strcmp(f->Value(), "Route")) {
							pos.name = wxString::FromUTF8(f->Attribute("Waypoint"));
							pos.lat = wxString::FromUTF8(f->Attribute("Latitude"));
							pos.lon = wxString::FromUTF8(f->Attribute("Longitude"));
							pos.time = wxString::FromUTF8(f->Attribute("ETD"));
							pos.guid = wxString::FromUTF8(f->Attribute("GUID"));
							pos.CTS = wxString::FromUTF8(f->Attribute("CTS"));
							pos.distTo = wxString::FromUTF8(f->Attribute("Dist"));

							m_pos.push_back(pos);
							
						}  																
					}
					tr.m_positionslist = m_pos;
					AddTidalRoute(tr);
				}
		

			else
				FAIL(_("Unrecognized xml node"));
		}
	}

	delete progressdialog;
	return true;
failed:

	if (reportfailure) {
		wxMessageDialog mdlg(this, error, _("otidalroute"), wxOK | wxICON_ERROR);
		mdlg.ShowModal();
	}
	return false;
	
}

void otidalrouteUIDialog::SaveXML(wxString filename)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "");
	doc.LinkEndChild(decl);

	TiXmlElement * root = new TiXmlElement("OpenCPNotidalrouteConfiguration");
	doc.LinkEndChild(root);

	char version[24];
	sprintf(version, "%d.%d", PLUGIN_VERSION_MAJOR, PLUGIN_VERSION_MINOR);
	root->SetAttribute("version", version);
	root->SetAttribute("creator", "Opencpn otidalroute plugin");

	for (std::vector<RouteMapPosition>::iterator it = Positions.begin();
		it != Positions.end(); it++) { 
		TiXmlElement *c = new TiXmlElement("Position");

		c->SetAttribute("Name", (*it).Name.mb_str());
		c->SetAttribute("Latitude", wxString::Format(_T("%.5f"), (*it).lat).mb_str());
		c->SetAttribute("Longitude", wxString::Format(_T("%.5f"), (*it).lon).mb_str());

		root->LinkEndChild(c);
	}
	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {

			TiXmlElement * TidalRoute = new TiXmlElement("TidalRoute");
			TidalRoute->SetAttribute("Name", (*it).Name);
			TidalRoute->SetAttribute("Type", (*it).Type);
			TidalRoute->SetAttribute("Start", (*it).Start);
			TidalRoute->SetAttribute("End", (*it).End);
			TidalRoute->SetAttribute("Time", (*it).Time);
			TidalRoute->SetAttribute("StartTime", (*it).StartTime);
			TidalRoute->SetAttribute("EndTime", (*it).EndTime);
			TidalRoute->SetAttribute("Distance", (*it).Distance);

			for (std::list<Position>::iterator itp = (*it).m_positionslist.begin();
				itp != (*it).m_positionslist.end(); itp++) {
				TiXmlElement *cp = new TiXmlElement("Route");
				cp->SetAttribute("Waypoint", (*itp).name);
				cp->SetAttribute("Latitude", (*itp).lat);
				cp->SetAttribute("Longitude", (*itp).lon);
				cp->SetAttribute("ETD", (*itp).time);
				cp->SetAttribute("GUID", (*itp).guid);
				cp->SetAttribute("CTS", (*itp).CTS);
				cp->SetAttribute("Dist", (*itp).distTo);
		
				TidalRoute->LinkEndChild(cp);
			}

		root->LinkEndChild(TidalRoute);
	}
	/*
	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {
		TiXmlElement *config = new TiXmlElement("Configuration");

		config->SetAttribute("Start", (*it).Start.mb_str());
		config->SetAttribute("End", (*it).End.mb_str());

		root->LinkEndChild(config);
	}
	*/
	if (!doc.SaveFile(filename.mb_str())) {
		wxMessageDialog mdlg(this, _("Failed to save xml file: ") + filename,
			_("otidalroute"), wxOK | wxICON_ERROR);
		mdlg.ShowModal();
	}
}

void otidalrouteUIDialog::OnDeletePosition(wxCommandEvent& event)
{
	long index = m_lPositions->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (index < 0)
		return;

	wxListItem itm;
	itm.SetId(index);
	itm.SetColumn(0);
	itm.SetMask(wxLIST_MASK_TEXT);
	m_lPositions->GetItem(itm);
	wxString name = itm.GetText();
	for (std::vector<RouteMapPosition>::iterator it = Positions.begin();
		it != Positions.end(); it++){
		if ((*it).Name == name) {
			Positions.erase(it);
			if (m_textCtrl3->GetValue() == name){
				m_textCtrl3->Clear();
			}
			if (m_textCtrl4->GetValue() == name){
				m_textCtrl4->Clear();
			}
			break;
		}
	}
	m_ConfigurationDialog.RemoveSource(name);
	m_lPositions->DeleteItem(index);

}

void otidalrouteUIDialog::OnDeleteAllPositions(wxCommandEvent& event)
{
	wxMessageDialog mdlg(this, _("Delete all the positions?\n"),
		_("Delete All Positions"), wxYES | wxNO | wxICON_WARNING);
	if (mdlg.ShowModal() == wxID_YES) {
		Positions.clear();
		m_ConfigurationDialog.ClearSources();
		m_lPositions->DeleteAllItems();
		m_textCtrl3->Clear();
		m_textCtrl4->Clear();
	}
}

void otidalrouteUIDialog::UpdateColumns()
{/*
	m_lTidalRoutes->DeleteAllColumns();

	for (int i = 0; i < NUM_COLS; i++) {

		columns[i] = m_lTidalRoutes->GetColumnCount();
		wxString name = column_names[i];

		if (i == STARTTIME || i == ENDTIME) {
			name += _T(" (");

			name += _("local");

			name += _T(")");
		}

		m_lTidalRoutes->InsertColumn(columns[i], name);

	}
	int p = 0;
	for (std::list<TidalRoute>::iterator it = m_TidalRoutes.begin();
		it != m_TidalRoutes.end(); it++) {
	
		wxListItem item;
		long index = m_lTidalRoutes->InsertItem(p, item);
		m_lTidalRoutes->SetItem(index, START, (it)->Start);
		m_lTidalRoutes->SetColumnWidth(START, wxLIST_AUTOSIZE);
		//m_lTidalRoutes->SetItemPtrData(i, (wxUIntPtr)&it); // somehow this gets lost
		//(it)->Update(this); // update utc/local switch to strings of start/end time
		p++;
	}
	*/
	//OnWeatherRouteSelected(); // update utc/local switch if configuration dialog is visible
}
enum { WIND, CURRENT };

