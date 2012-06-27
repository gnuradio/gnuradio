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
#include <qwt_symbol.h>

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


class LineStyleMenu: public QMenu
{
  Q_OBJECT

public:
  LineStyleMenu(int which, QWidget *parent)
    : QMenu("Line Style", parent), d_which(which)
  {
    d_act.push_back(new QAction("None", this));
    d_act.push_back(new QAction("Solid", this));
    d_act.push_back(new QAction("Dash", this));
    d_act.push_back(new QAction("Dots", this));
    d_act.push_back(new QAction("Dash-Dot", this));
    d_act.push_back(new QAction("Dash-Dot-Dot", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getNone()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getSolid()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getDash()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getDots()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(getDashDot()));
    connect(d_act[5], SIGNAL(triggered()), this, SLOT(getDashDotDot()));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      addAction(a);
    }
  }

  ~LineStyleMenu()
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
      throw std::runtime_error("LineStyleMenu::getAction: which out of range.\n");
  }

signals:
  void whichTrigger(int which, Qt::PenStyle);

public slots:
  void getNone()      { emit whichTrigger(d_which, Qt::NoPen); }
  void getSolid()      { emit whichTrigger(d_which, Qt::SolidLine); }
  void getDash()       { emit whichTrigger(d_which, Qt::DashLine); }
  void getDots()       { emit whichTrigger(d_which, Qt::DotLine); }
  void getDashDot()    { emit whichTrigger(d_which, Qt::DashDotLine); }
  void getDashDotDot() { emit whichTrigger(d_which, Qt::DashDotDotLine); }

private:
  QList<QAction *> d_act;
  int d_which;
};


/********************************************************************/


class LineMarkerMenu: public QMenu
{
  Q_OBJECT

public:
  LineMarkerMenu(int which, QWidget *parent)
    : QMenu("Line Marker", parent), d_which(which)
  {
    d_act.push_back(new QAction("None", this));
    d_act.push_back(new QAction("Circle", this));
    d_act.push_back(new QAction("Rectangle", this));
    d_act.push_back(new QAction("Diamond", this));
    d_act.push_back(new QAction("Triangle", this));
    d_act.push_back(new QAction("Down Triangle", this));
    d_act.push_back(new QAction("Left Triangle", this));
    d_act.push_back(new QAction("Right Triangle", this));
    d_act.push_back(new QAction("Cross", this));
    d_act.push_back(new QAction("X-Cross", this));
    d_act.push_back(new QAction("Horiz. Line", this));
    d_act.push_back(new QAction("Vert. Line", this));
    d_act.push_back(new QAction("Star 1", this));
    d_act.push_back(new QAction("Star 2", this));
    d_act.push_back(new QAction("Hexagon", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getNone()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getCircle()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getRect()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getDiamond()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(getTriangle()));
    connect(d_act[5], SIGNAL(triggered()), this, SLOT(getDTriangle()));
    connect(d_act[6], SIGNAL(triggered()), this, SLOT(getLTriangle()));
    connect(d_act[7], SIGNAL(triggered()), this, SLOT(getRTriangle()));
    connect(d_act[8], SIGNAL(triggered()), this, SLOT(getCross()));
    connect(d_act[9], SIGNAL(triggered()), this, SLOT(getXCross()));
    connect(d_act[10], SIGNAL(triggered()), this, SLOT(getHLine()));
    connect(d_act[11], SIGNAL(triggered()), this, SLOT(getVLine()));
    connect(d_act[12], SIGNAL(triggered()), this, SLOT(getStar1()));
    connect(d_act[13], SIGNAL(triggered()), this, SLOT(getStar2()));
    connect(d_act[14], SIGNAL(triggered()), this, SLOT(getHexagon()));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      addAction(a);
    }
  }

  ~LineMarkerMenu()
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
      throw std::runtime_error("LineMarkerMenu::getAction: which out of range.\n");
  }

signals:
  void whichTrigger(int which, QwtSymbol::Style);

public slots:
  void getNone()      { emit whichTrigger(d_which, QwtSymbol::NoSymbol); }
  void getCircle()    { emit whichTrigger(d_which, QwtSymbol::Ellipse); }
  void getRect()      { emit whichTrigger(d_which, QwtSymbol::Rect); }
  void getDiamond()   { emit whichTrigger(d_which, QwtSymbol::Diamond); }
  void getTriangle()  { emit whichTrigger(d_which, QwtSymbol::Triangle); }
  void getDTriangle() { emit whichTrigger(d_which, QwtSymbol::DTriangle); }
  void getLTriangle() { emit whichTrigger(d_which, QwtSymbol::LTriangle); }
  void getRTriangle() { emit whichTrigger(d_which, QwtSymbol::RTriangle); }
  void getCross()     { emit whichTrigger(d_which, QwtSymbol::Cross); }
  void getXCross()    { emit whichTrigger(d_which, QwtSymbol::XCross); }
  void getHLine()     { emit whichTrigger(d_which, QwtSymbol::HLine); }
  void getVLine()     { emit whichTrigger(d_which, QwtSymbol::VLine); }
  void getStar1()     { emit whichTrigger(d_which, QwtSymbol::Star1); }
  void getStar2()     { emit whichTrigger(d_which, QwtSymbol::Star2); }
  void getHexagon()   { emit whichTrigger(d_which, QwtSymbol::Hexagon); }

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
