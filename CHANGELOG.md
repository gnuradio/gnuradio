# Changelog
All notable changes to GNU Radio will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic
Versioning](http://semver.org/spec/v2.0.0.html), starting with version 3.7.12.0.

Older Logs can be found in `docs/RELEASE-NOTES-*`.

## [3.10.0.0] - Unreleased

### Changed

- Moved PDU blocks from gr-blocks to gr-network and gr-pdu
   - Compatibility shim included to allow access to these blocks from gr-blocks
     but these are deprecated from the gr-blocks namespace and the shim is
     scheduled for removal in 3.11.
- gr::blocks::pdu namespace has been reorganized in gr
   - PDU vector types are accessible in gr::types
   - PDU functions are accessible in gr::pdu
   - Common msg port names are accessible in gr::ports

#### Project Scope

- C++17
  - requires MSVC 1914 (Microsoft VS 2017 15.7)
- Windows build: removed unnecessary MSVC-specific system include overrides

#### gr-blocks

- Remove deprecated networking blocks: `udp_source`, `udp_sink`, `tcp_server_sink`; replaced
  in 3.9 with more capable blocks in `gr-network`

#### gr-analog

- `fastnoise_source`: Use `uint64_t` seed API, use `size_t` for vector length/indices
- `fastnoise_source`: Use a simple bitmask if the random pool length is a power
  of 2 to determine indices, instead of `%`, which consumed considerable CPU
- `sig_source`: Remove deprecated `freq` message port of signal source block; Use `cmd` port instead

### gr-filter

- Remove deprecated `mmse_interpolator` block; Replaced previously by `mmse_resampler`

### gr-digital

- Remove deprecated simple_{correlator,framer}
- Remove deprecated cma, lms, kurtotic equalizers; replaced in 3.9 by `linear_equalizer`

#### gnuradio-runtime

- `gr::random` uses xoroshiro128+ internally, takes `uint64_t` seed

### Added

- New in-tree module gr-pdu

#### Misc.

- dtools: Added run-clang-tidy-on-codebase, which does what the name suggests,
  then updates all bindtool hashes, and commits everything appropriately

## [3.9.0.0] - 2020-01-17

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
