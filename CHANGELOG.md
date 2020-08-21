# Changelog
All notable changes to GNU Radio will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic
Versioning](http://semver.org/spec/v2.0.0.html), starting with version 3.7.12.0.

Older Logs can be found in `docs/RELEASE-NOTES-*`.

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
