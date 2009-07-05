#!/bin/sh

EXTRACT=dtools/bin/extract_install_filenames

# libusrp-dev
NAME=debian/libusrp-dev.install
rm -f $NAME
touch $NAME
$EXTRACT usrp/host/lib/legacy/Makefile include_HEADERS >>$NAME
$EXTRACT usrp/firmware/include/Makefile include_HEADERS >>$NAME
echo usr/lib/libusrp.so >>$NAME
echo usr/lib/pkgconfig/usrp.pc >>$NAME

# libusrp2-dev
NAME=debian/libusrp2-dev.install
rm -f $NAME
touch $NAME
$EXTRACT usrp2/host/include/usrp2/Makefile usrp2include_HEADERS >>$NAME
echo usr/lib/libusrp2.so >>$NAME
echo usr/lib/pkgconfig/usrp2.pc >>$NAME

# libgnuradio-core-dev
NAME=debian/libgnuradio-core-dev.install
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
$EXTRACT gnuradio-core/src/lib/hier/Makefile grinclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/hier/Makefile swiginclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile grinclude_HEADERS >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile swiginclude_HEADERS >>$NAME
echo usr/lib/libgnuradio-core.so >>$NAME
echo usr/lib/pkgconfig/gnuradio-core.pc >>$NAME

# libgnuradio-audio-alsa
NAME=debian/libgnuradio-audio-alsa.install
rm -f $NAME
touch $NAME
$EXTRACT gr-audio-alsa/src/Makefile lib_LTLIBRARIES >>$NAME
$EXTRACT gr-audio-alsa/src/Makefile lib_LTLIBRARIES | \
    sed -e 's/\.la$/.so.*/' >>$NAME
echo etc/gnuradio/conf.d/gr-audio-alsa.conf >>$NAME

# libgnuradio-audio-alsa-dev
NAME=debian/libgnuradio-audio-alsa-dev.install
rm -f $NAME
touch $NAME
$EXTRACT gr-audio-alsa/src/Makefile grinclude_HEADERS >>$NAME
echo usr/lib/libgnuradio-audio-alsa.so >>$NAME
echo usr/lib/pkgconfig/gnuradio-audio-alsa.pc >>$NAME

# libgnuradio-qtgui
NAME=debian/libgnuradio-qtgui.install
rm -f $NAME
touch $NAME
$EXTRACT gr-qtgui/src/lib/Makefile lib_LTLIBRARIES >>$NAME
$EXTRACT gr-qtgui/src/lib/Makefile lib_LTLIBRARIES | \
    sed -e 's/\.la$/.so.*/' >>$NAME

# libgnuradio-qtgui-dev
NAME=debian/libgnuradio-qtgui-dev.install
rm -f $NAME
touch $NAME
$EXTRACT gr-qtgui/src/lib/Makefile grinclude_HEADERS >>$NAME
echo usr/lib/libgnuradio-qtgui.so >>$NAME

# libgnuradio-usrp
NAME=debian/libgnuradio-usrp.install
rm -f $NAME
touch $NAME
$EXTRACT gr-usrp/src/Makefile lib_LTLIBRARIES >>$NAME
$EXTRACT gr-usrp/src/Makefile lib_LTLIBRARIES | \
    sed -e 's/\.la$/.so.*/' >>$NAME
echo usr/lib/pkgconfig/gnuradio-usrp.pc >> $NAME

# libgnuradio-usrp-dev
NAME=debian/libgnuradio-usrp-dev.install
rm -f $NAME
touch $NAME
$EXTRACT gr-usrp/src/Makefile grinclude_HEADERS >>$NAME
echo usr/lib/libgnuradio-usrp.so >>$NAME

# libgnuradio-usrp2
NAME=debian/libgnuradio-usrp2.install
rm -f $NAME
touch $NAME
$EXTRACT gr-usrp2/src/Makefile lib_LTLIBRARIES >>$NAME
$EXTRACT gr-usrp2/src/Makefile lib_LTLIBRARIES | \
    sed -e 's/\.la$/.so.*/' >>$NAME
echo usr/lib/pkgconfig/gnuradio-usrp2.pc >> $NAME

