/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <cstdio>
#include <stdexcept>
#include <cmath>
#include "fsm.h"

fsm::fsm()
{
  d_I=0;
  d_S=0;
  d_O=0;
  d_NS.resize(0);
  d_OS.resize(0);
  d_PS.resize(0);
  d_PI.resize(0);
}

fsm::fsm(const fsm &FSM)
{
  d_I=FSM.I();
  d_S=FSM.S();
  d_O=FSM.O();
  d_NS=FSM.NS();
  d_OS=FSM.OS();
  d_PS=FSM.PS();
  d_PI=FSM.PI();
}

fsm::fsm(const int I, const int S, const int O, const std::vector<int> &NS, const std::vector<int> &OS)
{
  d_I=I;
  d_S=S;
  d_O=O;
  d_NS=NS;
  d_OS=OS;
  d_PS.resize(d_I*d_S);
  d_PI.resize(d_I*d_S);
  
  // generate the PS, PI tables for later use
  for(int i=0;i<d_S;i++) {
    int j=0;
    for(int ii=0;ii<d_S;ii++) for(int jj=0;jj<d_I;jj++) {
      if(d_NS[ii*d_I+jj]!=i) continue;
      d_PS[i*d_I+j]=ii;
      d_PI[i*d_I+j]=jj;
      j++;
    }
  }
}

//######################################################################
//# Read an FSM specification from a file.
//# Format (hopefully will become more flexible in the future...):
//# I S O (in the first line)
//# blank line
//# Next state matrix (S lines, each with I integers separated by spaces)
//# blank line
//# output symbol matrix (S lines, each with I integers separated by spaces)
//# optional comments
//######################################################################
fsm::fsm(const char *name) 
{
  FILE *fsmfile;

  if((fsmfile=fopen(name,"r"))==NULL) 
    throw std::runtime_error ("file open error in fsm()");
    //printf("file open error in fsm()\n");
  
  fscanf(fsmfile,"%d %d %d\n",&d_I,&d_S,&d_O);
  d_NS.resize(d_I*d_S);
  d_OS.resize(d_I*d_S);
  d_PS.resize(d_I*d_S);
  d_PI.resize(d_I*d_S);

  for(int i=0;i<d_S;i++) {
    for(int j=0;j<d_I;j++) fscanf(fsmfile,"%d",&(d_NS[i*d_I+j]));
  }
  for(int i=0;i<d_S;i++) {
    for(int j=0;j<d_I;j++) fscanf(fsmfile,"%d",&(d_OS[i*d_I+j]));
  }
  
  // generate the PS, PI tables for later use
  for(int i=0;i<d_S;i++) {
    int j=0;
    for(int ii=0;ii<d_S;ii++) for(int jj=0;jj<d_I;jj++) {
      if(d_NS[ii*d_I+jj]!=i) continue;
      d_PS[i*d_I+j]=ii;
      d_PI[i*d_I+j]=jj;
      j++;
    }
  }
}

//######################################################################
//# Automatically generate an FSM specification describing the 
//# ISI for a channel
//# of length ch_length and a modulation of size mod_size
//######################################################################
fsm::fsm(const int mod_size, const int ch_length)
{
  d_I=mod_size;
  d_S=(int) (pow(1.0*d_I,1.0*ch_length-1)+0.5);
  d_O=d_S*d_I;

  d_NS.resize(d_I*d_S);
  d_OS.resize(d_I*d_S);
  d_PS.resize(d_I*d_S);
  d_PI.resize(d_I*d_S);

  for(int s=0;s<d_S;s++) {
    for(int i=0;i<d_I;i++) { 
      int t=i*d_S+s;
      d_NS[s*d_I+i] = t/d_I;
      d_OS[s*d_I+i] = t;
    }
  }
  
  // generate the PS, PI tables for later use
  for(int i=0;i<d_S;i++) {
    int j=0;
    for(int ii=0;ii<d_S;ii++) for(int jj=0;jj<d_I;jj++) {
      if(d_NS[ii*d_I+jj]!=i) continue;
      d_PS[i*d_I+j]=ii;
      d_PI[i*d_I+j]=jj;
      j++;
    }
  }
}
