/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_BUFFER_H
#define INCLUDED_GR_BUFFER_H

#include <gr_core_api.h>
#include <gr_runtime_types.h>
#include <boost/weak_ptr.hpp>
#include <gruel/thread.h>
#include <gr_tags.h>
#include <deque>

class gr_vmcircbuf;

/*!
 * \brief Allocate a buffer that holds at least \p nitems of size \p sizeof_item.
 *
 * The total size of the buffer will be rounded up to a system
 * dependent boundary.  This is typically the system page size, but
 * under MS windows is 64KB.
 *
 * \param nitems is the minimum number of items the buffer will hold.
 * \param sizeof_item is the size of an item in bytes.
 * \param link is the block that writes to this buffer.
 */
GR_CORE_API gr_buffer_sptr gr_make_buffer (int nitems, size_t sizeof_item, gr_block_sptr link=gr_block_sptr());


/*!
 * \brief Single writer, multiple reader fifo.
 * \ingroup internal
 */
class GR_CORE_API gr_buffer {
 public:

  virtual ~gr_buffer ();

  /*!
   * \brief return number of items worth of space available for writing
   */
  int space_available ();

  /*!
   * \brief return size of this buffer in items
   */
  int bufsize() const { return d_bufsize; }

  /*!
   * \brief return pointer to write buffer.
   *
   * The return value points at space that can hold at least
   * space_available() items.
   */
  void *write_pointer ();

  /*!
   * \brief tell buffer that we wrote \p nitems into it
   */
  void update_write_pointer (int nitems);

  void set_done (bool done);
  bool done () const { return d_done; }

  /*!
   * \brief Return the block that writes to this buffer.
   */
  gr_block_sptr link() { return gr_block_sptr(d_link); }

  size_t nreaders() const { return d_readers.size(); }
  gr_buffer_reader* reader(size_t index) { return d_readers[index]; }

  gruel::mutex *mutex() { return &d_mutex; }

  uint64_t nitems_written() { return d_abs_write_offset; }

  size_t get_sizeof_item() { return d_sizeof_item; }

  /*!
   * \brief  Adds a new tag to the buffer.
   * 
   * \param tag        the new tag
   */
  void add_item_tag(const gr_tag_t &tag);

  /*!
   * \brief  Removes all tags before \p max_time from buffer
   * 
   * \param max_time        the time (item number) to trim up until.
   */
  void prune_tags(uint64_t max_time);

  std::deque<gr_tag_t>::iterator get_tags_begin() { return d_item_tags.begin(); }
  std::deque<gr_tag_t>::iterator get_tags_end() { return d_item_tags.end(); }

  // -------------------------------------------------------------------------

 private:

  friend class gr_buffer_reader;
  friend GR_CORE_API gr_buffer_sptr gr_make_buffer (int nitems, size_t sizeof_item, gr_block_sptr link);
  friend GR_CORE_API gr_buffer_reader_sptr gr_buffer_add_reader (gr_buffer_sptr buf, int nzero_preload, gr_block_sptr link);

 protected:
  char				       *d_base;		// base address of buffer
  unsigned int	 			d_bufsize;	// in items
 private:
  gr_vmcircbuf			       *d_vmcircbuf;
  size_t	 			d_sizeof_item;	// in bytes
  std::vector<gr_buffer_reader *>	d_readers;
  boost::weak_ptr<gr_block>		d_link;		// block that writes to this buffer

  //
  // The mutex protects d_write_index, d_abs_write_offset, d_done, d_item_tags 
  // and the d_read_index's and d_abs_read_offset's in the buffer readers.
  //
  gruel::mutex				d_mutex;
  unsigned int				d_write_index;	// in items [0,d_bufsize)
  uint64_t                              d_abs_write_offset; // num items written since the start
  bool					d_done;
  std::deque<gr_tag_t>                  d_item_tags;
  uint64_t                              d_last_min_items_read;
  
  unsigned
  index_add (unsigned a, unsigned b)
  {
    unsigned s = a + b;

    if (s >= d_bufsize)
      s -= d_bufsize;

    assert (s < d_bufsize);
    return s;
  }

  unsigned
  index_sub (unsigned a, unsigned b)
  {
    int s = a - b;

    if (s < 0)
      s += d_bufsize;

    assert ((unsigned) s < d_bufsize);
    return s;
  }

