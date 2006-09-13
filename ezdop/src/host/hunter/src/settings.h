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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

// wxWidgets includes
#include <wx/config.h>
#include <wx/gdicmn.h>

class HunterSettings
{
public:
    HunterSettings();
    ~HunterSettings();
    
    // Main window size
    wxSize GetWindowSize();
    void SetWindowSize(wxSize size);

    // Main window position
    int GetWindowXPos();
    void SetWindowXPos(int x);
    int GetWindowYPos();
    void SetWindowYPos(int y);

    // Autostart doppler on application bringup
    bool GetDopplerAutostart();
    void SetDopplerAutostart(bool val);

    // Doppler filter value
    int GetDopplerFilter();
    void SetDopplerFilter(int level);

    // Doppler filter value
    int GetDopplerRotation();
    void SetDopplerRotation(int rate);

    // Doppler calibration values
    float GetDopplerCalibration(int rate);
    void SetDopplerCalibration(int rate, float offset);
        
    // Autostart GPS on application bringup
    bool GetGPSAutostart();
    void SetGPSAutostart(bool val);

    // GPS interface device
    wxString GetGPSDeviceName();
    void SetGPSDeviceName(wxString &name);

    // Calibration adjust affects all rates
    bool GetCalibrationAffectAllRates();
    void SetCalibrationAffectAllRates(bool val);

    // Known transmitter location
    double GetKnownTransmitterLongitude();
    void SetKnownTransmitterLongitude(double lon);
    double GetKnownTransmitterLatitude();
    void SetKnownTransmitterLatitude(double lat);
    bool GetUseKnownTransmitter();
    void SetUseKnownTransmitter(bool use);

    // Display Orientation
    int GetDisplayOrientation();
    void SetDisplayOrientation(int orientation);

    // Display Doppler Bearing
    bool GetDisplayDoppler();
    void SetDisplayDoppler(bool val);

    // Display Known Bearing
    bool GetDisplayKnown();
    void SetDisplayKnown(bool val);

    // Display Estimated Bearing
    bool GetDisplayEstimated();
    void SetDisplayEstimated(bool val);

    wxString GetWorkingDirectory();
    void SetWorkingDirectory(const wxString &dir);

private:
    wxConfig *cfg;
};

#endif // __SETTINGS_H__
