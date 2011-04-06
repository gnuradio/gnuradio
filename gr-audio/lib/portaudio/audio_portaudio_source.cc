/* -*- c++ -*- */
/*
 * Copyright 2006-2011 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in he hope that it will be useful,
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
#include <audio_portaudio_source.h>
#include <gr_io_signature.h>
#include <gr_prefs.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <gri_portaudio.h>
#include <string.h>

AUDIO_REGISTER_SOURCE(REG_PRIO_MED, portaudio)(
    int sampling_rate, const std::string &device_name, bool ok_to_block
){
    return audio_source::sptr(new audio_portaudio_source(sampling_rate, device_name, ok_to_block));
}

//#define	LOGGING 0		// define to 0 or 1

#define SAMPLE_FORMAT 		paFloat32
typedef float sample_t;

// Number of portaudio buffers in the ringbuffer
static const unsigned int N_BUFFERS = 4;

static std::string 
default_device_name ()
{
  return gr_prefs::singleton()->get_string("audio_portaudio", "default_input_device", "");
}

void
audio_portaudio_source::create_ringbuffer(void)
{
  int bufsize_samples = d_portaudio_buffer_size_frames * d_input_parameters.channelCount;
  
  if (d_verbose)
    fprintf(stderr, "ring buffer size  = %d frames\n",
	    N_BUFFERS*bufsize_samples/d_input_parameters.channelCount);

  // FYI, the buffer indicies are in units of samples.
  d_writer = gr_make_buffer(N_BUFFERS * bufsize_samples, sizeof(sample_t));
  d_reader = gr_buffer_add_reader(d_writer, 0);
}

/*
 * This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 *
 * Our job is to copy framesPerBuffer frames from inputBuffer.
 */
int
portaudio_source_callback (const void *inputBuffer,
			   void *outputBuffer,
			   unsigned long framesPerBuffer,
			   const PaStreamCallbackTimeInfo* timeInfo,
			   PaStreamCallbackFlags statusFlags,
			   void *arg)
{
  audio_portaudio_source *self = (audio_portaudio_source *)arg;
  int nchan = self->d_input_parameters.channelCount;
  int nframes_to_copy = framesPerBuffer;
  int nframes_room = self->d_writer->space_available() / nchan;

  if (nframes_to_copy <= nframes_room){  // We've got room for the data ..
    //if (LOGGING)
    //  self->d_log->printf("PAsrc  cb: f/b = %4ld\n", framesPerBuffer);

    // copy from input buffer to ringbuffer
    {
      gruel::scoped_lock(d_ringbuffer_mutex);

      memcpy(self->d_writer->write_pointer(),
	     inputBuffer,
	     nframes_to_copy * nchan * sizeof(sample_t));
      self->d_writer->update_write_pointer(nframes_to_copy * nchan);
	 
      // Tell the source thread there is new data in the ringbuffer.
      self->d_ringbuffer_ready = true;
    }

    self->d_ringbuffer_cond.notify_one();
    return paContinue;
  }

  else {			// overrun
    self->d_noverruns++;
    ssize_t r = ::write(2, "aO", 2);	// FIXME change to non-blocking call
    if(r == -1) {
      perror("audio_portaudio_source::portaudio_source_callback write error to stderr.");
    }

    self->d_ringbuffer_ready = false;
    self->d_ringbuffer_cond.notify_one();  // Tell the sink to get going!
    return paContinue;
  }
}


// ----------------------------------------------------------------

