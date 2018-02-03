/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "audio_registry.h"

#include <gnuradio/audio/osx_impl.h>

#include <algorithm>
#include <iostream>
#include <locale>
#include <stdexcept>

std::ostream&
operator<<
(std::ostream& s,
 const AudioStreamBasicDescription& asbd)
{
  char format_id[sizeof(asbd.mFormatID)+1];
  memcpy(format_id, (void*)(&asbd.mFormatID), sizeof(asbd.mFormatID));
  format_id[sizeof(asbd.mFormatID)] = 0;
  s << "  Sample Rate      : " << asbd.mSampleRate << std::endl;
  s << "  Format ID        : " << format_id << std::endl;
  s << "  Format Flags     : " << asbd.mFormatFlags << std::endl;
  s << "    " << ((asbd.mFormatFlags & kAudioFormatFlagIsFloat) != 0)
    << " : Is Float" << std::endl;
  s << "    " << ((asbd.mFormatFlags & kAudioFormatFlagIsBigEndian) != 0)
    << " : Is Big Endian" << std::endl;
  s << "    " << ((asbd.mFormatFlags & kAudioFormatFlagIsSignedInteger) != 0)
    << " : Is Signed Integer" << std::endl;
  s << "    " << ((asbd.mFormatFlags & kAudioFormatFlagIsPacked) != 0)
    << " : Is Packed" << std::endl;
  s << "    " << ((asbd.mFormatFlags & kAudioFormatFlagIsAlignedHigh) != 0)
    << " : Is Aligned High" << std::endl;
  s << "    " << ((asbd.mFormatFlags & kAudioFormatFlagIsNonInterleaved) != 0)
    << " : Is Non-Interleaved" << std::endl;
  s << "    " << ((asbd.mFormatFlags & kAudioFormatFlagIsNonMixable) != 0)
    << " : Is Non-Mixable" << std::endl;
  s << "  Bytes / Packet   : " << asbd.mBytesPerPacket << std::endl;
  s << "  Frames / Packet  : " << asbd.mFramesPerPacket << std::endl;
  s << "  Bytes / Frame    : " << asbd.mBytesPerFrame << std::endl;
  s << "  Channels / Frame : " << asbd.mChannelsPerFrame << std::endl;
  s << "  Bits / Channel   : " << asbd.mBitsPerChannel;
  return(s);
};

