/* -*- c++ -*- */
/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_AD9081_COMMON_H
#define INCLUDED_IIO_AD9081_COMMON_H

#include <gnuradio/iio/api.h>

#include <cstdint>

namespace gr {
namespace iio {

/*!
 * \brief Perform nyquist folding calculation
 *
 * The use of this function is best illustated with a small example:
 * Imagine you want to receive a signal at f_c = 2.4 GHz, and your converter
 * runs at f_s = 4 GHz. The frequency that our NCO should run at is therefore
 * f_nco = f_s / 2 + (f_s/2 - f_c) = 1.6 GHz. Additionally, this function will
 * compensate the spectral inversion by returning an f_nco = -1.6 GHz.
 */
IIO_API int64_t nyquist_fold(int64_t f_s, int64_t f_c);

class IIO_API ad9081_common
{
public:
    static constexpr int MAX_CHANNEL_COUNT = 8;
    static constexpr int N_FNCO = 8;
    static constexpr int N_CNCO = 4;

    /*!
     * \brief Set main nco target frequency in Hz
     */
    virtual void set_main_nco_freq(int nco, int64_t freq) = 0;

    /*!
     * \brief Set main nco phase in radians
     */
    virtual void set_main_nco_phase(int nco, float phase) = 0;

    /*!
     * \brief Set channel nco frequency in Hz
     */
    virtual void set_channel_nco_freq(int nco, int64_t freq) = 0;

    /*!
     * \brief Set channel nco phase in radians
     */
    virtual void set_channel_nco_phase(int nco, float phase) = 0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_AD9081_COMMON_H */
