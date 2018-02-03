/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "single_threaded_scheduler.h"
#include <gnuradio/block.h>
#include <gnuradio/block_detail.h>
#include <gnuradio/buffer.h>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <limits>
#include <assert.h>
#include <stdio.h>

namespace gr {

  // must be defined to either 0 or 1
#define ENABLE_LOGGING 0

#if (ENABLE_LOGGING)
#define LOG(x) do { x; } while(0)
#else
#define LOG(x) do {;} while(0)
#endif

  static int which_scheduler  = 0;

  single_threaded_scheduler_sptr
  make_single_threaded_scheduler(const std::vector<block_sptr> &blocks)
  {
    return single_threaded_scheduler_sptr
      (new single_threaded_scheduler(blocks));
  }

  single_threaded_scheduler::single_threaded_scheduler(const std::vector<block_sptr> &blocks)
    : d_blocks(blocks), d_enabled(true), d_log(0)
  {
    if(ENABLE_LOGGING) {
      std::string name = str(boost::format("sst-%d.log") % which_scheduler++);
      d_log = new std::ofstream(name.c_str());
      *d_log << "single_threaded_scheduler: "
             << d_blocks.size ()
             << " blocks\n";
    }
  }

  single_threaded_scheduler::~single_threaded_scheduler()
  {
    if(ENABLE_LOGGING)
      delete d_log;
  }

  void
  single_threaded_scheduler::run()
  {
    // d_enabled = true;    // KLUDGE
    main_loop ();
  }

  void
  single_threaded_scheduler::stop()
  {
    if(0)
      std::cout << "gr_singled_threaded_scheduler::stop() "
                << this << std::endl;
    d_enabled = false;
  }

  inline static unsigned int
  round_up(unsigned int n, unsigned int multiple)
  {
    return ((n + multiple - 1) / multiple) * multiple;
  }

  inline static unsigned int
  round_down(unsigned int n, unsigned int multiple)
  {
    return (n / multiple) * multiple;
  }

  //
  // Return minimum available write space in all our downstream
  // buffers or -1 if we're output blocked and the output we're
  // blocked on is done.
  //
  static int
  min_available_space(block_detail *d, int output_multiple)
  {
    int min_space = std::numeric_limits<int>::max();

    for(int i = 0; i < d->noutputs (); i++) {
      buffer_sptr out_buf = d->output(i);
      int n = round_down (out_buf->space_available (), output_multiple);
      if(n == 0) {			// We're blocked on output.
        if(out_buf->done()) {	// Downstream is done, therefore we're done.
          return -1;
        }
        return 0;
      }
      min_space = std::min (min_space, n);
    }
    return min_space;
  }

