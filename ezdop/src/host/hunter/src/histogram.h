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

#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

// Application level includes
#include "sample.h"

// wxWidgets includes
#include <wx/panel.h>

// System level includes
#include <vector>

class ErrorHistogram
{
public:
    ErrorHistogram();

    void Reset();
    void Calc(const std::vector<Sample> &samples, const Spherical &location);
    void Add(float angle, float magnitude, float ierror, float qerror); // analytic errors are -pi to pi
    void Normalize();

    int   Count()                    const { return m_count; }    
    int   Mode()                     const { return m_mode; }
    float ModalFrequency()           const { return m_modal_frequency; }
    float Mean()                     const { return m_mean; }
    float Concentration()            const { return m_conc; }
    const std::vector<float> &Data() const { return m_bins; }
        
private:
    int   m_mode;
    float m_modal_frequency;
    float m_isum;
    float m_qsum;
    float m_msum;
    float m_conc;
    float m_mean;
    int   m_count;
    std::vector<float> m_bins;
};

class HistogramPanel : public wxPanel
{
public:
    enum Mode { Rectangular, Polar };

    HistogramPanel(wxWindow *parent);
    void SetData(const ErrorHistogram &histogram);
    void SetMode(Mode mode) { m_mode = mode; Refresh(); }
        
    // Event handlers
    void OnPaint(wxPaintEvent &event);
    void OnSize(wxSizeEvent &event);
                
private:
    void draw_panel(wxDC &dc);
    ErrorHistogram m_histogram;

    // State
    Mode m_mode;

    // Window size derived parameters
    wxPoint m_center;
    int m_width;
    int m_height;
    int m_extent;
    
    DECLARE_EVENT_TABLE();
};

#endif
