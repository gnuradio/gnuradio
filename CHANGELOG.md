# Changelog
All notable changes to GNU Radio will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html), starting with version 3.7.12.0.

Older Logs can be found in `docs/RELEASE-NOTES-*`.

## unreleased

### Fixed
#### Project Scope
- `boost::posix_time::[milli]second` takes integer numbers, and we now at least cast the floats to integers, as Boost 1.67 forced us to realize we're (incorrectly) using floating point her
- CMake: using regex to match compiler against "Clang" now enables correct build on OS X
#### GRC
- Fixed GRC bug which broke WX and Qt (by altering the template code), in multiple (less than awesome) steps
#### gr-analog
- `fmdet_cf`'s derivative coefficients were wrong.
#### gr-blocks
- `float_array_to_int`: int32 limits were wrongly hardcoded. 
- Fixed the undefined behaviour happening in `float_to_complex` when accessing the second input_items element in the single-input case

### Added
#### gr-analog
- QA for complex cosine
#### gr-utils
- `gr_modtool` now with autocompletion candidates for `rm`

### Changed
#### Project Scope
- Removed cppunit from all unit tests, replaced by Boost
- Removed the cppunit dependency

## [3.7.12.0] - 2018-04-01

### Software Engineering
- Changed Changelog format
- Introducing Semantic Versioning
- Change of development model: Rather than merging bugfixes to maint, merging maint to master, and next, if applicable, we'll be moving forward more in a typical management model: Development happens on master, and bugfixes are cherrypicked onto the maint-${RELEASE} branch. More info: http://lists.gnu.org/archive/html/discuss-gnuradio/2018-02/msg00133.html
- First attempt at having a .clang-format file to improve code contribution style quality. This is Work In Progress.

