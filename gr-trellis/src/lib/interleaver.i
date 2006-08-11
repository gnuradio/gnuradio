/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

class interleaver {
private:
  int d_K;
  std::vector<int> d_INTER;
  std::vector<int> d_DEINTER;
public:
  interleaver();
  interleaver(const interleaver & INTERLEAVER);
  interleaver(const int K, const std::vector<int> & INTER);
  interleaver(const char *name);
  int K () const { return d_K; }
  const std::vector<int> & INTER () const { return d_INTER; }
  const std::vector<int> & DEINTER () const { return d_DEINTER; }
};
