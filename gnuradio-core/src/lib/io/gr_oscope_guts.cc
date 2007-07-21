/* -*- c++ -*- */
/*
 * Copyright 2003,2005 Free Software Foundation, Inc.
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
#include <config.h>
#endif
#include <gr_oscope_guts.h>
#include <stdexcept>
#include <stdio.h>
#include <algorithm>
#include <unistd.h>
#include <math.h>
#include <assert.h>

static const int OUTPUT_RECORD_SIZE = 2048;	// must be power of 2

static inline int
wrap_bi (int buffer_index)		// wrap buffer index
{
  return buffer_index & (OUTPUT_RECORD_SIZE - 1);
}

static inline int
incr_bi (int buffer_index)		// increment buffer index
{
  return wrap_bi (buffer_index + 1);
}

static inline int
decr_bi (int buffer_index)		// decrement buffer index
{
  return wrap_bi (buffer_index - 1);
}

gr_oscope_guts::gr_oscope_guts (int nchannels, double sample_rate, gr_msg_queue_sptr msgq)
  : d_nchannels (nchannels),
    d_msgq (msgq), 
    d_trigger_mode (gr_TRIG_AUTO),
    d_trigger_channel (0),
    d_sample_rate (sample_rate),
    d_update_rate (20),
    d_trigger_level (0),
    d_obi (0),
    d_state (HOLD_OFF),
    d_decimator_count (0),
    d_decimator_count_init (1),
    d_hold_off_count (0),
    d_hold_off_count_init (OUTPUT_RECORD_SIZE/2-1),
    d_post_trigger_count (0),
    d_post_trigger_count_init (OUTPUT_RECORD_SIZE/2),
    d_prev_sample (0)
{
  if (d_nchannels > MAX_CHANNELS){
    fprintf (stderr, "gr_oscope_guts: too many channels.  MAX_CHANNELS = %d\n", MAX_CHANNELS);
    throw std::runtime_error ("too many channels");
  }

  for (int i = 0; i < MAX_CHANNELS; i++)
    d_buffer[i] = 0;

  for (int i = 0; i < d_nchannels; i++){
    d_buffer[i] = new float [OUTPUT_RECORD_SIZE];
    for (int j = 0; j < OUTPUT_RECORD_SIZE; j++)
      d_buffer[i][j] = 0.0;
  }

  // be sure buffer is full before first write
  enter_hold_off ();
  update_rate_or_decimation_changed ();
}

gr_oscope_guts::~gr_oscope_guts ()
{
  for (int i = 0; i < MAX_CHANNELS; i++)
    delete [] d_buffer[i];
}

// MANIPULATORS

// \p channel_data points to nchannels float values.  These are the values
// for each channel at this sample time.

void
gr_oscope_guts::process_sample (const float *channel_data)
{
  d_decimator_count--;
  if (d_decimator_count > 0)
    return;

  d_decimator_count = d_decimator_count_init;
  
  for (int i = 0; i < d_nchannels; i++)
    d_buffer[i][d_obi] = channel_data[i];		// copy data into buffer

  int trigger = 0;
  
  switch (d_state){
  case HOLD_OFF:
    d_hold_off_count--;
    if (d_hold_off_count <= 0)
      enter_look_for_trigger ();
    break;

  case LOOK_FOR_TRIGGER:
    trigger = found_trigger (d_buffer[d_trigger_channel][d_obi]);
    if (trigger != 0){
      enter_post_trigger ();
      if (trigger < 0)			// previous sample was closer
	d_post_trigger_count--;
    }
    break;

  case POST_TRIGGER:
    d_post_trigger_count--;
    if (d_post_trigger_count <= 0){
      write_output_records ();
      enter_hold_off ();
    }
    break;

  default:
    assert (0);
  }

  d_obi = incr_bi (d_obi);
}

/*
 * Functions called on state entry
 */

void
gr_oscope_guts::enter_hold_off ()
{
  d_state = HOLD_OFF;
  d_hold_off_count = d_hold_off_count_init;
}

void
gr_oscope_guts::enter_look_for_trigger ()
{
  d_state = LOOK_FOR_TRIGGER;
  d_prev_sample = d_buffer[d_trigger_channel][d_obi];
}

void
gr_oscope_guts::enter_post_trigger ()
{
  d_state = POST_TRIGGER;
  d_post_trigger_count = d_post_trigger_count_init;
}

// ----------------------------------------------------------------
// returns 0 if no trigger found. 
// returns +1 if this sample is the trigger point
// returns -1 if the previous sample is the trigger point

