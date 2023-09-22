/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_FIR_FILTER_H
#define INCLUDED_GR_UHD_RFNOC_FIR_FILTER_H

#include <gnuradio/uhd/api.h>
#include <gnuradio/uhd/rfnoc_block.h>
#include <vector>

namespace gr {
namespace uhd {

/*! RFNoC FIR Filter Block
 *
 * \ingroup uhd_blk
 */
class GR_UHD_API rfnoc_fir_filter : virtual public rfnoc_block
{
public:
    typedef std::shared_ptr<rfnoc_fir_filter> sptr;

    /*!
     * \param graph Reference to the rfnoc_graph object this block is attached to
     * \param block_args Additional block arguments
     * \param device_select Device Selection
     * \param instance Instance Selection
     */
    static sptr make(rfnoc_graph::sptr graph,
                     const ::uhd::device_addr_t& block_args,
                     const int device_select,
                     const int instance);

    /*! Set the FIR Filter coefficients
     *
     * \param coeffs Vector of Coeffs (float)
     * \param chan   Channel Index
     */
    virtual void set_coefficients(const std::vector<float>& coeffs,
                                  const size_t chan = 0) = 0;

    /*! Set the FIR Filter coefficients
     *
     * \param coeffs Vector of Coeffs (int16)
     * \param chan   Channel Index
     */
    virtual void set_coefficients(const std::vector<int16_t>& coeffs,
                                  const size_t chan = 0) = 0;

    /*! Get the number of FIR Filter coefficients
     *
     * \param chan   Channel Index
     */
    virtual size_t get_max_num_coefficients(const size_t chan = 0) = 0;

    /*! Returns a vector of FIR Filter coefficients
     *
     * \param chan   Channel Index
     */
    virtual std::vector<int16_t> get_coefficients(const size_t chan = 0) = 0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_FIR_FILTER_H */
