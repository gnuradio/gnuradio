/* -*- c++ -*- */
/*
 * Copyright 2006, 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_AUDIO_OSX_IMPL_H
#define INCLUDED_AUDIO_OSX_IMPL_H

#include <gnuradio/audio/api.h>

#include <iostream>
#include <vector>

#include <string.h>

#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>

// Check the version of MacOSX being used
#ifdef __APPLE_CC__
#include <AvailabilityMacros.h>
#ifndef MAC_OS_X_VERSION_10_6
#define MAC_OS_X_VERSION_10_6 1060
#endif
#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_6
#define GR_USE_OLD_AUDIO_UNIT
#endif
#endif

// helper function to print an ASBD

std::ostream& GR_AUDIO_API operator<<(std::ostream& s,
                                      const AudioStreamBasicDescription& asbd);

namespace gr {
namespace audio {
namespace osx {

// returns the number of channels for the provided AudioDeviceID,
// input and/or output depending on if the pointer is valid.

void GR_AUDIO_API get_num_channels_for_audio_device_id(AudioDeviceID ad_id,
                                                       UInt32* n_input,
                                                       UInt32* n_output);

// search all known audio devices, input or output, for all that
// match the provided device_name string (in part or in whole).
// Returns a vector of all matching IDs, and another of all
// matching names.  If the device name is empty, then match all
// input or output devices.

void GR_AUDIO_API find_audio_devices(const std::string& device_name,
                                     bool is_input,
                                     std::vector<AudioDeviceID>* all_ad_ids,
                                     std::vector<std::string>* all_names);

} /* namespace osx */
} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_OSX_IMPL_H */
