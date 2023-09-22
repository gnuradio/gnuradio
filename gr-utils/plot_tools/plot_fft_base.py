#!/usr/bin/env python
#
# Copyright 2007,2008,2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import numpy

try:
    from pylab import Button, connect, draw, figure, figtext, get_current_fig_manager, show, rcParams, ceil
except ImportError:
    print("Please install Python Matplotlib (http://matplotlib.sourceforge.net/) and Python TkInter https://wiki.python.org/moin/TkInter to run this script")
    raise SystemExit(1)

from argparse import ArgumentParser
from gnuradio.plot_data import datatype_lookup


class plot_fft_base(object):
    def __init__(self, datatype, filename, options):
        self.hfile = open(filename, "r")
        self.block_length = options.block
        self.start = options.start
        self.sample_rate = options.sample_rate

        self.datatype = datatype
        if self.datatype is None:
            self.datatype = datatype_lookup[options.data_type]
        # number of bytes per sample in file
        self.sizeof_data = self.datatype().nbytes

        self.axis_font_size = 16
        self.label_font_size = 18
        self.title_font_size = 20
        self.text_size = 22

        # Setup PLOT
        self.fig = figure(1, figsize=(16, 12), facecolor='w')
        rcParams['xtick.labelsize'] = self.axis_font_size
        rcParams['ytick.labelsize'] = self.axis_font_size

        self.text_file = figtext(
            0.10, 0.94, ("File: %s" % filename), weight="heavy", size=self.text_size)
        self.text_file_pos = figtext(
            0.10, 0.88, "File Position: ", weight="heavy", size=self.text_size)
        self.text_block = figtext(0.35, 0.88, ("Block Size: %d" % self.block_length),
                                  weight="heavy", size=self.text_size)
        self.text_sr = figtext(0.60, 0.88, ("Sample Rate: %.2f" % self.sample_rate),
                               weight="heavy", size=self.text_size)
        self.make_plots()

        self.button_left_axes = self.fig.add_axes(
            [0.45, 0.01, 0.05, 0.05], frameon=True)
        self.button_left = Button(self.button_left_axes, "<")
        self.button_left_callback = self.button_left.on_clicked(
            self.button_left_click)

        self.button_right_axes = self.fig.add_axes(
            [0.50, 0.01, 0.05, 0.05], frameon=True)
        self.button_right = Button(self.button_right_axes, ">")
        self.button_right_callback = self.button_right.on_clicked(
            self.button_right_click)

        self.xlim = self.sp_iq.get_xlim()

        self.manager = get_current_fig_manager()
        connect('draw_event', self.zoom)
        connect('key_press_event', self.click)
        show()

    def get_data(self):
        self.position = self.hfile.tell() / self.sizeof_data
        self.text_file_pos.set_text("File Position: %d" % (self.position))
        try:
            self.iq = numpy.fromfile(
                self.hfile, dtype=self.datatype, count=self.block_length)
        except MemoryError:
            print("End of File")
        else:
            self.iq_fft = self.dofft(self.iq)

            tstep = 1.0 / self.sample_rate
            #self.time = numpy.array([tstep*(self.position + i) for i in range(len(self.iq))])
            self.time = numpy.array([tstep * (i) for i in range(len(self.iq))])

            self.freq = self.calc_freq(self.time, self.sample_rate)

    def dofft(self, iq):
        N = len(iq)
        iq_fft = numpy.fft.fftshift(
            numpy.fft.fft(iq))       # fft and shift axis
        # convert to decibels, adjust power
        iq_fft = 20 * numpy.log10(abs((iq_fft + 1e-15) / N))
        # adding 1e-15 (-300 dB) to protect against value errors if an item in iq_fft is 0
        return iq_fft

    def calc_freq(self, time, sample_rate):
        N = len(time)
        Fs = 1.0 / (max(time) - min(time))
        Fn = 0.5 * sample_rate
        freq = numpy.array([-Fn + i * Fs for i in range(N)])
        return freq

    def make_plots(self):
        # if specified on the command-line, set file pointer
        self.hfile.seek(self.sizeof_data * self.start, 1)

        # Subplot for real and imaginary parts of signal
        self.sp_iq = self.fig.add_subplot(
            2, 2, 1, position=[0.075, 0.2, 0.4, 0.6])
        self.sp_iq.set_title(
            ("I&Q"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_iq.set_xlabel(
            "Time (s)", fontsize=self.label_font_size, fontweight="bold")
        self.sp_iq.set_ylabel(
            "Amplitude (V)", fontsize=self.label_font_size, fontweight="bold")

        # Subplot for FFT plot
        self.sp_fft = self.fig.add_subplot(
            2, 2, 2, position=[0.575, 0.2, 0.4, 0.6])
        self.sp_fft.set_title(
            ("FFT"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_fft.set_xlabel(
            "Frequency (Hz)", fontsize=self.label_font_size, fontweight="bold")
        self.sp_fft.set_ylabel("Power Spectrum (dBm)",
                               fontsize=self.label_font_size, fontweight="bold")

        self.get_data()

        self.plot_iq = self.sp_iq.plot([], 'bo-')  # make plot for reals
        self.plot_iq += self.sp_iq.plot([], 'ro-')  # make plot for imags
        self.draw_time()                           # draw the plot

        self.plot_fft = self.sp_fft.plot([], 'bo-')  # make plot for FFT
        self.draw_fft()                              # draw the plot

        draw()

    def draw_time(self):
        reals = self.iq.real
        imags = self.iq.imag
        self.plot_iq[0].set_data([self.time, reals])
        self.plot_iq[1].set_data([self.time, imags])
        self.sp_iq.set_xlim(self.time.min(), self.time.max())
        self.sp_iq.set_ylim([1.5 * min([reals.min(), imags.min()]),
                             1.5 * max([reals.max(), imags.max()])])

    def draw_fft(self):
        self.plot_fft[0].set_data([self.freq, self.iq_fft])
        self.sp_fft.set_xlim(self.freq.min(), self.freq.max())
        self.sp_fft.set_ylim([self.iq_fft.min() - 10, self.iq_fft.max() + 10])

    def update_plots(self):
        self.draw_time()
        self.draw_fft()

        self.xlim = self.sp_iq.get_xlim()
        draw()

    def zoom(self, event):
        newxlim = numpy.array(self.sp_iq.get_xlim())
        curxlim = numpy.array(self.xlim)
        if(newxlim[0] != curxlim[0] or newxlim[1] != curxlim[1]):
            self.xlim = newxlim
            #xmin = max(0, int(ceil(self.sample_rate*(self.xlim[0] - self.position))))
            #xmax = min(int(ceil(self.sample_rate*(self.xlim[1] - self.position))), len(self.iq))
            xmin = max(0, int(ceil(self.sample_rate * (self.xlim[0]))))
            xmax = min(
                int(ceil(self.sample_rate * (self.xlim[1]))), len(self.iq))

            iq = self.iq[xmin: xmax]
            time = self.time[xmin: xmax]

            iq_fft = self.dofft(iq)
            freq = self.calc_freq(time, self.sample_rate)

            self.plot_fft[0].set_data(freq, iq_fft)
            self.sp_fft.axis([freq.min(), freq.max(),
                              iq_fft.min() - 10, iq_fft.max() + 10])

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
        self.get_data()
        self.update_plots()

    def step_backward(self):
        # Step back in file position
        if(self.hfile.tell() >= 2 * self.sizeof_data * self.block_length):
            self.hfile.seek(-2 * self.sizeof_data * self.block_length, 1)
        else:
            self.hfile.seek(-self.hfile.tell(), 1)
        self.get_data()
        self.update_plots()

    @staticmethod
    def setup_options():
        description = "Takes a GNU Radio complex binary file and displays the I&Q data versus time as well as the frequency domain (FFT) plot. The y-axis values are plotted assuming volts as the amplitude of the I&Q streams and converted into dBm in the frequency domain (the 1/N power adjustment out of the FFT is performed internally). The script plots a certain block of data at a time, specified on the command line as -B or --block. This value defaults to 1000. The start position in the file can be set by specifying -s or --start and defaults to 0 (the start of the file). By default, the system assumes a sample rate of 1, so in time, each sample is plotted versus the sample number. To set a true time and frequency axis, set the sample rate (-R or --sample-rate) to the sample rate used when capturing the samples."

        parser = ArgumentParser(
            conflict_handler="resolve", description=description)
        parser.add_argument("-d", "--data-type", default="complex64",
                            choices=("complex64", "float32", "uint32", "int32", "uint16",
                                     "int16", "uint8", "int8"),
                            help="Specify the data type [default=%(default)r]")
        parser.add_argument("-B", "--block", type=int, default=1000,
                            help="Specify the block size [default=%(default)r]")
        parser.add_argument("-s", "--start", type=int, default=0,
                            help="Specify where to start in the file [default=%(default)r]")
        parser.add_argument("-R", "--sample-rate", type=float, default=1.0,
                            help="Set the sampler rate of the data [default=%(default)r]")
        parser.add_argument("file", metavar="FILE",
                            help="Input file with samples")
        return parser


def find(item_in, list_search):
    try:
        return list_search.index(item_in) != None
    except ValueError:
        return False


def main():
    parser = plot_fft_base.setup_options()
    args = parser.parse_args()

    plot_fft_base(None, args.file, args)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
