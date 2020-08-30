/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef _INTERLEAVER_FIFO_H_
#define _INTERLEAVER_FIFO_H_

#include <algorithm>
#include <vector>

/*!
 * \brief template class for interleaver fifo
 */

template <class symbol_type>
class interleaver_fifo
{
public:
    interleaver_fifo(unsigned int size);

    //! reset interleaver (flushes contents and resets commutator)
    void reset();

    //! stuff a symbol into the fifo and return the oldest
    symbol_type stuff(symbol_type input)
    {
        if (m_fifo.empty())
            return input;

        symbol_type retval = m_fifo[m_position];
        m_fifo[m_position] = input;
        m_position++;
        if (m_position >= m_fifo.size())
            m_position = 0;

        return retval;
    }

protected:
    unsigned int m_position = 0;
    std::vector<symbol_type> m_fifo;
};

template <class symbol_type>
interleaver_fifo<symbol_type>::interleaver_fifo(unsigned int size) : m_fifo(size)
{
}

template <class symbol_type>
void interleaver_fifo<symbol_type>::reset()
{
    m_position = 0;
    std::fill(std::begin(m_fifo), std::end(m_fifo), 0);
}

#endif /* _INTERLEAVER_FIFO_H_ */
