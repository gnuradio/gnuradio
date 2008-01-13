/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio.
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

#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include "mld_threads.h"
#include <stdexcept>

#ifndef DO_DEBUG
#define DO_DEBUG 0
#endif

#if DO_DEBUG
#define DEBUG(X) do{X} while(0);
#else
#define DEBUG(X) do{} while(0);
#endif

template <class T> class circular_buffer
{
private:
// the buffer to use
  T* d_buffer;

// the following are in Items (type T)
  UInt32 d_bufLen_I, d_readNdx_I, d_writeNdx_I;
  UInt32 d_n_avail_write_I, d_n_avail_read_I;

// stuff to control access to class internals
  mld_mutex_ptr d_internal;
  mld_condition_ptr d_readBlock, d_writeBlock;

// booleans to decide how to control reading, writing, and aborting
  bool d_doWriteBlock, d_doFullRead, d_doAbort;

  void delete_mutex_cond () {
    if (d_internal) {
      delete d_internal;
      d_internal = NULL;
    }
    if (d_readBlock) {
      delete d_readBlock;
      d_readBlock = NULL;
    }
    if (d_writeBlock) {
      delete d_writeBlock;
      d_writeBlock = NULL;
    }
  };

public:
  circular_buffer (UInt32 bufLen_I,
		   bool doWriteBlock = true, bool doFullRead = false) {
    if (bufLen_I == 0)
      throw std::runtime_error ("circular_buffer(): "
				"Number of items to buffer must be > 0.\n");
    d_bufLen_I = bufLen_I;
    d_buffer = (T*) new T[d_bufLen_I];
    d_doWriteBlock = doWriteBlock;
    d_doFullRead = doFullRead;
    d_internal = NULL;
    d_readBlock = d_writeBlock = NULL;
    reset ();
    DEBUG (fprintf (stderr, "c_b(): buf len (items) = %ld, "
		    "doWriteBlock = %s, doFullRead = %s\n", d_bufLen_I,
		    (d_doWriteBlock ? "true" : "false"),
		    (d_doFullRead ? "true" : "false")););
  };

  ~circular_buffer () {
    delete_mutex_cond ();
    delete [] d_buffer;
  };

  inline UInt32 n_avail_write_items () {
    d_internal->lock ();
    UInt32 retVal = d_n_avail_write_I;
    d_internal->unlock ();
    return (retVal);
  };

  inline UInt32 n_avail_read_items () {
    d_internal->lock ();
    UInt32 retVal = d_n_avail_read_I;
    d_internal->unlock ();
    return (retVal);
  };

  inline UInt32 buffer_length_items () {return (d_bufLen_I);};
  inline bool do_write_block () {return (d_doWriteBlock);};
  inline bool do_full_read () {return (d_doFullRead);};

  void reset () {
    d_doAbort = false;
    bzero (d_buffer, d_bufLen_I * sizeof (T));
    d_readNdx_I = d_writeNdx_I = d_n_avail_read_I = 0;
    d_n_avail_write_I = d_bufLen_I;
    delete_mutex_cond ();
    // create a mutex to handle contention of shared resources;
    // any routine needed access to shared resources uses lock()
    // before doing anything, then unlock() when finished.
    d_internal = new mld_mutex ();
    // link the internal mutex to the read and write conditions;
    // when wait() is called, the internal mutex will automatically
    // be unlock()'ed.  Upon return (from a signal() to the condition),
    // the internal mutex will be lock()'ed.
    d_readBlock = new mld_condition (d_internal);
    d_writeBlock = new mld_condition (d_internal);
  };

/*
 * enqueue: add the given buffer of item-length to the queue,
 *     first-in-first-out (FIFO).
 *
 * inputs:
 *     buf: a pointer to the buffer holding the data
 *
 *     bufLen_I: the buffer length in items (of the instantiated type)
 *
 * returns:
 *    -1: on overflow (write is not blocking, and data is being
 *                     written faster than it is being read)
 *     0: if nothing to do (0 length buffer)
 *     1: if success
 *     2: in the process of aborting, do doing nothing
 *
 * will throw runtime errors if inputs are improper:
 *     buffer pointer is NULL
 *     buffer length is larger than the instantiated buffer length
 */

  int enqueue (T* buf, UInt32 bufLen_I) {
    DEBUG (fprintf (stderr, "enqueue: buf = %X, bufLen = %ld, #av_wr = %ld, "
		    "#av_rd = %ld.\n", (unsigned int)buf, bufLen_I,
		    d_n_avail_write_I, d_n_avail_read_I););
    if (bufLen_I > d_bufLen_I) {
      fprintf (stderr, "cannot add buffer longer (%ld"
	       ") than instantiated length (%ld"
	       ").\n", bufLen_I, d_bufLen_I);
      throw std::runtime_error ("circular_buffer::enqueue()");
    }

    if (bufLen_I == 0)
      return (0);
    if (!buf)
      throw std::runtime_error ("circular_buffer::enqueue(): "
				"input buffer is NULL.\n");
    d_internal->lock ();
    if (d_doAbort) {
      d_internal->unlock ();
      return (2);
    }
    // set the return value to 1: success; change if needed
    int retval = 1;
    if (bufLen_I > d_n_avail_write_I) {
      if (d_doWriteBlock) {
	while (bufLen_I > d_n_avail_write_I) {
	  DEBUG (fprintf (stderr, "enqueue: #len > #a, waiting.\n"););
	  // wait will automatically unlock() the internal mutex
	  d_writeBlock->wait ();
	  // and lock() it here.
	  if (d_doAbort) {
	    d_internal->unlock ();
	    DEBUG (fprintf (stderr, "enqueue: #len > #a, aborting.\n"););
	    return (2);
	  }
	  DEBUG (fprintf (stderr, "enqueue: #len > #a, done waiting.\n"););
	}
      } else {
	d_n_avail_read_I = d_bufLen_I - bufLen_I;
	d_n_avail_write_I = bufLen_I;
	DEBUG (fprintf (stderr, "circular_buffer::enqueue: overflow\n"););
	retval = -1;
      }
    }
    UInt32 n_now_I = d_bufLen_I - d_writeNdx_I, n_start_I = 0;
    if (n_now_I > bufLen_I)
      n_now_I = bufLen_I;
    else if (n_now_I < bufLen_I)
      n_start_I = bufLen_I - n_now_I;
    bcopy (buf, &(d_buffer[d_writeNdx_I]), n_now_I * sizeof (T));
    if (n_start_I) {
      bcopy (&(buf[n_now_I]), d_buffer, n_start_I * sizeof (T));
      d_writeNdx_I = n_start_I;
    } else
      d_writeNdx_I += n_now_I;
    d_n_avail_read_I += bufLen_I;
    d_n_avail_write_I -= bufLen_I;
    d_readBlock->signal ();
    d_internal->unlock ();
    return (retval);
  };

/*
 * dequeue: removes from the queue the number of items requested, or
 *     available, into the given buffer on a FIFO basis.
 *
 * inputs:
 *     buf: a pointer to the buffer into which to copy the data
 *
 *     bufLen_I: pointer to the number of items to remove in items
 *         (of the instantiated type)
 *
 * returns:
 *     0: if nothing to do (0 length buffer)
 *     1: if success
 *     2: in the process of aborting, do doing nothing
 *
 * will throw runtime errors if inputs are improper:
 *     buffer pointer is NULL
 *     buffer length pointer is NULL
 *     buffer length is larger than the instantiated buffer length
 */

  int dequeue (T* buf, UInt32* bufLen_I) {
    DEBUG (fprintf (stderr, "dequeue: buf = %X, *bufLen = %ld, #av_wr = %ld, "
		    "#av_rd = %ld.\n", (unsigned int)buf, *bufLen_I,
		    d_n_avail_write_I, d_n_avail_read_I););
    if (!bufLen_I)
      throw std::runtime_error ("circular_buffer::dequeue(): "
				"input bufLen pointer is NULL.\n");
    if (!buf)
      throw std::runtime_error ("circular_buffer::dequeue(): "
				"input buffer pointer is NULL.\n");
    UInt32 l_bufLen_I = *bufLen_I;
    if (l_bufLen_I == 0)
      return (0);
    if (l_bufLen_I > d_bufLen_I) {
      fprintf (stderr, "cannot remove buffer longer (%ld"
	       ") than instantiated length (%ld"
	       ").\n", l_bufLen_I, d_bufLen_I);
      throw std::runtime_error ("circular_buffer::dequeue()");
    }

    d_internal->lock ();
    if (d_doAbort) {
      d_internal->unlock ();
      return (2);
    }
    if (d_doFullRead) {
      while (d_n_avail_read_I < l_bufLen_I) {
	DEBUG (fprintf (stderr, "dequeue: #a < #len, waiting.\n"););
	// wait will automatically unlock() the internal mutex
	d_readBlock->wait ();
	// and lock() it here.
	if (d_doAbort) {
	  d_internal->unlock ();
	  DEBUG (fprintf (stderr, "dequeue: #a < #len, aborting.\n"););
	  return (2);
	}
	DEBUG (fprintf (stderr, "dequeue: #a < #len, done waiting.\n"););
     }
    } else {
      while (d_n_avail_read_I == 0) {
	DEBUG (fprintf (stderr, "dequeue: #a == 0, waiting.\n"););
	// wait will automatically unlock() the internal mutex
	d_readBlock->wait ();
	// and lock() it here.
	if (d_doAbort) {
	  d_internal->unlock ();
	  DEBUG (fprintf (stderr, "dequeue: #a == 0, aborting.\n"););
	  return (2);
	}
	DEBUG (fprintf (stderr, "dequeue: #a == 0, done waiting.\n"););
      }
    }
    if (l_bufLen_I > d_n_avail_read_I)
      l_bufLen_I = d_n_avail_read_I;
    UInt32 n_now_I = d_bufLen_I - d_readNdx_I, n_start_I = 0;
    if (n_now_I > l_bufLen_I)
      n_now_I = l_bufLen_I;
    else if (n_now_I < l_bufLen_I)
      n_start_I = l_bufLen_I - n_now_I;
    bcopy (&(d_buffer[d_readNdx_I]), buf, n_now_I * sizeof (T));
    if (n_start_I) {
      bcopy (d_buffer, &(buf[n_now_I]), n_start_I * sizeof (T));
      d_readNdx_I = n_start_I;
    } else
      d_readNdx_I += n_now_I;
    *bufLen_I = l_bufLen_I;
    d_n_avail_read_I -= l_bufLen_I;
    d_n_avail_write_I += l_bufLen_I;
    d_writeBlock->signal ();
    d_internal->unlock ();
    return (1);
  };

  void abort () {
    d_internal->lock ();
    d_doAbort = true;
    d_writeBlock->signal ();
    d_readBlock->signal ();
    d_internal->unlock ();
  };
};

#endif /* _CIRCULAR_BUFFER_H_ */
