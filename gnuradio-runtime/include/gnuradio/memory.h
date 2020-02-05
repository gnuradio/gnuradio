/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MEMORY_H
#define INCLUDED_GR_MEMORY_H

namespace gr {

/*!
 * \brief Constructs an object of type T and wraps it in a std::unique_ptr.
 *
 * The gr::make_unique is needed because boost::make_unique was added
 * in boost 1.56 while the minimal boost version that gnuradio requires is 1.53.
 *
 * The other alternative is std::make_unique but it is a c++14 feature
 * and once gnuradio moves to c++14 gr::make_unique can be removed.
 */
template <class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace gr

#endif /* INCLUDED_GR_MEMORY_H */
