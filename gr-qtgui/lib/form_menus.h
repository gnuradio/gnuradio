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
#include <filter/firdes.h>
#include "qtgui_types.h"

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


class MarkerAlphaMenu: public QMenu
{
  Q_OBJECT

public:
  MarkerAlphaMenu(int which, QWidget *parent)
    : QMenu("Line Transparency", parent), d_which(which)
  {
    d_act.push_back(new QAction("None", this));
    d_act.push_back(new QAction("Low", this));
    d_act.push_back(new QAction("Medium", this));
    d_act.push_back(new QAction("High", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getNone()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getLow()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getMedium()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getHigh()));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      addAction(a);
    }
  }

  ~MarkerAlphaMenu()
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
      throw std::runtime_error("MarkerAlphaMenu::getAction: which out of range.\n");
  }

signals:
  void whichTrigger(int which, int);

public slots:
  void getNone()   { emit whichTrigger(d_which, 255); }
  void getLow()    { emit whichTrigger(d_which, 200); }
  void getMedium() { emit whichTrigger(d_which, 125); }
  void getHigh()   { emit whichTrigger(d_which, 50); }

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

    d_text = new QLineEdit();

    QGridLayout *layout = new QGridLayout(d_diag);
    QPushButton *btn_ok = new QPushButton(tr("OK"));
    QPushButton *btn_cancel = new QPushButton(tr("Cancel"));

    layout->addWidget(d_text, 0, 0, 1, 2);
    layout->addWidget(btn_ok, 1, 0);
    layout->addWidget(btn_cancel, 1, 1);

    connect(btn_ok, SIGNAL(clicked()), this, SLOT(getText()));
    connect(btn_cancel, SIGNAL(clicked()), d_diag, SLOT(close()));

    connect(this, SIGNAL(triggered()), this, SLOT(getTextDiag()));
  }

  ~LineTitleAction()
  {}
  
signals:
  void whichTrigger(int which, const QString &text);

public slots:
  void getTextDiag()
  {
    d_diag->exec();
  }

private slots:
  void getText()
  { 
    emit whichTrigger(d_which, d_text->text());
    d_diag->accept();
  }

private:
  int d_which;

  QDialog *d_diag;
  QLineEdit *d_text;
};


/********************************************************************/


class OtherAction: public QAction
{
  Q_OBJECT

public:
  OtherAction(QWidget *parent)
    : QAction("Other", parent)
  {
    d_diag = new QDialog(parent);
    d_diag->setModal(true);

    d_text = new QLineEdit();

    QGridLayout *layout = new QGridLayout(d_diag);
    QPushButton *btn_ok = new QPushButton(tr("OK"));
    QPushButton *btn_cancel = new QPushButton(tr("Cancel"));

    layout->addWidget(d_text, 0, 0, 1, 2);
    layout->addWidget(btn_ok, 1, 0);
    layout->addWidget(btn_cancel, 1, 1);

    connect(btn_ok, SIGNAL(clicked()), this, SLOT(getText()));
    connect(btn_cancel, SIGNAL(clicked()), d_diag, SLOT(close()));

    connect(this, SIGNAL(triggered()), this, SLOT(getTextDiag()));
  }

  ~OtherAction()
  {}

signals:
  void whichTrigger(const QString &text);

public slots:
  void getTextDiag()
  {
    d_diag->exec();
  }

private slots:
  void getText()
  { 
    emit whichTrigger(d_text->text());
    d_diag->accept();
  }

private:
  QDialog *d_diag;
  QLineEdit *d_text;
};

/********************************************************************/


class OtherDualAction: public QAction
{
  Q_OBJECT

public:
  OtherDualAction(QString label0, QString label1, QWidget *parent)
    : QAction("Other", parent)
  {
    d_diag = new QDialog(parent);
    d_diag->setModal(true);

    d_text0 = new QLineEdit();
    d_text1 = new QLineEdit();
    
    QLabel *_label0 = new QLabel(label0);
    QLabel *_label1 = new QLabel(label1);

    QGridLayout *layout = new QGridLayout(d_diag);
    QPushButton *btn_ok = new QPushButton(tr("OK"));
    QPushButton *btn_cancel = new QPushButton(tr("Cancel"));

    layout->addWidget(_label0, 0, 0, 1, 2);
    layout->addWidget(_label1, 1, 0, 1, 2);

    layout->addWidget(d_text0, 0, 1, 1, 2);
    layout->addWidget(d_text1, 1, 1, 1, 2);
    layout->addWidget(btn_ok, 2, 0);
    layout->addWidget(btn_cancel, 2, 1);

    connect(btn_ok, SIGNAL(clicked()), this, SLOT(getText()));
    connect(btn_cancel, SIGNAL(clicked()), d_diag, SLOT(close()));

    connect(this, SIGNAL(triggered()), this, SLOT(getTextDiag()));
  }

