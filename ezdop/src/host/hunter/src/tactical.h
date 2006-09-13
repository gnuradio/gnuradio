/*
 Copyright 2006 Johnathan Corgan.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License version 2
 as published by the Free Software Foundation.
 
 This software is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with GNU Radio; see the file COPYING.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street,
 Boston, MA 02110-1301, USA.
*/

#ifndef __TACTICAL_H__
#define __TACTICAL_H__

// wxWidgets includes
#include <wx/panel.h>

enum Orientation {
    TrackUp,
    NorthUp
};

class TacticalPanel : public wxPanel
{
public:
    TacticalPanel(wxWindow *parent);

    // Event handlers
    void OnPaint(wxPaintEvent &event);
    void OnSize(wxSizeEvent &event);

    // Configuration
    void SetOrientation(Orientation orientation) { m_orientation = orientation; Refresh(); }
    void SetDisplayDoppler(bool display) { m_display_doppler = display; Refresh(); }
    void SetDisplayKnown(bool display) { m_display_known = display; Refresh(); }
    void SetDisplayEstimated(bool display) { m_display_estimated = display; Refresh(); }
    
    // State updates
    void SetHeading(float heading) { m_heading = heading; Refresh(); }
    void SetDopplerBearing(float bearing) { m_doppler_bearing = bearing; Refresh(); }
    void SetEstimatedBearing(float bearing) { m_estimated_bearing = bearing; Refresh(); }
    void SetActualBearing(float bearing) { m_actual_bearing = bearing; Refresh(); }
    
private:
    Orientation m_orientation;
    bool  m_display_doppler;
    bool  m_display_known;
    bool  m_display_estimated;
    
    float m_heading;
    float m_doppler_bearing;
    float m_estimated_bearing;
    float m_actual_bearing;
    
    void drawPanel(wxDC &dc);

    // Window size derived parameters
    wxPoint m_center;
    int m_width;
    int m_height;
    int m_radius;
    
    DECLARE_EVENT_TABLE();
};

#endif // __TACTICAL_H__
