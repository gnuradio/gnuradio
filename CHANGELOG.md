# Changelog
All notable changes to GNU Radio will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic
Versioning](http://semver.org/spec/v2.0.0.html), starting with version 3.7.12.0.

Older Logs can be found in `docs/RELEASE-NOTES-*`.

## [3.10.10.0] - 2024-04-06

### Changed

#### Runtime

- Modernize location of config files. `XDG_CONFIG_HOME` will be used if set
(often `$HOME/.config`). This change attempts to be backward compatible with
existing config file locations, but be aware that config files may show up
in the old (`$HOME/.gnuradio/`) and new (`$XDG_CONFIG_HOME/gnuradio`)
locations depending on GNU Radio version. Files are not automatically moved,
since some users run multiple versions of GNU Radio.
- Ctrlport Probe, Ctrlport Probe PSD and gr-ctrlport-monitor. Ctrlport Monitor blocks
are still broken.

#### GRC

- NEW Qt-based front end! Run `gnuradio-companion --qt` to try it out. This feature is
still in testing, so the Gtk front end runs by default. In a future release, Qt will
become the default, removing Gtk as a manditory dependency. Maintenance will focus on
the Qt front end at that point.
- Restore property field background colors (as a View option, off by default) for the Gtk
front end. Background colors were previously replaced with textual type string.
- The canvas can be panned using the middle mouse button
- C++ code generation improvements related to parameters and strings
- C++ code generation fixed for Add Const

#### gr-audio

- Added 96 kHz and 192 kHz selections to ALSA source/sink

#### gr-blocks

- Float To UChar block adds vector support, and also scale and bias params

#### gr-digital

- Additive Scrambler adds soft symbol handling

#### modtool

- Improvements to handling of Python blocks (add, rm, and rename work reliably)
- New parseable manifest format (yaml instead of md) to better support the OOT ecosystem
- Manifest and examples are installed by "make install"

#### Build system and packaging

- Python byte-compiled (pyc) files are no longer installed, as they are
deprecated by Python

#### Testing

- Added MinGW test runner and fixed various MinGW compilation failures
- Update Fedora to test 38, 39 and 40

## [3.10.9.0] - 2023-12-24

### Changed

#### Project
- Python minimum version is now 3.7.2, vs 3.6.5, to support type hints. Even
Python 3.7 is EOL, so this is not expected to affect people using newer versions of
GNU Radio.
- Add tox.ini, so that editors use the same formatting as github CI.
- Use pointers to pass s32fc arguments to VOLK in gr-blocks, gr-digital, and
gr-dtv to avoid undefined behavior. The fix is conditional on VOLK 3.1.0, which
add a new supporting function.

#### Runtime
- Disallow None for pmt_t arguments in Python.
- Support spdlog installations with internal or external libfmt.

#### GRC
- Use text labels to specify types for block parameters, instead of background colors
which were difficult to read/remember.
- Enable setting of documentation URLs, relative or absolute in yaml. This allows
OOTs to use the documentation link in the block parameter dialog.
- Disallow use of block ids that are Python keywords and "gnuradio".
- Add type annotations in some places (required bump to Python 3.7.2).
- Use C version of YAML loader (yaml.CSafeLoader) for better performance.
- Connections may have properties for supported connection domains. This feature
was added for RFNoC connections. Standard stream and message connections do not
support this feature, but they may in future versions.
- Check grc file version. The above feature required a version bump to "2". Where
no connection properties are used, version "1" is still emitted. Note that previous
versions of GRC do not check for file version. RFNoC flowgraphs with connection
properties will fail to load in previous versions as a result.
- Fix bug where an impressive number of backslashes were added to some filenames.

#### gr-dtv
- Read expected data as little-endian

#### gr-fec
- Add FEC_API to CCSDS Reed-Solomon functions so they can be used by OOTs.

#### gr-qtgui
- QT GUI Msg Push Button: add a callback for Message Value to allow it to
change at runtime.

#### gr-uhd
- RFNoC Rx-Streamer: Add start stream options
- Add back-edge property to RFNoC connections

#### Build system and packaging
- Update conda build. Re-rendered with conda-build 3.27.0, conda-smithy 3.28.0,
and conda-forge-pinning 2023.11.07.18.09.01.
- Use utf-8 encoding when writing files in gr_python_install.

#### Testing
There has been a great effort to identify why tests fail intermittently, or only
on certain platforms. This has lead to a number of improved test, and identification
of a number of real bugs.

## [3.10.8.0] - 2023-10-20

### Changed

#### Runtime
- Add `MAP_FIXED` to circular buffer implementations using `shm_open()` and
  mmap backed by tmp files. These versions are not used by most system, and
  flaws were discovered during a FreeBSD build.
- PMTs can be formatted for logging (format wrapper added).
- New `io_signature::make()` variant replaces `makev()`, and optionally specifies
  buffer types. Previous variants are still valid, for backward compatibility. The
  single-stream `make()`, along with the `makev()`, `make2()` and `make3()` variants
  should not be used in new code, and may be removed in the future.

```
    make(int min_streams,
         int max_streams,
         const std::vector<size_t>& sizeof_stream_items,
         const gr::gr_vector_buffer_type& buftypes =
             gr::gr_vector_buffer_type(1, default_buftype::type));
```

- Fix logging params to be compatible with C++20

#### GRC
- Add "Choose Editor" button to Python block properties. Use the GTK app chooser.
  Save choice to the config file.
- Correct Python Qt imports in Hier blocks and flowgraph templates.

#### gr-blocks
- Enable building the benchmark testing executable, which runs tests on various
  math functions.
- Repeat block implemented as a basic block (vs sync interpolator) and output
  buffer allocation fixed.
- Add a GRC example for Throttle usage.

#### gr-digital
- Add `set_sps()` to Symbol Sync.
- Header Format: Fix CRC and OFDM formats, add option to `header_buffer` to read
  bits lsb first, and refactor `extract_bits` functions as templates.
- Constellation Sink uses different colors for each input by default.
- Rework Constellation Soft Decoder, Constellation Object and LDPC Decoder Definition.
  Previously, the LDPC Decoder did not work at all. The `sigma` parameter was removed
  from the decoder and an optional noise power `npwr` parameter was added to the
  constellation.

#### gr-network
- Better support for vectorized output from UDP source. The payload size must still
  be a multiple of `item size * vector size` for this to work.

#### gr-qtgui
- Range widget `eng_slider` and `eng` modes can now be selected in GRC.
- Range widget and a couple of UHD apps now accept values on `editingFinished`, e.g.,
  loss of focus, rather than on `returnPressed`. Since UIs generated by GRC do not
  have OK/Apply for such values, there is no "correct" behavior. The behavior is
  now selectable on the Entry widget.
- Frequency Sink startup time improved where sample rate is low

#### gr-soapy
- RTLSDR buffer size may be specified

#### gr-uhd
- RFNoC NullSrcSink block added. The block may be both source and sink.
- Add support and examples for RFNoC loopback.
- RFNoC Rx Radio adds `issue_stream_cmd()` and block message handler.

#### gr-zeromq
- Explicitly shutdown and close source/sinks to prevent hangs in some cases.
- Require `zmq.hpp` (cppzmq) version with `context_t.shutdown()` defined.
  If `shutdown` is not defined, the `gr-zeromq` is disabled.

#### Modtool
- Add `cmake-format` support for generated modules

#### Build system and packaging
- Update Read-the-docs config to include build.tools

#### Testing
- Change Debian 11 to Debian 12 in CI

## [3.10.7.0] - 2023-07-15

### Changed

#### Runtime
- Setting the minimum buffer size should have the desired effect now, and
not be overwritten. NOTE: the value returned by min_buffer_size() is not
intended to indicate the actual buffer size. Header Payload Demod was the
only block attempting to use this value, and was corrected.
- Use a set to store thread group (more efficient)
- Message Debug can now output via the logging system
- The field prefs.singleton is no longer externally exposed (was unintentional)
- PMT dict can be generated more easily using pmt::dict_from_mapping()

#### GRC
- Save changes under all exit conditions (a couple were previously missed)
- Prevent silent Generate/Run failures for unsaved flowgraphs

#### Testing
- Add Fedora 38, using the clang compiler
- Remove EOL Fedora 36

#### gr-analog
- C++ code generation for Quadrature Demod
- Add max_gain parameter for AGC

#### gr-blocks
- Probe Rate adds a name parameter, for clearer logging
- Selector has a new "sync" more that consumes the same number of items from all
inputs. Default is now to consume as many items as possible from the active input,
and no more than that many items from other inputs. The previous behavior was, well,
broken.
- Throttle reset item count on restart, to avoid long delays

#### gr-digital
- Constellation Encoder and Decoder: constellation can be changed at runtime

#### gr-filter
- Filter design tool: multiple improvements in bounds checking and exception handling
- Filter design tool: update QMessageBox to work in Qt5

#### gr-network
- Multiple memory management errors fixed in UDP Source/Sink and TCP Sink


#### gr-soapy
- Better AGC and gain behavior in RTL, AirspyHF and SDRPlay blocks
- Support bias controls in RTL and SDRPlay blocks

#### gr-uhd
- Remove possibility of infinite recursion for network overruns
- Support fmtlib v10
- RFNoC: bindings and block yml for Vector IIR, Replay and Log Power blocks
- RFNoC: add S16 format to RX Streamer

#### gr-vocoder
- Support additional codec2 modes

#### Modtool
- Don't override user-defined CMAKE_INSTALL_PREFIX

## [3.10.6.0] - 2023-03-31

### Changed

#### Runtime
- Add Python loggers to top_block and hier_block2
- Change the default log level (in the config file) to INFO instead of DEBUG
- Logging improvements in the scheduler
- Correctly determine native page size for Windows

#### GRC
- Fixed: opening the source of a hierachical block using the toolbar button produced an error
- Use the logger, instead of print statements, in generated top blocks
- Remove libX11 load from generated Python code - this was unncessary and produced warnings
- Choose Editor dialog stays above parent

#### gr-analog
- Signal Source: option to hide the message port

#### gr-blocks
- Throttle: supports max time or number of samples per work iteration, useful for reducing latency at low sample rates
- Delay block: option to hide the message port
- File Meta Sink: fix missing Python import in template code

#### gr-channels
- Default taps should be 1.0, not 1.0 + j1.0

#### gr-digital
- Async Decoder: several changes to improve performance robustness (see the commit log for more details)

#### gr-fec
- Tagged Decoder: correctly calculate the frame size for terminated CC decoder

#### gr-filter
- Fixed reverse parameters in fir_filter_with_buffer and pfb_arb_resampler, which could cause crashes
- Fixed PFB Arbitrary Resampler was ignoring attenuation parameter

#### gr-iio
- Set gain mode as specified (was always manual)
- Use the specified gain parameter for second channel (was same as first channel)

#### gr-qtgui
- Histogram Sink: calculate range of bins correctly to avoid strange distributions
- Save (to image) dialogs add file extensions and have a Save button (i.e., they work now)

#### gr-soapy
- Sources: add tags when the frequency changes

#### gr-uhd
- Support for more RFNoC blocks
  - Fosphor, which produces data to drive an on-screen, OpenGL-based renderer which is expected to be in the next release
  - Moving Average
  - Switchboard
  - Split Stream
- FFT: add properties for direction, magnitude and scaling
- RX Stream: flush after timeout
- Fully support multi-channel TX/RX (params were available for one one channel)

#### gr-vocoder
- Add a number of new codec modes for Codec2 and FreeDV

#### gr-zmq
- Selectable bind/connect to support more flexible ZMQ patterns and NAT'd networks
- Stream sources produce when available, instead of waiting for a buffer to fill, helping with latency

#### Modtool
- Use interp and decim keywords correctly when generating blocks

#### Build system and packaging
- Uninstall removes icons and desktop files


## [3.10.5.1] - 2023-01-25

Some important blocks turned out to be broken in 3.10.5.0. This unscheduled release fixes those regressions and includes a small number of other cleanups and fixes. v3.10.5.1 is intended to be ABI compatible with v3.10.5.0. We'd still recommend rebuilding dependent packages, if possible.

### Changed

#### Runtime
- Restore the ability to set a default block buffer size using the `buffer_size` parameter in the config file. This was lost during refactoring in v3.9.

#### GRC
- Add Python snipped hook point at "init before blocks", right before blocks are instantiated.

#### gr-audio
- Remove support for OSX 10.3 and earlier.

#### gr-digital
- Make tags visible to subclasses of OFDM Frame Equalizer.

#### gr-dtv
- Correct constant in DVBS2 Modulator.

#### gr-fec
- Fix errors in Channel Construction AWGN

#### gr-iio
- Fix IIO blocks, which were broken due to a build-time dependency problem.

#### gr-network
- Fix crash in UDP Source when buffer overruns.

#### gr-qtgui
- Remove support for QWT 6.0 and earlier.

#### gr-uhd
- Add async message port to USRP Source and publish overflow notifications.
- Add bindings and example for RFNoC AddSub block.

## [3.10.5.0] - 2022-12-19

### Changed

#### Runtime
- Python block have access to the block logger, as in C++
- Default log level changed to INFO (from OFF)
- Memory-based logger `gr.dictionary_logger_backend()` added for log debugging
- API Note: The Python block gateway is now completely implemented in the PyBind11 wrapper, in order to clean up Python dependencies. This is technically an API change, but should not have any external effect.
- PMT serialization of Complex32 vectors is now `REAL | IMAG` on all platforms
- Python IO signature replication (multiple ports specified by one signature) fixed

#### GRC
- Continue processing block connections after a connection error occurs
- Drawing/scaling fixes that improve user experience on HiDPI and Windows machines

#### Build system and packaging
- Many deprecation warnings fixed
- Make target link libraries PRIVATE wherever possible, removing unnecessary downstream dependencies
- Add Fedora 37 and drop Fedora 35 CI targets
- Conda re-rendered with more recent packages - thanks to Ryan Volz for making Conda an easy-to-use, cross-platform method of installing GNU Radio
- Debian and Fedora packaging specs are no longer included in the code base, since they were out of date, and are maintained by downstreams

#### Testing
- Code formatting rules for clang format updated to v14
- Removed all compiler warning suppression
- Enable Python block testing for Conda on macOS
- Many other improvements that make maintenance easier - thanks again to Clayton Smith. In the process of fixing tests, a number of latent bugs were fixed throughout the code.

#### gr-analog
- AGC3 performance and bug fixes
- Python has access to `control_loop` parent class in PLL blocks
- CTCSS detection of standard tones improved by fixing floating point comparison

#### gr-blocks
- Probe Signal cross platform reliability improved by better thread synchronization

#### gr-digital
- CRC32 and CRC16 blocks use little-endian order regardless of host order. This is a wire format change. The options were to have different endian machines unable to communicate, or older and newer versions unable to communicate. Note that there is a more general set of blocks (CRC Append and CRC Check) that are recommended for use wherever possible.
- Packet headers use consistent bit order across machines
- Floating point/rounding fix in constellation lookup table

#### gr-fec
- LDPC G matrix `n` and `k` can be access from Python
- LDPC matrix output size calculation corrected
- CCSDS/Viterbi path metrics overflow fix

#### gr-network
- Improve UDP Source/Sink efficiency by removing a layer of buffering and using the GR circular buffer instead of the Boost equivalent

#### gr-qtgui
- Fixed Python code generation for Msg CheckBox, Digital Number Control, Toggle Button, Toggle Switch

#### gr-soapy
- Sources will generate `rx_time`, `rx_freq` and `rx_rate` tags, as in UHD sources, where supported by the underlying Soapy driver

#### gr-uhd
- Re-enable `uhd.find_devices()`, in addition to `uhd.find()`
- RFNoC: generate correct Python code when using clock/time source
- RFNoC: allow specification of adapter IDs for streamers
- RFNoC: enable setting of vlen and types for streamers
- RFNoC: streamers pay attention to stream args
- RFNoC: sync block controller with gr-ettus OOT
- RFNoC:`set_property()` and `get_property()` added to the C++ and Python APIs
- RFNoC: Python binds added for `rfnoc_block_generic`

#### gr-zeromq
- Sinks will optionally block on full queue, providing backpressure. Previously, overflow data was dropped.

## [3.10.4.0] - 2022-09-16

### Changed

#### Project Scope
- Replace `get_initial_sptr()` calls with `make_block_sptr()` calls. There were a number of places the incorrect function was being used.

#### Runtime
- Use correctly typed arguments to log messages to prevent build errors.

#### GRC
- Add xfce4-terminal and urxvt to the list of terminal emulators discovered during the build process.
- Suppress GUI hint errors that were being shown in the terminal window.
- Use integers for screenshot size (floats were causing Cairo errors).

#### Build system and packaging
- Reformat cmake files and make cmake formatting part of the workflow.
- Allow GNU Radio to be a part of other cmake-based projects.
- Correct linking to libiio and libad9361 on macOS.
- Update method for determining Python installation directory. This should work correctly now on (all?) distro releases.

#### gr-blocks
- New Block Interleaver/Deinterleaver interleaves blocks of symbols
- Correct calculation of `items_remaining` in File Source, which allows `seek()` to work correctly.
- Add an example for Wavefile Sink

#### gr-digital
- Deprecate the CRC32 and CRC16 blocks, which will be removed in the future. There are more general CRC blocks which do the same thing (and more).

#### gr-filter
- Fix demo for PFB channelizer

#### gr-iio
- FMCOMMS2 Sink assumes CS16 data is scaled to 32768, rather than 2048.
- FMCOMMS2 returns the correct samples for the second channel in 2-channel mode.

#### gr-trellis
- Correct Python bindings for `trellis::metrics`.

#### gr-qtgui
- Range widget can now output messages when value changes.
- Add C++ code generation for Time Sink
- Regenerate Python bindings for some blocks when necessary.
- Waterfall Sink correctly uses half spectrum for float input.

#### gr-uhd
- Add Python bindings for the UHD `find()` functino.

#### gr-zeromq
- Support newer `get()` and older/deprecated `getsockopt()` functions in cppzmq depending on availability.

#### Modtool
- Parse IO signatures with or without `gr::` prefix.

#### Documentation

- Update certain file lists to keep build paths out of documentation.

#### Testing
- Update Conda recipe for Qt 5.15 and re-render CI support files.
- Add testing on Ubuntu 22.04.
- Link tests directly against spdlog with not linking to GR runtime.
- Ignore Python "missing whitespace after keywork" formatting error.

## [3.10.3.0] - 2022-06-09

### Changed

#### Project Scope

- Continue replacement of Boost functionality with standard C++ continues, where practical, making the code more maintainable.
- Fix more flaky CI tests that were failing unnecessarily. This helps greatly with maintenance.

#### gnuradio-runtime
- Only catch Thrift transport exceptions
- Import PyQt5 before matplotlib to work around a bug
- Fix broken log format string in set_min_output_buffer
- Process system messages before others. This helps with orderly flowgraph termination.
- Custom buffers: add missing (simulated) data transfer to input/output_blocked_callback functions in host_buffer class
- Fix Mach-kernel timebase (numer and denom were reversed)
- Fix signed integer overflows in fixed-point table generation

#### GRC
- Add parentheses on arithmetic expressions to avoid operator precedence problems in templated code
- Fix create hier / missing top_block error

#### Build system and packaging
- CI: Add testing for Fedora 36, remove Fedora 34.
- cmake: Use platform-specific Python install schemes
- cmake: Always prefix git hash used as version with "g"
- cmake: Allow MPIR/MPLIB package find to fail gracefully
- cmake: Remove 'REQUIRED' flag for Volk

#### gr-blocks
- Fix rotator_cc scheduled phase increment updates
- Wavefile Sink: add support for Ogg Opus if libsndfile is >= 1.0.29
- Probe Signal: synchronize access to d_level to prevent race conditions

#### gr-digital
- Use memcmp for CRC comparisons to avoid alignment errors

#### gr-dtv
- Use unsigned integer for CRC calculation
- Fix use of uninitialized memory
- Fix initialization of L1Post struct

#### gr-filter
- Fix various bugs in Generic Filterbank

#### gr-iio
- Fix grc pluto sink attenuation callback

#### gr-qtgui
- Several sinks produce wrong error messages, when GUI Hint is used. Reorder params in yml files to fix.

#### gr-soapy
- Deactivate stream before closing. Some modules depend on this behavior.

#### gr-uhd
- Implement `get_gpio_attr()`

#### Code generation tools
- C++ generation: Fix various template errors

#### Modtool
- gr_newmod: Fix copying python bindings to test dir on Windows
- gr_newmod: Make untagged conda package version less specific
- modtool: Add a conda recipe to the OOT template
- Make the pydoc_h.mako more clang compliant

#### Documentation
- Add shim Sphinx config for readthedocs

### Authors

The following people contributed commits to this release. They are credited by the name used in commits. There are may people who contribute in other ways ... discussions, reviews, bug reporting, testing, etc. We just don't have an easy way to provide credit for all that valuable work.

- Adrian Suciu <adrian.suciu@analog.com>
- Bjoern Kerler <info@revskills.de>
- Clayton Smith <argilo@gmail.com>
- Daniel Estévez <daniel@destevez.net>
- David Sorber <david.sorber@blacklynx.tech>
- Igor Freire <igor@blockstream.com>
- Jeff Long <willcode4@gmail.com>
- Josh Morman <jmorman@gnuradio.org>
- maitbot
- Martin Braun <martin@gnuradio.org>
- Michael Roe
- Paul Atreides <maud.dib1984@gmail.com>
- Philipp Niedermayer <eltos@outlook.de>
- Ron Economos <w6rz@comcast.net>
- Ryan Volz <ryan.volz@gmail.com>
- Volker Schroer
- wakass


## [3.10.2.0] - 2022-04-09

### Changed

#### Project Scope

- Clayton Smith continues the effort to replace Boost usage with modern C++ equivalents. In a related effort, he has continued the logging modernization started by Marcus Müller. In his spare time, he has tackled some tricky, intermittent CI failures, some of which turned out to be real bugs. Much of this work is invisible to end users, but is extremely useful in making GNU Radio more reliable and maintainable. Special thanks are due to Clayton for a lot of hard work this cycle.
- Use exceptions instead of `exit()` in several places.
- Fixed a variety of Python deprecation warnings.
- Packager note: `jsonschema` is required for the JSON Config and YAML Config blocks. Those blocks will be disabled if `jsonschema` is not found.

#### gnuradio-runtime

- Correct size/usage for single-mapped buffers (part of the new Custom Buffers feature).
- Correct buffer size allocation. This was actually the single change in v3.10.1.1, which did not get its own CHANGELOG entry.

#### GRC

- Improve discovery of xterm and related programs.
- Save generated hierarchical block code to the block library instead of the directory containing the current GRC flowgraph.
- New JSON Config and YAML Config blocks that load configuration variables from files at runtime. Those variables may then be used in block parameters.
- Store the GNU Radio version in flowgraph metadata when saving.
- Minor change in Python evaluation code to allow `affinity`, `minoutbuf` and `maxoutbuf` to be adjusted via script parameters.


#### Build system and packaging

- Require C++-17 for `gnuradio-runtime` and code compiled against it (via cmake flags).
- Add `pythonschema` to build- and run-time dependencies.

#### gr-blocks

- Add exponential distribution to Message Strobe Random block's `delay` selection.
- Quiet down debug messages in File Sink.
- Skip alignment check in File Source when the input file is not seekable (e.g., it is a pipe).

#### gr-filter

- Fix crash in Rational Resampler logging

#### gr-digital

- Add generic CRC blocks: CRC Append and CRC Check.

#### gr-qtgui

- Improve text/background color on Range widget.
- Digital Number Control emits message with new, instead of previous, value.
- Message Edit Box sends message only when return is pressed, rather than whenever focus is lost.
- Vector Sink allows legend to be disabled.
- Type error fixes (Python 3.10 is stricter about int casting).

#### gr-trellis

- Provide Python bindings for PCCC Encoder and Viterbi Combo.

#### gr-vocoder

- Add C++ generation support to gr-vocoder

#### Code generation tools

- Support strongly-typed enums in Python bindings

### Authors

The following people contributed commits to this release. There are may people who contribute in other ways ... discussions, reviews, bug reporting, testing, etc. We just don't have an easy way to provide credit for all that valuable work.

- A. Maitland Bottoms <bottoms@debian.org>
- André Apitzsch <andre.apitzsch@etit.tu-chemnitz.de>
- AsciiWolf <mail@asciiwolf.com>
- Bjoern Kerler <info@revskills.de>
- Campbell McDiarmid <campbell.mcdiarmid@icloud.com>
- Clayton Smith <argilo@gmail.com>
- Daniel Estévez <daniel@destevez.net>
- Davide Gerhard <rainbow@irh.it>
- David Sorber <david.sorber@blacklynx.tech>
- Håkon Vågsether <haakonsv@gmail.com>
- JaredD <jareddpub@gmail.com>
- jb41997
- Jeff Long <willcode4@gmail.com>
- Josh Morman <jmorman@peratonlabs.com>
- jsallay
- luz paz <luzpaz@users.noreply.github.com>
- Marcus Müller <mmueller@gnuradio.org>
- Philip Balister <philip@balister.org>
- Ron Economos <w6rz@comcast.net>
- Ryan Volz <ryan.volz@gmail.com>
- Volker Schroer


## [3.10.1.0] - 2022-01-29

This is mostly a bug fix release. It is API compatible with 3.10.X.Y releases. Code built against GNU Radio libraries (including OOTs) will likely need to be recompiled, as ABI compatibility is not guaranteed.

### Changed

#### Runtime

- Add ownership and locking to hier_block2 to avoid crash/freeze after disconnect.

#### gr-analog

- Fix C++ code generation for random_uniform_source

#### gr-blocks

- Minimal implementation of a SigMF Sink, allowing users to easily try out generation SigMF output. SigMF uses a raw data file and a separate JSON metadata file. A SigMF Source is also provided. At this time, it is a wrapper around a File Source (the data files are compatible), but metadata is not processed.

#### gr-filter

- Bug fix: buses should now work with PFB channelizer and synthesizer.

#### gr-iio

- Various fixes for fmcomms2/3/4.

#### gr-uhd

- Bug fix: overflow count was uninitialized.
- Correct descriptor names in uhd_fpga_ddc/duc.

#### GRC

- Bug fix: initialize value for "priority" parameter in Python Snippets.
- Show blocks with "deprecated" flags as deprecated.

## [3.10.0.0] - 2022-01-14

It is with much excitement that we release the next step forward for GNU Radio - 3.10.0.0!

Not only does this release bring in some extremely useful new modules (gr-iio, gr-pdu, and arguably gr-soapy thought that thankfully made it also into recent 3.9 maintenance releases), but also sets a path forward for using GNU Radio in heterogeneous compute environments by providing "custom buffers" for more efficiently interacting with accelerators (GPUS, FPGAs, TPUs, etc.).

We have been fortunate this year to have extremely active backporting and consistent maintenance releases from co-maintainter Jeff Long - so many of the fixes and smaller feature (and larger ones) have already seen the light of day in the 3.9.x.x and even 3.8.x.x releases.

A special specific thanks to the contributors that made these larger features and upstreamed modules possible, but much appreciation to all that contributed through code, documentation, review, and just generally being a part of this wonderful community.
- gr-pdu: Jacob Gilbert and the team at Sandia National Labs
- gr-iio: Travis Collins and the team at Analog Devices as well as Adam Horden, David Winter, and Volker Shroer for bringing this in-tree and working through many of the complexities.
- Custom Buffers Support: We have David Sorber to thank for this incredible, yet advanced, feature that came out of the DARPA SDR 4.0 program and should get a lot of traction.  Check out https://wiki.gnuradio.org/index.php/Custom_Buffers for more of the gory detail.  Also thanks to Seth Hitefield whose initial work in this area helped get this concept into the mainstream for GNU Radio.
-  Logging Infrastructure Overhaul: A huge thanks to Marcus Müller for fixing all of this up, replacing Log4CPP with spdlog and also for providing ongoing architectural leadership to the project

### Changed

- Moved PDU blocks from gr-blocks to gr-network and gr-pdu
   - Compatibility shim included to allow access to these blocks from gr-blocks
     but these are deprecated from the gr-blocks namespace and the shim is
     scheduled for removal in 3.11.
- gr::blocks::pdu namespace has been reorganized in gr
   - PDU vector types are accessible in gr::types
   - PDU functions are accessible in gr::pdu
   - Common msg port names are accessible in gr::ports
- Logging Infrastructure changed to use spdlog
   - +dependency spdlog, -dependency Log4CPP
   - New, more convenient logging methods
   - Modernized Interface
   - Removed iostream and cstdio from logging statements

#### Project Scope

- C++17
  - requires MSVC 1914 (Microsoft VS 2017 15.7)
  - replace boost::filesystem with std::filesystem
- Windows build: removed unnecessary MSVC-specific system include overrides
- Removed unused volk_benchmark
- Use Pre-Compiled Headers - speeds up compilation time
- Further replacements of boost::bind with lambda functions
- Remove more manual memory management and general c++ modernization
- PEP8 formatting applied and enforced on all Python files
- Centralized min dependency and compiler versions in one place for GR and modtool created OOTs
- Update QA tests to work with OpenEmbedded cross compilations
- Dependency versions:
  - Python 3.6.5
  - numpy 1.17.4
  - VOLK 2.4.1
  - CMake 3.16.3
  - Boost 1.69
  - Mako 1.1.0
  - PyBind11 2.4.3
  - pygccxml 2.0.0
- Compiler options:
  - GCC 9.3.0
  - Clang 11.0.0 / Apple Clang 1100
  - MSVC 1916 (Microsoft VS 2017 15.9)
- Replace deprecated distutils in CMake macros
- Build targets with python dependencies conditionally on `ENABLE_PYTHON`


#### gr-blocks

- Remove deprecated networking blocks: `udp_source`, `udp_sink`, `tcp_server_sink`; replaced
  in 3.9 with more capable blocks in `gr-network`
- Document the supported operations in transcendental

#### gr-analog

- `fastnoise_source`: Use `uint64_t` seed API, use `size_t` for vector length/indices
- `fastnoise_source`: Use a simple bitmask if the random pool length is a power
  of 2 to determine indices, instead of `%`, which consumed considerable CPU
- `sig_source`: Remove deprecated `freq` message port of signal source block; Use `cmd` port instead

### gr-filter

- Remove deprecated `mmse_interpolator` block; Replaced previously by `mmse_resampler`
- Speed up filter building with moves
- Add const to temporary tap vectors

### gr-digital

- Remove deprecated simple_{correlator,framer}
- Remove deprecated cma, lms, kurtotic equalizers; replaced in 3.9 by `linear_equalizer`
- Un-deprecate pfb_clock_sync
- Add header payload demux example
- Remove crc32 utility and most of packet_utils
- Remove yml files for non-existent QAM mod/demod blocks

#### gr-dtv

- Refactor ATSC blocks to have separate metadata stream rather than passing structs
- Add energy normalization for DVB-S2X constellations

#### gr-network

- Fix segfaults when TCP and UDP are restarted

#### gr-qtgui

- Remove spurious volk includes
- Fix segfaulting overflow in time_sink and waterfall
- Support for Qwt 6.2
- Frequency/Waterfall Sinks expand range to 32k and enums in GRC
- Frequency/Waterfall Sinks force power of 2 for fft size

#### gr-uhd

- Python bindings for RFNoC blocks

#### gr-utils

- gr_modtool bind workaround for pygccxml incompatibility with spdlog

#### gr-video-sdl

- Clean up the SDL sinks:
   -  Remove unused format parameter

#### gnuradio-runtime

- `gr::random` uses xoroshiro128+ internally, takes `uint64_t` seed
- Remove unused misc.cc/h
- Accelerator Device Support
    - Major changes to the runtime to support "custom buffers"
    - Single Mapped Buffer abstraction that can be inherited out of tree
- Remove Tag Checker
- Explicitly convert path to string to fix MSVC build
- size_t for vmcircbuf constructor and buffer factories
- Reconfigurable timer value for input blocked condition (scheduler detail workaround)

#### grc

- grcc --output switch for hierarchical blocks
- Clean up Bokeh server loop
- Don't blacklist `default` as a flowgraph ID to prevent always starting in an error state

#### Testing

- rm dependencies from disabled components in blocks/runtime tests

#### Misc.

- dtools: Added run-clang-tidy-on-codebase, which does what the name suggests,
  then updates all bindtool hashes, and commits everything appropriately
- `gr_filter_design`
  - update to support PyQt5
  - fix loading of previously saved .csv files

### Added

- New in-tree module gr-pdu
- New in-tree module gr-iio

## [3.9.5.0] - 2022-01-XX

This is an API compatible update to GNU Radio 3.9. Code written for 3.9.X versions should compile and link without modification. The ABI is not guaranteed to be compatible, so a rebuild of OOT modules may be necessary.

### Changes

#### GRC
- Fix: dependent variables sometimes fail to evaluate
- Modify and cleanup bokeh server loop

#### gr-analog
- Update python bindings for power squelch

#### gr-dtv
- Add energy normalization for DVB-S2X constellations.

#### gr-filter
- Remove pyqwt and qt4 from filter_design

#### gr-network
- Fix: segfaults when TCP & UDP blocks are restarted
- Add throttle flag to the tcp source and sink blocks

#### gr-qtgui
- Enable use of Qwt 6.2

#### gr-uhd
- Add Python bindings for rfnoc_{block, ddc, duc, rx/tx_radio}

#### modtool
- Improvements in generated QA code

#### Build System
- Better support for cross-compiling (OpenEmbedded)
- Find log4cpp on Ubuntu systems
- Handle optional components in `find_package`
- Add version check for pygccxml

#### CI/QA
- Update tests to work with OpenEmbedded


## [3.9.4.0] - 2021-10-25

This is an API compatible update to GNU Radio 3.9. Code written for 3.9.X versions should compile and link without modification. The ABI is not guaranteed to be compatible, so a rebuild of OOT modules may be necessary.

### Changes

#### Regressions Fixes

- Remove `#include <filesystem>` (C++17 feature) from one file
- Restore `pyqwidget()` in gr-qtgui

#### GRC

- GRC now runs on Fedora 35 ... Gtk initialization checks were too strict
- Add keyboard shortcuts for zoom
- Account for scale factor when computing drawing area size
- Use font size from config for block comments
- Change type aliasing to allow interleaved short/byte to be connected to vectors of short/byte. Stricter type checking was added previously and caused some blocks to be unconnectable when using these types.
- Required params no longer default to `0` when left empty. This caused hard-to-find errors. Older flowgraphs that have empty required fields will need to be fixed.
- Tooltips fixed for categories and modules

#### gr-runtime

- PMT uses the VOLK allocator for vectors
- `get_tags_in_window()` Python wrapper calls the correct function
- Add `--pybind` option to `gnuradio-config-info` to get PyBind11 version

#### gr-blocks

- Add example for XMLRPC
- Add a unit test for Message Strobe
- Fix C++ support for Unpacked to Packed

#### gr-channels

- RNG seeds are initialized correctly

#### gr-digital

- Fix yml file for Header/Payload Demux

#### gr-network

- Suppress warning in tuntap

#### gr-qtgui

- Remove unusable int type in Number Sink yml
- Use `no_quotes()` function in several yml files

#### modtool

- Hashes can be fixed using modtool using `--update-hash-only`
- Use `tempfile()` instead of `/tmp` in bindtool and modtool
- Use `static_cast` instead of `reinterpret_cast` in templates
- Correct broken Python general block template

#### Build system

- Better check for Boost version
- Determine the Python prefix more reliably
- Use GR-specified compiler standard (C++14) in gr-soapy, instead of the SoapySDR standard (C++11)
- GrPython.cmake is compatible with older cmake

#### Documentation

- Man pages are in-tree
- Repair many examples, especially in gr-digital
- Allow UTF-8 in pydoc templates

#### CI

- Execute `make install` during test
- Add Ubuntu 18.04 test target

## [3.9.3.0] - 2021-09-30

This is an API compatible update to GNU Radio 3.9. Code written for 3.9.X versions should
compile and link without modification. The ABI is not guaranteed to be compatible, so
a rebuild of OOT modules may be necessary.

### Changes

#### General

- Many cleanups and some C++ modernization changes
- Replace Boost with stdc++ equivalents (ongoing effort)
- Logging cleanup and performance improvements (ongoing effort)
- Cleanup of many unused includes
- Various block yaml cleanups and repairs
- Use Soapy instead of UHD for flowgraph testing in gr-analog and gr-dtv

#### GRC

- Improvements in C++ templates and code generation, more blocks are usable
- Add specification of packages to find (via cmake) for C++ templates
- Fix C++ hier block param template
- Add GUI hints for widgets in C++ code generation
- Add no_quotes() convenience function to strip quotes from strings, callable from templates
- Allow short and byte as valid types in an enum
- Fix desync when dragging block (block would not always track cursor)
- Correctly evaluate interdependent variables
- Allow error messages to be copied to clipboard
- Update disabled blocks if they depend on others

#### gnuradio-runtime

- Deprecate tag_checker class (will be removed in 3.10)
- Detect and follow symbolic links for installation prefix in gr::prefix()
- Add "<" operator for comparison of tags (instead of offset_compare())

#### gr-blocks

- New Matrix Interleaver block
- Throttle and Head blocks can be input-only when no output is needed, improving performance
- Fix tag propagation in (un)pack-k-bits blocks
- Fix namespace for nco and vco in benchmarks

#### gr-digital

- New Async CRC16 block
- Add NRZI option to Differential En/Decoder
- Constellation performance improvements
- Fix constellation normalization by average power
- Remove unimplemented msg output port from Chunks to Symbols block
- Make unpacking of bits optional in GMSK modulator so the blocks can accept unpacked bits
- In GFSK/GMSK hier blocks, replace M&M clock recovery with newer and more capable Digital Symbol Sync block

#### gr-fec

- Cleanups and code improvements

#### gr-fft

- Add a "shift" paramenter to Log Power FFT, to place DC bin at center

#### gr-filter

- Performance improvement in PFB Arbitrary Resampler when interpolating, especially at low rates

#### gr-qtgui

- Fix display of tags on the last sample (would be dropped)
- Python wrapping: replace pyqwidget() with qwidget() - this is not intended to be a visible fix, but is mentioned here in case it causes trouble for anyone
- C++ generation
  - add double quotes to cpp_opts keys with colons
  - position windows correctly depending on gui_hint
  - use option attributes to generate enums
  - use the cpp enums for wintype and trigger_type

#### gr-uhd

- Fix input filename in freq hopping examples
- Make RFNoC a separately enabled component in cmake
- Python bindings for rfnoc_graph,_rx/tx_streamer

#### gr-video-sdl

- U and V channels were reversed on sink blocks

#### gr-soapy

- Added message support for SoapySDR 0.8 API

#### modtool

- Set VERSION_PATCH to 0 instead of "git" in new modules
- Fix "rm", "bind", disable", "rename" and "makeyml" which had unexpected side effects, or did not work as a user would expect

#### Build System

- Correct minimum version checking for Mako
- Ensure that RC_*_VERSION are numeric (Windows)
- Fix finding libunwind
- Pass through extra arguments to GR_PYTHON_INSTALL to install command
- Remove absolute paths and private links from exported targets
- Add gir1.2-gtk-3.0 as deb runtime dependency

#### Documentation

- Code of Conduct updates and link to Wiki

#### CI

- Do not error out on deprecations, allowing testing of deprecated code

## [3.9.2.0] - 2021-06-10

### Changed

#### SoapySDR support (gr-soapy)
The new gr-soapy built-in module provides access to Soapy hardware drivers using the SoapySDR driver framework. See https://wiki.gnuradio.org/index.php/Soapy for more information.

If SoapySDR 0.7.2 or newer is available during GNU Radio configuration, the gr-soapy module will be enabled.

The SoapySDR framework and Soapy driver modules are not maintained by the GNU Radio project. Driver modules are dynamically discovered and linked. They may be added and updated independently from the GNU Radio release cycle.

#### GRC

- Parameter expressions and/or values can be displayed in blocks on the flowgraph. Previously, only values were displayed. Look for the "Show Parameter ..." toggles under the View menu.
- Deprecated blocks are shown in a distinct color (orange), and the optional `deprecated` property has been added to block yaml
- Dark theme works better, especially for parameter fields
- Vector length is now correctly applied to all input ports
- Validation has been improved. Raw types are validated. Port connections are checked by type rather than by item size.
- Variable names that cause name conflicts in Python code (e.g., with package names) are rejected
- Bus logic fixes
- Blocks can no longer be dragged off the screen and lost forever

#### gnuradio-runtime

- Remove `pmt::pmt_*` functions, which were not actually implemented. This is technically an API change, but any use of the API would have resulted in a link error.
- Remove Python bindings for internal buffer functions

#### gr-analog

- Added an example for PLL Frequency Detector
- Better names for parameters in PLL GRC blocks

#### gr-blocks

- Consistent naming for Vector Length parameter in GRC blocks
- `count_bits` uses VOLK `popcnt` for better performance (used for example in the Correlate Access Code block)
- Rotator block phase increment parameter is controllable via a message, and a tag can be added at the point where the increment is adjusted

#### gr-digital

- MPSK example updated to use a Linear Equalizer (replacing deprecated block)

#### gr-filter

- Filter Delay documentation improvement
- Interpolating FIR filter can generate C++ code

#### gr-dtv

- VL-SNR bugs fixed (incorrect constants).

#### gr-qtgui

- Improve autoscaling for vector sinks
- Fix floating-point resolution problems in several widgets, due to interpretation of PMT doubles as floats

#### gr-uhd

- Add policy enum to Python bindings for `tune_request`
- Additional time spec options on UHD blocks (PC Clock on Next PPS, GPS Time on Next PPS)
- Fix up code that was generating warnings
- Fix command handler logic to apply commands from messages to the correct channel

#### gr_filter_design

- "File/Save" is disabled until the taps have been computed, and the GUI is reset after a save, to make it clearer which data is being saved.
- Entries are hidden for parameters that do not apply to the selected filter type.
- Save window type as an integer instead of a Python enum name

#### gr_modtool

- Improve validation of module and block names

#### Build System and Infrastructure

- Improved messages related to dependencies MPIR and GMP. Only one of these packages is required, and the previous error messages were confusing.
- Cleaner builds for Windows and Conda. A number of general cmake improvements have resulted from this work.
- In-tree packaging files for DEB and RPM, used with Launchpad and COPR
- Added man pages for GNU Radio tools
- Test code generation for all in-tree GRC examples
- In GrPybind.cmake, `PYBIND11_INCLUDE_DIR` (incorrect) was changed to `pybind11_INCLUDE_DIR`

### Contributors
At LEAST the following authors contributed to this release. Note that only authors of commits are included here. A number of people contribute in other ways, including code review, documentation and testing.

- 0xloem <0xloem@gmail.com>
- Christophe Seguinot <christophe.seguinot@univ-lille.fr>
- Chuang Zhu <genelocated@yandex.com>
- Codey McCodeface <Codey.McCodeface@gmail.com>
- Ferenc Gerlits <fgerlits@gmail.com>
- Håkon Vågsether <hauk142@gmail.com>
- Igor Freire <igor@blockstream.com>
- Jacob Gilbert <jacob.gilbert@protonmail.com>
- Jeff Long <willcode4@gmail.com>
- Josh Morman <jmorman@perspectalabs.com>
- Liu, Andrew Z <liu.andrew@vast-inc.com>
- Marcus Müller <mmueller@gnuradio.org>
- Martin Braun <martin@gnuradio.org>
- Martyn van Dijke <martijnvdijke600@gmail.com>
- Nicholas Corgan <n.corgan@gmail.com>
- Ron Economos <w6rz@comcast.net>
- Ryan Volz <ryan.volz@gmail.com>
- Solomon Tan <solomonbstoner@yahoo.com.au>
- Volker Schroer
- Zackery Spytz <zspytz@gmail.com>

## [3.9.1.0] - 2021-03-17 "Luck-O-The-Radio"

API is compatible with C++ code written against previous v3.9 releases.

ABI (link time) compatibility is not guaranteed. Out-of-tree C++ code
linked to previous v3.9 releases should be rebuilt against this version.

### Project Scope
- (internal) Build system improvements and fixes
- (internal) Use C++14 features instead of Boost where possible
- Fewer compiler warnings (due to fixed code, not removal of warnings!)

### OOT Support
- cmake: standard FindGSL used in lieu of custom one, so OOTs should no longer
  copy this module from the GNU Radio tree if needed
- modtool: better exception/error handling
- modtool: will non-interactively add blocks with no parameters
  (`--argument-list=""`)
- PyBind11 header hash hotfixer
  (gr-utils/bindtool/scripts/binding-hash-hotfixer.zsh) makes life easier

### GRC
- Block param values can be shown as expressions and/or evaluated values. Under
  the View menu, see "Show parameter ..." options.
- Better handling of gui hints, avoid hiding widgets in cases where some have
  GUI hints and some do not
- Ensure that strings are valid utf8 when evaluating parameters
- Save embedded python blocks/modules to hier_block_directory
- Save config and update recent file sub-menu before executing flowgraph
- Revert to previous "block id" display behavior
- Fix drag n' drop issue with Quartz (macOS) backend
- C++ generated code: remove gnuradio prefix from linked libraries
- Python generated code: prefix generated modules with flowgraph id
- connect() will be called for hier blocks even if they don't have ports
- Check for PyYAML 3.11 (was checking for 3.10 even though 3.11 was required)

### gnuradio-runtime
- Default buffer size can be adjusted by setting `buffer_size` in the
`[Default]` section of the GNU Radio config file. Otherwise, the longstanding
  default of `32768` is used.
- Windows/Python: add DLL search path, as required by newer versions of Python

### gr-audio
- Windows: reduce priority of Windows native audio to LOW, preferring Portaudio

### gr-blocks
- Message Debug: prints warning when a non-PDU message is received on the
  Print PDU port
- File Source handles EOF properly in non-seekable files (e.g., pipes)
- Several bounds checking and boundary condition improvements
- Remove DC Spike AutoSync algorithm fixed

### gr-digital
- MSK timing error detector improvement
- Python bindings include control_loop parent class in child class
- Correlation Estimator parameter `threshold` well documented

### gr-fft
- New window types: Nuttall, Blackman Nuttall, Nuttall CFD, Welch, Parzen,
  Exponential, Riemann, Gaussian and Tukey

### gr-filter
- Taps blocks (e.g., Low Pass Filter Taps) now propagate parameter changes
- Filter Designer: IIR/Band Pass/Bessel works

### gr-qtgui
- Add toggling label and icon on plot play/stop button
- Time Raster Sink: axis labels and ranges are configurable
- Qt Chooser: label improvements

### gr-uhd
- DEPRECATION: UHD blocks currently accept control messages (e.g. for
  frequency change) as PMT pairs and tuples, in addition to the intended
  PMT dict format. In v3.11, only the dict format will be accepted.
- Command note: `freq` messages in this version must be dicts. Additionally,
  the `direction` key must be present and set to `TX` or `RX`.
  Python example: `pmt.to_pmt({'freq':100e6,'direction':'RX'})`
  The original behavior may be restored in an update, but code should be
  converted to use the dict format in preparation for future versions.
- `set_filter()` adds `chan` parameter
- Python generated code improved for FE corrections
- Allow control of frequency and gain in both directions at the same moment
- Add command handler for mtune ("manual tune")
- Add GPIO control via messages
- Add start time param to GRC blocks
- Support streaming modes: `STREAM_MODE_START_CONTINUOUS`,
  `STREAM_MODE_STOP_CONTINUOUS`, `STREAM_MODE_NUM_SAMPS_AND_DONE`,
  `STREAM_MODE_NUM_SAMPS_AND_MORE`
- Documentation improvements
- Tag sink demo example
- Better logging of over/underflows, time errors and time tags
- Added Python binding for ALL_LOS (all local oscillators)
- `uhd_fft` application X11/Qt related cleanups

### QA
- `waitFor(condition, ...)` available for tests that use timeouts

### Contributors
At LEAST the following authors contributed to this release.

- Artem Pisarenko <artem.k.pisarenko@gmail.com>
- Christophe Seguinot <christophe.seguinot@univ-lille.fr>
- Daniel Estévez <daniel@destevez.net>
- David Pi <david.pinho@gmail.com>
- Emmanuel Blot <emmanuel.blot@free.fr>
- gnieboer <gnieboer@corpcomm.net>
- Håkon Vågsether <hauk142@gmail.com>
- Ipsit <mmkipsit@gmail.com>
- Jacob Gilbert <jacob.gilbert@protonmail.com>
- Jason Uher <jason.uher@jhuapl.edu>
- Jeff Long <willcode4@gmail.com>
- Jason Uher <jason.uher@jhuapl.edu>
- Jeppe Ledet-Pedersen <jlp@satlab.com>
- Josh Blum <josh@joshknows.com>
- Josh Morman <jmorman@perspectalabs.com>
- karel <5636152+karel@users.noreply.github.com>
- luz paz <luzpaz@users.noreply.github.com>
- Marcus Müller <mmueller@gnuradio.org>
- Martin Braun <martin@gnuradio.org>
- Nick Foster <bistromath@gmail.com>
- Niki <niki@aveer.io>
- rear1019 <rear1019@posteo.de>
- Ron Economos <w6rz@comcast.net>
- Ryan Volz <ryan.volz@gmail.com>
- Sebastian Koslowski <sebastian.koslowski@gmail.com>
- Terry May <terrydmay@gmail.com>
- Vasilis Tsiligiannis <acinonyx@openwrt.gr>
- Victor Wollesen <victor.w@pervices.com>
- Volker Schroer
- Zackery Spytz <zspytz@gmail.com>

## [3.9.0.0] - 2020-01-17

The future is not set, there is no fate but what we make for ourselves. In this
very spirit, GNU Radio 3.9 packs a whole bunch of power when it comes to
transforming the way GNU Radio and its ecosytem can be developed in the future.

Not only did we have great progressions from old dependencies that proved to be
all too problematic (SWIG, Python2), but also did we see an incredibly influx of
people actively working on how maintainable this code base is. This will nurture
the project for years to come.

All in all, the main breaking change for pure GRC users will consist in a few
changed blocks – an incredible feat, considering the amount of shift under the
hood. Mentioning large shifts, the work that went into the PyBind binding, the
CMake modernization, the C++ cleanup, the bug-fixing and the CI infrastructure
is worthy of explicit call out; I especially thank

* Josh Morman
* Thomas Habets
* Jacob Gilbert
* Andrej Rode
* Ryan Volz

here.

For developers of OOTs, I'm sure PyBind11 will pose a surprise. If you're used
to SWIG, yes, that's more code to write yourself. But in effect, it's less code
that breaks, and when it breaks, it breaks in much more understandable ways.
Josh has put a lot of effort into automating as much of that as possible.
There's certainly no shortage of demand for that! The ecosystem (remember GNU
Radio's tagline?) is in a steady upwind. We've seen more, and more stable,
contributions from OOT maintainers. That's great!

For in-tree development, newer dependencies and removal of anachronisms will
make sure things move much smoother. Our CI is getting – lately literally every
day – better, which means we not only catch bugs earlier, but also allow for
much quicker review cycles.

One central change:

If you're contributing code upstream, we no longer need you to submit a CLA;
instead, we ask you to just certify, yourself, that you're allowed to contribute
that code (and not, e.g. misappropriating someone else's code).

That's what the DCO (Developer Certificate of Origin) is: Just a quick, "hey,
this code is actually for me to contribute under the project's license"; nothing
more.

Which means that our infrastructure will refuse to let your code into the tree
if you didn't add a

`Signed-off-by: Maxime Example <maxime@example.com>`

to your commit message. Luckily, git can do that for you: `git commit -s`.

### Changed

#### Project Scope

- **We now require commits to be signed off (DCO)**; this means you have to
attach `-s` to your `git commit` command line
- License headers now SPDX format
- C++14
 - use C++11 facilities in a lot of places where Boost was still used,
   especially smart pointers, range loops
 - lambdas where `boost::bind` was used
- C11
- Dependency versions:
  - Python 3.6.5
  - numpy 1.13.3
  - VOLK 2.4.1
  - CMake 3.10.2
  - Boost 1.65
  - Mako 1.0.7
  - PyBind11 2.4.3
- Compiler options:
  - GCC 8.3.0
  - Clang 11.0.0 / Apple Clang 1100
  - MSVC 1910 (Microsoft VS 2017 15.0)
- VOLK now "regular" dependency, not in-tree submodule
- numpy now also a CMake-checked hard dependency for Python support
- Exception Handling: throw by value, catch by reference (clang-tidy check)
- C++11: Emplace in vectors where you can; brings performance boni, but not
  included in clang-tidy-checks
- Further clang-tidy based code optimizations:
- empty() instead of size() == 0
- override where overriding virtual functions (which we do a lot)
- Logging: removed all `std::cerr` and `fprintf(stderr,…)` by GNU Radio logging
- Logging: Changed logging format for many multiline error logs
- purged `snprintf`, `printf` logging
- There were a lot of places where a malloc'ed object was used internally, where
  that was inappropriate. Using simple instance-holding fields now.
- `const` for members that were only set at construction time is now desired,
  and implemented in most places
- `const` -> `constexp` in a lot of places.
- `assert` -> `static_assert`
- An exception-throwing block will now terminate the flow graph process,
  configurable through `top_block`
- `gr-utils` cleanup, folder restructuring
- config version checks installed CMake file will accept "at least this version"
  now
- PyBind11 replaces SWIG
  - Full tree conversion from SWIG to Pybind11 bindings
- Doxygen now uses MathJax, full LaTeX installation no longer required

**NOTE**: Most of the changes above change the generally preferred coding style
in a lot of situations.

#### GRC

- start flowgraph in folder where it resides

#### gnuradio-runtime

- When calculating offsets in non-integer rate FEC, `lround`
- default seed for `gr::random` now actually as documented time-dependent
- loggers moved from `gr::block` to `gr::basic_block`
- PMT serialization
- PMT dicts no longer indistinguishable from pairs
- PMT symbol hashing no longer suffers under oddball own implementations

#### gr-audio

- Increased ALSA buffer nperiods

#### gr-blocks

- `add`, `add_const` VOLK'ized, templated
  - this seems to break things in other places, even if it proves to be
    mathematically identical
- `wavfile` infrastructure: `libsndfile` now dependency

#### gr-digital

- `transcendental` block: default to 32 bit float complex, not double
- Linear equalizer: separate adaptive algorithm, allows for using e.g. trained
  sequences instead of the classical LMS, CMA
- DFE: better structure for decision history

#### gr-dtv

- LDPC encoder: template functions instead of `#define`d macros
- LDPC encoder: smaller tables through `uint16_t` for index tables

#### gr-fec

- API `uint8_t`, not `char`

#### gr-fft

- FFT blocks/functions templatized

#### gr-filter

- logging format
- `rational_resampler_base` -> `rational_resampler`

#### gr-uhd

- Required UHD version bumped to 3.9.7
- logging format

### Deprecated

#### gr-analog

- `sig_source`: `freq` port will be removed in the future

#### gr-audio

- `audio-sink`, `-source`: Windows audio sink/source deprecated, the portaudio
sink/source works even better under windows anyway

#### gr-digital

- In favor of `symbol_sync`, deprecate:
  - `clock_recovery_mm`
  - `msk_timing_recovery`
  - `pfb_clock_sync`

### Added

#### Project Scope

- C++ Generation all over the place
- PyBind bindings + generator
- Github actions
- Reproducible builds-compatible CMake TIMESTAMP

#### gnuradio-runtime

- `block_gateway`: `set_max_output_buffer`
- `GR_PREFS_PATH` environment variable sensitivity to configure the path to the
  config file
- `gnuradio-config-info --print-all`

#### GRC

- option to toggle ID visibilities globally
- Validation check for QT GUI hints
- Python snippets

#### block header parsing tool

- block header parsing tool (GSoC 2019)

#### gr_modtool

- option to convert blacklisted files

#### gr-analog

- `sig_source`: `cmd` port adds support for dicts, setting of frequency,
amplitude, offset and phase parameters

#### gr-blocks

- `selector` now has control message ports
- Rotator-based freq shift convenience wrapper
- Message-to-Variable and vice versa blocks
- DC Spike removal
- IQ Swap
- Complex to interleaved char / short: scaling option
- Delay block: control message port
- Phase Shift block with message port
- `wavfile_sink`, `_source` can now deal with a lot of audio formats:
  uncompressed WAV/AIFF, µ- and A-law compressed audio, OGG/Vorbis, FLAC, even
  octave files
- Stream Demux, which demuxes streams according to lengths vector
- `rotator`: `phase()` getter

#### gr-digital

- OFDM: multiple CP lengths
- `ofdm_equalizer_simpledfe`: `enable_soft_output`
- Constellation Encoder
- Constellation: normalization options

#### gr-fec

- `{en,de}code_rs_8`, `{en,de}code_rs_ccsds`: Reed-Solomon en- and decoders

#### gr-fft

- Windows:
  - Gaussian
  - Flat Top
  - Tukey
- Window build() call now with default beta

#### gr-filter

- GRC: File taps loader block
- Low pass FFT filter convenience wrapper
- ichar / ishort decimator
- phase continuity for `freq_xlating_fir_filter`

#### gr-network

- `gr-network`: a whole new networking blocks module!
  - TCP
  - UDP
- Much better lockup/multithreading support than 3.7-era `blks2` nightmare
  infrastructure :)


#### gr-qtgui

- Azimuth/Elevation plot
- Autocorrelation plot
- Compass visualization
- Dial control
- Gauge: dial, level
- Distance plot
- LED-like indicator
- Message-passing check box
- Message-passing numeric control
- Message-passing push button
- Toggle Button
- Eye sink
- Vertical slider

#### gr-uhd

- Filter API
- UHD 4.0 support
- Power Reference API
- Bidirectional setting messages on both sink, source

#### gr-vocoder

- Codec2 dev branch support
- FreeDV: In/output rates can differ
- FreeDV: text message output

#### gr-zeromq

- C++ GRC templates
- Tag filtering for tag-forwarding blocks

### Removed

#### Project Scope

- VOLK is no longer a submodule
- Sphinx: consolidate into doxygen, or wiki-maintained block list.
- Python 2
- SWIG
- `gru` python module

#### gnuradio-runtime

- `circular_file.cc`
- `math/common_factor.hpp`

#### gr-blocks

- `bin_statistics_f`
- `log2_const`

#### gr-digital

- PFB clock sync: `set_taps`
- deprecated old OFDM infrastructur
  - `ofdm_frame_acquisition`
  - `ofdm_frame_sink`
  - `ofdm_insert_preamble`
  - `ofdm_sync_fixed`
  - `ofdm_sync_pn`
  - `ofdm_sync_pnac`
  - `ofdm_sync_ml`
  - `ofdm_receiver`
- `digital_voice`

#### gr-fft

- `malloc_float`, `_double`: rely on VOLK
- Goertzel: dtor superfluous

#### gr-filter

- deprecated window function duplicates (use them from gr-fft!)

### Fixed

#### Project Scope

- CMake: Qwt, Log4Cpp detection
- ctrlport strings unicodified
- Freedesktop install script was not executed
- Redundant icons installed
- Path substitution on Windows was backslash-broken
- YAML definitions: more than I can count
- Cross-building: py interpreter at runtime != build time

#### gnuradio-runtime

- ctrlport: unholy stored reference to stack-allocated object removed
- Sine table generation for fixed point math
- `gr_unittest`: `floatAlmostEqual` had a lot of false passes due abuse of
  `all()`
- `get_tags_in_range` for `delay < (end-start)`
- Premature tag pruning
- release flattened flowgraph after stopping, fixes restartability/shutdown
  problem
- PMT serialization portability
- latency issue caused by setting block alias on msg block
- Windows logging errors
- ctrlport: Thrift >= 0.13 broke

#### GRC

- Tab widget ID visibilities
- A lot of YAML templates
- Default setting in qtgui chooser restored
- Boolean parameters no longer switch buttons
- Nested namespace handling
- Don't rely on set ordering in tests
- configparser import
- input box color theme on dark themes
- Search box typing doesn't inadvertedly interact with the rest of GRC anymore

#### gr_modtool

- Empty argument lists allowed
- Boost UTF replaced CppUnit, this needed to be done here, too

#### gr-analog

- `wfm` left/right, filters

#### gr-audio

- portaudio: lock acquisition was improper

#### gr-blocks

- Throttle now uses monotonic clock
- Tag debug only saved last `work` call's tags
- File sink flushes on `stop`
- `gr_read_file_metadata.py` used to lose `rx_time` precision
- File source big file handling under Windows
- `file_*`: `fseek` errors used to be ignored

#### gr-digital

- `map_bb`: thread safety, buffer overflows
- `additive_scrambler`: reset was broken
- Constellation scalefactor wasn't always initialized
- long-standing `qa_header_payload_demux` bug addressed by waiting for both RX
  and TX, not only either
- false triggers in `correlate_access_code`

#### gr-dtv

- rate mismatch in ATSC flowgraphs

#### gr-fec

- `async_decoder` Heap corruption
- `cc_encoder`: constraint length K > 8 led to wrong output

#### gr-fft

- thread safety of copy assignment/ctor
- log power FFT Python

#### gr-filter

- `variable_band_pass_filter` GRC complex taps input
- RRC filter gain for alpha = 1

#### gr-qtgui

- Remove copies of image data in returns by using move semantics
- Remove bogus overriding in drawing functions of `plot_raster`, `_waterfall`
- Edit MSG box: don't require key to be set
- Don't check for Python2 libs
- Number Sink ignored averaging setting

#### gr-uhd

- UHD apps: Py3 fixes
- USRP blocks: multichannel objects not properly populating channels

#### gr-video-sdl

- YUV formats fixed

#### gr-zeromq

- Don't depend on deprecated ZMQ functionality (fix warnings, include what you
  use)
