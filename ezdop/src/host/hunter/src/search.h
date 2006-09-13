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

#ifndef __SEARCH_H__
#define __SEARCH_H__

// Application level includes
#include "spherical.h"
#include "samplelog.h"
#include "histogram.h"

// wxWidgets includes
#include <wx/event.h>

// System level includes
#include <ctime>

// Forward declarations
class wxFile;
class Sample;
class TransmitterSearch;

class SearchUpdate : public wxNotifyEvent
{
public:
    SearchUpdate(const wxEventType &event, bool done);
    virtual wxEvent *Clone() const { return new SearchUpdate(*this); }
    bool m_done;
};

extern const wxEventType wxEVT_SEARCH_UPDATE;

typedef void(wxEvtHandler::*SearchUpdateFunction)(SearchUpdate&);

#define EVT_SEARCH_UPDATE(fn) \
        DECLARE_EVENT_TABLE_ENTRY( \
            wxEVT_SEARCH_UPDATE, -1, -1, \
            (wxObjectEventFunction)(wxEventFunction)(SearchUpdateFunction)&fn, \
            (wxObject *)NULL \
        ),

class TransmitterSearch : public wxThreadHelper
{
public:
    TransmitterSearch(wxEvtHandler *dest);
    ~TransmitterSearch();

    void Solve(SampleLog *log);
    bool HasSolution() { return m_log ? m_log->Count() > 1 : false; }
    void Reset();
    void ClearStats();
    bool Busy() const { return m_busy; }
    int Mode() const { return m_histogram.Mode(); }
    float Mean() const { return m_histogram.Mean(); }
    float Concentration() { return m_histogram.Concentration(); }

    const ErrorHistogram &Histogram() { return m_histogram; }
    Spherical GetEstimatedLocation() { return m_solution; }

    virtual void *Entry();
                                    
private:
    static const int MaxIterations = 20; // TODO: make configurable

    bool hillclimb(std::vector<Sample> &samples);
    float calc_trial_error(const std::vector<Sample> &samples, const Spherical &trial, 
                           float &solution_error);

    void background_solve();
    void post_update(bool done);
    
    // Background processing
    wxEvtHandler *m_dest;
    wxMutex m_mutex;
    wxCondition m_condition;
    
    // Solution state
    SampleLog *m_log;
    Spherical m_solution;
    int m_iterations;
    bool m_initialized;
    bool m_busy;
        
    // Solution options
    float m_scale;
    float m_resolution;

    // Estimated solution histogram
    ErrorHistogram m_histogram;
};

#endif
