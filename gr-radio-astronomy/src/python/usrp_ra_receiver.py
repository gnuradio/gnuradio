#!/usr/bin/env python
#
# Copyright 2004,2005 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from gnuradio import gr, gru
from gnuradio import usrp
import usrp_dbid
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui, ra_fftsink, ra_stripchartsink, waterfallsink, form, slider
from optparse import OptionParser
import wx
import sys
import Numeric 
import time
import FFT
import ephem

class continuum_calibration(gr.feval_dd):
    def eval(self, x):
        str = globals()["calibration_codelet"]
        exec(str)
        return(x)

class app_flow_graph(stdgui.gui_flow_graph):
    def __init__(self, frame, panel, vbox, argv):
        stdgui.gui_flow_graph.__init__(self)

        self.frame = frame
        self.panel = panel
        
        parser = OptionParser(option_class=eng_option)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=(0, 0),
                          help="select USRP Rx side A or B (default=A)")
        parser.add_option("-d", "--decim", type="int", default=16,
                          help="set fgpa decimation rate to DECIM [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ", metavar="FREQ")
	parser.add_option("-a", "--avg", type="eng_float", default=1.0,
		help="set spectral averaging alpha")
	parser.add_option("-i", "--integ", type="eng_float", default=1.0,
		help="set integration time")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-l", "--reflevel", type="eng_float", default=30.0,
                          help="Set Total power reference level")
        parser.add_option("-y", "--division", type="eng_float", default=0.5,
                          help="Set Total power Y division size")
        parser.add_option("-e", "--longitude", type="eng_float", default=-76.02,                          help="Set Observer Longitude")
        parser.add_option("-c", "--latitude", type="eng_float", default=44.85,                          help="Set Observer Latitude")
        parser.add_option("-o", "--observing", type="eng_float", default=0.0,
                        help="Set observing frequency")
        parser.add_option("-x", "--ylabel", default="dB", help="Y axis label") 
        parser.add_option("-z", "--divbase", type="eng_float", default=0.025, help="Y Division increment base") 
        parser.add_option("-v", "--stripsize", type="eng_float", default=2400, help="Size of stripchart, in 2Hz samples") 
        parser.add_option("-F", "--fft_size", type="eng_float", default=1024, help="Size of FFT")

        parser.add_option("-N", "--decln", type="eng_float", default=999.99, help="Observing declination")
        parser.add_option("-X", "--prefix", default="./")
        parser.add_option("-M", "--fft_rate", type="eng_float", default=8.0, help="FFT Rate")
        parser.add_option("-A", "--calib_coeff", type="eng_float", default=1.0, help="Calibration coefficient")
        parser.add_option("-B", "--calib_offset", type="eng_float", default=0.0, help="Calibration coefficient")
        parser.add_option("-Q", "--calib_eqn", default="x = x * 1.0", help="Calibration equation")
        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)

        self.show_debug_info = True
        
        # Calibration coefficient and offset
        self.calib_coeff = options.calib_coeff
        self.calib_offset = options.calib_offset

        self.calib_eqn = options.calib_eqn
        globals()["calibration_codelet"] = self.calib_eqn

        self.integ = options.integ
        self.avg_alpha = options.avg
        self.gain = options.gain
        self.decln = options.decln

        # Set initial values for datalogging timed-output
        self.continuum_then = time.time()
        self.spectral_then = time.time()
      
        # build the graph

        self.u = usrp.source_c(decim_rate=options.decim)
        self.u.set_mux(usrp.determine_rx_mux_value(self.u, options.rx_subdev_spec))
        self.cardtype = self.u.daughterboard_id(0)
        # Set initial declination
        self.decln = options.decln

        # determine the daughterboard subdevice we're using
        self.subdev = usrp.selected_subdev(self.u, options.rx_subdev_spec)

        input_rate = self.u.adc_freq() / self.u.decim_rate()

        #
        # Set prefix for data files
        #
        self.prefix = options.prefix

        #
        # The lower this number, the fewer sample frames are dropped
        #  in computing the FFT.  A sampled approach is taken to
        #  computing the FFT of the incoming data, which reduces
        #  sensitivity.  Increasing sensitivity inreases CPU loading.
        #
        self.fft_rate = options.fft_rate

        self.fft_size = options.fft_size

        # This buffer is used to remember the most-recent FFT display
        #   values.  Used later by self.write_spectral_data() to write
        #   spectral data to datalogging files.
        self.fft_outbuf = Numeric.zeros(options.fft_size, Numeric.Float64)

        # Set up FFT display
        self.scope = ra_fftsink.ra_fft_sink_c (self, panel, 
           fft_size=int(self.fft_size), sample_rate=input_rate,
           fft_rate=int(self.fft_rate), title="Spectral",  
           ofunc=self.fft_outfunc, xydfunc=self.xydfunc)

        # Set up ephemeris data
        self.locality = ephem.Observer()
        self.locality.long = str(options.longitude)
        self.locality.lat = str(options.latitude)

        # Set up stripchart display
        self.stripsize = int(options.stripsize)
        self.chart = ra_stripchartsink.stripchart_sink_f (self, panel,
            stripsize=self.stripsize,
            title="Continuum",
            xlabel="LMST Offset (Seconds)",
            scaling=1.0, ylabel=options.ylabel,
            divbase=options.divbase)

        # Set center frequency
        self.centerfreq = options.freq

        # Set observing frequency (might be different from actual programmed
        #    RF frequency)
        if options.observing == 0.0:
            self.observing = options.freq
        else:
            self.observing = options.observing

        self.bw = input_rate

        # We setup the first two integrators to produce a fixed integration
        # Down to 1Hz, with output at 1 samples/sec
        N = input_rate/5000

        # Second stage runs on decimated output of first
        M = (input_rate/N)

        # Create taps for first integrator
        t = range(0,N-1)
        tapsN = []
        for i in t:
             tapsN.append(1.0/N)

        # Create taps for second integrator
        t = range(0,M-1)
        tapsM = []
        for i in t:
            tapsM.append(1.0/M)

        #
        # The 3rd integrator is variable, and user selectable at runtime
        # This integrator doesn't decimate, but is used to set the
        #  final integration time based on the constant 1Hz input samples
        # The strip chart is fed at a constant 1Hz rate as a result
        #

        #
        # Call constructors for receive chains
        #

        # The three integrators--two FIR filters, and an IIR final filter
        self.integrator1 = gr.fir_filter_fff (N, tapsN)
        self.integrator2 = gr.fir_filter_fff (M, tapsM)
        self.integrator3 = gr.single_pole_iir_filter_ff(1.0)

        # Split complex USRP stream into a pair of floats
        self.splitter = gr.complex_to_float (1);

        # I squarer (detector)
        self.multI = gr.multiply_ff();

        # Q squarer (detector)
        self.multQ = gr.multiply_ff();

        # Adding squared I and Q to produce instantaneous signal power
        self.adder = gr.add_ff();

        # Signal probe
        self.probe = gr.probe_signal_f();

        #
        # Continuum calibration stuff
        #
        self.cal_mult = gr.multiply_const_ff(self.calib_coeff);
        self.cal_offs = gr.add_const_ff(self.calib_offset);

        #self.cal_eqn = continuum_calibration();

        #
        # Start connecting configured modules in the receive chain
        #
        self.connect(self.u, self.scope)
        self.connect(self.u, self.splitter)

        # Connect splitter outputs to multipliers
        # First do I^2
        self.connect((self.splitter, 0), (self.multI,0))
        self.connect((self.splitter, 0), (self.multI,1))

        # Then do Q^2
        self.connect((self.splitter, 1), (self.multQ,0))
        self.connect((self.splitter, 1), (self.multQ,1))

        # Then sum the squares
        self.connect(self.multI, (self.adder,0))
        self.connect(self.multQ, (self.adder,1))

        # Connect adder output to two-stages of FIR integrator
        #   followed by a single stage IIR integrator, and
        #   the calibrator
        self.connect(self.adder, self.integrator1, 
           self.integrator2, self.integrator3, self.cal_mult, 
           self.cal_offs, self.chart)

        # Connect calibrator to probe
        # SPECIAL NOTE:  I'm setting the ground work here
        #   for completely changing the way local_calibrator
        #   works, including removing some horrible kludges for
        #   recording data.
        # But for now, self.probe() will be used to display the
        #  current instantaneous integrated detector value
        self.connect(self.cal_offs, self.probe)

        self._build_gui(vbox)

        # Make GUI agree with command-line
        self.integ = options.integ
        self.myform['integration'].set_value(int(options.integ))
        self.myform['average'].set_value(int(options.avg))

        # Make integrator agree with command line
        self.set_integration(int(options.integ))

        self.avg_alpha = options.avg

        # Make spectral averager agree with command line
        if options.avg != 1.0:
            self.scope.set_avg_alpha(float(1.0/options.avg))
            self.scope.set_average(True)

        # Set division size
        self.chart.set_y_per_div(options.division)

        # Set reference(MAX) level
        self.chart.set_ref_level(options.reflevel)

        # set initial values

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.subdev.gain_range()
            options.gain = float(g[0]+g[1])/2

        if options.freq is None:
            # if no freq was specified, use the mid-point
            r = self.subdev.freq_range()
            options.freq = float(r[0]+r[1])/2

        # Set the initial gain control
        self.set_gain(options.gain)

        if not(self.set_freq(options.freq)):
            self._set_status_msg("Failed to set initial frequency")

        # Set declination
        self.set_decln (self.decln)


        # RF hardware information
        self.myform['decim'].set_value(self.u.decim_rate())
        self.myform['fs@usb'].set_value(self.u.adc_freq() / self.u.decim_rate())
        self.myform['dbname'].set_value(self.subdev.name())

        # Set analog baseband filtering, if DBS_RX
        if self.cardtype == usrp_dbid.DBS_RX:
            lbw = (self.u.adc_freq() / self.u.decim_rate()) / 2
            if lbw < 1.0e6:
                lbw = 1.0e6
            self.subdev.set_bw(lbw)

        # Start the timer for the LMST display and datalogging
        self.lmst_timer.Start(1000)


    def _set_status_msg(self, msg):
        self.frame.GetStatusBar().SetStatusText(msg, 0)

    def _build_gui(self, vbox):

        def _form_set_freq(kv):
            return self.set_freq(kv['freq'])

        def _form_set_decln(kv):
            return self.set_decln(kv['decln'])

        # Position the FFT display
        vbox.Add(self.scope.win, 15, wx.EXPAND)

        # Position the Total-power stripchart
        vbox.Add(self.chart.win, 15, wx.EXPAND)
        
        # add control area at the bottom
        self.myform = myform = form.form()
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((7,0), 0, wx.EXPAND)
        vbox1 = wx.BoxSizer(wx.VERTICAL)
        myform['freq'] = form.float_field(
            parent=self.panel, sizer=vbox1, label="Center freq", weight=1,
            callback=myform.check_input_and_call(_form_set_freq, self._set_status_msg))

        vbox1.Add((4,0), 0, 0)

        myform['lmst_high'] = form.static_text_field(
            parent=self.panel, sizer=vbox1, label="Current LMST", weight=1)
        vbox1.Add((4,0), 0, 0)

        myform['spec_data'] = form.static_text_field(
            parent=self.panel, sizer=vbox1, label="Spectral Cursor", weight=1)
        vbox1.Add((4,0), 0, 0)

        vbox2 = wx.BoxSizer(wx.VERTICAL)
        g = self.subdev.gain_range()
        myform['gain'] = form.slider_field(parent=self.panel, sizer=vbox2, label="RF Gain",
                                           weight=1,
                                           min=int(g[0]), max=int(g[1]),
                                           callback=self.set_gain)

        vbox2.Add((4,0), 0, 0)
        myform['average'] = form.slider_field(parent=self.panel, sizer=vbox2, 
                    label="Spectral Averaging (FFT frames)", weight=1, min=1, max=2000, callback=self.set_averaging)

        vbox2.Add((4,0), 0, 0)

        myform['integration'] = form.slider_field(parent=self.panel, sizer=vbox2,
               label="Continuum Integration Time (sec)", weight=1, min=1, max=180, callback=self.set_integration)

        vbox2.Add((4,0), 0, 0)
        myform['decln'] = form.float_field(
            parent=self.panel, sizer=vbox2, label="Current Declination", weight=1,
            callback=myform.check_input_and_call(_form_set_decln))
        vbox2.Add((4,0), 0, 0)

        buttonbox = wx.BoxSizer(wx.HORIZONTAL)
        vbox.Add(buttonbox, 0, wx.CENTER)
        hbox.Add(vbox1, 0, 0)
	hbox.Add(vbox2, wx.ALIGN_RIGHT, 0)
        vbox.Add(hbox, 0, wx.EXPAND)

        self._build_subpanel(vbox)

        self.lmst_timer = wx.PyTimer(self.lmst_timeout)
        self.lmst_timeout()


    def _build_subpanel(self, vbox_arg):
        # build a secondary information panel (sometimes hidden)

        # FIXME figure out how to have this be a subpanel that is always
        # created, but has its visibility controlled by foo.Show(True/False)
        
        if not(self.show_debug_info):
            return

        panel = self.panel
        vbox = vbox_arg
        myform = self.myform

        #panel = wx.Panel(self.panel, -1)
        #vbox = wx.BoxSizer(wx.VERTICAL)

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0)
        myform['decim'] = form.static_float_field(
            parent=panel, sizer=hbox, label="Decim")

        hbox.Add((5,0), 1)
        myform['fs@usb'] = form.static_float_field(
            parent=panel, sizer=hbox, label="Fs@USB")

        hbox.Add((5,0), 1)
        myform['dbname'] = form.static_text_field(
            parent=panel, sizer=hbox)

        hbox.Add((5,0), 1)
        myform['baseband'] = form.static_float_field(
            parent=panel, sizer=hbox, label="Analog BB")

        hbox.Add((5,0), 1)
        myform['ddc'] = form.static_float_field(
            parent=panel, sizer=hbox, label="DDC")

        hbox.Add((5,0), 0)
        vbox.Add(hbox, 0, wx.EXPAND)

        
        
    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter.
        """
        #
        # Everything except BASIC_RX should support usrp.tune()
        #
        if not (self.cardtype == usrp_dbid.BASIC_RX):
            r = usrp.tune(self.u, 0, self.subdev, target_freq)
        else:
            r = self.u.set_rx_freq(0, target_freq)
            f = self.u.rx_freq(0)
            if abs(f-target_freq) > 2.0e3:
                r = 0
        if r:
            self.myform['freq'].set_value(target_freq)     # update displayed value
            #
            # Make sure calibrator knows our target freq
            #

            # Remember centerfreq---used for doppler calcs
            delta = self.centerfreq - target_freq
            self.centerfreq = target_freq
            self.observing -= delta
            self.scope.set_baseband_freq (self.observing)

            self.myform['baseband'].set_value(r.baseband_freq)
            self.myform['ddc'].set_value(r.dxc_freq)

            return True

        return False

    def set_decln(self, dec):
        self.decln = dec
        self.myform['decln'].set_value(dec)     # update displayed value

    def set_gain(self, gain):
        self.myform['gain'].set_value(gain)     # update displayed value
        self.subdev.set_gain(gain)
        self.gain = gain

    def set_averaging(self, avval):
        self.myform['average'].set_value(avval)
        self.scope.set_avg_alpha(1.0/(avval))
        self.scope.set_average(True)
        self.avg_alpha = avval

    def set_integration(self, integval):
        self.integrator3.set_taps(1.0/integval)
        self.myform['integration'].set_value(integval)
        self.integ = integval

    #
    # Timeout function
    # Used to update LMST display, as well as current
    #  continuum value
    #
    # We also write external data-logging files here
    #
    def lmst_timeout(self):
         self.locality.date = ephem.now()
         x = self.probe.level()
         sidtime = self.locality.sidereal_time()
         # LMST
         s = str(ephem.hours(sidtime))
         # Continuum detector value
         sx = "%7.4f" % x
         s = s + "\nDet: " + str(sx)
         self.myform['lmst_high'].set_value(s)

         #
         # Write data out to recording files
         #
         self.write_continuum_data(x,sidtime)
         self.write_spectral_data(self.fft_outbuf,sidtime)

    def fft_outfunc(self,data,l):
        self.fft_outbuf=data

    def write_continuum_data(self,data,sidtime):
    
        # Create localtime structure for producing filename
        foo = time.localtime()
        pfx = self.prefix
        filenamestr = "%s/%04d%02d%02d%02d" % (pfx, foo.tm_year, 
           foo.tm_mon, foo.tm_mday, foo.tm_hour)
    
        # Open the data file, appending
        continuum_file = open (filenamestr+".tpdat","a")
      
        flt = "%6.3f" % data
        inter = self.decln
        integ = self.integ
        fc = self.observing
        fc = fc / 1000000
        bw = self.bw
        bw = bw / 1000000
        ga = self.gain
    
        now = time.time()
    
        #
        # If time to write full header info (saves storage this way)
        #
        if (now - self.continuum_then > 20):
            self.continuum_then = now
        
            continuum_file.write(str(ephem.hours(sidtime))+" "+flt+" Dn="+str(inter)+",")
            continuum_file.write("Ti="+str(integ)+",Fc="+str(fc)+",Bw="+str(bw))
            continuum_file.write(",Ga="+str(ga)+"\n")
        else:
            continuum_file.write(str(ephem.hours(sidtime))+" "+flt+"\n")
    
        continuum_file.close()
        return(data)

    def write_spectral_data(self,data,sidtime):
    
        now = time.time()
        delta = 10
    		
        # If time to write out spectral data
        # We don't write this out every time, in order to
        #   save disk space.  Since the spectral data are
        #   typically heavily averaged, writing this data
        #   "once in a while" is OK.
        #
        if (now - self.spectral_then >= delta):
            self.spectral_then = now

            # Get localtime structure to make filename from
            foo = time.localtime()
        
            pfx = self.prefix
            filenamestr = "%s/%04d%02d%02d%02d" % (pfx, foo.tm_year, 
               foo.tm_mon, foo.tm_mday, foo.tm_hour)
    
            # Open the file
            spectral_file = open (filenamestr+".sdat","a")
      
            # Setup data fields to be written
            r = data
            inter = self.decln
            fc = self.observing
            fc = fc / 1000000
            bw = self.bw
            bw = bw / 1000000
            av = self.avg_alpha

            # Write those fields
            spectral_file.write("data:"+str(ephem.hours(sidtime))+" Dn="+str(inter)+",Fc="+str(fc)+",Bw="+str(bw)+",Av="+str(av))
            spectral_file.write(" "+str(r)+"\n")
            spectral_file.close()
            return(data)
    
        return(data)

    def xydfunc(self,xyv):
        magn = int(Numeric.log10(self.observing))
        if (magn == 6 or magn == 7 or magn == 8):
            magn = 6
        dfreq = xyv[0] * pow(10.0,magn)
        ratio = self.observing / dfreq
        vs = 1.0 - ratio
        vs *= 299792.0
        if magn >= 9:
           xhz = "Ghz"
        elif magn >= 6:
           xhz = "Mhz"
        elif magn <= 5:
           xhz =  "Khz"
        s = "%.6f%s\n%.3fdB" % (xyv[0], xhz, xyv[1])
        s2 = "\n%.3fkm/s" % vs
        self.myform['spec_data'].set_value(s+s2)

    def toggle_cal(self):
        if (self.calstate == True):
          self.calstate = False
          self.u.write_io(0,0,(1<<15))
          self.calibrator.SetLabel("Calibration Source: Off")
        else:
          self.calstate = True
          self.u.write_io(0,(1<<15),(1<<15))
          self.calibrator.SetLabel("Calibration Source: On")

    def toggle_annotation(self):
        if (self.annotate_state == True):
          self.annotate_state = False
          self.annotation.SetLabel("Annotation: Off")
        else:
          self.annotate_state = True
          self.annotation.SetLabel("Annotation: On")
        

def main ():
    app = stdgui.stdapp(app_flow_graph, "RADIO ASTRONOMY SPECTRAL/CONTINUUM RECEIVER: $Revision$", nstatus=1)
    app.MainLoop()

if __name__ == '__main__':
    main ()
