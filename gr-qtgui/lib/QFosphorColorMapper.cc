/* -*- c++ -*- */
/*
 * Copyright 2013-2015 Sylvain Munaut
 * Copyright 2015 Ettus Research
 * Copyright 2020 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "QFosphorColorMapper.h"
#include <QGLContext>

namespace gr {
namespace qtgui {

QFosphorColorMapper::QFosphorColorMapper(QObject* parent) : QObject(parent)
{
    /* Shader init */
    d_shader = new QGLShaderProgram(this);

    d_shader->addShaderFromSourceCode(
        QGLShader::Fragment,
        "uniform sampler2D cmap;\n"
        "uniform sampler2D tex;\n"
        "uniform vec2 range;\n"
        "\n"
        "void main()\n"
        "{\n"
        " float intensity = texture2D(tex, gl_TexCoord[0].st).x;\n"
        " float map = (intensity + range.y) * range.x;\n"
        " vec4 color = texture2D(cmap, vec2(map, 0.0));\n"
        " gl_FragColor = color;\n"
        "}\n");
    d_shader->link();

    d_u_cmap = d_shader->uniformLocation("cmap");
    d_u_tex = d_shader->uniformLocation("tex");
    d_u_range = d_shader->uniformLocation("range");

    /* Load default set */
    QFile f(":/fosphor/palettes.txt");
    loadFromFile(f);
    initializeGLFunctions();
}

int QFosphorColorMapper::loadFromFile(QFile& file)
{
    QLinearGradient* gradient = NULL;
    char* name;
    int kp = -1;

    /* Make sure it's open */
    if (!file.isOpen())
        file.open(QFile::ReadOnly);

    /* Scan until the end */
    while (!file.atEnd()) {
        /* Grab a line */
        QByteArray line = file.readLine().simplified();

        /* Skip comments and empty lines */
        if (line.isEmpty() || line.startsWith('#'))
            continue;

        /* Are we in a palette ? */
        if (kp < 0) {
            sscanf(line.constData(), "%d %ms", &kp, &name);

            if (kp)
                gradient = new QLinearGradient();
        } else if (kp == 0) {
            /* Create palette */
            QString filename = line;
            QPixmap pixmap(filename);

            addPalette(name, pixmap);

            /* Release name & prepare for next */
            free(name);
            name = NULL;

            kp = -1;
        } else {
            float f[4];

            /* Read data point */
            sscanf(line.constData(), "%f %f %f %f", f, f + 1, f + 2, f + 3);
            gradient->setColorAt(f[0], QColor::fromRgbF(f[1], f[2], f[3]));

            /* Is it over ? */
            if (!--kp) {
                /* Add the newly created palette */
                addPalette(name, *gradient);

                /* Release gradient, name & prepare for next */
                delete gradient;
                gradient = NULL;

                free(name);
                name = NULL;

                kp = -1;
            }
        }
    }

    return 0;
}

bool QFosphorColorMapper::addPalette(std::string name, QLinearGradient& gradient)
{
    /* Configure the gradient */
    gradient.setStart(0, 0);
    gradient.setFinalStop(1, 0);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);

    /* Draw it on a pixmap */
    QPixmap pixmap(256, 1);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.fillRect(0, 0, 256, 1, gradient);

    /* Create it from pixmap */
    return addPalette(name, pixmap);
}

bool QFosphorColorMapper::addPalette(std::string name, QPixmap& pixmap)
{
    /* Convert to an OpenGL texture */
    /* Note: We use TEXTURE_2D because 1D isn't really supported by Qt
     * and it's also not in OpenGL ES */
    QGLContext* ctx = (QGLContext*)QGLContext::currentContext();
    GLuint tex_id = ctx->bindTexture(pixmap, GL_TEXTURE_2D);

    /* Configure behavior */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Insert new texture (remove old one if needed) */
    if (d_palettes.count(name))
        ctx->deleteTexture(d_palettes[name]);

    d_palettes[name] = tex_id;

    return true;
}

void QFosphorColorMapper::drawScale(
    std::string name, float x0, float y0, float x1, float y1)
{
    /* Enable texture-2D */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, d_palettes[name]);
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    /* Draw QUAD */
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(x0, y0);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(x1, y0);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(x1, y1);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(x0, y1);
    glEnd();

    /* Disable texturing */
    glDisable(GL_TEXTURE_2D);
}

void QFosphorColorMapper::enable(std::string name, GLuint tex_id)
{
    /* Enable shader */
    d_shader->bind();

    /* Texture unit 0: Main texture */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    d_shader->setUniformValue(d_u_tex, 0);

    /* Texture unit 1: Palette texture */
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, d_palettes[name]);
    d_shader->setUniformValue(d_u_cmap, 1);

    /* Range definition */
    d_shader->setUniformValue(d_u_range, 1.0f, 0.00f);
}

void QFosphorColorMapper::disable()
{
    glActiveTexture(GL_TEXTURE0);
    d_shader->release();
}

} // namespace qtgui
} // namespace gr
