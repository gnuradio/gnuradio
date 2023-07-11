/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QMediaPlayer>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QAbstractSlider;
class QComboBox;
QT_END_NAMESPACE

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerControls(QWidget* parent = nullptr);

    QMediaPlayer::State state() const;
    int volume() const;
    bool isMuted() const;
    qreal playbackRate() const;

public slots:
    void setState(QMediaPlayer::State state);
    void setVolume(int volume);
    void setMuted(bool muted);
    void setPlaybackRate(float rate);

signals:
    void play();
    void pause();
    void stop();
    void next5();
    void previous5();
    void changeVolume(int volume);
    void changeMuting(bool muting);
    void changeRate(qreal rate);

private slots:
    void playClicked();
    void muteClicked();
    void updateRate();
    void onVolumeSliderValueChanged();

private:
    QMediaPlayer::State m_playerState = QMediaPlayer::StoppedState;
    bool m_playerMuted = false;
    QAbstractButton* m_playButton = nullptr;
    QAbstractButton* m_stopButton = nullptr;
    QAbstractButton* m_nextButton = nullptr;
    QAbstractButton* m_previousButton = nullptr;
    QAbstractButton* m_muteButton = nullptr;
    QAbstractSlider* m_volumeSlider = nullptr;
    QComboBox* m_rateBox = nullptr;
};

#endif // PLAYERCONTROLS_H
