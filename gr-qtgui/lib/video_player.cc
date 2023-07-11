/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "video_player.h"

video_player::video_player(QWidget* parent, QString videoFile)
    : QWidget(parent), d_player(new QMediaPlayer), d_videoFile(videoFile)
{
    QVideoWidget* videoWidget = new QVideoWidget(this);
    d_player->setVideoOutput(videoWidget);
    d_player->setMedia(QUrl::fromLocalFile(d_videoFile));
    QSlider* timeSlider = new QSlider(Qt::Horizontal, this);
    QLabel* durationLabel = new QLabel(this);
    QLabel* playedTimeLabel = new QLabel(this);

    PlayerControls* controls = new PlayerControls(this);
    controls->setState(d_player->state());
    controls->setVolume(d_player->volume());
    controls->setMuted(controls->isMuted());

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(controls);

    QHBoxLayout* sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(playedTimeLabel);
    sliderLayout->addWidget(timeSlider);
    sliderLayout->addWidget(durationLabel);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(videoWidget);
    layout->addItem(sliderLayout);
    layout->addItem(buttonLayout);
    setLayout(layout);

    d_timer.setInterval(2000);
    d_timer.setSingleShot(true);

    connect(controls, &PlayerControls::next5, [=]() {
        if (d_position < d_player->duration() - 5000) {
            d_position += 5000;
            d_player->setPosition(d_position);
        } else {
            d_position = d_player->duration();
            d_player->setPosition(d_position);
        }
    });

    connect(controls, &PlayerControls::previous5, [=]() {
        if (d_position >= 5000) {
            d_position -= 5000;
            d_player->setPosition(d_position);
        } else {
            d_position = 0;
            d_player->setPosition(d_position);
        }
    });

    connect(controls, &PlayerControls::play, d_player, &QMediaPlayer::play);
    connect(controls, &PlayerControls::pause, d_player, &QMediaPlayer::pause);
    connect(controls, &PlayerControls::stop, d_player, &QMediaPlayer::stop);
    connect(controls, &PlayerControls::changeVolume, d_player, &QMediaPlayer::setVolume);
    connect(controls, &PlayerControls::changeMuting, d_player, &QMediaPlayer::setMuted);
    connect(
        controls, &PlayerControls::changeRate, d_player, &QMediaPlayer::setPlaybackRate);
    connect(controls,
            &PlayerControls::stop,
            videoWidget,
            QOverload<>::of(&QVideoWidget::update));

    connect(d_player, &QMediaPlayer::stateChanged, controls, &PlayerControls::setState);
    connect(d_player, &QMediaPlayer::volumeChanged, controls, &PlayerControls::setVolume);
    connect(d_player, &QMediaPlayer::mutedChanged, controls, &PlayerControls::setMuted);

    connect(&d_timer, &QTimer::timeout, [=]() {
        if (d_downloading) {
            d_downloading = false;
        }
    });

    connect(timeSlider, &QSlider::sliderMoved, [=](int position) {
        d_position = position;
        d_player->setPosition(position);
    });

    connect(d_player, &QMediaPlayer::positionChanged, [=](qint64 position) {
        timeSlider->setValue(position);

        QString playedTime = QTime(0, 0).addMSecs(position).toString("mm:ss");
        playedTimeLabel->setText(playedTime);
        if (position >= d_player->duration()) {
            emit d_player->durationChanged(position);
        }
    });

    connect(d_player, &QMediaPlayer::durationChanged, [=](qint64 duration) {
        if (d_duration <= duration)
            d_duration = duration;
        timeSlider->setRange(0, duration);
        QString totalDuration = QTime(0, 0).addMSecs(duration).toString("mm:ss");
        durationLabel->setText(totalDuration);
    });
    connect(d_player,
            &QMediaPlayer::mediaStatusChanged,
            [this](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::LoadedMedia) {
                    d_player->setPosition(d_position);
                    d_player->play();
                }
                if (status == QMediaPlayer::EndOfMedia && d_downloading) {
                    if (d_position <= d_duration) {
                        d_position = d_duration;
                    }
                    d_player->setMedia(QUrl::fromLocalFile(d_videoFile));
                } else if (status == QMediaPlayer::EndOfMedia && !d_downloading) {
                    d_position = 0;
                    d_player->setMedia(QUrl::fromLocalFile(d_videoFile));
                    d_player->setPosition(d_position);
                    d_player->stop();
                }
            });
}

video_player::~video_player()
{
    d_player->stop();
    delete d_player;
}

void video_player::data_read()
{
    d_downloading = true;
    d_timer.start();
    if (!d_started) {
        d_player->play();
        d_started = true;
    }
}
