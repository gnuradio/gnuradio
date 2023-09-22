/* -*- c++ -*- */
/*
 * Copyright 2021 NTESS LLC.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_TAKE_SKIP_TO_PDU_IMPL_H
#define INCLUDED_PDU_TAKE_SKIP_TO_PDU_IMPL_H

#include <gnuradio/pdu/take_skip_to_pdu.h>

namespace gr {
namespace pdu {

template <class T>
class PDU_API take_skip_to_pdu_impl : public take_skip_to_pdu<T>
{
private:
    // overloaded pmt uniform vector initializers
    inline pmt::pmt_t init_data(const std::vector<unsigned char> data)
    {
        return pmt::init_u8vector(data.size(), (const unsigned char*)&data[0]);
    }
    inline pmt::pmt_t init_data(const std::vector<short> data)
    {
        return pmt::init_s16vector(data.size(), (const short*)&data[0]);
    }
    inline pmt::pmt_t init_data(const std::vector<int> data)
    {
        return pmt::init_s32vector(data.size(), (const int*)&data[0]);
    }
    inline pmt::pmt_t init_data(const std::vector<float> data)
    {
        return pmt::init_f32vector(data.size(), (const float*)&data[0]);
    }
    inline pmt::pmt_t init_data(const std::vector<gr_complex> data)
    {
        return pmt::init_c32vector(data.size(), (const gr_complex*)&data[0]);
    }

private:
    uint32_t d_take;
    uint32_t d_skip;
    uint64_t d_next;
    bool d_triggered;
    uint64_t d_burst_counter;
    uint8_t d_prev_byte;

    std::vector<T> d_vector;
    pmt::pmt_t d_meta_dict;

    /*!
     * \brief Publishes accumulated data as a PDU.
     */
    void publish_message();

public:
    take_skip_to_pdu_impl(uint32_t take, uint32_t skip);

    ~take_skip_to_pdu_impl() override;
    bool stop() override;

    void set_take(const uint32_t take) override;
    void set_skip(const uint32_t skip) override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace pdu
} // namespace gr

#endif /* INCLUDED_PDU_TAKE_SKIP_TO_PDU_IMPL_H */
