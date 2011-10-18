#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
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

import math, struct, sys
from optparse import OptionParser
from math import log10

try:
    import scipy
    from scipy import fftpack
except ImportError:
    print "Error: Program requires scipy (see: www.scipy.org)."
    sys.exit(1)

try:
    from pylab import *
    from matplotlib.font_manager import fontManager, FontProperties
except ImportError:
    print "Error: Program requires matplotlib (see: matplotlib.sourceforge.net)."
    sys.exit(1)

matplotlib.interactive(True)
matplotlib.use('TkAgg')

class draw_constellation:
    def __init__(self, options):
        derot_file = "ofdm_frame_sink_c.dat"
        acq_file = "ofdm_frame_acq_c.dat"
        fft_file = "ofdm_receiver-fft_out_c.dat"

        self.h_derot_file = open(derot_file, "r")
        self.h_acq_file = open(acq_file, "r")
        self.h_fft_file = open(fft_file, "r")

        self.occ_tones = options.occ_tones
        self.fft_size  = options.fft_size
        self.symbol = options.start
        self.sample_rate = options.sample_rate
        
        self.axis_font_size = 16
        self.label_font_size = 18
        self.title_font_size = 20
        self.text_size = 22
        
        # Setup PLOT
        self.fig = figure(1, figsize=(14, 9), facecolor='w')
        rcParams['xtick.labelsize'] = self.axis_font_size
        rcParams['ytick.labelsize'] = self.axis_font_size

        self.text_sym = figtext(0.05, 0.95, ("Symbol: %s" % self.symbol), weight="heavy", size=self.text_size)

        self.make_plots()

        self.button_left_axes = self.fig.add_axes([0.45, 0.01, 0.05, 0.05], frameon=True)
        self.button_left = Button(self.button_left_axes, "<")
        self.button_left_callback = self.button_left.on_clicked(self.button_left_click)

        self.button_right_axes = self.fig.add_axes([0.50, 0.01, 0.05, 0.05], frameon=True)
        self.button_right = Button(self.button_right_axes, ">")
        self.button_right_callback = self.button_right.on_clicked(self.button_right_click)

        self.xlim = self.sp_eq.get_xlim()

        self.manager = get_current_fig_manager()
        #connect('draw_event', self.zoom)
        connect('key_press_event', self.click)
        show()

    def get_data(self):
        self.text_sym.set_text("Symbol: %d" % (self.symbol))

        derot_data = scipy.fromfile(self.h_derot_file, dtype=scipy.complex64, count=self.occ_tones)
        acq_data = scipy.fromfile(self.h_acq_file, dtype=scipy.complex64, count=self.occ_tones)
        fft_data = scipy.fromfile(self.h_fft_file, dtype=scipy.complex64, count=self.fft_size)
        if(len(acq_data) == 0):
            print "End of File"
        else:
            self.acq_data_reals = [r.real for r in acq_data]
            self.acq_data_imags = [i.imag for i in acq_data]
            self.derot_data_reals = [r.real for r in derot_data]
            self.derot_data_imags = [i.imag for i in derot_data]

            self.unequalized_angle = [math.atan2(x.imag, x.real) for x in fft_data]
            self.equalized_angle = [math.atan2(x.imag, x.real) for x in acq_data]
            self.derot_equalized_angle = [math.atan2(x.imag, x.real) for x in derot_data]

            self.time = [i*(1/self.sample_rate) for i in range(len(acq_data))]
            ffttime = [i*(1/self.sample_rate) for i in range(len(fft_data))]

            self.freq = self.get_freq(ffttime, self.sample_rate)

            for i in range(len(fft_data)):
                if(abs(fft_data[i]) == 0.0):
                    fft_data[i] = complex(1e-6,1e-6)
            self.fft_data = [20*log10(abs(f)) for f in fft_data]
              
    def get_freq(self, time, sample_rate, T=1):
        N = len(time)
        Fs = 1.0 / (max(time) - min(time))
        Fn = 0.5 * sample_rate
        freq = [-Fn + i*Fs for i in range(N)]
        return freq

    def make_plots(self):
        self.h_acq_file.seek(8*self.symbol*self.occ_tones, 0)
        self.h_fft_file.seek(8*self.symbol*self.fft_size, 0)
        self.h_derot_file.seek(8*self.symbol*self.occ_tones, 0)

        self.get_data()
        
        # Subplot:  constellation of rotated symbols
        self.sp_const = self.fig.add_subplot(4,1,1, position=[0.15, 0.55, 0.3, 0.35])
        self.sp_const.set_title(("Constellation"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_const.set_xlabel("Inphase", fontsize=self.label_font_size, fontweight="bold")
        self.sp_const.set_ylabel("Qaudrature", fontsize=self.label_font_size, fontweight="bold")
        self.plot_const = plot(self.acq_data_reals, self.acq_data_imags, 'bo')
        self.plot_const += plot(self.derot_data_reals, self.derot_data_imags, 'ro')
        self.sp_const.axis([-2, 2, -2, 2])

        # Subplot: unequalized angle
        self.sp_uneq = self.fig.add_subplot(4,2,1, position=[0.575, 0.55, 0.3, 0.35])
        self.sp_uneq.set_title(("Unequalized Angle"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_uneq.set_xlabel("Time (s)", fontsize=self.label_font_size, fontweight="bold")
        self.sp_uneq.set_ylabel("Angle", fontsize=self.label_font_size, fontweight="bold")
        uneqscale = range(len(self.unequalized_angle))
        self.plot_uneq = plot(uneqscale, self.unequalized_angle, 'bo')

        # Subplot: equalized angle
        self.sp_eq = self.fig.add_subplot(4,1,2, position=[0.15, 0.1, 0.3, 0.35])
        self.sp_eq.set_title(("Equalized Angle"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_eq.set_xlabel("Time (s)", fontsize=self.label_font_size, fontweight="bold")
        self.sp_eq.set_ylabel("Angle", fontsize=self.label_font_size, fontweight="bold")
        eqscale = range(len(self.equalized_angle))
        self.plot_eq = plot(eqscale, self.equalized_angle, 'bo')
        self.plot_eq += plot(eqscale, self.derot_equalized_angle, 'ro', markersize=4)

        # Subplot: FFT
        self.sp_fft = self.fig.add_subplot(4,2,2, position=[0.575, 0.1, 0.3, 0.35])
        self.sp_fft.set_title(("FFT"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_fft.set_xlabel("Frequency (MHz)", fontsize=self.label_font_size, fontweight="bold")
        self.sp_fft.set_ylabel("Power (dBm)", fontsize=self.label_font_size, fontweight="bold")
        self.plot_fft = plot(self.freq, self.fft_data, '-bo')

        draw()

    def update_plots(self):
        eqscale = range(len(self.equalized_angle))
        uneqscale = range(len(self.unequalized_angle))
        self.plot_eq[0].set_data([eqscale, self.equalized_angle])
        self.plot_eq[1].set_data([eqscale, self.derot_equalized_angle])
        self.plot_uneq[0].set_data([uneqscale, self.unequalized_angle])
        self.sp_eq.set_ylim([-4, 4])
        self.sp_uneq.set_ylim([-4, 4])

        #self.sp_iq.axis([min(self.time), max(self.time),
        #                 1.5*min([min(self.acq_data_reals), min(self.acq_data_imags)]),
        #                 1.5*max([max(self.acq_data_reals), max(self.acq_data_imags)])])

        self.plot_const[0].set_data([self.acq_data_reals, self.acq_data_imags])
        self.plot_const[1].set_data([self.derot_data_reals, self.derot_data_imags])
        self.sp_const.axis([-2, 2, -2, 2])

        self.plot_fft[0].set_data([self.freq, self.fft_data])

        draw()
        
    def zoom(self, event):
        newxlim = self.sp_eq.get_xlim()
        if(newxlim != self.xlim):
            self.xlim = newxlim
            r = self.reals[int(ceil(self.xlim[0])) : int(ceil(self.xlim[1]))]
            i = self.imags[int(ceil(self.xlim[0])) : int(ceil(self.xlim[1]))]

            self.plot_const[0].set_data(r, i)
            self.sp_const.axis([-2, 2, -2, 2])
            self.manager.canvas.draw()
            draw()

    def click(self, event):
        forward_valid_keys = [" ", "down", "right"]
        backward_valid_keys = ["up", "left"]

        if(find(event.key, forward_valid_keys)):
            self.step_forward()
            
        elif(find(event.key, backward_valid_keys)):
            self.step_backward()

    def button_left_click(self, event):
        self.step_backward()

    def button_right_click(self, event):
        self.step_forward()

    def step_forward(self):
        self.symbol += 1
        self.get_data()
        self.update_plots()

    def step_backward(self):
        # Step back in file position
        self.symbol -= 1
        if(self.h_acq_file.tell() >= 16*self.occ_tones):
            self.h_acq_file.seek(-16*self.occ_tones, 1)
        else:
            self.symbol = 0
            self.h_acq_file.seek(-self.h_acq_file.tell(),1)


        if(self.h_derot_file.tell() >= 16*self.occ_tones):
            self.h_derot_file.seek(-16*self.occ_tones, 1)
        else:
            self.symbol = 0
            self.h_derot_file.seek(-self.h_derot_file.tell(),1)


        if(self.h_fft_file.tell() >= 16*self.fft_size):
            self.h_fft_file.seek(-16*self.fft_size, 1)
        else:
            self.symbol = 0
            self.h_fft_file.seek(-self.h_fft_file.tell(),1)

        self.get_data()
        self.update_plots()
        
            

#FIXME: there must be a way to do this with a Python builtin
def find(item_in, list_search):
    for l in list_search:
        if item_in == l:
            return True
    return False

def main():
    usage="%prog: [options]"

    parser = OptionParser(conflict_handler="resolve", usage=usage)
    parser.add_option("", "--fft-size", type="int", default=512,
                      help="Specify the size of the FFT [default=%default]")
    parser.add_option("", "--occ-tones", type="int", default=200,
                      help="Specify the number of occupied tones [default=%default]")
    parser.add_option("-s", "--start", type="int", default=0,
                      help="Specify the starting symbol to plot [default=%default]")
    parser.add_option("-R", "--sample-rate", type="float", default=1.0,
                      help="Set the sampler rate of the data [default=%default]")
    
    (options, args) = parser.parse_args ()

    dc = draw_constellation(options)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
    


