/* -*- c++ -*- */
/*
 * Copyright 2022 Volker Schroer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "display_text_window.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QTextStream>

display_text_window::display_text_window(int splitlength, int maxlines, QWidget* parent)
    : QWidget(parent)
{


    /** Build gui */
    // Header text
    d_vertical_layout = new QVBoxLayout(this);
    d_label = new QLabel(this);
    d_label->setText("");

    d_vertical_layout->addWidget(d_label);

    // Scroll Area
    d_display_area = new QScrollArea(this);

    d_display_area->setLineWidth(3);
    d_display_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d_display_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    d_display_area->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    d_display_area->setWidgetResizable(true);
    d_display_area->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);

    d_vertical_layout->addWidget(d_display_area);

    d_d_display_box = new QWidget();
    d_display_area->setWidget(d_d_display_box);
    d_lines_layout = new QVBoxLayout(d_d_display_box);
    d_lines_layout->setSpacing(0);
    d_lines_layout->setContentsMargins(0, 0, 0, 0);

    d_horizontal_layout = new QHBoxLayout();
    d_clear = new QPushButton(QString("Clear"), this);

    d_horizontal_layout->addWidget(d_clear);

    d_horizontal_spacer =
        new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    d_horizontal_layout->addItem(d_horizontal_spacer);

    d_auto_focus = new QRadioButton(QString("Auto Focus"), this);

    d_auto_focus->setChecked(true);

    d_horizontal_layout->addWidget(d_auto_focus);

    d_horizontal_spacer_2 =
        new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    d_horizontal_layout->addItem(d_horizontal_spacer_2);

    d_save = new QPushButton(QString("Save text to file"), this);


    d_horizontal_layout->addWidget(d_save);


    d_vertical_layout->addLayout(d_horizontal_layout);

    connect(d_clear, SIGNAL(clicked()), this, SLOT(clear_text()));
    connect(d_save, SIGNAL(clicked()), this, SLOT(text2file()));
    connect(d_auto_focus, SIGNAL(toggled(bool)), this, SLOT(autofocus_enable(bool)));

    d_splitlength = splitlength;
    d_maxlines = maxlines;
    d_linecount = 0;
    d_line_to_be_continued = false;

    d_lines = new QLabel*[maxlines];
    d_autofocus = true;
    d_auto_focus->setChecked(d_autofocus);

    // Setup lines
    for (unsigned int i = 0; i < d_maxlines; i++) {
        d_lines[i] = new QLabel(d_d_display_box);
        d_lines_layout->addWidget(d_lines[i]);
    }
}

display_text_window::~display_text_window() {}

void display_text_window::set_text(const char* c, int count)
{
    if (count == 0) {
        return;
    }

    d_line = QString::fromLatin1(c, count);
    if (d_line_to_be_continued)
        d_line.prepend(d_lines[d_linecount]->text());
    if (d_splitlength <= 0)
        move_input2lines();
    else {
        int nl_pos = d_line.indexOf(QChar('\n'), 0);
        if (nl_pos < 0) { // No newline found
            insert_nl_into_line(0);
        } else {
            int prev_pos = 0;
            while (nl_pos > 0) {
                if ((nl_pos - prev_pos) > d_splitlength) {
                    // add additional nls
                    prev_pos = prev_pos + d_splitlength;
                    d_line.insert(prev_pos, QChar('\n'));
                    // line_length++;
                    prev_pos++;
                } else {
                    prev_pos = nl_pos + 1;
                }
                nl_pos = d_line.indexOf(QChar('\n'), prev_pos);
            }

            if (prev_pos < d_line.length())
                insert_nl_into_line(prev_pos + 1);
        }
        move_input2lines();
    }

    if (d_autofocus && (d_linecount > 2)) {
        d_display_area->ensureWidgetVisible(d_lines[d_linecount], 50, 5);
    }

    update();
}
void display_text_window::text2file()
{
    QString file_name;
    QString dir = QDir::homePath();

    file_name = QFileDialog::getSaveFileName(0, tr("Save Text"), dir, "*.txt");
    if (!file_name.isEmpty()) {
        QFile file(file_name);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            for (unsigned int i; i < d_linecount; i++)
                stream << d_lines[i]->text() << '\n';
            QMessageBox::information(
                0, "gr-display", QString("Text saved to file: ") + file_name);
        }
    }
}
void display_text_window::set_header(QString header)
{
    if (header.isEmpty())
        d_label->hide();
    else {
        d_label->setText(header);
        d_label->show();
    }
}
void display_text_window::clear_text()
{
    d_linecount = 0;
    for (unsigned int i = 0; i < d_maxlines; i++)
        d_lines[i]->clear();
}
void display_text_window::autofocus_enable(bool b) { d_autofocus = b; }

void display_text_window::insert_nl_into_line(int from)
{
    int length = d_line.length();
    if ((length - from) >= d_splitlength) // Must insert nl
    {
        int pos = from + d_splitlength;
        d_line.insert(pos, QChar('\n'));
        pos++;
        length++;
        while ((length - pos) >= d_splitlength) {
            pos += d_splitlength;
            d_line.insert(pos, QChar('\n'));
            length++;
            pos++;
        }
        //        d_remaining_chars = length - pos;
    } // else
      //   d_remaining_chars += length - from;
}
void display_text_window::move_input2lines()
{
    QStringList lines = d_line.split('\n');
    if (d_line.endsWith(QChar('\n'))) {
        lines.removeLast();
        d_line_to_be_continued = false;
    } else
        d_line_to_be_continued = true;
    // Check, if there are enough free lines
    if ((lines.size() + d_linecount) >=
        d_maxlines) { // Actual lines + new lines overrun display buffer
        if ((unsigned int)lines.size() >=
            d_maxlines) { // There are even more lines than the display buffer can take
            // So remove those lines that would disappear at once.
            unsigned int remove = lines.size() - d_maxlines;
            for (unsigned int i = 0; i <= remove; i++)
                lines.removeFirst();
            d_linecount = 0;

            // FIXME: add a statusbar and insert a warning,
            //        that the number of lines to be displayed
            //        should be increased
        } else {
            scroll_up(lines.size() + d_linecount - d_maxlines, false);
        }
    }
    for (int i = 0; i < lines.size(); i++) {
        d_lines[d_linecount]->setText(lines.at(i));
        d_linecount++;
        if (d_linecount >= d_maxlines) {
            scroll_up(d_maxlines / 2, true);
        }
    }

    if (d_line_to_be_continued) {
        d_linecount--;
    }
}
void display_text_window::scroll_up(unsigned int rows, bool clear)
{
    if (rows <= 0)
        return;

    for (unsigned int i = rows; i < d_maxlines; i++)
        d_lines[i - rows]->setText(d_lines[i]->text());
    d_linecount -= rows;
    if (!clear) {
        return;
    }
    for (unsigned int i = d_maxlines - rows; i < d_maxlines; i++)
        d_lines[i]->clear();
}
