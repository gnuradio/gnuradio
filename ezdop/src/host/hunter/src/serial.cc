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

#include "serial.h"

#include <wx/log.h>

#ifdef __WIN32__
// I hate Windows.
#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

wxArrayString EnumerateSerialPorts()
{
    wxArrayString result;
    
#ifdef __WIN32__
    wxString port;
    for (int i = 1; i <= 8; i++) {
        port.Printf("COM%i", i);
        result.Add(port);
    }
#else
    result.Add(_T("/dev/ttyS0"));
    result.Add(_T("/dev/ttyS1"));
    result.Add(_T("/dev/ttyS2"));
    result.Add(_T("/dev/ttyS3"));
    result.Add(_T("/dev/ttyUSB0"));
    result.Add(_T("/dev/ttyUSB1"));
#endif

    return result;
}

SerialPort::SerialPort(wxString &port)
{
    wxLogDebug(_T("SerialPort::SerialPort(): %s"), port.c_str());
    m_port = port;
    m_opened = false;
#ifdef __WIN32__
    m_handle = INVALID_HANDLE_VALUE;
#else
    m_fd = -1;
#endif
}

bool SerialPort::Open(int speed)
{
    wxLogDebug(_T("SerialPort::Open: %i baud"), speed);
    if (m_opened) {
        wxLogWarning(_T("SerialPort::Open: called on already opened object."));
        return false;
    }

#ifdef __WIN32__
    m_handle = CreateFile(m_port.c_str(), 
                          GENERIC_READ | GENERIC_WRITE,
                          0, 
                          NULL, 
                          OPEN_EXISTING, 
                          0, 
                          NULL);
    if (m_handle == INVALID_HANDLE_VALUE) {
        wxLogError("SerialPort::Open: CreateFile() failed");
        return false;
    }

    DCB dcb;
    if (!GetCommState(m_handle, &dcb)) {
        wxLogError("SerialPort::Open: GetCommState failed.");
        CloseHandle(m_handle);
        return false;
    }

    dcb.BaudRate = speed;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    dcb.fBinary = TRUE;
    dcb.fParity = FALSE;

    if (!SetCommState(m_handle, &dcb)) {
        wxLogError("SerialPort::Open: SetCommState failed.");
        CloseHandle(m_handle);
        return false;
    }
    
    COMMTIMEOUTS timeouts;
    if (!GetCommTimeouts(m_handle, &timeouts)) {
        wxLogError("SerialPort::Open: GetCommTimeouts failed.");
        CloseHandle(m_handle);
        return false;
    }

    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 100;

    if (!SetCommTimeouts(m_handle, &timeouts)) {
        wxLogError("SerialPort::Open: SetCommTimeouts failed.");
        CloseHandle(m_handle);
        return false;
    }

    m_opened = true;
#else
    m_fd = open((char *)m_port.c_str(), O_RDWR|O_NONBLOCK);
    if (m_fd < 0) {
        wxLogError(_T("SerialPort::Open: open() returned %i"), m_fd);
        return false;
    }

    if (!isatty(m_fd)) {
        wxLogError(_T("SerialPort::Open: device %s is not a tty"), m_port.c_str());
        close(m_fd);
        return false;
    }

    if (tcgetattr(m_fd, &m_ttyset_old) != 0) {
        wxLogError(_T("SerialPort::Open: failed to get port attributes"));
        close(m_fd);
        return false;
    }

    memcpy(&m_ttyset_new, &m_ttyset_old, sizeof(m_ttyset_new));
	cfsetispeed(&m_ttyset_new, (speed_t)speed);
	cfsetospeed(&m_ttyset_new, (speed_t)speed);
	m_ttyset_new.c_cflag &= ~(PARENB|CRTSCTS); // Disable parity and flowcontrol
	m_ttyset_new.c_cflag |= CS8|CREAD|CLOCAL;  // 8 bits, read, no modem status lines
	m_ttyset_new.c_iflag = 0;
	m_ttyset_new.c_oflag = ONLCR;              // Convert LF to CRLF on receive
	m_ttyset_new.c_lflag = 0;

	if (tcsetattr(m_fd, TCSANOW, &m_ttyset_new) != 0) {
        wxLogError(_T("SerialPort::Open: failed to set port attributes"));
        close(m_fd);
	    return false;
    }

    m_opened = true;
#endif
    return m_opened;
}

void SerialPort::Close()
{
    wxLogDebug(_T("SerialPort::Close()"));
#ifdef __WIN32__
    CloseHandle(m_handle);
#else
    if (m_opened >= 0) {
        m_ttyset_old.c_cflag |= HUPCL;
        tcsetattr(m_fd, TCSANOW, &m_ttyset_old);
        close(m_fd);
    }
#endif

    m_opened = false;
}

SerialPort::~SerialPort()
{
    wxLogDebug(_T("SerialPort::~SerialPort()"));

    if (m_opened)
        Close();
}

int SerialPort::RxReady()
{
    int count = 0;
#ifdef __WIN32__
    return 1; // No equivalent Win32 call, use read timeouts instead
#else
    if (m_fd < 0 || ioctl(m_fd, FIONREAD, &count) < 0)
        return -1;
#endif
    return count;
}

int SerialPort::Read(char *buffer, int len)
{
    wxASSERT(buffer);
    wxASSERT(len);

    if (!m_opened)
        return -1;
        
#ifdef __WIN32__
    DWORD num;
    if (ReadFile(m_handle, buffer, (DWORD)len, &num, NULL))
        return num;
    else
        return -1;
#else
    return read(m_fd, buffer, len);
#endif
}
