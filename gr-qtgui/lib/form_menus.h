/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef FORM_MENUS_H
#define FORM_MENUS_H

#include <stdexcept>
#include <vector>
#include <QtGui/QtGui>

class LineColorMenu: public QMenu
{
  Q_OBJECT

public:
  LineColorMenu(int which, QWidget *parent)
    : QMenu("Line Color", parent), d_which(which)
  {
    d_act.push_back(new QAction("Blue", this));
    d_act.push_back(new QAction("Red", this));
    d_act.push_back(new QAction("Green", this));
    d_act.push_back(new QAction("Black", this));
    d_act.push_back(new QAction("Cyan", this));
    d_act.push_back(new QAction("Magenta", this));
    d_act.push_back(new QAction("Yellow", this));
    d_act.push_back(new QAction("Gray", this));
    d_act.push_back(new QAction("Dark Red", this));
    d_act.push_back(new QAction("Dark Green", this));
    d_act.push_back(new QAction("Dark Blue", this));
    d_act.push_back(new QAction("Dark Gray", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getBlue()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getRed()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getGreen()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getBlack()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(getCyan()));
    connect(d_act[5], SIGNAL(triggered()), this, SLOT(getMagenta()));
    connect(d_act[6], SIGNAL(triggered()), this, SLOT(getYellow()));
    connect(d_act[7], SIGNAL(triggered()), this, SLOT(getGray()));
    connect(d_act[8], SIGNAL(triggered()), this, SLOT(getDarkRed()));
    connect(d_act[9], SIGNAL(triggered()), this, SLOT(getDarkGreen()));
    connect(d_act[10], SIGNAL(triggered()), this, SLOT(getDarkBlue()));
    connect(d_act[11], SIGNAL(triggered()), this, SLOT(getDarkGray()));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      addAction(a);
    }
  }

  ~LineColorMenu()
  {}

  int getNumActions() const
  {
    return d_act.size();
  }
  
  QAction * getAction(int which)
  {
    if(which < d_act.size())
      return d_act[which];
    else
      throw std::runtime_error("LineColorMenu::getAction: which out of range.\n");
  }

signals:
  void whichTrigger(int which, const QString &name);

public slots:
  void getBlue() { emit whichTrigger(d_which, "blue"); }
  void getRed() { emit whichTrigger(d_which, "red"); }
  void getGreen() { emit whichTrigger(d_which, "green"); }
  void getBlack() { emit whichTrigger(d_which, "black"); }
  void getCyan() { emit whichTrigger(d_which, "cyan"); }
  void getMagenta() { emit whichTrigger(d_which, "magenta"); }
  void getYellow() { emit whichTrigger(d_which, "yellow"); }
  void getGray() { emit whichTrigger(d_which, "gray"); }
  void getDarkRed() { emit whichTrigger(d_which, "darkred"); }
  void getDarkGreen() { emit whichTrigger(d_which, "darkgreen"); }
  void getDarkBlue() { emit whichTrigger(d_which, "darkblue"); }
  void getDarkGray() { emit whichTrigger(d_which, "darkgray"); }

private:
  QList<QAction *> d_act;
  int d_which;
};


/********************************************************************/


class LineWidthMenu: public QMenu
{
  Q_OBJECT

public:
  LineWidthMenu(int which, QWidget *parent)
    : QMenu("Line Width", parent), d_which(which)
  {
    d_act.push_back(new QAction("1", this));
    d_act.push_back(new QAction("2", this));
    d_act.push_back(new QAction("3", this));
    d_act.push_back(new QAction("4", this));
    d_act.push_back(new QAction("5", this));
    d_act.push_back(new QAction("6", this));
    d_act.push_back(new QAction("7", this));
    d_act.push_back(new QAction("8", this));
    d_act.push_back(new QAction("9", this));
    d_act.push_back(new QAction("10", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getOne()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getTwo()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getThree()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getFour()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(getFive()));
    connect(d_act[5], SIGNAL(triggered()), this, SLOT(getSix()));
    connect(d_act[6], SIGNAL(triggered()), this, SLOT(getSeven()));
    connect(d_act[7], SIGNAL(triggered()), this, SLOT(getEight()));
    connect(d_act[8], SIGNAL(triggered()), this, SLOT(getNine()));
    connect(d_act[9], SIGNAL(triggered()), this, SLOT(getTen()));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      addAction(a);
    }
  }

  ~LineWidthMenu()
  {}

  int getNumActions() const
  {
    return d_act.size();
  }
  
  QAction * getAction(int which)
  {
    if(which < d_act.size())
      return d_act[which];
    else
      throw std::runtime_error("LineWidthMenu::getAction: which out of range.\n");
  }

signals:
  void whichTrigger(int which, int width);

public slots:
  void getOne()   { emit whichTrigger(d_which, 1); }
  void getTwo()   { emit whichTrigger(d_which, 2); }
  void getThree() { emit whichTrigger(d_which, 3); }
  void getFour()  { emit whichTrigger(d_which, 4); }
  void getFive()  { emit whichTrigger(d_which, 5); }
  void getSix()   { emit whichTrigger(d_which, 6); }
  void getSeven() { emit whichTrigger(d_which, 7); }
  void getEight() { emit whichTrigger(d_which, 8); }
  void getNine()  { emit whichTrigger(d_which, 9); }
  void getTen()   { emit whichTrigger(d_which, 10); }

private:
  QList<QAction *> d_act;
  int d_which;
};


/********************************************************************/


class LineTitleAction: public QAction
{
  Q_OBJECT

public:
  LineTitleAction(int which, QWidget *parent)
    : QAction("Line Title", parent), d_which(which)
  {
    d_diag = new QDialog(parent);
    d_diag->setModal(true);

    d_name = new QLineEdit();

    QGridLayout *layout = new QGridLayout(d_diag);
    QPushButton *btn_ok = new QPushButton(tr("OK"));
    QPushButton *btn_cancel = new QPushButton(tr("Cancel"));

    layout->addWidget(d_name, 0, 0);
    layout->addWidget(btn_ok, 1, 0);
    layout->addWidget(btn_cancel, 1, 1);

    connect(btn_ok, SIGNAL(clicked()), this, SLOT(getName()));
    connect(btn_cancel, SIGNAL(clicked()), d_diag, SLOT(close()));

    connect(this, SIGNAL(triggered()), this, SLOT(getNameDiag()));
  }

  ~LineTitleAction()
  {}
  
signals:
  void whichTrigger(int which, const QString &name);

public slots:
  void getNameDiag()
  {
    d_diag->exec();
  }

private slots:
  void getName()
  { 
    emit whichTrigger(d_which, d_name->text());
    d_diag->accept();
  }

private:
  int d_which;

  QDialog *d_diag;
  QLineEdit *d_name;
};

#endif /* FORM_MENUS_H */
