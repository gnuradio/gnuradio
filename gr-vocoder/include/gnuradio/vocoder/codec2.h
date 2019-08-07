/* -*- c++ -*- */
/*
 * Copyright 2014,2016,2019 Free Software Foundation, Inc.
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

#ifndef _VOCODER_CODEC2_H_
#define _VOCODER_CODEC2_H_

#include <gnuradio/vocoder/api.h>

extern "C" {
#include <codec2/codec2.h>
}

namespace gr {
namespace vocoder {

class VOCODER_API codec2
{
public:
    enum bit_rate {
        MODE_3200 = CODEC2_MODE_3200,
        MODE_2400 = CODEC2_MODE_2400,
        MODE_1600 = CODEC2_MODE_1600,
        MODE_1400 = CODEC2_MODE_1400,
        MODE_1300 = CODEC2_MODE_1300,
        MODE_1200 = CODEC2_MODE_1200,
#ifdef CODEC2_MODE_700
        MODE_700 = CODEC2_MODE_700,
#endif
#ifdef CODEC2_MODE_700B
        MODE_700B = CODEC2_MODE_700B,
#endif
#ifdef CODEC2_MODE_700C
        MODE_700C = CODEC2_MODE_700C,
#endif
#ifdef CODEC2_MODE_WB
        MODE_WB = CODEC2_MODE_WB,
#endif
    };

private:
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* _VOCODER_CODEC2_H_ */
