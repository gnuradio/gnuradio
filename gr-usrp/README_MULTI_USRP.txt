Multi usrp

With this code you can connect two or more usrps (with a locked clock) and get synchronised samples.
You must connect a (flat)cable between a dboard on the master in RXA and a dboard on the slave in RXA.
You then put one usrp in master mode, put the other in slave mode.

For a quick start using the examples look at gnuradio-examples/python/multi_usrp/README

Use the usrp_multi block which is installed by gr-usrp.
instantiate in the following way:

        self.multi=usrp_multi.multi_source_align( fg=self, master_serialno=options.master_serialno, decim=options.decim, nchan=options.nchan )

nchan should be 2 or 4.

You determine which is the master by master_serialno (this is a text string a hexadecimal number).
If you enter a serial number which is not found it will print the serial numbers which are available.
If you give no serial number  (master_serialno=None), the code will pick a Master for you.

You can get a reference to the master and the slave usrp in the following way:

        self.um=self.multi.get_master_usrp()
        self.us=self.multi.get_slave_usrp()

You only need these references for setting freqs/gains or getting info about daughterboards.
Don't use the output directly but use the aligned output from multi.get_master_source_c() and multi.get_slave_source_c()

You get references to the aligned output samples in the following way:
aligned_master_source_c=self.multi.get_master_source_c()
aligned_slave_source_c=self.multi.get_slave_source_c()

These blocks have multiple outputs.
output 0 is the sample counter (high bits in I, low bits in Q)
You normally don't need the samplecounters so you can ignore output 0

output 1 is the first aligend output channel (if you enable 2 or 4 channels)
output 2 is the second output channel (only if you enable 4 channels)

so the usefull 4 channels are:
self.aligned_master_chan1=(self.multi.get_master_source_c(),1)
self.aligned_master_chan2=(self.multi.get_master_source_c(),2)
self.aligned_slave_chan1=(self.multi.get_slave_source_c(),1)
self.aligned_slave_chan2=(self.multi.get_slave_source_c(),2)

The two samplecounters are:
self.aligned_master_samplecounter=(self.multi.get_master_source_c(),0)
self.aligned_slave_samplecounter=(self.multi.get_slave_source_c(),0)

You can set the gain or tune the frequency for all 4 receive daughetrboards at once:
        self.multi.set_gain_all_rx(options.gain)
        result,r1,r2,r3,r4 = self.multi.tune_all_rx(options.freq)

This will only work reliably when you have all the same daughterboards.
Otherwise set all freqs and gains individually.

You must call self.multi.sync() at least once AFTER the flowgraph has started running.
(This will synchronise the streams of the two usrps)

This work was funded by Toby Oliver at Sensus Analytics / Path Intelligence.
Many Thanks for making this possible.

It was written by Martin Dudok van Heel at Olifantasia.

Quick start multi-usrp:
Unpack, build and install usrp, gnuradio-core and gr-usrp
Versions need to be more recent then 2.7cvs/svn 11 may 2006

Make sure usrp/fpga/rbf/rev2/multi*.rbf is installed in /usr/local/share/usrp/rev2/
Make sure usrp/fpga/rbf/rev4/multi*.rbf is installed in /usr/local/share/usrp/rev4/
(If in doubt, copy manually)
 
build and install gr-wxgui gr-audio-xxx  and so on.

unpack gnuradio-examples.

There is a gnuradio-examples/python/multi_usrp directory which contains examples and a README


Put at least a basic RX or dbsrx board in RXA of the master and RXA of the slave board.
Make sure that the usrps have a serial or unique identifier programmed in their eeprom.
(All new rev 4.1 boards have this)
You can do without a serial but then you never know which usrp is the master and which is the slave.


