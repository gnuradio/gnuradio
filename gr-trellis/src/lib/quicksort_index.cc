/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#include "quicksort_index.h"

template <class T>
void
SWAP
(T & a, T & b)
{
  T temp = a;
  a = b;
  b = temp;
}

template <class T>
void
quicksort_index
(std::vector<T> & p, std::vector<int> & index, int left, int right)
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
	SWAP <T> (p[i],p[j]);
	SWAP <int> (index[i],index[j]);
      }
    } while (i < j);
    SWAP <T> (p[left], p[j]);
    SWAP <int> (index[left], index[j]);
    quicksort_index <T> (p,index, left, j-1);
    quicksort_index <T> (p,index, j+1, right);
  }
}

// instantiate an <int> version of the quicksort_index
//template <int> void SWAP (int & a, int & b);
template
void
quicksort_index<int>
(std::vector<int> & p, std::vector<int> & index, int left, int right);
