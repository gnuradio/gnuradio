/* -*- c++ -*- */
/*
 * Copyright 2005-2011 Free Software Foundation, Inc.
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

#include "gr_audio_registry.h"
#include <audio_jack_sink.h>
#include <gr_io_signature.h>
#include <gr_prefs.h>
#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <gri_jack.h>

#ifndef NO_PTHREAD
#include <pthread.h>
#endif

AUDIO_REGISTER_SINK(REG_PRIO_MED, jack)(
    int sampling_rate, const std::string &device_name, bool ok_to_block
){
    return audio_sink::sptr(new audio_jack_sink(sampling_rate, device_name, ok_to_block));
}

typedef jack_default_audio_sample_t sample_t;


// Number of jack buffers in the ringbuffer
// TODO: make it to match at least the quantity of items passed by work()
static const unsigned int N_BUFFERS = 16;

static std::string 
default_device_name ()
{
  return gr_prefs::singleton()->get_string("audio_jack", "default_output_device", "gr_sink");
}

int
jack_sink_process (jack_nframes_t nframes, void *arg)
{
  audio_jack_sink *self = (audio_jack_sink *)arg;
  unsigned int read_size = nframes*sizeof(sample_t);

  if (jack_ringbuffer_read_space (self->d_ringbuffer) < read_size) {
    self->d_nunderuns++;
    // FIXME: move this fputs out, we shouldn't use blocking calls in process()
    fputs ("jU", stderr);
    return 0;
  }

  char *buffer = (char *) jack_port_get_buffer (self->d_jack_output_port, nframes);

  jack_ringbuffer_read (self->d_ringbuffer, buffer, read_size);

#ifndef NO_PTHREAD
  // Tell the sink thread there is room in the ringbuffer.
  // If it is already running, the lock will not be available.
  // We can't wait here in the process() thread, but we don't
  // need to signal in that case, because the sink thread will 
  // check for room availability.

  if (pthread_mutex_trylock (&self->d_jack_process_lock) == 0) {
    pthread_cond_signal (&self->d_ringbuffer_ready);
    pthread_mutex_unlock (&self->d_jack_process_lock);
  }
#endif

  return 0;
}

// ----------------------------------------------------------------

audio_jack_sink::audio_jack_sink (int sampling_rate,
				  const std::string device_name,
				  bool ok_to_block)
  : gr_sync_block ("audio_jack_sink",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature (0, 0, 0)),
    d_sampling_rate (sampling_rate),
    d_device_name (device_name.empty() ? default_device_name() : device_name),
    d_ok_to_block (ok_to_block),
    d_jack_client (0),
    d_ringbuffer (0),
    d_nunderuns (0)
{
#ifndef NO_PTHREAD
  pthread_cond_init(&d_ringbuffer_ready, NULL);;
  pthread_mutex_init(&d_jack_process_lock, NULL);
#endif
  
  // try to become a client of the JACK server
  jack_options_t options = JackNullOption;
  jack_status_t status;
  const char *server_name = NULL;
  if ((d_jack_client = jack_client_open (d_device_name.c_str (),
  					 options, &status,
					 server_name)) == NULL) {
    fprintf (stderr, "audio_jack_sink[%s]: jack server not running?\n",
	     d_device_name.c_str());
    throw std::runtime_error ("audio_jack_sink");
  }

  // tell the JACK server to call `jack_sink_process()' whenever
  // there is work to be done.
  jack_set_process_callback (d_jack_client, &jack_sink_process, (void*)this);

  // tell the JACK server to call `jack_shutdown()' if
  // it ever shuts down, either entirely, or if it
  // just decides to stop calling us.

  //jack_on_shutdown (d_jack_client, &jack_shutdown, (void*)this);
 
  d_jack_output_port = 
    jack_port_register (d_jack_client, "out", 
			JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);


  d_jack_buffer_size = jack_get_buffer_size (d_jack_client);

  set_output_multiple (d_jack_buffer_size);

  d_ringbuffer =
    jack_ringbuffer_create (N_BUFFERS*d_jack_buffer_size*sizeof(sample_t));
  if (d_ringbuffer == NULL)
    bail ("jack_ringbuffer_create failed", 0);

  assert(sizeof(float)==sizeof(sample_t));
  set_input_signature (gr_make_io_signature (1, 1, sizeof (sample_t)));


  jack_nframes_t sample_rate = jack_get_sample_rate (d_jack_client);

  if ((jack_nframes_t)sampling_rate != sample_rate){
    fprintf (stderr, "audio_jack_sink[%s]: unable to support sampling rate %d\n",
	     d_device_name.c_str (), sampling_rate);
    fprintf (stderr, "  card requested %d instead.\n", sample_rate);
  }
}


bool
audio_jack_sink::check_topology (int ninputs, int noutputs)
{
  if (ninputs != 1)
    return false;

  // tell the JACK server that we are ready to roll 
  if (jack_activate (d_jack_client))
    throw std::runtime_error ("audio_jack_sink");

  return true;
}

audio_jack_sink::~audio_jack_sink ()
{
  jack_client_close (d_jack_client);
  jack_ringbuffer_free (d_ringbuffer);
}

int
audio_jack_sink::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  // write_size and work_size are in bytes
  int work_size = noutput_items*sizeof(sample_t);
  unsigned int write_size;

  while (work_size > 0) {
    unsigned int write_space;	// bytes

#ifdef NO_PTHREAD
    while ((write_space=jack_ringbuffer_write_space (d_ringbuffer)) < 
	   d_jack_buffer_size*sizeof(sample_t)) {
      usleep(1000000*((d_jack_buffer_size-write_space/sizeof(sample_t))/d_sampling_rate));
    }
#else
    // JACK actually requires POSIX

    pthread_mutex_lock (&d_jack_process_lock);
    while ((write_space=jack_ringbuffer_write_space (d_ringbuffer)) < 
	   d_jack_buffer_size*sizeof(sample_t)) {

      // wait until jack_sink_process() signals more room
      pthread_cond_wait (&d_ringbuffer_ready, &d_jack_process_lock);
    }
    pthread_mutex_unlock (&d_jack_process_lock);
#endif

    write_space -= write_space%(d_jack_buffer_size*sizeof(sample_t));
    write_size = std::min(write_space, (unsigned int)work_size);

    if (jack_ringbuffer_write (d_ringbuffer, (char *) input_items[0],
			       write_size) < write_size) {
      bail ("jack_ringbuffer_write failed", 0);
    }
    work_size -= write_size;
  }

  return noutput_items;
}

void
audio_jack_sink::output_error_msg (const char *msg, int err)
{
  fprintf (stderr, "audio_jack_sink[%s]: %s: %d\n",
	   d_device_name.c_str (), msg,  err);
}

void
audio_jack_sink::bail (const char *msg, int err) throw (std::runtime_error)
{
  output_error_msg (msg, err);
  throw std::runtime_error ("audio_jack_sink");
}
