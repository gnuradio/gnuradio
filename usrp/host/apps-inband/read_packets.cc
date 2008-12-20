/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <usrp_inband_usb_packet.h>
#include <mblock/class_registry.h>
#include <vector>
#include <usrp_usb_interface.h>
#include <fstream>

typedef usrp_inband_usb_packet transport_pkt;   // makes conversion to gigabit easy

int main(int argc, char *argv[]) {

  if(argc !=2) {
    std::cout << "Usage: ./read_packets <data_file>\n";
    return -1;
  }

  std::ifstream infile;
  std::ofstream outfile;  

  unsigned int pkt_size = transport_pkt::max_pkt_size();
  unsigned int pkt_num=0;

  transport_pkt *pkt;
  char pkt_data[pkt_size];          // allocate the number of bytes for a single packet

  pkt = (transport_pkt *)pkt_data;  // makes operations cleaner to read

  // Open the file and read the packets, dumping information
  infile.open(argv[1], std::ios::binary|std::ios::in);
  if(!infile.is_open())
    exit(-1);

  //outfile.open("dump.dat",std::ios::out|std::ios::binary);  

  // read 1 packet in to the memory
  infile.read(pkt_data, pkt_size);

  while(!infile.eof()) {
  
    printf("Packet %u\n", pkt_num);

    if(pkt->start_of_burst())
      printf("\tstart of burst\n");
      
    if(pkt->end_of_burst())
      printf("\tend of burst\n");
    
//    if(pkt->carrier_sense())
//      printf("\tcarrier sense\n");

    if(pkt->underrun())
      printf("\tunderrun\n");
    
    if(pkt->overrun())
      printf("\toverrun\n");

    printf("\tchannel: \t0x%x\n", pkt->chan());
    printf("\ttimestamp: \t0x%x\n", pkt->timestamp());
    //printf("\ttimestamp: \t%u\n", pkt->timestamp());
    printf("\tlength: \t%u\n", pkt->payload_len());
    printf("\trssi: \t%u\n", pkt->rssi());

    printf("\tpayload: \n");
    for(int i=0; i < pkt->payload_len(); i++)
    //for(int i=0; i < pkt->max_payload(); i++)
    {
      printf("\t%d\t0x%x\n", i, *(pkt->payload()+i));
      //outfile.write((const char*)(pkt->payload()+i),1);
      //printf("\t\t0x%x\n", pkt->payload()+i);

    }
    printf("\n\n");

    pkt_num++;
  
    // read 1 packet in to the memory
    infile.read(pkt_data, pkt_size);

  }

  infile.close();
  //outfile.close();

}
