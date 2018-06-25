ChangeLog v3.7.10.2
===================

This is the second bug-fix release for v3.7.10.

Contributors
------------

The following list of people directly contributed code to this
release:

* Alexandru Csete <oz9aec@gmail.com>
* A. Maitland Bottoms <bottoms@debian.org>
* Andrej Rode <andrej.rode@ettus.com>
* Andy Walls <awalls.cx18@gmail.com>
* Bastian Bloessl <bloessl@ccs-labs.org>
* Ben Hilburn <ben.hilburn@ettus.com>
* Bob Iannucci <bob@sv.cmu.edu>
* Chris Kuethe <chris.kuethe+github@gmail.com>
* Clayton Smith <argilo@gmail.com>
* Darek Kawamoto <darek@he360.com>
* Ethan Trewhitt <ethan.trewhitt@gtri.gatech.edu>
* Geof Nieboer <gnieboer@corpcomm.net>
* Hatsune Aru <email withheld>
* Jacob Gilbert <mrjacobagilbert@gmail.com>
* Jiří Pinkava <j-pi@seznam.cz>
* Johannes Demel <demel@ant.uni-bremen.de>
* Johnathan Corgan <johnathan@corganlabs.com>
* Johannes Schmitz <johannes.schmitz1@gmail.com>
* Josh Blum <josh@joshknows.com>
* Kartik Patel <kartikpatel1995@gmail.com>
* Konstantin Mochalov <incredible.angst@gmail.com>
* Kyle Unice <kyle.unice@L-3com.com>
* Marcus Müller <marcus@hostalia.de>
* Martin Braun <martin.braun@ettus.com>
* Michael De Nil <michael@morsemicro.com>
* Michael Dickens <michael.dickens@ettus.com>
* Nick Foster <bistromath@gmail.com>
* Paul Cercueil <paul.cercueil@analog.com>
* Pedro Lobo <pedro.lobo@upm.es>
* Peter Horvath <ejcspii@gmail.com>
* Philip Balister <philip@balister.org>
* Ron Economos <w6rz@comcast.net>
* Sean Nowlan <nowlans@ieee.org>
* Sebastian Koslowski <koslowski@kit.edu>
* Sebastian Müller <senpo@posteo.de>
* Sylvain Munaut <tnt@246tNt.com>
* Thomas Habets <habets@google.com>
* Tim O'Shea <tim.oshea753@gmail.com>
* Tobias Blomberg <email withheld>


Bug Fixes
=========

The GNU Radio project tracks bug fixes via Github pull requests.  You
can get details on each of the below by going to:

https://github.com/gnuradio/gnuradio


### gnuradio-runtime

* \#1034 Fixed performance counter clock option (Pedro Lobo)
* \#1041 Connect message ports before unlock (Bastian Bloessl)
* \#1065 Fixed initialization order of ctrlport static variables (Kyle Unice)
* \#1071 Fixed cmake lib/lib64 issues (Philip Balister)
* \#1075 Fixed pmt thread safety issue (Darek Kawamoto)
* \#1119 Start RPC on message port only blocks (Jacob Gilbert)
* \#1121 Fixed tag_t default copy constructor / operator= bug (Darek Kawamoto)
* \#1125 Fixed pmt_t threading issue with memory fence (Darek Kawamoto)
* \#1152 Fixed numpy warning in pmt code (Bob Iannucci)
* \#1160 Fixed swig operator= warning messages (Darek Kawamoto)


### gnuradio-companion

* \#901  Backwards compatibility fix for pygtk 2.16 (Michael De Nil)
* \#1060 Fixed for Python 2.6.6 compatibility (Ben Hilburn)
* \#1063 Fixed IndexError when consuming \b (Sebastian Koslowski)
* \#1074 Fixed display scaling (Sebastian Koslowski)
* \#1095 Fixed new flowgraph generation mode (Sebastian Koslowski)
* \#1096 Fixed column widths for proper scaling (Sebastian Müller)
* \#1135 Fixed trailing whitespace output (Clayton Smith)
* \#1168 Fixed virtual connection with multiple upstream (Sebastian Koslowski)
* \#1200 Fixed cheetah template evaluation 'optional' tag (Sean Nowls)


### docs

* \#1114 Fixed obsolete doxygen tags (A. Maitland Bottoms)


### gr-analog

* \#1201 Added missing probe_avg_mag_sqrd_cf block to GRC (Sean Nowls)


### gr-blocks

* \#1161 Fixed minor inconsistencies in block XML (Sebastian Koslowski)
* \#1191 Fixed typo on xor block XML (Hatsune Aru)
* \#1194 Fixed peak detector fix initial value (Bastian Bloessl)


### gr-digital

* \#1084 Fixed msk_timing_recovery out-of-bounds (warning) (Nick Foster)
* \#1149 Clarify documentation of clock_recovery_mm_xx (Thomas Habets)


### gr-dtv

* \#902  Fixed incorrect assert and set_relative_rate() (Ron Economos)
* \#1066 Fixed GSL link problem with gr-dtv and gr-atsc (Peter Horvath)
* \#1177 Add missing find_package for GSL (Geof Gnieboer)


### gr-fcd

* \#1030 Updated hidapi to latest HEAD (Alexandru Csete)


### gr-fec

* \#1049 Throw exception if K and R are not supported (Clayton Smith)
* \#1174 Fixed missing header file installation (Sean Nowls)


### gr-filter

* \#1070 Fix pfb_arb_resampler producing too many samples (Sylvain Munaut)


### gr-qtgui

* \#899  Fixed dark.qss data lines forced-on (Tim O'Shea)
* \#918  Fixed y-axis unit display in Frequency Sink (Tobias Blomberg)
* \#920  Fixed axis labels checkbox in Frequency Sink (Tobias Blomberg)
* \#1023 Fixed control panel FFT slider in Frequency Sink (Tobias Blomberg)
* \#1028 Fixed cmake for C++ example (Bastian Bloessl)
* \#1036 Corrected whitespace issues (Sebastian Koslowski)
* \#1037 Fixed tag color to obey style sheet (Johannes Demel)
* \#1158 Fixed SIGSEGV for tag trigger with constellation sink (Andy Walls)
* \#1187 Fixed time sink complex message configuration (Kartik Patel)
* \#1192 Fixed redundant time sink configuration options (Kartik Patel)


### gr-uhd

* \#914  Fixed order of include dirs (Martin Braun)
* \#1133 Fixed channel number resolution (Andrej Rode)
* \#1137 Disable boost thread interrupts during send() and recv() (Andrej Rode)
* \#1142 Fixed documentation for pmt usage (Marcus Müller)


### Platform-specific changes

* \#886  Fixed numerous Windows/MSVC portability issues (Josh Blum)
* \#1062 Set default filepath to documents dir for windows (Geof Gnieboer)
* \#1085 Fixed mingw-w64 portability issues (Paul Cercueil)
* \#1140 Added boost atomic and chrono linkage for Windows (Josh Blum)
* \#1146 Use -undefined dynamic_lookup linkage for (swig) on MacOS (Konstantin Mochalov)
* \#1172 Fixed file monitor on windows (Sebastian Koslowski)
* \#1179 MSVC build updates (Josh Blum)
