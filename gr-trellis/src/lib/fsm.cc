/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <stdlib.h>
#include "base.h"
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
  d_TMi.resize(0);
  d_TMl.resize(0);
}

fsm::fsm(const fsm &FSM)
{
  d_I=FSM.I();
  d_S=FSM.S();
  d_O=FSM.O();
  d_NS=FSM.NS();
  d_OS=FSM.OS();
  d_PS=FSM.PS(); // is this going to make a deep copy?
  d_PI=FSM.PI();
  d_TMi=FSM.TMi();
  d_TMl=FSM.TMl();
}

fsm::fsm(int I, int S, int O, const std::vector<int> &NS, const std::vector<int> &OS)
{
  d_I=I;
  d_S=S;
  d_O=O;
  d_NS=NS;
  d_OS=OS;
 
  generate_PS_PI();
  generate_TM();
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
    throw std::runtime_error ("fsm::fsm(const char *name): file open error\n");
    //printf("file open error in fsm()\n");

  if(fscanf(fsmfile,"%d %d %d\n",&d_I,&d_S,&d_O) == EOF) {
    if(ferror(fsmfile) != 0)
      throw std::runtime_error ("fsm::fsm(const char *name): file read error\n");
  }
  
  d_NS.resize(d_I*d_S);
  d_OS.resize(d_I*d_S);

  for(int i=0;i<d_S;i++) {
    for(int j=0;j<d_I;j++) {
      if(fscanf(fsmfile,"%d",&(d_NS[i*d_I+j])) == EOF) {
	if(ferror(fsmfile) != 0)
	  throw std::runtime_error ("fsm::fsm(const char *name): file read error\n");
      }
    }
  }
  for(int i=0;i<d_S;i++) {
    for(int j=0;j<d_I;j++) {
      if(fscanf(fsmfile,"%d",&(d_OS[i*d_I+j])) == EOF) {
	if(ferror(fsmfile) != 0)
	  throw std::runtime_error ("fsm::fsm(const char *name): file read error\n");
      }
    }
  }

  fclose(fsmfile);
 
  generate_PS_PI();
  generate_TM();
}



//######################################################################
//# Automatically generate the FSM from the generator matrix
//# of a (n,k) binary convolutional code
//######################################################################
fsm::fsm(int k, int n, const std::vector<int> &G)
{

  // calculate maximum memory requirements for each input stream
  std::vector<int> max_mem_x(k,-1);
  int max_mem = -1;
  for(int i=0;i<k;i++) {
    for(int j=0;j<n;j++) {
      int mem = -1;
      if(G[i*n+j]!=0)
        mem=(int)(log(double(G[i*n+j]))/log(2.0));
      if(mem>max_mem_x[i])
        max_mem_x[i]=mem;
      if(mem>max_mem)
        max_mem=mem;
    }
  }
  
//printf("max_mem_x\n");
//for(int j=0;j<max_mem_x.size();j++) printf("%d ",max_mem_x[j]); printf("\n");

  // calculate total memory requirements to set S
  int sum_max_mem = 0;
  for(int i=0;i<k;i++)
    sum_max_mem += max_mem_x[i];

//printf("sum_max_mem = %d\n",sum_max_mem);

  d_I=1<<k;
  d_S=1<<sum_max_mem;
  d_O=1<<n;
 
  // binary representation of the G matrix
  std::vector<std::vector<int> > Gb(k*n);
  for(int j=0;j<k*n;j++) {
    Gb[j].resize(max_mem+1);
    dec2base(G[j],2,Gb[j]);
//printf("Gb\n");
//for(int m=0;m<Gb[j].size();m++) printf("%d ",Gb[j][m]); printf("\n");
  }

  // alphabet size of each shift register 
  std::vector<int> bases_x(k);
  for(int j=0;j<k ;j++) 
    bases_x[j] = 1 << max_mem_x[j];
//printf("bases_x\n");
//for(int j=0;j<max_mem_x.size();j++) printf("%d ",max_mem_x[j]); printf("\n");

  d_NS.resize(d_I*d_S);
  d_OS.resize(d_I*d_S);

  std::vector<int> sx(k);
  std::vector<int> nsx(k);
  std::vector<int> tx(k);
  std::vector<std::vector<int> > tb(k);
  for(int j=0;j<k;j++)
    tb[j].resize(max_mem+1);
  std::vector<int> inb(k);
  std::vector<int> outb(n);


  for(int s=0;s<d_S;s++) {
    dec2bases(s,bases_x,sx); // split s into k values, each representing one of the k shift registers
//printf("state = %d \nstates = ",s);
//for(int j=0;j<sx.size();j++) printf("%d ",sx[j]); printf("\n");
    for(int i=0;i<d_I;i++) {
      dec2base(i,2,inb); // input in binary
//printf("input = %d \ninputs = ",i);
//for(int j=0;j<inb.size();j++) printf("%d ",inb[j]); printf("\n");

      // evaluate next state
      for(int j=0;j<k;j++)
        nsx[j] = (inb[j]*bases_x[j]+sx[j])/2; // next state (for each shift register) MSB first
      d_NS[s*d_I+i]=bases2dec(nsx,bases_x); // collect all values into the new state

      // evaluate transitions
      for(int j=0;j<k;j++)
        tx[j] = inb[j]*bases_x[j]+sx[j]; // transition (for each shift register)MSB first
      for(int j=0;j<k;j++) {
        dec2base(tx[j],2,tb[j]); // transition in binary
//printf("transition = %d \ntransitions = ",tx[j]);
//for(int m=0;m<tb[j].size();m++) printf("%d ",tb[j][m]); printf("\n");
      }

      // evaluate outputs
      for(int nn=0;nn<n;nn++) {
        outb[nn] = 0;
        for(int j=0;j<k;j++) {
          for(int m=0;m<max_mem+1;m++)
            outb[nn] = (outb[nn] + Gb[j*n+nn][m]*tb[j][m]) % 2; // careful: polynomial 1+D ir represented as 110, not as 011
//printf("output %d equals %d\n",nn,outb[nn]);
        }
      }
      d_OS[s*d_I+i] = base2dec(outb,2);
    }
  }

  generate_PS_PI();
  generate_TM();
}




