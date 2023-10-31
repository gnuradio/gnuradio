/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef VIDEOPLAYER_HH
#define VIDEOPLAYER_HH

#include "playercontrols.h"

#include <QtMultimediaWidgets/QVideoWidget>
#include <QtWidgets/QWidget>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
class video_player : public QWidget
{
    Q_OBJECT

public:
    video_player(QWidget* parent = nullptr, QString videoFile = "");
    ~video_player();

public slots:
    void data_read();

private:
    QMediaPlayer* d_player;
    QString d_videoFile;
    bool d_downloading = true;
    bool d_started = false;
    qint64 d_position = 0;
    qint64 d_duration = 0;
    QTimer d_timer;
};

#endif // VIDEOPLAYER_HH

#ifndef VIDEOSIGNAL_HH
#define VIDEOSIGNAL_HH

class video_player_signal : public QObject
{
    Q_OBJECT

public:
    video_player_signal() {}
    ~video_player_signal() {}

signals:
    void data_send();
};

#endif // VIDEOSIGNAL_HH
