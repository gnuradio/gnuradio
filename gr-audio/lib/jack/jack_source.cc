/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2010,2013-2014 Free Software Foundation, Inc.
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

#include "audio_registry.h"
#include <jack_source.h>
#include <jack_impl.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <stdio.h>
#include <iostream>
#include <stdexcept>

#ifndef NO_PTHREAD
#include <pthread.h>
#endif

namespace gr {
  namespace audio {

    source::sptr
    jack_source_fcn(int sampling_rate,
                    const std::string &device_name,
                    bool ok_to_block)
    {
      return source::sptr
        (new jack_source(sampling_rate, device_name, ok_to_block));
    }

    typedef jack_default_audio_sample_t sample_t;

    // Number of jack buffers in the ringbuffer
    // TODO: make it to match at least the quantity of items passed to work()
    static const unsigned int N_BUFFERS = 16;

    static std::string
    default_device_name()
    {
      return prefs::singleton()->get_string
        ("audio_jack", "default_input_device", "gr_source");
    }

    int
    jack_source_process(jack_nframes_t nframes, void *arg)
    {
      jack_source *self = (jack_source *)arg;
      unsigned int write_size = nframes*sizeof(sample_t);

      for(int i = 0; i < self->d_portcount; i++) {
        if(jack_ringbuffer_write_space (self->d_ringbuffer[i]) < write_size) {
          self->d_noverruns++;
          // FIXME: move this fputs out, we shouldn't use blocking calls in process()
          fputs ("jO", stderr);
          return 0;
        }

        char *buffer = (char *)jack_port_get_buffer(self->d_jack_input_port[i], nframes);

        jack_ringbuffer_write (self->d_ringbuffer[i], buffer, write_size);
      }

#ifndef NO_PTHREAD
      // Tell the source thread there is data in the ringbuffer.
      // If it is already running, the lock will not be available.
      // We can't wait here in the process() thread, but we don't
      // need to signal in that case, because the source thread will
      // check for data availability.

      if(pthread_mutex_trylock(&self->d_jack_process_lock) == 0) {
        pthread_cond_signal(&self->d_ringbuffer_ready);
        pthread_mutex_unlock(&self->d_jack_process_lock);
      }
#endif

      return 0;
    }

    // ----------------------------------------------------------------

    jack_source::jack_source(int sampling_rate,
                             const std::string device_name,
                             bool ok_to_block)
      : sync_block("audio_jack_source",
                      io_signature::make(0, 0, 0),
                      io_signature::make(0, 0, 0)),
        d_sampling_rate(sampling_rate),
        d_device_name(device_name.empty() ? default_device_name() : device_name),
        d_ok_to_block(ok_to_block),
        d_jack_client(0),
        d_portcount(0),
        d_jack_input_port(),
        d_ringbuffer(),
        d_noverruns(0)
    {
#ifndef NO_PTHREAD
      pthread_cond_init(&d_ringbuffer_ready, NULL);;
      pthread_mutex_init(&d_jack_process_lock, NULL);
#endif

      // try to become a client of the JACK server
      jack_options_t options = JackNullOption;
      jack_status_t status;
      const char *server_name = NULL;
      if((d_jack_client = jack_client_open(d_device_name.c_str(),
                                           options, &status,
                                           server_name)) == NULL) {
        GR_LOG_ERROR(d_logger, boost::format("[%1%]: jack server not running?") \
                     % d_device_name);
        throw std::runtime_error("audio_jack_source");
      }

      // tell the JACK server to call `jack_source_process()' whenever
      // there is work to be done.
      jack_set_process_callback(d_jack_client, &jack_source_process, (void*)this);

      // tell the JACK server to call `jack_shutdown()' if
      // it ever shuts down, either entirely, or if it
      // just decides to stop calling us.

      //jack_on_shutdown (d_jack_client, &jack_shutdown, (void*)this);

      d_jack_buffer_size = jack_get_buffer_size(d_jack_client);

      set_output_multiple(d_jack_buffer_size);

      assert(sizeof(float)==sizeof(sample_t));
      set_output_signature(io_signature::make(1, MAX_PORTS, sizeof(sample_t)));

      jack_nframes_t sample_rate = jack_get_sample_rate(d_jack_client);

      if((jack_nframes_t)sampling_rate != sample_rate) {
        GR_LOG_INFO(d_logger, boost::format("[%1%]: unable to support sampling rate %2%\n\tCard requested %3% instead.") \
                     % d_device_name % sampling_rate % d_sampling_rate);
      }
    }

    bool
    jack_source::check_topology(int ninputs, int noutputs)
    {

      if(noutputs > MAX_PORTS)
        return false;

      d_portcount = noutputs;  // # of channels we're really using

      // Create ports and ringbuffers
      for(int i = 0; i < d_portcount; i++) {
        std::string portname("in" + boost::to_string(i));

        d_jack_input_port[i] = jack_port_register(d_jack_client, portname.c_str(),
          JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

        d_ringbuffer[i] = jack_ringbuffer_create(N_BUFFERS*d_jack_buffer_size*sizeof(sample_t));
        if(d_ringbuffer[i] == NULL)
          bail("jack_ringbuffer_create failed", 0);
      }

      // tell the JACK server that we are ready to roll
      if(jack_activate (d_jack_client))
        throw std::runtime_error("audio_jack_source");

      return true;
    }

    jack_source::~jack_source()
    {
      jack_client_close(d_jack_client);


      for(int i = 0; i < d_portcount; i++)
        jack_ringbuffer_free(d_ringbuffer[i]);
    }

    int
    jack_source::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {

      const float **out = (const float **)&output_items[0];

      // Minimize latency
      noutput_items = std::min (noutput_items, (int)d_jack_buffer_size);

      for(int i = 0; i < d_portcount; i++) {

        int k = 0;

        // read_size and work_size are in bytes
        unsigned int read_size;

        int work_size = noutput_items*sizeof(sample_t);

        while(work_size > 0) {
          unsigned int read_space;    // bytes

#ifdef NO_PTHREAD
          while((read_space=jack_ringbuffer_read_space (d_ringbuffer[i])) <
                d_jack_buffer_size*sizeof(sample_t)) {
            usleep(1000000*((d_jack_buffer_size-read_space/sizeof(sample_t))/d_sampling_rate));
          }
#else

          // JACK actually requires POSIX
          pthread_mutex_lock(&d_jack_process_lock);
          while((read_space = jack_ringbuffer_read_space(d_ringbuffer[i])) <
                d_jack_buffer_size*sizeof(sample_t)) {
            // wait until jack_source_process() signals more data
            pthread_cond_wait(&d_ringbuffer_ready, &d_jack_process_lock);
          }
          pthread_mutex_unlock(&d_jack_process_lock);
#endif

          read_space -= read_space%(d_jack_buffer_size*sizeof(sample_t));
          read_size = std::min(read_space, (unsigned int)work_size);

          if(jack_ringbuffer_read(d_ringbuffer[i], (char *) &(out[i][k]),
                                  read_size) < read_size) {
            bail("jack_ringbuffer_read failed", 0);
          }
          work_size -= read_size;
          k += read_size/sizeof(sample_t);
        }
      }

      return noutput_items;
    }

    void
    jack_source::output_error_msg(const char *msg, int err)
    {
      GR_LOG_ERROR(d_logger, boost::format("[%1%]: %2%: %3%") \
                   % d_device_name % msg % err);
    }

    void
    jack_source::bail(const char *msg, int err) throw (std::runtime_error)
    {
      output_error_msg(msg, err);
      throw std::runtime_error("audio_jack_source");
    }

  } /* namespace audio */
} /* namespace gr */