- Unhandled exceptions now handled, much calmer
- Avoid infinite blocking in `tb.stop()` by using `ZMQ_LINGER`

#### gr-uhd

- Change of log method, and increase message verbosity

### Added

#### gr-uhd

- Added `gr::prefs`-configurable logging interval for underflow, overflow and
  command time error (`[uhd]` section, `logging_interval_ms` key, milliseconds
  integer)

## [3.8.5.0] - 2022-01-XX

This is an API compatible update to GNU Radio 3.8. Code written for 3.8.X versions should compile and link without modification. The ABI is not guaranteed to be compatible, so a rebuild of OOT modules may be necessary.

#### GRC
- GRC now runs on Fedora 35 ... Gtk initialization checks were too strict
- Fix: dependent variables sometimes fail to evaluate
- Change type aliasing to allow interleaved short/byte to be connected to vectors of short/byte. Stricter type checking was added previously and caused some blocks to be unconnectable when using these types.
- Account for scale factor when computing drawing area size
- Tooltips fixed for categories and modules
-
#### gr-digital
- Fix yml file for Header/Payload Demux

#### gr-dtv
- Add energy normalization for DVB-S2X constellations.

#### gr-filter
- Remove pyqwt and qt4 from filter_design

#### gr-qtgui
- Enable use of Qwt 6.2
- Remove unusable int type in Number Sink yml
- RangeWidget - implement Eng & EngSlider

