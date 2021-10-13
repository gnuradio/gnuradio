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
#include <mutex>
#include <string>
#include <vector>

namespace gr {

// This is the function pointer declaration for the factory-like functions
// used to create buffer subclasses
typedef buffer_sptr (*factory_func_ptr)(int nitems,
                                        size_t sizeof_item,
                                        uint64_t downstrea_lcm_nitems,
                                        uint32_t downstream_max_out_mult,
                                        block_sptr link,
                                        block_sptr buf_owner);


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
    buffer_type_base(buffer_type_base const& other)
        : d_name(other.d_name), d_factory(other.d_factory)
    {
    }

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
    inline buffer_sptr make_buffer(int nitems,
                                   size_t sizeof_item,
                                   uint64_t downstream_lcm_nitems,
                                   uint32_t downstream_max_out_mult,
                                   block_sptr link,
                                   block_sptr buf_owner) const
    {
        // Delegate call to factory function
        return d_factory(nitems,
                         sizeof_item,
                         downstream_lcm_nitems,
                         downstream_max_out_mult,
                         link,
                         buf_owner);
    }

protected:
    const std::string d_name;
    factory_func_ptr d_factory;

    // Protected constructor
    buffer_type_base(const char* name, factory_func_ptr factory_func)
        : d_name(name), d_factory(factory_func)
    {
    }
};

typedef const buffer_type_base& buffer_type;
typedef std::vector<std::reference_wrapper<const buffer_type_base>> gr_vector_buffer_type;

#define DEFINE_CUSTOM_BUFFER_TYPE(CLASSNAME, FACTORY_FUNC_PTR)                    \
    class GR_RUNTIME_API buftype_##CLASSNAME : public buffer_type_base            \
    {                                                                             \
    public:                                                                       \
        buftype_##CLASSNAME() : buffer_type_base(#CLASSNAME, FACTORY_FUNC_PTR) {} \
    };

} // namespace gr

#endif /* INCLUDED_GR_RUNTIME_CUSTOM_BUFFER_TYPE_H */
