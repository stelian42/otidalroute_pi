/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  otidalroute Plugin Friends
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */

#ifndef __otidalrouteUIDIALOG_H__
#define __otidalrouteUIDIALOG_H__

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/fileconf.h>
#include <wx/glcanvas.h>

#include "otidalrouteUIDialogBase.h"
#include "routeprop.h"
#include "NavFunc.h"

#include <wx/progdlg.h>
#include <list>
#include <vector>

#include "tcmgr.h"
#include "wx/dateevt.h"
#include "wx/stattext.h"
#include "ocpn_plugin.h"
#include "wx/dialog.h"
#include <wx/calctrl.h>
#include "wx/window.h"
#include "timectrl.h"
#include <wx/colordlg.h>
#include <wx/event.h>
#include "wx/jsonreader.h"
#include "wx/jsonwriter.h"
#include "GribRecordSet.h"
#include "tinyxml.h"
#include <wx/scrolwin.h>
#include <wx/datetime.h>
#include <wx/thread.h>
#include <wx/event.h>
#include <wx/listctrl.h>
#include "tableroutes.h"

/* XPM */
static const char *eye[] = {
	"20 20 7 1",
	". c none",
	"# c #000000",
	"a c #333333",
	"b c #666666",
	"c c #999999",
	"d c #cccccc",
	"e c #ffffff",
	"....................",
	"....................",
	"....................",
	"....................",
	".......######.......",
	".....#aabccb#a#.....",
	"....#deeeddeebcb#...",
	"..#aeeeec##aceaec#..",
	".#bedaeee####dbcec#.",
	"#aeedbdabc###bcceea#",
	".#bedad######abcec#.",
	"..#be#d######dadb#..",
	"...#abac####abba#...",
	".....##acbaca##.....",
	".......######.......",
	"....................",
	"....................",
	"....................",
	"....................",
	"...................." };


using namespace std;

#ifndef PI
#define PI        3.1415926535897931160E0      /* pi */
#endif

#if !defined(NAN)
static const long long lNaN = 0xfff8000000000000;
#define NAN (*(double*)&lNaN)
#endif

#define RT_RCDATA2           MAKEINTRESOURCE(999)

/* Maximum value that can be returned by the rand function. */
#define RAND_MAX 0x7fff


#define distance(X, Y) sqrt((X)*(X) + (Y)*(Y)) // much faster than hypot#define distance(X, Y) sqrt((X)*(X) + (Y)*(Y)) // much faster than hypot

class otidalrouteOverlayFactory;
class PlugIn_ViewPort;
class PositionRecordSet;


class wxFileConfig;
class otidalroute_pi;
class wxGraphicsContext;
class routeprop;
class TableRoutes;
class ConfigurationDialog;
class NewPositionDialog;

class Position
{
public:
	//double latD, lonD;
    wxString lat, lon;
	wxString name;
	wxString guid;
	wxString time;
	wxString etd;
	wxString CTS;
	wxString distTo;
	wxString set;
	wxString rate;
};

struct RouteMapPosition {
	RouteMapPosition(wxString n, double lat0, double lon0)
		: Name(n), lat(lat0), lon(lon0) {}
public:
	wxString Name;
	double lat, lon;
};


struct Arrow
{
	wxDateTime m_dt;
	double m_dir;
	double m_force;
	double m_lat;
	double m_lon;
	double m_cts;
	double m_tforce;

};

struct TotalTideArrow
{
	double m_dir;
	double m_force;
};

class TidalRoute
{
public:
	void Update(Position *rpl, bool stateonly = false);

	bool Filtered;
	wxString Name, Type, Start, StartTime, End, EndTime, Time, Distance//,
		//AvgSpeed, MaxSpeed, AvgSpeedGround, MaxSpeedGround, AvgWind, MaxWind,
		//AvgCurrent, MaxCurrent, AvgSwell, MaxSwell, UpwindPercentage, PortStarboard,
		//Tacks, State
		;
	list<Position> m_positionslist;
	
};

static const wxString column_names[] = { _T(""), _("Start"), _("Start Time"),
_("End"), _("End Time"), _("Time"), _("Distance")//,
//_("Avg Speed"), _("Max Speed")//,
//_("Avg Speed Ground"), _("Max Speed Ground"),
//_("Avg Wind"), _("Max Wind"),
//_("Avg Current"), _("Max Current"),
//_("Avg Swell"), _("Max Swell"),
//_("Upwind Percentage"),
//_("Port Starboard"), _("Tacks"), _("State") 
};

struct RouteMapConfiguration
{
	RouteMapConfiguration() : StartLon(0), EndLon(0), grib_is_data_deficient(false) {} /* avoid waiting forever in update longitudes */
	bool Update();

	wxString Start, End;
	wxDateTime StartTime;

	double dt; /* time in seconds between propagations */

	double MaxDivertedCourse, MaxCourseAngle, MaxSearchAngle, MaxTrueWindKnots, MaxApparentWindKnots;
	double MaxSwellMeters, MaxLatitude, TackingTime, WindVSCurrent;

	bool AvoidCycloneTracks;
	int CycloneMonths, CycloneDays;

	bool UseGrib;
	bool AllowDataDeficient;
	double WindStrength; // wind speed multiplier

	bool DetectLand, DetectBoundary, Currents, InvertedRegions, Anchoring;

