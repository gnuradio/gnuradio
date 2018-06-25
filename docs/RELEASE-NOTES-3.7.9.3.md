ChangeLog v3.7.9.3
==================

This is the final, bug-fix only release of the 3.7.9 release series.


Contributors
------------

The following list of people directly contributed code to this
release.

 * A. Maitland Bottoms <bottoms@debian.org>
 * Andrej Rode <mail@andrejro.de>
 * Andy Sloane <andy@a1k0n.net>
 * Andy Walls <awalls@md.metrocast.net>
 * Clayton Smith <argilo@gmail.com>
 * Daehyun Yang <daehyun.yang@gmail.com>
 * Geof Nieboer <gnieboer@corpcomm.net>
 * Glenn Richardson <glenn.richardson@live.com>
 * Jiří Pinkava <j-pi@seznam.cz>
 * Johannes Schmitz <johannes.schmitz1@gmail.com>
 * Johnathan Corgan <johnathan@corganlabs.com>
 * Laur Joost <daremion@gmail.com>
 * Marcus Müller <marcus.mueller@ettus.com>
 * Martin Braun <martin.braun@ettus.com>
 * Nathan West <nathan.west@gnuradio.org>
 * Paul Cercueil <paul.cercueil@analog.com>
 * Philip Balister <philip@balister.org>
 * Sebastian Koslowski <koslowski@kit.edu>
 * Stefan Wunsch <stefan.wunsch@student.kit.edu>
 * Tom Rondeau <tom@trondeau.com>
 * Tracie Renea <tracie.perez@mavs.uta.edu>


Bug fixes
---------------

* cmake: Quiet excessive warnings from cmake regarding deprecated
  usage. (Stefan Wunsch)

* gnuradio-runtime: Fixed race condition during flowgraph
  locking/unlocking (Jiří Pinkava)

* gnuradio-runtime: Fixed incorrect pmt serialization for double
  (Johannes Schmitz)

* gnuradio-runtime: Fixed invalid string reference usage (Daehyun
  Yang)

* gr-audio: Removed inline comments in conf file to workaround parsing
  bug (Marcus Mueller)

* gr-blocks: Fixed stream corruption at termination with UDP
  source/sink blocks. (Andy Sloane)

* gr-blocks: Fixed tag propagation for stream mux block (Andrej Rode)

* gr-digital: Fixed incorrect symbol reporting for unusual carrier
  allocations in OFDM blocks (Martin Braun)

* gr-digital: Fixed missing documentation in OFDM blocks (Martin
  Braun)

* gr-dtv: Fixed broken assertions in DVB-T blocks (Clayton Smith)

* gr-fec: Cleaned up and fixed LDPC encoder/decoder pair for memory
  handling, documentation, and Python function errors. (Tracie Renea)

* gr-filter: Improved documentation iir_filter blocks. (Laur Joost)

* gr-filter: Fixed broken imports of optfir module in examples (Jiří
  Pinkava)

* gr-uhd: Fixed timing of set_sample_rate call during initialization
  (Marcus Mueller)

* gr-zmq: Accommodate gcc 6.x changes to casting requirements (Philip
  Balister)

Numerous MSVC/mingw incompatibilities that have crept in the last few
releases have been corrected in this release. Special thanks go to
Geof Gnieboer and Paul Cercueil (Analog Devices, Inc.) for their
testing and patches.
