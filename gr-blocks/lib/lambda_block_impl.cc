/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, A National Instruments Brand
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "lambda_block_impl.h"
#include <gnuradio/io_signature.h>
#include <utility>

namespace gr {
namespace blocks {

lambda_block::sptr lambda_block::make(general_work_func_t&& work_func,
                                      io_signature::sptr in,
                                      io_signature::sptr out,
                                      const std::string& name)
{
    return gnuradio::make_block_sptr<lambda_block_impl>(
        std::move(work_func), in, out, name);
}

lambda_block_impl::lambda_block_impl(general_work_func_t&& work_func,
                                     io_signature::sptr in,
                                     io_signature::sptr out,
                                     const std::string& name)
    : block(name, in, out), _work_func(std::move(work_func))
{
}

lambda_block_impl::~lambda_block_impl() {}

int lambda_block_impl::general_work(int noutput_items,
                                    gr_vector_int& ninput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    return _work_func(this, noutput_items, ninput_items, input_items, output_items);
}

} /* namespace blocks */
} /* namespace gr */
