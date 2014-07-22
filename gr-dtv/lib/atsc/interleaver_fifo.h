/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef _INTERLEAVER_FIFO_H_
#define _INTERLEAVER_FIFO_H_

#include <string.h>

/*!
 * \brief template class for interleaver fifo
 */

template<class symbol_type>
class interleaver_fifo {
 public:

  interleaver_fifo (unsigned int size);
  ~interleaver_fifo ();

  //! reset interleaver (flushes contents and resets commutator)
  void reset ();

  //! stuff a symbol into the fifo and return the oldest
  symbol_type stuff (symbol_type input){
    if (m_size == 0)
      return input;

    symbol_type retval = m_fifo[m_position];
    m_fifo[m_position] = input;
    m_position++;
    if (m_position >= m_size)
      m_position = 0;

    return retval;
  }

protected:
  unsigned int	m_size;
  unsigned int	m_position;
  symbol_type	*m_fifo;
};

template<class symbol_type>
interleaver_fifo<symbol_type>::interleaver_fifo (unsigned int size)
{
  m_size = size;
  m_position = 0;
  m_fifo = new symbol_type[size];
  memset (m_fifo, 0, m_size * sizeof (symbol_type));
}

template<class symbol_type>
interleaver_fifo<symbol_type>::~interleaver_fifo ()
{
  delete [] m_fifo;
}

template<class symbol_type> void
interleaver_fifo<symbol_type>::reset ()
{
  m_position = 0;
  memset (m_fifo, 0, m_size * sizeof (symbol_type));
}

#endif /* _INTERLEAVER_FIFO_H_ */
