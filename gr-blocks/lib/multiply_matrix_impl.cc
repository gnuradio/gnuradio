/* -*- c++ -*- */
/*
 * Copyright 2014,2017,2018 Free Software Foundation, Inc.
 * Copyright 2021 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "multiply_matrix_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

// Copy tags from input k to output l if A[l][k] is not zero
template <>
void multiply_matrix_impl<gr_complex>::propagate_tags_by_A(int noutput_items,
                                                           size_t ninput_ports,
                                                           size_t noutput_ports)
{
    std::vector<gr::tag_t> tags;
    for (size_t in_idx = 0; in_idx < ninput_ports; in_idx++) {
        this->get_tags_in_window(tags, in_idx, 0, noutput_items);

        for (size_t out_idx = 0; out_idx < noutput_ports; out_idx++) {
            if (d_A[out_idx][in_idx] == std::complex<float>(0, 0)) {
                continue;
            }
            for (size_t i = 0; i < tags.size(); i++) {
                this->add_item_tag(out_idx, tags[i]);
            }
        }
    }
}

// Check dimensions before copying
template <>
bool multiply_matrix_impl<gr_complex>::set_A(
    const std::vector<std::vector<gr_complex>>& new_A)
{
    if (d_A.size() != new_A.size()) {
        d_logger->error("Attempted to set matrix with invalid dimensions. (Old and new "
                        "number of rows different)");
        return false;
    }
    for (size_t i = 0; i < d_A.size(); i++) {
        if (d_A[i].size() != new_A[i].size()) {
            d_logger->error("Attempted to set matrix with invalid dimensions. (Old and "
                            "new number of columns different in row {:d})",
                            i);
            return false;
        }
    }
    d_A = new_A;
    return true;
}

template <>
void multiply_matrix_impl<gr_complex>::msg_handler_A(pmt::pmt_t A)
{
    if (!pmt::is_vector(A) && !pmt::is_tuple(A)) {
        d_logger->error("Invalid message to set A (wrong type).");
        return;
    }
    if (pmt::length(A) != d_A.size()) {
        d_logger->error("Invalid message to set A (wrong size).");
        return;
    }

    std::vector<std::vector<gr_complex>> new_A(d_A);
    for (size_t i = 0; i < pmt::length(A); i++) {
        pmt::pmt_t row;
        if (pmt::is_vector(A)) {
            row = pmt::vector_ref(A, i);
        } else if (pmt::is_tuple(A)) {
            row = pmt::tuple_ref(A, i);
        }
        if (pmt::is_vector(row) || pmt::is_tuple(row)) {
            if (pmt::length(row) != d_A[0].size()) {
                d_logger->error("Invalid message to set A (wrong number of columns).");
                return;
            }
            for (size_t k = 0; k < pmt::length(row); k++) {
                new_A[i][k] =
                    pmt::to_complex(pmt::is_vector(row) ? pmt::vector_ref(row, k)
                                                        : pmt::tuple_ref(row, k));
            }
        } else if (pmt::is_c32vector(row)) {
            size_t row_len = 0;
            const gr_complex* elements = pmt::c32vector_elements(row, row_len);
            if (row_len != d_A[0].size()) {
                d_logger->error("Invalid message to set A (wrong number of columns).");
                return;
            }
            new_A[i].assign(elements, elements + row_len);
        }
    }

    if (!set_A(new_A)) {
        d_logger->error("Invalid message to set A.");
    }
}

// Copy tags from input k to output l if A[l][k] is not zero
template <>
void multiply_matrix_impl<float>::propagate_tags_by_A(int noutput_items,
                                                      size_t ninput_ports,
                                                      size_t noutput_ports)
{
    std::vector<gr::tag_t> tags;
    for (size_t in_idx = 0; in_idx < ninput_ports; in_idx++) {
        get_tags_in_window(tags, in_idx, 0, noutput_items);

        for (size_t out_idx = 0; out_idx < noutput_ports; out_idx++) {
            if (d_A[out_idx][in_idx] == 0) {
                continue;
            }
            for (size_t i = 0; i < tags.size(); i++) {
                add_item_tag(out_idx, tags[i]);
            }
        }
    }
}

// Check dimensions before copying
template <>
bool multiply_matrix_impl<float>::set_A(const std::vector<std::vector<float>>& new_A)
{
    if (d_A.size() != new_A.size()) {
        d_logger->error("Attempted to set matrix with invalid dimensions. (Old and new "
                        "number of rows different)");
        return false;
    }
    for (size_t i = 0; i < d_A.size(); i++) {
        if (d_A[i].size() != new_A[i].size()) {
            d_logger->error("Attempted to set matrix with invalid dimensions. (Old and "
                            "new number of columns different in row {:d})",
                            i);
            return false;
        }
    }
    d_A = new_A;
    return true;
}

template <>
void multiply_matrix_impl<float>::msg_handler_A(pmt::pmt_t A)
{
    if (!pmt::is_vector(A) && !pmt::is_tuple(A)) {
        d_logger->error("Invalid message to set A (wrong type).");
        return;
    }
    if (pmt::length(A) != d_A.size()) {
        d_logger->error("Invalid message to set A (wrong size).");
        return;
    }

    std::vector<std::vector<float>> new_A(d_A);
    for (size_t i = 0; i < pmt::length(A); i++) {
        pmt::pmt_t row;
        if (pmt::is_vector(A)) {
            row = pmt::vector_ref(A, i);
        } else if (pmt::is_tuple(A)) {
            row = pmt::tuple_ref(A, i);
        }
        if (pmt::is_vector(row) || pmt::is_tuple(row)) {
            if (pmt::length(row) != d_A[0].size()) {
                d_logger->error("Invalid message to set A (wrong number of columns).");
                return;
            }
            for (size_t k = 0; k < pmt::length(row); k++) {
                new_A[i][k] =
                    pmt::to_double(pmt::is_vector(row) ? pmt::vector_ref(row, k)
                                                       : pmt::tuple_ref(row, k));
            }
        } else if (pmt::is_f32vector(row)) {
            size_t row_len = 0;
            const float* elements = pmt::f32vector_elements(row, row_len);
            if (row_len != d_A[0].size()) {
                d_logger->error("Invalid message to set A (wrong number of columns).");
                return;
            }
            new_A[i].assign(elements, elements + row_len);
        }
    }

    if (!set_A(new_A)) {
        d_logger->error("Invalid message to set A.");
    }
}


template <class T>
typename multiply_matrix<T>::sptr
multiply_matrix<T>::make(std::vector<std::vector<T>> A,
                         gr::block::tag_propagation_policy_t tag_propagation_policy)
{
    if (A.empty() || A[0].empty()) {
        throw std::invalid_argument("matrix A has invalid dimensions.");
    }
    return gnuradio::make_block_sptr<multiply_matrix_impl<T>>(A, tag_propagation_policy);
}

template <>
multiply_matrix_impl<gr_complex>::multiply_matrix_impl(
    std::vector<std::vector<gr_complex>> A,
    gr::block::tag_propagation_policy_t tag_propagation_policy)
    : gr::sync_block("multiply_matrix_cc",
                     gr::io_signature::make(A[0].size(), A[0].size(), sizeof(gr_complex)),
                     gr::io_signature::make(A.size(), A.size(), sizeof(gr_complex))),
      d_A(A)
{
    this->MSG_PORT_NAME_SET_A = "set_A";
    this->set_tag_propagation_policy(tag_propagation_policy);
    const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
    set_alignment(std::max(1, alignment_multiple));

    pmt::pmt_t port_name = pmt::string_to_symbol("set_A");
    message_port_register_in(port_name);
    set_msg_handler(port_name, [this](pmt::pmt_t msg) { this->msg_handler_A(msg); });
}

template <>
multiply_matrix_impl<float>::multiply_matrix_impl(
    std::vector<std::vector<float>> A,
    gr::block::tag_propagation_policy_t tag_propagation_policy)
    : gr::sync_block("multiply_matrix_ff",
                     gr::io_signature::make(A[0].size(), A[0].size(), sizeof(float)),
                     gr::io_signature::make(A.size(), A.size(), sizeof(float))),
      d_A(A)
{
    this->MSG_PORT_NAME_SET_A = "set_A";
    this->set_tag_propagation_policy(tag_propagation_policy);
    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));

    pmt::pmt_t port_name = pmt::string_to_symbol("set_A");
    message_port_register_in(port_name);
    set_msg_handler(port_name, [this](pmt::pmt_t msg) { this->msg_handler_A(msg); });
}


template <class T>
multiply_matrix_impl<T>::~multiply_matrix_impl()
{
}

template <>
int multiply_matrix_impl<gr_complex>::work(int noutput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
{
    for (size_t out_idx = 0; out_idx < output_items.size(); out_idx++) {
        gr_complex* out = reinterpret_cast<gr_complex*>(output_items[out_idx]);
        // Do input 0 first, this saves a memset
        const gr_complex* in = reinterpret_cast<const gr_complex*>(input_items[0]);
#if VOLK_VERSION >= 030100
        volk_32fc_s32fc_multiply2_32fc(out, in, &d_A[out_idx][0], noutput_items);
#else
        volk_32fc_s32fc_multiply_32fc(out, in, d_A[out_idx][0], noutput_items);
#endif
        // Then do inputs 1 through N
        for (size_t in_idx = 1; in_idx < input_items.size(); in_idx++) {
            in = reinterpret_cast<const gr_complex*>(input_items[in_idx]);
            // Yeah, this needs VOLK-ifying (TODO)
            for (int i = 0; i < noutput_items; i++) {
                out[i] += in[i] * d_A[out_idx][in_idx];
            }
        }
    }
    if (tag_propagation_policy() == TPP_CUSTOM) {
        propagate_tags_by_A(noutput_items, input_items.size(), output_items.size());
    }
    return noutput_items;
}

template <>
int multiply_matrix_impl<float>::work(int noutput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    for (size_t out_idx = 0; out_idx < output_items.size(); out_idx++) {
        float* out = reinterpret_cast<float*>(output_items[out_idx]);
        // Do input 0 first, this saves a memset
        const float* in = reinterpret_cast<const float*>(input_items[0]);
        volk_32f_s32f_multiply_32f(out, in, d_A[out_idx][0], noutput_items);
        // Then do inputs 1 through N
        for (size_t in_idx = 1; in_idx < input_items.size(); in_idx++) {
            in = reinterpret_cast<const float*>(input_items[in_idx]);
            // Yeah, this needs VOLK-ifying (TODO)
            for (int i = 0; i < noutput_items; i++) {
                out[i] += in[i] * d_A[out_idx][in_idx];
            }
        }
    }
    if (tag_propagation_policy() == TPP_CUSTOM) {
        propagate_tags_by_A(noutput_items, input_items.size(), output_items.size());
    }
    return noutput_items;
}


template class multiply_matrix<float>;
template class multiply_matrix<gr_complex>;

} /* namespace blocks */
} /* namespace gr */
