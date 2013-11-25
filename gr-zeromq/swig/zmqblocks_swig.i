/* -*- c++ -*- */

#define ZMQBLOCKS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "zmqblocks_swig_doc.i"

%{
#include "zmqblocks/sink_reqrep.h"
#include "zmqblocks/source_reqrep.h"
#include "zmqblocks/sink_reqrep_nopoll.h"
#include "zmqblocks/source_reqrep_nopoll.h"
#include "zmqblocks/sink_pushpull.h"
#include "zmqblocks/source_pushpull.h"
#include "zmqblocks/source_pushpull_feedback.h"
#include "zmqblocks/sink_pubsub.h"
%}

%include "zmqblocks/sink_reqrep.h"
GR_SWIG_BLOCK_MAGIC2(zmqblocks, sink_reqrep);
%include "zmqblocks/source_reqrep.h"
GR_SWIG_BLOCK_MAGIC2(zmqblocks, source_reqrep);
%include "zmqblocks/sink_reqrep_nopoll.h"
GR_SWIG_BLOCK_MAGIC2(zmqblocks, sink_reqrep_nopoll);
%include "zmqblocks/source_reqrep_nopoll.h"
GR_SWIG_BLOCK_MAGIC2(zmqblocks, source_reqrep_nopoll);
%include "zmqblocks/sink_pushpull.h"
GR_SWIG_BLOCK_MAGIC2(zmqblocks, sink_pushpull);
%include "zmqblocks/source_pushpull.h"
GR_SWIG_BLOCK_MAGIC2(zmqblocks, source_pushpull);

%include "zmqblocks/source_pushpull_feedback.h"
GR_SWIG_BLOCK_MAGIC2(zmqblocks, source_pushpull_feedback);
%include "zmqblocks/sink_pubsub.h"
GR_SWIG_BLOCK_MAGIC2(zmqblocks, sink_pubsub);

