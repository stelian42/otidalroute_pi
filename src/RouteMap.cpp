/***************************************************************************
 *
 * Project:  OpenCPN Weather Routing plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2016 by Sean D'Epagnier                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

/* generate a datastructure which contains positions for
   isochron line segments which describe the position of the boat at a given time..

   Starting at a given location, propagate outwards in all directions.
   the outward propagation is guarenteed a closed region, and circular linked
   lists are used. If the route comes upon a boundary or reason to stop
   searching, then the point is flagged so that it is not propagated any further.

   To merge regions requires virtually the same algorithm for descrambling
   (normalizing) a single region.

   To normalize a region means that no two line segments intersect.

   For each segment go through and see if it intersects
   with any other line segment.  When it does the old route will follow
   the correct direction of the intersection on the intersected route,
   and the new region generated will be recursively normalized and then
   merged.

   A positive intersection comes in from the right.  Negative intersections
   signal negative regions.

   For each segment in a given route
   If the intersection occurs with the route and itself
   a new region is created with the same sign as the intersection
   and added to the list of either positive or negative subregions
   otherwise if the intersection occurs on different routes
   the intersecting route is merged into this one,
   swapping their connections

   Once we reach the end of the route, we can declare that it is complete,
   so in turn recursively normalize each inner subroute.  The subregions
   with the same sign are inner routes.  Once these regions are all normalized,
   the remaining regions with a different sign are the perminent subregions.
   Any inner routes remaining with matching sign can be discarded.
   
   Any outer subregions are also normalized to give outer regions
   with both signs which can be appended to the incomming lists

   Any remaining routes should be tested to ensure they are outside this one,
   Any inside routes may be discarded leaving only inverted subroutes
*/

#include <wx/wx.h>

#include <stdlib.h>
#include <math.h>

#include "ocpn_plugin.h"
#include "GribRecordSet.h"

#include "RouteMap.h"
#include "otidalroute_pi.h"

#include "wx/jsonreader.h"
#include "wx/jsonwriter.h"

#define distance(X, Y) sqrt((X)*(X) + (Y)*(Y)) // much faster than hypot



enum {WIND, CURRENT};

static inline bool GribCurrent(GribRecordSet *grib, double lat, double lon,
                               double &C, double &VC)
{
    if(!grib)
        return false;

    if(!GribRecord::getInterpolatedValues(VC, C,
                                          grib->m_GribRecordPtrArray[Idx_SEACURRENT_VX],
                                          grib->m_GribRecordPtrArray[Idx_SEACURRENT_VY],
                                          lon, lat))
        return false;

    VC *= 3.6 / 1.852; // knots



    C += 180;
    if(C > 360)
        C -= 360;
    return true;
}

 