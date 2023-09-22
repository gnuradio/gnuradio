ChangeLog v3.7.11
=================

This is a feature release of the 3.7 API series, and incorporates all
the bug fixes implemented in the 3.7.10.2 maintenance release.


Contributors
------------

The following list of people directly contributed code to this
release and the incorporated maintenance release:

* A. Maitland Bottoms <bottoms@debian.org>
* Alexandru Csete <oz9aec@gmail.com>
* Andrej Rode <andrej.rode@ettus.com>
* Andy Walls <awalls.cx18@gmail.com>
* Artem Pisarenko <ArtemPisarenko@users.noreply.github.com>
* Bastian Bloessl <bloessl@ccs-labs.org>
* Ben Hilburn <ben.hilburn@ettus.com>
* Bob Iannucci <bob@sv.cmu.edu>
* Chris Kuethe <chris.kuethe+github@gmail.com>
* Christopher Chavez <chrischavez@gmx.us>
* Clayton Smith <argilo@gmail.com>
* Darek Kawamoto <darek@he360.com>
* Ethan Trewhitt <ethan.trewhitt@gtri.gatech.edu>
* Geof Gnieboer <gnieboer@corpcomm.net>
* Hatsune Aru <email withheld>
* Jacob Gilbert <mrjacobagilbert@gmail.com>
* Jiří Pinkava <j-pi@seznam.cz>
* Johannes Demel <demel@ant.uni-bremen.de>
* Johannes Schmitz <johannes.schmitz1@gmail.com>
* Johnathan Corgan <johnathan@corganlabs.com>
* Jonathan Brucker <jonathan.brucke@gmail.com>
* Josh Blum <josh@joshknows.com>
* Kartik Patel <kartikpatel1995@gmail.com>
* Konstantin Mochalov <incredible.angst@gmail.com>
* Kyle Unice <kyle.unice@L-3com.com>
* Marcus Müller <marcus@hostalia.de>
* Martin Braun <martin.braun@ettus.com>
* Michael De Nil <michael@morsemicro.com>
* Michael Dickens <michael.dickens@ettus.com>
* Nathan West <nathan.west@gnuradio.org>
* Nicholas Corgan <n.corgan@gmail.com>
* Nick Foster <bistromath@gmail.com>
* Nicolas Cuervo <nicolas.cuervo@ettus.com>
* Paul Cercueil <paul.cercueil@analog.com>
* Pedro Lobo <pedro.lobo@upm.es>
* Peter Horvath <ejcspii@gmail.com>
* Philip Balister <philip@balister.org>
* Ron Economos <w6rz@comcast.net>
* Sean Nowlan <nowlans@ieee.org>
* Sebastian Koslowski <koslowski@kit.edu>
* Sebastian Müller <senpo@posteo.de>
* Stephen Larew <stephen@slarew.net>
* Sylvain Munaut <tnt@246tNt.com>
* Thomas Habets <habets@google.com>
* Tim O'Shea <tim.oshea753@gmail.com>
* Tobias Blomberg <email withheld>


Changes
=======

The GNU Radio project tracks changes via Github pull requests.  You
can get details on each of the below by going to:

https://github.com/gnuradio/gnuradio

Note: Please see the release notes for 3.7.10.2 for details on the bug
fixes included in this release.


### gnuradio-runtime

* \#1077 Support dynamically loaded gnuradio installs (Josh Blum)


### gnuradio-companion

* \#1118 Support vector types in embedded Python blocks (Clayton Smith)

### gr-audio

* \#1051 Re-implemented defunct Windows audio source (Geof Gnieboer)
* \#1052 Implemented block in Windows audio sink (Geof Gnieboer)


### gr-blocks

* \#896 Added PDU block setters and GRC callbacks (Jacob Gilbert)
* \#900 Exposed non-vector multiply const to GRC (Ron Economos)
* \#903 Deprecated old-style message queue blocks (Johnathan Corgan)
* \#1067 Deprecated blks2 namespace blocks (Johnathan Corgan)


### gr-digital

* \#910 Deprecated correlate_and_sync block 3.8 (Johnathan Corgan)
* \#912 Deprecated modulation blocks for 3.8 (Sebastian Müller)
* \#1069 Improved build memory usage with swig split (Michael Dickens)
* \#1097 Deprecated mpsk_receiver_cc block (Johnathan Corgan)
* \#1099 Deprecated old-style OFDM receiver blocks (Martin Braun)

### gr-dtv

* \#875 Added ability to cross-compile gr-dtv (Ron Economos)
* \#876 Improved ATSC transmitter performance (Ron Economos)
* \#894 Refactored DVB-T RS decoder to use gr-fec (Ron Economos)
* \#898 Improved error handling and logging (Ron Economos)
* \#900 Improved DVB-T performance (Ron Economos)
* \#907 Updated examples to use QT (Ron Economos)
* \#1025 Refactor DVB-T2 interleaver (Ron Economos)


### gr-filter

* \#885 Added set parameter msg port to fractional resampler (Sebastian Müller)


### gr-trellis

* \#908 Updated examples to use QT (Martin Braun)


### gr-uhd

* \#872 Added relative phase plots to uhd_fft (Martin Braun)
* \#1032 Replace zero-timeout double-recv() with one recv() (Martin Braun)
* \#1053 UHD apps may now specify multiple subdevs (Martin Braun)
* \#1101 Support TwinRX LO sharing parameters (Andrej Rode)
* \#1139 Use UHD internal normalized gain methods (Martin Braun)


### gr-utils

* \#897 Improved python docstring generation in gr_modtool