  ~OtherDualAction()
  {}

signals:
  void whichTrigger(const QString &text0, const QString &text1);

public slots:
  void getTextDiag()
  {
    d_diag->exec();
  }

private slots:
  void getText()
  { 
    emit whichTrigger(d_text0->text(), d_text1->text());
    d_diag->accept();
  }

private:
  QDialog *d_diag;
  QLineEdit *d_text0;
  QLineEdit *d_text1;
};


/********************************************************************/


class FFTSizeMenu: public QMenu
{
  Q_OBJECT

public:
  FFTSizeMenu(QWidget *parent)
    : QMenu("FFT Size", parent)
  {
    d_act.push_back(new QAction("32", this));
    d_act.push_back(new QAction("64", this));
    d_act.push_back(new QAction("128", this));
    d_act.push_back(new QAction("256", this));
    d_act.push_back(new QAction("512", this));
    d_act.push_back(new QAction("1024", this));
    d_act.push_back(new QAction("2048", this));
    d_act.push_back(new QAction("4096", this));
    d_act.push_back(new QAction("8192", this));
    d_act.push_back(new QAction("16384", this));
    d_act.push_back(new QAction("32768", this));
    d_act.push_back(new OtherAction(this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(get05()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(get06()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(get07()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(get08()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(get09()));
    connect(d_act[5], SIGNAL(triggered()), this, SLOT(get10()));
    connect(d_act[6], SIGNAL(triggered()), this, SLOT(get11()));
    connect(d_act[7], SIGNAL(triggered()), this, SLOT(get12()));
    connect(d_act[8], SIGNAL(triggered()), this, SLOT(get13()));
    connect(d_act[9], SIGNAL(triggered()), this, SLOT(get14()));
    connect(d_act[10], SIGNAL(triggered()), this, SLOT(get15()));
    connect(d_act[11], SIGNAL(whichTrigger(const QString&)),
	     this, SLOT(getOther(const QString&)));

     QListIterator<QAction*> i(d_act);
     while(i.hasNext()) {
       QAction *a = i.next();
       addAction(a);
     }
   }

   ~FFTSizeMenu()
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
       throw std::runtime_error("FFTSizeMenu::getAction: which out of range.\n");
   }

 signals:
   void whichTrigger(int size);

 public slots:
   void get05() { emit whichTrigger(32); }
   void get06() { emit whichTrigger(64); }
   void get07() { emit whichTrigger(128); }
   void get08() { emit whichTrigger(256); }
   void get09() { emit whichTrigger(512); }
   void get10() { emit whichTrigger(1024); }
   void get11() { emit whichTrigger(2048); }
   void get12() { emit whichTrigger(4096); }
   void get13() { emit whichTrigger(8192); }
   void get14() { emit whichTrigger(16384); }
   void get15() { emit whichTrigger(32768); }
   void getOther(const QString &str) 
   {
     int value = str.toInt();
     emit whichTrigger(value);
   }

private:
  QList<QAction *> d_act;
  OtherAction *d_other;
};


/********************************************************************/


class FFTAverageMenu: public QMenu
{
  Q_OBJECT

public:
  FFTAverageMenu(QWidget *parent)
    : QMenu("FFT Average", parent)
  {
    d_act.push_back(new QAction("Off", this));
    d_act.push_back(new QAction("High", this));
    d_act.push_back(new QAction("Medium", this));
    d_act.push_back(new QAction("Low", this));
    d_act.push_back(new OtherAction(this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getOff()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getHigh()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getMedium()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getLow()));
    connect(d_act[4], SIGNAL(whichTrigger(const QString&)),
	     this, SLOT(getOther(const QString&)));

     QListIterator<QAction*> i(d_act);
     while(i.hasNext()) {
       QAction *a = i.next();
       addAction(a);
     }
   }

   ~FFTAverageMenu()
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
       throw std::runtime_error("FFTSizeMenu::getAction: which out of range.\n");
   }

 signals:
   void whichTrigger(float alpha);

 public slots:
   void getOff() { emit whichTrigger(1.0); }
   void getHigh() { emit whichTrigger(0.05); }
   void getMedium() { emit whichTrigger(0.1); }
   void getLow() { emit whichTrigger(0.2); }
   void getOther(const QString &str) 
   {
     float value = str.toFloat();
     emit whichTrigger(value);
   }

private:
  QList<QAction *> d_act;
  OtherAction *d_other;
};


/********************************************************************/


class FFTWindowMenu: public QMenu
{
  Q_OBJECT

public:
  FFTWindowMenu(QWidget *parent)
    : QMenu("FFT Window", parent)
  {
    d_act.push_back(new QAction("None", this));
    d_act.push_back(new QAction("Hamming", this));
    d_act.push_back(new QAction("Hann", this));
    d_act.push_back(new QAction("Blackman", this));
    d_act.push_back(new QAction("Blackman-harris", this));
    d_act.push_back(new QAction("Rectangular", this));
    d_act.push_back(new QAction("Kaiser", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getNone()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getHamming()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getHann()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getBlackman()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(getBlackmanharris()));
    connect(d_act[5], SIGNAL(triggered()), this, SLOT(getRectangular()));
    connect(d_act[6], SIGNAL(triggered()), this, SLOT(getKaiser()));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      addAction(a);
    }
  }

  ~FFTWindowMenu()
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
      throw std::runtime_error("FFTWindowMenu::getAction: which out of range.\n");
  }

signals:
  void whichTrigger(const gr::filter::firdes::win_type type);

public slots:
  void getNone() { emit whichTrigger(gr::filter::firdes::WIN_NONE); }
  void getHamming() { emit whichTrigger(gr::filter::firdes::WIN_HAMMING); }
  void getHann() { emit whichTrigger(gr::filter::firdes::WIN_HANN); }
  void getBlackman() { emit whichTrigger(gr::filter::firdes::WIN_BLACKMAN); }
  void getBlackmanharris() { emit whichTrigger(gr::filter::firdes::WIN_BLACKMAN_hARRIS); }
  void getRectangular() { emit whichTrigger(gr::filter::firdes::WIN_RECTANGULAR); }
  void getKaiser() { emit whichTrigger(gr::filter::firdes::WIN_KAISER); }

private:
  QList<QAction *> d_act;
  int d_which;
};


/********************************************************************/


class NPointsMenu: public QAction
{
  Q_OBJECT

public:
  NPointsMenu(QWidget *parent)
    : QAction("Number of Points", parent)
  {
    d_diag = new QDialog(parent);
    d_diag->setModal(true);

    d_text = new QLineEdit();

    QGridLayout *layout = new QGridLayout(d_diag);
    QPushButton *btn_ok = new QPushButton(tr("OK"));
    QPushButton *btn_cancel = new QPushButton(tr("Cancel"));

    layout->addWidget(d_text, 0, 0, 1, 2);
    layout->addWidget(btn_ok, 1, 0);
    layout->addWidget(btn_cancel, 1, 1);

    connect(btn_ok, SIGNAL(clicked()), this, SLOT(getText()));
    connect(btn_cancel, SIGNAL(clicked()), d_diag, SLOT(close()));

    connect(this, SIGNAL(triggered()), this, SLOT(getTextDiag()));
  }

  ~NPointsMenu()
  {}

signals:
  void whichTrigger(const int npts);

public slots:
  void getTextDiag()
  {
    d_diag->show();
  }

private slots:
  void getText()
  { 
    emit whichTrigger(d_text->text().toInt());
    d_diag->accept();
  }

private:
  QDialog *d_diag;
  QLineEdit *d_text;
};


/********************************************************************/


class ColorMapMenu: public QMenu
{
  Q_OBJECT

public:
  ColorMapMenu(QWidget *parent)
    : QMenu("Color Map", parent)
  {
    d_act.push_back(new QAction("Multi-Color", this));
    d_act.push_back(new QAction("White Hot", this));
    d_act.push_back(new QAction("Black Hot", this));
    d_act.push_back(new QAction("Incandescent", this));
    d_act.push_back(new QAction("Other", this));
    //d_act.push_back(new OtherDualAction("Min Intensity: ", "Max Intensity: ", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getMultiColor()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getWhiteHot()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getBlackHot()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getIncandescent()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(getOther()));

     QListIterator<QAction*> i(d_act);
     while(i.hasNext()) {
       QAction *a = i.next();
       addAction(a);
     }

     d_max_value = QColor("white");
     d_min_value = QColor("white");
   }

   ~ColorMapMenu()
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
       throw std::runtime_error("ColorMapMenu::getAction: which out of range.\n");
   }

 signals:
  void whichTrigger(const int type, const QColor &min_color=QColor(),
		    const QColor &max_color=QColor());

 public slots:
  void getMultiColor() { emit whichTrigger(INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR); }
  void getWhiteHot() { emit whichTrigger(INTENSITY_COLOR_MAP_TYPE_WHITE_HOT); }
  void getBlackHot() { emit whichTrigger(INTENSITY_COLOR_MAP_TYPE_BLACK_HOT); }
  void getIncandescent() { emit whichTrigger(INTENSITY_COLOR_MAP_TYPE_INCANDESCENT); }
  //void getOther(const QString &min_str, const QString &max_str)
  void getOther()
  {
    QMessageBox::information(this, "Set low and high intensities",
       "In the next windows, select the low and then the high intensity colors.",
       QMessageBox::Ok);
    d_min_value = QColorDialog::getColor(d_min_value, this);
    d_max_value = QColorDialog::getColor(d_max_value, this);

    emit whichTrigger(INTENSITY_COLOR_MAP_TYPE_USER_DEFINED,
		      d_min_value, d_max_value);
  }

private:
  QList<QAction *> d_act;
  OtherDualAction *d_other;
  QColor d_max_value, d_min_value;
};


/********************************************************************/


#endif /* FORM_MENUS_H */
