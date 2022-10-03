/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_WINDOW_H
#define INCLUDED_GR_UHD_RFNOC_WINDOW_H

#include <gnuradio/uhd/api.h>
#include <gnuradio/uhd/rfnoc_block.h>

namespace gr {
namespace uhd {

/*! RFNoC Window Block
 *
 * \ingroup uhd_blk
 */
class GR_UHD_API rfnoc_window : virtual public rfnoc_block
{
public:
    typedef std::shared_ptr<rfnoc_window> sptr;

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

    /*! Set the Window coefficients
     *
     * \param coeffs Vector of Coeffs (float)
     * \param chan   Channel Index
     */
    virtual void set_coefficients(const std::vector<float>& coeffs,
                                  const size_t chan) = 0;

    /*! Set the Window coefficients
     *
     * \param coeffs Vector of Coeffs (int16)
     * \param chan   Channel Index
     */
    virtual void set_coefficients(const std::vector<int16_t>& coeffs,
                                  const size_t chan) = 0;

    /*! Get the number of Window coefficients
     *
     * \param chan   Channel Index
     */
    virtual size_t get_max_num_coefficients(const size_t chan) = 0;

    /*! Returns a vector of Window coefficients
     *
     * \param chan   Channel Index
     */
    virtual std::vector<int16_t> get_coefficients(const size_t chan) = 0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_WINDOW_H */
