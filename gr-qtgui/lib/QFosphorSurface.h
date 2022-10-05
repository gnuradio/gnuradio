/* -*- c++ -*- */
/*
 * Copyright 2015 Ettus Research
 * Copyright 2020 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_QFOSPHORSURFACE_H
#define INCLUDED_QTGUI_QFOSPHORSURFACE_H

#include <QGLFunctions>
#include <QGLWidget>

#include <string>

extern "C" {
#include "axis.h"
}

namespace gr {
namespace qtgui {

class QFosphorColorMapper;

class QFosphorSurface : public ::QGLWidget, protected ::QGLFunctions
{
    Q_OBJECT

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

public:
    QFosphorSurface(int fft_bins, int pwr_bins, int wf_lines, QWidget* parent);
    ~QFosphorSurface() override;

    void setFrequencyRange(const double center_freq, const double span);
    void setWaterfall(bool enabled);
    void setGrid(bool enabled);
    void setPalette(std::string name);
    void sendFrame(void* frame, int frame_len);
    void sendWaterfall(const uint8_t* wf, int n);

private:
    void drawHistogram();
    void drawHistogramIntensityScale();
    void drawSpectrum();
    void drawGrid();
    void drawWaterfall();
    void drawWaterfallIntensityScale();
    void drawMargins();
    void uploadFrameData();
    void uploadWaterfallData();
    void refreshPowerAxis();
    void refreshFrequencyAxis();
    void refreshLayout();

    int d_fft_bins;
    int d_pwr_bins;
    int d_wf_lines;

    bool d_grid_enabled;
    std::string d_palette;

    struct {
        bool dirty;
        void* data;
        GLuint tex;
        GLuint vbo;
        float* vbo_buf;
    } d_frame;

    struct {
        bool dirty;
        int pos;
        uint8_t* data_buf;
        GLuint tex;
    } d_wf;

    struct {
        bool dirty;
        int width;
        int height;

        bool wf_enabled;

        float x[4];
        float y[6];
        float x_div;
        float y_div;

        GLuint pwr_tex;
        GLuint freq_tex;
    } d_layout;

    QFosphorColorMapper* d_cmap;

    struct freq_axis d_freq_axis;
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_QTGUI_QFOSPHORSURFACE_H */
