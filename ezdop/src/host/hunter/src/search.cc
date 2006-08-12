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
#include "search.h"
#include "sample.h"
#include "util.h"

// wxWidgets includes
#include <wx/log.h>
#include <wx/utils.h>

// System level includes
#include <vector>

using namespace std;

const wxEventType wxEVT_SEARCH_UPDATE = wxNewEventType();

SearchUpdate::SearchUpdate(const wxEventType &event, bool done) :
wxNotifyEvent(event)
{
    m_done = done;
}

TransmitterSearch::TransmitterSearch(wxEvtHandler *dest) :
m_condition(m_mutex)
{
    Reset();
    m_resolution = 0.0005;  // 182 foot granularity
    m_dest = dest;
    m_log = NULL;
    m_scale = 0.85;    // Estimated from Mt. Umunhum data
    m_busy = false;
    Create();  // wxThreadHelper
    GetThread()->Run();
}

TransmitterSearch::~TransmitterSearch()
{
    GetThread()->Delete();
}

void TransmitterSearch::Reset()
{
    m_iterations = 0;
    m_initialized = false;
    m_solution = Spherical(0.0, 0.0);
    m_log = NULL;
    ClearStats();
}

void TransmitterSearch::ClearStats()
{
    m_histogram.Reset();
}

void *TransmitterSearch::Entry()
{
    wxLogDebug(_T("TransmitterSearch::Entry(): entered"));
    m_mutex.Lock();

	while (!GetThread()->TestDestroy())
        if (m_condition.WaitTimeout(1000) == wxCOND_NO_ERROR)
            background_solve();

    m_mutex.Unlock();
    wxLogDebug(_T("TransmitterSearch::Entry(): exited"));
}

void TransmitterSearch::background_solve()
{
    if (!m_log)
        return;

    m_iterations = 0;
    int count = m_log->Count();
    ClearStats();

    m_busy = true;
    if (count == 0)  // No data to solve from
        return post_update(true);
    
    if (!m_initialized) { // Get initial solution from first sample, but only once
        m_solution = m_log->Samples().begin()->Location();
        m_initialized = true;
    }

    if (count == 1)
        return post_update(true);

    while(1) {
        m_iterations = 0;
        while (++m_iterations <= MaxIterations) {
            wxMutexLocker locker(m_log->Mutex());
            if (hillclimb(m_log->Samples())) {     // true return indicates solution of some sort achieved
                m_histogram.Calc(m_log->Samples(), m_solution);
                return post_update(true);
            }
        }
        
        // Max iterations reached, send interim solution
        m_histogram.Calc(m_log->Samples(), m_solution);
        post_update(false);
    }    
}

void TransmitterSearch::post_update(bool done)
{
    SearchUpdate update(wxEVT_SEARCH_UPDATE, done);
    wxPostEvent(m_dest, update);
    m_busy = !done;
}

void TransmitterSearch::Solve(SampleLog *log)
{
    // FIXME: what if we get here while background thread is still busy?
    if (m_log && (m_log != log))
        wxLogError(_T("TransmitterSearch::Solve: supplied log different from current one."));
        
    m_log = log;
    m_condition.Signal();
}

// Return value indicates solution of some sort achieved
bool TransmitterSearch::hillclimb(vector<Sample> &samples)
{
    int nx, ny;
    int min_x = 0, min_y = 0;
    int num;
    
    Spherical trial;

    float min_error;
    float trial_error;
            
    // Initialize search with current solution
    if (calc_trial_error(samples, m_solution, min_error) == 0.0) {
        wxLogDebug(_T("TransmitterSearch::hillclimb: no enabled samples, returning"));
        return true; // No enabled data points, we're done
    }
            
    // Check if moving 'resolution' distance in one of four directions decreases error
    for (nx = -1; nx < 2; nx++) {
        trial.SetLongitude(m_solution.Longitude() + nx*m_resolution);
        for (ny = -1; ny < 2; ny++) {
            // Skip non-compass directions
            if (nx == ny)
                continue;
            trial.SetLatitude(m_solution.Latitude() + ny*m_resolution);
            calc_trial_error(samples, trial, trial_error);
            if (trial_error < min_error) {
                min_error = trial_error;
                min_x = nx; min_y = ny;
            }
        }
    }

    // Indicate if solution achieved
    if (min_x == 0 && min_y == 0)
        return true;
    else {
        m_solution.SetLatitude(m_solution.Latitude()+min_y*m_resolution);
        m_solution.SetLongitude(m_solution.Longitude()+min_x*m_resolution);
        return false; // Make outer loop call us again
    }
}

// Return value is number of enabled samples in vector
float TransmitterSearch::calc_trial_error(const vector<Sample>&samples, 
                                          const Spherical &trial, 
                                          float &trial_error)
{
    float wsum = 0.0;
    trial_error = 0.0;
    float strength = 1.0;
            
    for (int i = 0; i < samples.size(); i++) {
        const Sample &sample = samples[i];
        
        float angle, ierror, qerror;
        sample.CalcError(trial, angle, ierror, qerror);

        // Wrapped cauchy distribution
        float p = m_scale;
        float likelihood = (1-p*p)/(1+p*p-2*p*cos(angle*M_PI/180.0));

        trial_error += -log(likelihood)*sample.Strength();
        wsum += sample.Strength();
    }    

    if (wsum > 0.0)
        trial_error = trial_error/wsum;

    return wsum;
}
