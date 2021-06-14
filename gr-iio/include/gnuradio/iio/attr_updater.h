/* -*- c++ -*- */
/*
 * Copyright 2019 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_ATTR_UPDATER_H
#define INCLUDED_IIO_ATTR_UPDATER_H

#include <gnuradio/block.h>
#include <gnuradio/iio/api.h>

namespace gr {
namespace iio {

/*!
 * \brief Generic helper block to create message in a format that will be
 *        accepted by the IIO Attribute Sink Block
 * \ingroup iio
 *
 * \details
 * This block is a simple but flexible message source which can be connected to
 * the IIO Attribute Sink block for easy interaction and real-time control of
 * IIO driver attributes.
 */
class IIO_API attr_updater : virtual public gr::block
{
public:
    typedef std::shared_ptr<attr_updater> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::attr_updater.
     *
     * \param attribute  String of name of attribute to be updated
     * \param value  String of value to update the attribute too
     * \param interval_ms Integer of interval in milliseconds to produce
     *        messages. If zero outputs will be made only on callback changes
     *        and when the flowgraph starts
     */
    static sptr
    make(const std::string attribute, const std::string value, unsigned int interval_ms);

    virtual void set_value(std::string value) = 0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_ATTR_UPDATER_H */
