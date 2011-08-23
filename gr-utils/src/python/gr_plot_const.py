#!/usr/bin/env python
#
# Copyright 2007,2008,2011 Free Software Foundation, Inc.
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

try:
    import scipy
except ImportError:
    print "Please install SciPy to run this script (http://www.scipy.org/)"
    raise SystemExit, 1

try:
    from pylab import *
    from matplotlib.font_manager import fontManager, FontProperties
except ImportError:
    print "Please install Matplotlib to run this script (http://matplotlib.sourceforge.net/)"
    raise SystemExit, 1

from optparse import OptionParser

class draw_constellation:
    def __init__(self, filename, options):
        self.hfile = open(filename, "r")
        self.block_length = options.block
        self.start = options.start
        self.sample_rate = options.sample_rate

        self.datatype = scipy.complex64
        self.sizeof_data = self.datatype().nbytes    # number of bytes per sample in file

        self.axis_font_size = 16
        self.label_font_size = 18
        self.title_font_size = 20

        # Setup PLOT
        self.fig = figure(1, figsize=(16, 9), facecolor='w')
        rcParams['xtick.labelsize'] = self.axis_font_size
        rcParams['ytick.labelsize'] = self.axis_font_size
        
        self.text_file     = figtext(0.10, 0.95, ("File: %s" % filename), weight="heavy", size=16)
        self.text_file_pos = figtext(0.10, 0.90, "File Position: ", weight="heavy", size=16)
        self.text_block    = figtext(0.40, 0.90, ("Block Size: %d" % self.block_length),
                                     weight="heavy", size=16)        
        self.text_sr       = figtext(0.60, 0.90, ("Sample Rate: %.2f" % self.sample_rate),
                                     weight="heavy", size=16)
        self.make_plots()

        self.button_left_axes = self.fig.add_axes([0.45, 0.01, 0.05, 0.05], frameon=True)
        self.button_left = Button(self.button_left_axes, "<")
        self.button_left_callback = self.button_left.on_clicked(self.button_left_click)

        self.button_right_axes = self.fig.add_axes([0.50, 0.01, 0.05, 0.05], frameon=True)
        self.button_right = Button(self.button_right_axes, ">")
        self.button_right_callback = self.button_right.on_clicked(self.button_right_click)

        self.xlim = self.sp_iq.get_xlim()

        self.manager = get_current_fig_manager()
        connect('draw_event', self.zoom)
        connect('key_press_event', self.click)
        connect('button_press_event', self.mouse_button_callback)
        show()

    def get_data(self):
        self.text_file_pos.set_text("File Position: %d" % (self.hfile.tell()//self.sizeof_data))
        try:
            iq = scipy.fromfile(self.hfile, dtype=self.datatype, count=self.block_length)
        except MemoryError:
            print "End of File"
        else:
            # retesting length here as newer version of scipy does not throw a MemoryError, just
            # returns a zero-length array
            if(len(iq) > 0):
                self.reals = scipy.array([r.real for r in iq])
                self.imags = scipy.array([i.imag for i in iq])
                
                self.time = scipy.array([i*(1/self.sample_rate) for i in range(len(self.reals))])
                return True
            else:
                print "End of File"
                return False

    def make_plots(self):
        # if specified on the command-line, set file pointer
        self.hfile.seek(self.sizeof_data*self.start, 1)

        r = self.get_data()
        
        # Subplot for real and imaginary parts of signal
        self.sp_iq = self.fig.add_subplot(2,1,1, position=[0.075, 0.2, 0.4, 0.6])
        self.sp_iq.set_title(("I&Q"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_iq.set_xlabel("Time (s)", fontsize=self.label_font_size, fontweight="bold")
        self.sp_iq.set_ylabel("Amplitude (V)", fontsize=self.label_font_size, fontweight="bold")
        self.plot_iq  = self.sp_iq.plot(self.time, self.reals, 'bo-', self.time, self.imags, 'ro-')

        # Subplot for constellation plot
        self.sp_const = self.fig.add_subplot(2,2,1, position=[0.575, 0.2, 0.4, 0.6])
        self.sp_const.set_title(("Constellation"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_const.set_xlabel("Inphase", fontsize=self.label_font_size, fontweight="bold")
        self.sp_const.set_ylabel("Qaudrature", fontsize=self.label_font_size, fontweight="bold")
        self.plot_const  = self.sp_const.plot(self.reals, self.imags, 'bo')

        # Add plots to mark current location of point between time and constellation plots
        self.indx = 0
        self.plot_iq += self.sp_iq.plot([self.time[self.indx],], [self.reals[self.indx],], 'mo', ms=8)
        self.plot_iq += self.sp_iq.plot([self.time[self.indx],], [self.imags[self.indx],], 'mo', ms=8)
        self.plot_const += self.sp_const.plot([self.reals[self.indx],], [self.imags[self.indx],], 'mo', ms=12)

        # Adjust axis
        self.sp_iq.axis([self.time.min(), self.time.max(),
                         1.5*min([self.reals.min(), self.imags.min()]),
                         1.5*max([self.reals.max(), self.imags.max()])])
        self.sp_const.axis([-2, 2, -2, 2])

        draw()

    def update_plots(self):
        self.plot_iq[0].set_data([self.time, self.reals])
        self.plot_iq[1].set_data([self.time, self.imags])
        self.sp_iq.axis([self.time.min(), self.time.max(),
                         1.5*min([self.reals.min(), self.imags.min()]),
                         1.5*max([self.reals.max(), self.imags.max()])])

        self.plot_const[0].set_data([self.reals, self.imags])
        self.sp_const.axis([-2, 2, -2, 2])
        draw()
        
    def zoom(self, event):
        newxlim = scipy.array(self.sp_iq.get_xlim())
        curxlim = scipy.array(self.xlim)
        if(newxlim[0] != curxlim[0] or newxlim[1] != curxlim[1]):
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
        trace_forward_valid_keys = [">",]
        trace_backward_valid_keys = ["<",]

        if(find(event.key, forward_valid_keys)):
            self.step_forward()
            
        elif(find(event.key, backward_valid_keys)):
            self.step_backward()

        elif(find(event.key, trace_forward_valid_keys)):
            self.indx = min(self.indx+1, len(self.time)-1)
            self.set_trace(self.indx)

        elif(find(event.key, trace_backward_valid_keys)):
            self.indx = max(0, self.indx-1)
            self.set_trace(self.indx)

    def button_left_click(self, event):
        self.step_backward()

    def button_right_click(self, event):
        self.step_forward()

    def step_forward(self):
        r = self.get_data()
        if(r):
            self.update_plots()

    def step_backward(self):
        # Step back in file position
        if(self.hfile.tell() >= 2*self.sizeof_data*self.block_length ):
            self.hfile.seek(-2*self.sizeof_data*self.block_length, 1)
        else:
            self.hfile.seek(-self.hfile.tell(),1)
        r = self.get_data()
        if(r):
            self.update_plots()
    
        
    def mouse_button_callback(self, event):
        x, y = event.xdata, event.ydata
        
        if x is not None and y is not None:
            if(event.inaxes == self.sp_iq):
                self.indx = searchsorted(self.time, [x])
                self.set_trace(self.indx)
                

    def set_trace(self, indx):
        self.plot_iq[2].set_data(self.time[indx], self.reals[indx])
        self.plot_iq[3].set_data(self.time[indx], self.imags[indx])
        self.plot_const[1].set_data(self.reals[indx], self.imags[indx])
        draw()

            
def find(item_in, list_search):
    try:
	return list_search.index(item_in) != None
    except ValueError:
	return False
	

def main():
    usage="%prog: [options] input_filename"
    description = "Takes a GNU Radio complex binary file and displays the I&Q data versus time and the constellation plot (I vs. Q). You can set the block size to specify how many points to read in at a time and the start position in the file. By default, the system assumes a sample rate of 1, so in time, each sample is plotted versus the sample number. To set a true time axis, set the sample rate (-R or --sample-rate) to the sample rate used when capturing the samples."

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

    dc = draw_constellation(filename, options)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
    


