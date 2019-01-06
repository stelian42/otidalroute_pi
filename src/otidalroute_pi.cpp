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
      m_otidalroute_dialog_sx = 300;
      m_otidalroute_dialog_sy = 400;
      m_potidalrouteDialog = NULL;
      m_potidalrouteOverlayFactory = NULL;
      m_botidalrouteShowIcon = true;

      ::wxDisplaySize(&m_display_width, &m_display_height);

      

	  
	  m_pconfig = GetOCPNConfigObject();
	 


      //    And load the configuration items
      LoadConfig();

	 // wxMessageBox(wxString::Format(wxT("%i"), m_otidalroute_dialog_sx));

      // Get a pointer to the opencpn display canvas, to use as a parent for the otidalroute dialog
      m_parent_window = GetOCPNCanvasWindow();

	 
	  //    This PlugIn needs a toolbar icon, so request its insertion
	  if (m_botidalrouteShowIcon) {
#ifdef OTIDALROUTE_USE_SVG
		  m_leftclick_tool_id = InsertPlugInToolSVG(_T("oTidalRoute"), _svg_otidalroute, _svg_otidalroute_rollover, _svg_otidalroute_toggled,
			  wxITEM_CHECK, _("oTidalRoute"), _T(""), NULL, otidalroute_TOOL_POSITION, 0, this);
#else
		  m_leftclick_tool_id = InsertPlugInTool(_T(""), _img_otidalroute, _img_otidalroute, wxITEM_CHECK,
			  _("oTidalRoute"), _T(""), NULL,
			  otidalroute_TOOL_POSITION, 0, this);
#endif
	  }

	  m_potidalrouteDialog = NULL;

      return (WANTS_OVERLAY_CALLBACK |
              WANTS_OPENGL_OVERLAY_CALLBACK |
              WANTS_TOOLBAR_CALLBACK    |
              INSTALLS_TOOLBAR_TOOL     |
              WANTS_CONFIG              |              
              WANTS_PLUGIN_MESSAGING
            );
}

bool otidalroute_pi::DeInit(void)
{
   	
	if(NULL != m_potidalrouteDialog) {

		//Capture dialog position
		wxPoint p = m_potidalrouteDialog->GetPosition();
		wxRect r = m_potidalrouteDialog->GetRect();
		SetotidalrouteDialogX(p.x);
		SetotidalrouteDialogY(p.y);
		SetotidalrouteDialogSizeX(r.GetWidth());
		SetotidalrouteDialogSizeY(r.GetHeight());

        m_potidalrouteDialog->Close();
        delete m_potidalrouteDialog;
        m_potidalrouteDialog = NULL;

		m_bShowotidalroute = false;
		SetToolbarItemState(m_leftclick_tool_id, m_bShowotidalroute);
    }

    delete m_potidalrouteOverlayFactory;
    m_potidalrouteOverlayFactory = NULL;

	SaveConfig();

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


void otidalroute_pi::OnToolbarToolCallback(int id)
{
    
	if(NULL == m_potidalrouteDialog)
    {
		       		
		m_potidalrouteDialog = new otidalrouteUIDialog(m_parent_window, this);
        wxPoint p = wxPoint(m_otidalroute_dialog_x, m_otidalroute_dialog_y);
        m_potidalrouteDialog->Move(0,0);        // workaround for gtk autocentre dialog behavior
        m_potidalrouteDialog->Move(p);
		m_potidalrouteDialog->SetSize(m_otidalroute_dialog_sx, m_otidalroute_dialog_sy);


        // Create the drawing factory
        m_potidalrouteOverlayFactory = new otidalrouteOverlayFactory( *m_potidalrouteDialog );
        m_potidalrouteOverlayFactory->SetParentSize( m_display_width, m_display_height);		
        
    }	
	
	SendPluginMessage(wxString(_T("GRIB_TIMELINE_REQUEST")), _T(""));




      //Toggle otidalroute overlay display
      m_bShowotidalroute = !m_bShowotidalroute;

      //    Toggle dialog?
      if(m_bShowotidalroute) {
		  m_potidalrouteDialog->Move(wxPoint(m_otidalroute_dialog_x, m_otidalroute_dialog_y));
		  m_potidalrouteDialog->SetSize(m_otidalroute_dialog_sx, m_otidalroute_dialog_sy);
          m_potidalrouteDialog->Show();
      } else {
          m_potidalrouteDialog->Hide();         
          }

      // Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
      // to actual status to ensure correct status upon toolbar rebuild
      SetToolbarItemState( m_leftclick_tool_id, m_bShowotidalroute );
	  //SetCanvasContextMenuItemViz(m_position_menu_id, true);

	  //Capture dialog position
	  wxPoint p = m_potidalrouteDialog->GetPosition();
	  wxRect r = m_potidalrouteDialog->GetRect();
	  SetotidalrouteDialogX(p.x);
	  SetotidalrouteDialogY(p.y);
	  SetotidalrouteDialogSizeX(r.GetWidth());
	  SetotidalrouteDialogSizeY(r.GetHeight());

      RequestRefresh(m_parent_window); // refresh main window
}

void otidalroute_pi::OnotidalrouteDialogClose()
{

	m_bShowotidalroute = false;
	SetToolbarItemState(m_leftclick_tool_id, m_bShowotidalroute);
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

    pConf->SetPath ( _T( "/Settings/otidalroute" ) );
	pConf->Read(_T("ShowotidalrouteIcon"), &m_botidalrouteShowIcon, 1);
	

    m_otidalroute_dialog_sx = pConf->Read ( _T( "otidalrouteDialogSizeX" ), 300L );
    m_otidalroute_dialog_sy = pConf->Read ( _T( "otidalrouteDialogSizeY" ), 540L );
#ifdef __WXOSX__
	m_otidalroute_dialog_sy = pConf->Read(_T("otidalrouteDialogSizeY"), 540L);
#else
	m_otidalroute_dialog_sy = pConf->Read(_T("otidalrouteDialogSizeY"), 540L);
#endif
	
	m_otidalroute_dialog_x =  pConf->Read ( _T( "otidalrouteDialogPosX" ), 20L );
    m_otidalroute_dialog_y =  pConf->Read ( _T( "otidalrouteDialogPosY" ), 170L );
	
  
    return true;
}

bool otidalroute_pi::SaveConfig(void)
{
    wxFileConfig *pConf = (wxFileConfig *)m_pconfig;



    if(!pConf)
        return false;

    pConf->SetPath ( _T( "/Settings/otidalroute" ) );
	pConf->Write(_T("ShowotidalrouteIcon"), m_botidalrouteShowIcon);
    
    pConf->Write ( _T( "otidalrouteDialogSizeX" ),  m_otidalroute_dialog_sx );
    pConf->Write ( _T( "otidalrouteDialogSizeY" ),  m_otidalroute_dialog_sy );
    pConf->Write ( _T( "otidalrouteDialogPosX" ),   m_otidalroute_dialog_x );
    pConf->Write ( _T( "otidalrouteDialogPosY" ),   m_otidalroute_dialog_y );

    return true;
}

void otidalroute_pi::SetColorScheme(PI_ColorScheme cs)
{
    DimeWindow(m_potidalrouteDialog);
}



