/* -*- c++ -*- */
/*
 * Copyright 2006,2008,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "test.h"
#include <gnuradio/io_signature.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace gr {

test_sptr make_test(const std::string& name,
                    int min_inputs,
                    int max_inputs,
                    unsigned int sizeof_input_item,
                    int min_outputs,
                    int max_outputs,
                    unsigned int sizeof_output_item,
                    unsigned int history,
                    unsigned int output_multiple,
                    double relative_rate,
                    bool fixed_rate,
                    consume_type_t cons_type,
                    produce_type_t prod_type)
{
    return gnuradio::make_block_sptr<test>(name,
                                           min_inputs,
                                           max_inputs,
                                           sizeof_input_item,
                                           min_outputs,
                                           max_outputs,
                                           sizeof_output_item,
                                           history,
                                           output_multiple,
                                           relative_rate,
                                           fixed_rate,
                                           cons_type,
                                           prod_type);
}

test::test(const std::string& name,
           int min_inputs,
           int max_inputs,
           unsigned int sizeof_input_item,
           int min_outputs,
           int max_outputs,
           unsigned int sizeof_output_item,
           unsigned int history,
           unsigned int output_multiple,
           double relative_rate,
           bool fixed_rate,
           consume_type_t cons_type,
           produce_type_t prod_type)
    : block(name,
            io_signature::make(min_inputs, max_inputs, sizeof_input_item),
            io_signature::make(min_outputs, max_outputs, sizeof_output_item)),
      d_sizeof_input_item(sizeof_input_item),
      d_sizeof_output_item(sizeof_output_item),
      d_check_topology(true),
      d_consume_type(cons_type),
      d_min_consume(0),
      d_max_consume(0),
      d_produce_type(prod_type),
      d_min_produce(0),
      d_max_produce(0)
{
    set_history(history);
    set_output_multiple(output_multiple);
    set_relative_rate(relative_rate);
    set_fixed_rate(fixed_rate);
}

int test::general_work(int noutput_items,
                       gr_vector_int& ninput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items)
{
    // touch all inputs and outputs to detect segfaults
    unsigned ninputs = input_items.size();
    unsigned noutputs = output_items.size();
    for (unsigned i = 0; i < ninputs; i++) {
        char* in = (char*)input_items[i];
        if (ninput_items[i] < (int)(noutput_items + history())) {
            std::ostringstream msg;
            msg << "ninput_items[" << i << "] < noutput_items+history()"
                << "ninput_items[" << i << "] = " << ninput_items[i]
                << "noutput_items+history() = " << (noutput_items + history())
                << "noutput_items = " << noutput_items << "history() = " << history();
            GR_LOG_ERROR(d_logger, msg.str());
            throw std::runtime_error("test");
        } else {
            for (int j = 0; j < ninput_items[i]; j++) {
                // Touch every available input_item
                // We use a class variable to avoid the compiler to optimize this away
                for (unsigned int k = 0; k < d_sizeof_input_item; k++)
                    d_temp = in[j * d_sizeof_input_item + k];
            }
            switch (d_consume_type) {
            case CONSUME_NOUTPUT_ITEMS:
                consume(i, noutput_items);
                break;
            case CONSUME_NOUTPUT_ITEMS_LIMIT_MAX:
                consume(i, std::min(noutput_items, d_max_consume));
                break;
            case CONSUME_NOUTPUT_ITEMS_LIMIT_MIN:
                consume(
                    i, std::min(std::max(noutput_items, d_min_consume), ninput_items[i]));
                break;
            case CONSUME_ALL_AVAILABLE:
                consume(i, ninput_items[i]);
                break;
            case CONSUME_ALL_AVAILABLE_LIMIT_MAX:
                consume(i, std::min(ninput_items[i], d_max_consume));
                break;
                /*         //This could result in segfault, uncomment if you want to test
                   this case CONSUME_ALL_AVAILABLE_LIMIT_MIN:
                             consume(i,std::max(ninput_items[i],d_max_consume));
                             break;*/
            case CONSUME_ZERO:
                consume(i, 0);
                break;
            case CONSUME_ONE:
                consume(i, 1);
                break;
            case CONSUME_MINUS_ONE:
                consume(i, -1);
                break;
            default:
                consume(i, noutput_items);
            }
        }
    }
    for (unsigned i = 0; i < noutputs; i++) {
        char* out = (char*)output_items[i];
        {
            for (int j = 0; j < noutput_items; j++) {
                // Touch every available output_item
                for (unsigned int k = 0; k < d_sizeof_output_item; k++)
                    out[j * d_sizeof_input_item + k] = 0;
            }
        }
    }
    // Now copy input to output until max ninputs or max noutputs is reached
    int common_nports = std::min(ninputs, noutputs);
    if (d_sizeof_output_item == d_sizeof_input_item)
        for (int i = 0; i < common_nports; i++) {
            memcpy(output_items[i], input_items[i], noutput_items * d_sizeof_input_item);
        }
    int noutput_items_produced = 0;
    switch (d_produce_type) {
    case PRODUCE_NOUTPUT_ITEMS:
        noutput_items_produced = noutput_items;
        break;
    case PRODUCE_NOUTPUT_ITEMS_LIMIT_MAX:
        noutput_items_produced = std::min(noutput_items, d_max_produce);
        break;
        /*         //This could result in segfault, uncomment if you want to test this
                     case PRODUCE_NOUTPUT_ITEMS_LIMIT_MIN:
                     noutput_items_produced=std::max(noutput_items,d_min_produce);
                     break;*/
    case PRODUCE_ZERO:
        noutput_items_produced = 0;
        break;
    case PRODUCE_ONE:
        noutput_items_produced = 1;
        break;
    case PRODUCE_MINUS_ONE:
        noutput_items_produced = -1;
        break;
    default:
        noutput_items_produced = noutput_items;
    }
    return noutput_items_produced;
}

} /* namespace gr */
