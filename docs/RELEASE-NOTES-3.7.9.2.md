ChangeLog v3.7.9.2
==================

Contributors
------------

The following list of people directly contributed code to this release.

 * Glenn Richardson <glenn.richardson@live.com>
 * Jacob Gilbert <mrjacobagilbert@gmail.com>
 * Jaroslav Škarvada <jskarvad@redhat.com>
 * Johnathan Corgan <johnathan@corganlabs.com>
 * Marcus Müller <marcus.mueller@ettus.com>
 * Martin Braun <martin.braun@ettus.com>
 * Nathan West <nathan.west@okstate.edu>
 * Nicholas Corgan <nick.corgan@ettus.com>
 * Paul Cercueil <paul.cercueil@analog.com>
 * Sean Nowlan <nowlans@ieee.org>
 * Sebastian Koslowski <koslowski@kit.edu>
 * Tim O'Shea <tim.oshea753@gmail.com>
 * Tom Rondeau <tom@trondeau.com>
 * Tracie Perez <tracie.perez@mavs.uta.edu>

### Closed issues

-   \# 856
-   \# 901
-   \# 903
-   \# 904

Code
----

Several changes fixed type consistency of parameters and documentation/comment
clarifications. The following sections summarize substantial changes by
component.

### GRC

Fix GRC support for scrolling with keyboard PAGEUP and PAGEDOWN buttons.

Pasting blocks will now
 * remove position offset when pasting blocks into an empty flow graph
 * ignore unknown block keys when pasting blocks

XML comments are now ignored rather than parsed as part of the block wrapper.

Rewrite block before adding connections during flowgraph import E.g.: Not all
connections to a block with nports controlled via a parameter block could be
restored from file.

Stop overwritting modified param values in epy blocks

Flowgraphs now run even if a bypassed block has errors.

### QT GUIs

Fixed sample range that fetches tags that would previously duplicate tags that
show up on the last sample in a buffer.

### gr-digital

Fix the internal mask for access codes in correlate_access_code_bb_ts and
correlate_access_code_ff_ts. Previously the top (most significant) bits were
set for the internal mask. This matches a fix fo the
correlate_access_code_tag_bb block from v3.7.5.2. Look for similar fixes and
consistency changes to the correlate_access_code blocks in the future.

Added test descriptions for the burst shaper QA and removed unnecessary
padding.

### gr-fec

FEC documentation continues to improve with every release. Fixes to improve
support for LDPC blocks in GRC. The example flowgraph ber_curve_gen_ldpc should
run properly now.

### gr-filter

Reformatted documentation.

### gr-fft

Always use volk_malloc rather than fftwf_malloc because some binary builds may
not include AVX which will cause alignment faults when fftwf_malloc created
buffers are used in AVX+ proto-kernels.

### gr-blocks

Remove duplicated tags in the tagged_stream_align block.

Fix the type of nitems in set_length (was int, now uint64_t).

### gr-uhd

Loosen requirements for multi-channel operations to have timed command capability.

A few usability fixes to uhd_rx_cfile related to messages in verbose mode and
default options.

XML files call the correct functions for correcting DC offset and IQ imbalance.

### Utilities

gr-perf-monitorx has several small fixes. First, a stability issue that
manifested with the ATSC receive flowgraphs was fixed by adding a small offset
to prevent calculating log(0). Additionally the import of networkx has been
updated to match newer matplotlib and networkx modules while maintaining
compatibility with older versions.

### modtool

Fix gr_modtool rename command for GRC XML files to include the module name.

Fix template expansion code for out of tree modules by adding build_utils.py to
PYTHONPATH. New modules will also have a CMAKE_MODULES_PATH with the module
`cmake/Modules` directory first over the installed GNU Radio modules.

The gr_modtool alias and description for renaming blocks match the
functionality. The new alias is `mv`.

### Builds

A misnamed variable, INCLUDE_DIRS (set by pkg-config) vs INCLUDE_DIR (never
set), has been fixed in FindThrift.cmake. The headers should now be found for
the case of thrift being installed in a prefix that is different than the
target prefix.

Cross compiling with thrift will now use the SDK sysroot's native thrift binary
rather than the system thrift binary.

Minor cmake/swig fix to generate non-make build files.
