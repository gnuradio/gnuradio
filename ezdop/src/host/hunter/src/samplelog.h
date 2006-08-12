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

#ifndef __SAMPLELOG_H__
#define __SAMPLELOG_H__

// Application level includes
#include "sample.h"

// wxWidgets includes
#include <wx/string.h>
#include <wx/event.h>

// System level includes
#include <vector>

// Forward declarations
class wxFile;

class SampleLog
{
public:
    SampleLog();
    
    // Sample access
    void Add(Sample &sample);
    int  Count() const { return m_samples.size(); }
    wxMutex &Mutex() { return m_mutex; }
    std::vector<Sample>& Samples() { return m_samples; }

    // File operations
    void Load(wxString filename);
    bool Save();
    bool Save(wxString &filename);
    bool HasFile() const { return (m_file != NULL); }
    bool IsDirty() const { return m_save >= 0; }

private:
    bool readline(wxString &line);

    std::vector<Sample> m_samples;
    wxMutex m_mutex;
    int m_save;
    wxString m_filename;
    wxFile *m_file;
};

#endif
