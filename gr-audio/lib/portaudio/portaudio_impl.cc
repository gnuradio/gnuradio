/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
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

#include <portaudio_impl.h>
#include <portaudio.h>
#include <string.h>

namespace gr {
  namespace audio {

    PaDeviceIndex
    pa_find_device_by_name(const char *name)
    {
      int i;
      int numDevices;
      const PaDeviceInfo *pdi;
      int len = strlen(name);
      PaDeviceIndex result = paNoDevice;
      numDevices = Pa_GetDeviceCount();
      for(i = 0; i < numDevices; i++) {
        pdi = Pa_GetDeviceInfo(i);
        if(strncmp(name, pdi->name, len) == 0) {
          result = i;
          break;
        }
      }
      return result;
    }

    void
    print_devices()
    {
      int numDevices, defaultDisplayed;
      const PaDeviceInfo *deviceInfo;

      numDevices = Pa_GetDeviceCount();
      if(numDevices < 0)
        return;

      printf("Number of devices found = %d\n", numDevices);

      for(int i = 0; i < numDevices; i++) {
        deviceInfo = Pa_GetDeviceInfo(i);
        printf("--------------------------------------- device #%d\n", i);
        /* Mark global and API specific default devices */
        defaultDisplayed = 0;
        if(i == Pa_GetDefaultInputDevice()) {
          printf("[ Default Input");
          defaultDisplayed = 1;
        }
        else if(i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultInputDevice) {
          const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
          printf("[ Default %s Input", hostInfo->name);
          defaultDisplayed = 1;
        }

        if(i == Pa_GetDefaultOutputDevice()) {
          printf((defaultDisplayed ? "," : "["));
          printf(" Default Output");
          defaultDisplayed = 1;
        }
        else if(i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultOutputDevice) {
          const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
          printf((defaultDisplayed ? "," : "["));
          printf(" Default %s Output", hostInfo->name);
          defaultDisplayed = 1;
        }
        if(defaultDisplayed)
          printf(" ]\n");

        /* print device info fields */
        printf("Name                        = %s\n", deviceInfo->name);
        printf("Host API                    = %s\n", Pa_GetHostApiInfo(deviceInfo->hostApi)->name );
        printf("Max inputs = %d", deviceInfo->maxInputChannels);
        printf(", Max outputs = %d\n", deviceInfo->maxOutputChannels);

        printf("Default low input latency   = %8.3f\n", deviceInfo->defaultLowInputLatency);
        printf("Default low output latency  = %8.3f\n", deviceInfo->defaultLowOutputLatency);
        printf("Default high input latency  = %8.3f\n", deviceInfo->defaultHighInputLatency);
        printf("Default high output latency = %8.3f\n", deviceInfo->defaultHighOutputLatency);
      }
    }

  } /* namespace audio */
} /* namespace gr */
