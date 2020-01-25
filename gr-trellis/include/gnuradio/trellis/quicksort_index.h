/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QUICKSORT_INDEX_H
#define INCLUDED_QUICKSORT_INDEX_H

#include <vector>

namespace gr {
namespace trellis {

template <class T>
void SWAP(T& a, T& b);

template <class T>
void quicksort_index(std::vector<T>& p, std::vector<int>& index, int left, int right);

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_QUICKSORT_INDEX_H */