  virtual bool allocate_buffer (int nitems, size_t sizeof_item);

  /*!
   * \brief constructor is private.  Use gr_make_buffer to create instances.
   *
   * Allocate a buffer that holds at least \p nitems of size \p sizeof_item.
   *
   * \param nitems is the minimum number of items the buffer will hold.
   * \param sizeof_item is the size of an item in bytes.
   * \param link is the block that writes to this buffer.
   *
   * The total size of the buffer will be rounded up to a system
   * dependent boundary.  This is typically the system page size, but
   * under MS windows is 64KB.
   */
  gr_buffer (int nitems, size_t sizeof_item, gr_block_sptr link);

  /*!
   * \brief disassociate \p reader from this buffer
   */
  void drop_reader (gr_buffer_reader *reader);

};

/*!
 * \brief Create a new gr_buffer_reader and attach it to buffer \p buf
 * \param buf is the buffer the \p gr_buffer_reader reads from.
 * \param nzero_preload -- number of zero items to "preload" into buffer.
 * \param link is the block that reads from the buffer using this gr_buffer_reader.
 */
GR_CORE_API gr_buffer_reader_sptr 
gr_buffer_add_reader (gr_buffer_sptr buf, int nzero_preload, gr_block_sptr link=gr_block_sptr());

//! returns # of gr_buffers currently allocated
GR_CORE_API long gr_buffer_ncurrently_allocated ();


// ---------------------------------------------------------------------------

/*!
 * \brief How we keep track of the readers of a gr_buffer.
 * \ingroup internal
 */

class GR_CORE_API gr_buffer_reader {
 public:

  ~gr_buffer_reader ();

  /*!
   * \brief Return number of items available for reading.
   */
  int items_available () const;

  /*!
   * \brief Return buffer this reader reads from.
   */
  gr_buffer_sptr buffer () const { return d_buffer; }


  /*!
   * \brief Return maximum number of items that could ever be available for reading.
   * This is used as a sanity check in the scheduler to avoid looping forever.
   */
  int max_possible_items_available () const { return d_buffer->d_bufsize - 1; }

  /*!
   * \brief return pointer to read buffer.
   *
   * The return value points to items_available() number of items
   */
  const void *read_pointer ();

  /*
   * \brief tell buffer we read \p items from it
   */
  void update_read_pointer (int nitems);

  void set_done (bool done)   { d_buffer->set_done (done); }
  bool done () const { return d_buffer->done (); }

  gruel::mutex *mutex() { return d_buffer->mutex(); }


  uint64_t nitems_read() { return d_abs_read_offset; }

  size_t get_sizeof_item() { return d_buffer->get_sizeof_item(); }

  /*!
   * \brief Return the block that reads via this reader.
   *
   */
  gr_block_sptr link() { return gr_block_sptr(d_link); }


  /*!
   * \brief Given a [start,end), returns a vector all tags in the range.
   *
   * Get a vector of tags in given range. Range of counts is from start to end-1.
   *
   * Tags are tuples of:
   *      (item count, source id, key, value)
   *
   * \param v            a vector reference to return tags into
   * \param abs_start    a uint64 count of the start of the range of interest
   * \param abs_end      a uint64 count of the end of the range of interest
   */
  void get_tags_in_range(std::vector<gr_tag_t> &v,
			 uint64_t abs_start,
			 uint64_t abs_end);

  // -------------------------------------------------------------------------

 private:

  friend class gr_buffer;
  friend GR_CORE_API gr_buffer_reader_sptr 
  gr_buffer_add_reader (gr_buffer_sptr buf, int nzero_preload, gr_block_sptr link);


  gr_buffer_sptr		d_buffer;
  unsigned int			d_read_index;	// in items [0,d->buffer.d_bufsize)
  uint64_t                      d_abs_read_offset;  // num items seen since the start
  boost::weak_ptr<gr_block>	d_link;		// block that reads via this buffer reader

  //! constructor is private.  Use gr_buffer::add_reader to create instances
  gr_buffer_reader (gr_buffer_sptr buffer, unsigned int read_index, gr_block_sptr link);
};

//! returns # of gr_buffer_readers currently allocated
GR_CORE_API long gr_buffer_reader_ncurrently_allocated ();


#endif /* INCLUDED_GR_BUFFER_H */
