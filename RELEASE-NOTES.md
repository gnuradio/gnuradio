ChangeLog v3.7.10.1
=================

This is the first bug-fix release for v3.7.10

Contributors
------------

The following list of people directly contributed code to this
release:

 * Artem Pisarenko <ArtemPisarenko@users.noreply.github.com>
 * Ben Hilburn <ben.hilburn@ettus.com>
 * Christopher Chavez <chrischavez@gmx.us>
 * Johnathan Corgan <johnathan@corganlabs.com>
 * Jonathan Brucker <jonathan.brucke@gmail.com>
 * Nicholas Corgan <n.corgan@gmail.com>
 * Nicolas Cuervo <nicolas.cuervo@ettus.com>
 * Ron Economos <w6rz@comcast.net>
 * Sebastian Koslowski <koslowski@kit.edu>
 * Stephen Larew <stephen@slarew.net>


## Major Development Areas

This contains bug fixes primarily for GRC and DTV.

### GRC

Catch more exceptions thrown by ConfigParser when reading corrupted grc.conf files.

Fix the docstring update error for empty categories.

Fix grcc to call refactored GRC code.

Convert initially opened files to absolute paths to prevent attempting to read from tmp.

Move startup checks back in to gnuradio-companion script from grc/checks.py.


### DTV

Fix a segfault that occurs from out-of-bounds access in
dvbt_bit_inner_interleaver forecast by forecasting an enumerated list of all
input streams.

Fix VL-SNR framing.

### Digital

Enable update rate in block_recovery_mm blocks to keep tags close to the the proper clock-recovered sample time. Tag offsets will still be off between calls to work, but each work call updates the tag rate.

### Analog

Fix the derivative calculation in fmdet block.

### Builds

Fix linking GSL to gr-fec.

Use gnu99 C standard rather than gnu11 standard to maintain support for GCC 4.6.3.

### Other

Minor spelling and documentation fixes.

Fix uhd_siggen_gui when using lo_locked.

