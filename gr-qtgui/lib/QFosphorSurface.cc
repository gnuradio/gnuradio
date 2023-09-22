/* -*- c++ -*- */
/*
 * Copyright 2013-2015 Sylvain Munaut
 * Copyright 2015 Ettus Research
 * Copyright 2020 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "QFosphorColorMapper.h"
#include "QFosphorSurface.h"

#include <cstdio>
#include <cstring>

#include <QFont>
#include <QPainter>
#include <QPixmap>


namespace gr {
namespace qtgui {

QFosphorSurface::QFosphorSurface(int fft_bins,
                                 int pwr_bins,
                                 int wf_lines,
                                 QWidget* parent)
    : QGLWidget(parent),
      d_fft_bins(fft_bins),
      d_pwr_bins(pwr_bins),
      d_wf_lines(wf_lines),
      d_grid_enabled(true),
      d_palette("iron")
{
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    memset(&d_frame, 0, sizeof(d_frame));
    memset(&d_wf, 0, sizeof(d_wf));
    memset(&d_layout, 0, sizeof(d_layout));

    d_frame.vbo_buf = new float[4 * fft_bins]();
    d_wf.data_buf = new uint8_t[wf_lines * fft_bins]();

    setFrequencyRange(0.0, 0.0);
}

QFosphorSurface::~QFosphorSurface()
{
    delete[] d_frame.vbo_buf;
    delete[] d_wf.data_buf;
}


/* -------------------------------------------------------------------- */
/* Overloaded GL functions                                              */
/* -------------------------------------------------------------------- */

void QFosphorSurface::initializeGL()
{
    initializeGLFunctions();

    /* Init frame texture */
    glGenTextures(1, &d_frame.tex);

    glBindTexture(GL_TEXTURE_2D, d_frame.tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_R8,
                 d_fft_bins,
                 d_pwr_bins,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 NULL);

    /* Init frame VBO */
    glGenBuffers(1, &d_frame.vbo);

    glBindBuffer(GL_ARRAY_BUFFER, d_frame.vbo);

    glBufferData(
        GL_ARRAY_BUFFER, 2 * sizeof(float) * 2 * d_fft_bins, NULL, GL_DYNAMIC_DRAW);

    /* Init waterfall texture */
    glGenTextures(1, &d_wf.tex);

    glBindTexture(GL_TEXTURE_2D, d_wf.tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_R8,
                 d_fft_bins,
                 d_wf_lines,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 NULL);

    /* Color map */
    d_cmap = new QFosphorColorMapper(this);
}

void QFosphorSurface::resizeGL(int width, int height)
{
    /* Setup matrix to map GL coord to exact pixels */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (double)width, 0.0, (double)height, -1.0, 1.0);

    /* Viewport on the whole area */
    glViewport(0, 0, width, height);

    /* Update layout */
    d_layout.width = width;
    d_layout.height = height;
    d_layout.dirty = true;
}

void QFosphorSurface::paintGL()
{
    /* If no data, abort early */
    if (!d_frame.data)
        return;

    /* Upload texture if needed */
    if (d_frame.dirty) {
        uploadFrameData();
    }

    if (d_wf.dirty) {
        uploadWaterfallData();
    }

    /* Refresh layout if needed */
    if (d_layout.dirty) {
        refreshLayout();
    }

    /* Clear everything */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw the various UI element */
    drawHistogram();
    drawHistogramIntensityScale();
    drawSpectrum();
    drawGrid();

    if (d_layout.wf_enabled) {
        drawWaterfall();
        drawWaterfallIntensityScale();
    }

    drawMargins();
}


/* -------------------------------------------------------------------- */
/* Public API                                                           */
/* -------------------------------------------------------------------- */

void QFosphorSurface::setFrequencyRange(const double center_freq, const double span)
{
    freq_axis_build(&d_freq_axis, center_freq, span, 10);
    d_layout.dirty = true; // FIXME more fine grain refresh
}

void QFosphorSurface::setWaterfall(bool enabled)
{
    d_layout.wf_enabled = enabled;
    d_layout.dirty = true;
}

void QFosphorSurface::setGrid(bool enabled) { d_grid_enabled = enabled; }

void QFosphorSurface::setPalette(std::string name) { d_palette = name; }

void QFosphorSurface::sendFrame(void* frame, int frame_len)
{
    d_frame.data = frame;
    d_frame.dirty = true;
    QMetaObject::invokeMethod(this, "updateGL");
}

