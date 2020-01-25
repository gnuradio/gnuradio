/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_AUDIO_SOURCE_H
#define INCLUDED_GR_AUDIO_SOURCE_H

#include <gnuradio/audio/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace audio {

/*!
 * \brief Creates a source from an audio device.
 * \ingroup audio_blk
 */
class GR_AUDIO_API source : virtual public sync_block
{
public:
    typedef boost::shared_ptr<source> sptr;

    /*!
     * Creates a source from an audio device at a specified
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
    static sptr
    make(int sampling_rate, const std::string device_name = "", bool ok_to_block = true);
};

} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_GR_AUDIO_SOURCE_H */
