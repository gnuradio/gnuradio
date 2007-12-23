#!/usr/bin/env python
#
# Copyright 2004,2005,2007 Free Software Foundation, Inc.
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


#
#
# Pulsar receiver application
#
# Performs both harmonic folding analysis
#  and epoch folding analysis
#
#
from gnuradio import gr, gru, blks2, audio
from usrpm import usrp_dbid
from gnuradio import usrp, optfir
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui2, ra_fftsink, ra_stripchartsink, form, slider
from optparse import OptionParser
import wx
import sys
import Numeric
import numpy.fft
import ephem
import time
import os
import math


class app_flow_graph(stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__(self, frame, panel, vbox, argv)

        self.frame = frame
        self.panel = panel
        
        parser = OptionParser(option_class=eng_option)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=(0, 0),
                          help="select USRP Rx side A or B (default=A)")
        parser.add_option("-d", "--decim", type="int", default=16,
                          help="set fgpa decimation rate to DECIM [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-Q", "--observing", type="eng_float", default=0.0,
                          help="set observing frequency to FREQ")
        parser.add_option("-a", "--avg", type="eng_float", default=1.0,
		help="set spectral averaging alpha")
        parser.add_option("-V", "--favg", type="eng_float", default=2.0,
                help="set folder averaging alpha")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-l", "--reflevel", type="eng_float", default=30.0,
                          help="Set pulse display reference level")
        parser.add_option("-L", "--lowest", type="eng_float", default=1.5,
                          help="Lowest valid frequency bin")
        parser.add_option("-e", "--longitude", type="eng_float", default=-76.02,                          help="Set Observer Longitude")
        parser.add_option("-c", "--latitude", type="eng_float", default=44.85,                          help="Set Observer Latitude")
        parser.add_option("-F", "--fft_size", type="eng_float", default=1024, help="Size of FFT")

        parser.add_option ("-t", "--threshold", type="eng_float", default=2.5, help="pulsar threshold")
        parser.add_option("-p", "--lowpass", type="eng_float", default=100, help="Pulse spectra cutoff freq")
        parser.add_option("-P", "--prefix", default="./", help="File prefix")
        parser.add_option("-u", "--pulsefreq", type="eng_float", default=0.748, help="Observation pulse rate")
        parser.add_option("-D", "--dm", type="eng_float", default=1.0e-5, help="Dispersion Measure")
        parser.add_option("-O", "--doppler", type="eng_float", default=1.0, help="Doppler ratio")
        parser.add_option("-B", "--divbase", type="eng_float", default=20, help="Y/Div menu base")
        parser.add_option("-I", "--division", type="eng_float", default=100, help="Y/Div")
        parser.add_option("-A", "--audio_source", default="plughw:0,0", help="Audio input device spec")
        parser.add_option("-N", "--num_pulses", default=1, type="eng_float", help="Number of display pulses")
        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)

        self.show_debug_info = True

        self.reflevel = options.reflevel
        self.divbase = options.divbase
        self.division = options.division
        self.audiodev = options.audio_source
        self.mult = int(options.num_pulses)

        # Low-pass cutoff for post-detector filter
        # Set to 100Hz usually, since lots of pulsars fit in this
        #   range
        self.lowpass = options.lowpass

        # What is lowest valid frequency bin in post-detector FFT?
        # There's some pollution very close to DC
        self.lowest_freq = options.lowest

        # What (dB) threshold to use in determining spectral candidates
        self.threshold = options.threshold

        # Filename prefix for recording file
        self.prefix = options.prefix

        # Dispersion Measure (DM)
        self.dm = options.dm

        # Doppler shift, as a ratio
        #  1.0 == no doppler shift
        #  1.005 == a little negative shift
        #  0.995 == a little positive shift
        self.doppler = options.doppler

        #
        # Input frequency and observing frequency--not necessarily the
        #   same thing, if we're looking at the IF of some downconverter
        #   that's ahead of the USRP and daughtercard.  This distinction
        #   is important in computing the correct de-dispersion filter.
        #
        self.frequency = options.freq
        if options.observing <= 0:
            self.observing_freq = options.freq
        else:
            self.observing_freq = options.observing
        
        # build the graph
        self.u = usrp.source_c(decim_rate=options.decim)
        self.u.set_mux(usrp.determine_rx_mux_value(self.u, options.rx_subdev_spec))

        #
        # Recording file, in case we ever need to record baseband data
        #
        self.recording = gr.file_sink(gr.sizeof_char, "/dev/null")
        self.recording_state = False

        self.pulse_recording = gr.file_sink(gr.sizeof_short, "/dev/null")
        self.pulse_recording_state = False

        #
        # We come up with recording turned off, but the user may
        #  request recording later on
        self.recording.close()
        self.pulse_recording.close()

        #
        # Need these two for converting 12-bit baseband signals to 8-bit
        #
        self.tofloat = gr.complex_to_float()
        self.tochar = gr.float_to_char()

        # Need this for recording pulses (post-detector)
        self.toshort = gr.float_to_short()


        #
        # The spectral measurer sets this when it has a valid
        #   average spectral peak-to-peak distance
        # We can then use this to program the parameters for the epoch folder
        #
        # We set a sentimental value here
        self.pulse_freq = options.pulsefreq

        # Folder runs at this raw sample rate
        self.folder_input_rate = 20000

        # Each pulse in the epoch folder is sampled at 128 times the nominal
        #  pulse rate
        self.folding = 128

 
        #
        # Try to find candidate parameters for rational resampler
        #
        save_i = 0
        candidates = []
        for i in range(20,300):
            input_rate = self.folder_input_rate
            output_rate = int(self.pulse_freq * i)
            interp = gru.lcm(input_rate, output_rate) / input_rate
            decim = gru.lcm(input_rate, output_rate) / output_rate
            if (interp < 500 and decim < 250000):
                 candidates.append(i)

        # We didn't find anything, bail!
        if (len(candidates) < 1):
            print "Couldn't converge on resampler parameters"
            sys.exit(1)

        #
        # Now try to find candidate with the least sampling error
        #
        mindiff = 999.999
        for i in candidates:
            diff = self.pulse_freq * i
            diff = diff - int(diff)
            if (diff < mindiff):
                mindiff = diff
                save_i = i

        # Recompute rates
        input_rate = self.folder_input_rate
        output_rate = int(self.pulse_freq * save_i)

        # Compute new interp and decim, based on best candidate
        interp = gru.lcm(input_rate, output_rate) / input_rate
        decim = gru.lcm(input_rate, output_rate) / output_rate

        # Save optimized folding parameters, used later
        self.folding = save_i
        self.interp = int(interp)
        self.decim = int(decim)

        # So that we can view N pulses in the pulse viewer window
        FOLD_MULT=self.mult

        # determine the daughterboard subdevice we're using
        self.subdev = usrp.selected_subdev(self.u, options.rx_subdev_spec)
        self.cardtype = self.u.daughterboard_id(0)

        # Compute raw input rate
        input_rate = self.u.adc_freq() / self.u.decim_rate()

        # BW==input_rate for complex data
        self.bw = input_rate

        #
        # Set baseband filter bandwidth if DBS_RX:
        #
        if self.cardtype == usrp_dbid.DBS_RX:
            lbw = input_rate / 2
            if lbw < 1.0e6:
                lbw = 1.0e6
            self.subdev.set_bw(lbw)

        #
        # We use this as a crude volume control for the audio output
        #
        #self.volume = gr.multiply_const_ff(10**(-1))
        

        #
        # Create location data for ephem package
        #
        self.locality = ephem.Observer()
        self.locality.long = str(options.longitude)
        self.locality.lat = str(options.latitude)

        #
        # What is the post-detector LPF cutoff for the FFT?
        #
        PULSAR_MAX_FREQ=int(options.lowpass)

        # First low-pass filters down to input_rate/FIRST_FACTOR
        #   and decimates appropriately
        FIRST_FACTOR=int(input_rate/(self.folder_input_rate/2))
        first_filter = gr.firdes.low_pass (1.0,
                                          input_rate,
                                          input_rate/FIRST_FACTOR,
                                          input_rate/(FIRST_FACTOR*20),         
                                          gr.firdes.WIN_HAMMING)

        # Second filter runs at the output rate of the first filter,
        #  And low-pass filters down to PULSAR_MAX_FREQ*10
        #
        second_input_rate =  int(input_rate/(FIRST_FACTOR/2))
        second_filter = gr.firdes.band_pass(1.0, second_input_rate,
                                          0.10,
                                          PULSAR_MAX_FREQ*10,
                                          PULSAR_MAX_FREQ*1.5,
                                          gr.firdes.WIN_HAMMING)

        # Third filter runs at PULSAR_MAX_FREQ*20
        #   and filters down to PULSAR_MAX_FREQ
        #
        third_input_rate = PULSAR_MAX_FREQ*20
        third_filter = gr.firdes_band_pass(1.0, third_input_rate,
                                           0.10, PULSAR_MAX_FREQ,
                                           PULSAR_MAX_FREQ/10.0,
                                           gr.firdes.WIN_HAMMING)


        #
        # Create the appropriate FFT scope
        #
        self.scope = ra_fftsink.ra_fft_sink_f (panel, 
           fft_size=int(options.fft_size), sample_rate=PULSAR_MAX_FREQ*2,
           title="Post-detector spectrum",  
           ofunc=self.pulsarfunc, xydfunc=self.xydfunc, fft_rate=200)

        #
        # Tell scope we're looking from DC to PULSAR_MAX_FREQ
        #
        self.scope.set_baseband_freq (0.0)


        #
        # Setup stripchart for showing pulse profiles
        #
        hz = "%5.3fHz " % self.pulse_freq
        per = "(%5.3f sec)" % (1.0/self.pulse_freq)
        sr = "%d sps" % (int(self.pulse_freq*self.folding))
        times = " %d Pulse Intervals" % self.mult
        self.chart = ra_stripchartsink.stripchart_sink_f (panel,
               sample_rate=1,
               stripsize=self.folding*FOLD_MULT, parallel=True, title="Pulse Profiles: "+hz+per+times, 
               xlabel="Seconds @ "+sr, ylabel="Level", autoscale=True,
               divbase=self.divbase, scaling=1.0/(self.folding*self.pulse_freq))
        self.chart.set_ref_level(self.reflevel)
        self.chart.set_y_per_div(self.division)

        # De-dispersion filter setup
        #
        # Do this here, just before creating the filter
        #  that will use the taps.
        #
        ntaps = self.compute_disp_ntaps(self.dm,self.bw,self.observing_freq)

        # Taps for the de-dispersion filter
        self.disp_taps = Numeric.zeros(ntaps,Numeric.Complex64)

        # Compute the de-dispersion filter now
        self.compute_dispfilter(self.dm,self.doppler,
            self.bw,self.observing_freq)

        #
        # Call constructors for receive chains
        #

        #
        # Now create the FFT filter using the computed taps
        self.dispfilt = gr.fft_filter_ccc(1, self.disp_taps)

        #
        # Audio sink
        #
        #print "input_rate ", second_input_rate, "audiodev ", self.audiodev
        #self.audio = audio.sink(second_input_rate, self.audiodev)

        #
        # The three post-detector filters
        # Done this way to allow an audio path (up to 10Khz)
        # ...and also because going from xMhz down to ~100Hz
        # In a single filter doesn't seem to work.
        #
        self.first = gr.fir_filter_fff (FIRST_FACTOR/2, first_filter)

        p = second_input_rate / (PULSAR_MAX_FREQ*20)
        self.second = gr.fir_filter_fff (int(p), second_filter)
        self.third = gr.fir_filter_fff (10, third_filter)

        # Detector
        self.detector = gr.complex_to_mag_squared()

        self.enable_comb_filter = False
        # Epoch folder comb filter
        if self.enable_comb_filter == True:
            bogtaps = Numeric.zeros(512, Numeric.Float64)
            self.folder_comb = gr.fft_filter_ccc(1,bogtaps)

        # Rational resampler
        self.folder_rr = blks2.rational_resampler_fff(self.interp, self.decim)

        # Epoch folder bandpass
        bogtaps = Numeric.zeros(1, Numeric.Float64)
        self.folder_bandpass = gr.fir_filter_fff (1, bogtaps)

        # Epoch folder F2C/C2F
        self.folder_f2c = gr.float_to_complex()
        self.folder_c2f = gr.complex_to_float()

        # Epoch folder S2P
        self.folder_s2p = gr.serial_to_parallel (gr.sizeof_float, 
             self.folding*FOLD_MULT)

        # Epoch folder IIR Filter (produces average pulse profiles)
        self.folder_iir = gr.single_pole_iir_filter_ff(1.0/options.favg,
             self.folding*FOLD_MULT)

        #
        # Set all the epoch-folder goop up
        #
        self.set_folding_params()

        # 
        # Start connecting configured modules in the receive chain
        #

        # Connect raw USRP to de-dispersion filter, detector
        self.connect(self.u, self.dispfilt, self.detector)

        # Connect detector output to FIR LPF
        #  in two stages, followed by the FFT scope
        self.connect(self.detector, self.first,
            self.second, self.third, self.scope)

        # Connect audio output
        #self.connect(self.first, self.volume)
        #self.connect(self.volume, (self.audio, 0))
        #self.connect(self.volume, (self.audio, 1))

        # Connect epoch folder
        if self.enable_comb_filter == True:
            self.connect (self.first, self.folder_bandpass, self.folder_rr,
                self.folder_f2c,
                self.folder_comb, self.folder_c2f,
                self.folder_s2p, self.folder_iir,
                self.chart)

        else:
            self.connect (self.first, self.folder_bandpass, self.folder_rr,
                self.folder_s2p, self.folder_iir, self.chart)

        # Connect baseband recording file (initially /dev/null)
        self.connect(self.u, self.tofloat, self.tochar, self.recording)

        # Connect pulse recording file (initially /dev/null)
        self.connect(self.first, self.toshort, self.pulse_recording)

        #
        # Build the GUI elements
        #
        self._build_gui(vbox)

        # Make GUI agree with command-line
        self.myform['average'].set_value(int(options.avg))
        self.myform['foldavg'].set_value(int(options.favg))


        # Make spectral averager agree with command line
        if options.avg != 1.0:
            self.scope.set_avg_alpha(float(1.0/options.avg))
            self.scope.set_average(True)


        # set initial values

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.subdev.gain_range()
            options.gain = float(g[0]+g[1])/2

        if options.freq is None:
            # if no freq was specified, use the mid-point
            r = self.subdev.freq_range()
            options.freq = float(r[0]+r[1])/2

        self.set_gain(options.gain)
        #self.set_volume(-10.0)

        if not(self.set_freq(options.freq)):
            self._set_status_msg("Failed to set initial frequency")

        self.myform['decim'].set_value(self.u.decim_rate())
        self.myform['fs@usb'].set_value(self.u.adc_freq() / self.u.decim_rate())
        self.myform['dbname'].set_value(self.subdev.name())
        self.myform['DM'].set_value(self.dm)
        self.myform['Doppler'].set_value(self.doppler)

        #
        # Start the timer that shows current LMST on the GUI
        #
        self.lmst_timer.Start(1000)


    def _set_status_msg(self, msg):
        self.frame.GetStatusBar().SetStatusText(msg, 0)

    def _build_gui(self, vbox):

        def _form_set_freq(kv):
            return self.set_freq(kv['freq'])

        def _form_set_dm(kv):
            return self.set_dm(kv['DM'])

        def _form_set_doppler(kv):
            return self.set_doppler(kv['Doppler'])

        # Position the FFT or Waterfall
        vbox.Add(self.scope.win, 5, wx.EXPAND)
        vbox.Add(self.chart.win, 5, wx.EXPAND)

        # add control area at the bottom
        self.myform = myform = form.form()
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((7,0), 0, wx.EXPAND)
        vbox1 = wx.BoxSizer(wx.VERTICAL)
        myform['freq'] = form.float_field(
            parent=self.panel, sizer=vbox1, label="Center freq", weight=1,
            callback=myform.check_input_and_call(_form_set_freq, self._set_status_msg))

        vbox1.Add((3,0), 0, 0)

        # To show current Local Mean Sidereal Time
        myform['lmst_high'] = form.static_text_field(
            parent=self.panel, sizer=vbox1, label="Current LMST", weight=1)
        vbox1.Add((3,0), 0, 0)

        # To show current spectral cursor data
        myform['spec_data'] = form.static_text_field(
            parent=self.panel, sizer=vbox1, label="Pulse Freq", weight=1)
        vbox1.Add((3,0), 0, 0)

        # To show best pulses found in FFT output
        myform['best_pulse'] = form.static_text_field(
            parent=self.panel, sizer=vbox1, label="Best freq", weight=1)
        vbox1.Add((3,0), 0, 0)

        vboxBogus = wx.BoxSizer(wx.VERTICAL)
        vboxBogus.Add ((2,0), 0, wx.EXPAND)
        vbox2 = wx.BoxSizer(wx.VERTICAL)
        g = self.subdev.gain_range()
        myform['gain'] = form.slider_field(parent=self.panel, sizer=vbox2, label="RF Gain",
                                           weight=1,
                                           min=int(g[0]), max=int(g[1]),
                                           callback=self.set_gain)

        vbox2.Add((6,0), 0, 0)
        myform['average'] = form.slider_field(parent=self.panel, sizer=vbox2, 
                    label="Spectral Averaging", weight=1, min=1, max=200, callback=self.set_averaging)
        vbox2.Add((6,0), 0, 0)
        myform['foldavg'] = form.slider_field(parent=self.panel, sizer=vbox2,
                    label="Folder Averaging", weight=1, min=1, max=20, callback=self.set_folder_averaging)
        vbox2.Add((6,0), 0, 0)
        #myform['volume'] = form.quantized_slider_field(parent=self.panel, sizer=vbox2,
                    #label="Audio Volume", weight=1, range=(-20, 0, 0.5), callback=self.set_volume)
        #vbox2.Add((6,0), 0, 0)
        myform['DM'] = form.float_field(
            parent=self.panel, sizer=vbox2, label="DM", weight=1,
            callback=myform.check_input_and_call(_form_set_dm))
        vbox2.Add((6,0), 0, 0)
        myform['Doppler'] = form.float_field(
            parent=self.panel, sizer=vbox2, label="Doppler", weight=1,
            callback=myform.check_input_and_call(_form_set_doppler))
        vbox2.Add((6,0), 0, 0)


        # Baseband recording control
        buttonbox = wx.BoxSizer(wx.HORIZONTAL)
        self.record_control = form.button_with_callback(self.panel,
              label="Recording baseband: Off                           ",
              callback=self.toggle_recording)
        self.record_pulse_control = form.button_with_callback(self.panel,
              label="Recording pulses: Off                              ",
              callback=self.toggle_pulse_recording)

        buttonbox.Add(self.record_control, 0, wx.CENTER)
        buttonbox.Add(self.record_pulse_control, 0, wx.CENTER)
        vbox.Add(buttonbox, 0, wx.CENTER)
        hbox.Add(vbox1, 0, 0)
        hbox.Add(vboxBogus, 0, 0)
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
        r = usrp.tune(self.u, 0, self.subdev, target_freq)

        if r:
            self.myform['freq'].set_value(target_freq)     # update displayed value
            self.myform['baseband'].set_value(r.baseband_freq)
            self.myform['ddc'].set_value(r.dxc_freq)
            # Adjust self.frequency, and self.observing_freq
            # We pick up the difference between the current self.frequency
            #   and the just-programmed one, and use this to adjust
            #   self.observing_freq.  We have to do it this way to
            #   make the dedispersion filtering work out properly.
            delta = target_freq - self.frequency
            self.frequency = target_freq
            self.observing_freq += delta

            # Now that we're adjusted, compute a new dispfilter, and
            #   set the taps for the FFT filter.
            ntaps = self.compute_disp_ntaps(self.dm, self.bw, self.observing_freq)
            self.disp_taps = Numeric.zeros(ntaps, Numeric.Complex64)
            self.compute_dispfilter(self.dm,self.doppler,self.bw,
                self.observing_freq)
            self.dispfilt.set_taps(self.disp_taps)

            return True

        return False

    # Callback for gain-setting slider
    def set_gain(self, gain):
        self.myform['gain'].set_value(gain)     # update displayed value
        self.subdev.set_gain(gain)


    #def set_volume(self, vol):
        #self.myform['volume'].set_value(vol)
        #self.volume.set_k((10**(vol/10))/8192)

    # Callback for spectral-averaging slider
    def set_averaging(self, avval):
        self.myform['average'].set_value(avval)
        self.scope.set_avg_alpha(1.0/(avval))
        self.scope.set_average(True)

    def set_folder_averaging(self, avval):
        self.myform['foldavg'].set_value(avval)
        self.folder_iir.set_taps(1.0/avval)

    # Timer callback to update LMST display
    def lmst_timeout(self):
         self.locality.date = ephem.now()
         sidtime = self.locality.sidereal_time()
         self.myform['lmst_high'].set_value(str(ephem.hours(sidtime)))

    #
    # Turn recording on/off
    # Called-back by "Recording" button
    #
    def toggle_recording(self):
        # Pick up current LMST
        self.locality.date = ephem.now()
        sidtime = self.locality.sidereal_time()

        # Pick up localtime, for generating filenames
        foo = time.localtime()

        # Generate filenames for both data and header file
        filename = "%04d%02d%02d%02d%02d.pdat" % (foo.tm_year, foo.tm_mon,
           foo.tm_mday, foo.tm_hour, foo.tm_min)
        hdrfilename = "%04d%02d%02d%02d%02d.phdr" % (foo.tm_year, foo.tm_mon,
           foo.tm_mday, foo.tm_hour, foo.tm_min)

        # Current recording?  Flip state
        if (self.recording_state == True):
          self.recording_state = False
          self.record_control.SetLabel("Recording baseband: Off                           ")
          self.recording.close()
        # Not recording?
        else:
          self.recording_state = True
          self.record_control.SetLabel("Recording baseband to: "+filename)

          # Cause gr_file_sink object to accept new filename
          #   note use of self.prefix--filename prefix from
          #   command line (defaults to ./)
          #
          self.recording.open (self.prefix+filename)

          #
          # We open the header file as a regular file, write header data,
          #   then close
          hdrf = open(self.prefix+hdrfilename, "w")
          hdrf.write("receiver center frequency: "+str(self.frequency)+"\n")
          hdrf.write("observing frequency: "+str(self.observing_freq)+"\n")
          hdrf.write("DM: "+str(self.dm)+"\n")
          hdrf.write("doppler: "+str(self.doppler)+"\n")

          hdrf.write("sidereal: "+str(ephem.hours(sidtime))+"\n")
          hdrf.write("bandwidth: "+str(self.u.adc_freq() / self.u.decim_rate())+"\n")
          hdrf.write("sample type: complex_char\n")
          hdrf.write("sample size: "+str(gr.sizeof_char*2)+"\n")
          hdrf.close()
    #
    # Turn recording on/off
    # Called-back by "Recording" button
    #
    def toggle_pulse_recording(self):
        # Pick up current LMST
        self.locality.date = ephem.now()
        sidtime = self.locality.sidereal_time()

        # Pick up localtime, for generating filenames
        foo = time.localtime()

        # Generate filenames for both data and header file
        filename = "%04d%02d%02d%02d%02d.padat" % (foo.tm_year, foo.tm_mon,
           foo.tm_mday, foo.tm_hour, foo.tm_min)
        hdrfilename = "%04d%02d%02d%02d%02d.pahdr" % (foo.tm_year, foo.tm_mon,
           foo.tm_mday, foo.tm_hour, foo.tm_min)

        # Current recording?  Flip state
        if (self.pulse_recording_state == True):
          self.pulse_recording_state = False
          self.record_pulse_control.SetLabel("Recording pulses: Off                           ")
          self.pulse_recording.close()
        # Not recording?
        else:
          self.pulse_recording_state = True
          self.record_pulse_control.SetLabel("Recording pulses to: "+filename)

          # Cause gr_file_sink object to accept new filename
          #   note use of self.prefix--filename prefix from
          #   command line (defaults to ./)
          #
          self.pulse_recording.open (self.prefix+filename)

          #
          # We open the header file as a regular file, write header data,
          #   then close
          hdrf = open(self.prefix+hdrfilename, "w")
          hdrf.write("receiver center frequency: "+str(self.frequency)+"\n")
          hdrf.write("observing frequency: "+str(self.observing_freq)+"\n")
          hdrf.write("DM: "+str(self.dm)+"\n")
          hdrf.write("doppler: "+str(self.doppler)+"\n")
          hdrf.write("pulse rate: "+str(self.pulse_freq)+"\n")
          hdrf.write("pulse sps: "+str(self.pulse_freq*self.folding)+"\n")
          hdrf.write("file sps: "+str(self.folder_input_rate)+"\n")

          hdrf.write("sidereal: "+str(ephem.hours(sidtime))+"\n")
          hdrf.write("bandwidth: "+str(self.u.adc_freq() / self.u.decim_rate())+"\n")
          hdrf.write("sample type: short\n")
          hdrf.write("sample size: 1\n")
          hdrf.close()

    # We get called at startup, and whenever the GUI "Set Folding params"
    #   button is pressed
    #
    def set_folding_params(self):
        if (self.pulse_freq <= 0):
            return

        # Compute required sample rate
        self.sample_rate = int(self.pulse_freq*self.folding)

        # And the implied decimation rate
        required_decimation = int(self.folder_input_rate / self.sample_rate)

        # We also compute a new FFT comb filter, based on the expected
        #  spectral profile of our pulse parameters
        #
        # FFT-based comb filter
        #
        N_COMB_TAPS=int(self.sample_rate*4)
        if N_COMB_TAPS > 2000:
            N_COMB_TAPS = 2000
        self.folder_comb_taps = Numeric.zeros(N_COMB_TAPS,Numeric.Complex64)
        fincr = (self.sample_rate)/float(N_COMB_TAPS)
        for i in range(0,len(self.folder_comb_taps)):
            self.folder_comb_taps[i] = complex(0.0, 0.0)

        freq = 0.0
        harmonics = [1.0,2.0,3.0,4.0,5.0,6.0,7.0]
        for i in range(0,len(self.folder_comb_taps)/2):
            for j in range(0,len(harmonics)):
                 if abs(freq - harmonics[j]*self.pulse_freq) <= fincr:
                     self.folder_comb_taps[i] = complex(4.0, 0.0)
                     if harmonics[j] == 1.0:
                         self.folder_comb_taps[i] = complex(8.0, 0.0)
            freq += fincr

        if self.enable_comb_filter == True:
            # Set the just-computed FFT comb filter taps
            self.folder_comb.set_taps(self.folder_comb_taps)

        # And compute a new decimated bandpass filter, to go in front
        #   of the comb.  Primary function is to decimate and filter down
        #   to an exact-ish multiple of the target pulse rate
        #
        self.folding_taps = gr.firdes_band_pass (1.0, self.folder_input_rate,
            0.10, self.sample_rate/2, 10, 
            gr.firdes.WIN_HAMMING)

        # Set the computed taps for the bandpass/decimate filter
        self.folder_bandpass.set_taps (self.folding_taps)
    #
    # Record a spectral "hit" of a possible pulsar spectral profile
    #
    def record_hit(self,hits, hcavg, hcmax):
        # Pick up current LMST
        self.locality.date = ephem.now()
        sidtime = self.locality.sidereal_time()

        # Pick up localtime, for generating filenames
        foo = time.localtime()

        # Generate filenames for both data and header file
        hitfilename = "%04d%02d%02d%02d.phit" % (foo.tm_year, foo.tm_mon,
           foo.tm_mday, foo.tm_hour)

        hitf = open(self.prefix+hitfilename, "a")
        hitf.write("receiver center frequency: "+str(self.frequency)+"\n")
        hitf.write("observing frequency: "+str(self.observing_freq)+"\n")
        hitf.write("DM: "+str(self.dm)+"\n")
        hitf.write("doppler: "+str(self.doppler)+"\n")

        hitf.write("sidereal: "+str(ephem.hours(sidtime))+"\n")
        hitf.write("bandwidth: "+str(self.u.adc_freq() / self.u.decim_rate())+"\n")
        hitf.write("spectral peaks: "+str(hits)+"\n")
        hitf.write("HCM: "+str(hcavg)+" "+str(hcmax)+"\n")
        hitf.close()

    # This is a callback used by ra_fftsink.py (passed on creation of
    #   ra_fftsink)
    # Whenever the user moves the cursor within the FFT display, this
    #   shows the coordinate data
    #
    def xydfunc(self,xyv):
        s = "%.6fHz\n%.3fdB" % (xyv[0], xyv[1])
        if self.lowpass >= 500:
            s = "%.6fHz\n%.3fdB" % (xyv[0]*1000, xyv[1])
        
        self.myform['spec_data'].set_value(s)

    # This is another callback used by ra_fftsink.py (passed on creation
    #   of ra_fftsink).  We pass this as our "calibrator" function, but
    #   we create interesting side-effects in the GUI.
    #
    # This function finds peaks in the FFT output data, and reports
    #  on them through the "Best" text object in the GUI
    #  It also computes the Harmonic Compliance Measure (HCM), and displays
    #  that also.
    #
    def pulsarfunc(self,d,l):
       x = range(0,l)
       incr = float(self.lowpass)/float(l)
       incr = incr * 2.0
       bestdb = -50.0
       bestfreq = 0.0
       avg = 0
       dcnt = 0
       #
       # First, we need to find the average signal level
       #
       for i in x:
           if (i * incr) > self.lowest_freq and (i*incr) < (self.lowpass-2):
               avg += d[i]
               dcnt += 1
       # Set average signal level
       avg /= dcnt
       s2=" "
       findcnt = 0
       #
       # Then we find candidates that are greater than the user-supplied
       #   threshold.
       #
       # We try to cluster "hits" whose whole-number frequency is the
       #   same, and compute an average "hit" frequency.
       #
       lastint = 0
       hits=[]
       intcnt = 0
       freqavg = 0
       for i in x:
           freq = i*incr
           # If frequency within bounds, and the (dB-avg) value is above our
           #   threshold
           if freq > self.lowest_freq and freq < self.lowpass-2 and (d[i] - avg) > self.threshold:
               # If we're finding a new whole-number frequency
               if lastint != int(freq):
                   # Record "center" of this hit, if this is a new hit
                   if lastint != 0:
                       s2 += "%5.3fHz " % (freqavg/intcnt)
                       hits.append(freqavg/intcnt)
                       findcnt += 1
                   lastint = int(freq)
                   intcnt = 1
                   freqavg = freq
               else:
                   intcnt += 1
                   freqavg += freq
           if (findcnt >= 14):
               break

       if intcnt > 1:
           s2 += "%5.3fHz " % (freqavg/intcnt)
           hits.append(freqavg/intcnt)

       #
       # Compute the HCM, by dividing each of the "hits" by each of the
       #   other hits, and comparing the difference between a "perfect"
       #   harmonic, and the observed frequency ratio.
       #
       measure = 0
       max_measure=0
       mcnt = 0
       avg_dist = 0
       acnt = 0
       for i in range(1,len(hits)):
           meas = hits[i]/hits[0] - int(hits[i]/hits[0])
           if abs((hits[i]-hits[i-1])-hits[0]) < 0.1:
               avg_dist += hits[i]-hits[i-1]
               acnt += 1
           if meas > 0.98 and meas < 1.0:
               meas = 1.0 - meas
           meas *= hits[0]
           if meas >= max_measure:
               max_measure = meas
           measure += meas
           mcnt += 1
       if mcnt > 0:
           measure /= mcnt
           if acnt > 0:
               avg_dist /= acnt
       if len(hits) > 1:
           measure /= mcnt
           s3="\nHCM: Avg %5.3fHz(%d) Max %5.3fHz Dist %5.3fHz(%d)" % (measure,mcnt,max_measure, avg_dist, acnt)
           if max_measure < 0.5 and len(hits) >= 2:
               self.record_hit(hits, measure, max_measure)
               self.avg_dist = avg_dist
       else:
           s3="\nHCM: --"
       s4="\nAvg dB: %4.2f" % avg
       self.myform['best_pulse'].set_value("("+s2+")"+s3+s4)

       # Since we are nominally a calibrator function for ra_fftsink, we
       #  simply return what they sent us, untouched.  A "real" calibrator
       #  function could monkey with the data before returning it to the
       #  FFT display function.
       return(d)

    #
    # Callback for the "DM" gui object
    #
    # We call compute_dispfilter() as appropriate to compute a new filter,
    #   and then set that new filter into self.dispfilt.
    #
    def set_dm(self,dm):
       self.dm = dm

       ntaps = self.compute_disp_ntaps (self.dm, self.bw, self.observing_freq)
       self.disp_taps = Numeric.zeros(ntaps, Numeric.Complex64)
       self.compute_dispfilter(self.dm,self.doppler,self.bw,self.observing_freq)
       self.dispfilt.set_taps(self.disp_taps)
       self.myform['DM'].set_value(dm)
       return(dm)

    #
    # Callback for the "Doppler" gui object
    #
    # We call compute_dispfilter() as appropriate to compute a new filter,
    #   and then set that new filter into self.dispfilt.
    #
    def set_doppler(self,doppler):
       self.doppler = doppler

       ntaps = self.compute_disp_ntaps (self.dm, self.bw, self.observing_freq)
       self.disp_taps = Numeric.zeros(ntaps, Numeric.Complex64)
       self.compute_dispfilter(self.dm,self.doppler,self.bw,self.observing_freq)
       self.dispfilt.set_taps(self.disp_taps)
       self.myform['Doppler'].set_value(doppler)
       return(doppler)

    #
    # Compute a de-dispersion filter
    #  From Hankins, et al, 1975
    #
    # This code translated from dedisp_filter.c from Swinburne
    #   pulsar software repository
    #
    def compute_dispfilter(self,dm,doppler,bw,centerfreq):
        npts = len(self.disp_taps)
        tmp = Numeric.zeros(npts, Numeric.Complex64)
        M_PI = 3.14159265358
        DM = dm/2.41e-10

        #
        # Because astronomers are a crazy bunch, the "standard" calcultion
        #   is in Mhz, rather than Hz
        #
        centerfreq = centerfreq / 1.0e6
        bw = bw / 1.0e6
        
        isign = int(bw / abs (bw))
        
        # Center frequency may be doppler shifted
        cfreq     = centerfreq / doppler

        # As well as the bandwidth..
        bandwidth = bw / doppler

        # Bandwidth divided among bins
        binwidth  = bandwidth / npts

        # Delay is an "extra" parameter, in usecs, and largely
        #  untested in the Swinburne code.
        delay = 0.0
        
        # This determines the coefficient of the frequency response curve
        # Linear in DM, but quadratic in center frequency
        coeff = isign * 2.0*M_PI * DM / (cfreq*cfreq)
        
        # DC to nyquist/2
        n = 0
        for i in range(0,int(npts/2)):
            freq = (n + 0.5) * binwidth
            phi = coeff*freq*freq/(cfreq+freq) + (2.0*M_PI*freq*delay)
            tmp[i] = complex(math.cos(phi), math.sin(phi))
            n += 1

        # -nyquist/2 to DC
        n = int(npts/2)
        n *= -1
        for i in range(int(npts/2),npts):
            freq = (n + 0.5) * binwidth
            phi = coeff*freq*freq/(cfreq+freq) + (2.0*M_PI*freq*delay)
            tmp[i] = complex(math.cos(phi), math.sin(phi))
            n += 1
        
        self.disp_taps = numpy.fft.ifft(tmp)
        return(self.disp_taps)

    #
    # Compute minimum number of taps required in de-dispersion FFT filter
    #
    def compute_disp_ntaps(self,dm,bw,freq):
        #
        # Dt calculations are in Mhz, rather than Hz
        #    crazy astronomers....
        mbw = bw/1.0e6
        mfreq = freq/1.0e6

        f_lower = mfreq-(mbw/2)
        f_upper = mfreq+(mbw/2)

        # Compute smear time
        Dt = dm/2.41e-4 * (1.0/(f_lower*f_lower)-1.0/(f_upper*f_upper))

        # ntaps is now bandwidth*smeartime
        # Should be bandwidth*smeartime*2, but the Gnu Radio FFT filter
        #   already expands it by a factor of 2
        ntaps = bw*Dt
        if ntaps < 64:
            ntaps = 64
        return(int(ntaps))

def main ():
    app = stdgui2.stdapp(app_flow_graph, "RADIO ASTRONOMY PULSAR RECEIVER: $Revision$", nstatus=1)
    app.MainLoop()

if __name__ == '__main__':
    main ()
