/* -*- c++ -*- */
/*
 * Copyright 2020 NTESS LLC
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/message/port_names.h>
#include <pmt/pmt.h>

namespace gr {
namespace message {


const pmt::pmt_t PMTCONSTSTR__emit()
{
    static const pmt::pmt_t val = pmt::mp("emit");
    return val;
}
const pmt::pmt_t PMTCONSTSTR__msg()
{
    static const pmt::pmt_t val = pmt::mp("msg");
    return val;
}


} /* namespace message */
} /* namespace gr */
