/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#define ZEROMQ_API

%include "gnuradio.i"

//load generated python docstrings
%include "zeromq_swig_doc.i"

%{
#include "gnuradio/zeromq/sink_pubsub.h"
#include "gnuradio/zeromq/push_sink.h"
#include "gnuradio/zeromq/sink_reqrep.h"
#include "gnuradio/zeromq/sink_reqrep_nopoll.h"
#include "gnuradio/zeromq/source_pushpull_feedback.h"
#include "gnuradio/zeromq/pull_source.h"
#include "gnuradio/zeromq/source_reqrep.h"
#include "gnuradio/zeromq/source_reqrep_nopoll.h"
%}

%include "gnuradio/zeromq/sink_pubsub.h"
%include "gnuradio/zeromq/push_sink.h"
%include "gnuradio/zeromq/sink_reqrep.h"
%include "gnuradio/zeromq/sink_reqrep_nopoll.h"
%include "gnuradio/zeromq/source_pushpull_feedback.h"
%include "gnuradio/zeromq/pull_source.h"
%include "gnuradio/zeromq/source_reqrep.h"
%include "gnuradio/zeromq/source_reqrep_nopoll.h"

GR_SWIG_BLOCK_MAGIC2(zeromq, sink_pubsub);
GR_SWIG_BLOCK_MAGIC2(zeromq, push_sink);
GR_SWIG_BLOCK_MAGIC2(zeromq, sink_reqrep);
GR_SWIG_BLOCK_MAGIC2(zeromq, sink_reqrep_nopoll);
GR_SWIG_BLOCK_MAGIC2(zeromq, source_pushpull_feedback);
GR_SWIG_BLOCK_MAGIC2(zeromq, pull_source);
GR_SWIG_BLOCK_MAGIC2(zeromq, source_reqrep);
GR_SWIG_BLOCK_MAGIC2(zeromq, source_reqrep_nopoll);
