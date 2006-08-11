/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "quicksort_index.h"

template <class T> void SWAP (T & a, T & b)
{
T temp=a;
a=b;
b=temp;
}

template <class T> void quicksort_index(std::vector<T> & p, std::vector<int> & index, int left, int right)
{
T pivot;

if (left < right) {
    int i = left;
    int j = right + 1;
    pivot = p[left];
    do {
        do 
            i++;
        while ((p[i] < pivot) && (i < right));
        do 
            j--;
        while ((p[j] > pivot) && (j > left));
        if (i < j) {
            SWAP <T> (p[i],p[j]);
            SWAP <int> (index[i],index[j]);
        }
    } while (i < j);
    SWAP <T> (p[left], p[j]);
    SWAP <int> (index[left], index[j]);
    quicksort_index(p,index, left, j-1);
    quicksort_index(p,index, j+1, right);
}
}
