/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2009,2010 Free Software Foundation, Inc.
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

#include <gr_block_executor.h>
#include <gr_block.h>
#include <gr_block_detail.h>
#include <gr_buffer.h>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <limits>
#include <assert.h>
#include <stdio.h>

// must be defined to either 0 or 1
#define ENABLE_LOGGING 0

#if (ENABLE_LOGGING)
#define LOG(x) do { x; } while(0)
#else
#define LOG(x) do {;} while(0)
#endif

static int which_scheduler  = 0;

inline static unsigned int
round_up (unsigned int n, unsigned int multiple)
{
  return ((n + multiple - 1) / multiple) * multiple;
}

inline static unsigned int
round_down (unsigned int n, unsigned int multiple)
{
  return (n / multiple) * multiple;
}

//
// Return minimum available write space in all our downstream buffers
// or -1 if we're output blocked and the output we're blocked
// on is done.
//
static int
min_available_space (gr_block_detail *d, int output_multiple)
{
  int	min_space = std::numeric_limits<int>::max();

  for (int i = 0; i < d->noutputs (); i++){
    gruel::scoped_lock guard(*d->output(i)->mutex());
#if 0
    int n = round_down(d->output(i)->space_available(), output_multiple);
#else
    int n = round_down(std::min(d->output(i)->space_available(),
				d->output(i)->bufsize()/2),
		       output_multiple);
#endif
    if (n == 0){			// We're blocked on output.
      if (d->output(i)->done()){	// Downstream is done, therefore we're done.
	return -1;
      }
      return 0;
    }
    min_space = std::min (min_space, n);
  }
  return min_space;
}

static bool
propagate_tags(gr_block::tag_propagation_policy_t policy, gr_block_detail *d,
	       const std::vector<uint64_t> &start_nitems_read, double rrate,
	       std::vector<gr_tag_t> &rtags)
{
  // Move tags downstream
  // if a sink, we don't need to move downstream
  if(d->sink_p()) {
    return true;
  }

  switch(policy) {
  case gr_block::TPP_DONT:
    return true;
    break;
  case gr_block::TPP_ALL_TO_ALL:
    // every tag on every input propogates to everyone downstream
    for(int i = 0; i < d->ninputs(); i++) {
      d->get_tags_in_range(rtags, i, start_nitems_read[i],
			   d->nitems_read(i));

      std::vector<gr_tag_t>::iterator t;
      if(rrate == 1.0) {
	for(t = rtags.begin(); t != rtags.end(); t++) {
	  for(int o = 0; o < d->noutputs(); o++)
	    d->output(o)->add_item_tag(*t);
	}
      }
      else {
	for(t = rtags.begin(); t != rtags.end(); t++) {
	  gr_tag_t new_tag = *t;
	  new_tag.offset *= rrate;
	  for(int o = 0; o < d->noutputs(); o++)
	    d->output(o)->add_item_tag(new_tag);
	}
      }
    }
    break;
  case gr_block::TPP_ONE_TO_ONE:
    // tags from input i only go to output i
    // this requires d->ninputs() == d->noutputs; this is checked when this
    // type of tag-propagation system is selected in gr_block_detail
    if(d->ninputs() == d->noutputs()) {
      for(int i = 0; i < d->ninputs(); i++) {
	d->get_tags_in_range(rtags, i, start_nitems_read[i],
			     d->nitems_read(i));

	std::vector<gr_tag_t>::iterator t;
	for(t = rtags.begin(); t != rtags.end(); t++) {
	  gr_tag_t new_tag = *t;
	  new_tag.offset *= rrate;
	  d->output(i)->add_item_tag(new_tag);
	}
      }
    }
    else  {
      std::cerr << "Error: gr_block_executor: propagation_policy 'ONE-TO-ONE' requires ninputs == noutputs" << std::endl;
      return false;
    }
    
    break;
  default:
    return true;
  }
  return true;
}

gr_block_executor::gr_block_executor (gr_block_sptr block, int max_noutput_items)
  : d_block(block), d_log(0), d_max_noutput_items(max_noutput_items)
{
  if (ENABLE_LOGGING){
    std::string name = str(boost::format("sst-%03d.log") % which_scheduler++);
    d_log = new std::ofstream(name.c_str());
    std::unitbuf(*d_log);		// make it unbuffered...
    *d_log << "gr_block_executor: "
	   << d_block << std::endl;
  }

  d_block->start();			// enable any drivers, etc.
}

gr_block_executor::~gr_block_executor ()
{
  if (ENABLE_LOGGING)
    delete d_log;

  d_block->stop();			// stop any drivers, etc.
}