//######################################################################
//# Automatically generate an FSM specification describing the 
//# ISI for a channel
//# of length ch_length and a modulation of size mod_size
//######################################################################
fsm::fsm(int mod_size, int ch_length)
{
  d_I=mod_size;
  d_S=(int) (pow(1.0*d_I,1.0*ch_length-1)+0.5);
  d_O=d_S*d_I;

  d_NS.resize(d_I*d_S);
  d_OS.resize(d_I*d_S);

  for(int s=0;s<d_S;s++) {
    for(int i=0;i<d_I;i++) { 
      int t=i*d_S+s;
      d_NS[s*d_I+i] = t/d_I;
      d_OS[s*d_I+i] = t;
    }
  }
 
  generate_PS_PI();
  generate_TM();
}




//######################################################################
//# Automatically generate an FSM specification describing the 
//# the trellis for a CPM with h=K/P (relatively prime), 
//# alphabet size M, and frequency pulse duration L symbols
//#
//# This FSM is based on the paper by B. Rimoldi
//# "A decomposition approach to CPM", IEEE Trans. Info Theory, March 1988
//# See also my own notes at http://www.eecs.umich.edu/~anastas/docs/cpm.pdf
//######################################################################
fsm::fsm(int P, int M, int L)
{
  d_I=M;
  d_S=(int)(pow(1.0*M,1.0*L-1)+0.5)*P;
  d_O=(int)(pow(1.0*M,1.0*L)+0.5)*P;

  d_NS.resize(d_I*d_S);
  d_OS.resize(d_I*d_S);
  int nv;
  for(int s=0;s<d_S;s++) {
    for(int i=0;i<d_I;i++) {
      int s1=s/P;
      int v=s%P;
      int ns1= (i*(int)(pow(1.0*M,1.0*(L-1))+0.5)+s1)/M;
      if (L==1)
        nv=(i+v)%P;
      else
        nv=(s1%M+v)%P;
      d_NS[s*d_I+i] = ns1*P+nv;
      d_OS[s*d_I+i] = i*d_S+s;
    }
  }

  generate_PS_PI();
  generate_TM();
}