void QFosphorSurface::sendWaterfall(const uint8_t* wf, int n)
{
    int m;

    // Limit to max height
    if (n > d_wf_lines) {
        wf += d_fft_bins * (n - d_wf_lines);
        n = d_wf_lines;
    }

    // Copy the new data
    while (n) {
        m = n > (d_wf_lines - d_wf.pos) ? (d_wf_lines - d_wf.pos) : n;
        memcpy(&d_wf.data_buf[d_wf.pos * d_fft_bins], wf, m * d_fft_bins);
        d_wf.pos = (d_wf.pos + m) % d_wf_lines;
        wf += m * d_fft_bins;
        n -= m;
    }

    // Force refresh
    d_wf.dirty = true;
}


/* -------------------------------------------------------------------- */
/* Private helpers                                                      */
/* -------------------------------------------------------------------- */

void QFosphorSurface::drawHistogram()
{
    float x[2], y[2];
    float e = 0.5f / d_fft_bins;

    /* Draw Histogram texture */
    d_cmap->enable(d_palette, d_frame.tex);

    x[0] = d_layout.x[1];
    x[1] = d_layout.x[2];
    y[0] = d_layout.y[3];
    y[1] = d_layout.y[4];

    glBegin(GL_QUADS);
    glTexCoord2d(0.0f + e, -0.038f);
    glVertex2d(x[0], y[0]);
    glTexCoord2d(1.0f + e, -0.038f);
    glVertex2d(x[1], y[0]);
    glTexCoord2d(1.0f + e, 1.000f);
    glVertex2d(x[1], y[1]);
    glTexCoord2d(0.0f + e, 1.000f);
    glVertex2d(x[0], y[1]);
    glEnd();

    d_cmap->disable();
}

void QFosphorSurface::drawHistogramIntensityScale()
{
    d_cmap->drawScale(d_palette,
                      d_layout.x[2] + 2.0f,
                      d_layout.y[3],
                      d_layout.x[2] + 10.0f,
                      d_layout.y[4]);
}

void QFosphorSurface::drawSpectrum()
{
    /* Setup the 2D transform */
    glPushMatrix();

    glTranslatef(d_layout.x[1], d_layout.y[3], 0.0f);

    glScalef(d_layout.x[2] - d_layout.x[1], d_layout.y[4] - d_layout.y[3], 1.0f);

    glScalef(1.0f / d_fft_bins, 0.9632f / 256.0f, 1.0f);

    glTranslatef(0.0f,
                 9.4208f, /* (100 - 96.32) / 100 * 256 */
                 0.0f);

    /* Setup GL state */
    glBindBuffer(GL_ARRAY_BUFFER, d_frame.vbo);
    glVertexPointer(2, GL_FLOAT, 0, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0f);

    /* Draw Max-Hold */
    glColor4f(1.0f, 0.0f, 0.0f, 0.75f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_STRIP, 0, d_fft_bins);
    glDisableClientState(GL_VERTEX_ARRAY);

    /* Draw Live */
    glColor4f(1.0f, 1.0f, 1.0f, 0.75f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINE_STRIP, d_fft_bins, d_fft_bins);
    glDisableClientState(GL_VERTEX_ARRAY);

    /* Cleanup GL state */
    glDisable(GL_BLEND);

    /* Restore */
    glPopMatrix();
}

void QFosphorSurface::drawGrid()
{
    float x[2], y[2];
    int i;

    if (!d_grid_enabled)
        return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_LINES);
    glColor4f(0.02f, 0.02f, 0.02f, 0.5f);

    /* Vertical div */
    x[0] = d_layout.x[1];
    x[1] = d_layout.x[2];

    for (i = 0; i < 11; i++) {
        y[0] = d_layout.y[3] + i * d_layout.y_div + 0.5f;

        glVertex2f(x[0], y[0]);
        glVertex2f(x[1], y[0]);
    }

    /* Horizontal div */
    y[0] = d_layout.y[3];
    y[1] = d_layout.y[4];

    for (i = 0; i < 11; i++) {
        x[0] = d_layout.x[1] + i * d_layout.x_div + 0.5f;

        glVertex2f(x[0], y[0]);
        glVertex2f(x[0], y[1]);
    }

    glEnd();
    glDisable(GL_BLEND);
}

