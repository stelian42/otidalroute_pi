/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  otidalroute Plugin
 * Author:   David Register, Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 */


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
  #include <wx/glcanvas.h>
#endif //precompiled headers

#include <wx/fileconf.h>
#include <wx/stdpaths.h>

#include "ocpn_plugin.h"

#include "otidalroute_pi.h"
#include "otidalrouteUIDialogBase.h"
#include "otidalrouteUIDialog.h"

wxString myVColour[] = {_T("rgb(127, 0, 255)"), _T("rgb(0, 166, 80)"),  _T("rgb(253, 184, 19)"),  _T("rgb(248, 128, 64)"),  _T("rgb(248, 0, 0)")};

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new otidalroute_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}



//---------------------------------------------------------------------------------------------------------
//
//    otidalroute PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"


//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

otidalroute_pi::otidalroute_pi(void *ppimgr)
      :opencpn_plugin_110(ppimgr)
{
      // Create the PlugIn icons
      initialize_images();
      m_bShowotidalroute = false;
	  
     

}

otidalroute_pi::~otidalroute_pi(void)
{
	  delete _img_otidalroute_pi;
      delete _img_otidalroute;
}

int otidalroute_pi::Init(void)
{
      AddLocaleCatalog( _T("opencpn-otidalroute_pi") );

      // Set some default private member parameters
      m_otidalroute_dialog_x = 0;
      m_otidalroute_dialog_y = 0;
      m_otidalroute_dialog_sx = 200;
      m_otidalroute_dialog_sy = 400;
      m_potidalrouteDialog = NULL;
      m_potidalrouteOverlayFactory = NULL;
      m_botidalrouteShowIcon = true;

      ::wxDisplaySize(&m_display_width, &m_display_height);

      

	  
	  m_pconfig = GetOCPNConfigObject();
	 


      //    And load the configuration items
      LoadConfig();



      // Get a pointer to the opencpn display canvas, to use as a parent for the otidalroute dialog
      m_parent_window = GetOCPNCanvasWindow();

	

      //    This PlugIn needs a toolbar icon, so request its insertion if enabled locally
      if(m_botidalrouteShowIcon)
          m_leftclick_tool_id = InsertPlugInTool(_T(""), _img_otidalroute, _img_otidalroute, wxITEM_CHECK,
                                                 _("otidalroute"), _T(""), NULL,
                                                 otidalroute_TOOL_POSITION, 0, this);	  
	 


      return (WANTS_OVERLAY_CALLBACK |
              WANTS_OPENGL_OVERLAY_CALLBACK |
              WANTS_TOOLBAR_CALLBACK    |
              INSTALLS_TOOLBAR_TOOL     |
              WANTS_CONFIG              |
              WANTS_PREFERENCES         |
              WANTS_PLUGIN_MESSAGING
            );
}

bool otidalroute_pi::DeInit(void)
{
    if(m_potidalrouteDialog) {
        m_potidalrouteDialog->Close();
        delete m_potidalrouteDialog;
        m_potidalrouteDialog = NULL;
    }

    delete m_potidalrouteOverlayFactory;
    m_potidalrouteOverlayFactory = NULL;

    return true;
}

int otidalroute_pi::GetAPIVersionMajor()
{
      return MY_API_VERSION_MAJOR;
}

int otidalroute_pi::GetAPIVersionMinor()
{
      return MY_API_VERSION_MINOR;
}

int otidalroute_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int otidalroute_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxBitmap *otidalroute_pi::GetPlugInBitmap()
{
      return _img_otidalroute_pi;
}

wxString otidalroute_pi::GetCommonName()
{
      return _T("otidalroute");
}


wxString otidalroute_pi::GetShortDescription()
{
      return _("otidalroute PlugIn for OpenCPN");
}


wxString otidalroute_pi::GetLongDescription()
{
      return _("otidalroute PlugIn for OpenCPN\nCalculates EP positions/Courses using Grib tidal current data.\n\n\
			   ");
}

void otidalroute_pi::SetDefaults(void)
{
}


int otidalroute_pi::GetToolbarToolCount(void)
{
      return 1;
}

