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

// Application includes
#include "hunter.h"
#include "hunter.xpm"
#include "doppler.h"
#include "tactical.h"
#include "calibrate.h"
#include "settings.h"
#include "gps.h"
#include "serial.h"
#include "search.h"
#include "util.h"
#include "spherical.h"
#include "sample.h"
#include "samplelog.h"
#include "known.h"
#include "histogram.h"

// wxWidgets includes
#include <wx/xrc/xmlres.h>
#include <wx/wx.h>          // Too many to do individually
#include <wx/file.h>        // hmmm, fails compile on mingw32 without it

// Event table for HunterFrame
BEGIN_EVENT_TABLE(HunterFrame, wxFrame)
    // Application level events
	EVT_CLOSE(HunterFrame::OnClose)
    
    // File menu events
    EVT_MENU(XRCID("file_new_menuitem"), HunterFrame::OnFileNew)
    EVT_MENU(XRCID("file_open_menuitem"), HunterFrame::OnFileOpen)
    EVT_MENU(XRCID("file_close_menuitem"), HunterFrame::OnFileClose)
    EVT_MENU(XRCID("file_save_menuitem"), HunterFrame::OnFileSave)
	EVT_MENU(XRCID("file_exit_menuitem"), HunterFrame::OnExit)

    // Doppler menu events
    EVT_MENU(XRCID("doppler_toggle_menuitem"), HunterFrame::OnDopplerToggle)
    EVT_MENU(XRCID("doppler_autostart_menuitem"), HunterFrame::OnDopplerAutostart)
    EVT_MENU(XRCID("doppler_reset_menuitem"), HunterFrame::OnDopplerReset)

    // Calibration menu events
    EVT_MENU(XRCID("calibration_savetofile_menuitem"), HunterFrame::OnCalibrationSaveToFile)
    EVT_MENU(XRCID("calibration_loadfromfile_menuitem"), HunterFrame::OnCalibrationLoadFromFile)
    EVT_MENU(XRCID("calibration_loadtransmitter_menuitem"), HunterFrame::OnCalibrationLoadTransmitter)
    EVT_MENU(XRCID("calibration_savetransmitter_menuitem"), HunterFrame::OnCalibrationSaveTransmitter)

    // About menu events
	EVT_MENU(XRCID("about_menuitem"), HunterFrame::OnAbout)

    // Status panel events
    EVT_RADIOBOX(XRCID("statistics_source_radiobox"), HunterFrame::OnHistogramSourceChg)
    EVT_RADIOBOX(XRCID("statistics_coords_radiobox"), HunterFrame::OnHistogramCoordsChg)

    // Doppler tab events
	EVT_BUTTON(XRCID("doppler_toggle_button"), HunterFrame::OnDopplerToggle)
	EVT_CHECKBOX(XRCID("doppler_autostart_checkbox"), HunterFrame::OnDopplerAutostart)
	EVT_BUTTON(XRCID("doppler_reset_button"), HunterFrame::OnDopplerReset)
	EVT_COMMAND_SCROLL(XRCID("doppler_filter_slider"), HunterFrame::OnDopplerFilterChg)
	EVT_RADIOBOX(XRCID("doppler_rotation_radiobox"), HunterFrame::OnDopplerRotationChg)
	EVT_DOPPLER_UPDATE(HunterFrame::OnDopplerUpdate)

    // GPS tab events
    EVT_BUTTON(XRCID("gps_toggle_button"), HunterFrame::OnGPSToggle)
	EVT_CHECKBOX(XRCID("gps_autostart_checkbox"), HunterFrame::OnGPSAutostart)
    EVT_COMBOBOX(XRCID("gps_device_combobox"), HunterFrame::OnGPSDeviceSelect)
    EVT_TEXT_ENTER(XRCID("gps_device_combobox"), HunterFrame::OnGPSDeviceSelect)
	EVT_GPS_UPDATE(HunterFrame::OnGPSUpdate)

    // Calibration tab events
	EVT_BUTTON(XRCID("calibration_equalize_button"), HunterFrame::OnCalibrationEqualize)
    EVT_BUTTON(XRCID("calibration_zero_button"), HunterFrame::OnCalibrationZero)
    EVT_SPIN_UP(XRCID("calibration_adjust_spinner"), HunterFrame::OnCalibrationAdjustRight)
    EVT_SPIN_DOWN(XRCID("calibration_adjust_spinner"), HunterFrame::OnCalibrationAdjustLeft)
	EVT_CHECKBOX(XRCID("calibration_all_checkbox"), HunterFrame::OnCalibrationAffectAllRates)
    EVT_BUTTON(XRCID("known_transmitter_update_button"), HunterFrame::OnKnownTransmitterUpdate)
    EVT_CHECKBOX(XRCID("known_transmitter_checkbox"), HunterFrame::OnUseKnownTransmitter)

    // Search tab events
    EVT_BUTTON(XRCID("search_newsave_button"), HunterFrame::OnSearchNewSave)
    EVT_BUTTON(XRCID("search_openclose_button"), HunterFrame::OnSearchOpenClose)
    EVT_BUTTON(XRCID("search_toggle_button"), HunterFrame::OnSearchToggle)
    EVT_BUTTON(XRCID("search_once_button"), HunterFrame::OnSearchOnce)
    EVT_SEARCH_UPDATE(HunterFrame::OnSearchUpdate)
    
    // Display tab events
	EVT_RADIOBOX(XRCID("display_orientation_radiobox"), HunterFrame::OnDisplayOrientation)
    EVT_CHECKBOX(XRCID("display_doppler_checkbox"), HunterFrame::OnDisplayDoppler)
    EVT_CHECKBOX(XRCID("display_known_checkbox"), HunterFrame::OnDisplayKnown)
    EVT_CHECKBOX(XRCID("display_estimated_checkbox"), HunterFrame::OnDisplayEstimated)

END_EVENT_TABLE()

HunterFrame::HunterFrame() :
wxFrame(),
m_search(this)
{
    m_settings = NULL;
    m_doppler = NULL;
    m_gps = NULL;
    m_log = NULL;
    m_tactical_panel = NULL;
    m_error_histogram_panel = NULL;
        
    m_doppler_started = false;
    m_gps_started = false;
    m_capture = false;
    m_one_shot = false;
    m_histogram_source = 0;
            
    InitializeSettings();
	InitializeWindows();
	InitializeDoppler();
    InitializeGPS();
    InitializeCalibration();
    InitializeSearch();
    InitializeDisplay();
}
	
