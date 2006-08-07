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

#ifndef INCLUDED_TRELLIS_FSM_H
#define INCLUDED_TRELLIS_FSM_H

#include <vector>

/*!
 * \brief  FSM class
 */
class fsm {
private:
  int d_I;
  int d_S;
  int d_O;
  std::vector<int> d_NS;
  std::vector<int> d_OS;
  std::vector<int> d_PS;
  std::vector<int> d_PI;
public:
  fsm();
  fsm(const fsm &FSM);
  fsm(const int I, const int S, const int O, const std::vector<int> &NS, const std::vector<int> &OS);
  fsm(const char *name);
  fsm(const int mod_size, const int ch_length);
  int I () const { return d_I; }
  int S () const { return d_S; }
  int O () const { return d_O; }
  const std::vector<int> & NS () const { return d_NS; }
  const std::vector<int> & OS () const { return d_OS; }
  const std::vector<int> & PS () const { return d_PS; }
  const std::vector<int> & PI () const { return d_PI; }
};

#endif
