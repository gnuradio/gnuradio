/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_INTERLEAVER_FIFO_H
#define INCLUDED_DTV_ATSC_INTERLEAVER_FIFO_H

#include <string.h>

namespace gr {
namespace dtv {

template <class symbol_type>
class interleaver_fifo
{
public:
    interleaver_fifo(unsigned int size);
    ~interleaver_fifo();

    //! reset interleaver (flushes contents and resets commutator)
    void reset();

    //! stuff a symbol into the fifo and return the oldest
    symbol_type stuff(symbol_type input)
    {
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
    unsigned int m_size;
    unsigned int m_position;
    symbol_type* m_fifo;
};

template <class symbol_type>
interleaver_fifo<symbol_type>::interleaver_fifo(unsigned int size)
{
    m_size = size;
    m_position = 0;
    m_fifo = new symbol_type[size];
    memset(m_fifo, 0, m_size * sizeof(symbol_type));
}

template <class symbol_type>
interleaver_fifo<symbol_type>::~interleaver_fifo()
{
    delete[] m_fifo;
}

template <class symbol_type>
void interleaver_fifo<symbol_type>::reset()
{
    m_position = 0;
    memset(m_fifo, 0, m_size * sizeof(symbol_type));
}

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_INTERLEAVER_FIFO_H */