//######################################################################
//# Automatically generate an FSM specification describing the 
//# the joint trellis of fsm1 and fsm2
//######################################################################
fsm::fsm(const fsm &FSM1, const fsm &FSM2)
{
  d_I=FSM1.I()*FSM2.I();
  d_S=FSM1.S()*FSM2.S();
  d_O=FSM1.O()*FSM2.O();

  d_NS.resize(d_I*d_S);
  d_OS.resize(d_I*d_S);

  for(int s=0;s<d_S;s++) {
    for(int i=0;i<d_I;i++) {
      int s1=s/FSM2.S();
      int s2=s%FSM2.S();
      int i1=i/FSM2.I();
      int i2=i%FSM2.I();
      d_NS[s*d_I+i] = FSM1.NS()[s1 * FSM1.I() + i1] * FSM2.S() + FSM2.NS()[s2 * FSM2.I() + i2];
      d_OS[s*d_I+i] = FSM1.OS()[s1 * FSM1.I() + i1] * FSM2.O() + FSM2.OS()[s2 * FSM2.I() + i2];
    }
  }

  generate_PS_PI();
  generate_TM();
}




//######################################################################
//# Generate a new FSM representing n stages through the original FSM
//# AKA radix-n FSM
//######################################################################
fsm::fsm(const fsm &FSM, int n)
{
  d_I=(int) (pow(1.0*FSM.I(),1.0*n)+0.5);
  d_S=FSM.S();
  d_O=(int) (pow(1.0*FSM.O(),1.0*n)+0.5);

  d_NS.resize(d_I*d_S);
  d_OS.resize(d_I*d_S);

  for(int s=0;s<d_S;s++ ) {
    for(int i=0;i<d_I;i++ ) {
      std::vector<int> ii(n);
      dec2base(i,FSM.I(),ii);
      std::vector<int> oo(n);
      int ns=s;
      for(int k=0;k<n;k++) {
        oo[k]=FSM.OS()[ns*FSM.I()+ii[k]];
        ns=FSM.NS()[ns*FSM.I()+ii[k]];
      }
      d_NS[s*d_I+i]=ns;
      d_OS[s*d_I+i]=base2dec(oo,FSM.O());
    }
  }

  generate_PS_PI();
  generate_TM();
}









//######################################################################
//# generate the PS and PI tables for later use
//######################################################################
void fsm::generate_PS_PI()
{
  d_PS.resize(d_S);
  d_PI.resize(d_S);

  for(int i=0;i<d_S;i++) {
    d_PS[i].resize(d_I*d_S); // max possible size
    d_PI[i].resize(d_I*d_S);
    int j=0;
    for(int ii=0;ii<d_S;ii++) for(int jj=0;jj<d_I;jj++) {
      if(d_NS[ii*d_I+jj]!=i) continue;
      d_PS[i][j]=ii;
      d_PI[i][j]=jj;
      j++;
    }
    d_PS[i].resize(j);
    d_PI[i].resize(j);
  }
}


//######################################################################
//# generate the termination matrices TMl and TMi for later use
//######################################################################
void fsm::generate_TM()
{
  d_TMi.resize(d_S*d_S);
  d_TMl.resize(d_S*d_S);

  for(int i=0;i<d_S*d_S;i++) {
    d_TMi[i] = -1; // no meaning
    d_TMl[i] = d_S; //infinity: you need at most S-1 steps
    if (i/d_S == i%d_S)
      d_TMl[i] = 0;
  }

  for(int s=0;s<d_S;s++) {
    bool done = false;
    int attempts = 0;
    while (done == false && attempts < d_S-1) {
      done = find_es(s);
      attempts ++;
    }
    if (done == false && d_S > 1) {
      //throw std::runtime_error ("fsm::generate_TM(): FSM appears to be disconnected\n");
      printf("fsm::generate_TM(): FSM appears to be disconnected\n");
      printf("state %d cannot be reached from all other states\n",s);
    }
  }
}


// find a path from any state to the ending state "es"
bool fsm::find_es(int es)
{
  bool done = true;
  for(int s=0;s<d_S;s++) {
    if(d_TMl[s*d_S+es] < d_S) 
      continue;
    int minl=d_S;
    int mini=-1;
    for(int i=0;i<d_I;i++) {
      if( 1 + d_TMl[d_NS[s*d_I+i]*d_S+es] < minl) {
        minl = 1 + d_TMl[d_NS[s*d_I+i]*d_S+es];
        mini = i;
      }
    }
    if (mini != -1) {
      d_TMl[s*d_S+es]=minl;
      d_TMi[s*d_S+es]=mini;
    }
    else
      done = false;
  }
  return done;
}