At LEAST the following authors contributed to this release.

- Bill Muzika <bill.muzika@outlook.com>
- Chris <christopher.donohue@gmail.com>
- Chris Vine <vine35792468@gmail.com>
- Clayton Smith <argilo@gmail.com>
- Doron Behar <doron.behar@gmail.com>
- Jeff Long <willcode4@gmail.com>
- John Sallay <jasallay@gmail.com>
- Josh Morman <jmorman@gnuradio.org>
- Marcus Müller <mmueller@gnuradio.org>
- Mark Pentler <tehhustler@hotmail.com>
- Martin Braun <martin@gnuradio.org>
- masw <masw@masw.tech>
- Matt Mills <mmills@2bn.net>
- Nick Østergaard <oe.nick@gmail.com>
- Ron Economos <w6rz@comcast.net>
- Ryan Volz <ryan.volz@gmail.com>
- Sec <sec@42.org>
- Volker Schroer

## [3.8.4.0] - 2021-09-30

API is compatible with C++ code written against previous v3.8 releases.

ABI (link time) compatibility is not guaranteed. Out-of-tree C++ code
linked to previous v3.8 releases should be rebuilt against this version.

### Changed

#### GRC

- Fix drag and drop issue with Quartz
- Fix desync when dragging block
- Update disabled blocks if they depend on others
- Allow short and byte as valid types in an enum
- Fix evaluation of interdependent variables

