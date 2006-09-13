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
#include "samplelog.h"

// wxWidgets includes
#include <wx/file.h>
#include <wx/log.h>

// System level includes
#include <cmath>

SampleLog::SampleLog()
{
    m_file = NULL;
    m_save = -1;
}

void SampleLog::Load(wxString filename)
{
    m_file = new wxFile(filename.c_str(), wxFile::read_write);
    wxString line;
            
    if (m_file && m_file->IsOpened()) {
        while (readline(line)) {
            Sample sample(line); // can't use inline temporary in next line
            Add(sample);      // Locking is handled in Add
        }
        
        m_save = -1;
    }
}

bool SampleLog::readline(wxString &line)
{
    char ch;
    size_t count;

    line.Empty();
    count = m_file->Read(&ch, 1);
    while (count == 1 && ch != '\n') {
        line.Append(ch);
        count = m_file->Read(&ch, 1);
    }

    return !line.IsEmpty();
}

void SampleLog::Add(Sample &sample)
{
    wxMutexLocker locker(m_mutex);
    
    if (m_save < 0)
        m_save = m_samples.size();

    m_samples.push_back(sample);
    return;
}

bool SampleLog::Save(wxString &filename)
{
    wxASSERT(!m_file);  // Save called with filename when it already exists is an error
    wxLogError(_T("SampleLog::Save: called with %s when file already exists"), filename.c_str());
    
    m_filename = filename;
    m_file = new wxFile(m_filename.c_str(), wxFile::write);

    return Save();
}

bool SampleLog::Save()
{
    wxASSERT(m_file);
    if (m_save < 0)
        return false;

    wxMutexLocker locker(m_mutex);
    
    char str[256];
    if (m_file && m_file->IsOpened()) {
        for (int i = m_save; i < m_samples.size(); i++) {
            m_samples[i].Dump(str, 255);
            m_file->Write(str, strlen(str));
            m_file->Write("\n", strlen("\n"));
        }
        m_save = -1;
        return true;
    }

    return false;
}
