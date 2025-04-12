/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCK_GATEWAY_H
#define INCLUDED_BLOCK_GATEWAY_H

#include <gnuradio/block.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <gnuradio/types.h>
#include <pmt/pmt.h>

#include <pybind11/pybind11.h>

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace py = pybind11;

/*!
 * The gateway block which performs all the magic.
 *
 * The gateway provides access to all the gr::block routines.
 */
using gw_block_t = enum {
    GW_BLOCK_GENERAL = 0,
    GW_BLOCK_SYNC,
    GW_BLOCK_DECIM,
    GW_BLOCK_INTERP
};

class block_gateway : public gr::block
{
public:
    block_gateway(const py::handle& py_handle,
                  const std::string& name,
                  gr::io_signature::sptr in_sig,
                  gr::io_signature::sptr out_sig);

    // gr::block_gateway::sptr
    using sptr = std::shared_ptr<block_gateway>;

    /*!
     * Make a new gateway block.
     * \param py_handle the pybind11 object with callback
     * \param name the name of the block (Ex: "Shirley")
     * \param in_sig the input signature for this block
     * \param out_sig the output signature for this block
     * \return a new gateway block
     */
    static sptr make(const py::object& py_handle,
                     const std::string& name,
                     gr::io_signature::sptr in_sig,
                     gr::io_signature::sptr out_sig);

    /*******************************************************************
     * Public versions of gr::block protected methods, necessary because
     * we need public methods for the python bindings
     ******************************************************************/
    void _add_item_tag(unsigned int which_output, const gr::tag_t& tag);

    void _add_item_tag(unsigned int which_output,
                       uint64_t abs_offset,
                       const pmt::pmt_t& key,
                       const pmt::pmt_t& value,
                       const pmt::pmt_t& srcid);

    std::vector<gr::tag_t>
    _get_tags_in_range(unsigned int which_input, uint64_t abs_start, uint64_t abs_end);

    std::vector<gr::tag_t> _get_tags_in_range(unsigned int which_input,
                                              uint64_t abs_start,
                                              uint64_t abs_end,
                                              const pmt::pmt_t& key);

    std::vector<gr::tag_t>
    _get_tags_in_window(unsigned int which_input, uint64_t rel_start, uint64_t rel_end);

    std::vector<gr::tag_t> _get_tags_in_window(unsigned int which_input,
                                               uint64_t rel_start,
                                               uint64_t rel_end,
                                               const pmt::pmt_t& key);

    [[nodiscard]] std::optional<gr::tag_t> _get_first_tag_in_range(
        unsigned int which_input, uint64_t start, uint64_t end, const pmt::pmt_t& key);
    [[nodiscard]] std::optional<gr::tag_t>
    _get_first_tag_in_range(unsigned int which_input,
                            uint64_t start,
                            uint64_t end,
                            std::function<bool(const gr::tag_t&)>);

    /*******************************************************************
     * Overloads for various scheduler-called functions
     ******************************************************************/
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    bool start() override;

    bool stop() override;

    void set_msg_handler_pybind(const pmt::pmt_t& which_port, std::string& handler_name);

    inline gr::logger_ptr _get_logger() const { return d_logger; }

private:
    py::handle d_py_handle;

    // Message handlers back into python using pybind API
    using msg_handlers_pybind_t = std::map<pmt::pmt_t, std::string, pmt::comparator>;
    msg_handlers_pybind_t d_msg_handlers_pybind;

    bool has_msg_handler(pmt::pmt_t which_port) override;

    void dispatch_msg(pmt::pmt_t which_port, pmt::pmt_t msg) override;
};

#endif /* INCLUDED_BLOCK_GATEWAY_H */
