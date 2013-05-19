/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_AUDIO_SINK_H
#define INCLUDED_GR_AUDIO_SINK_H

#include <gnuradio/audio/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace audio {

    /*!
     * \brief Creates a sink from an audio device.
     * \ingroup audio_blk
     */
    class GR_AUDIO_API sink : virtual public sync_block
    {
    public:
      typedef boost::shared_ptr<sink> sptr;

      /*!
       * Creates a sink from an audio device at a specified
       * sample_rate. The specific audio device to use can be
       * specified as the device_name parameter. Typical choices are:
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
      static sptr make(int sampling_rate,
                       const std::string device_name = "",
                       bool ok_to_block = true);
    };

  } /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_GR_AUDIO_SINK_H */
