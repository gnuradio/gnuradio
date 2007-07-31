#!/usr/bin/env python
#
# Copyright 2004,2005 Free Software Foundation, Inc.
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

from gnuradio import gr, gru
from gnuradio import usrp
from usrpm import usrp_dbid
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui, ra_fftsink, ra_stripchartsink, ra_waterfallsink, form, slider
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
        parser.add_option("-W", "--waterfall", action="store_true", default=False, help="Use Waterfall FFT display")
        parser.add_option("-S", "--setimode", action="store_true", default=False, help="Enable SETI processing of spectral data")
        parser.add_option("-K", "--setik", type="eng_float", default=1.5, help="K value for SETI analysis")
        parser.add_option("-T", "--setibandwidth", type="eng_float", default=12500, help="Instantaneous SETI observing bandwidth--must be divisor of 250Khz")
        parser.add_option("-n", "--notches", action="store_true", default=False,
            help="Notches appear after all other arguments")
        (options, args) = parser.parse_args()

        self.notches = Numeric.zeros(64,Numeric.Float64)
        if len(args) != 0 and options.notches == False:
            parser.print_help()
            sys.exit(1)

        if len(args) == 0 and options.notches != False:
            parser.print_help()
            sys.exit()

        self.use_notches = options.notches

        # Get notch locations
        j = 0
        for i in args:
            self.notches[j] = float(i)
            j = j+1

        self.notch_count = j

        self.show_debug_info = True

        # Pick up waterfall option
        self.waterfall = options.waterfall

        # SETI mode stuff
        self.setimode = options.setimode
        self.seticounter = 0
        self.setik = options.setik
        # Because we force the input rate to be 250Khz, 12.5Khz is
        #  exactly 1/20th of this, which makes building decimators
        #  easier.
        # This also allows larger FFTs to be used without totally-gobbling
        #  CPU.  With an FFT size of 16384, for example, this bandwidth
        #  yields a binwidth of 0.762Hz, and plenty of CPU left over
        #  for other things, like the SETI analysis code.
        #
        self.seti_fft_bandwidth = int(options.setibandwidth)

        # Calculate binwidth
        binwidth = self.seti_fft_bandwidth / options.fft_size

        # Use binwidth, and knowledge of likely chirp rates to set reasonable
        #  values for SETI analysis code.   We assume that SETI signals will
        #  chirp at somewhere between 0.10Hz/sec and 0.25Hz/sec.
        #
        # upper_limit is the "worst case"--that is, the case for which we have
        #  wait the longest to actually see any drift, due to the quantizing
        #  on FFT bins.
        upper_limit = binwidth / 0.10
        self.setitimer = int(upper_limit * 2.00)
        self.scanning = True

        # Calculate the CHIRP values based on Hz/sec
        self.CHIRP_LOWER = 0.10 * self.setitimer
        self.CHIRP_UPPER = 0.25 * self.setitimer

        # Reset hit counter to 0
        self.hitcounter = 0
        # We scan through 1Mhz of bandwidth around the chosen center freq
        self.seti_freq_range = 1.0e6
        # Calculate lower edge
        self.setifreq_lower = options.freq - (self.seti_freq_range/2)
        self.setifreq_current = options.freq
        # Calculate upper edge
        self.setifreq_upper = options.freq + (self.seti_freq_range/2)

        # We change center frequencies every 10 self.setitimer intervals
        self.setifreq_timer = self.setitimer * 10

        # Create actual timer
        self.seti_then = time.time()

        # The hits recording array
        self.nhits = 10
        self.nhitlines = 3
        self.hits_array = Numeric.zeros((self.nhits,self.nhitlines), Numeric.Float64)
        self.hit_intensities = Numeric.zeros((self.nhits,self.nhitlines), Numeric.Float64)
        # Calibration coefficient and offset
        self.calib_coeff = options.calib_coeff
        self.calib_offset = options.calib_offset
        if self.calib_offset < -750:
            self.calib_offset = -750
        if self.calib_offset > 750:
            self.calib_offset = 750

        if self.calib_coeff < 1:
            self.calib_offset = 1
        if self.calib_coeff > 100:
            self.calib_offset = 100

        self.integ = options.integ
        self.avg_alpha = options.avg
        self.gain = options.gain
        self.decln = options.decln

        # Set initial values for datalogging timed-output
        self.continuum_then = time.time()
        self.spectral_then = time.time()
      
        # build the graph

        #
        # If SETI mode, we always run at maximum USRP decimation
        #
        if (self.setimode):
            options.decim = 256

        self.u = usrp.source_c(decim_rate=options.decim)
        self.u.set_mux(usrp.determine_rx_mux_value(self.u, options.rx_subdev_spec))
        # Set initial declination
        self.decln = options.decln

        # determine the daughterboard subdevice we're using
        self.subdev = usrp.selected_subdev(self.u, options.rx_subdev_spec)
        self.cardtype = self.subdev.dbid()

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

        self.fft_size = int(options.fft_size)

        # This buffer is used to remember the most-recent FFT display
        #   values.  Used later by self.write_spectral_data() to write
        #   spectral data to datalogging files, and by the SETI analysis
        #   function.
        #
        self.fft_outbuf = Numeric.zeros(self.fft_size, Numeric.Float64)

        #
        # If SETI mode, only look at seti_fft_bandwidth (currently 12.5Khz)
        #   at a time.
        #
        if (self.setimode):
            self.fft_input_rate = self.seti_fft_bandwidth

            #
            # Build a decimating bandpass filter
            #
            self.fft_input_taps = gr.firdes.complex_band_pass (1.0,
               input_rate,
               -(int(self.fft_input_rate/2)), int(self.fft_input_rate/2), 200,
               gr.firdes.WIN_HAMMING, 0)

            #
            # Compute required decimation factor
            #
            decimation = int(input_rate/self.fft_input_rate)
            self.fft_bandpass = gr.fir_filter_ccc (decimation, 
                self.fft_input_taps)
        else:
            self.fft_input_rate = input_rate

        # Set up FFT display
        if self.waterfall == False:
           self.scope = ra_fftsink.ra_fft_sink_c (self, panel, 
               fft_size=int(self.fft_size), sample_rate=self.fft_input_rate,
               fft_rate=int(self.fft_rate), title="Spectral",  
               ofunc=self.fft_outfunc, xydfunc=self.xydfunc)
        else:
            self.scope = ra_waterfallsink.ra_waterfallsink_c (self, panel,
                fft_size=int(self.fft_size), sample_rate=self.fft_input_rate,
                fft_rate=int(self.fft_rate), title="Spectral", ofunc=self.fft_outfunc, xydfunc=self.xydfunc_waterfall)

        # Set up ephemeris data
        self.locality = ephem.Observer()
        self.locality.long = str(options.longitude)
        self.locality.lat = str(options.latitude)
        # We make notes about Sunset/Sunrise in Continuum log files
        self.sun = ephem.Sun()
        self.sunstate = "??"

        # Set up stripchart display
        self.stripsize = int(options.stripsize)
        if self.setimode == False:
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

        if self.setimode == False:
            # The three integrators--two FIR filters, and an IIR final filter
            self.integrator1 = gr.fir_filter_fff (N, tapsN)
            self.integrator2 = gr.fir_filter_fff (M, tapsM)
            self.integrator3 = gr.single_pole_iir_filter_ff(1.0)
    
            # The detector
            self.detector = gr.complex_to_mag_squared()

    
            # Signal probe
            self.probe = gr.probe_signal_f();
    
            #
            # Continuum calibration stuff
            #
            self.cal_mult = gr.multiply_const_ff(self.calib_coeff/100.0);
            self.cal_offs = gr.add_const_ff(self.calib_offset*4000);

            if self.use_notches == True:
                NOTCH_TAPS = 256
                tmptaps = Numeric.zeros(NOTCH_TAPS,Numeric.Complex64)
                binwidth = self.bw / NOTCH_TAPS

                for i in range(0,NOTCH_TAPS):
                    tmptaps[i] = complex(1.0,0.0)

                for i in self.notches:
                    diff = i - self.observing
                    if i == 0:
                        break
                    if (diff > 0):
                        idx = diff / binwidth
                        idx = idx + 1
                        tmptaps[int(idx)] = complex(0.0, 0.0)
                    if (diff < 0):
                        idx = -diff / binwidth
                        idx = (NOTCH_TAPS/2) - idx
                        idx = int(idx+(NOTCH_TAPS/2))
                        tmptaps[idx] = complex(0.0, 0.0)
    
                    self.notch_taps = FFT.inverse_fft(tmptaps)
                    self.notch_filt = gr.fft_filter_ccc(1, self.notch_taps)

        #
        # Start connecting configured modules in the receive chain
        #

        # The scope--handle SETI mode
        if (self.setimode == False):
            if (self.use_notches == True):
                self.connect(self.u, self.notch_filt, self.scope)
            else:
                self.connect(self.u, self.scope)
        else:
            if (self.use_notches == True):
                self.connect(self.u, self.notch_filt, 
                    self.fft_bandpass, self.scope)
            else:
                self.connect(self.u, self.fft_bandpass, self.scope)

        if self.setimode == False:
            if (self.use_notches == True):
                self.connect(self.notch_filt, self.detector, 
                    self.integrator1, self.integrator2,
                    self.integrator3, self.cal_mult, self.cal_offs, self.chart)
            else:
                self.connect(self.u, self.detector, 
                    self.integrator1, self.integrator2,
                    self.integrator3, self.cal_mult, self.cal_offs, self.chart)
    
            #  current instantaneous integrated detector value
            self.connect(self.cal_offs, self.probe)

        self._build_gui(vbox)

        # Make GUI agree with command-line
        self.integ = options.integ
        if self.setimode == False:
            self.myform['integration'].set_value(int(options.integ))
            self.myform['offset'].set_value(self.calib_offset)
            self.myform['dcgain'].set_value(self.calib_coeff)
        self.myform['average'].set_value(int(options.avg))


        if self.setimode == False:
            # Make integrator agree with command line
            self.set_integration(int(options.integ))

        self.avg_alpha = options.avg

        # Make spectral averager agree with command line
        if options.avg != 1.0:
            self.scope.set_avg_alpha(float(1.0/options.avg))
            self.scope.set_average(True)

        if self.setimode == False:
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
        if self.cardtype in (usrp_dbid.DBS_RX, usrp_dbid.DBS_RX_REV_2_1):
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
            # Adjust current SETI frequency, and limits
            self.setifreq_lower = kv['freq'] - (self.seti_freq_range/2)
            self.setifreq_current = kv['freq']
            self.setifreq_upper = kv['freq'] + (self.seti_freq_range/2)

            # Reset SETI analysis timer
            self.seti_then = time.time()
            # Zero-out hits array when changing frequency
            self.hits_array[:,:] = 0.0
            self.hit_intensities[:,:] = -60.0

            return self.set_freq(kv['freq'])

        def _form_set_decln(kv):
            return self.set_decln(kv['decln'])

        # Position the FFT display
        vbox.Add(self.scope.win, 15, wx.EXPAND)

        if self.setimode == False:
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
        if self.setimode == False:
            vbox3 = wx.BoxSizer(wx.VERTICAL)
        g = self.subdev.gain_range()
        myform['gain'] = form.slider_field(parent=self.panel, sizer=vbox2, label="RF Gain",
                                           weight=1,
                                           min=int(g[0]), max=int(g[1]),
                                           callback=self.set_gain)

        vbox2.Add((4,0), 0, 0)
        myform['average'] = form.slider_field(parent=self.panel, sizer=vbox2, 
                    label="Spectral Averaging (FFT frames)", weight=1, min=1, max=3000, callback=self.set_averaging)

        # Set up scan control button when in SETI mode
        if (self.setimode == True):
	        # SETI scanning control
	        buttonbox = wx.BoxSizer(wx.HORIZONTAL)
	        self.scan_control = form.button_with_callback(self.panel,
	              label="Scan: On ",
	              callback=self.toggle_scanning)
	
	        buttonbox.Add(self.scan_control, 0, wx.CENTER)
	        vbox2.Add(buttonbox, 0, wx.CENTER)

        vbox2.Add((4,0), 0, 0)

        if self.setimode == False:
            myform['integration'] = form.slider_field(parent=self.panel, sizer=vbox2,
                   label="Continuum Integration Time (sec)", weight=1, min=1, max=180, callback=self.set_integration)

            vbox2.Add((4,0), 0, 0)

        myform['decln'] = form.float_field(
            parent=self.panel, sizer=vbox2, label="Current Declination", weight=1,
            callback=myform.check_input_and_call(_form_set_decln))
        vbox2.Add((4,0), 0, 0)

        if self.setimode == False:
            myform['offset'] = form.slider_field(parent=self.panel, sizer=vbox3,
                label="Post-Detector Offset", weight=1, min=-750, max=750, 
                callback=self.set_pd_offset)
            vbox3.Add((2,0), 0, 0)
            myform['dcgain'] = form.slider_field(parent=self.panel, sizer=vbox3,
                label="Post-Detector Gain", weight=1, min=1, max=100, 
                callback=self.set_pd_gain)
            vbox3.Add((2,0), 0, 0)
        hbox.Add(vbox1, 0, 0)
        hbox.Add(vbox2, wx.ALIGN_RIGHT, 0)

        if self.setimode == False:
            hbox.Add(vbox3, wx.ALIGN_RIGHT, 0)

        vbox.Add(hbox, 0, wx.EXPAND)

        self._build_subpanel(vbox)

        self.lmst_timer = wx.PyTimer(self.lmst_timeout)
        #self.lmst_timeout()


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
        if self.setimode == False:
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
         if self.setimode == False:
             x = self.probe.level()
         sidtime = self.locality.sidereal_time()
         # LMST
         s = str(ephem.hours(sidtime)) + " " + self.sunstate
         # Continuum detector value
         if self.setimode == False:
             sx = "%7.4f" % x
             s = s + "\nDet: " + str(sx)
         else:
             sx = "%2d" % self.hitcounter
             sy = "%3.1f-%3.1f" % (self.CHIRP_LOWER, self.CHIRP_UPPER)
             s = s + "\nHits: " + str(sx) + "\nCh lim: " + str(sy)
         self.myform['lmst_high'].set_value(s)

         #
         # Write data out to recording files
         #
         if self.setimode == False:
             self.write_continuum_data(x,sidtime)
             self.write_spectral_data(self.fft_outbuf,sidtime)

         else:
             self.seti_analysis(self.fft_outbuf,sidtime)
             now = time.time()
             if ((self.scanning == True) and ((now - self.seti_then) > self.setifreq_timer)):
                 self.seti_then = now
                 self.setifreq_current = self.setifreq_current + self.fft_input_rate
                 if (self.setifreq_current > self.setifreq_upper):
                     self.setifreq_current = self.setifreq_lower
                 self.set_freq(self.setifreq_current)
                 # Make sure we zero-out the hits array when changing
                 #   frequency.
                 self.hits_array[:,:] = 0.0
                 self.hit_intensities[:,:] = 0.0

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
            self.sun.compute(self.locality)
            enow = ephem.now()
            sun_insky = "Down"
            self.sunstate = "Dn"
            if ((self.sun.rise_time < enow) and (enow < self.sun.set_time)):
               sun_insky = "Up"
               self.sunstate = "Up"
            self.continuum_then = now
        
            continuum_file.write(str(ephem.hours(sidtime))+" "+flt+" Dn="+str(inter)+",")
            continuum_file.write("Ti="+str(integ)+",Fc="+str(fc)+",Bw="+str(bw))
            continuum_file.write(",Ga="+str(ga)+",Sun="+str(sun_insky)+"\n")
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

    def seti_analysis(self,fftbuf,sidtime):
        l = len(fftbuf)
        x = 0
        hits = []
        hit_intensities = []
        if self.seticounter < self.setitimer:
            self.seticounter = self.seticounter + 1
            return
        else:
            self.seticounter = 0

        # Run through FFT output buffer, computing standard deviation (Sigma)
        avg = 0
        # First compute average
        for i in range(0,l):
            avg = avg + fftbuf[i]
        avg = avg / l

        sigma = 0.0
        # Then compute standard deviation (Sigma)
        for i in range(0,l):
            d = fftbuf[i] - avg
            sigma = sigma + (d*d)

        sigma = Numeric.sqrt(sigma/l)

        #
        # Snarfle through the FFT output buffer again, looking for
        #    outlying data points

        start_f = self.observing - (self.fft_input_rate/2)
        current_f = start_f
        f_incr = self.fft_input_rate / l
        l = len(fftbuf)
        hit = -1

        # -nyquist to DC
        for i in range(l/2,l):
            #
            # If current FFT buffer has an item that exceeds the specified
            #  sigma
            #
            if ((fftbuf[i] - avg) > (self.setik * sigma)):
                hits.append(current_f)
                hit_intensities.append(fftbuf[i])
            current_f = current_f + f_incr

        # DC to nyquist
        for i in range(0,l/2):
            #
            # If current FFT buffer has an item that exceeds the specified
            #  sigma
            #
            if ((fftbuf[i] - avg) > (self.setik * sigma)):
                hits.append(current_f)
                hit_intensities.append(fftbuf[i])
            current_f = current_f + f_incr

        # No hits
        if (len(hits) <= 0):
            return

        #
        # OK, so we have some hits in the FFT buffer
        #   They'll have a rather substantial gauntlet to run before
        #   being declared a real "hit"
        #

        # Weed out buffers with an excessive number of strong signals
        if (len(hits) > self.nhits):
            return

        # Weed out FFT buffers with apparent multiple narrowband signals
        #   separated significantly in frequency.  This means that a
        #   single signal spanning multiple bins is OK, but a buffer that
        #   has multiple, apparently-separate, signals isn't OK.
        #
        last = hits[0]
        for i in range(1,len(hits)):
            if ((hits[i] - last) > (f_incr*2.0)):
                return
            last = hits[i]

        #
        # Run through all three hit buffers, computing difference between
        #   frequencies found there, if they're all within the chirp limits
        #   declare a good hit
        #
        good_hit = 0
        good_hit = False
        for i in range(0,min(len(hits),len(self.hits_array[:,0]))):
            f_d1 = abs(self.hits_array[i,0] - hits[i])
            f_d2 = abs(self.hits_array[i,1] - self.hits_array[i,0])
            f_d3 = abs(self.hits_array[i,2] - self.hits_array[i,1])
            if (self.seti_isahit ([f_d1, f_d2, f_d3])):
                good_hit = True
                self.hitcounter = self.hitcounter + 1
                break


        # Save 'n shuffle hits
        for i in range(self.nhitlines,1):
            self.hits_array[:,i] = self.hits_array[:,i-1]
            self.hit_intensities[:,i] = self.hit_intensities[:,i-1]

        for i in range(0,len(hits)):
            self.hits_array[i,0] = hits[i]
            self.hit_intensities[i,0] = hit_intensities[i]

        # Finally, write the hits/intensities buffer
        if (good_hit):
            self.write_hits(sidtime)

        return

    def seti_isahit(self,fdiffs):
        truecount = 0

        for i in range(0,len(fdiffs)):
            if (fdiffs[i] >= self.CHIRP_LOWER and fdiffs[i] <= self.CHIRP_UPPER):
                truecount = truecount + 1

        if truecount == len(fdiffs):
            return (True)
        else:
            return (False)

    def write_hits(self,sidtime):
        # Create localtime structure for producing filename
        foo = time.localtime()
        pfx = self.prefix
        filenamestr = "%s/%04d%02d%02d%02d" % (pfx, foo.tm_year, 
           foo.tm_mon, foo.tm_mday, foo.tm_hour)
    
        # Open the data file, appending
        hits_file = open (filenamestr+".seti","a")

        # Write sidtime first
        hits_file.write(str(ephem.hours(sidtime))+" "+str(self.decln)+" ")

        #
        # Then write the hits/hit intensities buffers with enough
        #   "syntax" to allow parsing by external (not yet written!)
        #   "stuff".
        #
        for i in range(0,self.nhitlines):
            hits_file.write(" ")
            for j in range(0,self.nhits):
                hits_file.write(str(self.hits_array[j,i])+":")
                hits_file.write(str(self.hit_intensities[j,i])+",")
        hits_file.write("\n")
        hits_file.close()
        return

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

    def xydfunc_waterfall(self,pos):
        lower = self.observing - (self.seti_fft_bandwidth / 2)
        upper = self.observing + (self.seti_fft_bandwidth / 2)
        binwidth = self.seti_fft_bandwidth / 1024
        s = "%.6fMHz" % ((lower + (pos.x*binwidth)) / 1.0e6)
        self.myform['spec_data'].set_value(s)

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
    #
    # Turn scanning on/off
    # Called-back by "Recording" button
    #
    def toggle_scanning(self):
        # Current scanning?  Flip state
        if (self.scanning == True):
          self.scanning = False
          self.scan_control.SetLabel("Scan: Off")
        # Not scanning
        else:
          self.scanning = True
          self.scan_control.SetLabel("Scan: On ")

    def set_pd_offset(self,offs):
         self.myform['offset'].set_value(offs)
         self.calib_offset=offs
         self.cal_offs.set_k(offs*4000)

    def set_pd_gain(self,gain):
         self.myform['dcgain'].set_value(gain)
         self.cal_mult.set_k(gain*0.01)
         self.calib_coeff = gain

def main ():
    app = stdgui.stdapp(app_flow_graph, "RADIO ASTRONOMY SPECTRAL/CONTINUUM RECEIVER: $Revision$", nstatus=1)
    app.MainLoop()

if __name__ == '__main__':
    main ()
