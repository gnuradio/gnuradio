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
#include <audio_portaudio_sink.h>
#include <gr_io_signature.h>
#include <gr_prefs.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <gri_portaudio.h>
#include <string.h>

AUDIO_REGISTER_SINK(REG_PRIO_MED, portaudio)(
    int sampling_rate, const std::string &device_name, bool ok_to_block
){
    return audio_sink::sptr(new audio_portaudio_sink(sampling_rate, device_name, ok_to_block));
}

//#define	LOGGING   0		// define to 0 or 1

#define SAMPLE_FORMAT 		paFloat32
typedef float sample_t;

// Number of portaudio buffers in the ringbuffer
static const unsigned int N_BUFFERS = 4;

static std::string 
default_device_name ()
{
  return gr_prefs::singleton()->get_string("audio_portaudio", "default_output_device", "");
}

void
audio_portaudio_sink::create_ringbuffer(void)
{
  int bufsize_samples = d_portaudio_buffer_size_frames * d_output_parameters.channelCount;
  
  if (d_verbose)
    fprintf(stderr,"ring buffer size  = %d frames\n",
	    N_BUFFERS*bufsize_samples/d_output_parameters.channelCount);

  // FYI, the buffer indicies are in units of samples.
  d_writer = gr_make_buffer(N_BUFFERS * bufsize_samples, sizeof(sample_t));
  d_reader = gr_buffer_add_reader(d_writer, 0);
}

/*
 * This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 *
 * Our job is to write framesPerBuffer frames into outputBuffer.
 */
int
portaudio_sink_callback (const void *inputBuffer,
			 void *outputBuffer,
			 unsigned long framesPerBuffer,
			 const PaStreamCallbackTimeInfo* timeInfo,
			 PaStreamCallbackFlags statusFlags,
			 void *arg)
{
  audio_portaudio_sink *self = (audio_portaudio_sink *)arg;
  int nreqd_samples =
    framesPerBuffer * self->d_output_parameters.channelCount;

  int navail_samples = self->d_reader->items_available();
  
  if (nreqd_samples <= navail_samples) {  // We've got enough data...
    {
      gruel::scoped_lock guard(self->d_ringbuffer_mutex);

      memcpy(outputBuffer,
	     self->d_reader->read_pointer(),
	     nreqd_samples * sizeof(sample_t));
      self->d_reader->update_read_pointer(nreqd_samples);

      self->d_ringbuffer_ready = true;
    }

    // Tell the sink thread there is new room in the ringbuffer.
    self->d_ringbuffer_cond.notify_one();
    return paContinue;
  }

  else {			// underrun
    self->d_nunderuns++;
    ssize_t r = ::write(2, "aU", 2);	// FIXME change to non-blocking call
    if(r == -1) {
      perror("audio_portaudio_source::portaudio_source_callback write error to stderr.");
    }

    // FIXME we should transfer what we've got and pad the rest
    memset(outputBuffer, 0, nreqd_samples * sizeof(sample_t));

    self->d_ringbuffer_ready = true;
    self->d_ringbuffer_cond.notify_one();  // Tell the sink to get going!

    return paContinue;
  }
}


// ----------------------------------------------------------------

