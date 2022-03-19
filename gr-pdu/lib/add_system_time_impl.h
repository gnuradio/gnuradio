/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_PDU_ADD_SYSTEM_TIME_IMPL_H
#define INCLUDED_PDU_ADD_SYSTEM_TIME_IMPL_H

#include <gnuradio/pdu/add_system_time.h>

namespace gr {
namespace pdu {

class add_system_time_impl : public add_system_time
{
private:
    std::string d_name;
    pmt::pmt_t d_key;

    void handle_pdu(const pmt::pmt_t& pdu);

public:
    add_system_time_impl(const pmt::pmt_t key);
    ~add_system_time_impl() override;
};

} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_ADD_SYSTEM_TIME_IMPL_H */