void HunterFrame::InitializeSettings()
{
    m_settings = new HunterSettings();
    wxSetWorkingDirectory(m_settings->GetWorkingDirectory());
}

void HunterFrame::InitializeWindows()
{
	// Build main window controls
	bool loaded = wxXmlResource::Get()->LoadFrame(this, NULL, wxT("main_frame"));
	wxASSERT(loaded);

	// Hack until XRC understands <gravity> for wxSplitterWindow!!
	XRCCTRL(*this, "horiz_splitter", wxSplitterWindow)->SetSashGravity(1.0);

    // Increase font size for better visibility in certain text displays
    wxFont font = XRCCTRL(*this, "doppler_relative_bearing_text", wxStaticText)->GetFont();
    float points = font.GetPointSize()*2.0;
    font.SetPointSize((int)points);
    XRCCTRL(*this, "doppler_relative_bearing_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "doppler_absolute_bearing_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "transmitter_estimated_bearing_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "transmitter_estimated_range_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "transmitter_actual_bearing_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "transmitter_actual_range_text", wxStaticText)->SetFont(font);
    points *= 0.75;
    font.SetPointSize((int)points);
    XRCCTRL(*this, "gps_latitude_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "gps_longitude_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "gps_heading_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "gps_speed_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "search_latitude_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "search_longitude_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "search_count_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "search_status_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "search_mode_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "search_mean_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "search_score_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "search_disterror_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "error_count_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "error_mode_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "error_mean_text", wxStaticText)->SetFont(font);
    XRCCTRL(*this, "error_conc_text", wxStaticText)->SetFont(font);

    // Create the menu 
	SetMenuBar(wxXmlResource::Get()->LoadMenuBar(wxT("main_menu")));
	CreateStatusBar(1);
	SetIcon(wxIcon(hunter_xpm));

    // Restore saved window size and position
    int x = m_settings->GetWindowXPos();
    int y = m_settings->GetWindowYPos();
    wxSize size = m_settings->GetWindowSize();
    int hsplitpos = size.GetHeight()-170;   // HACK!
    SetSize(x, y, size.GetWidth(), size.GetHeight());
	XRCCTRL(*this, "horiz_splitter", wxSplitterWindow)->SetSashPosition(hsplitpos);

    // Error histogram is a custom control outside XRC system
    m_error_histogram_panel = new HistogramPanel(this);
	wxXmlResource::Get()->AttachUnknownControl(wxT("error_histogram_panel"), 
	                                               m_error_histogram_panel, this);
}

void HunterFrame::InitializeDoppler()
{
	m_doppler = new EZDoppler(this);
	m_doppler->Initialize();
	m_doppler_started = false;
    SetDopplerParams();
}

void HunterFrame::InitializeGPS()
{
    m_gps = new GPS(this);
	m_gps_started = false;

    // Populate device selection combobox
    XRCCTRL(*this, "gps_device_combobox", wxComboBox)->Clear();
	wxArrayString ports = EnumerateSerialPorts();
    for (int i = 0; i < ports.GetCount(); i++)
        XRCCTRL(*this, "gps_device_combobox", wxComboBox)->Append(ports[i]);
    XRCCTRL(*this, "gps_device_combobox", wxComboBox)->SetValue(m_settings->GetGPSDeviceName());
    
    // GPS autostart
    if (m_settings->GetGPSAutostart()) {
       XRCCTRL(*this, "gps_autostart_checkbox", wxCheckBox)->SetValue(true);
       StartGPS();            
    }
}

void HunterFrame::InitializeCalibration()
{
    XRCCTRL(*this, "calibration_all_checkbox", wxCheckBox)->SetValue(m_settings->GetCalibrationAffectAllRates());

    if (m_settings->GetUseKnownTransmitter()) {
        m_known.Location(Spherical(m_settings->GetKnownTransmitterLatitude(),
                                   m_settings->GetKnownTransmitterLongitude()));
        XRCCTRL(*this, "known_transmitter_checkbox", wxCheckBox)->SetValue(true);
    }

    UpdateKnownLocation();
}

void HunterFrame::InitializeDisplay()
{
    // Tactical display is a custom control outside XRC system
	m_tactical_panel = new TacticalPanel(this);
	wxXmlResource::Get()->AttachUnknownControl(wxT("tactical_panel"), 
	                                               m_tactical_panel, this);

    if (m_settings->GetDisplayOrientation()) {
        XRCCTRL(*this, "display_orientation_radiobox", wxRadioBox)->SetSelection(1);
        m_tactical_panel->SetOrientation(NorthUp);
    }

    if (m_settings->GetDisplayDoppler()) {
        XRCCTRL(*this, "display_doppler_checkbox", wxCheckBox)->SetValue(true);
        m_tactical_panel->SetDisplayDoppler(true);
    }

    if (m_settings->GetDisplayKnown()) {
        XRCCTRL(*this, "display_known_checkbox", wxCheckBox)->SetValue(true);
        m_tactical_panel->SetDisplayKnown(true);
    }

    if (m_settings->GetDisplayEstimated()) {
        XRCCTRL(*this, "display_estimated_checkbox", wxCheckBox)->SetValue(true);
        m_tactical_panel->SetDisplayEstimated(true);
    }
}

void HunterFrame::InitializeSearch()
{
    EnableSearchItems(false);
}

void HunterFrame::SetDopplerParams()
{
    // NOTE: This is not in InitializeDoppler() as it needs to be called 
    // separately when resetting Doppler

	// Adjust windows based on device status
	if (!m_doppler->IsOnline()) { 
	    // Disable all GUI elements associated with Doppler
        
        // Doppler control tab
	    XRCCTRL(*this, "doppler_control_panel", wxPanel)->Disable();

        // Doppler menu items
        this->GetMenuBar()->FindItem(XRCID("doppler_toggle_menuitem"))->Enable(false);
        this->GetMenuBar()->FindItem(XRCID("doppler_autostart_menuitem"))->Enable(false);
        this->GetMenuBar()->FindItem(XRCID("doppler_reset_menuitem"))->Enable(false);

        // Calibration control tab
        XRCCTRL(*this, "calibration_equalize_button", wxButton)->Enable(false);
        XRCCTRL(*this, "calibration_zero_button", wxButton)->Enable(false);
        XRCCTRL(*this, "calibration_adjust_spinner", wxSpinButton)->Enable(false);

        return;
	}

    // Doppler filter level
    int filter = m_settings->GetDopplerFilter();
    XRCCTRL(*this, "doppler_filter_slider", wxSlider)->SetValue(filter);
    wxScrollEvent dummy;
    OnDopplerFilterChg(dummy);

    // Doppler rotation rate
    int rate = m_settings->GetDopplerRotation();
    XRCCTRL(*this, "doppler_rotation_radiobox", wxRadioBox)->SetSelection(rate);
    wxCommandEvent dummy2;
    OnDopplerRotationChg(dummy2);

    // Doppler calibration values
    for (int i=0; i < 7; i++) {  // i==6 gets zero offset
        float offset = m_settings->GetDopplerCalibration(i);
        m_doppler->SetCalibration(i, offset);
    }

    // Doppler autostart
    if (m_settings->GetDopplerAutostart()) {
        this->GetMenuBar()->FindItem(XRCID("doppler_autostart_menuitem"))->Check(true);
        XRCCTRL(*this, "doppler_autostart_checkbox", wxCheckBox)->SetValue(true);
        StartDoppler();            
    }
}

void HunterFrame::StartDoppler()
{
	m_doppler->Start();
	m_doppler_started = true;
	XRCCTRL(*this, "doppler_toggle_button", wxButton)->SetLabel(_T("Stop"));
    XRCCTRL(*this, "calibration_equalize_button", wxButton)->Enable();
    XRCCTRL(*this, "calibration_zero_button", wxButton)->Enable();
    XRCCTRL(*this, "calibration_adjust_spinner", wxSpinButton)->Enable();
    this->GetMenuBar()->FindItem(XRCID("doppler_toggle_menuitem"))->SetText(_T("&Stop"));
    this->GetMenuBar()->FindItem(XRCID("doppler_reset_menuitem"))->Enable(true);
}

void HunterFrame::StopDoppler()
{
	m_doppler->Stop();
	m_doppler_started = false;
    UpdateDopplerStatus(false);
	XRCCTRL(*this, "doppler_toggle_button", wxButton)->SetLabel(_T("Start"));
    this->GetMenuBar()->FindItem(XRCID("doppler_toggle_menuitem"))->SetText(_("&Start"));
    this->GetMenuBar()->FindItem(XRCID("doppler_reset_menuitem"))->Enable(false);
    XRCCTRL(*this, "calibration_equalize_button", wxButton)->Disable();
    XRCCTRL(*this, "calibration_zero_button", wxButton)->Disable();
    XRCCTRL(*this, "calibration_adjust_spinner", wxSpinButton)->Disable();
}

void HunterFrame::StartGPS()
{
    wxString port = XRCCTRL(*this, "gps_device_combobox", wxComboBox)->GetValue();
    if (!m_gps->Start(port)) {
        wxMessageDialog(this, wxT("Failed to start GPS!"), wxT("GPS Error"),
                        wxOK|wxICON_ERROR).ShowModal();
        return;
    }    
    
    m_gps_started = true;
    XRCCTRL(*this, "gps_toggle_button", wxButton)->SetLabel(_T("Stop"));
    XRCCTRL(*this, "gps_device_combobox", wxComboBox)->Disable();
    XRCCTRL(*this, "display_orientation_radiobox", wxRadioBox)->Enable();
    if (XRCCTRL(*this, "display_orientation_radiobox", wxRadioBox)->GetSelection() == 1) {
        m_tactical_panel->SetOrientation(NorthUp);
    }

    if (m_search.HasSolution())
        UpdateSearchDirection(true);

    UpdateKnownDirection();
    UpdateKnownStatistics();
}

void HunterFrame::StopGPS()
{
    m_gps->Stop();
    m_gps_started = false;
    m_tactical_panel->SetOrientation(TrackUp);
    m_tactical_panel->SetActualBearing(-1.0);
    m_tactical_panel->SetEstimatedBearing(-1.0);
	XRCCTRL(*this, "gps_toggle_button", wxButton)->SetLabel(_T("Start"));
    XRCCTRL(*this, "gps_device_combobox", wxComboBox)->Enable();
    XRCCTRL(*this, "display_orientation_radiobox", wxRadioBox)->Disable();

    UpdateGPSStatus(false);
    UpdateSearchDirection(false);
    UpdateKnownDirection();
    UpdateKnownStatistics();
        
    // Note: can't replace with call to UpdateDopplerStatus as we only want to clear one field
    XRCCTRL(*this, "doppler_absolute_bearing_text", wxStaticText)->SetLabel(_T(""));
}

void HunterFrame::EnableSearchItems(bool enable)
{
	XRCCTRL(*this, "search_toggle_button", wxButton)->Enable(enable);
	XRCCTRL(*this, "search_once_button", wxButton)->Enable(enable);

    // These fields will get populated when samples come in
    UpdateSearchStatus(false);
    UpdateSearchLocation(false);
    UpdateSearchDirection(false);    

    this->GetMenuBar()->FindItem(XRCID("file_save_menuitem"))->Enable(enable);
    this->GetMenuBar()->FindItem(XRCID("file_close_menuitem"))->Enable(enable);

    if (!enable) {
        XRCCTRL(*this, "search_toggle_button", wxButton)->SetLabel(_T("Stop"));
        XRCCTRL(*this, "search_toggle_button", wxButton)->SetLabel(_T("Automatic"));
    }
}

void HunterFrame::OnClose(wxCloseEvent &event)
{
    wxCommandEvent dummy;
        
    // Cleanup for this scope should be done here
    // TODO: factor out into methods corresponding to start up initialization

    // Save window size and position
    int x, y;
    GetPosition(&x, &y);
    m_settings->SetWindowXPos(x);
    m_settings->SetWindowYPos(y);
    m_settings->SetWindowSize(GetSize());

    wxASSERT(m_doppler != NULL);
    if (m_doppler_started)
        StopDoppler();
    m_doppler->Finalize();
    if (m_gps_started)
        StopGPS();

    if (m_log)
        OnFileClose(dummy);

        
    m_settings->SetWorkingDirectory(wxGetCwd());

    delete m_doppler;
    delete m_gps;
    delete m_settings;
    if (m_log)
        delete m_log;        

    // Last thing to do
	Destroy();
}

void HunterFrame::OnExit(wxCommandEvent &event)
{
	// Cleanup for this scope should be done in ::OnClose, not here.  This
	// method is not called when exiting from the system menu, but rather it
	// goes straight to ::OnClose

	// Sends close event
	this->Close();
}

void HunterFrame::OnAbout(wxCommandEvent &event)
{
	wxMessageBox(wxT("Copyright(C) 2005 Johnathan Corgan"), 
		     wxT("About AE6HO Radio Location System"), 
		     wxOK | wxICON_INFORMATION, this);
}

void HunterFrame::OnDopplerToggle(wxCommandEvent &event)
{
	if (m_doppler_started)
	    StopDoppler();
	else
	    StartDoppler();
}

void HunterFrame::OnDopplerAutostart(wxCommandEvent &event)
{
    bool autostart = event.IsChecked();
    this->GetMenuBar()->FindItem(XRCID("doppler_autostart_menuitem"))->Check(autostart);
    XRCCTRL(*this, "doppler_autostart_checkbox", wxCheckBox)->SetValue(autostart);
    m_settings->SetDopplerAutostart(autostart);
}

void HunterFrame::OnDopplerReset(wxCommandEvent &event)
{
	StopDoppler();
	m_doppler->Reset();
    SetDopplerParams();  // restarts Doppler if autostart is configured
}

void HunterFrame::OnDopplerFilterChg(wxScrollEvent &event)
{
	int n = XRCCTRL(*this, "doppler_filter_slider", wxSlider)->GetValue();
	m_doppler->SetFilter(n);
    m_settings->SetDopplerFilter(n);
}

void HunterFrame::OnDopplerRotationChg(wxCommandEvent &event)
{
	int n = XRCCTRL(*this, "doppler_rotation_radiobox", wxRadioBox)->GetSelection();
	m_doppler->SelectRotationRate(n);
    m_settings->SetDopplerRotation(n);
}

void HunterFrame::OnDopplerUpdate(EZDopplerUpdate &event)
{
    if (!m_doppler_started) // Spurious event after doppler stopped
        return;
        
    // Update current state variables
    m_sample.Volume(event.m_volume);
    m_sample.InPhase(event.m_in_phase);
    m_sample.Quadrature(event.m_quadrature);
    m_sample.Strength(sqrt(event.m_in_phase*event.m_in_phase + 
                           event.m_quadrature*event.m_quadrature));
    m_sample.Phase(atan2(event.m_quadrature, event.m_in_phase));

    UpdateDopplerStatus(true);
}

void HunterFrame::UpdateDopplerStatus(bool display)
{
    wxString str;
    if (!display) {
        m_tactical_panel->SetDopplerBearing(-1.0);
        XRCCTRL(*this, "doppler_level_gauge", wxGauge)->SetValue(0);
        XRCCTRL(*this, "doppler_audio_gauge", wxGauge)->SetValue(0);
        XRCCTRL(*this, "doppler_relative_bearing_text", wxStaticText)->SetLabel(_T(""));
        XRCCTRL(*this, "doppler_absolute_bearing_text", wxStaticText)->SetLabel(_T(""));
        return;
    }

    float display_relative_bearing = degree_normalize(to_degrees(m_sample.Phase()));
    if (m_tactical_panel)
        m_tactical_panel->SetDopplerBearing(display_relative_bearing);

    int display_magnitude = (int)limit((m_sample.Strength()*200.0), 0.0, 100.0);
    XRCCTRL(*this, "doppler_level_gauge", wxGauge)->SetValue(display_magnitude);

    int display_amplitude = (int)limit((m_sample.Volume()*100.0), 0.0, 100.0);
    XRCCTRL(*this, "doppler_audio_gauge", wxGauge)->SetValue(display_amplitude);

    str.Printf(_T("%03i"), degree_normalize((int)(display_relative_bearing+0.5))); // So zero is from -0.5 to 0.5
    XRCCTRL(*this, "doppler_relative_bearing_text", wxStaticText)->SetLabel(str);

    if (m_gps_started) {
        str.Printf(_T("%03i"), degree_normalize((int)(m_sample.Heading()+display_relative_bearing))); 
        XRCCTRL(*this, "doppler_absolute_bearing_text", wxStaticText)->SetLabel(str);
    }
}

void HunterFrame::CalcKnownStatistics()
{
    if (m_log)
        m_known.Calc(m_log->Samples());

    UpdateKnownStatistics();
}   

void HunterFrame::UpdateKnownStatistics()
{
    if (m_error_histogram_panel && m_histogram_source == 1)
        m_error_histogram_panel->SetData(m_known.Histogram());
    
    if (!m_known.HasStats()) {
        XRCCTRL(*this, "error_count_text", wxStaticText)->SetLabel(_T(""));
        XRCCTRL(*this, "error_mode_text", wxStaticText)->SetLabel(_T(""));
        XRCCTRL(*this, "error_mean_text", wxStaticText)->SetLabel(_T(""));
        XRCCTRL(*this, "error_conc_text", wxStaticText)->SetLabel(_T(""));
        return;
    }
    
    wxString str;

    str.Printf(_T("%i"), m_known.Count());
    XRCCTRL(*this, "error_count_text", wxStaticText)->SetLabel(str);

    str.Printf(_T("%i"), m_known.Mode());
    XRCCTRL(*this, "error_mode_text", wxStaticText)->SetLabel(str);

    str.Printf(_T("%5.1f"), m_known.Mean());
    XRCCTRL(*this, "error_mean_text", wxStaticText)->SetLabel(str);

    str.Printf(_T("%1.3f"), m_known.Concentration());
    XRCCTRL(*this, "error_conc_text", wxStaticText)->SetLabel(str);
}

void HunterFrame::OnCalibrationEqualize(wxCommandEvent &event)
{
    CalibrationDialog *dlg = new CalibrationDialog(this);
    dlg->ShowModal();
}

void HunterFrame::DoCalibrationStep(int which)
{
    static int delay;
    
    if (which == 0) {       // Set up doppler 
        delay = XRCCTRL(*this, "doppler_filter_slider", wxSlider)->GetValue(); // Empirically determined
        if (delay == 0)
            delay = 1;
    }

    // Set rotation rate for this step
    wxCommandEvent dummy;
    XRCCTRL(*this, "doppler_rotation_radiobox", wxRadioBox)->SetSelection(which);
    OnDopplerRotationChg(dummy);

    // Wait until stable value
    wxStartTimer();
    while(wxGetElapsedTime(false) < 1000*delay)
        wxYield(); // eeewwww!
    
    // Now stable reading can be calibrated
    m_doppler->Calibrate(-M_PI);      // Sets to zero

    float offset=m_doppler->GetCalibration(which);
    m_settings->SetDopplerCalibration(which, offset);
}

void HunterFrame::OnCalibrationZero(wxCommandEvent &event)
{
    m_doppler->SetOffset();
    float offset=m_doppler->GetCalibration(6);
    m_settings->SetDopplerCalibration(6, offset);
}

void HunterFrame::OnCalibrationAdjustLeft(wxSpinEvent &event)
{
    if (m_settings->GetCalibrationAffectAllRates())
        m_doppler->NudgeAll(to_radians(-0.5));
    else
        m_doppler->Nudge(to_radians(-0.5));

    float offset=m_doppler->GetCalibration(6);
    m_settings->SetDopplerCalibration(6, offset);
}

void HunterFrame::OnCalibrationAdjustRight(wxSpinEvent &event)
{
    if (m_settings->GetCalibrationAffectAllRates())
        m_doppler->NudgeAll(to_radians(0.5));
    else
        m_doppler->Nudge(to_radians(0.5));

    float offset=m_doppler->GetCalibration(6);
    m_settings->SetDopplerCalibration(6, offset);
}

void HunterFrame::OnCalibrationAffectAllRates(wxCommandEvent &event)
{
    bool affect = event.IsChecked();
    XRCCTRL(*this, "calibration_all_checkbox", wxCheckBox)->SetValue(affect);
    m_settings->SetCalibrationAffectAllRates(affect);
}

void HunterFrame::OnGPSToggle(wxCommandEvent &event)
{
	if (m_gps_started)
	    StopGPS();
	else
	    StartGPS();
}

void HunterFrame::OnGPSAutostart(wxCommandEvent &event)
{
    bool start = XRCCTRL(*this, "gps_autostart_checkbox", wxCheckBox)->GetValue();
    m_settings->SetGPSAutostart(start);
}

void HunterFrame::OnGPSDeviceSelect(wxCommandEvent &event)
{
	wxString device = XRCCTRL(*this, "gps_device_combobox", wxComboBox)->GetValue();
    m_settings->SetGPSDeviceName(device);
}

void HunterFrame::OnGPSUpdate(GPSUpdate &update)
{
    // Update state variables
    GPRMC *gprmc = update.m_gprmc;
    m_sample.Valid(update.m_gprmc->m_valid);
    m_sample.Time(update.m_gprmc->m_stamp);
    m_sample.Location(update.m_gprmc->m_fix);
    m_sample.Heading(update.m_gprmc->m_heading);
    m_sample.Speed(update.m_gprmc->m_speed*1.15077945); // Conversion from knots to mph
    m_sample.Rate(XRCCTRL(*this, "doppler_rotation_radiobox", wxRadioBox)->GetSelection());
    m_sample.Filtering(XRCCTRL(*this, "doppler_filter_slider", wxSlider)->GetValue());
    
    UpdateGPSValidity(update.m_gprmc->m_valid);                 // Colors red for invalid, black for valid
    UpdateGPSStatus(true);                                      // gps lat, lon, heading, speed
    UpdateKnownDirection();                                     // actual bearing and range
    CalcKnownStatistics();

    if (m_log && m_capture && m_doppler_started &&
        m_sample.Speed() >= 5.0 && m_sample.Valid()) {
        m_log->Add(m_sample);
        CalcSolution();
        if (m_one_shot == true)
            StopCapture();
    }

    if (m_search.HasSolution()) {
        UpdateSearchStatus(true);
        UpdateSearchDirection(true);
    }
        
    delete update.m_gprmc;
}

void HunterFrame::CalcSolution()
{
    GetStatusBar()->SetStatusText(wxT("Searching..."));    
    m_search.Solve(m_log);
}

void HunterFrame::OnSearchUpdate(SearchUpdate &update)
{
    if (update.m_done)
        GetStatusBar()->SetStatusText(wxT(""));    

    if (m_search.HasSolution()) {
        UpdateSearchStatus(true);
        UpdateSearchLocation(true);
        if (m_gps_started)
           UpdateSearchDirection(true);
    }
}

void HunterFrame::UpdateSearchStatus(bool display)
{
    wxString str;

    if (m_error_histogram_panel && m_histogram_source == 0)
        m_error_histogram_panel->SetData(m_search.Histogram());
        
    if (!display) {
        XRCCTRL(*this, "search_count_text", wxStaticText)->SetLabel(_T(""));    
        XRCCTRL(*this, "search_status_text", wxStaticText)->SetLabel(_T(""));    
        XRCCTRL(*this, "search_mode_text", wxStaticText)->SetLabel(_T(""));    
        XRCCTRL(*this, "search_mean_text", wxStaticText)->SetLabel(_T(""));    
        XRCCTRL(*this, "search_score_text", wxStaticText)->SetLabel(_T(""));    
        return;
    }

    str.Printf(_T("%i"), m_log->Count());
    XRCCTRL(*this, "search_count_text", wxStaticText)->SetLabel(str);

    str.Printf(_T("%s"), m_search.Busy() ? _T("BUSY") : _T(""));
    XRCCTRL(*this, "search_status_text", wxStaticText)->SetLabel(str);

    str.Printf(_T("%i"), m_search.Mode());
    XRCCTRL(*this, "search_mode_text", wxStaticText)->SetLabel(str);

    str.Printf(_T("%6.1f"), m_search.Mean());
    XRCCTRL(*this, "search_mean_text", wxStaticText)->SetLabel(str);

    str.Printf(_T("%i"), (int)(m_search.Concentration()*100.0));
    XRCCTRL(*this, "search_score_text", wxStaticText)->SetLabel(str);
}

void HunterFrame::UpdateGPSStatus(bool display)
{            
    wxString str;

    if (!display) {
        XRCCTRL(*this, "gps_latitude_text", wxStaticText)->SetLabel(_T(""));    
        XRCCTRL(*this, "gps_longitude_text", wxStaticText)->SetLabel(_T(""));    
        XRCCTRL(*this, "gps_heading_text", wxStaticText)->SetLabel(_T(""));    
        XRCCTRL(*this, "gps_speed_text", wxStaticText)->SetLabel(_T(""));    
        return;
    }

    // Calculate latitude to display
    str.Printf(_T("%1.5lf"), fabs(m_sample.Latitude()));
    str.Append(m_sample.Latitude() < 0.0 ? _T("S") : _T("N"));
    XRCCTRL(*this, "gps_latitude_text", wxStaticText)->SetLabel(str);    

    // Calculate longitude to display
    str.Printf(_T("%1.5lf"), fabs(m_sample.Longitude()));
    str.Append(m_sample.Longitude() < 0.0 ? _T("W") : _T("E"));
    XRCCTRL(*this, "gps_longitude_text", wxStaticText)->SetLabel(str);    

    // Calculate heading to display
    m_tactical_panel->SetHeading(m_sample.Heading());
    str.Printf(_T("%03i"), (unsigned int)(m_sample.Heading()));
    XRCCTRL(*this, "gps_heading_text", wxStaticText)->SetLabel(str);    

    // Calculate speed to display
    str.Printf(_T("%i"), (unsigned int)(m_sample.Speed()));
    XRCCTRL(*this, "gps_speed_text", wxStaticText)->SetLabel(str);    
}    

void HunterFrame::UpdateSearchDirection(bool display)
{
    wxString str;

    if (!display) {
        XRCCTRL(*this, "transmitter_estimated_bearing_text", wxStaticText)->SetLabel(_T(""));
        XRCCTRL(*this, "transmitter_estimated_range_text", wxStaticText)->SetLabel(_T(""));
        if (m_tactical_panel)
            m_tactical_panel->SetEstimatedBearing(-1.0);
        return;
    }

    float estimated_bearing = 
        bearing(m_sample.Location(), m_search.GetEstimatedLocation());
                
    float estimated_range = 
        range(m_sample.Location(), m_search.GetEstimatedLocation());
    
    m_tactical_panel->SetEstimatedBearing(estimated_bearing);
    str.Printf(_T("%03i"), degree_normalize((int)estimated_bearing));
    XRCCTRL(*this, "transmitter_estimated_bearing_text", wxStaticText)->SetLabel(str);
    if (estimated_range > 99.9)
        str.Printf(_T("%1.0f"), estimated_range);
    else if (estimated_range > 9.99)
        str.Printf(_T("%1.1f"), estimated_range);
    else
        str.Printf(_T("%1.2f"), estimated_range);
    XRCCTRL(*this, "transmitter_estimated_range_text", wxStaticText)->SetLabel(str);
}

void HunterFrame::UpdateSearchLocation(bool display)
{
    wxString str;
    
    if (!display) {
        XRCCTRL(*this, "search_latitude_text", wxStaticText)->SetLabel(_T(""));    
        XRCCTRL(*this, "search_longitude_text", wxStaticText)->SetLabel(_T(""));    
        XRCCTRL(*this, "search_disterror_text", wxStaticText)->SetLabel(_T(""));    
        return;
    }

    Spherical estimated_location = m_search.GetEstimatedLocation();

    // Calculate latitude to display
    str.Printf(_T("%1.5f"), fabs(estimated_location.Latitude()));
    str.Append(estimated_location.Latitude() < 0.0 ? _T("S") : _T("N"));
    XRCCTRL(*this, "search_latitude_text", wxStaticText)->SetLabel(str);    

    // Calculate longitude to display
    str.Printf(_T("%1.5f"), fabs(estimated_location.Longitude()));
    str.Append(estimated_location.Longitude() < 0.0 ? _T("W") : _T("E"));
    XRCCTRL(*this, "search_longitude_text", wxStaticText)->SetLabel(str);    

    if (m_known.IsSet()) {
        float distance_error = range(m_search.GetEstimatedLocation(), m_known.Location());
        if (distance_error > 99.9)
            str.Printf(_T("%1.0f"), distance_error);
        else if (distance_error > 9.99)
            str.Printf(_T("%1.1f"), distance_error);
        else
            str.Printf(_T("%1.2f"), distance_error);
        XRCCTRL(*this, "search_disterror_text", wxStaticText)->SetLabel(str);
    }
}

void HunterFrame::UpdateKnownDirection()
{
    wxString str;

    if (!m_known.IsSet() || !m_gps_started) {
        XRCCTRL(*this, "transmitter_actual_bearing_text", wxStaticText)->SetLabel(_T(""));
        XRCCTRL(*this, "transmitter_actual_range_text", wxStaticText)->SetLabel(_T(""));
        if (m_tactical_panel)
            m_tactical_panel->SetActualBearing(-1.0);
        return;
    }    

    float actual_bearing = bearing(m_sample.Location(), m_known.Location());
    float actual_range   = range(m_sample.Location(), m_known.Location());
                                       
    m_tactical_panel->SetActualBearing(actual_bearing);

    str.Printf(_T("%03i"), degree_normalize((int)actual_bearing));
    XRCCTRL(*this, "transmitter_actual_bearing_text", wxStaticText)->SetLabel(str);
    if (actual_range > 99.9)
        str.Printf(_T("%1.0f"), actual_range);
    else if (actual_range > 9.99)
        str.Printf(_T("%1.1f"), actual_range);
    else
        str.Printf(_T("%1.2f"), actual_range);
    XRCCTRL(*this, "transmitter_actual_range_text", wxStaticText)->SetLabel(str);
}

void HunterFrame::UpdateKnownLocation()
{
    wxString str;
    
    if (!m_known.IsSet()) {
        XRCCTRL(*this, "known_transmitter_latitude_edit", wxTextCtrl)->SetValue(_T(""));
        XRCCTRL(*this, "known_transmitter_longitude_edit", wxTextCtrl)->SetValue(_T(""));
        XRCCTRL(*this, "search_disterror_text", wxStaticText)->SetLabel(_T(""));
        return;
    }    

    str.Printf(_T("%1.5f"), m_known.Latitude());        
    XRCCTRL(*this, "known_transmitter_latitude_edit", wxTextCtrl)->SetValue(str);
    str.Printf(_T("%1.5f"), m_known.Longitude());
    XRCCTRL(*this, "known_transmitter_longitude_edit", wxTextCtrl)->SetValue(str);
    UpdateSearchLocation(true); // to update the known error
}

void HunterFrame::OnKnownTransmitterUpdate(wxCommandEvent &event)
{
    // Note: this is an event handler for a calibration tab button
    wxString lat_text = XRCCTRL(*this, "known_transmitter_latitude_edit", wxTextCtrl)->GetValue();
    wxString lon_text = XRCCTRL(*this, "known_transmitter_longitude_edit", wxTextCtrl)->GetValue();

    double lat = 0.0, lon = 0.0;

    // TODO: use Spherical constructor/exceptions
    if (!lat_text.ToDouble(&lat) || lat > 90.0 || lat < -90.0) {
        wxMessageDialog(this, wxT("Invalid latitude entered."), wxT("Data Error"),
                        wxOK|wxICON_ERROR).ShowModal();
        return;
    }        

    if (!lon_text.ToDouble(&lon) || lon >180.0 || lon < -180.0) {
        wxMessageDialog(this, wxT("Invalid longitude entered."), wxT("Data Error"),
                        wxOK|wxICON_ERROR).ShowModal();
        return;
    }        

    m_known.Location(Spherical(lat, lon));
    CalcKnownStatistics();
    
    m_settings->SetKnownTransmitterLongitude(lon);
    m_settings->SetKnownTransmitterLatitude(lat);

    UpdateKnownLocation();
    if (m_gps_started)
        UpdateKnownDirection();
}

void HunterFrame::OnUseKnownTransmitter(wxCommandEvent &event)
{
    if (event.IsChecked())
        m_known.Location(Spherical(m_settings->GetKnownTransmitterLatitude(),
                                   m_settings->GetKnownTransmitterLongitude()));
    else
        m_known.Clear();

    m_settings->SetUseKnownTransmitter(m_known.IsSet());
    CalcKnownStatistics();
    UpdateKnownLocation();
    if (m_gps_started)
        UpdateKnownDirection();    
}

void HunterFrame::UpdateGPSValidity(bool valid)
{
    m_sample.Valid(valid);

    wxColour color = *wxBLACK;
    if (!valid)
        color = *wxRED;

    XRCCTRL(*this, "doppler_absolute_bearing_text", wxStaticText)->SetForegroundColour(color);
    XRCCTRL(*this, "transmitter_actual_bearing_text", wxStaticText)->SetForegroundColour(color);
    XRCCTRL(*this, "transmitter_actual_range_text", wxStaticText)->SetForegroundColour(color);
    XRCCTRL(*this, "gps_latitude_text", wxStaticText)->SetForegroundColour(color);
    XRCCTRL(*this, "gps_longitude_text", wxStaticText)->SetForegroundColour(color);
    XRCCTRL(*this, "gps_heading_text", wxStaticText)->SetForegroundColour(color);
    XRCCTRL(*this, "gps_speed_text", wxStaticText)->SetForegroundColour(color);
}

void HunterFrame::OnFileNew(wxCommandEvent &event)
{
    if (m_log) {
        wxLogDebug(_T("Not Implemented: current log is discarded without saving..."));
        StopCapture();
        delete m_log;
        m_search.Reset();
        m_log = NULL;
    }

    MakeNewLogAndSearch();
    this->SetTitle(wxT("AE6HO Radio Location System - Unsaved Search")); // refactor into EnableSearchItems()
    XRCCTRL(*this, "search_newsave_button", wxButton)->SetLabel(_T("Save"));
    XRCCTRL(*this, "search_openclose_button", wxButton)->SetLabel(_T("Close"));
    this->GetMenuBar()->FindItem(XRCID("file_new_menuitem"))->Enable(false);
    this->GetMenuBar()->FindItem(XRCID("file_open_menuitem"))->Enable(false);
    EnableSearchItems(true);
}

void HunterFrame::MakeNewLogAndSearch()
{
    m_log = new SampleLog();
    m_search.Reset();
}

void HunterFrame::OnFileOpen(wxCommandEvent &event)
{
    wxString filename;

    if (m_log)
        OnFileClose(event);

    wxFileDialog dlg(this, wxT("Open Log File"), _T("."), _T(""), _T("*.dat"),
                     wxOPEN|wxFILE_MUST_EXIST|wxCHANGE_DIR);
    if (dlg.ShowModal() == wxID_OK) {
        this->GetMenuBar()->FindItem(XRCID("file_new_menuitem"))->Enable(false);
        this->GetMenuBar()->FindItem(XRCID("file_open_menuitem"))->Enable(false);
        filename = dlg.GetPath();
        MakeNewLogAndSearch();
        m_log->Load(filename);        
        this->SetTitle(wxT("AE6HO Radio Location System - ")+filename);
        XRCCTRL(*this, "search_newsave_button", wxButton)->SetLabel(_T("Save"));
        XRCCTRL(*this, "search_openclose_button", wxButton)->SetLabel(_T("Close"));
        EnableSearchItems(true);
        CalcKnownStatistics();
        CalcSolution();
    }
}

void HunterFrame::OnFileSave(wxCommandEvent &event)
{
    wxASSERT(m_log);
    
    wxString filename;
    if (!m_log->HasFile()) {
        wxFileDialog dlg(this, wxT("Save Search Data"), _T("."), _T(""), _T("*.dat"), 
                         wxSAVE|wxOVERWRITE_PROMPT|wxCHANGE_DIR);
        if (dlg.ShowModal() == wxID_OK) {
            filename = dlg.GetPath();
            m_log->Save(filename);
            this->SetTitle(wxT("AE6HO Radio Location System - ")+filename);
        }
    }
    else
        m_log->Save();  // Adds additional samples since last save
}

void HunterFrame::OnFileClose(wxCommandEvent &event)
{
    // FIXME: ask user if they want to save changed data instead of going straight to save dialog
    if (m_log->IsDirty())
        OnFileSave(event);

    StopCapture();
    delete m_log;
    m_search.Reset();
    m_log = NULL;
    
    this->SetTitle(wxT("AE6HO Radio Location System")); // refactor into EnableSearchItems()
    XRCCTRL(*this, "search_newsave_button", wxButton)->SetLabel(_T("New"));
    XRCCTRL(*this, "search_openclose_button", wxButton)->SetLabel(_T("Open"));
    this->GetMenuBar()->FindItem(XRCID("file_new_menuitem"))->Enable(true);
    this->GetMenuBar()->FindItem(XRCID("file_open_menuitem"))->Enable(true);
    EnableSearchItems(false);
    m_known.ClearStats();
    UpdateKnownStatistics();
}

void HunterFrame::OnCalibrationSaveToFile(wxCommandEvent &event)
{
    wxString filename;
    wxFileDialog dlg(this, wxT("Save Calibration Data"), _T("."), _T(""), _T("*.cal"), 
                     wxSAVE|wxOVERWRITE_PROMPT|wxCHANGE_DIR);
    if (dlg.ShowModal() == wxID_OK) {
        filename = dlg.GetPath();
        wxFile file(filename.c_str(), wxFile::write);
        for (int i = 0; i < 7; i++) {
            wxString str;
            float offset = m_settings->GetDopplerCalibration(i);
            str.Printf(_T("%f\n"), offset);
            file.Write(str.c_str(), str.length());
        }
    }
}

void HunterFrame::OnCalibrationLoadFromFile(wxCommandEvent &event)
{
    wxString filename;
    wxFileDialog dlg(this, wxT("Load Calibration Data"), _T("."), _T(""), _T("*.cal"),
                     wxOPEN|wxFILE_MUST_EXIST|wxCHANGE_DIR);
    if (dlg.ShowModal() == wxID_OK) {
        filename = dlg.GetPath();
        wxFile file(filename.c_str(), wxFile::read);
        for (int i = 0; i < 7; i++) {
            char ch;
            size_t count;
            wxString line;
            count = file.Read(&ch, 1);
            while (count == 1 && ch != '\n') {
                line.Append(ch);
                count = file.Read(&ch, 1);
            }
            float offset = atof((char *)line.c_str());
            m_settings->SetDopplerCalibration(i, offset);
            m_doppler->SetCalibration(i, offset);
        }
    }
}

void HunterFrame::OnCalibrationSaveTransmitter(wxCommandEvent &event)
{
    wxString filename;
    if (!m_known.IsSet())
        return; // FIXME: disable menu item when no known transmitter so we can't get here

    wxFileDialog dlg(this, wxT("Save Transmitter Coordinates"), _T("."), _T(""), _T("*.loc"), 
                     wxSAVE|wxOVERWRITE_PROMPT|wxCHANGE_DIR);
    if (dlg.ShowModal() == wxID_OK) {
        filename = dlg.GetPath();
        wxFile file(filename.c_str(), wxFile::write);
        wxString str;
        // TODO: use Spherical method to output strings
        str.Printf(_T("%f\n%f\n"), m_known.Latitude(),
                               m_known.Longitude());
        file.Write(str.c_str(), str.length());
    }
}

void HunterFrame::OnCalibrationLoadTransmitter(wxCommandEvent &event)
{
    wxString filename;
    wxFileDialog dlg(this, wxT("Load Transmitter Location"), _T("."), _T(""), _T("*.loc"),
                     wxOPEN|wxFILE_MUST_EXIST|wxCHANGE_DIR);
    if (dlg.ShowModal() == wxID_OK) {
        filename = dlg.GetPath();
        wxFile file(filename.c_str(), wxFile::read);
        wxString latitude, longitude;

        for (int i = 0; i < 2; i++) {
            char ch;
            size_t count;
            wxString line;

            count = file.Read(&ch, 1);
            while (count == 1 && ch != '\n') {
                line.Append(ch);
                count = file.Read(&ch, 1);
            }
            if (i == 0) {
                latitude = line;
            }
            else if (i == 1) {
                longitude = line;
            }
        }

        m_known.Location(Spherical(latitude, longitude));
        CalcKnownStatistics();

        XRCCTRL(*this, "known_transmitter_checkbox", wxCheckBox)->SetValue(true);
        m_settings->SetUseKnownTransmitter(true);
        m_settings->SetKnownTransmitterLatitude(m_known.Latitude());
        m_settings->SetKnownTransmitterLongitude(m_known.Longitude());

        UpdateKnownLocation();
        if (m_gps_started)
            UpdateKnownDirection();
    }
}

void HunterFrame::OnSearchNewSave(wxCommandEvent &event)
{
    if (!m_log)
        OnFileNew(event);
    else
        OnFileSave(event);
}

void HunterFrame::OnSearchOpenClose(wxCommandEvent &event)
{
    if (!m_log)
        OnFileOpen(event);
    else
        OnFileClose(event);
}

void HunterFrame::OnSearchToggle(wxCommandEvent &event)
{
	if (!m_capture)
        StartCaptureAutomatic();
	else
        StopCapture();
}

void HunterFrame::StartCaptureAutomatic()
{
    m_capture = true;
    m_one_shot = false;
    XRCCTRL(*this, "search_toggle_button", wxButton)->SetLabel(_T("Stop"));
    XRCCTRL(*this, "search_once_button", wxButton)->Enable(false);
}

void HunterFrame::StartCaptureOnce()
{
    m_capture = true;
    m_one_shot = true;
    XRCCTRL(*this, "search_once_button", wxButton)->SetLabel(_T("Cancel"));
    XRCCTRL(*this, "search_toggle_button", wxButton)->Enable(false);
}

void HunterFrame::StopCapture()
{
    m_capture = false;
    m_one_shot = false;
    XRCCTRL(*this, "search_toggle_button", wxButton)->Enable(true);
    XRCCTRL(*this, "search_once_button", wxButton)->Enable(true);
    XRCCTRL(*this, "search_toggle_button", wxButton)->SetLabel(_T("Automatic"));
    XRCCTRL(*this, "search_once_button", wxButton)->SetLabel(_T("One Shot"));
}

void HunterFrame::OnSearchOnce(wxCommandEvent &event)
{
    if (!m_capture)
        StartCaptureOnce();
    else
        StopCapture();
}

void HunterFrame::OnDisplayOrientation(wxCommandEvent &event)
{
    int selection = XRCCTRL(*this, "display_orientation_radiobox", wxRadioBox)->GetSelection();
    if (selection == 0)
        m_tactical_panel->SetOrientation(TrackUp);
    else
        m_tactical_panel->SetOrientation(NorthUp);

    m_settings->SetDisplayOrientation(selection);
}

void HunterFrame::OnDisplayDoppler(wxCommandEvent &event)
{
    bool checked = event.IsChecked();
    m_tactical_panel->SetDisplayDoppler(checked);
    m_settings->SetDisplayDoppler(checked);
}

void HunterFrame::OnDisplayKnown(wxCommandEvent &event)
{
    bool checked = event.IsChecked();
    m_tactical_panel->SetDisplayKnown(checked);
    m_settings->SetDisplayKnown(checked);
}

void HunterFrame::OnDisplayEstimated(wxCommandEvent &event)
{
    bool checked = event.IsChecked();
    m_tactical_panel->SetDisplayEstimated(checked);
    m_settings->SetDisplayEstimated(checked);
}

void HunterFrame::OnHistogramSourceChg(wxCommandEvent &event)
{
	m_histogram_source = XRCCTRL(*this, "statistics_source_radiobox", wxRadioBox)->GetSelection();
    if (m_histogram_source == 0)
        m_error_histogram_panel->SetData(m_search.Histogram());
    else if (m_histogram_source == 1)
        m_error_histogram_panel->SetData(m_known.Histogram());
    // TODO: remember this in m_settings
    Refresh(); // Needed?
}

void HunterFrame::OnHistogramCoordsChg(wxCommandEvent &event)
{
	int n = XRCCTRL(*this, "statistics_coords_radiobox", wxRadioBox)->GetSelection();
    if (n == 0)
        m_error_histogram_panel->SetMode(HistogramPanel::Rectangular);
    else if (n == 1)
        m_error_histogram_panel->SetMode(HistogramPanel::Polar);
    // TODO: remember this in m_settings
}