void otidalroute_pi::ShowPreferencesDialog( wxWindow* parent )
{
    otidalroutePreferencesDialog *Pref = new otidalroutePreferencesDialog(parent);

    Pref->m_cbUseRate->SetValue(m_bCopyUseRate);
    Pref->m_cbUseDirection->SetValue(m_bCopyUseDirection);
	Pref->m_cbFillColour->SetValue(m_botidalrouteUseHiDef);

	wxColour myC0 = wxColour(myVColour[0]);
	Pref->myColourPicker0->SetColour(myC0);
    
	wxColour myC1 = wxColour(myVColour[1]);
	Pref->myColourPicker1->SetColour(myC1);

	wxColour myC2 = wxColour(myVColour[2]);
	Pref->myColourPicker2->SetColour(myC2);

	wxColour myC3 = wxColour(myVColour[3]);
	Pref->myColourPicker3->SetColour(myC3);

	wxColour myC4 = wxColour(myVColour[4]);
	Pref->myColourPicker4->SetColour(myC4);


 if( Pref->ShowModal() == wxID_OK ) {

	 //bool copyFillColour = true;
    
	 myVColour[0] = Pref->myColourPicker0->GetColour().GetAsString();
	 myVColour[1] = Pref->myColourPicker1->GetColour().GetAsString();
	 myVColour[2] = Pref->myColourPicker2->GetColour().GetAsString();
	 myVColour[3] = Pref->myColourPicker3->GetColour().GetAsString();
	 myVColour[4] = Pref->myColourPicker4->GetColour().GetAsString();


     bool copyrate = Pref->m_cbUseRate->GetValue();
     bool copydirection = Pref->m_cbUseDirection->GetValue();
	 bool FillColour = Pref->m_cbFillColour->GetValue();

		 if (m_botidalrouteUseHiDef != FillColour){		 
			 m_botidalrouteUseHiDef = FillColour;
		 }
	 
        if( m_bCopyUseRate != copyrate || m_bCopyUseDirection != copydirection||  m_botidalrouteUseHiDef != FillColour ) {
             m_bCopyUseRate = copyrate;
             m_bCopyUseDirection = copydirection;   
			 m_botidalrouteUseHiDef = FillColour;
         }

		
         if(m_potidalrouteDialog )
		 {	
			 m_potidalrouteDialog->OpenFile(true);
			 m_potidalrouteDialog->m_FolderSelected = m_CopyFolderSelected;
			 m_potidalrouteDialog->m_IntervalSelected = m_CopyIntervalSelected;

			 m_potidalrouteDialog->m_bUseRate = m_bCopyUseRate;
			 m_potidalrouteDialog->m_bUseDirection = m_bCopyUseDirection;	
			 m_potidalrouteDialog->m_bUseFillColour = m_botidalrouteUseHiDef;

			 m_potidalrouteDialog->myUseColour[0] = myVColour[0];
 			 m_potidalrouteDialog->myUseColour[1] = myVColour[1];
 			 m_potidalrouteDialog->myUseColour[2] = myVColour[2];
 			 m_potidalrouteDialog->myUseColour[3] = myVColour[3];
 			 m_potidalrouteDialog->myUseColour[4] = myVColour[4];			 
		 }

		 if (m_potidalrouteOverlayFactory)
		 {			 
			 m_potidalrouteOverlayFactory->m_bShowRate = m_bCopyUseRate;
			 m_potidalrouteOverlayFactory->m_bShowDirection = m_bCopyUseDirection;
			 m_potidalrouteOverlayFactory->m_bShowFillColour = m_botidalrouteUseHiDef;
		 }

         SaveConfig();
		 
		 RequestRefresh(m_parent_window); // refresh main window
     }
	
}