namespace gr {
namespace audio {
namespace osx {

static UInt32
_get_num_channels
(AudioDeviceID ad_id,
 AudioObjectPropertyScope scope)
{
  // retrieve the AudioBufferList associated with this ID using
  // the provided scope

  UInt32 num_channels = 0;
  UInt32 prop_size = 0;
  AudioObjectPropertyAddress ao_address = {
    kAudioDevicePropertyStreamConfiguration, scope, 0
  };
  OSStatus err = noErr;
  if ((err = AudioObjectGetPropertyDataSize
       (ad_id, &ao_address, 0, NULL,
	&prop_size)) == noErr) {
    boost::scoped_array<AudioBufferList> buf_list
      (reinterpret_cast<AudioBufferList*>
       (new char[prop_size]));
    if ((err = AudioObjectGetPropertyData
	 (ad_id, &ao_address, 0, NULL,
	  &prop_size, buf_list.get())) == noErr) {
      for (UInt32 mm = 0; mm < buf_list.get()->mNumberBuffers; ++mm) {
	num_channels += buf_list.get()->mBuffers[mm].mNumberChannels;
      }
    }
    else {
      // assume 2 channels
      num_channels = 2;
    }
  }
  else {
    // assume 2 channels
    num_channels = 2;
  }
  return(num_channels);
}

// works with both char and wchar_t
template<typename charT>
struct ci_equal {
  ci_equal( const std::locale& loc ) : loc_(loc) {}
  bool operator()(charT ch1, charT ch2) {
    return std::tolower(ch1, loc_) == std::tolower(ch2, loc_);
  }
private:
  const std::locale& loc_;
};

// find substring (case insensitive)
static std::string::size_type ci_find_substr
(const std::string& str1, const std::string& str2,
 const std::locale& loc = std::locale())
{
  std::string::const_iterator it = std::search
    (str1.begin(), str1.end(),
     str2.begin(), str2.end(),
     ci_equal<std::string::value_type>(loc));
  if (it != str1.end()) {
    return(it - str1.begin());
  }
  // not found
  return(std::string::npos);
}

void
get_num_channels_for_audio_device_id
(AudioDeviceID ad_id,
 UInt32* n_input,
 UInt32* n_output)
{
  if (n_input) {
    *n_input = _get_num_channels
      (ad_id, kAudioDevicePropertyScopeInput);
  }
  if (n_output) {
    *n_output = _get_num_channels
      (ad_id, kAudioDevicePropertyScopeOutput);
  }
}

void
find_audio_devices
(const std::string& device_name,
 bool is_input,
 std::vector < AudioDeviceID >* all_ad_ids,
 std::vector < std::string >* all_names)
{
  if ((!all_ad_ids) && (!all_names)) {
    // if nothing is requested, no point in doing anything!
    return;
  }

  OSStatus err = noErr;

  // retrieve the size of the array of known audio device IDs

  UInt32 prop_size = 0;

  AudioObjectPropertyAddress ao_address = {
    kAudioHardwarePropertyDevices,
    kAudioObjectPropertyScopeGlobal,
    kAudioObjectPropertyElementMaster
  };

  if ((err = AudioObjectGetPropertyDataSize
       (kAudioObjectSystemObject, &ao_address,
	0, NULL, &prop_size)) != noErr) {
#if _OSX_AU_DEBUG_
    std::cerr << "audio_osx::find_audio_devices: "
	      << "Unable to retrieve number of audio objects: "
	      << err << std::endl;
#endif
    return;
  }

  // get the total number of audio devices (input and output)

  UInt32 num_devices = prop_size / sizeof(AudioDeviceID);

  // retrieve all audio device ids

  boost::scoped_array < AudioDeviceID > all_dev_ids
    (new AudioDeviceID[num_devices]);

  if ((err = AudioObjectGetPropertyData
       (kAudioObjectSystemObject, &ao_address,
	0, NULL, &prop_size, all_dev_ids.get())) != noErr) {
#if _OSX_AU_DEBUG_
    std::cerr << "audio_osx::find_audio_devices: "
	      << "Unable to retrieve audio object ids: "
	      << err << std::endl;
#endif
    return;
  }

  // success; loop over all retrieved output device ids, retrieving
  // the name for each and comparing with the desired name.

  std::vector< std::string > valid_names(num_devices);
  std::vector< UInt32 > valid_indices(num_devices);
  UInt32 num_found_devices = 0;
  AudioObjectPropertyScope scope = is_input ?
    kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;

  for (UInt32 nn = 0; nn < num_devices; ++nn) {

    // make sure this device has input / output channels (it might
    // also have output / input channels, too, but we do not care
    // about that here)

    AudioDeviceID t_id = all_dev_ids[nn];

    if (is_input) {
      UInt32 n_input_channels = 0;
      get_num_channels_for_audio_device_id
	(t_id, &n_input_channels, NULL);
      if (n_input_channels == 0) {
	// no input channels; must be output device; just continue
	// to the next audio device.
	continue;
      }
    } else {
      UInt32 n_output_channels = 0;
      get_num_channels_for_audio_device_id
	(t_id, NULL, &n_output_channels);
      if (n_output_channels == 0) {
	// no output channels; must be input device; just continue
	// to the next audio device.
	continue;
      }
    }

    // retrieve the device name; max name length is 64 characters.

    prop_size = 65;
    char c_name_buf[prop_size];
    bzero((void*)c_name_buf, prop_size);
    --prop_size;

    AudioObjectPropertyAddress ao_address = {
      kAudioDevicePropertyDeviceName, scope, 0
    };

    if ((err = AudioObjectGetPropertyData
	 (t_id, &ao_address, 0, NULL,
	  &prop_size, (void*)c_name_buf)) != noErr) {
#if _OSX_AU_DEBUG_
      std::cerr << "audio_osx::find_audio_devices: "
		<< "Unable to retrieve audio device name #"
		<< (nn+1) << ": " << err << std::endl;
#endif
      continue;
    }
    std::string name_buf(c_name_buf);

    // compare the retrieved name with the desired one, if
    // provided; case insensitive.

    if (device_name.length() > 0) {

      std::string::size_type found =
	ci_find_substr(name_buf, device_name);
      if (found == std::string::npos) {
	// not found; continue to the next ID
	continue;
      }
    }

    // store this info

    valid_names[nn] = name_buf;
    valid_indices[num_found_devices++] = nn;

  }

  // resize valid function arguments, then copy found values

  if (all_ad_ids) {
    all_ad_ids->resize(num_found_devices);
    for (UInt32 nn = 0; nn < num_found_devices; ++nn) {
      (*all_ad_ids)[nn] = all_dev_ids[valid_indices[nn]];
    }
  }

  if (all_names) {
    all_names->resize(num_found_devices);
    for (UInt32 nn = 0; nn < num_found_devices; ++nn) {
      (*all_names)[nn] = valid_names[valid_indices[nn]];
    }
  }
}

} /* namespace osx */
} /* namespace audio */
} /* namespace gr */
