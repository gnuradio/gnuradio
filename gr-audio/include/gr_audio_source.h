/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_AUDIO_SOURCE_H
#define INCLUDED_GR_AUDIO_SOURCE_H

#include <gr_audio_api.h>
#include <gr_sync_block.h>

/*!
 * \brief Creates a source from an audio device.
 * \ingroup audio_blk
 */
class GR_AUDIO_API audio_source : virtual public gr_sync_block{
public:
    typedef boost::shared_ptr<audio_source> sptr;
};

/*!
 * Creates a source from an audio device at a specified
 * sample_rate. The specific audio device to use can be specified as
 * the device_name parameter. Typical choices are:
 * \li pulse
 * \li hw:0,0
 * \li plughw:0,0
 * \li surround51
 * \li /dev/dsp
 *
 * \xmlonly
 *    - pulse, hw:0,0, plughw:0,0, surround51, /dev/dsp
 * \endxmlonly
 */
GR_AUDIO_API audio_source::sptr audio_make_source(
    int sampling_rate,
    const std::string device_name = "",
    bool ok_to_block = true
);

#endif /* INCLUDED_GR_AUDIO_SOURCE_H */