### Fixed
#### Project Scope
- Buildability on systems where custom CMake command would try to find "" (empty string) as executable
- Buildable with C++11 compilers (though deprecation warning-rich, due to `unique_ptr`)
- several previously improperly handled include directories now actually included in individual lib/CMakeLists
- Unit test/QA now parallelizable
- Logger works with log4cpp <= 1.1.2 both in and out of tree
- Python linkage under MinGW64
- Documentation Directory now uniformly "docs/" (instead of sometimes "doc/")
- `log` vs `logf` performance bugs
- on-time `pmt::mp("string")` calls (instead of cached PMTs) performance bug
- Doxygen build now reproducible
#### GRC
- Dialogs' parent windows
- Embedded Python Blocks now work in hierarchical GRC flow graphs
#### gnuradio-runtime
- `random.h`: Uniform integer Generation depended on Boost version > min version
- `random.h`: time-dependency when seeding with `0` replaced with actual standard seed
- block startup now synchronized via barrier
- New tag propagation policy "custom" for own implementation
- `get_tags_in_range` used `v.resize(0)` to clear vectors, which lead to unnecessary memory reallocations
- `logger_ptr` now always a pointer, no matter whether logging is enabled; fixes heap corruption bug
- `buffer[_reader]_sptr` superfluous grab/release performance bug
- BSD threading and shared mem compatibility
- PMT: `equal()` comparison
#### gr-utils
- Sorted files in `gr_modtool` makexml processing, leading to improved reproducibility of builds
#### gr-audio
- OS X: memcpy bug
- OS X: debug mode
- Non-Linux ALSA compatibibilty
#### gr-analog
- `fmdet_cf` now has GRC file
- `fastnoise_source`: non-threadsafe `libc` `rand()` replaced by XOROSHIRO128+
#### gr-blocks
- `simple_copy` Ctrlport example
- Metadata sink/source not closing file on `close()`
- `multiply_matrix` formerly used "magic int" value where its API expected enum (and that int wasn't covered)
- TCP connection ASIO bug
- `tags_strobe`: superfluous tags addition
- `vector_sink`: multithreading hazards on `data` access
- `tuntap_pdu`: MTU-sized IP packet reception
- `socket_pdu`: Garbage Collect closed TCP connections
#### gr-channels
- IQ imbalance block formula now mathematically correct now mathematically correct
- `selective_fading_model`: proper scaling of path length
#### gr-digital
- `additive_scrambler_bb`: tag triggered reset
- `stream_to_tagged_stream`: tag offset after `unlock()`
- `blks2/packet.py`: Flushing for small packets
- `interpolating_resampler`: GCC 4.0 compile error
- `constellation`: 16QAM demapper was wrong for one constellation point
- `chunks_to_symbols`: mutex-guard LUT against modification during use
- MSVC compatibility throug `and` -> `&&` etc
- `crc32_bb`: handle unpacked bytes correctly
- `ofdm_carrier_allocator`: catching wrong allocations instead of crashing
#### gr-dtv
- All examples now are installed
- DVB-T2: Removed L1 post scrambling in spec v1.1.1 mode
#### gr-fcd
- Building on non-Linux systems
- Extended symbol bug in Reed-Solomon encoder
- DVB-T Cell identifier
#### gr-fcd
- Building on non-Linux systems
#### gr-fec
- `channel_construction.py`: return int frozen bit output
- `GSL_LDFLAGS` propagation
- `polar_decoder` used to be flaky
#### gr-fft
- locking of fftw wisdom
- `window.cc`: removed NaN due to FP math
- compilation under MinGW
- `logpwrfft.py` reference scaling integer division bug
#### gr-filter
- Simpson's rule code removed in favor of GSL call
#### gr-qtgui
- Time sink: Segfault with tag trigger
#### gr-uhd
- Antenna selection in UHD apps
- `uhd_fft` scalar gain-capability
- Device selection for multi-device operations in UHD apps
- TX streamer creation
#### gr-zeromq
- Multi-part messages reception

### Added
#### Project Scope
- Ninja (https://ninja-build.org) now a supported build system
#### gnuradio-runtime
- Python can now get `gr.WORK_CALLED_PRODUCE` and `gr.DONE`
#### gr-utils
- `gr_modtool`: Autocompletion, line editing
- `gr_modtool`: `copyrightholder` option
#### GRC
- "Create Duplicate"
- "Save a copy"
- BokehGUI
#### gr-blocks
- New `tag_share` block: take tags from one two input streams to one output stream
- `tag_gate`: tag filtering
- `file_source`: configurable `file_begin` stream tag
- `moving_average`: Vector mode (with element-wise MA)
- `plateau_detector`: threshold get/set
#### gr-digital
- `costas_loop_cc`: Optional loop variable outputs
- New `correlate_access_code_tag` block
- `glfsr` exposed through Python
#### gr-dtv
- 256QAM mode for ITU-T J.83B transmitter
#### gr-fec
- negative Polynomials in `cc_encoder`
#### gr-filter
- `symbol_sync_{cc|ff}`
#### gr-uhd
- `uhd_app.py`: selectable time/clock sources
- make `recv()` call's timeout parameterizable
- message-based RX tuning through sink, TX through source

### Changed
#### Project Scope
- VOLK version requirement: 1.4.0
- Minimal required SWIG version now 2.0.4
- GSL optional dependency
- Threw out completely unused `gr_??int` types
- Doxygen build no longer contains timestamps
- Doxygen build now reproducible
- Though factually before, now officially include ZeroMQ and Sphinx as optional dependencies in docs
#### gnuradio-runtime
- Python gateway allows variable number of ports
#### gr-utils
- `gr_modtool`: Block template now uses `numpy.float32` instead of float. 32bit float is actually the default GNU Radio type.e.
#### gr-blocks
- `vector_sink` can preallocate memory for desired amount of items
#### gr-digital
- `header_format_default` now sps>1-capable
- `correlate_acces_code` uses `GR_LOG`
#### gr-uhd
- `uhd_fft`: respect `lo_offset` in `set_bandwidth`
- `usrp_source`: freq tag now actually reflects last set frequency, even on multiple channels
#### gr-qtgui
- Labels: now with 98.121212388% improved floating point formatting
- Time Sink: Stem plot option exposed

### Deprecated
#### gnuradio-runtime
- `gr::sys_pri`
