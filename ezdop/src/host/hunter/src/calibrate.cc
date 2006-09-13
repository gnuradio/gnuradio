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

// Application level includes
#include "calibrate.h"
#include "hunter.h"

// wxWidgets includes
#include <wx/xrc/xmlres.h>
#include <wx/button.h>
#include <wx/gauge.h>
#include <wx/stattext.h>
#include <wx/log.h>

// Event table for CalibrationDialog
BEGIN_EVENT_TABLE(CalibrationDialog, wxDialog)
    EVT_BUTTON(XRCID("calibration_start_button"), CalibrationDialog::OnStart)
    EVT_BUTTON(XRCID("calibration_cancel_button"), CalibrationDialog::OnCancelOrZero)
END_EVENT_TABLE()

CalibrationDialog::CalibrationDialog(HunterFrame *parent)
{
    m_hunter_frame = parent;
    wxXmlResource::Get()->LoadDialog(this, parent, _T("calibration_dialog"));
    m_start_button = XRCCTRL(*this, "calibration_start_button", wxButton);
    m_cancel_button = XRCCTRL(*this, "calibration_cancel_button", wxButton);
    m_progress_gauge = XRCCTRL(*this, "calibration_progress_gauge", wxGauge);
    m_instructions_text = XRCCTRL(*this, "calibration_instructions_text", wxStaticText);
    m_progress_text = XRCCTRL(*this, "calibration_progress_text", wxStaticText);
    m_cancelled = false;
}

void CalibrationDialog::OnStart(wxCommandEvent &event)
{
    wxLogDebug(_T("CalibrationDialog::OnStart()"));
    wxString msg;
    
    m_start_button->Disable();
    m_equalized = false;
    for (int i = 0; i < 6; i++) {
        msg.Printf(_T("Peforming calibration step #%i"), i+1);
        m_progress_text->SetLabel(msg);
        if (m_cancelled)
             break;
        wxYield();
        m_hunter_frame->DoCalibrationStep(i);
        if (m_cancelled)
             break;
        m_progress_gauge->SetValue(i+1);        
    }
    m_equalized = true;
    m_instructions_text->SetLabel(_T("After pressing OK, you will need to\n" \
                                  "set the signal source to straight ahead\n" \
                                  "and press the Doppler 'Zero' button."));
    m_progress_text->SetLabel(_T("Calibration completed."));
    m_cancel_button->SetLabel(_T("OK"));
}

void CalibrationDialog::OnCancelOrZero(wxCommandEvent &event)
{
    wxLogDebug(_T("CalibrationDialog::OnCancel()"));
    if (!m_equalized) {
        m_cancelled = true;
    }

    EndModal(0);
}
