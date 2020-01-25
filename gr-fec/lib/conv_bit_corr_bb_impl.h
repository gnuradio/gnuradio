/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_CONV_BIT_CORR_BB_IMPL_H
#define INCLUDED_FEC_CONV_BIT_CORR_BB_IMPL_H

#include <gnuradio/fec/conv_bit_corr_bb.h>

namespace gr {
namespace fec {

class FEC_API conv_bit_corr_bb_impl : public conv_bit_corr_bb
{
private:
    std::vector<std::vector<int>> d_score_keeper;

    int d_angry_fop;
    int d_acquire;
    // int d_acquire_track;
    unsigned int d_produce;
    unsigned int d_message;
    unsigned int d_thresh;
    unsigned int d_corr_len;
    unsigned int d_corr_sym;
    unsigned int d_lane;
    unsigned int d_op;
    unsigned int d_flush;
    unsigned int d_flush_count;
    std::vector<std::vector<unsigned char>> d_correlator;
    std::vector<unsigned int> d_acc;
    uint64_t d_cut;
    uint64_t d_counter;
    float d_data_garble_rate;

    void alert_fops();

    //      //rpcbasic_register_get<conv_bit_corr_bb, std::vector< int> >
    //      d_correlator_rpc; rpcbasic_register_variable_rw<uint64_t> d_cut_rpc; //
    //      integration period rpcbasic_register_variable_rw<int> d_flush_rpc; // time to
    //      flush rpcbasic_register_variable<uint64_t> d_msgsent_rpc;
    //      rpcbasic_register_variable<uint64_t> d_msgrecv_rpc;
    //      rpcbasic_register_variable<float> d_data_garble_rate_rpc;

    uint64_t d_msgsent, d_msgrecv;
    std::vector<int> get_corr()
    {
        std::vector<int> bits;
        if (d_correlator.empty()) {
            return bits;
        }
        for (size_t i = 0; i < d_correlator[0].size(); i++) {
            bits.push_back(d_correlator[0][i]);
        }
        return bits;
    }

    bool d_havelock;
    // rpcbasic_register_variable<bool> d_havelock_rpc;

public:
    conv_bit_corr_bb_impl(std::vector<unsigned long long> correlator,
                          int corr_sym,
                          int corr_len,
                          int cut,
                          int flush,
                          float thresh);
    ~conv_bit_corr_bb_impl();

    void catch_msg(pmt::pmt_t msg);
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    float data_garble_rate(int taps, float target);
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CONV_BIT_CORR_BB_IMPL_H */
