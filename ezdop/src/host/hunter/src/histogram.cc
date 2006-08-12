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
#include "histogram.h"

// wxWidgets includes
#include <wx/log.h>
#include <wx/dc.h>
#include <wx/dcclient.h>

using namespace std;

ErrorHistogram::ErrorHistogram()
{
    Reset();
}

void ErrorHistogram::Reset()
{
    m_bins = vector<float>(360);
    m_mode = 0;
    m_modal_frequency = 0.0;
    m_isum = 0.0;
    m_qsum = 0.0;
    m_msum = 0.0;
    m_conc = 0.0;
    m_mean = 0.0;
    m_count = 0;
}

void ErrorHistogram::Calc(const vector<Sample> &samples, const Spherical &location)
{
    Reset();    
    for (int i = 0; i < samples.size(); i++) {
        const Sample &sample = samples[i];
        float angle, ierror, qerror;
        sample.CalcError(location, angle, ierror, qerror); 
        Add(angle, sample.Strength(), ierror, qerror);
    }
    Normalize();
}

void ErrorHistogram::Add(float angle, float magnitude, float ierror, float qerror)
{
    int index = (int)(angle+180.0);
    while (index > 359)
        index -= 360;
    while (index < 0)
        index += 360;
    wxASSERT(index >= 0 && index < 360);
    
    float freq = m_bins[index] += magnitude;
    if (freq > m_modal_frequency) {
        m_modal_frequency = freq;
        m_mode = index-180;
    }
    
    m_isum += ierror;
    m_qsum += qerror;
    m_msum += magnitude;
    m_count++;
}

// This turns the histogram into an actual PDF
void ErrorHistogram::Normalize()
{
    if (m_msum == 0.0)
        return;
        
    for (int i = 0; i < 360; i++)
        m_bins[i] = m_bins[i]/(m_msum);

    m_modal_frequency /= m_msum;
    m_conc = (m_isum*m_isum+m_qsum*m_qsum)/(m_msum*m_msum);
    if (m_conc > 0.0)
        m_mean = atan2(m_qsum, m_isum)*180.0/M_PI;
}

// Event table for HistogramPanel
BEGIN_EVENT_TABLE(HistogramPanel, wxPanel)
    EVT_PAINT(HistogramPanel::OnPaint)
    EVT_SIZE(HistogramPanel::OnSize)
END_EVENT_TABLE()

HistogramPanel::HistogramPanel(wxWindow *parent) :
wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
{
	SetBackgroundColour(*wxBLACK);
    m_mode = Rectangular;
}

void HistogramPanel::SetData(const ErrorHistogram &histogram)
{
    m_histogram = histogram; // Copy constructor invoked
    Refresh();
}

void HistogramPanel::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);
	draw_panel(dc);
}

void HistogramPanel::draw_panel(wxDC &dc)
{
    const vector<float> &data = m_histogram.Data();            
    if (m_histogram.Count() == 0)
        return;
    
    if (m_mode == Polar) {
        // Draw histogram bars
      	dc.SetPen(wxPen(*wxRED, 1, wxSOLID));
        for (int i = 0; i < 360; i++) {
            float len = data[i]*(m_extent*0.75-10)/m_histogram.ModalFrequency();
        	float radians = i*M_PI/180.0;
            wxPoint tip = wxPoint((int)(m_center.x-sin(radians)*len),
        	              (int)(m_center.y+cos(radians)*len));
    
            dc.DrawLine(m_center, tip);
        }
    }
    else if (m_mode == Rectangular) {
        // Draw zero tick
        dc.SetPen(wxPen(*wxWHITE, 1, wxSOLID));
        dc.DrawLine(m_center.x, 0, m_center.x, 10);

        // Draw mode tick
        dc.SetPen(wxPen(*wxGREEN, 1, wxSOLID));
        int mode = (int)((m_histogram.Mode()+180)/360.0*m_width);
        dc.DrawLine(mode, 0, mode, 9);

        // Draw histogram bars
        dc.SetPen(wxPen(*wxRED, 1, wxSOLID));
        float freq = m_histogram.ModalFrequency();
        for (int i = 0; i < 360; i++) {
            int len = (int)(data[i]/freq*(m_height-10));
            int pos = (int)(i/360.0*m_width);
            dc.DrawLine(pos, m_height, pos, m_height-len);
        }
    }
}

void HistogramPanel::OnSize(wxSizeEvent &event)
{
    GetClientSize(&m_width, &m_height);
    m_center = wxPoint(m_width/2, (int)(m_height*0.75));

    if (m_width > m_height)
        m_extent = m_height;
    else
        m_extent = m_width;
        
    Refresh();
}
