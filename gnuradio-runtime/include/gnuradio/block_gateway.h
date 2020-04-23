/* -*- c++ -*- */
/*
 * Copyright 2011-2013,2017,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_RUNTIME_BLOCK_GATEWAY_H
#define INCLUDED_RUNTIME_BLOCK_GATEWAY_H

#include <gnuradio/api.h>
#include <gnuradio/block.h>
#include <string>

#include <pybind11/pybind11.h> // must be first
#include <pybind11/stl.h>
namespace py = pybind11;

namespace gr {

/*!
 * The gateway block which performs all the magic.
 *
 * The gateway provides access to all the gr::block routines.
 */
typedef enum {
    GW_BLOCK_GENERAL = 0,
    GW_BLOCK_SYNC,
    GW_BLOCK_DECIM,
    GW_BLOCK_INTERP
} gw_block_t;

class GR_RUNTIME_API block_gateway : virtual public gr::block
{
private:
    py::handle d_py_handle;
public:
    // gr::block_gateway::sptr
    typedef std::shared_ptr<block_gateway> sptr;

    /*!
     * Make a new gateway block.
     * \param py_object the pybind11 object with callback
     * \param name the name of the block (Ex: "Shirley")
     * \param in_sig the input signature for this block
     * \param out_sig the output signature for this block
     * \return a new gateway block
     */
    static sptr make(const py::object& py_handle,
                     const std::string& name,
                     gr::io_signature::sptr in_sig,
                     gr::io_signature::sptr out_sig);


    // Protected members of gr::block trampolined here for python blocks to use
    void _add_item_tag(unsigned int which_output, const tag_t& tag)
    {
        return gr::block::add_item_tag(which_output, tag);
    }

    void _add_item_tag(unsigned int which_output,
                             uint64_t abs_offset,
                             const pmt::pmt_t& key,
                             const pmt::pmt_t& value,
                             const pmt::pmt_t& srcid = pmt::PMT_F)
    {
        return gr::block::add_item_tag(which_output, abs_offset, key, value, srcid);
    }


    std::vector<tag_t> _get_tags_in_range(unsigned int which_input,
                                                uint64_t abs_start,
                                                uint64_t abs_end)
    {
        std::vector<gr::tag_t> tags;
        gr::block::get_tags_in_range(tags, which_input, abs_start, abs_end);
        return tags;
    }

    std::vector<tag_t> _get_tags_in_range(unsigned int which_input,
                                                uint64_t abs_start,
                                                uint64_t abs_end,
                                                const pmt::pmt_t& key)
    {
        std::vector<gr::tag_t> tags;
        gr::block::get_tags_in_range(tags, which_input, abs_start, abs_end, key);
        return tags;
    }

    std::vector<tag_t> _get_tags_in_window(unsigned int which_input,
                                                 uint64_t rel_start,
                                                 uint64_t rel_end)
    {
        std::vector<gr::tag_t> tags;
        gr::block::get_tags_in_window(tags, which_input, rel_start, rel_end);
        return tags;
    }

    std::vector<tag_t> _get_tags_in_window(unsigned int which_input,
                                                 uint64_t rel_start,
                                                 uint64_t rel_end,
                                                 const pmt::pmt_t& key)
    {
        std::vector<gr::tag_t> tags;
        gr::block::get_tags_in_window(tags, which_input, rel_start, rel_end, key);
        return tags;
    }

    virtual void set_msg_handler_pybind(pmt::pmt_t which_port, std::string& handler_name) = 0;

protected:
    virtual bool has_msg_handler(pmt::pmt_t which_port) = 0;
    virtual void dispatch_msg(pmt::pmt_t which_port, pmt::pmt_t msg) = 0;

};

} /* namespace gr */

#endif /* INCLUDED_RUNTIME_BLOCK_GATEWAY_H */