//######################################################################
//#  generate trellis representation of FSM as an SVG file
//######################################################################
void fsm::write_trellis_svg( std::string filename ,int number_stages)
{
   std::ofstream trellis_fname (filename.c_str());
   if (!trellis_fname) {std::cout << "file not found " << std::endl ; exit(-1);}
   const int TRELLIS_Y_OFFSET = 30;
   const int TRELLIS_X_OFFSET = 20;
   const int STAGE_LABEL_Y_OFFSET = 25;
   const int STAGE_LABEL_X_OFFSET = 20;
   const int STATE_LABEL_Y_OFFSET = 30;
   const int STATE_LABEL_X_OFFSET = 5;
   const int STAGE_STATE_OFFSETS = 10;
//   std::cout << "################## BEGIN SVG TRELLIS PIC #####################" << std::endl;
   trellis_fname << "<svg viewBox = \"0 0 200 200\" version = \"1.1\">" << std::endl;

    for( int stage_num = 0;stage_num < number_stages;stage_num ++){
    // draw states
      for ( int state_num = 0;state_num < d_S ; state_num ++ ) {
        trellis_fname << "<circle cx = \"" << stage_num * STAGE_STATE_OFFSETS + TRELLIS_X_OFFSET << 
        "\" cy = \"" << state_num * STAGE_STATE_OFFSETS + TRELLIS_Y_OFFSET << "\" r = \"1\"/>" << std::endl;
      //draw branches
        if(stage_num != number_stages-1){
          for( int branch_num = 0;branch_num < d_I; branch_num++){
            trellis_fname << "<line x1 =\"" << STAGE_STATE_OFFSETS * stage_num+ TRELLIS_X_OFFSET  << "\" ";
            trellis_fname << "y1 =\"" << state_num * STAGE_STATE_OFFSETS + TRELLIS_Y_OFFSET<< "\" ";
            trellis_fname << "x2 =\"" <<  STAGE_STATE_OFFSETS *stage_num + STAGE_STATE_OFFSETS+ TRELLIS_X_OFFSET << "\" ";
            trellis_fname << "y2 =\"" << d_NS[d_I * state_num + branch_num] * STAGE_STATE_OFFSETS + TRELLIS_Y_OFFSET << "\" ";
            trellis_fname << " stroke-dasharray = \"3," <<  branch_num << "\" ";
            trellis_fname << " stroke = \"black\" stroke-width = \"0.3\"/>" << std::endl;
          }
        }
      }
    }
  // label the stages
  trellis_fname << "<g font-size = \"4\" font= \"times\" fill = \"black\">" << std::endl;
  for( int stage_num = 0;stage_num < number_stages ;stage_num ++){
    trellis_fname << "<text x = \"" << stage_num * STAGE_STATE_OFFSETS + STAGE_LABEL_X_OFFSET << 
      "\" y = \""  << STAGE_LABEL_Y_OFFSET  << "\" >" << std::endl;
    trellis_fname << stage_num <<  std::endl;
    trellis_fname << "</text>" << std::endl;
  }
  trellis_fname << "</g>" << std::endl;

  // label the states
  trellis_fname << "<g font-size = \"4\" font= \"times\" fill = \"black\">" << std::endl;
  for( int state_num = 0;state_num < d_S ; state_num ++){
    trellis_fname << "<text y = \"" << state_num * STAGE_STATE_OFFSETS + STATE_LABEL_Y_OFFSET << 
      "\" x = \""  << STATE_LABEL_X_OFFSET  << "\" >" << std::endl;
    trellis_fname << state_num <<  std::endl;
    trellis_fname << "</text>" << std::endl;
  }
  trellis_fname << "</g>" << std::endl;


  trellis_fname << "</svg>" << std::endl;
//  std::cout << "################## END SVG TRELLIS PIC ##################### " << std::endl;
  trellis_fname.close();
}






//######################################################################
//# Write trellis specification to a text file,
//# in the same format used when reading FSM files
//######################################################################
void fsm::write_fsm_txt(std::string filename)
{
   std::ofstream trellis_fname (filename.c_str());
   if (!trellis_fname) {std::cout << "file not found " << std::endl ; exit(-1);}
   trellis_fname << d_I << ' ' << d_S << ' ' << d_O << std::endl;
   trellis_fname << std::endl;
   for(int i=0;i<d_S;i++) {
     for(int j=0;j<d_I;j++)  trellis_fname << d_NS[i*d_I+j] << ' ';
     trellis_fname << std::endl;
   }
   trellis_fname << std::endl;
   for(int i=0;i<d_S;i++) {
     for(int j=0;j<d_I;j++) trellis_fname << d_OS[i*d_I+j] << ' ';
     trellis_fname << std::endl;
   }
   trellis_fname << std::endl;
   trellis_fname.close();
}

