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
#include "sample.h"
#include "known.h"
#include "search.h"

// wxWidgets includes
#include <wx/frame.h>
#include <wx/spinbutt.h>  // Unknown why these two aren't in wx.h
#include <wx/splitter.h>

// Forward declarations
class EZDoppler;
class EZDopplerUpdate;
class GPS;
class GPSUpdate;
class HunterSettings;
class TacticalPanel;
class HistogramPanel;
class Spherical;
class SampleLog;
class KnownTransmitter;

class HunterFrame : public wxFrame
{
public:
	// Constructor
	HunterFrame();
    void DoCalibrationStep(int which);
    
private:
	// Initialization routines
    void InitializeSettings();    
	void InitializeWindows();
	void InitializeDoppler();
    void InitializeGPS();
    void InitializeCalibration();
    void InitializeSearch();
    void InitializeDisplay();

    // System related functions
	void OnClose(wxCloseEvent &event);
    
    // Status related functions
    void OnHistogramSourceChg(wxCommandEvent &event);
    void OnHistogramCoordsChg(wxCommandEvent &event);

    // Search related functions
    void EnableSearchItems(bool enable);
    void MakeNewLogAndSearch();
    void CalcSolution();
    void StartCaptureAutomatic();
    void StartCaptureOnce();
    void StopCapture();
    void UpdateSearchStatus(bool display);
    void UpdateSearchLocation(bool display);
    void UpdateSearchDirection(bool display);
    void OnSearchNewSave(wxCommandEvent &event);
    void OnSearchOpenClose(wxCommandEvent &event);
    void OnSearchToggle(wxCommandEvent &event);
    void OnSearchOnce(wxCommandEvent &event);
    void OnSearchUpdate(SearchUpdate &event);                
    
    // Doppler related functions
    void SetDopplerParams();
    void StartDoppler();
    void StopDoppler();
    void UpdateDopplerStatus(bool display);
	void OnDopplerToggle(wxCommandEvent &event);
	void OnDopplerAutostart(wxCommandEvent &event);
	void OnDopplerReset(wxCommandEvent &event);
	void OnDopplerFilterChg(wxScrollEvent &event);
	void OnDopplerRotationChg(wxCommandEvent &event);
    void OnDopplerUpdate(EZDopplerUpdate &event);                

    // GPS related functions
    void StartGPS();
    void StopGPS();
    void UpdateGPSStatus(bool dipslay);
    void UpdateGPSValidity(bool valid);
    void OnGPSToggle(wxCommandEvent &event);
    void OnGPSAutostart(wxCommandEvent &event);
    void OnGPSDeviceSelect(wxCommandEvent &event);
    void OnGPSUpdate(GPSUpdate &event);                
        	
    // Calibration related functions
    void CalcKnownStatistics();
    void UpdateKnownLocation();
    void UpdateKnownDirection();
    void UpdateKnownStatistics();
    void OnCalibrationEqualize(wxCommandEvent &event);
    void OnCalibrationZero(wxCommandEvent &event);
    void OnCalibrationAdjustLeft(wxSpinEvent &event);
    void OnCalibrationAdjustRight(wxSpinEvent &event);
    void OnCalibrationAffectAllRates(wxCommandEvent &event);
    void OnKnownTransmitterUpdate(wxCommandEvent &event);
    void OnUseKnownTransmitter(wxCommandEvent &event);
        
    // Tactical display related functions
    void OnDisplayOrientation(wxCommandEvent &event);
    void OnDisplayDoppler(wxCommandEvent &event);
    void OnDisplayKnown(wxCommandEvent &event);
    void OnDisplayEstimated(wxCommandEvent &event);
    
	// Menu event handlers
	void OnExit(wxCommandEvent &event);
	void OnAbout(wxCommandEvent &event);
    void OnFileNew(wxCommandEvent &event);
    void OnFileOpen(wxCommandEvent &event);
    void OnFileClose(wxCommandEvent &event);
    void OnFileSave(wxCommandEvent &event);
    void OnCalibrationSaveToFile(wxCommandEvent &event);
    void OnCalibrationLoadFromFile(wxCommandEvent &event);
    void OnCalibrationLoadTransmitter(wxCommandEvent &event);
    void OnCalibrationSaveTransmitter(wxCommandEvent &event);
            
	// Member data
    HunterSettings *m_settings;         // Configuration file
	EZDoppler *m_doppler;               // Attached Doppler device
    GPS *m_gps;                         // Attached GPS device
    SampleLog *m_log;                   // Accumulated sample points
    KnownTransmitter m_known;           // Identified known transmitter location for calibration
    TransmitterSearch m_search;         // Search being conducted
    
	bool m_doppler_started;             // Tracks start/stop of doppler device
    bool m_gps_started;                 // Tracks start/stop of GPS device
    bool m_capture;                     // Tracks start/stop of bearing capture
    bool m_one_shot;                    // Tracks whether capture is one bearing only
    int  m_histogram_source;            // Tracks histogram data source (0=estimated, 1=actual);
    
    Sample m_sample;                    // Current sample being updated by doppler and GPS
                    
    // Child windows that need remembering
    TacticalPanel *m_tactical_panel;
    HistogramPanel *m_error_histogram_panel;
    
	// This class handles events
	DECLARE_EVENT_TABLE();
};
