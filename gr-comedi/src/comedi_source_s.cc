/* -*- c++ -*- */
/*
 * Copyright 2005,2010 Free Software Foundation, Inc.
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

#include <sys/mman.h>

#include <comedi_source_s.h>
#include <gr_io_signature.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <stdexcept>
#include <gri_comedi.h>


// FIXME these should query some kind of user preference


static std::string 
default_device_name ()
{
  return "/dev/comedi0";
}

// ----------------------------------------------------------------

comedi_source_s_sptr
comedi_make_source_s (int sampling_freq, const std::string dev)
{
  return gnuradio::get_initial_sptr(new comedi_source_s (sampling_freq, dev));
}

comedi_source_s::comedi_source_s (int sampling_freq,
				  const std::string device_name)
  : gr_sync_block ("comedi_source_s",
		   gr_make_io_signature (0, 0, 0),
		   gr_make_io_signature (0, 0, 0)),
    d_sampling_freq (sampling_freq),
    d_device_name (device_name.empty() ? default_device_name() : device_name),
    d_dev (0),
    d_subdevice (0/*COMEDI_SUBD_AI*/),
    d_n_chan (1),	// number of input channels
    d_map (0),
    d_buffer_size (0),
    d_buf_front (0),
    d_buf_back (0)
{
  int  aref=AREF_GROUND;
  int  range=0;

  d_dev = comedi_open(d_device_name.c_str());
  if (d_dev == 0){
    comedi_perror(d_device_name.c_str());
    throw std::runtime_error ("comedi_source_s");
  }

  unsigned int chanlist[256];

  for(int i=0; i<d_n_chan; i++){
    chanlist[i]=CR_PACK(i,range,aref);
  }

  comedi_cmd cmd;
  int ret;

  ret = comedi_get_cmd_generic_timed(d_dev,d_subdevice,&cmd,d_n_chan,(unsigned int)(1e9/sampling_freq));
  if(ret<0)
    bail ("comedi_get_cmd_generic_timed", comedi_errno());

  // TODO: check period_ns is not to far off sampling_freq

  d_buffer_size = comedi_get_buffer_size(d_dev, d_subdevice);
  if (d_buffer_size <= 0)
    bail ("comedi_get_buffer_size", comedi_errno());

  d_map = mmap(NULL,d_buffer_size,PROT_READ,MAP_SHARED,comedi_fileno(d_dev),0);
  if (d_map == MAP_FAILED)
    bail ("mmap", errno);

  cmd.chanlist = chanlist;
  cmd.chanlist_len = d_n_chan;
  cmd.scan_end_arg = d_n_chan;

  cmd.stop_src=TRIG_NONE;
  cmd.stop_arg=0;

  /* comedi_command_test() tests a command to see if the
   * trigger sources and arguments are valid for the subdevice.
   * If a trigger source is invalid, it will be logically ANDed
   * with valid values (trigger sources are actually bitmasks),
   * which may or may not result in a valid trigger source.
   * If an argument is invalid, it will be adjusted to the
   * nearest valid value.  In this way, for many commands, you
   * can test it multiple times until it passes.  Typically,
   * if you can't get a valid command in two tests, the original
   * command wasn't specified very well. */
  ret = comedi_command_test(d_dev,&cmd);

  if(ret<0)
    bail ("comedi_command_test", comedi_errno());

  ret = comedi_command_test(d_dev,&cmd);

  if(ret<0)
    bail ("comedi_command_test", comedi_errno());

  /* start the command */
  ret = comedi_command(d_dev,&cmd);

  if(ret<0)
    bail ("comedi_command", comedi_errno());

  set_output_multiple (d_n_chan*sizeof(sampl_t));

  assert(sizeof(sampl_t) == sizeof(short));
  set_output_signature (gr_make_io_signature (1, 1, sizeof (sampl_t)));
}

bool
comedi_source_s::check_topology (int ninputs, int noutputs)
{
  if (noutputs > d_n_chan)
    throw std::runtime_error ("comedi_source_s");

  return true;
}

comedi_source_s::~comedi_source_s ()
{
  if (d_map) {
    munmap(d_map, d_buffer_size);
    d_map = 0;
  }

  comedi_close(d_dev);
}

int
comedi_source_s::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  int ret;

  int work_left = noutput_items * sizeof(sampl_t) * d_n_chan;
  sampl_t *pbuf = (sampl_t*)d_map;

  do {

    do {
      ret = comedi_get_buffer_contents(d_dev,d_subdevice);
      if (ret < 0)
        bail ("comedi_get_buffer_contents", comedi_errno());

      assert(ret % sizeof(sampl_t) == 0);
      assert(work_left % sizeof(sampl_t) == 0);

      ret = std::min(ret, work_left);
      d_buf_front += ret;

      assert(d_buffer_size%d_n_chan == 0);
      if (d_buf_front-d_buf_back > (unsigned)d_buffer_size) {
	      d_buf_front+=d_buffer_size;
	      d_buf_back +=d_buffer_size;
      }

      if(d_buf_front==d_buf_back){
        usleep(1000000*std::min(work_left,d_buffer_size/2)/(d_sampling_freq*sizeof(sampl_t)*d_n_chan));
        continue;
      }
    } while (d_buf_front==d_buf_back);

    for(unsigned i=d_buf_back/sizeof(sampl_t);i<d_buf_front/sizeof(sampl_t);i++){
      int chan = i%d_n_chan;
      int o_idx = noutput_items-work_left/d_n_chan/sizeof(sampl_t)+(i-d_buf_back/sizeof(sampl_t))/d_n_chan;

      if (output_items[chan])
        ((short*)(output_items[chan]))[o_idx] = 
		(int)pbuf[i%(d_buffer_size/sizeof(sampl_t))] - 32767;
    }

    ret = comedi_mark_buffer_read(d_dev,d_subdevice,d_buf_front-d_buf_back);
    if(ret<0)
      bail ("comedi_mark_buffer_read", comedi_errno());

    work_left -= d_buf_front-d_buf_back;

    d_buf_back = d_buf_front;

  } while(work_left>0);

  return noutput_items;
}

void
comedi_source_s::output_error_msg (const char *msg, int err)
{
  fprintf (stderr, "comedi_source_s[%s]: %s: %s\n",
	   d_device_name.c_str(), msg,  comedi_strerror(err));
}

void
comedi_source_s::bail (const char *msg, int err) throw (std::runtime_error)
{
  output_error_msg (msg, err);
  throw std::runtime_error ("comedi_source_s");
}
