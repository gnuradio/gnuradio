#!/usr/bin/env python

"""
Read samples 2 from two linked synchronised USRP's and write to file formatted as binary
single-precision complex values.
Make sure you read README on how to link the two usrps

"""

from gnuradio import gr, eng_notation
from gnuradio import audio
from gnuradio import usrp
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys

import time
from gnuradio import usrp_multi

class my_graph(gr.flow_graph):

    def __init__(self):
        gr.flow_graph.__init__(self)

        usage="%prog: [options] output_filename"
        parser = OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=(0, 0),
                          help="select USRP Rx side A or B (default=A)")
        parser.add_option("-d", "--decim", type="int", default=128,
                          help="set selfpa decimation rate to DECIM [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=0.0,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-n", "--nchan", type="intx", default=2,
                          help="set nchannels to NCHAN")
        parser.add_option("-N", "--nsamples", type="eng_float", default=None,
                          help="number of samples to collect [default=+inf]")
        parser.add_option ("-o", "--output-file-m", default="usrp_rx_m.dat",
                       help="write master data to FILE", metavar="FILE")
        parser.add_option ("--output-file-m2", default="usrp_rx_m2.dat",
                       help="write master data from second channel to FILE", metavar="FILE")
        parser.add_option ("-p", "--output-file-s", default="usrp_rx_s.dat",
                       help="write slave data to FILE", metavar="FILE")
        parser.add_option ("--output-file-s2", default="usrp_rx_s2.dat",
                       help="write slave data from second channel to FILE", metavar="FILE")
        parser.add_option("-x", "--master-serialno", type="string", default=None, 
                          help="Serial_no of the usrp which should be the MASTER (default= select any)")
        (options, args) = parser.parse_args ()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        if options.freq is None:
            parser.print_help()
            sys.stderr.write('You must specify the frequency with -f FREQ\n');
            raise SystemExit, 1

        # build the graph
        self.multi=usrp_multi.multi_source_align( fg=self, master_serialno=options.master_serialno, decim=options.decim, 
                                                  nchan=options.nchan )
        self.um=self.multi.get_master_usrp()
        self.us=self.multi.get_slave_usrp()

        dst_m=gr.file_sink (gr.sizeof_gr_complex, options.output_file_m)
        dst_s=gr.file_sink (gr.sizeof_gr_complex, options.output_file_s)
        if options.nsamples is None:
            self.connect((self.multi.get_master_source_c(),1),dst_m)
            self.connect((self.multi.get_slave_source_c(),1),dst_s)
        else:
            head_m = gr.head(gr.sizeof_gr_complex, int(options.nsamples))
            head_s = gr.head(gr.sizeof_gr_complex, int(options.nsamples))
            self.connect((self.multi.get_master_source_c(),1),head_m,dst_m)
            self.connect((self.multi.get_slave_source_c(),1),head_s,dst_s)

        if(4==options.nchan):
            dst_m2=gr.file_sink (gr.sizeof_gr_complex, options.output_file_m2)
            dst_s2=gr.file_sink (gr.sizeof_gr_complex, options.output_file_s2)
            if options.nsamples is None:
                self.connect((self.multi.get_master_source_c(),2),dst_m2)
                self.connect((self.multi.get_slave_source_c(),2),dst_s2) 
            else:
                head_m2 = gr.head(gr.sizeof_gr_complex, int(options.nsamples))
                head_s2 = gr.head(gr.sizeof_gr_complex, int(options.nsamples))
                self.connect((self.multi.get_master_source_c(),2),head_m2,dst_m2)
                self.connect((self.multi.get_slave_source_c(),2),head_s2,dst_s2) 

        if options.rx_subdev_spec is None:
            options.rx_subdev_spec = usrp.pick_rx_subdevice(self.um)

        if (options.nchan!=4):
          mux=usrp.determine_rx_mux_value(self.um, options.rx_subdev_spec)
          mux= (mux<<8 & 0xffffffff) | (mux & 0xff)
          self.um.set_mux(mux)
          self.us.set_mux(mux)

        # determine the daughterboard subdevice we're using
        self.subdevm = usrp.selected_subdev(self.um, options.rx_subdev_spec)
        self.subdevs = usrp.selected_subdev(self.us, options.rx_subdev_spec)
        print "Using MASTER RX d'board %s" % (self.subdevm.side_and_name(),)
        print "Using SLAVE RX d'board %s" % (self.subdevs.side_and_name(),)
        input_rate = self.um.adc_freq() / self.um.decim_rate()
        print "USB sample rate %s" % (eng_notation.num_to_str(input_rate))

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.subdevm.gain_range()
            options.gain = float(g[0]+g[1])/2

        self.multi.set_gain_all_rx(options.gain)
        result,r1,r2,r3,r4 = self.multi.tune_all_rx(options.freq)
        if not result:
            sys.stderr.write('Failed to set frequency\n')
            raise SystemExit, 1

    def sync_usrps(self):
        self.multi.sync()   
       
        
if __name__ == '__main__':
    fg=my_graph()
    fg.start()
    #time.sleep(0.5)
    fg.sync_usrps() 
    raw_input ('Press Enter to quit: ')
    fg.stop()
    #try:
    #    fg.start()
    #    fg.sync_usrps()
    #except KeyboardInterrupt:
    #    pass
