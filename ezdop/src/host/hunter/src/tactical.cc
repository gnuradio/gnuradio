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
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA.
*/

// Application level includes
#include "tactical.h"

// wxWidgets includes
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/log.h>

// System level includes
#include <cmath>

// Event table for TacticalPanel
BEGIN_EVENT_TABLE(TacticalPanel, wxPanel)
    EVT_PAINT(TacticalPanel::OnPaint)
    EVT_SIZE(TacticalPanel::OnSize)
END_EVENT_TABLE()

TacticalPanel::TacticalPanel(wxWindow *parent) :
wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
    m_orientation = TrackUp;
    m_display_doppler = false;
    m_display_known = false;
    m_display_estimated = false;
    
	m_heading = 0.0;
	m_doppler_bearing = -1.0;
    m_estimated_bearing = -1.0;
    m_actual_bearing = -1.0;
    
	SetBackgroundColour(*wxBLACK);
}

void TacticalPanel::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);
	drawPanel(dc);
}

void TacticalPanel::drawPanel(wxDC &dc)
{
    float radians;
    float brg = 0;
    
    // Draw circle
	dc.SetPen(wxPen(*wxRED, 2, wxSOLID));
	dc.SetBrush(wxBrush(*wxBLACK, wxTRANSPARENT));
    dc.DrawCircle(m_center, m_radius);

    // Calculate end of doppler bearing line
    // Doppler bearings are relative and must be adjusted for north up display
    wxPoint doppler_tip = m_center;
    if (m_doppler_bearing >= 0.0) {
        brg = m_doppler_bearing;
        if (m_orientation == NorthUp) {
            brg += m_heading;
            if (brg >= 360.0)
                brg -= 360.0;
        }
    	radians = brg*M_PI/180.0;
        doppler_tip = wxPoint((int)(m_center.x+sin(radians)*m_radius*0.95),
    	              (int)(m_height-(m_center.y+cos(radians)*m_radius*0.95)));
    }
        
    // Calculate end of actual bearing line
    // Actual bearings are absolute and must be adjusted for track up display
    wxPoint actual_tip = m_center;
    if (m_actual_bearing >= 0.0) {
        brg = m_actual_bearing;
        if (m_orientation == TrackUp) {
            brg -= m_heading;
            if (brg < 0.0)
                brg += 360.0;
        }
        radians = brg*M_PI/180.0;
        actual_tip = wxPoint((int)(m_center.x+sin(radians)*m_radius*0.95),
    	             (int)(m_height-(m_center.y+cos(radians)*m_radius*0.95)));
    }

    // Calculate end of estimated bearing line
    // Estimated bearings are absolute and must be adjusted for track up display
    wxPoint estimated_tip = m_center;
    if (m_estimated_bearing >= 0.0) {
        brg = m_estimated_bearing;
        if (m_orientation == TrackUp) {
            brg -= m_heading;
            if (brg < 0.0)
                brg += 360.0;
        }
        radians = brg*M_PI/180.0;
        estimated_tip = wxPoint((int)(m_center.x+sin(radians)*m_radius*0.95),
    	             (int)(m_height-(m_center.y+cos(radians)*m_radius*0.95)));
    }

    if (m_display_known) {
    	dc.SetPen(wxPen(*wxBLUE, 10, wxSOLID));
    	dc.DrawLine(m_center, actual_tip);
    }
    
    if (m_display_estimated) {
    	dc.SetPen(wxPen(*wxWHITE, 10, wxSOLID));
        dc.DrawLine(m_center, estimated_tip);
	}

    if (m_display_doppler) {
    	dc.SetPen(wxPen(*wxGREEN, 10, wxSOLID));
    	dc.DrawLine(m_center, doppler_tip);
    }
}

void TacticalPanel::OnSize(wxSizeEvent &event)
{
    GetClientSize(&m_width, &m_height);
    m_center = wxPoint(m_width/2, m_height/2);

	// Circle at 95% of window size
    if (m_width > m_height)
        m_radius = m_height/2;
    else
        m_radius = m_width/2;
    m_radius = (int)(m_radius*0.95);
        
    Refresh();
}
