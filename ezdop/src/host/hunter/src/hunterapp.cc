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
#include "hunterapp.h"
#include "hunter.h"

// wxWidgets includes
#include <wx/xrc/xmlres.h>
#include <wx/log.h>

// Provided in resource.cpp created from hunter.xrc by make system
extern void InitXmlResource();

bool Hunter::OnInit()
{
	m_logger = new wxLogStream();
	wxLog::SetActiveTarget(m_logger);

    // Get XML resources linked in via resource.cpp
	wxXmlResource::Get()->InitAllHandlers();
    InitXmlResource();

	HunterFrame *top = new HunterFrame();
	top->Show(true);

    // Everything Zen
	return true;
}

int Hunter::OnExit()
{
	return 0;
}

// Creates main() and WinMain() entry points
IMPLEMENT_APP(Hunter)