void QFosphorSurface::drawWaterfall()
{
    float x[2], y[2];
    float e = 0.5f / d_fft_bins;
    float v[2];

    /* Draw Waterfall texture */
    d_cmap->enable(d_palette, d_wf.tex);

    x[0] = d_layout.x[1];
    x[1] = d_layout.x[2];
    y[0] = d_layout.y[1];
    y[1] = d_layout.y[2];

    v[0] = (float)(d_wf.pos) / (float)(d_wf_lines);
    v[1] = v[0] + 1.0f;

    glBegin(GL_QUADS);
    glTexCoord2d(0.0f + e, v[0]);
    glVertex2d(x[0], y[0]);
    glTexCoord2d(1.0f + e, v[0]);
    glVertex2d(x[1], y[0]);
    glTexCoord2d(1.0f + e, v[1]);
    glVertex2d(x[1], y[1]);
    glTexCoord2d(0.0f + e, v[1]);
    glVertex2d(x[0], y[1]);
    glEnd();

    d_cmap->disable();
}

void QFosphorSurface::drawWaterfallIntensityScale()
{
    d_cmap->drawScale(d_palette,
                      d_layout.x[2] + 2.0f,
                      d_layout.y[1],
                      d_layout.x[2] + 10.0f,
                      d_layout.y[2]);
}

void QFosphorSurface::drawMargins()
{
    float x[2], y[2];

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    x[0] = d_layout.x[0];
    y[0] = d_layout.y[2];

    glBindTexture(GL_TEXTURE_2D, d_layout.pwr_tex);

    x[1] = d_layout.x[1];
    y[1] = d_layout.y[5];

    glBegin(GL_QUADS);
    glTexCoord2d(0.0f, 0.0f);
    glVertex2d(x[0], y[0]);
    glTexCoord2d(1.0f, 0.0f);
    glVertex2d(x[1], y[0]);
    glTexCoord2d(1.0f, 1.0f);
    glVertex2d(x[1], y[1]);
    glTexCoord2d(0.0f, 1.0f);
    glVertex2d(x[0], y[1]);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, d_layout.freq_tex);

    x[1] = d_layout.x[3];
    y[1] = d_layout.y[3];

    glBegin(GL_QUADS);
    glTexCoord2d(0.0f, 0.0f);
    glVertex2d(x[0], y[0]);
    glTexCoord2d(1.0f, 0.0f);
    glVertex2d(x[1], y[0]);
    glTexCoord2d(1.0f, 1.0f);
    glVertex2d(x[1], y[1]);
    glTexCoord2d(0.0f, 1.0f);
    glVertex2d(x[0], y[1]);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void QFosphorSurface::uploadFrameData()
{
    int i;

    /* Upload new texture */
    glBindTexture(GL_TEXTURE_2D, d_frame.tex);

    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    d_fft_bins,
                    d_pwr_bins,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    d_frame.data);

    /* Compute the new VBO data */
    for (i = 0; i < d_fft_bins; i++) {
        uint8_t* data = (uint8_t*)d_frame.data + (d_fft_bins * d_pwr_bins);
        int maxh_idx = 2 * i;
        int live_idx = 2 * (i + d_fft_bins);

        d_frame.vbo_buf[maxh_idx] = i;
        d_frame.vbo_buf[maxh_idx + 1] = data[i];

        d_frame.vbo_buf[live_idx] = i;
        d_frame.vbo_buf[live_idx + 1] = data[i + d_fft_bins];
    }

    /* Upload new VBO data */
    glBindBuffer(GL_ARRAY_BUFFER, d_frame.vbo);

    glBufferData(GL_ARRAY_BUFFER,
                 2 * sizeof(float) * 2 * d_fft_bins,
                 d_frame.vbo_buf,
                 GL_DYNAMIC_DRAW);

    /* Data is fresh */
    d_frame.dirty = false;
}

void QFosphorSurface::uploadWaterfallData()
{
    glBindTexture(GL_TEXTURE_2D, d_wf.tex);

    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    d_fft_bins,
                    d_wf_lines,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    d_wf.data_buf);
}

void QFosphorSurface::refreshPowerAxis()
{
    char buf[32];
    int i;

    /* Release previous texture */
    if (d_layout.pwr_tex)
        deleteTexture(d_layout.pwr_tex);

    /* Create a pixmap of right size */
    QPixmap pixmap((int)(d_layout.x[1] - d_layout.x[0]),
                   (int)(d_layout.y[5] - d_layout.y[2]));
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setPen(QColor(255, 255, 255, 255));

    QFont font("Monospace");
    font.setPixelSize(10);
    painter.setFont(font);

    /* Paint labels */
    for (i = 0; i < 11; i++) {
        int yv = (int)(d_layout.y[5] - d_layout.y[3] - i * d_layout.y_div);

        snprintf(buf, sizeof(buf) - 1, "%d", (i - 10) * 10);
        buf[sizeof(buf) - 1] = 0;

        painter.drawText(0,
                         yv - 10,
                         d_layout.x[1] - d_layout.x[0] - 5,
                         20,
                         Qt::AlignRight | Qt::AlignVCenter,
                         buf);
    }

    /* Create texture */
    d_layout.pwr_tex = bindTexture(pixmap);
}

