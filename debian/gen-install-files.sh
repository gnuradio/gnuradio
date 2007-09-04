#!/bin/sh

EXTRACT=dtools/bin/extract_install_filenames

# libgnuradio-core0-dev
NAME=debian/libgnuradio-core0-dev.install
rm -f $NAME
touch $NAME
$EXTRACT gnuradio-core/src/lib/filter/Makefile grinclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/filter/Makefile swiginclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/general/Makefile grinclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/general/Makefile swiginclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/gengen/Makefile grinclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/gengen/Makefile swiginclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/io/Makefile grinclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/io/Makefile swiginclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/reed-solomon/Makefile grinclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/reed-solomon/Makefile swiginclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/runtime/Makefile grinclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/runtime/Makefile swiginclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile grinclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile swiginclude_HEADERS >>$NAME

# libmblock0-dev
NAME=debian/libmblock0-dev.install
rm -f $NAME
touch $NAME
$EXTRACT mblock/src/lib/Makefile include_HEADERS >>$NAME
$EXTRACT mblock/src/lib/Makefile swiginclude_HEADERS >>$NAME

# python-gnuradio-core
NAME=debian/python-gnuradio-core.install
rm -f $NAME
touch $NAME
$EXTRACT gnuradio-core/src/python/gnuradio/Makefile grpython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile grgrlib_LTLIBRARIES >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile grgrlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile grgrpython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/gr/Makefile grgrpython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/blks/Makefile grblkspython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/blks2/Makefile grblks2python_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/blksimpl/Makefile grblkspython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/blksimpl2/Makefile grblkspython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/gru/Makefile grblkspython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/gruimpl/Makefile grupython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/vocoder/Makefile grvocoderpython_PYTHON >>$NAME

# libusrp0-dev
NAME=debian/libusrp0-dev.install
rm -f $NAME
touch $NAME
$EXTRACT usrp/host/lib/legacy/Makefile include_HEADERS >>$NAME
$EXTRACT usrp/firmware/include/Makefile include_HEADERS >>$NAME

# libusrp-inband0-dev
NAME=debian/libusrp-inband0-dev.install
rm -f $NAME
touch $NAME
$EXTRACT usrp/host/lib/inband/Makefile include_HEADERS >>$NAME

# python-usrp
NAME=debian/python-usrp.install
rm -f $NAME
touch $NAME
$EXTRACT usrp/host/swig/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT usrp/host/swig/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT usrp/host/lib/legacy/Makefile usrppython_PYTHON >>$NAME
$EXTRACT usrp/host/swig/Makefile ourpython_PYTHON >>$NAME

# python-gnuradio-usrp
NAME=debian/python-gnuradio-usrp.install
rm -f $NAME
touch $NAME
$EXTRACT gr-usrp/src/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-usrp/src/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-usrp/src/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-wxgui
NAME=debian/python-gnuradio-wxgui.install
rm -f $NAME
touch $NAME
echo etc/gnuradio/conf.d/gr-wxgui.conf >>$NAME
$EXTRACT gr-wxgui/src/python/Makefile ourpython_PYTHON >>$NAME

# python-gnuradio-atsc
NAME=debian/python-gnuradio-atsc.install
rm -f $NAME
touch $NAME
$EXTRACT gr-atsc/src/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-atsc/src/lib/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-atsc/src/lib/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-atsc/src/lib/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT gr-atsc/src/python/Makefile ourdata_DATA >>$NAME

