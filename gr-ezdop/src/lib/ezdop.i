/*
 * Copyright 2004,2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

//%feature("autodoc", "1");		// generate python docstrings

%include "exception.i"
%import "gnuradio.i"				// the common stuff

%{
#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include "ezdop_source_c.h"
%}

GR_SWIG_BLOCK_MAGIC(ezdop,source_c);

ezdop_source_c_sptr ezdop_make_source_c();

class ezdop_source_c : public gr_sync_block 
{
private:
    ezdop_source_c() throw (std::runtime_error);

public:
    int  rate();
    bool set_rate(int rate);
    bool rotate();
    bool stop_rotating();
};