audio_portaudio_sink::audio_portaudio_sink(int sampling_rate,
					   const std::string device_name,
					   bool ok_to_block)
  : gr_sync_block ("audio_portaudio_sink",
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
    d_nunderuns(0)
{
  memset(&d_output_parameters, 0, sizeof(d_output_parameters));
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
    fprintf(stderr,"\nUsing Default Device\n");
    device = Pa_GetDefaultOutputDevice();
    deviceInfo = Pa_GetDeviceInfo(device);
    fprintf(stderr,"%s is the chosen device using %s as the host\n",
	    deviceInfo->name, Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
  }
  else
  {
    bool found = false;
    fprintf(stderr,"\nTest Devices\n");
    for (i=0;i<numDevices;i++) {
      deviceInfo = Pa_GetDeviceInfo( i );
      fprintf(stderr,"Testing device name: %s",deviceInfo->name);
      if (deviceInfo->maxOutputChannels <= 0) {
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
      exit(1);
    }
  }


  d_output_parameters.device = device;
  d_output_parameters.channelCount     = deviceInfo->maxOutputChannels;
  d_output_parameters.sampleFormat     = SAMPLE_FORMAT;
  d_output_parameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
  d_output_parameters.hostApiSpecificStreamInfo = NULL;

  // We fill in the real channelCount in check_topology when we know
  // how many inputs are connected to us.

  // Now that we know the maximum number of channels (allegedly)
  // supported by the h/w, we can compute a reasonable input
  // signature.  The portaudio specs say that they'll accept any
  // number of channels from 1 to max.
  set_input_signature(gr_make_io_signature(1, deviceInfo->maxOutputChannels,
					   sizeof (sample_t)));
}


bool
audio_portaudio_sink::check_topology (int ninputs, int noutputs)
{
  PaError err;

  if (Pa_IsStreamActive(d_stream))
  {
      Pa_CloseStream(d_stream);
      d_stream = 0;
      d_reader.reset();		// boost::shared_ptr for d_reader = 0
      d_writer.reset();		// boost::shared_ptr for d_write = 0
  }

  d_output_parameters.channelCount = ninputs;	// # of channels we're really using

#if 1
  d_portaudio_buffer_size_frames = (int)(0.0213333333  * d_sampling_rate + 0.5);  // Force 1024 frame buffers at 48000
  fprintf(stderr, "Latency = %8.5f, requested sampling_rate = %g\n", // Force latency to 21.3333333.. ms
	  0.0213333333, (double)d_sampling_rate);
#endif
  err = Pa_OpenStream(&d_stream,
		      NULL,			// No input
		      &d_output_parameters,
		      d_sampling_rate,
		      d_portaudio_buffer_size_frames,
		      paClipOff,
		      &portaudio_sink_callback,
		      (void*)this);

  if (err != paNoError) {
    output_error_msg ("OpenStream failed", err);
    return false;
  }

#if 0  
  const PaStreamInfo *psi = Pa_GetStreamInfo(d_stream);

  d_portaudio_buffer_size_frames = (int)(d_output_parameters.suggestedLatency  * psi->sampleRate);
  fprintf(stderr, "Latency = %7.4f, psi->sampleRate = %g\n",
	  d_output_parameters.suggestedLatency, psi->sampleRate);
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

audio_portaudio_sink::~audio_portaudio_sink ()
{
  Pa_StopStream(d_stream);	// wait for output to drain
  Pa_CloseStream(d_stream);
  Pa_Terminate();
}

/*
 * This version consumes everything sent to it, blocking if required.
 * I think this will allow us better control of the total buffering/latency
 * in the audio path.
 */
int
audio_portaudio_sink::work (int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
{
  const float **in = (const float **) &input_items[0];
  const unsigned nchan = d_output_parameters.channelCount; // # of channels == samples/frame

  int k;

  for (k = 0; k < noutput_items; ){
    int nframes = d_writer->space_available() / nchan;	// How much space in ringbuffer
    if (nframes == 0){			// no room...
      if (d_ok_to_block){
	{
	  gruel::scoped_lock guard(d_ringbuffer_mutex);
	  while (!d_ringbuffer_ready)
	    d_ringbuffer_cond.wait(guard);
	}

	continue;
      }
      else {
	// There's no room and we're not allowed to block.
	// (A USRP is most likely controlling the pacing through the pipeline.)
	// We drop the samples on the ground, and say we processed them all ;)
	//
	// FIXME, there's probably room for a bit more finesse here.
	return noutput_items;
      }
    }

    // We can write the smaller of the request and the room we've got
    {
      gruel::scoped_lock guard(d_ringbuffer_mutex);

      int nf = std::min(noutput_items - k, nframes);
      float *p = (float *) d_writer->write_pointer();
      
      for (int i = 0; i < nf; i++)
	for (unsigned int c = 0; c < nchan; c++)
	  *p++ = in[c][k + i];
      
      d_writer->update_write_pointer(nf * nchan);
      k += nf;

      d_ringbuffer_ready = false;
    }
  }

  return k;  // tell how many we actually did
}

void
audio_portaudio_sink::output_error_msg (const char *msg, int err)
{
  fprintf (stderr, "audio_portaudio_sink[%s]: %s: %s\n",
	   d_device_name.c_str (), msg, Pa_GetErrorText(err));
}

void
audio_portaudio_sink::bail (const char *msg, int err) throw (std::runtime_error)
{
  output_error_msg (msg, err);
  throw std::runtime_error ("audio_portaudio_sink");
}
