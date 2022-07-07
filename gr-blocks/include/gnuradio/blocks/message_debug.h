/* -*- c++ -*- */
/*
 * Copyright 2005,2012-2013 Free Software Foundation, Inc.
 * Copyright 2022,2023 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MESSAGE_DEBUG_H
#define INCLUDED_GR_MESSAGE_DEBUG_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>
#include <spdlog/common.h>

namespace gr {
namespace blocks {

/*!
 * \brief Debug block for the message passing system.
 * \ingroup message_tools_blk
 * \ingroup measurement_tools_blk
 * \ingroup debug_tools_blk
 *
 * \details
 * The message debug block is used to capture and print or store
 * messages as they are received. Any block that generates a
 * message may connect that message port to one or more of the
 * two message input ports of this debug block. The message
 * ports are:
 *
 * \li print: prints the message directly to standard out.
 * \li store: stores the message in an internal vector. May be
 *            access using the get_message function.
 * \li print_pdu: DEPRECATED! use print() for all printing
 */
class BLOCKS_API message_debug : virtual public block
{
public:
    // gr::blocks::message_debug::sptr
    typedef std::shared_ptr<message_debug> sptr;

    /*!
     * \brief Build the message debug block. It takes a single parameter that can be used
     * to disable PDU vector printing and has two message ports: print and store.
     *
     * \param en_uvec Enable PDU Vector Printing.
     * \param log_level The log level of the `log` port. Defaults to "info".
     */
    static sptr make(bool en_uvec = true,
                     spdlog::level::level_enum log_level = spdlog::level::info);

    /*!
     * \brief Reports the number of messages received by this block.
     */
    virtual size_t num_messages() = 0;

    /*!
     * \brief Get a message (as a PMT) from the message vector at index \p i.
     *
     * Messages passed to the 'store' port will be stored in a
     * vector. This function retrieves those messages by index. They
     * are index in order of when they were received (all messages
     * are just pushed onto the back of a vector). This is mostly
     * useful in debugging message passing graphs and in QA code.
     *
     * \param i The index in the vector for the message to retrieve.
     *
     * \return a message at index \p i as a pmt_t.
     */
    virtual pmt::pmt_t get_message(size_t i) = 0;

    /*!
     * \brief Enables or disables printing of PDU uniform vector data.
     */
    virtual void set_vector_print(bool en) = 0;

    /*!
     * \brief get the log level
     */
    virtual spdlog::level::level_enum level() const = 0;
    /*!
     * \brief set the log level
     */
    virtual void set_level(spdlog::level::level_enum log_level) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_DEBUG_H */