CONNECTING THE CABLES
Now connect the 64MHz clocks between the boards with a short sma coax cable.
(See the wiki on how to enable clock-out and clock-in 
http://comsec.com/wiki?USRPClockingNotes )

You need one board with a clock out and one board with a clock in.

You can choose any of the two boards as master or slave, this is not dependant on which board has the clock-out or in.
In my experiments I had fewer problems when the board that has the clock-in will be the master board.

You can use a standard 16-pole flatcable to connect tvrx, basic-rx or dbsrx boards.
Of this 16pin flatcable only two pins are used (io15 and ground)
For all new daughterboards which use up a lot of io pins you have to use a cable with fewer connections.
The savest is using a 2pin headercable connected to io15,gnd (a cable like the ones used to connect frontpanel leds to the mainboard of a PC)

If using basic rx board:
  Connect a 16-pole flatcable from J25 on basicrx/dbs_rx in rxa of the master usrp to J25 on basicrx/dbsrx in RXA of the slave usrp
  Don't twist the cable (Make sure the pin1 marker (red line on the flatcable) is on the same side of the connector (at io-8 on the master and at io8 on the slave.))
  For basic_rx this means the marker should be on the side of the dboard with the sma connectors. 
  For dbs_rx this means the marker should be on the side of the dboard with the two little chips.
  In other words, don't twist the cable, you will burn your board if you do.

You can also connect a flatcable with multiple connectors from master-J25 to slave1-J25 to slave2-J25 to ...
You will however have to think of something to create a common 64Mhz clock for more then two usrps.

For all other daughterboards, connect a 2wire cable from masterRXA J25 io15,gnd to slaveRXA J25 io15,gnd 


So now the hardware is setup, software is setup. Lets do some tests.

Connect power to both usrps.
unpack the gnuradio_examples somewhere (cvs version later then 11 may 2006) 
go to the gnuradio-examples/python/multi_usrp folder.

Now run 
 ./multi_usrp_oscope.py -x 12345678

It should tell you that usrp 12345678 is not found and tell you which serials are available.

Now run  ./multi_usrp_oscope.py -x actualserialnum 
You should now get an oscope with two channels, one is from the master and one is from the slave
It will which show the I-signal from channel 0 of the master usrp and I-signal from channel 0 of the slave usrp.
(For testing connect the same signal source to the inputs of both boards)
The signals should be aligned.
If you click the sync button, it will resync the master and slave (should never be needed)

Now run
./multi_usrp_oscope.py --help
To see all available options.


Now you are ready to do phase-locked aligned signal processing.

You can also capture to file with:
./multi_usrp_rx_cfile.py 

run ./multi_usrp_rx_cfile.py --help to see all available options.


Here follows a brief of the new blocks and (changes)functionality for multi-usrp.
You can also look at the generated documentation in  
/usr/local/share/doc/gnuradio-core-X.X
/usr/local/share/doc/usrp-X.X
(Make sure to build and install the documentation, go to the doc directory of the sourcetree and issue make doc; make install)
 

gnuradio-examples:
new/changed files:
multi_usrp/multi_usrp_oscope.py
multi_usrp/multi_usrp_rx_cfile.py


gnuradio-core:
gr.align_on_samplenumbers_ss (int nchan,int align_interval) 

align several complex short (interleaved short) input channels with corresponding unsigned 32 bit sample_counters (provided as interleaved 16 bit values)

Parameters:
    	nchan 	number of complex_short input channels (including the 32 bit counting channel)
    	align_interval 	interval at which the samples are aligned, ignored for now.

Pay attention on how you connect this block It expects a minimum of 2 usrp_source_s with nchan number of channels and as mode  usrp_prims.bmFR_MODE_RX_COUNTING_32BIT enabled. This means that the first complex_short channel is an interleaved 32 bit counter. The samples are aligned by dropping samples untill the samplenumbers match.

files:
gnuradio-core/src/lib/general/gr_align_on_samplenumbers_ss.cc
gnuradio-core/src/lib/general/gr_align_on_samplenumbers_ss.h
gnuradio-core/src/lib/general/gr_align_on_samplenumbers_ss.i


gr-usrp
   added _write_fpga_reg_masked
   added usrp_multi.py
  new usrp_multi block which can instantiate two linked usrps as master and slave and alignes their output.
  It has a sync() function which should be called AFTER the flowgraph has started running.
  bool sync();
     \brief Call this on a master usrp to sync master and slave by outputing a sync pulse on rx_a_io[15].
        The 32 bit samplecounter of master and slave will be reset to zero and all phase and buffer related things in the usrps are reset.
        Call this only after the flowgraph has been started, otherwise there will be no effect since everything is kept in reset state as long as the flowgraph is not running.
     \returns true if successfull.

files:
configure.ac
src/Makefile.am
src/usrp1.i
src/usrp1_source_base.cc
src/usrp1_source_base.h
src/usrp_multi.py

usrp-0.11cvsmulti:
usrp:
   new constant bmFR_MODE_RX_COUNTING_32BIT    (could also be added as extra mode like FPGA_MODE_COUNTING_32BIT)
   Use this for the mode parameter when creating a usrp when you want to use the master/slave setup or if you want to use the 32 bit counter for other things, like testing with gr.check_counting_s(True)

  added register FR_RX_MASTER_SLAVE
  added bitno and bitmaskes:
   bmFR_MODE_RX_COUNTING_32BIT

   bitnoFR_RX_SYNC 
   bitnoFR_RX_SYNC_MASTER
   bitnoFR_RX_SYNC_SLAVE

   bitnoFR_RX_SYNC_INPUT_IOPIN 15
   bmFR_RX_SYNC_INPUT_IOPIN  (1<<bitnoFR_RX_SYNC_INPUT_IOPIN)
   bitnoFR_RX_SYNC_OUTPUT_IOPIN 15
   bmFR_RX_SYNC_OUTPUT_IOPIN (1<<bitnoFR_RX_SYNC_OUTPUT_IOPIN)
 
   added _write_fpga_reg_masked()
   added new toplevel folder usrp_multi
   added usrp_multi.v and master_control_multi.v
   added new MULTI_ON and COUNTER_32BIT_ON defines
      If these are turned off usrp_multi.v will behave exactly as usrp_std.v

   added setting_reg_masked.v
   changed reset behaviour of phase_acc.v and rx_buffer.v

   changed generate_regs.py to handle bm and bitno defines


files:
firmware/include/fpga_regs_standard.v
firmware/include/fpga_regs_common.h
firmware/include/generate_regs.py
firmware/include/fpga_regs_standard.h
host/lib/usrp_basic.h
host/lib/usrp_basic.cc
host/lib/usrp_standard.h
fpga/rbf/Makefile.am
fpga/toplevel/usrp_std/usrp_std.v
fpga/toplevel/usrp_multi/usrp_multi.esf
fpga/toplevel/usrp_multi/usrp_multi.vh
fpga/toplevel/usrp_multi/usrp_std.vh
fpga/toplevel/usrp_multi/usrp_multi_config_2rxhb_0tx.vh
fpga/toplevel/usrp_multi/usrp_multi_config_2rxhb_2tx.vh
fpga/toplevel/usrp_multi/usrp_multi.v
fpga/toplevel/usrp_multi/usrp_multi.qpf
fpga/toplevel/usrp_multi/usrp_multi.psf
fpga/toplevel/usrp_multi/usrp_multi_config_2rx_0tx.vh
fpga/toplevel/usrp_multi/usrp_multi.qsf
fpga/toplevel/usrp_multi/usrp_multi_config_4rx_0tx.vh
fpga/toplevel/usrp_multi/usrp_multi.csf
fpga/toplevel/usrp_multi/.cvsignore
fpga/sdr_lib/rx_buffer.v
fpga/sdr_lib/master_control_multi.v
fpga/sdr_lib/phase_acc.v
fpga/sdr_lib/setting_reg_masked.v


