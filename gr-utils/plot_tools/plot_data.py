#
# Copyright 2007,2008,2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""
Utility to help plotting data from files.
"""


from argparse import ArgumentParser

import numpy

try:
    from pylab import Button, connect, draw, figure, figtext, get_current_fig_manager, show, plot, rcParams
except ImportError:
    print("Please install Python Matplotlib (http://matplotlib.sourceforge.net/) and Python TkInter https://wiki.python.org/moin/TkInter to run this script")
    raise SystemExit(1)


datatype_lookup = {
    "complex64": numpy.complex64,
    "float32": numpy.float32,
    "uint32": numpy.uint32,
    "int32": numpy.int32,
    "uint16": numpy.uint16,
    "int16": numpy.int16,
    "uint8": numpy.uint8,
    "int8": numpy.int8,
}


class plot_data(object):
    def __init__(self, datatype, filenames, options):
        self.hfile = list()
        self.legend_text = list()
        for f in filenames:
            self.hfile.append(open(f, "r"))
            self.legend_text.append(f)

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
        self.fig = figure(1, figsize=(16, 9), facecolor='w')
        rcParams['xtick.labelsize'] = self.axis_font_size
        rcParams['ytick.labelsize'] = self.axis_font_size

        self.text_file_pos = figtext(
            0.10, 0.88, "File Position: ", weight="heavy", size=self.text_size)
        self.text_block = figtext(0.40, 0.88, ("Block Size: %d" % self.block_length),
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

        self.xlim = self.sp_f.get_xlim()

        self.manager = get_current_fig_manager()
        connect('key_press_event', self.click)
        show()

    def get_data(self, hfile):
        self.text_file_pos.set_text(
            "File Position: %d" % (hfile.tell() // self.sizeof_data))
        try:
            f = numpy.fromfile(hfile, dtype=self.datatype,
                               count=self.block_length)
        except MemoryError:
            print("End of File")
        else:
            self.f = numpy.array(f)
            self.time = numpy.array([i * (1 / self.sample_rate)
                                    for i in range(len(self.f))])

    def make_plots(self):
        self.sp_f = self.fig.add_subplot(
            2, 1, 1, position=[0.075, 0.2, 0.875, 0.6])
        self.sp_f.set_title(
            ("Amplitude"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_f.set_xlabel(
            "Time (s)", fontsize=self.label_font_size, fontweight="bold")
        self.sp_f.set_ylabel(
            "Amplitude (V)", fontsize=self.label_font_size, fontweight="bold")
        self.plot_f = list()

        maxval = -1e12
        minval = 1e12

        for hf in self.hfile:
            # if specified on the command-line, set file pointer
            hf.seek(self.sizeof_data * self.start, 1)

            self.get_data(hf)

            # Subplot for real and imaginary parts of signal
            self.plot_f += plot(self.time, self.f, 'o-')
            maxval = max(maxval, self.f.max())
            minval = min(minval, self.f.min())

        self.sp_f.set_ylim([1.5 * minval, 1.5 * maxval])

        self.leg = self.sp_f.legend(self.plot_f, self.legend_text)

        draw()

    def update_plots(self):
        maxval = -1e12
        minval = 1e12
        for hf, p in zip(self.hfile, self.plot_f):
            self.get_data(hf)
            p.set_data([self.time, self.f])
            maxval = max(maxval, self.f.max())
            minval = min(minval, self.f.min())

        self.sp_f.set_ylim([1.5 * minval, 1.5 * maxval])

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
        self.update_plots()

    def step_backward(self):
        for hf in self.hfile:
            # Step back in file position
            if(hf.tell() >= 2 * self.sizeof_data * self.block_length):
                hf.seek(-2 * self.sizeof_data * self.block_length, 1)
            else:
                hf.seek(-hf.tell(), 1)
        self.update_plots()

    @staticmethod
    def setup_options():
        description = "Takes a GNU Radio binary file and displays the samples versus time. You can set the block size to specify how many points to read in at a time and the start position in the file. By default, the system assumes a sample rate of 1, so in time, each sample is plotted versus the sample number. To set a true time axis, set the sample rate (-R or --sample-rate) to the sample rate used when capturing the samples."

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
        parser.add_argument("files", metavar="FILE", nargs='+',
                            help="Input file with samples")
        return parser


def find(item_in, list_search):
    try:
        return list_search.index(item_in) != None
    except ValueError:
        return False
