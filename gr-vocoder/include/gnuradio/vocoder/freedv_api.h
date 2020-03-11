/* -*- c++ -*- */
/*
 * Copyright 2016-2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef _VOCODER_FREEDV_H_
#define _VOCODER_FREEDV_H_

#include <gnuradio/vocoder/api.h>

// version >=0.9.1 contains fixes that doesn't require "extern C"
// between 0.8.1 and 0.9.1 the build fail
#ifdef CODEC2_LEGACY
extern "C" {
#endif
#include <codec2/codec2.h>
#include <codec2/freedv_api.h>
#include <codec2/modem_stats.h>
#ifdef CODEC2_LEGACY
}
#endif

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
