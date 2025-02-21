/* -*- c++ -*- */
/*
 * Copyright 2002,2007,2012 Free Software Foundation, Inc.
 * Copyright 2025 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef _GRI_MMSE_FIR_INTERPOLATOR_CC_H_
#define _GRI_MMSE_FIR_INTERPOLATOR_CC_H_

#include <gnuradio/filter/api.h>
#include <gnuradio/filter/mmse_fir_interpolator.h>
#include <gnuradio/gr_complex.h>

namespace gr {
namespace filter {
using mmse_fir_interpolator_cc = mmse_fir_interpolator<gr_complex>;
} /* namespace filter */
} /* namespace gr */

#endif /* _MMSE_FIR_INTERPOLATOR_CC_H_ */