void otidalroute_pi::OnToolbarToolCallback(int id)
{
    
	if(!m_potidalrouteDialog)
    {
		       		
		m_potidalrouteDialog = new otidalrouteUIDialog(m_parent_window, this);
        wxPoint p = wxPoint(m_otidalroute_dialog_x, m_otidalroute_dialog_y);
        m_potidalrouteDialog->Move(0,0);        // workaround for gtk autocentre dialog behavior
        m_potidalrouteDialog->Move(p);

        // Create the drawing factory
        m_potidalrouteOverlayFactory = new otidalrouteOverlayFactory( *m_potidalrouteDialog );
        m_potidalrouteOverlayFactory->SetParentSize( m_display_width, m_display_height);		
        
    }	
	
	SendPluginMessage(wxString(_T("GRIB_TIMELINE_REQUEST")), _T(""));


      // Qualify the otidalroute dialog position
            bool b_reset_pos = false;

#ifdef __WXMSW__
        //  Support MultiMonitor setups which an allow negative window positions.
        //  If the requested window does not intersect any installed monitor,
        //  then default to simple primary monitor positioning.
            RECT frame_title_rect;
            frame_title_rect.left =   m_otidalroute_dialog_x;
            frame_title_rect.top =    m_otidalroute_dialog_y;
            frame_title_rect.right =  m_otidalroute_dialog_x + m_otidalroute_dialog_sx;
            frame_title_rect.bottom = m_otidalroute_dialog_y + 30;


            if(NULL == MonitorFromRect(&frame_title_rect, MONITOR_DEFAULTTONULL))
                  b_reset_pos = true;
#else
       //    Make sure drag bar (title bar) of window on Client Area of screen, with a little slop...
            wxRect window_title_rect;                    // conservative estimate
            window_title_rect.x = m_otidalroute_dialog_x;
            window_title_rect.y = m_otidalroute_dialog_y;
            window_title_rect.width = m_otidalroute_dialog_sx;
            window_title_rect.height = 30;

            wxRect ClientRect = wxGetClientDisplayRect();
            ClientRect.Deflate(60, 60);      // Prevent the new window from being too close to the edge
            if(!ClientRect.Intersects(window_title_rect))
                  b_reset_pos = true;

#endif
			/*
            if(b_reset_pos)
            {
                  m_otidalroute_dialog_x = 20;
                  m_otidalroute_dialog_y = 170;
                  m_otidalroute_dialog_sx = 300;
                  m_otidalroute_dialog_sy = 540;
            }
			*/

      //Toggle otidalroute overlay display
      m_bShowotidalroute = !m_bShowotidalroute;

      //    Toggle dialog?
      if(m_bShowotidalroute) {
          m_potidalrouteDialog->Show();
      } else {
          m_potidalrouteDialog->Hide();         
          }

      // Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
      // to actual status to ensure correct status upon toolbar rebuild
      SetToolbarItemState( m_leftclick_tool_id, m_bShowotidalroute );
	  //SetCanvasContextMenuItemViz(m_position_menu_id, true);

      RequestRefresh(m_parent_window); // refresh main window
}

void otidalroute_pi::OnotidalrouteDialogClose()
{
    m_bShowotidalroute = false;
    //SetToolbarItemState( m_leftclick_tool_id, m_bShowotidalroute );
	//SetCanvasContextMenuItemViz(m_position_menu_id, m_bShowotidalroute);

    m_potidalrouteDialog->Hide();

    SaveConfig();

    RequestRefresh(m_parent_window); // refresh main window

}