#### modtool

- Set VERSION_PATH to 0 in new modules, instead of using GIT rev
- For Python3, return correct exeption ModuleNotFoundError instead of ImportError

#### gr-blocks

- New Matrix Interleaver block

#### gr-channels

- Fix "hide" expressions in yml files

#### gr-digital

- Remove unused msg output port from Chunks To Symbols block yml

#### gr-fft

- Add "shift" parameter to Log Power FFT

#### gr-qtgui

- Fix: tags on the last sample were not shown

#### gr-video-sdl

- Fix: U and V channels were reversed on sink blocks

At LEAST the following authors contributed to this release.

- Adrien Michel <adriengit@users.noreply.github.com>
- David Winter <david.winter@analog.com>
- Emmanuel Blot <emmanuel.blot@free.fr>
- Håkon Vågsether <hauk142@gmail.com>
- Jared Dulmage <jared.dulmage@caliola.com>
- Jason Uher <jason.uher@jhuapl.edu>
- Jeff Long <willcode4@gmail.com>
- Marc L <marcll@vt.edu>
- Marcus Müller <mmueller@gnuradio.org>
- Martin Braun <martin@gnuradio.org>
- Ron Economos <w6rz@comcast.net>
- Volker Schroer

## [3.8.3.1] - 2021-06-10

