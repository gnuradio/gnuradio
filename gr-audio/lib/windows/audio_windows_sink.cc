/* -*- c++ -*- */
/*
* Copyright 2004-2011 Free Software Foundation, Inc.
*
* This file is part of GNU Radio
*
* GNU Radio is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* GNU Radio is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with GNU Radio; see the file COPYING.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street,
* Boston, MA 02110-1301, USA.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gr_audio_registry.h"
#include <audio_windows_sink.h>
#include <gr_io_signature.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

AUDIO_REGISTER_SINK(REG_PRIO_HIGH, windows)(
    int sampling_rate, const std::string &device_name, bool
){
    return audio_sink::sptr(new audio_windows_sink(sampling_rate, device_name));
}

static const double CHUNK_TIME = 0.1;	//0.001;           // 100 ms

// FIXME these should query some kind of user preference

static std::string
default_device_name ()
{
  return "WAVE_MAPPER";
}

audio_windows_sink::audio_windows_sink (int sampling_freq, const std::string device_name)
  : gr_sync_block ("audio_windows_sink",
		   gr_make_io_signature (1, 2, sizeof (float)),
		   gr_make_io_signature (0, 0, 0)),
    d_sampling_freq (sampling_freq),
    d_device_name (device_name.empty ()? default_device_name () : device_name),
    d_fd (-1), d_buffer (0), d_chunk_size (0)
{
  d_wave_write_event = CreateEvent (NULL, FALSE, FALSE, NULL);
  if (open_waveout_device () < 0)
    {
      //fprintf (stderr, "audio_windows_sink:open_waveout_device() failed\n");
      perror ("audio_windows_sink:open_waveout_device( ) failed\n");
      throw
      std::runtime_error ("audio_windows_sink:open_waveout_device() failed");
    }

  d_chunk_size = (int) (d_sampling_freq * CHUNK_TIME);
  set_output_multiple (d_chunk_size);

  d_buffer = new short[d_chunk_size * 2];

}

audio_windows_sink::~audio_windows_sink ()
{
  /* Free the callback Event */
  CloseHandle (d_wave_write_event);
  waveOutClose (d_h_waveout);
  delete[]d_buffer;
}

int
audio_windows_sink::work (int noutput_items,
			  gr_vector_const_void_star & input_items,
			  gr_vector_void_star & output_items)
{
  const float *f0, *f1;
  bool playtestsound = false;
  if (playtestsound)
    {
      // dummy

      f0 = (const float *) input_items[0];

      for (int i = 0; i < noutput_items; i += d_chunk_size)
	{
	  for (int j = 0; j < d_chunk_size; j++)
	    {
	      d_buffer[2 * j + 0] = (short) (sin (2.0 * 3.1415926535897932384626 * (float) j * 1000.0 / (float) d_sampling_freq) * 8192 + 0);	//+32767
	      d_buffer[2 * j + 1] = d_buffer[2 * j + 0];
	    }
	  f0 += d_chunk_size;
	  if (write_waveout
	      ((HPSTR) d_buffer, 2 * d_chunk_size * sizeof (short)) < 0)
	    {
	      fprintf (stderr, "audio_windows_sink: write failed\n");
	      perror ("audio_windows_sink: write failed");
	    }
	}
      // break;
    }
  else
    {
      switch (input_items.size ())
	{

	case 1:		// mono input

	  f0 = (const float *) input_items[0];

	  for (int i = 0; i < noutput_items; i += d_chunk_size)
	    {
	      for (int j = 0; j < d_chunk_size; j++)
		{
		  d_buffer[2 * j + 0] = (short) (f0[j] * 32767);
		  d_buffer[2 * j + 1] = (short) (f0[j] * 32767);
		}
	      f0 += d_chunk_size;
	      if (write_waveout
		  ((HPSTR) d_buffer, 2 * d_chunk_size * sizeof (short)) < 0)
		{
		  //fprintf (stderr, "audio_windows_sink: write failed\n");
		  perror ("audio_windows_sink: write failed");
		}
	    }
	  break;

	case 2:		// stereo input

	  f0 = (const float *) input_items[0];
	  f1 = (const float *) input_items[1];

	  for (int i = 0; i < noutput_items; i += d_chunk_size)
	    {
	      for (int j = 0; j < d_chunk_size; j++)
		{
		  d_buffer[2 * j + 0] = (short) (f0[j] * 32767);
		  d_buffer[2 * j + 1] = (short) (f1[j] * 32767);
		}
	      f0 += d_chunk_size;
	      f1 += d_chunk_size;
	      if (write_waveout
		  ((HPSTR) d_buffer, 2 * d_chunk_size * sizeof (short)) < 0)
		{
		  //fprintf (stderr, "audio_windows_sink: write failed\n");
		  perror ("audio_windows_sink: write failed");
		}
	    }
	  break;
	}
    }
  return noutput_items;
}

int
audio_windows_sink::string_to_int (const std::string & s)
{
  int i;
  std::istringstream (s) >> i;
  return i;
}				//ToInt()