void otidalroute_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{
	if (message_id == _T("GRIB_TIMELINE"))
	{
		wxJSONReader r;
		wxJSONValue v;
		r.Parse(message_body, &v);

		wxDateTime time, adjTime;
		time.Set
			(v[_T("Day")].AsInt(), (wxDateTime::Month)v[_T("Month")].AsInt(), v[_T("Year")].AsInt(),
			v[_T("Hour")].AsInt(), v[_T("Minute")].AsInt(), v[_T("Second")].AsInt());

		wxTimeSpan correctTCTime = wxTimeSpan::Minutes(30);
		adjTime = time - correctTCTime;

		wxString dt;
		dt = adjTime.Format(_T("%Y-%m-%d  %H:%M "));

		if (m_potidalrouteDialog){
			m_potidalrouteDialog->m_GribTimelineTime = time.ToUTC();
			m_potidalrouteDialog->m_textCtrl1->SetValue(dt);
		}
	}
	if (message_id == _T("GRIB_TIMELINE_RECORD"))
	{
		wxJSONReader r;
		wxJSONValue v;
		r.Parse(message_body, &v);

		static bool shown_warnings;
		if (!shown_warnings) {
			shown_warnings = true;

			int grib_version_major = v[_T("GribVersionMajor")].AsInt();
			int grib_version_minor = v[_T("GribVersionMinor")].AsInt();

			int grib_version = 1000 * grib_version_major + grib_version_minor;
			int grib_min = 1000 * GRIB_MIN_MAJOR + GRIB_MIN_MINOR;
			int grib_max = 1000 * GRIB_MAX_MAJOR + GRIB_MAX_MINOR;

			if (grib_version < grib_min || grib_version > grib_max) {
				wxMessageDialog mdlg(m_parent_window,
					_("Grib plugin version not supported.")
					+ _T("\n\n") +
					wxString::Format(_("Use versions %d.%d to %d.%d"), GRIB_MIN_MAJOR, GRIB_MIN_MINOR, GRIB_MAX_MAJOR, GRIB_MAX_MINOR),
					_("Weather Routing"), wxOK | wxICON_WARNING);
				mdlg.ShowModal();
			}
		}

		wxString sptr = v[_T("TimelineSetPtr")].AsString();
		wxCharBuffer bptr = sptr.To8BitData();
		const char* ptr = bptr.data();

		GribRecordSet *gptr;
		sscanf(ptr, "%p", &gptr);
		
		double dir, spd;
		
		m_bGribValid = GribCurrent(gptr, m_grib_lat, m_grib_lon, dir, spd);
		
		m_tr_spd = spd;
		m_tr_dir = dir;

	}
}

bool otidalroute_pi::GribWind(GribRecordSet *grib, double lat, double lon,
	double &WG, double &VWG)
{
	if (!grib)
		return false;

	if (!GribRecord::getInterpolatedValues(VWG, WG,
		grib->m_GribRecordPtrArray[Idx_WIND_VX],
		grib->m_GribRecordPtrArray[Idx_WIND_VY], lon, lat))
		return false;

	VWG *= 3.6 / 1.852; // knots
	return true;
}




wxString otidalroute_pi::StandardPath()
{
	wxStandardPathsBase& std_path = wxStandardPathsBase::Get();
	wxString s = wxFileName::GetPathSeparator();
#ifdef __WXMSW__
	wxString stdPath = std_path.GetConfigDir();
#endif
#ifdef __WXGTK__
	wxString stdPath = std_path.GetUserDataDir();
#endif
#ifdef __WXOSX__
	wxString stdPath = (std_path.GetUserConfigDir() + s + _T("opencpn"));   // should be ~/Library/Preferences/opencpn
#endif

	return stdPath + wxFileName::GetPathSeparator() +
		_T("plugins") + wxFileName::GetPathSeparator() +
		_T("otidalroute") + wxFileName::GetPathSeparator();

	stdPath += s + _T("plugins");
	if (!wxDirExists(stdPath))
		wxMkdir(stdPath);

	stdPath += s + _T("otidalroute");

#ifdef __WXOSX__
	// Compatibility with pre-OCPN-4.2; move config dir to
	// ~/Library/Preferences/opencpn if it exists
	wxString oldPath = (std_path.GetUserConfigDir() + s + _T("plugins") + s + _T("weather_routing"));
	if (wxDirExists(oldPath) && !wxDirExists(stdPath)) {
		wxLogMessage("weather_routing_pi: moving config dir %s to %s", oldPath, stdPath);
		wxRenameFile(oldPath, stdPath);
	}
#endif

	if (!wxDirExists(stdPath))
		wxMkdir(stdPath);

	stdPath += s;
	return stdPath;
}

bool otidalroute_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp)
{
	
	if(!m_potidalrouteDialog ||
       !m_potidalrouteDialog->IsShown() ||
       !m_potidalrouteOverlayFactory)
        return false;

    m_potidalrouteDialog->SetViewPort( vp );
    m_potidalrouteOverlayFactory->RenderotidalrouteOverlay ( dc, vp );
    return true;
}

