/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#define CHANNELS_API

%include "gnuradio.i"

//load generated python docstrings
%include "channels_swig_doc.i"

%{
#include "gnuradio/channels/channel_model.h"
#include "gnuradio/channels/channel_model2.h"
#include "gnuradio/channels/cfo_model.h"
#include "gnuradio/channels/dynamic_channel_model.h"
#include "gnuradio/channels/fading_model.h"
#include "gnuradio/channels/selective_fading_model.h"
#include "gnuradio/channels/selective_fading_model2.h"
#include "gnuradio/channels/sro_model.h"
%}

%include "gnuradio/channels/channel_model.h"
%include "gnuradio/channels/channel_model2.h"
%include "gnuradio/channels/cfo_model.h"
%include "gnuradio/channels/dynamic_channel_model.h"
%include "gnuradio/channels/fading_model.h"
%include "gnuradio/channels/selective_fading_model.h"
%include "gnuradio/channels/selective_fading_model2.h"
%include "gnuradio/channels/sro_model.h"

GR_SWIG_BLOCK_MAGIC2(channels, channel_model);
GR_SWIG_BLOCK_MAGIC2(channels, channel_model2);
GR_SWIG_BLOCK_MAGIC2(channels, cfo_model);
GR_SWIG_BLOCK_MAGIC2(channels, dynamic_channel_model);
GR_SWIG_BLOCK_MAGIC2(channels, fading_model);
GR_SWIG_BLOCK_MAGIC2(channels, selective_fading_model);
GR_SWIG_BLOCK_MAGIC2(channels, selective_fading_model2);
GR_SWIG_BLOCK_MAGIC2(channels, sro_model);
