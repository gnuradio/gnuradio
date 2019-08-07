/* -*- c++ -*- */
/*
 * Copyright 2016-2019 Free Software Foundation, Inc.
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

#ifndef _VOCODER_FREEDV_H_
#define _VOCODER_FREEDV_H_

#include <gnuradio/vocoder/api.h>

extern "C" {
#include <codec2/codec2.h>
#include <codec2/freedv_api.h>
#include <codec2/modem_stats.h>
}

namespace gr {
namespace vocoder {

class VOCODER_API freedv_api
{
public:
    enum freedv_modes {
#ifdef FREEDV_MODE_1600
        MODE_1600 = FREEDV_MODE_1600,
#endif
#ifdef FREEDV_MODE_700
        MODE_700 = FREEDV_MODE_700,
#endif
#ifdef FREEDV_MODE_700B
        MODE_700B = FREEDV_MODE_700B,
#endif
#ifdef FREEDV_MODE_2400A
        MODE_2400A = FREEDV_MODE_2400A,
#endif
#ifdef FREEDV_MODE_2400B
        MODE_2400B = FREEDV_MODE_2400B,
#endif
#ifdef FREEDV_MODE_800XA
        MODE_800XA = FREEDV_MODE_800XA,
#endif
#ifdef FREEDV_MODE_700C
        MODE_700C = FREEDV_MODE_700C,
#endif
#ifdef FREEDV_MODE_700D
        MODE_700D = FREEDV_MODE_700D,
        SYNC_UNSYNC = FREEDV_SYNC_UNSYNC,
        SYNC_AUTO = FREEDV_SYNC_AUTO,
        SYNC_MANUAL = FREEDV_SYNC_MANUAL,
#endif
    };

private:
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* _VOCODER_FREEDV_H_ */
