/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MATRIX_DISPLAY_SIGNAL_H
#define MATRIX_DISPLAY_SIGNAL_H

#include <QObject>
#include <QVector>

class matrix_display_signal : public QObject
{
    Q_OBJECT

public:
    matrix_display_signal() {}
    ~matrix_display_signal() override {}

signals:
    void data_ready(QVector<double> data);
};

#endif /* MATRIX_DISPLAY_SIGNAL_H */
