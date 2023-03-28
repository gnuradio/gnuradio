/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, A National Instruments Brand
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_LAMBDA_BLOCK_IMPL_H
#define INCLUDED_GR_LAMBDA_BLOCK_IMPL_H

#include <gnuradio/blocks/lambda_block.h>

namespace gr {
namespace blocks {

class lambda_block_impl : public lambda_block
{
private:
    general_work_func_t _work_func;

public:
    lambda_block_impl(general_work_func_t&& work_func,
                      io_signature::sptr in,
                      io_signature::sptr out,
                      const std::string& name);
    ~lambda_block_impl() override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_LAMBDA_BLOCK_IMPL_H */