# libgnuradio-usrp2-dev
NAME=debian/libgnuradio-usrp2-dev.install
rm -f $NAME
touch $NAME
$EXTRACT gr-usrp2/src/Makefile grinclude_HEADERS >>$NAME
echo usr/lib/libgnuradio-usrp2.so >>$NAME

# python-usrp
NAME=debian/python-usrp.install
rm -f $NAME
touch $NAME
$EXTRACT usrp/host/swig/Makefile usrp_prims_pylib_LTLIBRARIES >>$NAME
$EXTRACT usrp/host/swig/Makefile usrp_prims_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT usrp/host/swig/Makefile usrp_prims_python_PYTHON >>$NAME
$EXTRACT usrp/host/lib/legacy/Makefile usrppython_PYTHON >>$NAME

# python-gnuradio-core
NAME=debian/python-gnuradio-core.install
rm -f $NAME
touch $NAME
$EXTRACT gnuradio-core/src/python/gnuradio/Makefile grpython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/gr/Makefile grgrpython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/blks2/Makefile grblks2python_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/blks2impl/Makefile grblkspython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/gru/Makefile grblkspython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/gruimpl/Makefile grupython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/python/gnuradio/vocoder/Makefile grvocoderpython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_filter_pylib_LTLIBRARIES >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_general_pylib_LTLIBRARIES >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_gengen_pylib_LTLIBRARIES >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_hier_pylib_LTLIBRARIES >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_io_pylib_LTLIBRARIES >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_runtime_pylib_LTLIBRARIES >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_filter_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_general_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_gengen_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_hier_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_io_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_runtime_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_filter_python_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_general_python_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_gengen_python_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_hier_python_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_io_python_PYTHON >>$NAME
$EXTRACT gnuradio-core/src/lib/swig/Makefile gnuradio_swig_py_runtime_python_PYTHON >>$NAME

# python-gnuradio-wxgui
NAME=debian/python-gnuradio-wxgui.install
rm -f $NAME
touch $NAME
echo etc/gnuradio/conf.d/gr-wxgui.conf >>$NAME
$EXTRACT gr-wxgui/src/python/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-wxgui/src/python/Makefile formspython_PYTHON >>$NAME
$EXTRACT gr-wxgui/src/python/plotter/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-wxgui/Makefile pkgconfig_DATA >>$NAME

# python-gnuradio-usrp
NAME=debian/python-gnuradio-usrp.install
rm -f $NAME
touch $NAME
$EXTRACT gr-usrp/src/Makefile usrp_swig_python_PYTHON >>$NAME
$EXTRACT gr-usrp/src/Makefile usrp_swig_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-usrp/src/Makefile usrp_swig_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-usrp2
NAME=debian/python-gnuradio-usrp2.install
rm -f $NAME
touch $NAME
$EXTRACT gr-usrp2/src/Makefile usrp2_python_PYTHON >>$NAME
$EXTRACT gr-usrp2/src/Makefile usrp2_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-usrp2/src/Makefile usrp2_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-audio-alsa
NAME=debian/python-gnuradio-audio-alsa.install
rm -f $NAME
touch $NAME
$EXTRACT gr-audio-alsa/src/Makefile audio_alsa_python_PYTHON >>$NAME
$EXTRACT gr-audio-alsa/src/Makefile audio_alsa_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-audio-alsa/src/Makefile audio_alsa_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so*/' >>$NAME