This is a PATCH level revision. The API is compatible with C++ code written for previous v3.8 releases. ABI (shared library signature) is intended to be compatible, so code linked against v3.8.3.0 should not require recompilation.

### Changed

#### Build system

- Improved messages related to dependencies MPIR and GMP. Only one of these packages is required, and the previous error messages were confusing.

#### GRC

- Parameter expressions and/or values can be displayed in blocks on the flowgraph. Previously, only values were displayed. Look for the "Show Parameter ..." toggles under the View menu.
- Vector length is now correctly applied to all input ports.
- Validation has been improved. Raw types are validated. Port connections are checked by type rather than by item size.
- Variable names that cause conflicts in Python code (e.g., package names) are rejected.
- Bus logic fixes.
- Blocks can no longer be dragged off the screen and lost forever.

#### gr-dtv

- VL-SNR bugs fixed (incorrect constants).

#### gr-qtgui

- Improve autoscaling for vector sinks.

#### gr_filter_design

- "File/Save" is disabled until the taps have been computed, and the GUI is reset after a save, to make it clearer which data is being saved.
- Entries are hidden for parameters that do not apply to the selected filter type.

At LEAST the following authors contributed to this release.

- 0xloem <0xloem@gmail.com>
- Christophe Seguinot <christophe.seguinot@univ-lille.fr>
- David Pi <david.pinho@gmail.com>
- Igor Freire <igor@blockstream.com>
- Jeff Long <willcode4@gmail.com>
- Josh Morman <jmorman@perspectalabs.com>
- Marcus Müller <mmueller@gnuradio.org>
- Ron Economos <w6rz@comcast.net>
- Solomon Tan <solomonbstoner@yahoo.com.au>
- Volker Schroer
- Zackery Spytz <zspytz@gmail.com>

