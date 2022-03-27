/* -*- c++ -*- */
/*
 * Copyright 2022 Volker Schroer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef DISPLAY_TEXT_WINDOW_H
#define DISPLAY_TEXT_WINDOW_H

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>


class display_text_window : public QWidget
{
    Q_OBJECT

public:
    explicit display_text_window(int splitlength, int maxlines, QWidget* parent = 0);
    ~display_text_window();
    void set_text(const char* c, int count);
    void set_header(QString header);

private:
    QVBoxLayout* d_vertical_layout;
    QLabel* d_label;
    QScrollArea* d_display_area;
    QHBoxLayout* d_horizontal_layout;
    QPushButton* d_clear;
    QSpacerItem* d_horizontal_spacer;
    QRadioButton* d_auto_focus;
    QSpacerItem* d_horizontal_spacer_2;
    QPushButton* d_save;
    QVBoxLayout* d_lines_layout;

    QString d_line;

    int d_splitlength;
    unsigned int d_linecount;
    unsigned int d_maxlines;
    bool d_line_to_be_continued;

    QWidget* d_d_display_box;
    QLabel** d_lines;
    bool d_autofocus;
    void insert_nl_into_line(int from);
    void move_input2lines();
    void scroll_up(unsigned int rows, bool d_clear);
private slots:
    void text2file();
    void clear_text();
    void autofocus_enable(bool);
};

#endif // DISPLAY_TEXT_WINDOW_H
