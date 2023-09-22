/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef WATERFALL_DISPLAY_FORM_H
#define WATERFALL_DISPLAY_FORM_H

#include <gnuradio/filter/firdes.h>
#include <gnuradio/qtgui/WaterfallDisplayPlot.h>
#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <QtGui/QtGui>
#include <vector>

#include <gnuradio/qtgui/displayform.h>

/*!
 * \brief DisplayForm child for managing waterfall (spectrogram) plots.
 * \ingroup qtgui_blk
 */
class WaterfallDisplayForm : public DisplayForm
{
    Q_OBJECT

public:
    WaterfallDisplayForm(int nplots = 1, QWidget* parent = 0);
    ~WaterfallDisplayForm() override;

    WaterfallDisplayPlot* getPlot() override;

    int getFFTSize() const;
    double getTimePerFFT();
    float getFFTAverage() const;
    gr::fft::window::win_type getFFTWindowType() const;

    int getColorMap(unsigned int which);
    int getAlpha(unsigned int which);
    double getMinIntensity(unsigned int which);
    double getMaxIntensity(unsigned int which);

    void clearData();

    // returns the frequency that was last double-clicked on by the user
    float getClickedFreq() const;

    // checks if there was a double-click event; reset if there was
    bool checkClicked();

public slots:
    void customEvent(QEvent* e) override;
    void setTimeTitle(const std::string);
    void setSampleRate(const QString& samprate) override;
    void setFFTSize(const int);
    void setFFTAverage(const float);
    void setFFTWindowType(const gr::fft::window::win_type);

    void setFrequencyRange(const double centerfreq, const double bandwidth);

    void setIntensityRange(const double minIntensity, const double maxIntensity);
    void setMaxIntensity(const QString& m);
    void setMinIntensity(const QString& m);

    void setAlpha(unsigned int which, unsigned int alpha);

    void setColorMap(unsigned int which,
                     const int newType,
                     const QColor lowColor = QColor("white"),
                     const QColor highColor = QColor("white"));

    void autoScale(bool en = false) override;
    void setPlotPosHalf(bool half);
    void setTimePerFFT(double t);
    void setUpdateTime(double t);

private slots:
    void newData(const QEvent* updateEvent) override;
    void onPlotPointSelected(const QPointF p) override;

private:
    QIntValidator* d_int_validator;

    double d_samp_rate, d_center_freq;
    int d_fftsize;
    double d_time_per_fft;
    float d_fftavg;
    gr::fft::window::win_type d_fftwintype;
    double d_units;

    bool d_clicked;
    double d_clicked_freq;

    double d_min_val, d_cur_min_val;
    double d_max_val, d_cur_max_val;

    FFTAverageMenu* d_avgmenu;
    FFTWindowMenu* d_winmenu;
};

#endif /* WATERFALL_DISPLAY_FORM_H */