## [3.8.3.0] - 2021-03-17

API is compatible with C++ code written against previous v3.8 releases.

ABI (link time) compatibility is not guaranteed. Out-of-tree C++ code
linked to previous v3.8 releases should be rebuilt against this version.

### Changed

#### Project Scope

- Preference file path may be set using the `GR_PREFS_PATH` environment variable
- Lower message latency, previously slow when block aliases were used
- PMT: `is_dict()` no longer returns True for PMT pairs, only for PMT dicts
- Util gnuradio-config-info gets --print-all for those that want it all
- Allow Thrift versions up thorugh 0.13
- (internal) Build system improvements and fixes
- (internal) Reduce Boost usage by replacing bind() with lambdas supported in C++11

#### OOT Support

- Allow override of gr python dir by setting `-DGR_PYTHON_DIR`
- Modtool will not try to add duplicate block names in a module
- Modtool can non-interactively add blocks with no parameters (`--argument-list=""`)
- Modtool will use current installation prefix instead of /usr/local as source of templates

#### GRC

- Better handling of gui hints, avoid hiding widgets in cases where some have GUI hints and some do not
- Dark theme detection fix
- Add window size option for bokeh gui

#### gr-audio

- ALSA nperiods default increased to 32 frames to reduce drops (can be set in conf file under the `audio_alsa` section)

