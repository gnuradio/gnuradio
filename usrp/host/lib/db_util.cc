/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <db_util.h>
#include <sstream>

std::string
int_seq_to_str(std::vector<int> &seq)
{
  //convert a sequence of integers into a string

  std::stringstream str; 
  std::vector<int>::iterator i;
  for(i = seq.begin(); i != seq.end(); i++) {
    str << char((unsigned int)*i);
  }
  return str.str();
}

std::vector<int> 
str_to_int_seq(std::string str)
{
  //convert a string to a list of integers
  std::vector<int> seq;
  std::vector<int>::iterator sitr;
  std::string::iterator i;
  for(i=str.begin(); i != str.end(); i++) {
    int a = (int)(*i);
    seq.push_back(a);
  }
  return seq;
}