	double FromDegree, ToDegree, ByDegrees;

	/* computed values */
	std::list<double> DegreeSteps;
	double StartLat, StartLon, EndLat, EndLon;

	double StartEndBearing; /* calculated from start and end */
	bool positive_longitudes; /* longitudes are either 0 to 360 or -180 to 180,
							  this means the map cannot cross both 0 and 180 longitude.
							  To fully support this requires a lot more logic and would probably slow the algorithm
							  by about 8%.  Is it even useful?  */

	// parameters
	GribRecordSet *grib;
	wxDateTime time;
	bool grib_is_data_deficient, polar_failed, wind_data_failed;	
};



class otidalrouteUIDialog: public otidalrouteUIDialogBase {
public:

    otidalrouteUIDialog(wxWindow *parent, otidalroute_pi *ppi);
    ~otidalrouteUIDialog();

	enum { POSITION_NAME = 0, POSITION_LAT, POSITION_LON };

	enum {
		VISIBLE = 0, START, STARTTIME, END, ENDTIME, TIME, DISTANCE, NUM_COLS
		
		//AVGSPEED, MAXSPEED,
		//AVGSPEEDGROUND, MAXSPEEDGROUND,
		//AVGWIND, MAXWIND, AVGCURRENT, MAXCURRENT, AVGSWELL, MAXSWELL,
		//UPWIND_PERCENTAGE, PORT_STARBOARD, TACKS, NUM_COLS
	};

	long columns[NUM_COLS];

	void OpenFile( bool newestFile = false );
    
    void SetCursorLatLon( double lat, double lon );
    void SetFactoryOptions( bool set_val = false );

    void SetViewPort( PlugIn_ViewPort *vp );
	PlugIn_ViewPort *vp;

	//int round(double c);

	bool m_bUseRate;    
	bool m_bUseDirection; 
	bool m_bUseFillColour;

	wxString myUseColour[5];

	wxDateTime m_dtNow;
	double m_dInterval;

	bool onNext;
	bool onPrev;

    wxString m_FolderSelected;
	int m_IntervalSelected;
	
	time_t myCurrentTime; 

	wxString MakeDateTimeLabel(wxDateTime myDateTime);
	void OnInformation(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void ShowTable();
	void GetTable(wxString myRoute);
	void AddPosition(double lat, double lon);
	void AddPosition(double lat, double lon, wxString name);
	void OnNewPosition(wxCommandEvent& event);
	void AddTidalRoute(TidalRoute tr);
	void AddConfiguration(RouteMapConfiguration configuration);
	void OnListLabelEdit(wxListEvent& event);


	void Export();
	void RequestGrib(wxDateTime time);
	virtual void Lock() { routemutex.Lock(); }
	virtual void Unlock() { routemutex.Unlock(); }
	void OverGround(double B, double VB, double C, double VC, double &BG, double &VBG);
	bool OpenXML(wxString filename, bool reportfailure);
	void SaveXML(wxString filename);
	void OnDeletePosition(wxCommandEvent& event);
	void OnDeleteAllPositions(wxCommandEvent& event);
	void UpdateColumns();
	void AddConfiguration(ConfigurationDialog configuration);

	double AttributeDouble(TiXmlElement *e, const char *name, double def);
	vector<RouteMapPosition>Positions;
	wxString m_default_configuration_path;
	list<Arrow> m_arrowList;
	list<Arrow> m_cList;
	list<TotalTideArrow> m_totaltideList;
	list<TidalRoute> m_TidalRoutes;
	bool b_showTidalArrow;
	RouteProp* routetable;
	wxDateTime m_GribTimelineTime;
	ConfigurationDialog m_ConfigurationDialog;
	

protected:
	bool m_bNeedsGrib;


private:
	wxMutex routemutex;

    void OnClose( wxCloseEvent& event );
    void OnMove( wxMoveEvent& event );
    void OnSize( wxSizeEvent& event );

	void OnSummary(wxCommandEvent& event);
	void OnNewRoute(wxCommandEvent& event);
	void OnDeleteRoute(wxCommandEvent& event);
	//void OnDelete();
	void OnDeleteAllRoutes(wxCommandEvent& event);
    void SetInterval( wxCommandEvent& event );
	//list<Arrow> GetHourlyCurrent(int i0, int i1);
	list<Arrow> GetHourlyCurrent();
	void HourlyCTS(wxCommandEvent& event);
	void SingleCTS(wxCommandEvent& event);
	bool GetGribSpdDir(wxDateTime dt, double lat, double lon, double &spd, double &dir);

	list<Arrow> GetCTS_DMG(double &CTS);

	int GetRandomNumber(int range_min, int range_max);
	

    //    Data
    wxWindow *pParent;
	otidalroute_pi *pPlugIn;

    PlugIn_ViewPort  *m_vp;
 
    double m_cursor_lat, m_cursor_lon;
	wxString         g_SData_Locn;
	TCMgr           *ptcmgr;
	wxString        *pTC_Dir;

	int         m_corr_mins;
    wxString    m_stz;
    int         m_t_graphday_00_at_station;
    wxDateTime  m_graphday;
    int         m_plot_y_offset;

	bool isNowButton;
	wxTimeSpan  myTimeOfDay;
	bool btc_valid;

};


WX_DECLARE_LIST(RouteMapPosition, Positions);// establish class as list member

#endif

