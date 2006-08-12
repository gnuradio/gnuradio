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

#ifndef __CALIBRATE_H__
#define __CALIBRATE_H__

// wxWidgets includes
#include <wx/event.h>
#include <wx/dialog.h>
#include <wx/gauge.h>  // Can't forward declare because wxGauge is a macro

// Forward declarations
class wxStaticText;
class HunterFrame;
class wxButton;

class CalibrationDialog : public wxDialog
{
public:
    CalibrationDialog(HunterFrame *parent);
    
private:
    void OnStart(wxCommandEvent &event);
    void OnCancelOrZero(wxCommandEvent &event);

    HunterFrame *m_hunter_frame;
    wxGauge *m_progress_gauge;
    wxStaticText *m_progress_text;
    wxStaticText *m_instructions_text;
    wxButton *m_start_button;
    wxButton *m_cancel_button;
    bool m_cancelled;
    bool m_equalized;
            
    DECLARE_EVENT_TABLE();
};

#endif // __CALIBRATE_H__
