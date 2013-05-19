/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#ifndef _CONVOLUTIONAL_INTERLEAVER_H_
#define _CONVOLUTIONAL_INTERLEAVER_H_

#include <vector>
#include <gnuradio/atsc/interleaver_fifo.h>
#include <assert.h>

/*!
 * \brief template class for generic convolutional interleaver
 */

template<class symbol_type>
class convolutional_interleaver {
 public:

  convolutional_interleaver (bool interleave_p, int nbanks, int fifo_size_incr);
  virtual ~convolutional_interleaver ();

  //! reset interleaver (flushes contents and resets commutator)
  void reset ();

  //! sync interleaver (resets commutator, but doesn't flush fifos)
  void sync () { m_commutator = 0; }

  //! return end to end delay in symbols (delay through concatenated interleaver / deinterleaver)
  int end_to_end_delay ();

  //! transform a single symbol
  symbol_type transform (symbol_type input){
    symbol_type retval = m_fifo[m_commutator]->stuff (input);
    m_commutator++;
    if (m_commutator >= m_nbanks)
      m_commutator = 0;
    return retval;
  }

  //! transform a bunch of symbols
  void transform (symbol_type *out, const symbol_type *in, int nsymbols);

protected:
  int	m_commutator;
  int	m_nbanks;
  int	m_fifo_size_incr;
  std::vector<interleaver_fifo<symbol_type> *> m_fifo;
};

template<class symbol_type>
convolutional_interleaver<symbol_type>::convolutional_interleaver (
					   bool interleave_p,
					   int nbanks,
					   int fifo_size_incr)
{
  assert (nbanks >= 1);
  assert (fifo_size_incr >= 1);

  m_nbanks = nbanks;
  m_fifo_size_incr = fifo_size_incr;

  m_fifo.resize (nbanks);

  if (interleave_p){	// configure as interleaver
    for (int i = 0; i < nbanks; i++)
      m_fifo[i] = new interleaver_fifo<symbol_type>(i * fifo_size_incr);
  }
  else {		// configure as de-interleaver
    for (int i = 0; i < nbanks; i++)
      m_fifo[nbanks - 1 - i] = new interleaver_fifo<symbol_type>(i * fifo_size_incr);
  }
  sync ();
}

template<class symbol_type>
convolutional_interleaver<symbol_type>::~convolutional_interleaver ()
{
  for (int i = 0; i < m_nbanks; i++)
    delete m_fifo[i];
}

template<class symbol_type> void
convolutional_interleaver<symbol_type>::reset ()
{
  sync ();
  for (int i = 0; i < m_nbanks; i++)
    m_fifo[i]->reset ();
}

template<class symbol_type> int
convolutional_interleaver<symbol_type>::end_to_end_delay ()
{
  int m = m_nbanks * m_fifo_size_incr;
  return m * (m_nbanks - 1);
}

template<class symbol_type> void
convolutional_interleaver<symbol_type>::transform (symbol_type *out,
						   const symbol_type *in,
						   int nsymbols)
{
  // we may want to unroll this a couple of times...
  for (int i = 0; i < nsymbols; i++)
    out[i] = transform (in[i]);
}

#endif /* _CONVOLUTIONAL_INTERLEAVER_H_ */