int
gr_oscope_guts::found_trigger (float new_sample)
{
  float prev_sample = d_prev_sample;
  d_prev_sample = new_sample;
  bool trig;

  switch (d_trigger_mode){

  case gr_TRIG_AUTO:		// always trigger
    return +1;
    
  case gr_TRIG_POS_SLOPE:
    trig = prev_sample < d_trigger_level && new_sample >= d_trigger_level;
    if (trig){
      if (fabs (prev_sample - d_trigger_level) < fabs (new_sample - d_trigger_level))
	return -1;
      else
	return +1;
    }
    return 0;

  case gr_TRIG_NEG_SLOPE:
    trig = prev_sample > d_trigger_level && new_sample <= d_trigger_level;
    if (trig){
      if (fabs (prev_sample - d_trigger_level) < fabs (new_sample - d_trigger_level))
	return -1;
      else
	return +1;
    }
    return 0;

  default:
    assert (0);
    return 0;
  }
}

// ----------------------------------------------------------------
// write output records (duh!)

void
gr_oscope_guts::write_output_records ()
{
  // if the output queue if full, drop the data on the ground.
  if (d_msgq->full_p())
    return;

  // Build a message to hold the output records
  gr_message_sptr msg = 
    gr_make_message(0,		                                       // msg type
		    d_nchannels,	                               // arg1 for other side
		    OUTPUT_RECORD_SIZE,                                // arg2 for other side
		    d_nchannels * OUTPUT_RECORD_SIZE * sizeof(float)); // sizeof payload

  float *out = (float *)msg->msg();	// get pointer to raw message buffer

  for (int ch = 0; ch < d_nchannels; ch++){
    // note that d_obi + 1 points at the oldest sample in the buffer
    for (int i = 0; i < OUTPUT_RECORD_SIZE; i++)
      out[i] = d_buffer[ch][wrap_bi(d_obi + 1 + i)];

    out += OUTPUT_RECORD_SIZE;
  }

  d_msgq->handle(msg);		// send the msg
}

// ----------------------------------------------------------------

bool
gr_oscope_guts::set_update_rate (double update_rate)
{
  d_update_rate = std::min (std::max (1./10., update_rate), d_sample_rate);
  update_rate_or_decimation_changed ();
  return true;
}

bool
gr_oscope_guts::set_decimation_count (int decimator_count)
{
  decimator_count = std::max (1, decimator_count);
  d_decimator_count_init = decimator_count;
  update_rate_or_decimation_changed ();
  return true;
}

bool
gr_oscope_guts::set_sample_rate(double sample_rate)
{
  d_sample_rate = sample_rate;
  return set_update_rate(update_rate());
}


void
gr_oscope_guts::update_rate_or_decimation_changed ()
{
  d_hold_off_count_init =
    (int) rint (d_sample_rate / d_update_rate / d_decimator_count_init);
}

bool
gr_oscope_guts::set_trigger_channel (int channel)
{
  if (channel >= 0 && channel < d_nchannels){
    d_trigger_channel = channel;
    trigger_changed ();
    return true;
  }

  return false;
}

bool
gr_oscope_guts::set_trigger_mode (gr_trigger_mode mode)
{
  switch (mode){
  case gr_TRIG_POS_SLOPE:
  case gr_TRIG_NEG_SLOPE:
  case gr_TRIG_AUTO:
    d_trigger_mode = mode;
    trigger_changed ();
    return true;
  }
  return false;
}

bool
gr_oscope_guts::set_trigger_level (double trigger_level)
{
  d_trigger_level = trigger_level;
  trigger_changed ();
  return true;
}

bool
gr_oscope_guts::set_trigger_level_auto ()
{
  // find the level 1/2 way between the min and the max

  float	min_v = d_buffer[d_trigger_channel][0];
  float	max_v = d_buffer[d_trigger_channel][0];

  for (int i = 1; i < OUTPUT_RECORD_SIZE; i++){
    min_v = std::min (min_v, d_buffer[d_trigger_channel][i]);
    max_v = std::max (max_v, d_buffer[d_trigger_channel][i]);
  }

  d_trigger_level = (min_v + max_v) * 0.5;
  trigger_changed ();
  return true;
}

void
gr_oscope_guts::trigger_changed ()
{
  // d_prev_sample = d_buffer[d_trigger_channel][decr_bi(d_obi)];
  enter_look_for_trigger ();
}

// ACCESSORS

int
gr_oscope_guts::num_channels () const
{
  return d_nchannels;
}

double
gr_oscope_guts::sample_rate () const
{
  return d_sample_rate;
}

double
gr_oscope_guts::update_rate () const
{
  return d_update_rate;
}

int
gr_oscope_guts::get_decimation_count () const
{
  return d_decimator_count_init;
}

int
gr_oscope_guts::get_trigger_channel () const
{
  return d_trigger_channel;
}

gr_trigger_mode
gr_oscope_guts::get_trigger_mode () const
{
  return d_trigger_mode;
}

double
gr_oscope_guts::get_trigger_level () const
{
  return d_trigger_level;
}

int
gr_oscope_guts::get_samples_per_output_record () const
{
  return OUTPUT_RECORD_SIZE;
}
