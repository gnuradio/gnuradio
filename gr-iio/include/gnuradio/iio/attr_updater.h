/* -*- c++ -*- */
/*
 * Copyright 2019 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
    typedef boost::shared_ptr<attr_updater> sptr;

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
