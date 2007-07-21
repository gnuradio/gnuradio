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

%rename(error_handler) gr_error_handler;
%rename(file_error_handler) gr_file_error_handler;

class gr_error_handler {
public:
  enum seriousness {
    ERR_DEBUG		= 0x00000000,
    ERR_MESSAGE		= 0x00010000,
    ERR_WARNING		= 0x00020000,
    ERR_ERROR		= 0x00030000,
    ERR_FATAL		= 0x00040000
  };

  gr_error_handler() {}
  virtual ~gr_error_handler();

  static gr_error_handler *default_handler();
  static gr_error_handler *silent_handler();

  static bool has_default_handler();
  static void set_default_handler(gr_error_handler *errh);

  virtual int nwarnings() const = 0;
  virtual int nerrors() const = 0;
  virtual void reset_counts() = 0;

  void verror_text(seriousness s, const std::string &text);
};

%ignore gr_base_error_handler;
class gr_base_error_handler : public gr_error_handler {
  int	d_nwarnings;
  int	d_nerrors;

public:
  gr_base_error_handler() : d_nwarnings(0), d_nerrors(0) {}
  int nwarnings() const { return d_nwarnings; }
  int nerrors() const   { return d_nerrors; }
  void reset_counts()   { d_nwarnings = d_nerrors = 0; }
  void count_error(seriousness s);
};

class gr_file_error_handler : public gr_base_error_handler {
public:
  gr_file_error_handler(int file_descriptor);
  ~gr_file_error_handler();
};
