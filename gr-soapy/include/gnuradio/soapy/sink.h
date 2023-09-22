/* -*- c++ -*- */
/*
 * Copyright 2021 Jeff Long
 * Copyright 2018-2021 Libre Space Foundation <http://libre.space/>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_SOAPY_SINK_H
#define INCLUDED_GR_SOAPY_SINK_H

#include <gnuradio/soapy/api.h>
#include <gnuradio/soapy/block.h>
#include <gnuradio/sync_block.h>
#include <cstdint>
#include <string>
#include <vector>

namespace gr {
namespace soapy {

/*!
 * \addtogroup block
 * \brief <b>Sink</b> block implements SoapySDR functionality for RX.
 * \ingroup soapy
 * \section sink Soapy Sink
 * The soapy sink block receives samples and writes to a stream.
 * The sink block also provides Soapy API calls for receiver settings.
 * Includes all parameters for full RX implementation.
 * Device is a string containing the driver and type name of the
 * device the user wants to use according to the Soapy* module
 * documentation.
 * Make parameters are passed through the xml block.
 * Some of the available parameters can be seen at Figure 2
 * Antenna and clock source can be left empty and default values
 * will be used.
 * This block has a message port, which consumes PMT messages.
 * For a description of the command syntax, see \ref cmd_handler_t.
 */
class SOAPY_API sink : virtual public block
{
public:
    using sptr = std::shared_ptr<sink>;

    /*!
     * \brief Return a shared_ptr to a new instance of soapy::sink.
     *
     * To avoid accidental use of raw pointers, soapy::sink's
     * constructor is in a private implementation
     * class. soapy::sink::make is the public interface for
     * creating new instances.
     * \param device the device driver and type
     * \param type output stream format
     * \param nchan number of channels
     * \param dev_args device specific arguments
     * \param stream_args stream arguments. Same for all enabled channels
     * \param tune_args list with tuning specific arguments, one entry for every
     * enabled channel, or a single entry to apply to all
     * \param other_settings list with general settings, one entry for every
     * enabled channel, or a single entry to apply to all. Supports also specific
     * gain settings.
     *
     * Driver name can be any of "uhd", "lime", "airspy",
     * "rtlsdr" or others
     */
    static sptr make(const std::string& device,
                     const std::string& type,
                     size_t nchan,
                     const std::string& dev_args = "",
                     const std::string& stream_args = "",
                     const std::vector<std::string>& tune_args = { "" },
                     const std::vector<std::string>& other_settings = { "" });

    virtual void set_length_tag_name(const std::string& length_tag_name) = 0;
};

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_GR_SOAPY_SINK_H */