# python-gnuradio-audio-jack
NAME=debian/python-gnuradio-audio-jack.install
rm -f $NAME
touch $NAME
echo etc/gnuradio/conf.d/gr-audio-jack.conf >>$NAME
$EXTRACT gr-audio-jack/src/Makefile audio_jack_python_PYTHON >>$NAME
$EXTRACT gr-audio-jack/src/Makefile audio_jack_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-audio-jack/src/Makefile audio_jack_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-audio-oss
NAME=debian/python-gnuradio-audio-oss.install
rm -f $NAME
touch $NAME
echo etc/gnuradio/conf.d/gr-audio-oss.conf >>$NAME
$EXTRACT gr-audio-oss/src/Makefile audio_oss_python_PYTHON >>$NAME
$EXTRACT gr-audio-oss/src/Makefile audio_oss_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-audio-oss/src/Makefile audio_oss_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-audio-portaudio
NAME=debian/python-gnuradio-audio-portaudio.install
rm -f $NAME
touch $NAME
echo etc/gnuradio/conf.d/gr-audio-portaudio.conf >>$NAME
$EXTRACT gr-audio-portaudio/src/Makefile audio_portaudio_python_PYTHON >>$NAME
$EXTRACT gr-audio-portaudio/src/Makefile audio_portaudio_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-audio-portaudio/src/Makefile audio_portaudio_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-cvsd-vocoder
NAME=debian/python-gnuradio-cvsd-vocoder.install
rm -f $NAME
touch $NAME
$EXTRACT gr-cvsd-vocoder/src/python/Makefile grblkspython_PYTHON >>$NAME
$EXTRACT gr-cvsd-vocoder/src/lib/Makefile cvsd_vocoder_python_PYTHON >>$NAME
$EXTRACT gr-cvsd-vocoder/src/lib/Makefile cvsd_vocoder_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-cvsd-vocoder/src/lib/Makefile cvsd_vocoder_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-gsm-fr-vocoder
NAME=debian/python-gnuradio-gsm-fr-vocoder.install
rm -f $NAME
touch $NAME
$EXTRACT gr-gsm-fr-vocoder/src/lib/Makefile gsm_full_rate_python_PYTHON >>$NAME
$EXTRACT gr-gsm-fr-vocoder/src/lib/Makefile gsm_full_rate_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-gsm-fr-vocoder/src/lib/Makefile gsm_full_rate_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# python-gnuradio-qtgui
NAME=debian/python-gnuradio-qtgui.install
rm -f $NAME
touch $NAME
$EXTRACT gr-qtgui/src/python/Makefile qtguipython_PYTHON >>$NAME
$EXTRACT gr-qtgui/src/lib/Makefile qtgui_python_PYTHON >>$NAME
$EXTRACT gr-qtgui/src/lib/Makefile qtgui_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-qtgui/src/lib/Makefile qtgui_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so*/' >>$NAME

# python-gnuradio-trellis
NAME=debian/python-gnuradio-trellis.install
rm -f $NAME
touch $NAME
$EXTRACT gr-trellis/src/lib/Makefile trellis_python_PYTHON >>$NAME
$EXTRACT gr-trellis/src/lib/Makefile trellis_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-trellis/src/lib/Makefile trellis_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME
$EXTRACT gr-trellis/src/examples/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT gr-trellis/src/examples/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gr-trellis/src/examples/fsm_files/Makefile dist_ourdata_DATA >>$NAME

# python-gnuradio-video-sdl
NAME=debian/python-gnuradio-video-sdl.install
rm -f $NAME
touch $NAME
$EXTRACT gr-video-sdl/src/Makefile video_sdl_python_PYTHON >>$NAME
$EXTRACT gr-video-sdl/src/Makefile video_sdl_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-video-sdl/src/Makefile video_sdl_pylib_LTLIBRARIES | \
    sed -e 's/\.la$/.so/' >>$NAME

# gnuradio-utils
NAME=debian/gnuradio-utils.install
rm -f $NAME
touch $NAME
$EXTRACT gr-utils/src/python/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-utils/src/python/Makefile bin_SCRIPTS >>$NAME

# gnuradio-examples
NAME=debian/gnuradio-examples.install
rm -f $NAME
touch $NAME
$EXTRACT gnuradio-examples/python/apps/hf_explorer/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/apps/hf_explorer/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/apps/hf_radio/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/apps/hf_radio/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/audio/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/digital/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/digital/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/digital-bert/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/digital-bert/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/digital_voice/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/mp-sched/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/mp-sched/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/multi-antenna/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/multi_usrp/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/multi_usrp/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/network/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/ofdm/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT gnuradio-examples/python/ofdm/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/usrp/Makefile dist_ourdata_SCRIPTS >>$NAME
$EXTRACT gnuradio-examples/python/usrp2/Makefile dist_ourdata_SCRIPTS >>$NAME

# gnuradio-pager
NAME=debian/gnuradio-pager.install
rm -f $NAME
touch $NAME
$EXTRACT gr-pager/src/Makefile dist_bin_SCRIPTS >>$NAME
$EXTRACT gr-pager/src/Makefile pager_swig_python_PYTHON >>$NAME
$EXTRACT gr-pager/src/Makefile pager_swig_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-pager/src/Makefile pager_swig_pylib_LTLIBRARIES |
    sed -e 's/\.la$/.so/' >>$NAME

