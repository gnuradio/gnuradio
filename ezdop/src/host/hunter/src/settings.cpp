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
#include "settings.h"

HunterSettings::HunterSettings()
{
	cfg = new wxConfig(_T("TransmitterHunter"));
}

HunterSettings::~HunterSettings()
{
	if (cfg) {
		cfg->Flush();
		delete cfg;
	}
}

wxSize HunterSettings::GetWindowSize()
{
    long width, height;
    if (cfg) {
        cfg->Read(wxT("Application/Width"), &width, 1024);
        cfg->Read(wxT("Application/Height"), &height, 768);
    }
    
    return wxSize((int)width, (int)height);
}

void HunterSettings::SetWindowSize(wxSize size)
{
    if (cfg) {
        cfg->Write(wxT("Application/Width"), size.GetWidth());
        cfg->Write(wxT("Application/Height"), size.GetHeight());
    }
}

int HunterSettings::GetWindowXPos()
{
	long x;
	if (cfg)
		cfg->Read(wxT("Application/XPos"), &x, 0);

	return (int)x;
}

void HunterSettings::SetWindowXPos(int x)
{
    if (cfg)
        cfg->Write(wxT("Application/XPos"), (long)x);
}

int HunterSettings::GetWindowYPos()
{
	long y;
	if (cfg)
		cfg->Read(wxT("Application/YPos"), &y, 0);

	return (int)y;
}

void HunterSettings::SetWindowYPos(int y)
{
    if (cfg)
        cfg->Write(wxT("Application/YPos"), (long)y);
}

bool HunterSettings::GetDopplerAutostart()
{
	bool start = false;
	if (cfg)
		cfg->Read(wxT("Doppler/Autostart"), &start, false);

	return start;
}

void HunterSettings::SetDopplerAutostart(bool start)
{
	if (cfg)
		cfg->Write(wxT("Doppler/Autostart"), start);
}

int HunterSettings::GetDopplerFilter()
{
	long filtering;
	if (cfg)
		cfg->Read(wxT("Doppler/FilterLevel"), &filtering, 20);

	return (int)filtering;
}

void HunterSettings::SetDopplerFilter(int level)
{
    if (cfg)
        cfg->Write(wxT("Doppler/FilterLevel"), (long)level);
}

int HunterSettings::GetDopplerRotation()
{
	long rate;
	if (cfg)
		cfg->Read(wxT("Doppler/Rotation"), &rate, 4);

	return (int)rate;
}

void HunterSettings::SetDopplerRotation(int rate)
{
    if (cfg)
        cfg->Write(wxT("Doppler/Rotation"), (long)rate);
}

float HunterSettings::GetDopplerCalibration(int rate)
{
	double calibration;
	wxString key;

	key.Printf(_T("Doppler/Rate%iCalibration"), rate);
	if (cfg)
		cfg->Read(key, &calibration, 0.0);
	return (float)calibration;
}

void HunterSettings::SetDopplerCalibration(int rate, float offset)
{
	wxString key;
	key.Printf(_T("Doppler/Rate%iCalibration"), rate);
	if (cfg)
		cfg->Write(key, offset);
}

bool HunterSettings::GetGPSAutostart()
{
	bool start = false;
	if (cfg)
		cfg->Read(wxT("GPS/Autostart"), &start, false);

	return start;
}

void HunterSettings::SetGPSAutostart(bool start)
{
	if (cfg)
		cfg->Write(wxT("GPS/Autostart"), start);
}

wxString HunterSettings::GetGPSDeviceName()
{
    wxString name;
    if (cfg)
        cfg->Read(wxT("GPS/DeviceName"), &name);
        
    return name; 
}

void HunterSettings::SetGPSDeviceName(wxString &name)
{
    if (cfg)
        cfg->Write(wxT("GPS/DeviceName"), name);
}

bool HunterSettings::GetCalibrationAffectAllRates()
{
	bool val = false;
	if (cfg)
		cfg->Read(wxT("Calibration/AffectAllRates"), &val, false);

	return val;
}

void HunterSettings::SetCalibrationAffectAllRates(bool val)
{
	if (cfg)
		cfg->Write(wxT("Calibration/AffectAllRates"), val);
}

double HunterSettings::GetKnownTransmitterLongitude()
{
    double lon;
    if (cfg)
        cfg->Read(wxT("KnownTransmitter/Longitude"), &lon);
        
    return lon;
}

void HunterSettings::SetKnownTransmitterLongitude(double lon)
{
    if (cfg)
        cfg->Write(wxT("KnownTransmitter/Longitude"), lon);
}

double HunterSettings::GetKnownTransmitterLatitude()
{
    double lat;
    if (cfg)
        cfg->Read(wxT("KnownTransmitter/Latitude"), &lat);
        
    return lat;
}

void HunterSettings::SetKnownTransmitterLatitude(double lat)
{
    if (cfg)
        cfg->Write(wxT("KnownTransmitter/Latitude"), lat);
}

bool HunterSettings::GetUseKnownTransmitter()
{
	bool use = false;
	if (cfg)
		cfg->Read(wxT("KnownTransmitter/Use"), &use, false);

	return use;
}

void HunterSettings::SetUseKnownTransmitter(bool use)
{
    if (cfg)
        cfg->Write(wxT("KnownTransmitter/Use"), use);
}

int HunterSettings::GetDisplayOrientation()
{
	long x;
	if (cfg)
		cfg->Read(wxT("Display/Orientation"), &x, 0);

	return (int)x;
}

void HunterSettings::SetDisplayOrientation(int orientation)
{
    if (cfg)
        cfg->Write(wxT("Display/Orientation"), (long)orientation);
}

bool HunterSettings::GetDisplayDoppler()
{
	bool val = false;
	if (cfg)
		cfg->Read(wxT("Display/DopplerPointer"), &val, false);

	return val;
}

void HunterSettings::SetDisplayDoppler(bool val)
{
    if (cfg)
        cfg->Write(wxT("Display/DopplerPointer"), val);
}

bool HunterSettings::GetDisplayKnown()
{
	bool val = false;
	if (cfg)
		cfg->Read(wxT("Display/KnownPointer"), &val, false);

	return val;
}

void HunterSettings::SetDisplayKnown(bool val)
{
    if (cfg)
        cfg->Write(wxT("Display/KnownPointer"), val);
}

bool HunterSettings::GetDisplayEstimated()
{
	bool val = false;
	if (cfg)
		cfg->Read(wxT("Display/EstimatedPointer"), &val, false);

	return val;
}

void HunterSettings::SetDisplayEstimated(bool val)
{
    if (cfg)
        cfg->Write(wxT("Display/EstimatedPointer"), val);
}

wxString HunterSettings::GetWorkingDirectory()
{
    wxString str(_T("."));
    if (cfg)
        return cfg->Read(wxT("Application/WorkingDirectory"), str);
    return str;
}

void HunterSettings::SetWorkingDirectory(const wxString &dir)
{
    if (cfg)
        cfg->Write(wxT("Application/WorkingDirectory"), dir);
}
