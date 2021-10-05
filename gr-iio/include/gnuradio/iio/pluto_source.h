/* -*- c++ -*- */
/*
 * Copyright 2017 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_PLUTO_SOURCE_H
#define INCLUDED_IIO_PLUTO_SOURCE_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/iio/fmcomms2_source.h>

namespace gr {
namespace iio {
/*!
 * \brief Source block for the PlutoSDR
 * \ingroup iio
 *
 */
class IIO_API pluto_source : virtual public fmcomms2_source<gr_complex>
{
public:
    typedef std::shared_ptr<pluto_source> sptr;

    static sptr make(const std::string& uri, unsigned long buffer_size);

};
} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_PLUTO_SOURCE_H */