gr_block_executor::state
gr_block_executor::run_one_iteration()
{
  int			noutput_items;
  int			max_items_avail;
  int                   max_noutput_items = d_max_noutput_items;
  int                   new_alignment=0;
  int                   alignment_state=-1;

  gr_block		*m = d_block.get();
  gr_block_detail	*d = m->detail().get();

  LOG(*d_log << std::endl << m);

  if (d->done()){
    assert(0);
    return DONE;
  }

  if (d->source_p ()){
    d_ninput_items_required.resize (0);
    d_ninput_items.resize (0);
    d_input_items.resize (0);
    d_input_done.resize(0);
    d_output_items.resize (d->noutputs ());
    d_start_nitems_read.resize(0);

    // determine the minimum available output space
    noutput_items = min_available_space (d, m->output_multiple ());
    noutput_items = std::min(noutput_items, d_max_noutput_items);
    LOG(*d_log << " source\n  noutput_items = " << noutput_items << std::endl);
    if (noutput_items == -1)		// we're done
      goto were_done;

    if (noutput_items == 0){		// we're output blocked
      LOG(*d_log << "  BLKD_OUT\n");
      return BLKD_OUT;
    }

    goto setup_call_to_work;		// jump to common code
  }

  else if (d->sink_p ()){
    d_ninput_items_required.resize (d->ninputs ());
    d_ninput_items.resize (d->ninputs ());
    d_input_items.resize (d->ninputs ());
    d_input_done.resize(d->ninputs());
    d_output_items.resize (0);
    d_start_nitems_read.resize(d->ninputs());
    LOG(*d_log << " sink\n");

    max_items_avail = 0;
    for (int i = 0; i < d->ninputs (); i++){
      {
	/*
	 * Acquire the mutex and grab local copies of items_available and done.
	 */
	gruel::scoped_lock guard(*d->input(i)->mutex());
	d_ninput_items[i] = d->input(i)->items_available();
	d_input_done[i] = d->input(i)->done();
      }

      LOG(*d_log << "  d_ninput_items[" << i << "] = " << d_ninput_items[i] << std::endl);
      LOG(*d_log << "  d_input_done[" << i << "] = " << d_input_done[i] << std::endl);
      
      if (d_ninput_items[i] < m->output_multiple() && d_input_done[i])
	goto were_done;
	
      max_items_avail = std::max (max_items_avail, d_ninput_items[i]);
    }

    // take a swag at how much output we can sink
    noutput_items = (int) (max_items_avail * m->relative_rate ());
    noutput_items = round_down (noutput_items, m->output_multiple ());
    noutput_items = std::min(noutput_items, d_max_noutput_items);
    LOG(*d_log << "  max_items_avail = " << max_items_avail << std::endl);
    LOG(*d_log << "  noutput_items = " << noutput_items << std::endl);

    if (noutput_items == 0){	// we're blocked on input
      LOG(*d_log << "  BLKD_IN\n");
      return BLKD_IN;
    }

    goto try_again;		// Jump to code shared with regular case.
  }

  else {
    // do the regular thing
    d_ninput_items_required.resize (d->ninputs ());
    d_ninput_items.resize (d->ninputs ());
    d_input_items.resize (d->ninputs ());
    d_input_done.resize(d->ninputs());
    d_output_items.resize (d->noutputs ());
    d_start_nitems_read.resize(d->ninputs());

    max_items_avail = 0;
    for (int i = 0; i < d->ninputs (); i++){
      {
	/*
	 * Acquire the mutex and grab local copies of items_available and done.
	 */
	gruel::scoped_lock guard(*d->input(i)->mutex());
	d_ninput_items[i] = d->input(i)->items_available ();
	d_input_done[i] = d->input(i)->done();
      }
      max_items_avail = std::max (max_items_avail, d_ninput_items[i]);
    }

    // determine the minimum available output space
    noutput_items = min_available_space (d, m->output_multiple ());
    if (ENABLE_LOGGING){
      *d_log << " regular ";
      if (m->relative_rate() >= 1.0)
	*d_log << "1:" << m->relative_rate() << std::endl;
      else
	*d_log << 1.0/m->relative_rate() << ":1\n";
      *d_log << "  max_items_avail = " << max_items_avail << std::endl;
      *d_log << "  noutput_items = " << noutput_items << std::endl;
    }
    if (noutput_items == -1)		// we're done
      goto were_done;

    if (noutput_items == 0){		// we're output blocked
      LOG(*d_log << "  BLKD_OUT\n");
      return BLKD_OUT;
    }

  try_again:
    if (m->fixed_rate()){
      // try to work it forward starting with max_items_avail.
      // We want to try to consume all the input we've got.
      int reqd_noutput_items = m->fixed_rate_ninput_to_noutput(max_items_avail);
      
      // only test this if we specifically set the output_multiple
      if(m->output_multiple_set())
	reqd_noutput_items = round_down(reqd_noutput_items, m->output_multiple());

      if (reqd_noutput_items > 0 && reqd_noutput_items <= noutput_items)
	noutput_items = reqd_noutput_items;

      // if we need this many outputs, overrule the max_noutput_items setting
      max_noutput_items = std::max(m->output_multiple(), max_noutput_items);
    }
    noutput_items = std::min(noutput_items, max_noutput_items);

    // Check if we're still unaligned; use up items until we're
    // aligned again. Otherwise, make sure we set the alignment
    // requirement.
    if(!m->output_multiple_set()) {
      if(m->is_unaligned()) {
	// When unaligned, don't just set noutput_items to the remaining
	// samples to meet alignment; this causes too much overhead in
	// requiring a premature call back here. Set the maximum amount
	// of samples to handle unalignment and get us back aligned.
	if(noutput_items >= m->unaligned()) {
	  noutput_items = round_up(noutput_items, m->alignment())	\
	    - (m->alignment() - m->unaligned());
	  new_alignment = 0;
	}
	else {
	  new_alignment = m->unaligned() - noutput_items;
	}
	alignment_state = 0;
      }
      else if(noutput_items < m->alignment()) {
	// if we don't have enough for an aligned call, keep track of
	// misalignment, set unaligned flag, and proceed.
	new_alignment = m->alignment() - noutput_items;
	m->set_unaligned(new_alignment);
	m->set_is_unaligned(true);
	alignment_state = 1;
      }
      else {
	// enough to round down to the nearest alignment and process.
	noutput_items = round_down(noutput_items, m->alignment());
	m->set_is_unaligned(false);
	alignment_state = 2;
      }
    }

    // ask the block how much input they need to produce noutput_items
    m->forecast (noutput_items, d_ninput_items_required);

    // See if we've got sufficient input available

    int i;
    for (i = 0; i < d->ninputs (); i++)
      if (d_ninput_items_required[i] > d_ninput_items[i])	// not enough
	break;

    if (i < d->ninputs ()){			// not enough input on input[i]
      // if we can, try reducing the size of our output request
      if (noutput_items > m->output_multiple ()){
	noutput_items /= 2;
	noutput_items = round_up (noutput_items, m->output_multiple ());
	goto try_again;
      }

      // We're blocked on input
      LOG(*d_log << "  BLKD_IN\n");
      if (d_input_done[i]) 	// If the upstream block is done, we're done
	goto were_done;

      // Is it possible to ever fulfill this request?
      if (d_ninput_items_required[i] > d->input(i)->max_possible_items_available ()){
	// Nope, never going to happen...
	std::cerr << "\nsched: <gr_block " << m->name()
		  << " (" << m->unique_id() << ")>"
		  << " is requesting more input data\n"
		  << "  than we can provide.\n"
		  << "  ninput_items_required = "
		  << d_ninput_items_required[i] << "\n"
		  << "  max_possible_items_available = "
		  << d->input(i)->max_possible_items_available() << "\n"
		  << "  If this is a filter, consider reducing the number of taps.\n";
	goto were_done;
      }

      // If we were made unaligned in this round but return here without
      // processing; reset the unalignment claim before next entry.
      if(alignment_state == 1) {
	m->set_unaligned(0);
	m->set_is_unaligned(false);
      }
      return BLKD_IN;
    }

    // We've got enough data on each input to produce noutput_items.
    // Finish setting up the call to work.

    for (int i = 0; i < d->ninputs (); i++)
      d_input_items[i] = d->input(i)->read_pointer();

  setup_call_to_work:

    d->d_produce_or = 0;
    for (int i = 0; i < d->noutputs (); i++)
      d_output_items[i] = d->output(i)->write_pointer();

    // determine where to start looking for new tags
    for (int i = 0; i < d->ninputs(); i++)
      d_start_nitems_read[i] = d->nitems_read(i);

    // Do the actual work of the block
    int n = m->general_work (noutput_items, d_ninput_items,
			     d_input_items, d_output_items);
    LOG(*d_log << "  general_work: noutput_items = " << noutput_items
	<< " result = " << n << std::endl);

    // Adjust number of unaligned items left to process
    if(m->is_unaligned()) {
      m->set_unaligned(new_alignment);
      m->set_is_unaligned(m->unaligned() != 0);
    }

    if(!propagate_tags(m->tag_propagation_policy(), d,
		       d_start_nitems_read, m->relative_rate(),
		       d_returned_tags))
      goto were_done;

    if (n == gr_block::WORK_DONE)
      goto were_done;

    if (n != gr_block::WORK_CALLED_PRODUCE)
      d->produce_each (n);	// advance write pointers
    
    if (d->d_produce_or > 0)	// block produced something
      return READY;

    // We didn't produce any output even though we called general_work.
    // We have (most likely) consumed some input.

    // If this is a source, it's broken.
    if (d->source_p()){
      std::cerr << "gr_block_executor: source " << m
		<< " produced no output.  We're marking it DONE.\n";
      // FIXME maybe we ought to raise an exception...
      goto were_done;
    }

    // Have the caller try again...
    return READY_NO_OUTPUT;
  }
  assert (0);
    
 were_done:
  LOG(*d_log << "  were_done\n");
  d->set_done (true);
  return DONE;
}
