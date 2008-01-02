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

import scipy
from pylab import *
from optparse import OptionParser

matplotlib.interactive(True)
matplotlib.use('TkAgg')

class draw_fft_c:
    def __init__(self, filename, options):
        self.hfile = open(filename, "r")
        self.block_length = options.block
        self.start = options.start
        self.sample_rate = options.sample_rate

        self.axis_font_size = 16
        self.label_font_size = 18
        self.title_font_size = 20
        self.text_size = 22

        # Setup PLOT
        self.fig = figure(1, figsize=(16, 9), facecolor='w')
        rcParams['xtick.labelsize'] = self.axis_font_size
        rcParams['ytick.labelsize'] = self.axis_font_size
        
        self.text_file     = figtext(0.10, 0.94, ("File: %s" % filename), weight="heavy", size=self.text_size)
        self.text_file_pos = figtext(0.10, 0.88, "File Position: ", weight="heavy", size=self.text_size)
        self.text_block    = figtext(0.40, 0.88, ("Block Size: %d" % self.block_length),
                                     weight="heavy", size=self.text_size)
        self.text_sr       = figtext(0.60, 0.88, ("Sample Rate: %.2f" % self.sample_rate),
                                     weight="heavy", size=self.text_size)
        self.make_plots()

        self.button_left_axes = self.fig.add_axes([0.45, 0.01, 0.05, 0.05], frameon=True)
        self.button_left = Button(self.button_left_axes, "<")
        self.button_left_callback = self.button_left.on_clicked(self.button_left_click)

        self.button_right_axes = self.fig.add_axes([0.50, 0.01, 0.05, 0.05], frameon=True)
        self.button_right = Button(self.button_right_axes, ">")
        self.button_right_callback = self.button_right.on_clicked(self.button_right_click)

        self.xlim = self.sp_f.get_xlim()

        self.manager = get_current_fig_manager()
        connect('key_press_event', self.click)
        show()
        
    def get_data(self):
        self.text_file_pos.set_text("File Position: %d" % (self.hfile.tell()//4))
        f = scipy.fromfile(self.hfile, dtype=scipy.float32, count=self.block_length)
        #print "Read in %d items" % len(self.f)
        if(len(f) == 0):
            print "End of File"
        else:
            self.f = f
            self.time = [i*(1/self.sample_rate) for i in range(len(self.f))]
        
    def make_plots(self):
        # if specified on the command-line, set file pointer
        self.hfile.seek(8*self.start, 1)

        self.get_data()
        
        # Subplot for real and imaginary parts of signal
        self.sp_f = self.fig.add_subplot(2,1,1, position=[0.075, 0.2, 0.875, 0.6])
        self.sp_f.set_title(("Amplitude"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_f.set_xlabel("Time (s)", fontsize=self.label_font_size, fontweight="bold")
        self.sp_f.set_ylabel("Amplitude (V)", fontsize=self.label_font_size, fontweight="bold")
        self.plot_f = plot(self.time, self.f, 'bo-')
        self.sp_f.set_ylim([1.5*min(self.f),
                            1.5*max(self.f)])

        draw()

    def update_plots(self):
        self.plot_f[0].set_data([self.time, self.f])
        self.sp_f.set_ylim([1.5*min(self.f),
                            1.5*max(self.f)])
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
        if(self.hfile.tell() >= 8*self.block_length ):
            self.hfile.seek(-8*self.block_length, 1)
        else:
            self.hfile.seek(-self.hfile.tell(),1)
        self.get_data()
        self.update_plots()
        
            

#FIXME: there must be a way to do this with a Python builtin
def find(item_in, list_search):
    for l in list_search:
        if item_in == l:
            return True
    return False

def main():
    usage="%prog: [options] input_filename"
    description = "Takes a GNU Radio floating point binary file and displays the samples versus time. You can set the block size to specify how many points to read in at a time and the start position in the file. By default, the system assumes a sample rate of 1, so in time, each sample is plotted versus the sample number. To set a true time axis, set the sample rate (-R or --sample-rate) to the sample rate used when capturing the samples."

    parser = OptionParser(conflict_handler="resolve", usage=usage, description=description)
    parser.add_option("-B", "--block", type="int", default=1000,
                      help="Specify the block size [default=%default]")
    parser.add_option("-s", "--start", type="int", default=0,
                      help="Specify where to start in the file [default=%default]")
    parser.add_option("-R", "--sample-rate", type="float", default=1.0,
                      help="Set the sampler rate of the data [default=%default]")
    
    (options, args) = parser.parse_args ()
    if len(args) != 1:
        parser.print_help()
        raise SystemExit, 1
    filename = args[0]

    dc = draw_fft_c(filename, options)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
    
