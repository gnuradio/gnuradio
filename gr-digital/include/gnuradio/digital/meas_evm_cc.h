/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_MEAS_EVM_CC_H
#define INCLUDED_DIGITAL_MEAS_EVM_CC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

/*!
 * \brief Measures EVM of incoming symbols relative to a constellation object
 * \ingroup equalizers
 *
 */
enum class evm_measurement_t { EVM_PERCENT = 0, EVM_DB = 1 };

class DIGITAL_API meas_evm_cc : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<meas_evm_cc> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of gr::digital::meas_evm_cc.
     *
     * To avoid accidental use of raw pointers, equalizers::meas_evm_cc's
     * constructor is in a private implementation
     * class. equalizers::meas_evm_cc::make is the public interface for
     * creating new instances.
     */
    static sptr make(constellation_sptr cons,
                     evm_measurement_t meas_type = evm_measurement_t::EVM_PERCENT);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_MEAS_EVM_CC_H */
