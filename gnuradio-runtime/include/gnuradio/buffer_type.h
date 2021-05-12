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

#include <cstdint>
#include <mutex>
#include <string>

namespace gr {


class GR_RUNTIME_API buffer_type_base
{
public:
    virtual ~buffer_type_base(){};

    // Do not allow copying or assignment
    buffer_type_base(buffer_type_base const&) = delete;
    void operator=(buffer_type_base const&) = delete;

    // Allow equality and inequality comparison
    bool operator==(const buffer_type_base& other) const
    {
        return d_value == other.d_value;
    }

    bool operator!=(const buffer_type_base& other) const
    {
        return d_value != other.d_value;
    }

    // Do not allow other comparison (just in case)
    bool operator<(const buffer_type_base& other) = delete;
    bool operator>(const buffer_type_base& other) = delete;
    bool operator<=(const buffer_type_base& other) = delete;
    bool operator>=(const buffer_type_base& other) = delete;

    const std::string& name() const { return d_name; }

protected:
    static uint32_t s_nextId;
    static std::mutex s_mutex;

    uint32_t d_value;
    std::string d_name;

    // Private constructor
    buffer_type_base(const char* name) : d_name(name)
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        d_value = s_nextId++;
    }
};

typedef const buffer_type_base& buffer_type_t;


#define MAKE_CUSTOM_BUFFER_TYPE(CLASSNAME)                             \
    class GR_RUNTIME_API buftype_##CLASSNAME : public buffer_type_base \
    {                                                                  \
    public:                                                            \
        static buffer_type_t get()                                     \
        {                                                              \
            static buftype_##CLASSNAME instance;                       \
            return instance;                                           \
        }                                                              \
                                                                       \
    private:                                                           \
        buftype_##CLASSNAME() : buffer_type_base(#CLASSNAME) {}        \
    };

MAKE_CUSTOM_BUFFER_TYPE(DEFAULT_NON_CUSTOM);
MAKE_CUSTOM_BUFFER_TYPE(CUSTOM_HOST); // used only for test purposes

} // namespace gr

#endif /* INCLUDED_GR_RUNTIME_CUSTOM_BUFFER_TYPE_H */