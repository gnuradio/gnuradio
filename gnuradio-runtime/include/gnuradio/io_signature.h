/* -*- c++ -*- */
/*
 * Copyright 2004,2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_IO_SIGNATURE_H
#define INCLUDED_IO_SIGNATURE_H

#include <gnuradio/api.h>
#include <gnuradio/runtime_types.h>

namespace gr {

  /*!
   * \brief i/o signature for input and output ports.
   * \brief misc
   */
  class GR_RUNTIME_API io_signature
  {
    int			d_min_streams;
    int			d_max_streams;
    std::vector<int>	d_sizeof_stream_item;

    io_signature(int min_streams, int max_streams,
                 const std::vector<int> &sizeof_stream_items);

  public:
    typedef boost::shared_ptr<io_signature> sptr;

    static const int IO_INFINITE = -1;

    ~io_signature();

    /*!
     * \brief Create an i/o signature
     *
     * \ingroup internal
     * \param min_streams  specify minimum number of streams (>= 0)
     * \param max_streams  specify maximum number of streams (>= min_streams or -1 -> infinite)
     * \param sizeof_stream_item  specify the size of the items in each stream
     */
    static sptr make(int min_streams, int max_streams,
                     int sizeof_stream_item);

    /*!
     * \brief Create an i/o signature
     *
     * \param min_streams  specify minimum number of streams (>= 0)
     * \param max_streams  specify maximum number of streams (>= min_streams or -1 -> infinite)
     * \param sizeof_stream_item1 specify the size of the items in the first stream
     * \param sizeof_stream_item2 specify the size of the items in the second and subsequent streams
     */
    static sptr make2(int min_streams, int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2);

    /*!
     * \brief Create an i/o signature
     *
     * \param min_streams  specify minimum number of streams (>= 0)
     * \param max_streams  specify maximum number of streams (>= min_streams or -1 -> infinite)
     * \param sizeof_stream_item1 specify the size of the items in the first stream
     * \param sizeof_stream_item2 specify the size of the items in the second stream
     * \param sizeof_stream_item3 specify the size of the items in the third and subsequent streams
     */
    static sptr make3(int min_streams, int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2,
                      int sizeof_stream_item3);

    /*!
     * \brief Create an i/o signature
     *
     * \param min_streams  specify minimum number of streams (>= 0)
     * \param max_streams  specify maximum number of streams (>= min_streams or -1 -> infinite)
     * \param sizeof_stream_items specify the size of the items in the streams
     *
     * If there are more streams than there are entries in
     * sizeof_stream_items, the value of the last entry in
     * sizeof_stream_items is used for the missing values.
     * sizeof_stream_items must contain at least 1 entry.
     */
    static sptr makev(int min_streams, int max_streams,
                      const std::vector<int> &sizeof_stream_items);

    int min_streams() const { return d_min_streams; }
    int max_streams() const { return d_max_streams; }
    int sizeof_stream_item(int index) const;
    std::vector<int> sizeof_stream_items() const;
  };

} /* namespace gr */

#endif /* INCLUDED_IO_SIGNATURE_H */