audio_portaudio_source::audio_portaudio_source(int sampling_rate,
					       const std::string device_name,
					       bool ok_to_block)
  : gr_sync_block ("audio_portaudio_source",
		   gr_make_io_signature(0, 0, 0),
		   gr_make_io_signature(0, 0, 0)),
    d_sampling_rate(sampling_rate),
    d_device_name(device_name.empty() ? default_device_name() : device_name),
    d_ok_to_block(ok_to_block),
    d_verbose(gr_prefs::singleton()->get_bool("audio_portaudio", "verbose", false)),
    d_portaudio_buffer_size_frames(0),
    d_stream(0),
    d_ringbuffer_mutex(),
    d_ringbuffer_cond(),
    d_ringbuffer_ready(false),
    d_noverruns(0)
{
  memset(&d_input_parameters, 0, sizeof(d_input_parameters));
  //if (LOGGING)
  //  d_log = gri_logger::singleton();

  PaError 	      err;
  int     	      i, numDevices;
  PaDeviceIndex	      device = 0;
  const PaDeviceInfo *deviceInfo = NULL;


  err = Pa_Initialize();
  if (err != paNoError) {
    bail ("Initialize failed", err);
  }

  if (d_verbose)
    gri_print_devices();

  numDevices = Pa_GetDeviceCount();
  if (numDevices < 0)
    bail("Pa Device count failed", 0);
  if (numDevices == 0)
    bail("no devices available", 0);

  if (d_device_name.empty()) 
  {
    // FIXME Get smarter about picking something
    device = Pa_GetDefaultInputDevice();
    deviceInfo = Pa_GetDeviceInfo(device);
    fprintf(stderr,"%s is the chosen device using %s as the host\n",
	    deviceInfo->name, Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
  }
  else
  {
    bool found = false;
    
    for (i=0;i<numDevices;i++) {
      deviceInfo = Pa_GetDeviceInfo( i );
      fprintf(stderr,"Testing device name: %s",deviceInfo->name);
      if (deviceInfo->maxInputChannels <= 0) {
	fprintf(stderr,"\n");
	continue;
      }
      if (strstr(deviceInfo->name, d_device_name.c_str())){
	fprintf(stderr,"  Chosen!\n");
	device = i;
	fprintf(stderr,"%s using %s as the host\n",d_device_name.c_str(),
		Pa_GetHostApiInfo(deviceInfo->hostApi)->name), fflush(stderr);
	found = true;
	deviceInfo = Pa_GetDeviceInfo(device);
	i = numDevices; 	// force loop exit
      }
      else
	fprintf(stderr,"\n"),fflush(stderr);
    }

    if (!found){
      bail("Failed to find specified device name", 0);
    }
  }


  d_input_parameters.device = device;
  d_input_parameters.channelCount     = deviceInfo->maxInputChannels;
  d_input_parameters.sampleFormat     = SAMPLE_FORMAT;
  d_input_parameters.suggestedLatency = deviceInfo->defaultLowInputLatency;
  d_input_parameters.hostApiSpecificStreamInfo = NULL;

  // We fill in the real channelCount in check_topology when we know
  // how many inputs are connected to us.

  // Now that we know the maximum number of channels (allegedly)
  // supported by the h/w, we can compute a reasonable output
  // signature.  The portaudio specs say that they'll accept any
  // number of channels from 1 to max.
  set_output_signature(gr_make_io_signature(1, deviceInfo->maxInputChannels,
					    sizeof (sample_t)));
}


bool
audio_portaudio_source::check_topology (int ninputs, int noutputs)
{
  PaError err;

  if (Pa_IsStreamActive(d_stream))
  {
      Pa_CloseStream(d_stream);
      d_stream = 0;
      d_reader.reset();		// boost::shared_ptr for d_reader = 0
      d_writer.reset();		// boost::shared_ptr for d_write = 0
  }

  d_input_parameters.channelCount = noutputs;	// # of channels we're really using

#if 1
  d_portaudio_buffer_size_frames = (int)(0.0213333333  * d_sampling_rate + 0.5);  // Force 512 frame buffers at 48000
  fprintf(stderr, "Latency = %8.5f, requested sampling_rate = %g\n", // Force latency to 21.3333333.. ms
	  0.0213333333, (double)d_sampling_rate);
#endif
  err = Pa_OpenStream(&d_stream,
		      &d_input_parameters,
		      NULL,			// No output
		      d_sampling_rate,
		      d_portaudio_buffer_size_frames,
		      paClipOff,
		      &portaudio_source_callback,
		      (void*)this);

  if (err != paNoError) {
    output_error_msg ("OpenStream failed", err);
    return false;
  }

#if 0  
  const PaStreamInfo *psi = Pa_GetStreamInfo(d_stream);

  d_portaudio_buffer_size_frames = (int)(d_input_parameters.suggestedLatency  * psi->sampleRate);
  fprintf(stderr, "Latency = %7.4f, psi->sampleRate = %g\n",
	  d_input_parameters.suggestedLatency, psi->sampleRate);
#endif

  fprintf(stderr, "d_portaudio_buffer_size_frames = %d\n", d_portaudio_buffer_size_frames);

  assert(d_portaudio_buffer_size_frames != 0);

  create_ringbuffer();

  err = Pa_StartStream(d_stream);
  if (err != paNoError) {
    output_error_msg ("StartStream failed", err);
    return false;
  }

  return true;
}

audio_portaudio_source::~audio_portaudio_source ()
{
  Pa_StopStream(d_stream);	// wait for output to drain
  Pa_CloseStream(d_stream);
  Pa_Terminate();
}

int
audio_portaudio_source::work (int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
{
  float **out = (float **) &output_items[0];
  const unsigned nchan = d_input_parameters.channelCount; // # of channels == samples/frame

  int k;
  for (k = 0; k < noutput_items; ){

    int nframes = d_reader->items_available() / nchan;	// # of frames in ringbuffer
    if (nframes == 0){		// no data right now...
      if (k > 0)      		// If we've produced anything so far, return that
	return k;

      if (d_ok_to_block) {
	gruel:: scoped_lock guard(d_ringbuffer_mutex);
	while (d_ringbuffer_ready == false)
	  d_ringbuffer_cond.wait(guard);	// block here, then try again
	continue;
      }
      
      assert(k == 0);

      // There's no data and we're not allowed to block.
      // (A USRP is most likely controlling the pacing through the pipeline.)
      // This is an underun.  The scheduler wouldn't have called us if it
      // had anything better to do.  Thus we really need to produce some amount
      // of "fill".
      //
      // There are lots of options for comfort noise, etc.
      // FIXME We'll fill with zeros for now.  Yes, it will "click"...

      // Fill with some frames of zeros
      {
	gruel::scoped_lock guard(d_ringbuffer_mutex);

	int nf = std::min(noutput_items - k, (int) d_portaudio_buffer_size_frames);
	for (int i = 0; i < nf; i++){
	  for (unsigned int c = 0; c < nchan; c++){
	    out[c][k + i] = 0;
	  }
	}
	k += nf;

	d_ringbuffer_ready = false;
	return k;
      }
    }

    // We can read the smaller of the request and what's in the buffer.
    {
      gruel::scoped_lock guard(d_ringbuffer_mutex);

      int nf = std::min(noutput_items - k, nframes);
      
      const float *p = (const float *) d_reader->read_pointer();
      for (int i = 0; i < nf; i++){
	for (unsigned int c = 0; c < nchan; c++){
	  out[c][k + i] = *p++;
	}
      }
      d_reader->update_read_pointer(nf * nchan);
      k += nf;
      d_ringbuffer_ready = false;
    }
  }

  return k;  // tell how many we actually did
}

void
audio_portaudio_source::output_error_msg (const char *msg, int err)
{
  fprintf (stderr, "audio_portaudio_source[%s]: %s: %s\n",
	   d_device_name.c_str (), msg, Pa_GetErrorText(err));
}

void
audio_portaudio_source::bail (const char *msg, int err) throw (std::runtime_error)
{
  output_error_msg (msg, err);
  throw std::runtime_error ("audio_portaudio_source");
}
