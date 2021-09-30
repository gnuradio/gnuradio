# Changelog
All notable changes to GNU Radio will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic
Versioning](http://semver.org/spec/v2.0.0.html), starting with version 3.7.12.0.

Older Logs can be found in `docs/RELEASE-NOTES-*`.

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
