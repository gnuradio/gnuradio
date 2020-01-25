/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define ZEROMQ_API

%include "gnuradio.i"

//load generated python docstrings
%include "zeromq_swig_doc.i"

%{
#include "gnuradio/zeromq/pub_sink.h"
#include "gnuradio/zeromq/pub_msg_sink.h"
#include "gnuradio/zeromq/push_sink.h"
#include "gnuradio/zeromq/push_msg_sink.h"
#include "gnuradio/zeromq/rep_sink.h"
#include "gnuradio/zeromq/rep_msg_sink.h"
#include "gnuradio/zeromq/sub_source.h"
#include "gnuradio/zeromq/sub_msg_source.h"
#include "gnuradio/zeromq/pull_source.h"
#include "gnuradio/zeromq/pull_msg_source.h"
#include "gnuradio/zeromq/req_source.h"
#include "gnuradio/zeromq/req_msg_source.h"
%}

%include "gnuradio/zeromq/pub_sink.h"
%include "gnuradio/zeromq/pub_msg_sink.h"
%include "gnuradio/zeromq/push_sink.h"
%include "gnuradio/zeromq/push_msg_sink.h"
%include "gnuradio/zeromq/rep_sink.h"
%include "gnuradio/zeromq/rep_msg_sink.h"
%include "gnuradio/zeromq/sub_source.h"
%include "gnuradio/zeromq/sub_msg_source.h"
%include "gnuradio/zeromq/pull_source.h"
%include "gnuradio/zeromq/pull_msg_source.h"
%include "gnuradio/zeromq/req_source.h"
%include "gnuradio/zeromq/req_msg_source.h"

GR_SWIG_BLOCK_MAGIC2(zeromq, pub_sink);
GR_SWIG_BLOCK_MAGIC2(zeromq, pub_msg_sink);
GR_SWIG_BLOCK_MAGIC2(zeromq, push_sink);
GR_SWIG_BLOCK_MAGIC2(zeromq, push_msg_sink);
GR_SWIG_BLOCK_MAGIC2(zeromq, rep_sink);
GR_SWIG_BLOCK_MAGIC2(zeromq, rep_msg_sink);
GR_SWIG_BLOCK_MAGIC2(zeromq, sub_source);
GR_SWIG_BLOCK_MAGIC2(zeromq, sub_msg_source);
GR_SWIG_BLOCK_MAGIC2(zeromq, pull_source);
GR_SWIG_BLOCK_MAGIC2(zeromq, pull_msg_source);
GR_SWIG_BLOCK_MAGIC2(zeromq, req_source);
GR_SWIG_BLOCK_MAGIC2(zeromq, req_msg_source);
