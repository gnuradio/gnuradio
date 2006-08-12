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

#ifndef __HUNTER_APP__
#define __HUNTER_APP__

// wxWidgets includes
#include <wx/app.h>

// Forward declarations
class wxLog;

class Hunter : public wxApp
{
public:
	// Called on application startup
	virtual bool OnInit();

	// Called by system when application is closing but
	// before wxWidgets is finished
	virtual int OnExit();
	
private:
	// Active log target
	wxLog *m_logger;
};

#endif
