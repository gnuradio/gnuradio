* gr_plot_*.py:
These are a collection of Python scripts to enable viewing and analysis of files produced by GNU Radio flow graphs. Most of them work off complex data produced by digital waveforms.


** gr_plot_float.py:
Takes a GNU Radio floating point binary file and displays the samples versus time. You can set the block size to specify how many points to read in at a time and the start position in the file.

By default, the system assumes a sample rate of 1, so in time, each sample is plotted versus the sample number. To set a true time axis, set the sample rate (-R or --sample-rate) to the sample rate used when capturing the samples.



** gr_plot_iq.py:
Takes a GNU Radio complex binary file and displays the I&Q data versus time. You can set the block size to specify how many points to read in at a time and the start position in the file.

By default, the system assumes a sample rate of 1, so in time, each sample is plotted versus the sample number. To set a true time axis, set the sample rate (-R or --sample-rate) to the sample rate used when capturing the samples.



** gr_plot_const.py:
Takes a GNU Radio complex binary file and displays the I&Q data versus time and the constellation plot (I vs. Q). You can set the block size to specify how many points to read in at a time and the start position in the file.

By default, the system assumes a sample rate of 1, so in time, each sample is plotted versus the sample number. To set a true time axis, set the sample rate (-R or --sample-rate) to the sample rate used when capturing the samples.



** gr_plot_fft_c.py:
Takes a GNU Radio complex binary file and displays the I&Q data versus time as well as the frequency domain (FFT) plot. The y-axis values are plotted assuming volts as the amplitude of the I&Q streams and converted into dBm in the frequency domain (the 1/N power adjustment out of the FFT is performed internally).

The script plots a certain block of data at a time, specified on the command line as -B or --block. This value defaults to 1000. The start position in the file can be set by specifying -s or --start and defaults to 0 (the start of the file).

By default, the system assumes a sample rate of 1, so in time, each sample is plotted versus the sample number. To set a true time and frequency axis, set the sample rate (-R or --sample-rate) to the sample rate used when capturing the samples.



** gr_plot_fft_f.py:
Takes a GNU Radio floating point binary file and displays the samples versus time as well as the frequency domain (FFT) plot. The y-axis values are plotted assuming volts as the amplitude of the I&Q streams and converted into dBm in the frequency domain (the 1/N power adjustment out of the FFT is performed internally).

The script plots a certain block of data at a time, specified on the command line as -B or --block. This value defaults to 1000. The start position in the file can be set by specifying -s or --start and defaults to 0 (the start of the file).

By default, the system assumes a sample rate of 1, so in time, each sample is plotted versus the sample number. To set a true time and frequency axis, set the sample rate (-R or --sample-rate) to the sample rate used when capturing the samples.