# python-gnuradio-audio-alsa
NAME=debian/python-gnuradio-audio-alsa.install
rm -f $NAME
touch $NAME
echo etc/gnuradio/conf.d/gr-audio-alsa.conf >>$NAME
$EXTRACT gr-audio-alsa/src/Makefile ourpython_PYTHON >>$NAME
# Note: these will migrate to libgnuradio-audio-alsa in 3.2
# They are only separate in this package for C++ only experimentation
$EXTRACT gr-audio-alsa/src/Makefile lib_LTLIBRARIES >>$NAME
$EXTRACT gr-audio-alsa/src/Makefile lib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
#
$EXTRACT gr-audio-alsa/src/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-audio-alsa/src/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-audio-jack
NAME=debian/python-gnuradio-audio-jack.install
rm -f $NAME
touch $NAME
echo etc/gnuradio/conf.d/gr-audio-jack.conf >>$NAME
$EXTRACT gr-audio-jack/src/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-audio-jack/src/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-audio-jack/src/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-audio-oss
NAME=debian/python-gnuradio-audio-oss.install
rm -f $NAME
touch $NAME
echo etc/gnuradio/conf.d/gr-audio-oss.conf >>$NAME
$EXTRACT gr-audio-oss/src/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-audio-oss/src/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-audio-oss/src/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-audio-portaudio
NAME=debian/python-gnuradio-audio-portaudio.install
rm -f $NAME
touch $NAME
echo etc/gnuradio/conf.d/gr-audio-portaudio.conf >>$NAME
$EXTRACT gr-audio-portaudio/src/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-audio-portaudio/src/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-audio-portaudio/src/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-comedi
NAME=debian/python-gnuradio-comedi.install
rm -f $NAME
touch $NAME
$EXTRACT gr-comedi/src/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-comedi/src/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-comedi/src/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-cvsd-vocoder
NAME=debian/python-gnuradio-cvsd-vocoder.install
rm -f $NAME
touch $NAME
$EXTRACT gr-cvsd-vocoder/src/python/Makefile grblkspython_PYTHON >>$NAME
$EXTRACT gr-cvsd-vocoder/src/lib/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-cvsd-vocoder/src/lib/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-cvsd-vocoder/src/lib/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-gsm-fr-vocoder
NAME=debian/python-gnuradio-gsm-fr-vocoder.install
rm -f $NAME
touch $NAME
$EXTRACT gr-gsm-fr-vocoder/src/lib/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-gsm-fr-vocoder/src/lib/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-gsm-fr-vocoder/src/lib/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-trellis
NAME=debian/python-gnuradio-trellis.install
rm -f $NAME
touch $NAME
$EXTRACT gr-trellis/src/lib/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-trellis/src/lib/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-trellis/src/lib/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT gr-trellis/src/examples/Makefile ourdata_DATA >>$NAME
$EXTRACT gr-trellis/src/examples/fsm_files/Makefile ourdata_DATA >>$NAME

# python-gnuradio-video-sdl
NAME=debian/python-gnuradio-video-sdl.install
rm -f $NAME
touch $NAME
$EXTRACT gr-video-sdl/src/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-video-sdl/src/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-video-sdl/src/Makefile ourlib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-video-sdl
NAME=debian/python-gnuradio-utils.install
rm -f $NAME
touch $NAME
$EXTRACT gr-utils/src/python/Makefile bin_SCRIPTS >>$NAME

# gnuradio-examples
NAME=debian/gnuradio-examples.install
rm -f $NAME
touch $NAME
$EXTRACT gnuradio-examples/python/apps/hf_explorer/Makefile ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/apps/hf_radio/Makefile ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/audio/Makefile ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/digital/Makefile ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/digital_voice/Makefile ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/multi-antenna/Makefile ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/multi_usrp/Makefile ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/ofdm/Makefile ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/usrp/Makefile ourdata_DATA >>$NAME

# gnuradio-pager
NAME=debian/gnuradio-pager.install
rm -f $NAME
touch $NAME
$EXTRACT gr-pager/src/Makefile bin_SCRIPTS >>$NAME
$EXTRACT gr-pager/src/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-pager/src/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-pager/src/Makefile ourlib_LTLIBRARIES |
    sed -e 's/\.la$/.so/' >>$NAME

# gnuradio-radio-astronomy
NAME=debian/gnuradio-radio-astronomy.install
rm -f $NAME
touch $NAME
$EXTRACT gr-radio-astronomy/src/python/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-radio-astronomy/src/python/Makefile wxguipython_PYTHON >>$NAME
$EXTRACT gr-radio-astronomy/src/python/Makefile bin_SCRIPTS >>$NAME
$EXTRACT gr-radio-astronomy/src/lib/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-radio-astronomy/src/lib/Makefile ourlib_LTLIBRARIES >>$NAME
$EXTRACT gr-radio-astronomy/src/lib/Makefile ourlib_LTLIBRARIES |
    sed -e 's/\.la$/.so/' >>$NAME

# gnuradio-radar-mono
NAME=debian/gnuradio-radar-mono.install
rm -f $NAME
touch $NAME
$EXTRACT gr-radar-mono/src/python/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-radar-mono/src/python/Makefile bin_SCRIPTS >>$NAME

# gnuradio-sounder
NAME=debian/gnuradio-sounder.install
rm -f $NAME
touch $NAME
$EXTRACT gr-sounder/src/python/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-sounder/src/python/Makefile bin_SCRIPTS >>$NAME