  void
  single_threaded_scheduler::main_loop()
  {
    static const int DEFAULT_CAPACITY = 16;

    int				noutput_items;
    gr_vector_int		ninput_items_required(DEFAULT_CAPACITY);
    gr_vector_int		ninput_items(DEFAULT_CAPACITY);
    gr_vector_const_void_star	input_items(DEFAULT_CAPACITY);
    gr_vector_void_star		output_items(DEFAULT_CAPACITY);
    unsigned int		bi;
    unsigned int		nalive;
    int				max_items_avail;
    bool			made_progress_last_pass;
    bool			making_progress;

    for(unsigned i = 0; i < d_blocks.size (); i++)
      d_blocks[i]->detail()->set_done (false);		// reset any done flags

    for(unsigned i = 0; i < d_blocks.size (); i++)	// enable any drivers, etc.
      d_blocks[i]->start();

    bi = 0;
    made_progress_last_pass = true;
    making_progress = false;

    // Loop while there are still blocks alive

    nalive = d_blocks.size ();
    while(d_enabled && nalive > 0) {
      if(boost::this_thread::interruption_requested())
        break;

      block *m = d_blocks[bi].get ();
      block_detail *d = m->detail().get ();

      LOG(*d_log << std::endl << m);

      if(d->done ())
        goto next_block;

      if(d->source_p ()) {
        // Invoke sources as a last resort. As long as the previous
        // pass made progress, don't call a source.
        if(made_progress_last_pass) {
          LOG(*d_log << "  Skipping source\n");
          goto next_block;
        }

        ninput_items_required.resize (0);
        ninput_items.resize (0);
        input_items.resize (0);
        output_items.resize (d->noutputs ());

        // determine the minimum available output space
        noutput_items = min_available_space (d, m->output_multiple ());
        LOG(*d_log << " source\n  noutput_items = " << noutput_items << std::endl);
        if(noutput_items == -1)    // we're done
          goto were_done;

        if(noutput_items == 0) {   // we're output blocked
          LOG(*d_log << "  BLKD_OUT\n");
          goto next_block;
        }

        goto setup_call_to_work;   // jump to common code
      }

      else if(d->sink_p ()) {
        ninput_items_required.resize (d->ninputs ());
        ninput_items.resize (d->ninputs ());
        input_items.resize (d->ninputs ());
        output_items.resize (0);
        LOG(*d_log << " sink\n");

        max_items_avail = 0;
        for(int i = 0; i < d->ninputs (); i++) {
          buffer_reader_sptr in_buf = d->input(i);
          ninput_items[i] = in_buf->items_available();
          //if (ninput_items[i] == 0 && in_buf->done())
          if(ninput_items[i] < m->output_multiple() && in_buf->done())
            goto were_done;

          max_items_avail = std::max (max_items_avail, ninput_items[i]);
        }

        // take a swag at how much output we can sink
        noutput_items = (int) (max_items_avail * m->relative_rate ());
        noutput_items = round_down (noutput_items, m->output_multiple ());
        LOG(*d_log << "  max_items_avail = " << max_items_avail << std::endl);
        LOG(*d_log << "  noutput_items = " << noutput_items << std::endl);

        if(noutput_items == 0) {      // we're blocked on input
          LOG(*d_log << "  BLKD_IN\n");
          goto next_block;
        }

        goto try_again;    // Jump to code shared with regular case.
      }

      else {
        // do the regular thing
        ninput_items_required.resize(d->ninputs ());
        ninput_items.resize(d->ninputs ());
        input_items.resize(d->ninputs ());
        output_items.resize(d->noutputs ());

        max_items_avail = 0;
        for(int i = 0; i < d->ninputs (); i++) {
          ninput_items[i] = d->input(i)->items_available ();
          max_items_avail = std::max(max_items_avail, ninput_items[i]);
        }

        // determine the minimum available output space
        noutput_items = min_available_space(d, m->output_multiple ());
        if(ENABLE_LOGGING){
          *d_log << " regular ";
          if(m->relative_rate() >= 1.0)
            *d_log << "1:" << m->relative_rate() << std::endl;
          else
            *d_log << 1.0/m->relative_rate() << ":1\n";
          *d_log << "  max_items_avail = " << max_items_avail << std::endl;
          *d_log << "  noutput_items = " << noutput_items << std::endl;
        }
        if(noutput_items == -1)    // we're done
          goto were_done;

        if(noutput_items == 0) {   // we're output blocked
          LOG(*d_log << "  BLKD_OUT\n");
          goto next_block;
        }

#if 0
        // Compute best estimate of noutput_items that we can really use.
        noutput_items =
          std::min((unsigned)noutput_items,
                   std::max((unsigned)m->output_multiple(),
                              round_up((unsigned)(max_items_avail * m->relative_rate()),
                                       m->output_multiple())));

        LOG(*d_log << "  revised noutput_items = " << noutput_items << std::endl);
#endif

      try_again:
        if(m->fixed_rate()) {
          // try to work it forward starting with max_items_avail.
          // We want to try to consume all the input we've got.
          int reqd_noutput_items = m->fixed_rate_ninput_to_noutput(max_items_avail);
          reqd_noutput_items = round_up(reqd_noutput_items, m->output_multiple());
          if(reqd_noutput_items > 0 && reqd_noutput_items <= noutput_items)
            noutput_items = reqd_noutput_items;
        }

        // ask the block how much input they need to produce noutput_items
        m->forecast(noutput_items, ninput_items_required);

        // See if we've got sufficient input available
        int i;
        for(i = 0; i < d->ninputs (); i++)
          if(ninput_items_required[i] > ninput_items[i])   // not enough
            break;

        if(i < d->ninputs()) {    // not enough input on input[i]
          // if we can, try reducing the size of our output request
          if(noutput_items > m->output_multiple ()){
            noutput_items /= 2;
            noutput_items = round_up (noutput_items, m->output_multiple ());
            goto try_again;
          }

          // We're blocked on input
          buffer_reader_sptr in_buf = d->input(i);
          LOG(*d_log << "  BLKD_IN\n");
          if(in_buf->done())    // If the upstream block is done, we're done
            goto were_done;

          // Is it possible to ever fulfill this request?
          if(ninput_items_required[i] > in_buf->max_possible_items_available ()) {
            // Nope, never going to happen...
            std::cerr << "\nsched: <block " << m->name()
                      << " (" << m->unique_id() << ")>"
                      << " is requesting more input data\n"
                      << "  than we can provide.\n"
                      << "  ninput_items_required = "
                      << ninput_items_required[i] << "\n"
                      << "  max_possible_items_available = "
                      << in_buf->max_possible_items_available() << "\n"
                      << "  If this is a filter, consider reducing the number of taps.\n";
            goto were_done;
          }

          goto next_block;
        }

        // We've got enough data on each input to produce noutput_items.
        // Finish setting up the call to work.
        for(int i = 0; i < d->ninputs (); i++)
          input_items[i] = d->input(i)->read_pointer();

      setup_call_to_work:

        for(int i = 0; i < d->noutputs (); i++)
          output_items[i] = d->output(i)->write_pointer();

        // Do the actual work of the block
        int n = m->general_work(noutput_items, ninput_items,
                                input_items, output_items);
        LOG(*d_log << "  general_work: noutput_items = " << noutput_items
            << " result = " << n << std::endl);

        if(n == -1)             // block is done
          goto were_done;

        d->produce_each(n);     // advance write pointers
        if(n > 0)
          making_progress = true;

        goto next_block;
      }
      assert(0);

    were_done:
      LOG(*d_log << "  were_done\n");
      d->set_done (true);
      nalive--;

    next_block:
      if(++bi >= d_blocks.size ()) {
        bi = 0;
        made_progress_last_pass = making_progress;
        making_progress = false;
      }
    }

    for(unsigned i = 0; i < d_blocks.size(); i++)    // disable any drivers, etc.
      d_blocks[i]->stop();
  }

} /* namespace gr */
