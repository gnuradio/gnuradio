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

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <wx/arrstr.h>

#ifdef __WIN32__
#include <windows.h>
#else
#include <termios.h>
#endif

wxArrayString EnumerateSerialPorts();

class SerialPort
{
public:
    SerialPort(wxString &port);
    ~SerialPort();

    bool Open(int speed);
    bool IsOpened() { return m_opened; }
    void Close();
    int  RxReady();
    int  Read(char *buffer, int len);
            
private:
#ifdef __WIN32__
    HANDLE m_handle;
#else
    int m_fd;
    struct termios m_ttyset_old;
    struct termios m_ttyset_new;
#endif
    wxString m_port;
    bool m_opened;
};

#endif // __SERIAL_H__
