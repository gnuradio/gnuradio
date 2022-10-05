/* -*- c++ -*- */
/*
 * Copyright 2013-2015 Sylvain Munaut
 * Copyright 2015 Ettus Research
 * Copyright 2020 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_QFOSPHORCOLORMAPPER_H
#define INCLUDED_QTGUI_QFOSPHORCOLORMAPPER_H

#include <QFile>
#include <QGLFunctions>
#include <QGLShaderProgram>
#include <QGradient>
#include <QObject>
#include <QPixmap>

#include <map>
#include <string>

namespace gr {
namespace qtgui {

class QFosphorColorMapper : public ::QObject, protected ::QGLFunctions
{
    Q_OBJECT

public:
    QFosphorColorMapper(QObject* parent = NULL);

    int loadFromFile(QFile& file);
    bool addPalette(std::string name, QLinearGradient& gradient);
    bool addPalette(std::string name, QPixmap& pixmap);

    void drawScale(std::string name, float x0, float y0, float x1, float y1);

    void enable(std::string name, GLuint tex_id);
    void disable();

private:
    QGLShaderProgram* d_shader;

    int d_u_cmap;
    int d_u_tex;
    int d_u_range;

    std::map<std::string, GLuint> d_palettes;
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_QTGUI_QFOSPHORCOLORMAPPER_H */
