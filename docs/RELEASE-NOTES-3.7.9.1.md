ChangeLog v3.7.9.1
==================

Contributors
------------

The following list of people directly contributed code to this release.
As an added bonus this is the first contribution for three of these authors!

-   Andrej Rode <mail@andrejro.de>
-   Paul David <pudavid@fastmail.com>
-   Derek Kozel <derekkozel@nameloop.com>
-   Johannes Schmitz <johannes.schmitz1@gmail.com>
-   Johnathan Corgan <johnathan@corganlabs.com>
-   Marcus Müller <marcus.mueller@ettus.com>
-   Martin Braun <martin.braun@ettus.com>
-   Philip Balister <philip@balister.org>
-   Ron Economos <w6rz@comcast.net>
-   Sebastian Koslowski <koslowski@kit.edu>
-   Sylvain Munaut <tnt@246tNt.com>
-   Tim O’Shea <tim.oshea753@gmail.com>
-   Tom Rondeau <tom@trondeau.com>

### Closed issues

-   \#528
-   \#719
-   \#768
-   \#831
-   \#864
-   \#868
-   \#876
-   \#879
-   \#882
-   \#883

Code
----

This release includes a number of minor typos and miscellaneous
rewording of\
error messages. The following sections summarize substantial changes.

### GRC

-   the ‘Parser errors’ menu item wasn’t correctly enabled
-   embedded python blocks: message ports are now optionial and show the
    correct label
-   custom run command now accounts for filepaths that need escaping
-   virtual sink/source message connections were dropped when opening a
    flow graph
-   tooltips in block library are truncated if too long
-   not all tooltips in block library were updated after docstring
    extraction finished
-   some expressions were wrongfully marked invalid after opening a flow
    graph
-   move random uniform source from analog to waveform generators in GRC

### Scheduler and Runtime fixes

A new unit test is available to test a bug reported on stack overflow
where blocks (such as the delay block) would drop tags. Paul David stepped in
with his first contribution to GNU Radio with a fix.

Fix an issue with flat\_flowgraph calculating alignment by
misinterpreting units of write and read indices. This was only apparent when these
indices are non-zero (a flowgraph has stopped and restarted).

Fix exceptions thrown in hier\_block2 constructors.

Check d\_ninput\_items\_required for overflow and shut down if
overflow is detected.

Allow hier\_block2’s to change I/O signature in the constructor, which
was previously allowed by the API, but broken.

### gr-blocks

Use unsigned char rather than char in add\_const\_bb. This was the
legacy behavior with the older add\_const\_XX and was accidentally
replaced with char when replacing the templated version.

Fix vector length units from bytes to number of items in PDU to tagged
stream and tagged stream to PDU blocks.

### gr-filter

Add a check to the rational resampler to stop working after we’ve
operated on all input items (closed issue \#831)

Throw an error when pfb clock sync is given constant taps which results
in a derivative of 0 (calculated as NaN). (closed issue \#812 and 734). Added
a reset\_taps function to externally set taps which should be used in
place of set\_taps.

Fix a memory leak in pfb decimator (closed issue \#882)

### gr-analog

Maximum Deviation parameter for NBFM transmitter exposed to GRC

### gr-digital

Change the constellation receiver to inherit from control\_loop
publicly
rather than the impl to better support the control port interface.
(closed issue \#876)

### gr-fec

TLC to puncture/depuncture GRC files in gr-fec

### gr-zeromq

Major performance and correctness fixes to gr-zeromq

### Builds

Fix cross compiling on for 64-bit architectures by not setting DEBIAN,
REDHAT, and SLACKWARE. OOT modules created with modtool should update their
cmake/Modules/GrPlatform.cmake to support cross 64-bit builds.

