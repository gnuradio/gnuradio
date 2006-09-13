/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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

%{
#include <atsc_types.h>
#include <atsc_consts.h>
#include <GrAtscRandomizer.h>
#include <GrAtscRSEncoder.h>
#include <GrAtscInterleaver.h>
#include <GrAtscTrellisEncoder.h>
#include <GrAtscFieldSyncMux.h>
#include <GrAtscSymbolMapper.h>
#include <GrAtscConvert2xTo20.h>
#include <GrWeaverModHead.h>
#include <GrWeaverModTail.h>

%}

// from atsc_types.h
class plinfo;
class atsc_mpeg_packet;
class atsc_mpeg_packet_no_sync;
class atsc_mpeg_packet_rs_encoded;
class atsc_data_segment;
class atsc_soft_data_segment;

%include <atsc_consts.h>


// leave out the VrHistoryProc and pretend we're directly derived from VrSigProc

// %template(VrHistoryProc_1) VrHistoryProc<atsc_mpeg_packet,atsc_mpeg_packet_no_sync>;

class GrAtscRandomizer : public VrSigProc
// class GrAtscRandomizer : public VrHistoryProc<atsc_mpeg_packet, atsc_mpeg_packet_no_sync>
{
public:
  GrAtscRandomizer ();
  ~GrAtscRandomizer ();
};

class GrAtscRSEncoder : public VrSigProc
{
public:
  GrAtscRSEncoder ();
  ~GrAtscRSEncoder ();
};

class GrAtscInterleaver : public VrSigProc
{
public:
  GrAtscInterleaver ();
  ~GrAtscInterleaver ();
};

class GrAtscTrellisEncoder : public VrSigProc
{
public:
  GrAtscTrellisEncoder ();
  ~GrAtscTrellisEncoder ();
};

class GrAtscFieldSyncMux : public VrSigProc
{
public:
  GrAtscFieldSyncMux ();
  ~GrAtscFieldSyncMux ();
};

template<class oType>
class GrAtscSymbolMapper : public VrSigProc
{
public:
  GrAtscSymbolMapper ();
  ~GrAtscSymbolMapper ();
};

%template(GrAtscSymbolMapperF) GrAtscSymbolMapper<float>;

template<class iType, class oType>
class GrWeaverModHead : public VrSigProc
{
public:
  GrWeaverModHead (int interp_factor);
  ~GrWeaverModHead ();
};

%template(GrWeaverModHeadFF) GrWeaverModHead<float,float>;

template<class iType, class oType>
class GrWeaverModTail : public VrSigProc {
public:
  GrWeaverModTail (float freq, float gain);
  ~GrWeaverModTail ();

  //! frequency is in Hz
  void set_freq (float frequency);
  void set_gain (float g);
};

%template(GrWeaverModTailFS) GrWeaverModTail<float,short>;

class GrAtscConvert2xTo20 : public VrSigProc
{
public:
  GrAtscConvert2xTo20 ();
  ~GrAtscConvert2xTo20 ();
};


#if 0	// FIXME
%template(VrSource_mpeg_packet)     VrSource<atsc_mpeg_packet>;
%template(VrFileSource_mpeg_packet) VrFileSource<atsc_mpeg_packet>;

%template(VrSink_mpeg_packet) 	    VrSink<atsc_mpeg_packet>;
%template(VrFileSink_mpeg_packet)   VrFileSink<atsc_mpeg_packet>;
#endif
