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

/*
 * Bad performance if it's large, and flaky triggering if it's too small
 */
static const int OUTPUT_RECORD_SIZE = 1024;  // Must be power of 2

/*
 * For (slow-updated) STRIPCHART triggering, we make the record size larger, since we
 *   potentially want to be able to "see" hours of data.  This works as long as the
 *   update rates to a STRIPCHART are low, which they generally are--that's rather what
 *   a stripchart is all about!
 */
static const int SCHART_MULT = 8;


static inline int
wrap_bi (int buffer_index, int mx)                // wrap buffer index
{
  return buffer_index & (mx - 1);
}

static inline int
incr_bi (int buffer_index, int mx)                // increment buffer index
{
  return wrap_bi (buffer_index + 1, mx);
}

static inline int
decr_bi (int buffer_index, int mx)                // decrement buffer index
{
  return wrap_bi (buffer_index - 1, mx);
}

gr_oscope_guts::gr_oscope_guts (double sample_rate, gr_msg_queue_sptr msgq)
  : d_nchannels (1),
    d_msgq (msgq), 
    d_trigger_mode (gr_TRIG_MODE_AUTO),
    d_trigger_slope (gr_TRIG_SLOPE_POS),
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
    d_pre_trigger_count (0),
    d_post_trigger_count (0),
    d_post_trigger_count_init (OUTPUT_RECORD_SIZE/2)
{
  for (int i = 0; i < MAX_CHANNELS; i++)
    d_buffer[i] = 0;

  for (int i = 0; i < MAX_CHANNELS; i++){
    d_buffer[i] = new float [OUTPUT_RECORD_SIZE*SCHART_MULT];
    for (int j = 0; j < OUTPUT_RECORD_SIZE*SCHART_MULT; j++)
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
  
  if (d_trigger_mode != gr_TRIG_MODE_STRIPCHART)
  {
	  for (int i = 0; i < d_nchannels; i++)
		d_buffer[i][d_obi] = channel_data[i];                // copy data into buffer

	  switch (d_state){
	  case HOLD_OFF:
		d_hold_off_count--;
		if (d_hold_off_count <= 0)
		  enter_look_for_trigger ();
		break;

	  case LOOK_FOR_TRIGGER:
		if (found_trigger ())
		  enter_post_trigger ();
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

	  d_obi = incr_bi (d_obi, OUTPUT_RECORD_SIZE);
  }
  else
  {
	  for (int i = 0; i < d_nchannels; i++)
	  {
	    for (int j = (OUTPUT_RECORD_SIZE*SCHART_MULT)-1; j > 0; j--)
	    {
			d_buffer[i][j] = d_buffer[i][j-1];
		}
		d_buffer[i][0] = channel_data[i];
	  }
	  d_trigger_off = 0;
	  write_output_records();
  }
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
  d_pre_trigger_count = 0;
  d_state = LOOK_FOR_TRIGGER;
}

void
gr_oscope_guts::enter_post_trigger ()
{
  d_state = POST_TRIGGER;
  d_post_trigger_count = d_post_trigger_count_init;
  //ensure that the trigger offset is no more than than half a sample
  if (d_trigger_off > .5) d_trigger_off -= 1;
  else d_post_trigger_count--;
}

// ----------------------------------------------------------------
// returns true if trigger found

bool
gr_oscope_guts::found_trigger ()
{
	int mx = d_trigger_mode == gr_TRIG_MODE_STRIPCHART ? OUTPUT_RECORD_SIZE*SCHART_MULT :
		OUTPUT_RECORD_SIZE;
		
  float prev_sample = d_buffer[d_trigger_channel][decr_bi(d_obi, mx)];
  float new_sample = d_buffer[d_trigger_channel][d_obi];

  switch (d_trigger_mode){

  case gr_TRIG_MODE_AUTO: //too many samples without a trigger
    d_pre_trigger_count++;
    if (d_pre_trigger_count > OUTPUT_RECORD_SIZE/2) return true;

  case gr_TRIG_MODE_NORM: //look for trigger
    switch (d_trigger_slope){

    case gr_TRIG_SLOPE_POS: //trigger point in pos slope?
      if (new_sample < d_trigger_level || prev_sample >= d_trigger_level) return false;
      break;

    case gr_TRIG_SLOPE_NEG: //trigger point in neg slope?
      if (new_sample > d_trigger_level || prev_sample <= d_trigger_level) return false;
      break;
    }

    //calculate the trigger offset in % sample
    d_trigger_off = (d_trigger_level - prev_sample)/(new_sample - prev_sample);
    return true;

  case gr_TRIG_MODE_FREE: //free run mode, always trigger
    d_trigger_off = 0;
    return true;

  default:
    assert (0);
    return false;
  }
}

// ----------------------------------------------------------------
// write output records (duh!)

void
gr_oscope_guts::write_output_records ()
{
	int mx;
	
	mx = d_trigger_mode == gr_TRIG_MODE_STRIPCHART ? 
		OUTPUT_RECORD_SIZE*SCHART_MULT : OUTPUT_RECORD_SIZE;
		
  // if the output queue if full, drop the data like its hot.
  if (d_msgq->full_p())
    return;
    // Build a message to hold the output records
  gr_message_sptr msg = 
    gr_make_message(0,                                         // msg type
            d_nchannels,                                       // arg1 for other side
            mx,                                // arg2 for other side
            ((d_nchannels * mx) + 1) * sizeof(float)); // sizeof payload

  float *out = (float *)msg->msg();        // get pointer to raw message buffer

  for (int ch = 0; ch < d_nchannels; ch++){
    // note that d_obi + 1 points at the oldest sample in the buffer
    for (int i = 0; i < mx; i++){
      out[i] = d_buffer[ch][wrap_bi(d_obi + 1 + i, mx)];
    }
    out += mx;
  }
  //Set the last sample as the trigger offset:
  //  The non gl scope sink will not look at this last sample.
  //  The gl scope sink will use this last sample as an offset.
  out[0] = d_trigger_off;
  d_msgq->handle(msg);                // send the msg
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
  d_trigger_mode = mode;
  trigger_changed ();
  return true;
}

bool
gr_oscope_guts::set_trigger_slope (gr_trigger_slope slope)
{
  d_trigger_slope = slope;
  trigger_changed ();
  return true;
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

  float min_v = d_buffer[d_trigger_channel][0];
  float max_v = d_buffer[d_trigger_channel][0];

  for (int i = 1; i < OUTPUT_RECORD_SIZE; i++){
    min_v = std::min (min_v, d_buffer[d_trigger_channel][i]);
    max_v = std::max (max_v, d_buffer[d_trigger_channel][i]);
  }
  return set_trigger_level((min_v + max_v) * 0.5);
}

bool
gr_oscope_guts::set_num_channels(int nchannels)
{
  if (nchannels > 0 && nchannels <= MAX_CHANNELS){
    d_nchannels = nchannels;
    return true;
  }
  return false;
}


void
gr_oscope_guts::trigger_changed ()
{
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

gr_trigger_slope
gr_oscope_guts::get_trigger_slope () const
{
  return d_trigger_slope;
}

double
gr_oscope_guts::get_trigger_level () const
{
  return d_trigger_level;
}

int
gr_oscope_guts::get_samples_per_output_record () const
{
	int mx;
	
	mx = OUTPUT_RECORD_SIZE;
	if (d_trigger_mode == gr_TRIG_MODE_STRIPCHART)
	{
		mx = OUTPUT_RECORD_SIZE*SCHART_MULT;
	}
  return mx;
}