void QFosphorSurface::refreshFrequencyAxis()
{
    char buf[32];
    int n_div, i;

    /* Release previous texture */
    if (d_layout.freq_tex)
        deleteTexture(d_layout.freq_tex);

    /* Create a pixmap of right size */
    QPixmap pixmap((int)(d_layout.x[3] - d_layout.x[0]),
                   (int)(d_layout.y[3] - d_layout.y[2]));
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setPen(QColor(255, 255, 255, 255));

    QFont font("Monospace");
    font.setPixelSize(10);
    painter.setFont(font);

    /* Paint labels */
    n_div = 10;

    for (i = 0; i <= n_div; i++) {
        int xv = (int)(d_layout.x[1] - d_layout.x[0] + i * d_layout.x_div);
        int xl, xw;
        int flags;

        freq_axis_render(&d_freq_axis, buf, i - (n_div >> 1));
        buf[sizeof(buf) - 1] = 0;

        if (i == 0) {
            xl = xv - 10;
            xw = 60;
            flags = Qt::AlignLeft | Qt::AlignVCenter;
        } else if (i == n_div) {
            xl = xv - 50;
            xw = 60;
            flags = Qt::AlignRight | Qt::AlignVCenter;
        } else {
            xl = xv - 30;
            xw = 60;
            flags = Qt::AlignHCenter | Qt::AlignVCenter;
        }

        painter.drawText(xl, 0, xw, (int)(d_layout.y[3] - d_layout.y[2]), flags, buf);
    }

    /* Create texture */
    d_layout.freq_tex = bindTexture(pixmap);
}

void QFosphorSurface::refreshLayout()
{
    int rsvd_pos[3], rsvd, avail, n_div, div, over, over_pos[3];

    /* Split the X space */
    rsvd_pos[0] = 40;
    rsvd_pos[1] = 20;

    rsvd = rsvd_pos[0] + rsvd_pos[1];
    avail = d_layout.width - rsvd - 1;
    div = avail / 10;
    over = avail - 10 * div;

    over_pos[0] = over / 2;
    over_pos[1] = over - over_pos[0];

    d_layout.x_div = (float)div;
    d_layout.x[0] = 0.0f;
    d_layout.x[1] = (float)(rsvd_pos[0] + over_pos[0]);
    d_layout.x[2] = d_layout.x[1] + 10.0f * div + 1.0f;
    d_layout.x[3] = d_layout.width;

    /* Split the Y space */
    rsvd_pos[0] = d_layout.wf_enabled ? 10 : 0;
    rsvd_pos[1] = 20;
    rsvd_pos[2] = 10;

    rsvd = rsvd_pos[0] + rsvd_pos[1] + rsvd_pos[2];
    avail = d_layout.height - rsvd - (d_layout.wf_enabled ? 2 : 1);
    n_div = d_layout.wf_enabled ? 20 : 10;
    div = avail / n_div;
    over = avail - n_div * div;

    if (d_layout.wf_enabled) {
        over_pos[0] = over / 3;
        over_pos[1] = over - (2 * over_pos[0]);
        over_pos[2] = over_pos[0];
    } else {
        over_pos[0] = 0;
        over_pos[1] = over / 2;
        over_pos[2] = over - over_pos[1];
    }

    d_layout.y_div = (float)div;

    d_layout.y[0] = 0.0f;

    if (d_layout.wf_enabled) {
        d_layout.y[1] = d_layout.y[0] + (float)(rsvd_pos[0] + over_pos[0]);
        d_layout.y[2] = d_layout.y[1] + 10.0f * div + 1.0f;
    } else {
        d_layout.y[1] = 0.0f;
        d_layout.y[2] = 0.0f;
    }

    d_layout.y[3] = d_layout.y[2] + (float)(rsvd_pos[1] + over_pos[1]);
    d_layout.y[4] = d_layout.y[3] + 10.0f * div + 1.0f;

    d_layout.y[5] = d_layout.height;

    /* Refresh axis */
    refreshPowerAxis();
    refreshFrequencyAxis();

    /* All refreshed now */
    d_layout.dirty = false;
}

} // namespace qtgui
} // namespace gr
