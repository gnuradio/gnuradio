/* -*- c++ -*- */

#define NETWORK_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "network_swig_doc.i"

%{
#include "gnuradio/network/tcp_sink.h"
#include "gnuradio/network/udp_source.h"
#include "gnuradio/network/udp_sink.h"
%}

%include "gnuradio/network/tcp_sink.h"
GR_SWIG_BLOCK_MAGIC2(network, tcp_sink);
%include "gnuradio/network/udp_source.h"
GR_SWIG_BLOCK_MAGIC2(network, udp_source);
%include "gnuradio/network/udp_sink.h"
GR_SWIG_BLOCK_MAGIC2(network, udp_sink);