#### gr-blocks

- File Source handles EOF properly in non-seekable files (e.g., pipes)
- File Source handles large files on Windows
- Several bounds checking and boundary condition improvements

#### gr-digital

- MSK timing error detector improvement
- Improvements to correlation estimator example
- Correlate Access Code will not trigger until it receives enough bits to compare to the access code

#### gr-dtv

- Performance improvements

#### gr-filter

- Filter designer works with Qt5 and Python3
- Taps blocks (e.g., Low Pass Filter Taps) now propagate parameter changes

#### gr-qtgui

- Chooser label defaults now correct
- Frequency Sink max hold plot for half spectrum width option shows correctly
- Number Sink honors setting of 'avg' more consistently

#### gr-uhd

- Add start time parameter to GRC blocks
- Add message handler for GPIO control messages
- Additional master clock rates in GRC blocks
- UHD source sends tags on center frequency change

#### gr-video-sdl

- Correct YUV format options

#### gr-zeromq

- Connections no longer hang when flowgraph stops and other end is still connected (`ZMQ_LINGER` now set)
- Receipt of bad PMTs logged (instead of exception)

### Contributors
At LEAST the following authors contributed to this release.

- alekhgupta1441 <alekhgupta1441@gmail.com>
- A. Maitland Bottoms <bottoms@debian.org>
- Christophe Seguinot <christophe.seguinot@univ-lille.fr>
- Clayton Smith <argilo@gmail.com>
- David Pi <david.pinho@gmail.com>
- Derek Kozel <derek@bitstovolts.com>
- Doron Behar <doron.behar@gmail.com>
- duggabe <barry@dcsmail.net>
- elms <elms@freshred.net>
- gnieboer <gnieboer@corpcomm.net>
- Håkon Vågsether <hauk142@gmail.com>
- Huang Rui <vowstar@gmail.com>
- Jacob Gilbert <mrjacobagilbert@gmail.com>
- Jeff Long <willcode4@gmail.com>
- Johannes Demel <demel@ant.uni-bremen.de>
- Marcus Müller <mmueller@gnuradio.org>
- Martin Braun <martin@gnuradio.org>
- Matt Mills <mmills@2bn.net>
- Michael Dickens <michael.dickens@ettus.com>
- mormj <mormjb@gmail.com>
- Nick Foster <bistromath@gmail.com>
- Nick Østergaard <oe.nick@gmail.com>
- Niki <niki@aveer.io>
- Notou <barthy42@laposte.net>
- rear1019 <rear1019@posteo.de>
- Ron Economos <w6rz@comcast.net>
- Ryan Volz <ryan.volz@gmail.com>
- Sam Schmidt <samuel.schmidt.ee@gmail.com>
- Sebastian Koslowski <sebastian.koslowski@gmail.com>
- Steve Lunsford <lunsford-stephen@vast-inc.com>
- Thomas Habets <thomas@habets.se>
- Vasil Velichkov <vvvelichkov@gmail.com>
- Volker Schroer
- Zackery Spytz <zspytz@gmail.com>

