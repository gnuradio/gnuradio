/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_FFT_H
#define INCLUDED_GR_UHD_RFNOC_FFT_H

#include <gnuradio/uhd/api.h>
#include <gnuradio/uhd/rfnoc_block.h>
#include <vector>

namespace gr {
namespace uhd {

/*! RFNoC FFT Block
 *
 * \ingroup uhd_blk
 */
class GR_UHD_API rfnoc_fft : virtual public rfnoc_block
{
public:
    typedef std::shared_ptr<rfnoc_fft> sptr;

    enum class fft_shift { NORMAL, REVERSE, NATURAL, BIT_REVERSE };
    enum class fft_direction { REVERSE, FORWARD };
    enum class fft_magnitude { COMPLEX, MAGNITUDE, MAGNITUDE_SQUARED };

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

    /*! Set the FFT direction
     *
     * \param direction FFT direction
     */
    virtual void set_direction(const fft_direction direction) = 0;

    /*! Set the FFT direction using a string
     *
     * \param direction  The FFT direction to choose ["REVERSE", "FORWARD"];
     */
    virtual void set_direction(const std::string& direction) = 0;

    /*! Get the FFT direction
     *
     * \returns The current FFT direction as a string
     */
    virtual std::string get_direction_string() const = 0;

    /*! Get the FFT direction
     *
     * \returns FFT direction
     */
    virtual fft_direction get_direction() const = 0;

    /*! Set the format of the returned FFT output data
     *
     * \param magnitude Format of the returned FFT output data
     */
    virtual void set_magnitude(const fft_magnitude magnitude) = 0;

    /*! Set the magnitude using a string
     *
     * \param magnitude  The magnitude to choose ["COMPLEX", "MAGNITUDE",
     * "MAGNITUDE_SQUARED"]
     */
    virtual void set_magnitude(const std::string& magnitude) = 0;

    /*! Get the FFT magnitude
     *
     * \returns The current FFT magnitude as a string
     */
    virtual std::string get_magnitude_string() const = 0;

    /*! Get the format of the returned FFT output data
     *
     * \returns Format of the returned FFT output data
     */
    virtual fft_magnitude get_magnitude() const = 0;

    /*! Set the shift configuration of the output FFT data
     *
     * \param shift Configuration for shifting FFT output data
     */
    virtual void set_shift_config(const fft_shift shift) = 0;

    /*! Set the shift configuration of the output FFT data using a string
     *
     * \param shift  The shift configuration to choose ["NORMAL", "REVERSE", "NATURAL",
     * "BIT_REVERSE"]
     */
    virtual void set_shift_config(const std::string& shift) = 0;

    /*! Get the shift configuration of the output FFT data
     *
     * \returns The current shift configuration of the output FFT data as a string
     */
    virtual std::string get_shift_config_string() const = 0;

    /*! Get the shift configuration of the output FFT data
     *
     * \returns Shift configuration of the output FFT data
     */
    virtual fft_shift get_shift_config() const = 0;

    /*! Set the scaling factor for the FFT block
     *
     * \param factor Desired scaling factor
     */
    virtual void set_scaling_factor(const double factor) = 0;

    /*! Set the scaling schedule for the FFT block
     *
     * \param scaling Scaling schedule for the FFT block
     */
    virtual void set_scaling(const uint32_t scaling) = 0;

    /*! Get the scaling schedule for the FFT block
     *
     * \returns Scaling schedule for the FFT block
     */
    virtual uint32_t get_scaling() const = 0;

    /*! Set the length of the FFT
     *
     * \param length Desired FFT length
     */
    virtual void set_length(const uint32_t length) = 0;

    /*! Get the length of the FFT
     *
     * \returns Current FFT length
     */
    virtual uint32_t get_length() const = 0;

    /*! Set the bypass mode of the FFT
     *
     * \param bypass FFT bypass moe
     */
    virtual void set_bypass_mode(const bool bypass) = 0;

    /*! Get the bypass mode of the FFT
     *
     * \returns Current FFT bypass mode
     */
    virtual bool get_bypass_mode() const = 0;

    /*! Get the number of items per clock cycle (NIPC)
     *
     * \returns NIPC
     */
    virtual uint32_t get_nipc() const = 0;

    /*! Get the maximum supported length of the FFT
     *
     * \returns Maximum supported FFT length
     */
    virtual uint32_t get_max_length() const = 0;

    /*! Get the maximum supported cyclic prefix length
     *
     * \returns Maximum supported cyclic prefix length
     */
    virtual uint32_t get_max_cp_length() const = 0;

    /*! Get the maximum supported number of values that can be written to the
     * cyclic prefix removal list
     *
     * \returns Maximum number of values for the cyclic prefix removal list
     */
    virtual uint32_t get_max_cp_removal_list_length() const = 0;

    /*! Get the maximum supported number of values that can be written to the
     * cyclic prefix insertion list
     *
     * \returns Maximum number of values for the cyclic prefix insertion list
     */
    virtual uint32_t get_max_cp_insertion_list_length() const = 0;

    /*! Load values to the cyclic prefix insertion list.
     *
     * \param cp_lengths The cyclic prefix lengths to be written to the list
     */
    virtual void set_cp_insertion_list(const std::vector<uint32_t> cp_lengths) = 0;

    /*! Gets the values from the cyclic prefix insertion list.
     *
     * \returns The cyclic prefix insertion list
     */
    virtual std::vector<uint32_t> get_cp_insertion_list() const = 0;

    /*! Load values to the cyclic prefix removal list.
     *
     * \param cp_lengths The cyclic prefix lengths to be written to the list
     */
    virtual void set_cp_removal_list(const std::vector<uint32_t> cp_lengths) = 0;

    /*! Gets the values from the cyclic prefix removal list.
     *
     * \returns The cyclic prefix removal list
     */
    virtual std::vector<uint32_t> get_cp_removal_list() const = 0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_FFT_H */
