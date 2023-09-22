/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_CUSTOM_BUFFER_TYPE_H
#define INCLUDED_GR_RUNTIME_CUSTOM_BUFFER_TYPE_H

#include <gnuradio/api.h>
#include <gnuradio/runtime_types.h>

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace gr {

/*!
 * \brief Base class for describing a buffer's type.
 */
class GR_RUNTIME_API buffer_type_base
{
public:
    virtual ~buffer_type_base(){};

    // Do not allow copying or assignment
    //    buffer_type_base(buffer_type_base const&) = delete;

    // Temporarily define copy constructor to work around pybind issue with
    // default non-copyable function argument
    buffer_type_base(buffer_type_base const& other) : d_name(other.d_name) {}

    void operator=(buffer_type_base const&) = delete;

    // Allow equality and inequality comparison
    bool operator==(const buffer_type_base& other) const
    {
        return d_name == other.d_name;
    }

    bool operator!=(const buffer_type_base& other) const
    {
        return d_name != other.d_name;
    }

    // Do not allow other comparison (just in case)
    bool operator<(const buffer_type_base& other) = delete;
    bool operator>(const buffer_type_base& other) = delete;
    bool operator<=(const buffer_type_base& other) = delete;
    bool operator>=(const buffer_type_base& other) = delete;
    /*!
     * \brief Get the human-readable name of the type
     */
    const std::string& name() const { return d_name; }

    /*!
     * \brief Make and return a buffer subclass of the corresponding type
     */
    virtual buffer_sptr
    make_buffer([[maybe_unused]] int nitems,
                [[maybe_unused]] size_t sizeof_item,
                [[maybe_unused]] uint64_t downstream_lcm_nitems,
                [[maybe_unused]] uint32_t downstream_max_out_mult,
                [[maybe_unused]] block_sptr link = block_sptr(),
                [[maybe_unused]] block_sptr buf_owner = block_sptr()) const
    {
        // NOTE: this is *never* intended to be called directly, instead the overridden
        // version from a derived class (created from the template below) will be
        // called.
        return nullptr;
    }

protected:
    const std::string d_name;

    buffer_type_base(const std::string name) : d_name(name) {}
};

typedef const buffer_type_base& buffer_type;
typedef std::vector<std::reference_wrapper<const buffer_type_base>> gr_vector_buffer_type;

/*!
 * \brief Template used to create buffer types. Note that the factory_class parameter
 * must contain a static function make_buffer() that matches the signature below
 * and will be used to create instances of the corresponding buffer type.
 */
template <typename classname, typename factory_class>
struct buftype : public buffer_type_base {
public:
    using factory = factory_class;
    buffer_sptr make_buffer(int nitems,
                            size_t sizeof_item,
                            uint64_t downstream_lcm_nitems,
                            uint32_t downstream_max_out_mult,
                            block_sptr link = block_sptr(),
                            block_sptr buf_owner = block_sptr()) const override
    {
        return factory::make_buffer(nitems,
                                    sizeof_item,
                                    downstream_lcm_nitems,
                                    downstream_max_out_mult,
                                    link,
                                    buf_owner);
    }

    buftype() : buffer_type_base(typeid(classname).name()) {}
};

} // namespace gr

#endif /* INCLUDED_GR_RUNTIME_CUSTOM_BUFFER_TYPE_H */
