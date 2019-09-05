/*
 * Copyright 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_RUNTIME_RUNTIME_LOG_BACKEND_H
#define INCLUDED_GR_RUNTIME_RUNTIME_LOG_BACKEND_H
#include "gnuradio/log/entry.h"
#include <string>
namespace gr {
namespace log {
/*! Logging backend base class
 */
class GR_RUNTIME_API backend
{
public:
    /*! logging operation
     * \param what the log entry to log
     * \return bool representing success of the logging operation
     */
    virtual bool log(const entry& what) = 0;
    virtual const std::string description() = 0; //! Return a description of the logger
    virtual bool flush() = 0;                    //! Flush the logging state out
    virtual ~backend() = default;                //! virtual dtor
};

} // namespace log
} // namespace gr
#endif /* INCLUDED_GR_RUNTIME_RUNTIME_LOG_BACKEND_H */