int
audio_windows_sink::open_waveout_device (void)
{

  UINT /*UINT_PTR */ u_device_id;
	/** Identifier of the waveform-audio output device to open. It can be either a device identifier or a handle of an open waveform-audio input device. You can use the following flag instead of a device identifier.
	*
	* Value Meaning
	* WAVE_MAPPER The function selects a waveform-audio output device capable of playing the given format.
	*/
  if (d_device_name.empty () || default_device_name () == d_device_name)
    u_device_id = WAVE_MAPPER;
  else
    u_device_id = (UINT) string_to_int (d_device_name);
  // Open a waveform device for output using event callback.

  unsigned long result;
  //HWAVEOUT      outHandle;
  WAVEFORMATEX wave_format;

  /* Initialize the WAVEFORMATEX for 16-bit, 44KHz, stereo */
  wave_format.wFormatTag = WAVE_FORMAT_PCM;
  wave_format.nChannels = 2;
  wave_format.nSamplesPerSec = d_sampling_freq;	//44100;
  wave_format.wBitsPerSample = 16;
  wave_format.nBlockAlign =
    wave_format.nChannels * (wave_format.wBitsPerSample / 8);
  wave_format.nAvgBytesPerSec =
    wave_format.nSamplesPerSec * wave_format.nBlockAlign;
  wave_format.cbSize = 0;

  /* Open the (preferred) Digital Audio Out device. */
  result = waveOutOpen (&d_h_waveout, WAVE_MAPPER, &wave_format, (DWORD_PTR) d_wave_write_event, 0, CALLBACK_EVENT | WAVE_ALLOWSYNC);	//|WAVE_FORMAT_DIRECT | CALLBACK_EVENT| WAVE_ALLOWSYNC
  if (result)
    {
      fprintf (stderr,
	       "audio_windows_sink: Failed to open waveform output device.\n");
      perror ("audio_windows_sink: Failed to open waveform output device.");
      //LocalUnlock(hFormat);
      //LocalFree(hFormat);
      //mmioClose(hmmio, 0);
      return -1;
    }

  //
  // Do not Swallow the "open" event.
  //
  //WaitForSingleObject(d_wave_write_event, INFINITE);

  // Allocate and lock memory for the header.

  d_h_wave_hdr = GlobalAlloc (GMEM_MOVEABLE | GMEM_SHARE,
			      (DWORD) sizeof (WAVEHDR));
  if (d_h_wave_hdr == NULL)
    {
      //GlobalUnlock(hData);
      //GlobalFree(hData);
      //fprintf (stderr, "audio_windows_sink: Not enough memory for header.\n");
      perror ("audio_windows_sink: Not enough memory for header.");
      return -1;
    }

  d_lp_wave_hdr = (LPWAVEHDR) GlobalLock (d_h_wave_hdr);
  if (d_lp_wave_hdr == NULL)
    {
      //GlobalUnlock(hData);
      //GlobalFree(hData);
      //fprintf (stderr, "audio_windows_sink: Failed to lock memory for header.\n");
      perror ("audio_windows_sink: Failed to lock memory for header.");
      return -1;
    }
  //d_lp_wave_hdr->dwFlags = WHDR_DONE;
  return 0;
}

int
audio_windows_sink::write_waveout (HPSTR lp_data, DWORD dw_data_size)
{
  UINT w_result;
  int teller = 100;
  // After allocation, set up and prepare header.
  /*while ((d_lp_wave_hdr->dwFlags & WHDR_DONE)==0 && teller>0)
     {
     teller--;
     Sleep(1);
     } */
  // Wait until previous wave write completes (first event is the open event).
  WaitForSingleObject (d_wave_write_event, 100);	//INFINITE
  d_lp_wave_hdr->lpData = lp_data;
  d_lp_wave_hdr->dwBufferLength = dw_data_size;
  d_lp_wave_hdr->dwFlags = 0L;
  /* Clear the WHDR_DONE bit (which the driver set last time that
     this WAVEHDR was sent via waveOutWrite and was played). Some
     drivers need this to be cleared */
  //d_lp_wave_hdr->dwFlags &= ~WHDR_DONE;

  d_lp_wave_hdr->dwLoops = 0L;
  w_result =
    waveOutPrepareHeader (d_h_waveout, d_lp_wave_hdr, sizeof (WAVEHDR));
  if (w_result != 0)
    {
      //GlobalUnlock( hData);
      //GlobalFree(hData);
      //fprintf (stderr, "audio_windows_sink: Failed to waveOutPrepareHeader. error %i\n",w_result);
      perror ("audio_windows_sink: Failed to waveOutPrepareHeader");
    }
  // Now the data block can be sent to the output device. The
  // waveOutWrite function returns immediately and waveform
  // data is sent to the output device in the background.
  //while (!  readyforplayback) Sleep(1);
  //readyforplayback=false;
  //
  //

  w_result = waveOutWrite (d_h_waveout, d_lp_wave_hdr, sizeof (WAVEHDR));
  if (w_result != 0)
    {
      //GlobalUnlock( hData);
      //GlobalFree(hData);
      //fprintf (stderr, "audio_windows_sink: Failed to write block to device.error %i\n",w_result);
      perror ("audio_windows_sink: Failed to write block to device");
      switch (w_result)
	{
	case MMSYSERR_INVALHANDLE:
	  fprintf (stderr, "Specified device handle is invalid. \n");
	  break;
	case MMSYSERR_NODRIVER:
	  fprintf (stderr, " No device driver is present.  \n");
	  break;
	case MMSYSERR_NOMEM:
	  fprintf (stderr, " Unable to allocate or lock memory.  \n");
	  break;
	case WAVERR_UNPREPARED:
	  fprintf (stderr,
		   " The data block pointed to by the pwh parameter hasn't been prepared.  \n");
	  break;
	default:
	  fprintf (stderr, "Unknown error %i\n", w_result);
	}
      waveOutUnprepareHeader (d_h_waveout, d_lp_wave_hdr, sizeof (WAVEHDR));
      return -1;
    }
  //   WaitForSingleObject(d_wave_write_event, INFINITE);
  return 0;
}
