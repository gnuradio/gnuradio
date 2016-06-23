# Copyright 2010-2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Install the header files
########################################################################
install(FILES
  api.h
  ber_sink_b.h
  CMakeLists.txt
  constellationdisplayform.h
  ConstellationDisplayPlot.h
  const_sink_c.h
  displayform.h
  DisplayPlot.h
  form_menus.h
  freqdisplayform.h
  freqcontrolpanel.h
  freq_sink_c.h
  freq_sink_f.h
  FrequencyDisplayPlot.h
  histogram_sink_f.h
  histogramdisplayform.h
  HistogramDisplayPlot.h
  edit_box_msg.h
  number_sink.h
  numberdisplayform.h
  plot_raster.h
  plot_waterfall.h
  qtgui_types.h
  sink_c.h
  sink_f.h
  spectrumUpdateEvents.h
  timedisplayform.h
  timecontrolpanel.h
  TimeDomainDisplayPlot.h
  timerasterdisplayform.h
  TimeRasterDisplayPlot.h
  timeRasterGlobalData.h
  time_raster_sink_b.h
  time_raster_sink_f.h
  time_sink_c.h
  time_sink_f.h
  trigger_mode.h
  utils.h
  waterfalldisplayform.h
  WaterfallDisplayPlot.h
  waterfallGlobalData.h
  waterfall_sink_c.h
  waterfall_sink_f.h
  vectordisplayform.h
  VectorDisplayPlot.h
  vector_sink_f.h
  DESTINATION ${GR_INCLUDE_DIR}/gnuradio/qtgui
  COMPONENT "qtgui_devel"
)
