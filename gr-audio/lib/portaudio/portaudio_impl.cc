/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "portaudio_impl.h"
#include <portaudio.h>
#include <string_view>
#include <tuple>
#include <vector>

namespace gr {
namespace audio {

PaDeviceIndex pa_find_device_by_name(const char* name)
{
    const std::string_view namesv{ name };
    const int numDevices = Pa_GetDeviceCount();
    for (int i = 0; i < numDevices; i++) {
        if (namesv == Pa_GetDeviceInfo(i)->name) {
            return i;
        }
    }
    return paNoDevice;
}

void print_devices(const gr::logger_ptr& logger)
{
    bool defaultDisplayed;

    const int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0)
        return;

    logger->info("Number of devices found = {:d}", numDevices);

    for (int i = 0; i < numDevices; i++) {
        auto* deviceInfo = Pa_GetDeviceInfo(i);
        logger->info("--------------------------------------- device #{:d}", i);
        /* Mark global and API specific default devices */
        defaultDisplayed = false;
        if (i == Pa_GetDefaultInputDevice()) {
            logger->info("[ Default Input ]");
            defaultDisplayed = true;
        } else if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultInputDevice) {
            const PaHostApiInfo* hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
            logger->info("[ Default {} Input ]", hostInfo->name);
            defaultDisplayed = true;
        }

        if (i == Pa_GetDefaultOutputDevice()) {
            logger->info("{} Default Output {}",
                         defaultDisplayed ? "," : "[",
                         defaultDisplayed ? " " : "]");
            defaultDisplayed = true;
        } else if (i == Pa_GetHostApiInfo(deviceInfo->hostApi)->defaultOutputDevice) {
            const PaHostApiInfo* hostInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
            logger->info("{} Default {} Output {}",
                         defaultDisplayed ? "," : "[",
                         hostInfo->name,
                         defaultDisplayed ? " " : "]");
        }


        /* print device info fields */
        logger->info("Name                        = {}", deviceInfo->name);
        logger->info("Host API                    = {}",
                     Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
        logger->info("Max inputs = {:d}", deviceInfo->maxInputChannels);
        logger->info(", Max outputs = {:d}", deviceInfo->maxOutputChannels);

        logger->info("Default low input latency   = {:8.3f}",
                     deviceInfo->defaultLowInputLatency);
        logger->info("Default low output latency  = {:8.3f}",
                     deviceInfo->defaultLowOutputLatency);
        logger->info("Default high input latency  = {:8.3f}",
                     deviceInfo->defaultHighInputLatency);
        logger->info("Default high output latency = {:8.3f}",
                     deviceInfo->defaultHighOutputLatency);
    }
}

} /* namespace audio */
} /* namespace gr */