# gnuradio-sounder
NAME=debian/gnuradio-sounder.install
rm -f $NAME
touch $NAME
$EXTRACT gr-sounder/src/python/Makefile sounder_python_PYTHON >>$NAME
$EXTRACT gr-sounder/src/python/Makefile dist_bin_SCRIPTS >>$NAME
echo usr/share/usrp/rev2/usrp_sounder.rbf >>$NAME
echo usr/share/usrp/rev4/usrp_sounder.rbf >>$NAME

# gnuradio-gpio
NAME=debian/gnuradio-gpio.install
rm -f $NAME
touch $NAME
$EXTRACT gr-gpio/src/python/Makefile dist_bin_SCRIPTS >>$NAME
$EXTRACT gr-gpio/src/python/Makefile ourpython_PYTHON >>$NAME
echo usr/share/usrp/rev2/std_2rxhb_2tx_dig.rbf >> $NAME
echo usr/share/usrp/rev4/std_2rxint_2tx_dig.rbf >> $NAME

# gnuradio-radar-mono
NAME=debian/gnuradio-radar-mono.install
rm -f $NAME
touch $NAME
$EXTRACT gr-radar-mono/src/python/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-radar-mono/src/python/Makefile dist_bin_SCRIPTS >>$NAME
echo usr/share/usrp/rev2/usrp_radar_mono.rbf >>$NAME
echo usr/share/usrp/rev4/usrp_radar_mono.rbf >>$NAME

# gnuradio-radio-astronomy
NAME=debian/gnuradio-radio-astronomy.install
rm -f $NAME
touch $NAME
$EXTRACT gr-radio-astronomy/src/python/Makefile ourpython_PYTHON >>$NAME
$EXTRACT gr-radio-astronomy/src/python/Makefile wxguipython_PYTHON >>$NAME
$EXTRACT gr-radio-astronomy/src/python/Makefile dist_bin_SCRIPTS >>$NAME
$EXTRACT gr-radio-astronomy/src/lib/Makefile ra_python_PYTHON >>$NAME
$EXTRACT gr-radio-astronomy/src/lib/Makefile ra_pylib_LTLIBRARIES >>$NAME
$EXTRACT gr-radio-astronomy/src/lib/Makefile ra_pylib_LTLIBRARIES |
    sed -e 's/\.la$/.so/' >>$NAME

# gnuradio-companion
NAME=debian/gnuradio-companion.install
rm -f $NAME
touch $NAME
$EXTRACT grc/Makefile ourpython_PYTHON >>$NAME
$EXTRACT grc/Makefile dist_etc_DATA >>$NAME
$EXTRACT grc/base/Makefile ourpython_PYTHON >>$NAME
$EXTRACT grc/base/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT grc/blocks/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT grc/examples/Makefile dist_audiodata_DATA >>$NAME
$EXTRACT grc/examples/Makefile dist_simpledata_DATA >>$NAME
$EXTRACT grc/examples/Makefile dist_trellisdata_DATA >>$NAME
$EXTRACT grc/examples/Makefile dist_usrpdata_DATA >>$NAME
$EXTRACT grc/examples/Makefile dist_xmlrpcdata_DATA >>$NAME
$EXTRACT grc/freedesktop/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT grc/freedesktop/Makefile dist_bin_SCRIPTS >>$NAME
$EXTRACT grc/grc_gnuradio/Makefile rootpython_PYTHON >>$NAME
$EXTRACT grc/grc_gnuradio/Makefile blks2python_PYTHON >>$NAME
$EXTRACT grc/grc_gnuradio/Makefile usrppython_PYTHON >>$NAME
$EXTRACT grc/grc_gnuradio/Makefile wxguipython_PYTHON >>$NAME
$EXTRACT grc/gui/Makefile ourpython_PYTHON >>$NAME
$EXTRACT grc/python/Makefile ourpython_PYTHON >>$NAME
$EXTRACT grc/python/Makefile dist_ourdata_DATA >>$NAME
$EXTRACT grc/scripts/Makefile dist_bin_SCRIPTS >>$NAME
