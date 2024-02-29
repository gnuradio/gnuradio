/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, a National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_REPLAY_H
#define INCLUDED_GR_UHD_RFNOC_REPLAY_H

#include <gnuradio/uhd/api.h>
#include <gnuradio/uhd/rfnoc_block.h>

namespace gr {
namespace uhd {

GR_UHD_API const pmt::pmt_t replay_cmd_key();
GR_UHD_API const pmt::pmt_t replay_cmd_port_key();
GR_UHD_API const pmt::pmt_t replay_cmd_offset_key();
GR_UHD_API const pmt::pmt_t replay_cmd_size_key();
GR_UHD_API const pmt::pmt_t replay_cmd_time_key();
GR_UHD_API const pmt::pmt_t replay_cmd_repeat_key();
GR_UHD_API const pmt::pmt_t replay_debug_port_key();
GR_UHD_API const pmt::pmt_t replay_mem_fullness_key();
GR_UHD_API const pmt::pmt_t replay_mem_size_key();
GR_UHD_API const pmt::pmt_t replay_word_size_key();

/*! RFNoC Replay Block
 *
 * This wraps a replay block into GNU Radio.
 *
 * \ingroup uhd_blk
 */
class GR_UHD_API rfnoc_replay : virtual public rfnoc_block
{
public:
    typedef std::shared_ptr<rfnoc_replay> sptr;

    /*!
     * \param graph Reference to the underlying rfnoc_graph object
     * \param block_args Additional block arguments
     * \param device_select Device Selection
     * \param instance Instance Selection
     */
    static sptr make(rfnoc_graph::sptr graph,
                     const ::uhd::device_addr_t& block_args,
                     const int device_select,
                     const int instance);

    /*! Record
     *
     * Begin recording. The offset sets the starting location in memory and the size
     * limits the length of the recording. The flow of data is controlled by upstream
     * RFNoC blocks.
     *
     * \param offset Memory offset where to start recording the data. This value
     * must be aligned to the memory word size. Use get_word_size() to get the
     * size of the memory word.
     * \param size Size limit, in bytes. This value must be aligned to the memory
     * word size and the item size. Use get_word_size() to get the size of the
     * memory word and get_item_size() to get the item size.  A value of 0 means
     * to use all available space.
     * \param port Which input port of the replay block to use
     * \throws uhd::value_error if offset+size exceeds the available memory.
     */
    virtual void
    record(const uint64_t offset, const uint64_t size, const size_t port = 0) = 0;

    /*! Restarts recording from the record offset
     *
     * This is a shortcut for calling record() again with the same arguments.
     *
     * \param port Which input port of the replay block to use
     */
    virtual void record_restart(const size_t port = 0) = 0;

    /*! Play back data.
     *
     * The offset and size define what data is played back on an output port.
     * It will stream out \p size bytes of data, starting at memory offset
     * \p offset.
     *
     * The data can be played once or repeated continuously until a stop
     * command is issued. If a time_spec is supplied, it will be placed in the
     * header of the first packet. Typically, this is used to tell a downstream
     * Radio block when to start transmitting the data.
     *
     * If the data type on the output port is not defined, this function will
     * throw an error.
     *
     * \param offset Memory offset of the data to be played. This value must be
     *               aligned to the size of the word in memory. Use get_word_size()
     *               to get the memory word size.
     * \param size Size of data to play back. This value must be aligned to the
     *             size of the memory word and item size. Use get_word_size() to
     *             get the memory word size and get_output_item_size() to get
     *             the item size. This value will be used for the num_samps
     *             component of the underlying stream command.
     * \param port Which output port of the replay block to use
     * \param time_spec Set the time for the first item. Any non-zero value is
     *                  used to set the time in the header of the first packet.
     *                  Most commonly, this is used to set the start time of a
     *                  transmission. Note that this block will not wait for a
     *                  time to occur, rather, it will tag the first outgoing
     *                  packet with this time stamp.
     * \param repeat Determines whether the data should be played repeatedly or
     *               just once. If set to true, stop_playback() must be called to stop
     *               the play back.
     * \throws uhd::value_error if offset+size exceeds the available memory.
     * \throws uhd::op_failed Too many play commands are queued.
     */
    virtual void play(const uint64_t offset,
                      const uint64_t size,
                      const size_t port = 0,
                      const ::uhd::time_spec_t time_spec = ::uhd::time_spec_t(0.0),
                      const bool repeat = false) = 0;

    /*! Stops playback
     *
     * Halts any currently executing play commands and cancels any other play commands
     * that are waiting to be executed for that output port.
     *
     * \param port Which output port of the replay block to use
     */
    virtual void stop_playback(const size_t port = 0) = 0;

    /*! Sets the data type for items in the current record buffer for the given input
     * port.
     *
     * \param type The data type
     * \param port Which input port of the replay block to use
     */
    virtual void set_record_type(const std::string type, const size_t port = 0) = 0;

    /*! Sets the data type for items in the current play buffer for the given output port.
     *
     * \param type The data type
     * \param port Which output port of the replay block to use
     */
    virtual void set_play_type(const std::string type, const size_t port = 0) = 0;

    /*! Issue a stream command to the replay block
     *
     * Issue stream commands to start or stop playback from the configured playback
     * buffer. Supports
     * STREAM_MODE_START_CONTINUOUS to start continuous repeating playback,
     * STREAM_MODE_NUM_SAMPS_AND_DONE to play the given number of samples once, and
     * STREAM_MODE_STOP_CONTINUOUS to stop all playback immediately.
     * If a time_spec is supplied, it is placed in the header of the first packet produced
     * for that command. Commands are queued and executed in order. A
     * STREAM_MODE_STOP_CONTINUOUS command will halt all playback and purge all commands
     * in the queue for a given output port.
     *
     * \param cmd The command to execute
     * \param port Which output port of the replay block to use
     * \throws uhd::op_failed Too many commands are queued.
     */
    virtual void issue_stream_cmd(const ::uhd::stream_cmd_t& cmd,
                                  const size_t port = 0) = 0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_REPLAY_H */
