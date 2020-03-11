/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/trellis/quicksort_index.h>

namespace gr {
namespace trellis {

template <class T>
void SWAP(T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
}

template <class T>
void quicksort_index(std::vector<T>& p, std::vector<int>& index, int left, int right)
{
    if (left < right) {
        int i = left;
        int j = right + 1;
        T pivot = p[left];
        do {
            do
                i++;
            while ((p[i] < pivot) && (i < right));
            do
                j--;
            while ((p[j] > pivot) && (j > left));
            if (i < j) {
                SWAP<T>(p[i], p[j]);
                SWAP<int>(index[i], index[j]);
            }
        } while (i < j);
        SWAP<T>(p[left], p[j]);
        SWAP<int>(index[left], index[j]);
        quicksort_index<T>(p, index, left, j - 1);
        quicksort_index<T>(p, index, j + 1, right);
    }
}

// instantiate an <int> version of the quicksort_index
// template <int> void SWAP (int & a, int & b);
template void
quicksort_index<int>(std::vector<int>& p, std::vector<int>& index, int left, int right);

} /* namespace trellis */
} /* namespace gr */
