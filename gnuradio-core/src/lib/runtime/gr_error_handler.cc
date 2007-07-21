/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
/*
 * This code is based on error.cc from the "Click Modular Router".
 * Original copyright follows:
 */
/* 
 * error.{cc,hh} -- flexible classes for error reporting
 * Eddie Kohler
 *
 * Copyright (c) 1999-2000 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_error_handler.h>
#include <assert.h>
#include <stdexcept>
#include <unistd.h>

#ifdef HAVE_IO_H
#include <io.h>
#endif

static gr_error_handler	*s_default_handler = 0;
static gr_error_handler *s_silent_handler = 0;

bool
gr_error_handler::has_default_handler()
{
  return s_default_handler != 0;
}

void
gr_error_handler::set_default_handler(gr_error_handler *errh)
{
  s_default_handler = errh;
}

gr_error_handler *
gr_error_handler::default_handler()
{
  assert (s_default_handler != 0);
  return s_default_handler;
}

gr_error_handler *
gr_error_handler::silent_handler()
{
  assert (s_silent_handler != 0);
  return s_silent_handler;
}

// ----------------------------------------------------------------

gr_error_handler::~gr_error_handler()
{
  // nop
}

void
gr_error_handler::debug(const char *format, ...)
{
  va_list val;
  va_start(val, format);
  verror(ERR_DEBUG, format, val);
  va_end(val);
}

void
gr_error_handler::message(const char *format, ...)
{
  va_list val;
  va_start(val, format);
  verror(ERR_MESSAGE, format, val);
  va_end(val);
}

void
gr_error_handler::warning(const char *format, ...)
{
  va_list val;
  va_start(val, format);
  verror(ERR_WARNING, format, val);
  va_end(val);
}

void
gr_error_handler::error(const char *format, ...)
{
  va_list val;
  va_start(val, format);
  verror(ERR_ERROR, format, val);
  va_end(val);
}

void
gr_error_handler::fatal(const char *format, ...)
{
  va_list val;
  va_start(val, format);
  verror(ERR_FATAL, format, val);
  va_end(val);
}

void
gr_error_handler::verror(seriousness s, const char *format, va_list val)
{
  std::string text = make_text(s, format, val);
  handle_text(s, text);
  count_error(s);
}

void
gr_error_handler::verror_text(seriousness s, const std::string &text)
{
  // text is already made
  handle_text(s, text);
  count_error(s);
}

std::string
gr_error_handler::make_text(seriousness s, const char *format, va_list val)
{
  char text_buf[4096];
  vsnprintf(text_buf, sizeof(text_buf), format, val);
  text_buf[sizeof(text_buf)-1] = 0;
  return text_buf;
}

// ----------------------------------------------------------------

void
gr_base_error_handler::count_error(seriousness s)
{
  if (s < ERR_WARNING)
    /* do nothing */;
  else if (s < ERR_ERROR)
    d_nwarnings++;
  else
    d_nerrors++;
}

// ----------------------------------------------------------------

gr_file_error_handler::gr_file_error_handler(FILE *file)
  : d_file(file), d_fd(-1)
{
}

gr_file_error_handler::gr_file_error_handler(int file_descriptor)
{
  d_fd = dup(file_descriptor);	// so we can fclose it
  if (d_fd == -1){
    perror("gr_file_error_handler:dup");
    throw std::invalid_argument("gr_file_error_handler:dup");
  }
  d_file = fdopen(d_fd, "w");
  if (d_file == 0){
    perror("gr_file_error_handler:fdopen");
    throw std::invalid_argument("gr_file_error_handler:fdopen");
  }
}

gr_file_error_handler::~gr_file_error_handler()
{
  if (d_fd != -1){
    fclose(d_file);
  }
}

void
gr_file_error_handler::handle_text(seriousness s, const std::string &text)
{
  if (text.length() <= 0)
    return;
  
  fwrite(text.data(), 1, text.length(), d_file);
  if (text[text.length()-1] != '\n')
    fwrite("\n", 1, 1, d_file);

  if (d_fd != -1)
    fflush(d_file);	// keep synced with any other users of fd
}
  

// ----------------------------------------------------------------
// static error handlers
//

class gr_silent_error_handler : public gr_base_error_handler
{
public:
  gr_silent_error_handler() {}
  void handle_text(seriousness s, const std::string &str);
};

void
gr_silent_error_handler::handle_text(seriousness s, const std::string &str)
{
  // nop
}

class force_init {
public:
  force_init()
  {
    s_default_handler = new gr_file_error_handler(stdout);
    s_silent_handler = new gr_silent_error_handler();
  }
};

static force_init	kludge;
