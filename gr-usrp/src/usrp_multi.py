#
# Copyright 2005 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

import math
from gnuradio import gr, gru
from gnuradio.gr import hier_block_base
from gnuradio import usrp
from gnuradio import usrp1              # usrp Rev 1 and later
from gnuradio import blks
from usrpm import usrp_prims
import sys


class multi_source_align(object):
    def __init__(self, fg, master_serialno,decim,nchan=2,pga_gain=0.0,cordic_freq=0.0,mux=None,align_interval=-1):
        """
        Align multiple sources (usrps) using samplenumbers in the first channel.

        Takes two ore more sources producing interleaved shorts.
				produces nchan * nsources gr_complex output streams.

        @param nchan: number of interleaved channels in source
        @param align_interval: number of samples to minimally skip between alignments
				                       default = -1 which means align only once per work call.
        @param master_serial_no: serial number of the source which must be the master.


        Exported sub-blocks (attributes):
          master_source
          slave_source
          usrp_master
          usrp_slave
        """
        mode=usrp.FPGA_MODE_NORMAL
        mode = mode | usrp_prims.bmFR_MODE_RX_COUNTING_32BIT #(1 << 2) #usrp1.FPGA_MODE_COUNTING_32BIT
        align=gr.align_on_samplenumbers_ss (nchan,align_interval) 
        self.usrp_master = None
        self.usrp_slave = None
        # um is master usrp
        # us is slave usrp
        if mux is None:
          mux=self.get_default_mux()  #Note that all channels have shifted left because of the added 32 bit counter channel
        
        u1 = usrp.source_s (1, decim, nchan, gru.hexint(mux), mode,fpga_filename="multi_2rxhb_2tx.rbf" )
        u0 = usrp.source_s (0, decim, nchan, gru.hexint(mux), mode,fpga_filename="multi_2rxhb_2tx.rbf" )
        print 'usrp[0] serial',u0.serial_number()
        print 'usrp[1] serial',u1.serial_number()
        #default, choose the second found usrp as master (which is usually the usrp which was first plugged in)
        um_index=1
        um=u1
        us_index=0
        us=u0
        if (not (master_serialno is None)): #((master_serialno>0) | (master_serialno <-2)):
          if (u0.serial_number() == master_serialno):
            um_index=0
            um=u0
            us_index=1
            us=u1
          elif (u1.serial_number() != master_serialno):              
              errorstring = 'Error. requested master_serialno ' + master_serialno +' not found\n'
              errorstring = errorstring + 'Available are:\n'
              errorstring = errorstring + 'usrp[1] serial_no = ' + u1.serial_number() +'\n' 
              errorstring = errorstring + 'usrp[0] serial_no = ' + u0.serial_number() +'\n'
              print errorstring
              raise ValueError, errorstring
          else: #default, just choose the first found usrp as master
            um_index=0
            um=u0
            us_index=1
            us=u1

        self.usrp_master=um
        self.usrp_slave=us
        print 'usrp_master=usrp[%i] serial_no = %s' % (um_index,self.usrp_master.serial_number() ,)
        print 'usrp_slave=usrp[%i] serial_no = %s' % (us_index,self.usrp_slave.serial_number() ,)
        self.subdev_mAr = usrp.selected_subdev(self.usrp_master, (0,0))
        self.subdev_mBr = usrp.selected_subdev(self.usrp_master, (1,0))
        self.subdev_sAr = usrp.selected_subdev(self.usrp_slave, (0,0))
        self.subdev_sBr = usrp.selected_subdev(self.usrp_slave, (1,0))
        #throttle = gr.throttle(gr.sizeof_gr_complex, input_rate)
        if not (pga_gain is None):
          um.set_pga (0, pga_gain)
          um.set_pga (1, pga_gain)

          us.set_pga (0, pga_gain)
          us.set_pga (1, pga_gain)

        self.input_rate = um.adc_freq () / um.decim_rate ()
        deintm=gr.deinterleave(gr.sizeof_gr_complex) 
        deints=gr.deinterleave(gr.sizeof_gr_complex)
        nullsinkm=gr.null_sink(gr.sizeof_gr_complex)
        nullsinks=gr.null_sink(gr.sizeof_gr_complex)

        tocomplexm=gr.interleaved_short_to_complex()
        tocomplexs=gr.interleaved_short_to_complex()

        fg.connect(um,(align,0))
        fg.connect(us,(align,1))
        fg.connect((align,0),tocomplexm)
        fg.connect((align,1),tocomplexs)
        fg.connect(tocomplexm,deintm)
        fg.connect(tocomplexs,deints)
        fg.connect((deintm,0),nullsinkm) #The counters are not usefull for the user but must be connected to something
        fg.connect((deints,0),nullsinks) #The counters are not usefull for the user but must be connected to something
        if 4==nchan:
          nullsinkm3=gr.null_sink(gr.sizeof_gr_complex)
          nullsinks3=gr.null_sink(gr.sizeof_gr_complex)
          fg.connect((deintm,3), nullsinkm3) #channel 4 is not used but must be connected
          fg.connect((deints,3), nullsinks3) #channel 4 is not used but must be connected

        self.fg=fg
        self.master_source=deintm
        self.slave_source=deints

        if not (cordic_freq is None):
          um.set_rx_freq (1, cordic_freq)
          um.set_rx_freq (0, cordic_freq)
          us.set_rx_freq (1, cordic_freq)
          us.set_rx_freq (0, cordic_freq)

        self.enable_master_and_slave()
        # add an idle handler
        self.unsynced=True

        # wire the block together
        #hier_block_multi_tail.__init__(self, fg, nchan,deintm,deints)
 
    def get_default_mux(self):
        return 0x10321032     # Note that all channels have shifted left because of the added 32 bit counter channel  

    def get_master_source_c(self):
        return self.master_source

    def get_slave_source_c(self):
        return self.slave_source

    def get_master_usrp(self):
        return self.usrp_master

    def get_slave_usrp(self):
        return self.usrp_slave

    def enable_master_and_slave(self):
        # Warning, allways FIRST enable the slave before you enable the master
        # This is to be sure you don't have two masters connecting to each other
        # Otherwise you could ruin your hardware because the two sync outputs would be connected together

        #SLAVE
        #disable master, enable slave and set sync pulse to zero
        reg_mask = usrp_prims.bmFR_RX_SYNC_SLAVE | usrp_prims.bmFR_RX_SYNC_MASTER | usrp_prims.bmFR_RX_SYNC
        self.usrp_slave._u._write_fpga_reg_masked(usrp_prims.FR_RX_MASTER_SLAVE, usrp_prims.bmFR_RX_SYNC_SLAVE,reg_mask)
        #set SYNC slave iopin on daughterboards RXA as input
        oe = 0 # set rx_a_io[bitnoFR_RX_SYNC_INPUT_IOPIN] as input
        oe_mask = usrp_prims.bmFR_RX_SYNC_INPUT_IOPIN 
        self.usrp_slave._u._write_oe(0,oe,oe_mask)
        #Now it is save to enable the master

        #MASTER
        #enable master, disable slave and set sync pulse to zero
        reg_mask = usrp_prims.bmFR_RX_SYNC_SLAVE | usrp_prims.bmFR_RX_SYNC_MASTER | usrp_prims.bmFR_RX_SYNC
        self.usrp_master._u._write_fpga_reg_masked(usrp_prims.FR_RX_MASTER_SLAVE,usrp_prims.bmFR_RX_SYNC_MASTER,reg_mask)
        #set SYNC master iopin on daughterboards RXA as output
        oe = usrp_prims.bmFR_RX_SYNC_OUTPUT_IOPIN # set rx_a_io[bitnoFR_RX_SYNC_OUTPUT_IOPIN] as output
        oe_mask = usrp_prims.bmFR_RX_SYNC_OUTPUT_IOPIN 
        self.usrp_master._u._write_oe(0,oe,oe_mask)

    def sync_usrps(self, evt):
        self.sync()

    def sync(self):
        result=False
        result = self.usrp_master._u._write_fpga_reg_masked (usrp_prims.FR_RX_MASTER_SLAVE, usrp_prims.bmFR_RX_SYNC, usrp_prims.bmFR_RX_SYNC )
        #There should be a small delay here, but the time it takes to get the sync to the usrp is long enough
        #turn sync pulse off
        result  = result & self.usrp_master._u._write_fpga_reg_masked (usrp_prims.FR_RX_MASTER_SLAVE,0 ,usrp_prims.bmFR_RX_SYNC);
        return result;

    def nullsink_counters(self):
        nullsinkm=gr.null_sink(gr.sizeof_gr_complex)
        nullsinks=gr.null_sink(gr.sizeof_gr_complex)
        self.fg.connect((self.master_source,0),nullsinkm)
        self.fg.connect((self.slave_source,0),nullsinks)


    def print_db_info(self):
        print "MASTER RX d'board %s" % (self.subdev_mAr.side_and_name(),)
        print "MASTER RX d'board %s" % (self.subdev_mBr.side_and_name(),)
        #print "TX d'board %s" % (self.subdev_At.side_and_name(),)
        #print "TX d'board %s" % (self.subdev_Bt.side_and_name(),)
        print "SLAVE RX d'board %s" % (self.subdev_sAr.side_and_name(),)
        print "SLAVE RX d'board %s" % (self.subdev_sBr.side_and_name(),)
        #print "TX d'board %s" % (self.subdev_At.side_and_name(),)
        #print "TX d'board %s" % (self.subdev_Bt.side_and_name(),)

    def tune_all_rx(self,target_freq):
        result = True
        r1 =  usrp.tune(self.usrp_master, 0, self.subdev_mAr, target_freq)
        if r1 is None:
          result=False
        r2 = usrp.tune(self.usrp_master, 1, self.subdev_mBr, target_freq)
        if r2 is None:
          result=False
        r3 = usrp.tune(self.usrp_slave, 0, self.subdev_sAr, target_freq)
        if r3 is None:
          result=False
        r4 = usrp.tune(self.usrp_slave, 1, self.subdev_sBr, target_freq)
        if r4 is None:
          result=False
        return result,r1,r2,r3,r4

    def set_gain_all_rx(self, gain):
        self.subdev_mAr.set_gain(gain)
        self.subdev_mBr.set_gain(gain)
        self.subdev_sAr.set_gain(gain)
        self.subdev_sBr.set_gain(gain)