## [3.8.2.0] - 2020-08-21

### Changed

#### Project Scope

- Call SWIG without `-modern` flag if new enough to make that inherent, thus
  removing warnings

#### GRC

- Mako templating now `strict_undefined`
- Show block comments by default
- dynamically updated block namespaces (execute imports from templates)
- Name-lookup detection of dark color themes, adjusted input box color scheme

#### gr-dtv

- `dvb_bbscrambler`: LUT 8B alignment (private, but relevant for future usable
  optimization)

### Fixed

#### GRC

- incorrect "clear console" and "save console" labels
- generator: `flow_graph.py.mako` now contains import for `print_function` for
  compatibility in Py2 generation
- `variable_config.block.yml` now properly handles `configparser` /
  `ConfigParser` import
- Change port types of all selected block on key-press cycling, not just first
  one
- Always check for Python version in flow graph generation, not only in specific
  block types
- Don't run stale flow graphs
- Prevent search keystrokes from modifying flowgraph

#### gr-blocks

- `file_sink` flushes contents to file on `stop()`
- `file_meta_sink` block YAML: emit proper default PMT dict
- XMLRPC client/server block templates: use Py3 XMLRPC when possible

#### gr-digital

- `soft_dec_lut_gen` now explicitly casts `2.0**prec` to `int`
- `map_bb`: add missing definition of static `constexpr size_t s_map_size`

#### gr-dtv

- Reduce `send_frame_size` in UHD examples (broken after UHD v3.13.0.3-rc1)

#### gr-filter

- `logpwrfft` YAML was hiding FFT length
- FFT frequency range calculation was occasionally off due to abuse of
  `numpy.arange`

#### gr-filter

- GRC band pass filter taps block: use `firdes.complex_band_pass` when complex
- `firdes.root_raised_cosine` gain for unity alpha

#### gr-qtgui

- `qtgui_chooser` regained 3.7 capability to set default
- Frequency Sink hovering now respects set units
- GRC blocks had optional stream inputs

#### gr-uhd

- ignore `len_tag_name` in USRP Source YAML generator python
- `uhd_fft`/`uhd_siggen_gui`: import order of sip and Qt
- GRC example block errors

### Added

#### GRC

- OS X: Meta modifier key support (eq. Ctrl on other platforms)
- configuration setting `show_block_comments`

#### gr-analog

- Signal source `cmd` port

#### gr-zeromq

- Message-based ZMQ blocks take bind argument, as necessary for N:1 patterns.
  Changes ABI.

### Deprecated

#### gr-analog

- Signal source `freq` port

## [3.8.1.0] - 2020-04-08

### Changed

#### Project Scope

- clang-tidy improvements
  - Throw exceptions by value, catch by reference
  - `emplace_back` where applicable
  - `empty()` instead of `vector::size() == 0`
- Use CMake to check for endianness instead of `BOOST_BIG_ENDIAN`

#### gr-fec

- Scipy becomes optional dependency (for polar channel code construction)

#### gr_modtool

- use Boost.UTF instead of cppunit

### Fixed

#### Project Scope

- FindQwt paths
- floatAlmostEqual unittest assert function wrongly passing on sequence types
- Only require boost unittest when testing is enabled
- FindLOG4CPP typo
- numpy.fft(pack) imports
- several scipy imports that can be done with numpy alone

#### gnuradio-runtime

- block gateway shadowed system port
- Flaky message passing unit test contained timeout (not the test's job)
- ctrlport/`rpcaggregator` & Co: removed storage of references to scope-lifetime objects
- Sine table generation python was wrong
- `get_tags_in_range` for delay < (end-start)
- premature tag pruning
- release flattened flowgraph after stopping; fixes restartability/shutdown issues

#### gr-analog

- clipping in FM receiver: remove superfluous gain
- C++ generation for multiple blocks

#### gr-audio

- portaudio source: lock acquisition

#### gr-blocks

- rotator VOLK workaround

#### gr-digital

- `map_bb` buffer overflow
- `map_bb` thread safety
- `additive_scrambler `count based reset

#### gr-fec

- heap corruption in `async_decoder`
- `cc_encoder` was broken for constraint lengths > 8

#### gr-fft

- restore Boost 1.53 compat

#### gr-qtgui

- no longer requiring unnecessary key in `edit_box_msg`

#### gr-uhd

- fixed examples under Py3
- multichannel objects not populating channels

#### gr_modtool

- wrong use of `input` -> `raw_input`
- allow empty argument list
- testing
- check for and deny TSB under Python
- QA addition bugs
- correct path for C++ QA tests

#### GRC

- several issues with YAML files
- nested objects now properly populate namespaces
- comments now included in block bounds calculation
- Wiki documentation link removed from OOT blocks' docs tab
- Dragging connections to auto-hide ports works now
- generated and re-generated several example flowgraphs
- `bokeh_layout` module name
- Revert toggle buttons to text entry for bool block props, allowing GRC
  variables to be used

### Added

#### Project Scope

- Codec2 development branch / future compat
- Boost 1.71 compat
- CI now checks for formatting

#### gnuradio-runtime

- dot graphs now contain message edges
- Python wrapping for blocks' `set_affinity` and `{g,s}et_alias`

#### gr-uhd

- UHD Filter API

#### GRC

- block affinity, buffer sizes available as advanced options for blocks
- testing
- Python snippets (please snippet responsibly!)
- `show_id` flag added to embedded python blocks, Probes
- global option to toggle showing of IDs
- Help "Keyboard Shortcut" entry

### Deprecated

#### gr-analog

- `sig_source`: `freq` port will be removed in the future

### Added
#### gr-analog

- `sig_source`: `cmd` port adds support for dicts, setting of frequency,
  amplitude, offset and phase parameters

## [3.8.0.0] - 2019-08-09

Witness me!

Tonight, we release GNU Radio 3.8.0.0.

It's the first minor release version since more than six years, not without
pride this community stands to face the brightest future SDR on general purpose
hardware ever had.

Since we've not been documenting changes in the shape of a Changelog for the
whole of the development that happened since GNU Radio 3.7.0, I'm afraid that
these release notes will be more of a GLTL;DR (git log too long; didn't read)
than a detailed account of what has changed.

What has _not** changed is the fact that GNU Radio is centered around a very
simple truth:

> Let the developers hack on DSP. Software interfaces are for humans, not the
> other way around.

And so, compared to the later 3.7 releases, nothing has fundamentally modified
the way one develops signal processing systems with GNU Radio: You write blocks,
and you combine blocks to be part of a larger signal processing flow graph.

With that as a success story, we of course have faced quite a bit of change in
the systems we use to develop and in the people that develop GNU Radio. This has
lead to several changes that weren't compatible with 3.7.

**This is an excerpt only:**

### Fixed

#### Project Scope

- Roughly 36 dBBug, (ETOOMANYBUGS)
- Fixed .so versioning

### Changed

#### Project Scope

- C++11
- merged the wholeness of the `next` branch
- Dependency version bumps: CMake, GCC, MSVC, Swig, Boost
- New dependencies: MPIR/GMP, Qt5, gsm, codec2
- Removed dependencies: libusb, Qt4, CppUnit
- Python: Python 2 & Python 3 compatible. 3.8 will be the last Py2k-compatible
  release series
- gengen was replaced by templates (if you don't know gengen, don't do any
  research; save yourself that sorrow)
- Modern CMake (as far as feasible at this point)
- VOLK version updated to v2.0.0
- .clang-format file now dictates coding style
- clang-format'ed the whole tree.
- installed CMake files now tell about configuration

#### gnuradio-runtime

- reworked fractional tag time handling, especially in the context of resamplers

#### GRC

- C++ generation as option
- YAML instead of XML
- removed `blks2`
- much better canvas tooling
- consistent gobject usage
- ROUNDED ARROWS

#### gr-qtgui

- moving from Qt4 to Qt5

#### gr-utils

- `gr_modtool` now vastly improved

#### gr-vocoder

- improved versatility
- removed in-tree libgsm, libcodec2, use system-wide libs


### Removed

#### Project Scope
- Modules `gr-comedi`, `gr-fcd` and `gr-wxgui` are gone

#### gr-comedi

- nobody could remember who used this, or for what. It has seen 0 active code
  contributions in the 3.7 lifecycle

#### gr-digital

- python-based `packet_encoder` and related tools: Bugs that were sporadic and
  never fixed, so after long deprecation, we're removing it

#### gr-fcd

- since it's currently untestable by the CI, it's being removed, as there was no
  code contributions. Generally, we strive to include all batteries with GNU
  Radio. Re-integration within a more general SDR interface would be desirable.

#### gr-utils

- removed PyQwt (dead) based tools

#### gr-wxgui

- Unmaintained, breaks on increasingly many systems, always was slower than
  Qtgui. We've been starting to tell people to migrate to Qt since at
  least 2015. Now, we're finally removing it.
