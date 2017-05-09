/* -*- c++ -*- */
/*
 * Copyright 2006, 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
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

std::ostream& GR_AUDIO_API
operator<<
(std::ostream& s,
 const AudioStreamBasicDescription& asbd);

namespace gr {
namespace audio {
namespace osx {

// returns the number of channels for the provided AudioDeviceID,
// input and/or output depending on if the pointer is valid.

void GR_AUDIO_API
get_num_channels_for_audio_device_id
(AudioDeviceID ad_id,
 UInt32* n_input,
 UInt32* n_output);

// search all known audio devices, input or output, for all that
// match the provided device_name string (in part or in whole).
// Returns a vector of all matching IDs, and another of all
// matching names.  If the device name is empty, then match all
// input or output devices.

void GR_AUDIO_API
find_audio_devices
(const std::string& device_name,
 bool is_input,
 std::vector < AudioDeviceID >* all_ad_ids,
 std::vector < std::string >* all_names);

} /* namespace osx */
} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_OSX_IMPL_H */
