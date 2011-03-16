/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2010 Free Software Foundation, Inc.
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
#include <audio_jack_source.h>
#include <gr_io_signature.h>
#include <gr_prefs.h>
#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <gri_jack.h>

#ifndef NO_PTHREAD
#include <pthread.h>
#endif

AUDIO_REGISTER_SOURCE(REG_PRIO_MED, jack)(
    int sampling_rate, const std::string &device_name, bool ok_to_block
){
    return audio_source::sptr(new audio_jack_source(sampling_rate, device_name, ok_to_block));
}

typedef jack_default_audio_sample_t sample_t;


// Number of jack buffers in the ringbuffer
// TODO: make it to match at least the quantity of items passed to work()
static const unsigned int N_BUFFERS = 16;

static std::string 
default_device_name ()
{
  return gr_prefs::singleton()->get_string("audio_jack", "default_input_device", "gr_source");
}


int
jack_source_process (jack_nframes_t nframes, void *arg)
{
  audio_jack_source *self = (audio_jack_source *)arg;
  unsigned int write_size = nframes*sizeof(sample_t);

  if (jack_ringbuffer_write_space (self->d_ringbuffer) < write_size) {
    self->d_noverruns++;
    // FIXME: move this fputs out, we shouldn't use blocking calls in process()
    fputs ("jO", stderr);
    return 0;
  }

  char *buffer = (char *) jack_port_get_buffer (self->d_jack_input_port, nframes);

  jack_ringbuffer_write (self->d_ringbuffer, buffer, write_size);

#ifndef NO_PTHREAD
  // Tell the source thread there is data in the ringbuffer.
  // If it is already running, the lock will not be available.
  // We can't wait here in the process() thread, but we don't
  // need to signal in that case, because the source thread will 
  // check for data availability.

  if (pthread_mutex_trylock (&self->d_jack_process_lock) == 0) {
    pthread_cond_signal (&self->d_ringbuffer_ready);
    pthread_mutex_unlock (&self->d_jack_process_lock);
  }
#endif

  return 0;
}

// ----------------------------------------------------------------

audio_jack_source::audio_jack_source (int sampling_rate,
				      const std::string device_name,
				      bool ok_to_block)
  : gr_sync_block ("audio_jack_source",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature (0, 0, 0)),
    d_sampling_rate (sampling_rate),
    d_device_name (device_name.empty() ? default_device_name() : device_name),
    d_ok_to_block(ok_to_block),
    d_jack_client (0),
    d_ringbuffer (0),
    d_noverruns (0)
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
    fprintf (stderr, "audio_jack_source[%s]: jack server not running?\n",
	     d_device_name.c_str());
    throw std::runtime_error ("audio_jack_source");
  }

  // tell the JACK server to call `jack_source_process()' whenever
  // there is work to be done.
  jack_set_process_callback (d_jack_client, &jack_source_process, (void*)this);

  // tell the JACK server to call `jack_shutdown()' if
  // it ever shuts down, either entirely, or if it
  // just decides to stop calling us.

  //jack_on_shutdown (d_jack_client, &jack_shutdown, (void*)this);
 
  d_jack_input_port = jack_port_register (d_jack_client, "in", 
					  JACK_DEFAULT_AUDIO_TYPE,
					  JackPortIsInput, 0);


  d_jack_buffer_size = jack_get_buffer_size (d_jack_client);

  set_output_multiple (d_jack_buffer_size);

  d_ringbuffer = jack_ringbuffer_create (N_BUFFERS*d_jack_buffer_size*sizeof(sample_t));
  if (d_ringbuffer == NULL)
    bail ("jack_ringbuffer_create failed", 0);

  assert(sizeof(float)==sizeof(sample_t));
  set_output_signature (gr_make_io_signature (1, 1, sizeof (sample_t)));


  jack_nframes_t sample_rate = jack_get_sample_rate (d_jack_client);

  if ((jack_nframes_t)sampling_rate != sample_rate){
    fprintf (stderr, "audio_jack_source[%s]: unable to support sampling rate %d\n",
	     d_device_name.c_str (), sampling_rate);
    fprintf (stderr, "  card requested %d instead.\n", sample_rate);
  }
}


bool
audio_jack_source::check_topology (int ninputs, int noutputs)
{
  // tell the JACK server that we are ready to roll 
  if (jack_activate (d_jack_client))
    throw std::runtime_error ("audio_jack_source");

  return true;
}

audio_jack_source::~audio_jack_source ()
{
  jack_client_close (d_jack_client);
  jack_ringbuffer_free (d_ringbuffer);
}

int
audio_jack_source::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  // read_size and work_size are in bytes
  unsigned int read_size;

  // Minimize latency
  noutput_items = std::min (noutput_items, (int)d_jack_buffer_size);

  int work_size = noutput_items*sizeof(sample_t);

  while (work_size > 0) {
    unsigned int read_space;	// bytes

#ifdef NO_PTHREAD
    while ((read_space=jack_ringbuffer_read_space (d_ringbuffer)) < 
	   d_jack_buffer_size*sizeof(sample_t)) {
      usleep(1000000*((d_jack_buffer_size-read_space/sizeof(sample_t))/d_sampling_rate));
    }
#else
    // JACK actually requires POSIX

    pthread_mutex_lock (&d_jack_process_lock);
    while ((read_space=jack_ringbuffer_read_space (d_ringbuffer)) < 
		    d_jack_buffer_size*sizeof(sample_t)) {

      // wait until jack_source_process() signals more data
      pthread_cond_wait (&d_ringbuffer_ready, &d_jack_process_lock);
    }
    pthread_mutex_unlock (&d_jack_process_lock);
#endif

    read_space -= read_space%(d_jack_buffer_size*sizeof(sample_t));
    read_size = std::min(read_space, (unsigned int)work_size);

    if (jack_ringbuffer_read (d_ringbuffer, (char *) output_items[0],
			      read_size) < read_size) {
      bail ("jack_ringbuffer_read failed", 0);
    }
    work_size -= read_size;
  }

  return noutput_items;
}

void
audio_jack_source::output_error_msg (const char *msg, int err)
{
  fprintf (stderr, "audio_jack_source[%s]: %s: %d\n",
	   d_device_name.c_str (), msg,  err);
}

void
audio_jack_source::bail (const char *msg, int err) throw (std::runtime_error)
{
  output_error_msg (msg, err);
  throw std::runtime_error ("audio_jack_source");
}