bool otidalroute_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
	

	if(!m_potidalrouteDialog ||
       !m_potidalrouteDialog->IsShown() ||
       !m_potidalrouteOverlayFactory)
        return false;

    m_potidalrouteDialog->SetViewPort( vp );
    m_potidalrouteOverlayFactory->RenderGLotidalrouteOverlay ( pcontext, vp );
    return true;
}

void otidalroute_pi::SetCursorLatLon(double lat, double lon)
{
    if(m_potidalrouteDialog)
        m_potidalrouteDialog->SetCursorLatLon(lat, lon);

	m_cursor_lat = lat;
	m_cursor_lon = lon;

}

bool otidalroute_pi::LoadConfig(void)
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

    if(!pConf)
        return false;

    pConf->SetPath ( _T( "/PlugIns/otidalroute" ) );

	m_bCopyUseRate = pConf->Read ( _T( "otidalrouteUseRate" ),1);
    m_bCopyUseDirection = pConf->Read ( _T( "otidalrouteUseDirection" ), 1);
	m_botidalrouteUseHiDef = pConf->Read ( _T( "otidalrouteUseFillColour" ), 1);

	//m_CopyFolderSelected = pConf->Read ( _T( "otidalrouteFolder" ));
	//if (m_CopyFolderSelected == wxEmptyString){

	 // wxString g_SData_Locn = *GetpSharedDataLocation();

      // Establish location of Tide and Current data
     // pTC_Dir = new wxString(_T("tcdata"));
     // pTC_Dir->Prepend(g_SData_Locn);

	 // m_CopyFolderSelected = *pTC_Dir;	  
	//}

	//m_CopyIntervalSelected = pConf->Read ( _T ( "otidalrouteInterval"), 20L);

    m_otidalroute_dialog_sx = pConf->Read ( _T( "otidalrouteDialogSizeX" ), 300L );
    m_otidalroute_dialog_sy = pConf->Read ( _T( "otidalrouteDialogSizeY" ), 540L );
    m_otidalroute_dialog_x =  pConf->Read ( _T( "otidalrouteDialogPosX" ), 20L );
    m_otidalroute_dialog_y =  pConf->Read ( _T( "otidalrouteDialogPosY" ), 170L );
	
    pConf->Read( _T("VColour0"), &myVColour[0], myVColour[0] );
    pConf->Read( _T("VColour1"), &myVColour[1], myVColour[1] );
	pConf->Read( _T("VColour2"), &myVColour[2], myVColour[2] );
	pConf->Read( _T("VColour3"), &myVColour[3], myVColour[3] );
	pConf->Read( _T("VColour4"), &myVColour[4], myVColour[4] );
	
    return true;
}

bool otidalroute_pi::SaveConfig(void)
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

    if(!pConf)
        return false;

    pConf->SetPath ( _T( "/PlugIns/otidalroute" ) );
    pConf->Write ( _T( "otidalrouteUseRate" ), m_bCopyUseRate );
    pConf->Write ( _T( "otidalrouteUseDirection" ), m_bCopyUseDirection );
	pConf->Write ( _T( "otidalrouteUseFillColour" ), m_botidalrouteUseHiDef );

	//pConf->Write ( _T( "otidalrouteFolder" ), m_CopyFolderSelected); 
	//pConf->Write ( _T( "otidalrouteInterval" ), m_CopyIntervalSelected);

    pConf->Write ( _T( "otidalrouteDialogSizeX" ),  m_otidalroute_dialog_sx );
    pConf->Write ( _T( "otidalrouteDialogSizeY" ),  m_otidalroute_dialog_sy );
    pConf->Write ( _T( "otidalrouteDialogPosX" ),   m_otidalroute_dialog_x );
    pConf->Write ( _T( "otidalrouteDialogPosY" ),   m_otidalroute_dialog_y );

	pConf->Write( _T("VColour0"), myVColour[0] );
	pConf->Write( _T("VColour1"), myVColour[1] );
	pConf->Write( _T("VColour2"), myVColour[2] );
	pConf->Write( _T("VColour3"), myVColour[3] );
	pConf->Write( _T("VColour4"), myVColour[4] );


    return true;
}

void otidalroute_pi::SetColorScheme(PI_ColorScheme cs)
{
    DimeWindow(m_potidalrouteDialog);
}



