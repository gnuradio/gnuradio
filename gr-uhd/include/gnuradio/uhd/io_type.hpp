//
// Copyright 2010-2011 Ettus Research LLC
// Copyright 2018 Ettus Research, a National Instruments Company
// Copyright 2019 Free Software Foundation, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifndef INCLUDED_UHD_TYPES_IO_TYPE_HPP
#define INCLUDED_UHD_TYPES_IO_TYPE_HPP

#include <uhd/config.hpp>

namespace uhd {

/* The DEPRECATED Input/Output configuration struct
 *
 * This is a copy from the UHD, for backward compatibility. UHD has deprecated
 * this structure in UHD 4.0. Older versions of UHD still have it, but it won't
 * be modified any more.
 */
class io_type_t
{
public:
    /*!
     * Built in IO types known to the system.
     */
    enum tid_t {
        //! Custom type (technically unsupported by implementation)
        CUSTOM_TYPE = int('?'),
        //! Complex floating point (64-bit floats) range [-1.0, +1.0]
        COMPLEX_FLOAT64 = int('d'),
        //! Complex floating point (32-bit floats) range [-1.0, +1.0]
        COMPLEX_FLOAT32 = int('f'),
        //! Complex signed integer (16-bit integers) range [-32768, +32767]
        COMPLEX_INT16 = int('s'),
        //! Complex signed integer (8-bit integers) range [-128, 127]
        COMPLEX_INT8 = int('b')
    };

    /*!
     * The size of this io type in bytes.
     */
    const size_t size;

    /*!
     * The type id of this io type.
     * Good for using with switch statements.
     */
    const tid_t tid;

    /*!
     * Create an io type from a built-in type id.
     * \param tid a type id known to the system
     */
    io_type_t(tid_t tid)
        : size(tid == COMPLEX_FLOAT64
                   ? sizeof(std::complex<double>)
                   : tid == COMPLEX_FLOAT32
                         ? sizeof(std::complex<float>)
                         : tid == COMPLEX_INT16 ? sizeof(std::complex<int16_t>)
                                                : sizeof(std::complex<int8_t>)),
          tid(tid)
    {
        // nop
    }

    /*!
     * Create an io type from attributes.
     * The tid will be set to custom.
     * \param size the size in bytes
     */
    io_type_t(size_t size) : size(size), tid(CUSTOM_TYPE) {}
};

} // namespace uhd

#endif /* INCLUDED_UHD_TYPES_IO_TYPE_HPP */
