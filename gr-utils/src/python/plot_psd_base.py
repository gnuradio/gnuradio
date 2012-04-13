#!/usr/bin/env python
#
# Copyright 2007,2008,2010,2011 Free Software Foundation, Inc.
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
    from scipy import fftpack
except ImportError:
    print "Please install SciPy to run this script (http://www.scipy.org/)"
    raise SystemExit, 1

try:
    from pylab import *
except ImportError:
    print "Please install Matplotlib to run this script (http://matplotlib.sourceforge.net/)"
    raise SystemExit, 1

from optparse import OptionParser
from scipy import log10
from gnuradio.eng_option import eng_option

class plot_psd_base:
    def __init__(self, datatype, filename, options):
        self.hfile = open(filename, "r")
        self.block_length = options.block
        self.start = options.start
        self.sample_rate = options.sample_rate
        self.psdfftsize = options.psd_size
        self.specfftsize = options.spec_size

        self.dospec = options.enable_spec  # if we want to plot the spectrogram

        self.datatype = getattr(scipy, datatype) #scipy.complex64
        self.sizeof_data = self.datatype().nbytes    # number of bytes per sample in file

        self.axis_font_size = 16
        self.label_font_size = 18
        self.title_font_size = 20
        self.text_size = 22

        # Setup PLOT
        self.fig = figure(1, figsize=(16, 12), facecolor='w')
        rcParams['xtick.labelsize'] = self.axis_font_size
        rcParams['ytick.labelsize'] = self.axis_font_size

        self.text_file     = figtext(0.10, 0.95, ("File: %s" % filename),
                                     weight="heavy", size=self.text_size)
        self.text_file_pos = figtext(0.10, 0.92, "File Position: ",
                                     weight="heavy", size=self.text_size)
        self.text_block    = figtext(0.35, 0.92, ("Block Size: %d" % self.block_length),
                                     weight="heavy", size=self.text_size)
        self.text_sr       = figtext(0.60, 0.915, ("Sample Rate: %.2f" % self.sample_rate),
                                     weight="heavy", size=self.text_size)
        self.make_plots()

        self.button_left_axes = self.fig.add_axes([0.45, 0.01, 0.05, 0.05], frameon=True)
        self.button_left = Button(self.button_left_axes, "<")
        self.button_left_callback = self.button_left.on_clicked(self.button_left_click)

        self.button_right_axes = self.fig.add_axes([0.50, 0.01, 0.05, 0.05], frameon=True)
        self.button_right = Button(self.button_right_axes, ">")
        self.button_right_callback = self.button_right.on_clicked(self.button_right_click)

        self.xlim = scipy.array(self.sp_iq.get_xlim())

        self.manager = get_current_fig_manager()
        connect('draw_event', self.zoom)
        connect('key_press_event', self.click)
        show()

    def get_data(self):
        self.position = self.hfile.tell()/self.sizeof_data
        self.text_file_pos.set_text("File Position: %d" % self.position)
        try:
            self.iq = scipy.fromfile(self.hfile, dtype=self.datatype, count=self.block_length)
        except MemoryError:
            print "End of File"
            return False
        else:
            # retesting length here as newer version of scipy does not throw a MemoryError, just
            # returns a zero-length array
            if(len(self.iq) > 0):
                tstep = 1.0 / self.sample_rate
                #self.time = scipy.array([tstep*(self.position + i) for i in xrange(len(self.iq))])
                self.time = scipy.array([tstep*(i) for i in xrange(len(self.iq))])

                self.iq_psd, self.freq = self.dopsd(self.iq)
                return True
            else:
                print "End of File"
                return False

    def dopsd(self, iq):
        ''' Need to do this here and plot later so we can do the fftshift '''
        overlap = self.psdfftsize/4
        winfunc = scipy.blackman
        psd,freq = mlab.psd(iq, self.psdfftsize, self.sample_rate,
                            window = lambda d: d*winfunc(self.psdfftsize),
                            noverlap = overlap)
        psd = 10.0*log10(abs(psd))
        return (psd, freq)

    def make_plots(self):
        # if specified on the command-line, set file pointer
        self.hfile.seek(self.sizeof_data*self.start, 1)

        iqdims = [[0.075, 0.2, 0.4, 0.6], [0.075, 0.55, 0.4, 0.3]]
        psddims = [[0.575, 0.2, 0.4, 0.6], [0.575, 0.55, 0.4, 0.3]]
        specdims = [0.2, 0.125, 0.6, 0.3]

        # Subplot for real and imaginary parts of signal
        self.sp_iq = self.fig.add_subplot(2,2,1, position=iqdims[self.dospec])
        self.sp_iq.set_title(("I&Q"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_iq.set_xlabel("Time (s)", fontsize=self.label_font_size, fontweight="bold")
        self.sp_iq.set_ylabel("Amplitude (V)", fontsize=self.label_font_size, fontweight="bold")

        # Subplot for PSD plot
        self.sp_psd = self.fig.add_subplot(2,2,2, position=psddims[self.dospec])
        self.sp_psd.set_title(("PSD"), fontsize=self.title_font_size, fontweight="bold")
        self.sp_psd.set_xlabel("Frequency (Hz)", fontsize=self.label_font_size, fontweight="bold")
        self.sp_psd.set_ylabel("Power Spectrum (dBm)", fontsize=self.label_font_size, fontweight="bold")

        r = self.get_data()

        self.plot_iq  = self.sp_iq.plot([], 'bo-') # make plot for reals
        self.plot_iq += self.sp_iq.plot([], 'ro-') # make plot for imags
        self.draw_time(self.time, self.iq)         # draw the plot

        self.plot_psd = self.sp_psd.plot([], 'b')  # make plot for PSD
        self.draw_psd(self.freq, self.iq_psd)      # draw the plot


        if self.dospec:
            # Subplot for spectrogram plot
            self.sp_spec = self.fig.add_subplot(2,2,3, position=specdims)
            self.sp_spec.set_title(("Spectrogram"), fontsize=self.title_font_size, fontweight="bold")
            self.sp_spec.set_xlabel("Time (s)", fontsize=self.label_font_size, fontweight="bold")
            self.sp_spec.set_ylabel("Frequency (Hz)", fontsize=self.label_font_size, fontweight="bold")

            self.draw_spec(self.time, self.iq)

        draw()

    def draw_time(self, t, iq):
        reals = iq.real
        imags = iq.imag
        self.plot_iq[0].set_data([t, reals])
        self.plot_iq[1].set_data([t, imags])
        self.sp_iq.set_xlim(t.min(), t.max())
        self.sp_iq.set_ylim([1.5*min([reals.min(), imags.min()]),
                             1.5*max([reals.max(), imags.max()])])

    def draw_psd(self, f, p):
        self.plot_psd[0].set_data([f, p])
        self.sp_psd.set_ylim([p.min()-10, p.max()+10])
        self.sp_psd.set_xlim([f.min(), f.max()])

    def draw_spec(self, t, s):
        overlap = self.specfftsize/4
        winfunc = scipy.blackman
        self.sp_spec.clear()
        self.sp_spec.specgram(s, self.specfftsize, self.sample_rate,
                              window = lambda d: d*winfunc(self.specfftsize),
                              noverlap = overlap, xextent=[t.min(), t.max()])

    def update_plots(self):
        self.draw_time(self.time, self.iq)
        self.draw_psd(self.freq, self.iq_psd)

        if self.dospec:
            self.draw_spec(self.time, self.iq)

        self.xlim = scipy.array(self.sp_iq.get_xlim()) # so zoom doesn't get called

        draw()

    def zoom(self, event):
        newxlim = scipy.array(self.sp_iq.get_xlim())
        curxlim = scipy.array(self.xlim)
        if(newxlim[0] != curxlim[0] or newxlim[1] != curxlim[1]):
            #xmin = max(0, int(ceil(self.sample_rate*(newxlim[0] - self.position))))
            #xmax = min(int(ceil(self.sample_rate*(newxlim[1] - self.position))), len(self.iq))
            xmin = max(0, int(ceil(self.sample_rate*(newxlim[0]))))
            xmax = min(int(ceil(self.sample_rate*(newxlim[1]))), len(self.iq))

            iq = scipy.array(self.iq[xmin : xmax])
            time = scipy.array(self.time[xmin : xmax])

            iq_psd, freq = self.dopsd(iq)

            self.draw_psd(freq, iq_psd)
            self.xlim = scipy.array(self.sp_iq.get_xlim())

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

    @staticmethod
    def setup_options():
        usage="%prog: [options] input_filename"
        description = "Takes a GNU Radio binary file (with specified data type using --data-type) and displays the I&Q data versus time as well as the power spectral density (PSD) plot. The y-axis values are plotted assuming volts as the amplitude of the I&Q streams and converted into dBm in the frequency domain (the 1/N power adjustment out of the FFT is performed internally). The script plots a certain block of data at a time, specified on the command line as -B or --block. The start position in the file can be set by specifying -s or --start and defaults to 0 (the start of the file). By default, the system assumes a sample rate of 1, so in time, each sample is plotted versus the sample number. To set a true time and frequency axis, set the sample rate (-R or --sample-rate) to the sample rate used when capturing the samples. Finally, the size of the FFT to use for the PSD and spectrogram plots can be set independently with --psd-size and --spec-size, respectively. The spectrogram plot does not display by default and is turned on with -S or --enable-spec."

        parser = OptionParser(option_class=eng_option, conflict_handler="resolve",
                              usage=usage, description=description)
        parser.add_option("-d", "--data-type", type="string", default="complex64",
                          help="Specify the data type (complex64, float32, (u)int32, (u)int16, (u)int8) [default=%default]")
        parser.add_option("-B", "--block", type="int", default=8192,
                          help="Specify the block size [default=%default]")
        parser.add_option("-s", "--start", type="int", default=0,
                          help="Specify where to start in the file [default=%default]")
        parser.add_option("-R", "--sample-rate", type="eng_float", default=1.0,
                          help="Set the sampler rate of the data [default=%default]")
        parser.add_option("", "--psd-size", type="int", default=1024,
                          help="Set the size of the PSD FFT [default=%default]")
        parser.add_option("", "--spec-size", type="int", default=256,
                          help="Set the size of the spectrogram FFT [default=%default]")
        parser.add_option("-S", "--enable-spec", action="store_true", default=False,
                          help="Turn on plotting the spectrogram [default=%default]")

        return parser

def find(item_in, list_search):
    try:
	return list_search.index(item_in) != None
    except ValueError:
	return False

def main():
    parser = plot_psd_base.setup_options()
    (options, args) = parser.parse_args ()
    if len(args) != 1:
        parser.print_help()
        raise SystemExit, 1
    filename = args[0]

    dc = plot_psd_base(options.data_type, filename, options)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass



