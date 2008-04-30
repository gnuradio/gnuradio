/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#include <gri_logger.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdexcept>
#include <boost/weak_ptr.hpp>
#include <string.h>


/*
 * This class creates the thread that reads from the ringbuffer and
 * and writes to the file.  This is opaque to the user.
 */
class gri_log_poster : public omni_thread
{
  FILE		       *d_fp;
  gr_buffer_sptr	d_writer;
  gr_buffer_reader_sptr	d_reader;
  omni_semaphore	d_ringbuffer_ready;
  volatile bool		d_time_to_die;
  volatile bool		d_writer_overrun;

  virtual void* run_undetached(void * arg);

public:
  gri_log_poster(const char *filename);
  ~gri_log_poster();

  void kill() { d_time_to_die = true; post(); }
  gr_buffer_sptr writer() const { return d_writer; }
  void post() { d_ringbuffer_ready.post(); }
  void note_writer_overrun() { d_writer_overrun = true; }
};

gri_log_poster::gri_log_poster(const char *filename)
  : omni_thread(),
    d_ringbuffer_ready(1, 1),		// binary semaphore
    d_time_to_die(false),
    d_writer_overrun(false)
{
  if ((d_fp = fopen(filename, "w")) == 0){
    perror (filename);
    throw std::runtime_error("can't open file");
  }

  // Create a 1MB buffer.
  d_writer = gr_make_buffer(1 * 1024 * 1024, sizeof(unsigned char));
  d_reader = gr_buffer_add_reader(d_writer, 0);

  start_undetached();  // start the thread
}

gri_log_poster::~gri_log_poster()
{
  if (d_fp != 0){
    fclose(d_fp);
    d_fp = 0;
  }
}

/*
 * This is the body of the logging thread.
 */
void *
gri_log_poster::run_undetached(void *arg)
{
  int nbytes;

  //fprintf(stderr, "Enter: run_undetached!\n");

  while (!d_time_to_die){
    while ((nbytes = d_reader->items_available()) > 0){
      fwrite(d_reader->read_pointer(), 1, nbytes, d_fp);
      d_reader->update_read_pointer(nbytes);
    }
    fflush(d_fp);
    d_ringbuffer_ready.wait();

    if (d_writer_overrun){
      fputs(">>>>> gri_logger: writer overrun.  Info lost <<<<<\n", d_fp);
      d_writer_overrun = false;
    }
  }

  // fprintf(stderr, "Exit: run_undetached!\n");
  return 0;
}

// ------------------------------------------------------------------------

static boost::weak_ptr<gri_logger> s_singleton;  // weak pointer IQ test ;-)
static omni_mutex s_singleton_mutex;

gri_logger_sptr
gri_logger::singleton()
{
  omni_mutex_lock l(s_singleton_mutex);
  gri_logger_sptr r;

  if (r = s_singleton.lock())
    return r;

  r = gri_logger_sptr(new gri_logger("gri_logger.log"));
  s_singleton = r;
  return r;
}
  

gri_logger::gri_logger(const char *filename)
{
  d_poster = new gri_log_poster(filename);
}

gri_logger::~gri_logger()
{
  d_poster->kill();
  d_poster->join(NULL);
}

void
gri_logger::write(const void *buf, size_t count)
{
  omni_mutex_lock l(d_write_mutex);
  gr_buffer_sptr writer = d_poster->writer();
  
  // either write it all, or drop it on the ground
  if (count <= (size_t) writer->space_available()){
    memcpy(writer->write_pointer(), buf, count);
    writer->update_write_pointer(count);
    d_poster->post();
  }
  else {
    d_poster->note_writer_overrun();
  }
}

void
gri_logger::printf(const char *format, ...)
{
  va_list	ap;
  char		buf[4096];
  int		n;
  
  va_start(ap, format);
  n = vsnprintf(buf, sizeof(buf), format, ap);
  va_end(ap);
  if (n > -1 && n < (ssize_t) sizeof(buf))
    write(buf, n);
}
