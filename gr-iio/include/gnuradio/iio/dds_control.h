/* -*- c++ -*- */
/*
 * Copyright 2019 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_DDS_CONTROL_H
#define INCLUDED_IIO_DDS_CONTROL_H

#include <gnuradio/block.h>
#include <gnuradio/iio/api.h>

namespace gr {
namespace iio {

/*!
 * \brief Control block for DDSs which are available in controlling FPGAs of
 *        certain IIO drivers
 * \ingroup iio
 *
 * \details
 * This block allow for control of multiple direct digital synthesizers (DDS)
 * to create transmit tones. The are available in the generic AXI DAC driver
 * from Analog Devices.
 */
class IIO_API dds_control : virtual public gr::block
{
public:
    typedef std::shared_ptr<dds_control> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::dds_control.
     *
     * \param uri  String of the context uri
     * \param enabled  Vector of integers where individual indexes represent
     *                 specific DDSs and when not zero the are enabled
     * \param frequencies  Vector of long integers where individual indexes
     *                 represent specific DDS frequencies in Hz
     * \param phases  Vector of floats where individual indexes represent
     *                specific DDS phase in degrees
     * \param scales  Vector of floats where individual indexes represent
     *                specific DDS scale from 0 to 1
     */
    static sptr make(const std::string& uri,
                     std::vector<int> enabled,
                     std::vector<long> frequencies,
                     std::vector<float> phases,
                     std::vector<float> scales);

    virtual void set_dds_confg(std::vector<long> frequencies,
                               std::vector<float> phases,
                               std::vector<float> scales) = 0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_DDS_CONTROL_H */
