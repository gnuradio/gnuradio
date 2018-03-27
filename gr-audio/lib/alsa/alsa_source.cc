/* -*- c++ -*- */
/*
 * Copyright 2004-2011,2013 Free Software Foundation, Inc.
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
#include <alsa_source.h>
#include <alsa_impl.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <stdio.h>
#include <iostream>
#include <stdexcept>

namespace gr {
  namespace audio {

    source::sptr
    alsa_source_fcn(int sampling_rate,
                    const std::string &device_name,
                    bool ok_to_block)
    {
      return source::sptr
        (new alsa_source(sampling_rate, device_name, ok_to_block));
    }

    static bool CHATTY_DEBUG = false;

    static snd_pcm_format_t acceptable_formats[] = {
      // these are in our preferred order...
      SND_PCM_FORMAT_S32,
      SND_PCM_FORMAT_S16
    };

#define NELEMS(x) (sizeof(x)/sizeof(x[0]))

    static std::string
    default_device_name()
    {
      return prefs::singleton()->get_string("audio_alsa",
                                               "default_input_device",
                                               "default");
    }

    static double
    default_period_time()
    {
      return std::max(0.001,
                      prefs::singleton()->get_double("audio_alsa", "period_time", 0.010));
    }

    static int
    default_nperiods()
    {
      return std::max(2L,
                      prefs::singleton()->get_long("audio_alsa", "nperiods", 4));
    }

    // ----------------------------------------------------------------

    alsa_source::alsa_source(int sampling_rate,
                             const std::string device_name,
                             bool ok_to_block)
      : sync_block("audio_alsa_source",
                      io_signature::make(0, 0, 0),
                      io_signature::make(0, 0, 0)),
        d_sampling_rate(sampling_rate),
        d_device_name(device_name.empty() ? default_device_name() : device_name),
        d_pcm_handle(0),
        d_hw_params((snd_pcm_hw_params_t*)(new char[snd_pcm_hw_params_sizeof()])),
        d_sw_params((snd_pcm_sw_params_t*)(new char[snd_pcm_sw_params_sizeof()])),
        d_nperiods(default_nperiods()),
        d_period_time_us((unsigned int)(default_period_time() * 1e6)),
        d_period_size(0),
        d_buffer_size_bytes(0), d_buffer(0),
        d_worker(0), d_hw_nchan(0),
        d_special_case_stereo_to_mono(false),
        d_noverruns(0), d_nsuspends(0)
    {
      CHATTY_DEBUG = prefs::singleton()->get_bool("audio_alsa", "verbose", false);

      int error;
      int dir;

      // open the device for capture
      error = snd_pcm_open(&d_pcm_handle, d_device_name.c_str(),
                           SND_PCM_STREAM_CAPTURE, 0);
      if(error < 0){
        GR_LOG_ERROR(d_logger, boost::format("[%1%]: %2%") \
                     % (d_device_name) % (snd_strerror(error)));
        throw std::runtime_error("audio_alsa_source");
      }

      // Fill params with a full configuration space for a PCM.
      error = snd_pcm_hw_params_any(d_pcm_handle, d_hw_params);
      if(error < 0)
        bail("broken configuration for playback", error);

      if(CHATTY_DEBUG)
        gri_alsa_dump_hw_params(d_pcm_handle, d_hw_params, stdout);

      // now that we know how many channels the h/w can handle, set output signature
      unsigned int umax_chan;
      unsigned int umin_chan;
      snd_pcm_hw_params_get_channels_min(d_hw_params, &umin_chan);
      snd_pcm_hw_params_get_channels_max(d_hw_params, &umax_chan);
      int min_chan = std::min(umin_chan, 1000U);
      int max_chan = std::min(umax_chan, 1000U);

      // As a special case, if the hw's min_chan is two, we'll accept
      // a single output and handle the demux ourselves.
      if(min_chan == 2) {
        min_chan = 1;
        d_special_case_stereo_to_mono = true;
      }

      set_output_signature(io_signature::make(min_chan, max_chan,
                                                sizeof(float)));

      // fill in portions of the d_hw_params that we know now...

      // Specify the access methods we implement
      // For now, we only handle RW_INTERLEAVED...
      snd_pcm_access_mask_t *access_mask;
      snd_pcm_access_mask_t **access_mask_ptr = &access_mask; // FIXME: workaround for compiler warning
      snd_pcm_access_mask_alloca(access_mask_ptr);
      snd_pcm_access_mask_none(access_mask);
      snd_pcm_access_mask_set(access_mask, SND_PCM_ACCESS_RW_INTERLEAVED);
      // snd_pcm_access_mask_set(access_mask, SND_PCM_ACCESS_RW_NONINTERLEAVED);

      if((error = snd_pcm_hw_params_set_access_mask(d_pcm_handle,
                                                    d_hw_params, access_mask)) < 0)
        bail("failed to set access mask", error);

      // set sample format
      if(!gri_alsa_pick_acceptable_format(d_pcm_handle, d_hw_params,
                                          acceptable_formats,
                                          NELEMS(acceptable_formats),
                                          &d_format,
                                          "audio_alsa_source",
                                          CHATTY_DEBUG))
        throw std::runtime_error("audio_alsa_source");

      // sampling rate
      unsigned int orig_sampling_rate = d_sampling_rate;
      if((error = snd_pcm_hw_params_set_rate_near(d_pcm_handle, d_hw_params,
                                                  &d_sampling_rate, 0)) < 0)
        bail("failed to set rate near", error);

      if(orig_sampling_rate != d_sampling_rate){
        GR_LOG_INFO(d_logger, boost::format("[%1%]: unable to support sampling rate %2%\n\tCard requested %3% instead.") \
                     % snd_pcm_name(d_pcm_handle) % orig_sampling_rate \
                     % d_sampling_rate);
      }

      /*
       * ALSA transfers data in units of "periods".
       * We indirectly determine the underlying buffersize by specifying
       * the number of periods we want (typically 4) and the length of each
       * period in units of time (typically 1ms).
       */
      unsigned int min_nperiods, max_nperiods;
      snd_pcm_hw_params_get_periods_min(d_hw_params, &min_nperiods, &dir);
      snd_pcm_hw_params_get_periods_max(d_hw_params, &max_nperiods, &dir);

      unsigned int orig_nperiods = d_nperiods;
      d_nperiods = std::min(std::max (min_nperiods, d_nperiods), max_nperiods);

      // adjust period time so that total buffering remains more-or-less constant
      d_period_time_us = (d_period_time_us * orig_nperiods) / d_nperiods;

      error = snd_pcm_hw_params_set_periods(d_pcm_handle, d_hw_params,
                                            d_nperiods, 0);
      if(error < 0)
        bail("set_periods failed", error);

      dir = 0;
      error = snd_pcm_hw_params_set_period_time_near(d_pcm_handle, d_hw_params,
                                                     &d_period_time_us, &dir);
      if(error < 0)
        bail("set_period_time_near failed", error);

      dir = 0;
      error = snd_pcm_hw_params_get_period_size(d_hw_params,
                                                &d_period_size, &dir);
      if(error < 0)
        bail("get_period_size failed", error);

      set_output_multiple(d_period_size);
    }

    bool
    alsa_source::check_topology(int ninputs, int noutputs)
    {
      // noutputs is how many channels the user has connected.
      // Now we can finish up setting up the hw params...

      unsigned int nchan = noutputs;
      int err;

      // Check the state of the stream
      // Ensure that the pcm is in a state where we can still mess with the hw_params
      snd_pcm_state_t state;
      state=snd_pcm_state(d_pcm_handle);
      if(state== SND_PCM_STATE_RUNNING)
        return true;  // If stream is running, don't change any parameters
      else if(state == SND_PCM_STATE_XRUN)
        snd_pcm_prepare(d_pcm_handle); // Prepare stream on underrun, and we can set parameters;

      bool special_case = nchan == 1 && d_special_case_stereo_to_mono;
      if(special_case)
        nchan = 2;

      d_hw_nchan = nchan;
      err = snd_pcm_hw_params_set_channels(d_pcm_handle, d_hw_params, d_hw_nchan);
      if(err < 0) {
        output_error_msg("set_channels failed", err);
        return false;
      }

      // set the parameters into the driver...
      err = snd_pcm_hw_params(d_pcm_handle, d_hw_params);
      if(err < 0) {
        output_error_msg("snd_pcm_hw_params failed", err);
        return false;
      }

      d_buffer_size_bytes =
        d_period_size * d_hw_nchan * snd_pcm_format_size(d_format, 1);

      d_buffer = new char[d_buffer_size_bytes];

      if(CHATTY_DEBUG) {
        GR_LOG_DEBUG(d_logger, boost::format("[%1%]: sample resolution = %2% bits") \
                     % snd_pcm_name(d_pcm_handle)                       \
                     % snd_pcm_hw_params_get_sbits(d_hw_params));
      }

      switch(d_format) {
      case SND_PCM_FORMAT_S16:
        if(special_case)
          d_worker = &alsa_source::work_s16_2x1;
        else
          d_worker = &alsa_source::work_s16;
        break;

      case SND_PCM_FORMAT_S32:
        if(special_case)
          d_worker = &alsa_source::work_s32_2x1;
        else
          d_worker = &alsa_source::work_s32;
        break;

      default:
        assert(0);
      }

      return true;
    }

    alsa_source::~alsa_source()
    {
      if(snd_pcm_state(d_pcm_handle) == SND_PCM_STATE_RUNNING)
        snd_pcm_drop(d_pcm_handle);

      snd_pcm_close(d_pcm_handle);
      delete [] ((char*)d_hw_params);
      delete [] ((char*)d_sw_params);
      delete [] d_buffer;
    }

    int
    alsa_source::work(int noutput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
      assert((noutput_items % d_period_size) == 0);
      assert(noutput_items != 0);

      // this is a call through a pointer to a method...
      return (this->*d_worker)(noutput_items, input_items, output_items);
    }

    /*
     * Work function that deals with float to S16 conversion
     */
    int
    alsa_source::work_s16(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
      typedef int16_t sample_t; // the type of samples we're creating
      static const float scale_factor = 1.0 / std::pow(2.0f, 16-1);

      unsigned int nchan = output_items.size ();
      float **out = (float **)&output_items[0];
      sample_t *buf = (sample_t *)d_buffer;
      int bi;

      unsigned int sizeof_frame = d_hw_nchan * sizeof (sample_t);
      assert(d_buffer_size_bytes == d_period_size * sizeof_frame);

      // To minimize latency, return at most a single period's worth of samples.
      // [We could also read the first one in a blocking mode and subsequent
      //  ones in non-blocking mode, but we'll leave that for later (or never).]

      if(!read_buffer(buf, d_period_size, sizeof_frame))
        return -1;		// No fixing this problem.  Say we're done.

      // process one period of data
      bi = 0;
      for(unsigned int i = 0; i < d_period_size; i++) {
        for(unsigned int chan = 0; chan < nchan; chan++) {
          out[chan][i] = (float) buf[bi++] * scale_factor;
        }
      }

      return d_period_size;
    }

    /*
     * Work function that deals with float to S16 conversion
     * and stereo to mono kludge...
     */
    int
    alsa_source::work_s16_2x1(int noutput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      typedef int16_t sample_t; // the type of samples we're creating
      static const float scale_factor = 1.0 / std::pow(2.0f, 16-1);

      float **out = (float**)&output_items[0];
      sample_t *buf = (sample_t*)d_buffer;
      int bi;

      assert(output_items.size () == 1);

      unsigned int sizeof_frame = d_hw_nchan * sizeof(sample_t);
      assert(d_buffer_size_bytes == d_period_size * sizeof_frame);

      // To minimize latency, return at most a single period's worth of samples.
      // [We could also read the first one in a blocking mode and subsequent
      //  ones in non-blocking mode, but we'll leave that for later (or never).]
      if(!read_buffer (buf, d_period_size, sizeof_frame))
        return -1;   // No fixing this problem.  Say we're done.

      // process one period of data
      bi = 0;
      for(unsigned int i = 0; i < d_period_size; i++) {
        int t = (buf[bi] + buf[bi+1]) / 2;
        bi += 2;
        out[0][i] = (float) t * scale_factor;
      }

      return d_period_size;
    }

    /*
     * Work function that deals with float to S32 conversion
     */
    int
    alsa_source::work_s32(int noutput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items)
    {
      typedef int32_t sample_t; // the type of samples we're creating
      static const float scale_factor = 1.0 / std::pow(2.0f, 32-1);

      unsigned int nchan = output_items.size ();
      float **out = (float**)&output_items[0];
      sample_t *buf = (sample_t*)d_buffer;
      int bi;

      unsigned int sizeof_frame = d_hw_nchan * sizeof(sample_t);
      assert(d_buffer_size_bytes == d_period_size * sizeof_frame);

      // To minimize latency, return at most a single period's worth of samples.
      // [We could also read the first one in a blocking mode and subsequent
      //  ones in non-blocking mode, but we'll leave that for later (or never).]

      if(!read_buffer(buf, d_period_size, sizeof_frame))
        return -1;  // No fixing this problem.  Say we're done.

      // process one period of data
      bi = 0;
      for(unsigned int i = 0; i < d_period_size; i++) {
        for(unsigned int chan = 0; chan < nchan; chan++) {
          out[chan][i] = (float) buf[bi++] * scale_factor;
        }
      }

      return d_period_size;
    }

    /*
     * Work function that deals with float to S32 conversion
     * and stereo to mono kludge...
     */
    int
    alsa_source::work_s32_2x1(int noutput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      typedef int32_t sample_t; // the type of samples we're creating
      static const float scale_factor = 1.0 / std::pow(2.0f, 32-1);

      float **out = (float**)&output_items[0];
      sample_t *buf = (sample_t*)d_buffer;
      int bi;

      assert(output_items.size () == 1);

      unsigned int sizeof_frame = d_hw_nchan * sizeof(sample_t);
      assert(d_buffer_size_bytes == d_period_size * sizeof_frame);

      // To minimize latency, return at most a single period's worth of samples.
      // [We could also read the first one in a blocking mode and subsequent
      //  ones in non-blocking mode, but we'll leave that for later (or never).]

      if(!read_buffer(buf, d_period_size, sizeof_frame))
        return -1;  // No fixing this problem.  Say we're done.

      // process one period of data
      bi = 0;
      for(unsigned int i = 0; i < d_period_size; i++) {
        int t = (buf[bi] + buf[bi+1]) / 2;
        bi += 2;
        out[0][i] = (float)t * scale_factor;
      }

      return d_period_size;
    }

    bool
    alsa_source::read_buffer(void *vbuffer, unsigned nframes, unsigned sizeof_frame)
    {
      unsigned char *buffer = (unsigned char*)vbuffer;

      while(nframes > 0) {
        int r = snd_pcm_readi (d_pcm_handle, buffer, nframes);
        if(r == -EAGAIN)
          continue;   // try again

        else if(r == -EPIPE) {  // overrun
          d_noverruns++;
          fputs("aO", stderr);
          if((r = snd_pcm_prepare (d_pcm_handle)) < 0) {
            output_error_msg("snd_pcm_prepare failed. Can't recover from overrun", r);
            return false;
          }
          continue;  // try again
        }
#ifdef ESTRPIPE
        else if(r == -ESTRPIPE) {   // h/w is suspended (whatever that means)
                                    // This is apparently related to power management
          d_nsuspends++;
          if((r = snd_pcm_resume (d_pcm_handle)) < 0) {
            output_error_msg ("failed to resume from suspend", r);
            return false;
          }
          continue;   // try again
        }
#endif
        else if(r < 0) {
          output_error_msg("snd_pcm_readi failed", r);
          return false;
        }

        nframes -= r;
        buffer += r * sizeof_frame;
      }

      return true;
    }

    void
    alsa_source::output_error_msg(const char *msg, int err)
    {
      GR_LOG_ERROR(d_logger, boost::format("[%1%]: %2%: %3%") \
                   % snd_pcm_name(d_pcm_handle) % msg % snd_strerror(err));
    }

    void
    alsa_source::bail(const char *msg, int err) throw (std::runtime_error)
    {
      output_error_msg(msg, err);
      throw std::runtime_error("audio_alsa_source");
    }

  } /* namespace audio */
} /* namespace gr */
