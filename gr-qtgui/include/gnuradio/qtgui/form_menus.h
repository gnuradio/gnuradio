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
#include <QtGui/QIntValidator>
#include <QtGui/QDoubleValidator>
#include <qwt_symbol.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/qtgui/qtgui_types.h>
#include <gnuradio/qtgui/trigger_mode.h>

class LineColorMenu: public QMenu
{
  Q_OBJECT

public:
  LineColorMenu(int which, QWidget *parent)
    : QMenu("Line Color", parent), d_which(which)
  {
    d_grp = new QActionGroup(this);

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
      a->setCheckable(true);
      a->setActionGroup(d_grp);
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
  QActionGroup *d_grp;
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
    d_grp = new QActionGroup(this);

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
      a->setCheckable(true);
      a->setActionGroup(d_grp);
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
  QActionGroup *d_grp;
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
    d_grp = new QActionGroup(this);

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
      a->setCheckable(true);
      a->setActionGroup(d_grp);
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
  QActionGroup *d_grp;
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
    d_grp = new QActionGroup(this);

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
      a->setCheckable(true);
      a->setActionGroup(d_grp);
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
  QActionGroup *d_grp;
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
    d_grp = new QActionGroup(this);

    d_act.push_back(new QAction("None", this));
    d_act.push_back(new QAction("Low", this));
    d_act.push_back(new QAction("Medium", this));
    d_act.push_back(new QAction("High", this));
    d_act.push_back(new QAction("Off", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getNone()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getLow()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getMedium()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getHigh()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(getOff()));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      a->setCheckable(true);
      a->setActionGroup(d_grp);
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
  void getOff()    { emit whichTrigger(d_which, 0); }

private:
  QActionGroup *d_grp;
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
    d_diag->setWindowTitle("Other");
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

  void setValidator(QValidator *v)
  {
    d_text->setValidator(v);
  }

  void setDiagText(QString text)
  {
    d_text->setText(text);
  }

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
    d_diag->setWindowTitle("Other");
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
    d_grp = new QActionGroup(this);

    d_act.push_back(new QAction("32", this));
    d_act.push_back(new QAction("64", this));
    d_act.push_back(new QAction("128", this));
    d_act.push_back(new QAction("256", this));
    d_act.push_back(new QAction("512", this));
    d_act.push_back(new QAction("1024", this));
    d_act.push_back(new QAction("2048", this));
    d_act.push_back(new QAction("4096", this));
    //d_act.push_back(new QAction("8192", this));
    //d_act.push_back(new QAction("16384", this));
    //d_act.push_back(new QAction("32768", this));
    d_act.push_back(new OtherAction(this));

    d_grp = new QActionGroup(this);
    for(int t = 0; t < d_act.size(); t++) {
      d_act[t]->setCheckable(true);
      d_act[t]->setActionGroup(d_grp);
    }

    QIntValidator *valid = new QIntValidator(32, 4096, this);
    ((OtherAction*)d_act[d_act.size()-1])->setValidator(valid);

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(get05()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(get06()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(get07()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(get08()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(get09()));
    connect(d_act[5], SIGNAL(triggered()), this, SLOT(get10()));
    connect(d_act[6], SIGNAL(triggered()), this, SLOT(get11()));
    connect(d_act[7], SIGNAL(triggered()), this, SLOT(get12()));
    //connect(d_act[8], SIGNAL(triggered()), this, SLOT(get13()));
    //connect(d_act[9], SIGNAL(triggered()), this, SLOT(get14()));
    //connect(d_act[10], SIGNAL(triggered()), this, SLOT(get15()));
    connect(d_act[8], SIGNAL(whichTrigger(const QString&)),
            this, SLOT(getOther(const QString&)));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      a->setCheckable(true);
      a->setActionGroup(d_grp);
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

  QAction * getActionFromSize(int size)
  {
    float ipt;
    float which = std::log(static_cast<float>(size))/std::log(2.0f) - 5;
    // If we're a predefined value
    if(std::modf(which,&ipt) == 0) {
      if(which < d_act.size()-1)
        return d_act[static_cast<int>(which)];
      else
        throw std::runtime_error("FFTSizeMenu::getActionFromString: which out of range.\n");
    }
    // Or a non-predefined value, return Other
    else {
      ((OtherAction*)d_act[d_act.size()-1])->setDiagText(QString().setNum(size));
      return d_act[d_act.size()-1];
    }
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
  //void get13() { emit whichTrigger(8192); }
  //void get14() { emit whichTrigger(16384); }
  //void get15() { emit whichTrigger(32768); }
  void getOther(const QString &str)
  {
    int value = str.toInt();
    emit whichTrigger(value);
  }

private:
  QList<QAction *> d_act;
  OtherAction *d_other;
  QActionGroup *d_grp;
};

/********************************************************************/

class AverageMenu: public QMenu
{
  Q_OBJECT

public:
  AverageMenu(const std::string &menuTitle, QWidget *parent)
    : QMenu(menuTitle.c_str(), parent)
  {
    d_grp = new QActionGroup(this);

    d_off = 1.0;
    d_high = 0.05;
    d_medium = 0.1;
    d_low = 0.2;

    d_act.push_back(new QAction("Off", this));
    d_act.push_back(new QAction("High", this));
    d_act.push_back(new QAction("Medium", this));
    d_act.push_back(new QAction("Low", this));
    d_act.push_back(new OtherAction(this));

    d_grp = new QActionGroup(this);
    for(int t = 0; t < d_act.size(); t++) {
      d_act[t]->setCheckable(true);
      d_act[t]->setActionGroup(d_grp);
    }
    d_act[0]->setChecked(true);

    QDoubleValidator *valid = new QDoubleValidator(0.0, 1.0, 3, this);
    ((OtherAction*)d_act[d_act.size()-1])->setValidator(valid);

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getOff()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getHigh()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getMedium()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getLow()));
    connect(d_act[4], SIGNAL(whichTrigger(const QString&)),
            this, SLOT(getOther(const QString&)));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      a->setCheckable(true);
      a->setActionGroup(d_grp);
      addAction(a);
    }
  }

  ~AverageMenu()
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

  QAction * getActionFromAvg(float avg)
  {
    int which = 0;
    if(avg == d_off)
      which = 0;
    else if(avg == d_high)
      which = 1;
    else if(avg == d_medium)
      which = 2;
    else if(avg == d_low)
      which = 3;
    else {
      ((OtherAction*)d_act[d_act.size()-1])->setDiagText(QString().setNum(avg));
      which = 4;
    }
    return d_act[static_cast<int>(which)];
  }

  void setHigh(float x)
  {
    d_high = x;
  }

  void setMedium(float x)
  {
    d_medium = x;
  }

  void setLow(float x)
  {
    d_low = x;
  }

 signals:
   void whichTrigger(float alpha);

 public slots:
   void getOff() { emit whichTrigger(d_off); }
   void getHigh() { emit whichTrigger(d_high); }
   void getMedium() { emit whichTrigger(d_medium); }
   void getLow() { emit whichTrigger(d_low); }
   void getOther(const QString &str)
   {
     float value = str.toFloat();
     emit whichTrigger(value);
   }

private:
  QList<QAction *> d_act;
  OtherAction *d_other;
  QActionGroup *d_grp;
  float d_off, d_high, d_medium, d_low;
};

/********************************************************************/

class FFTAverageMenu : public AverageMenu
{
public:
  FFTAverageMenu(QWidget *parent) : AverageMenu("FFT Average", parent)
  {
    // nop
  }

  ~FFTAverageMenu() {}
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
    d_act.push_back(new QAction("Flat-top", this));

    d_grp = new QActionGroup(this);
    for(int t = 0; t < d_act.size(); t++) {
      d_act[t]->setCheckable(true);
      d_act[t]->setActionGroup(d_grp);
    }

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getNone()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getHamming()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getHann()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getBlackman()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(getBlackmanharris()));
    connect(d_act[5], SIGNAL(triggered()), this, SLOT(getRectangular()));
    connect(d_act[6], SIGNAL(triggered()), this, SLOT(getKaiser()));
    connect(d_act[7], SIGNAL(triggered()), this, SLOT(getFlattop()));

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

  QAction * getActionFromWindow(gr::filter::firdes::win_type type)
  {
    int which = 0;
    switch(static_cast<int>(type)) {
    case((gr::filter::firdes::WIN_NONE)): which = 0; break;
    case((gr::filter::firdes::WIN_HAMMING)): which = 1; break;
    case((gr::filter::firdes::WIN_HANN)): which = 2; break;
    case((gr::filter::firdes::WIN_BLACKMAN)): which = 3; break;
    case((gr::filter::firdes::WIN_BLACKMAN_hARRIS)): which = 4; break;
    case((gr::filter::firdes::WIN_RECTANGULAR)): which = 5; break;
    case((gr::filter::firdes::WIN_KAISER)): which = 6; break;
    case((gr::filter::firdes::WIN_FLATTOP)): which = 7; break;
    }
    return d_act[which];
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
  void getFlattop() { emit whichTrigger(gr::filter::firdes::WIN_FLATTOP); }

private:
  QList<QAction *> d_act;
  QActionGroup *d_grp;
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
    d_diag->setWindowTitle("Number of Points");
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
  void setDiagText(const int npts)
  {
    d_text->setText(QString().setNum(npts));
  }

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
  ColorMapMenu(int which, QWidget *parent)
    : QMenu("Color Map", parent), d_which(which)
  {
    d_grp = new QActionGroup(this);

    d_act.push_back(new QAction("Multi-Color", this));
    d_act.push_back(new QAction("White Hot", this));
    d_act.push_back(new QAction("Black Hot", this));
    d_act.push_back(new QAction("Incandescent", this));
    d_act.push_back(new QAction("Sunset", this));
    d_act.push_back(new QAction("Cool", this));
    d_act.push_back(new QAction("Other", this));
    //d_act.push_back(new OtherDualAction("Min Intensity: ", "Max Intensity: ", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getMultiColor()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getWhiteHot()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getBlackHot()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getIncandescent()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(getSunset()));
    connect(d_act[5], SIGNAL(triggered()), this, SLOT(getCool()));
    connect(d_act[6], SIGNAL(triggered()), this, SLOT(getOther()));

     QListIterator<QAction*> i(d_act);
     while(i.hasNext()) {
       QAction *a = i.next();
       a->setCheckable(true);
       a->setActionGroup(d_grp);
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
  void whichTrigger(int which, const int type,
		    const QColor &min_color=QColor(),
		    const QColor &max_color=QColor());

 public slots:
  void getMultiColor() { emit whichTrigger(d_which, INTENSITY_COLOR_MAP_TYPE_MULTI_COLOR); }
  void getWhiteHot() { emit whichTrigger(d_which, INTENSITY_COLOR_MAP_TYPE_WHITE_HOT); }
  void getBlackHot() { emit whichTrigger(d_which, INTENSITY_COLOR_MAP_TYPE_BLACK_HOT); }
  void getIncandescent() { emit whichTrigger(d_which, INTENSITY_COLOR_MAP_TYPE_INCANDESCENT); }
  void getSunset() { emit whichTrigger(d_which, INTENSITY_COLOR_MAP_TYPE_SUNSET); }
  void getCool() { emit whichTrigger(d_which, INTENSITY_COLOR_MAP_TYPE_COOL); }
  //void getOther(d_which, const QString &min_str, const QString &max_str)
  void getOther()
  {
    QMessageBox::information(this, "Set low and high intensities",
       "In the next windows, select the low and then the high intensity colors.",
       QMessageBox::Ok);
    d_min_value = QColorDialog::getColor(d_min_value, this);
    d_max_value = QColorDialog::getColor(d_max_value, this);

    emit whichTrigger(d_which, INTENSITY_COLOR_MAP_TYPE_USER_DEFINED,
		      d_min_value, d_max_value);
  }

private:
  QActionGroup *d_grp;
  QList<QAction *> d_act;
  OtherDualAction *d_other;
  QColor d_max_value, d_min_value;
  int d_which;
};


/********************************************************************/


class TriggerModeMenu: public QMenu
{
  Q_OBJECT

public:
  TriggerModeMenu(QWidget *parent)
    : QMenu("Mode", parent)
  {
    d_grp = new QActionGroup(this);
    d_act.push_back(new QAction("Free", this));
    d_act.push_back(new QAction("Auto", this));
    d_act.push_back(new QAction("Normal", this));
    d_act.push_back(new QAction("Tag", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getFree()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getAuto()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getNorm()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getTag()));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      a->setCheckable(true);
      a->setActionGroup(d_grp);
      addAction(a);
    }
  }

  ~TriggerModeMenu()
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
      throw std::runtime_error("TriggerModeMenu::getAction: which out of range.\n");
  }

  QAction * getAction(gr::qtgui::trigger_mode mode)
  {
    switch(mode) {
    case gr::qtgui::TRIG_MODE_FREE:
      return d_act[0];
      break;
    case gr::qtgui::TRIG_MODE_AUTO:
      return d_act[1];
      break;
    case gr::qtgui::TRIG_MODE_NORM:
      return d_act[2];
      break;
    case gr::qtgui::TRIG_MODE_TAG:
      return d_act[3];
      break;
    default:
      throw std::runtime_error("TriggerModeMenu::getAction: unknown trigger mode.\n");
    }
  }

signals:
  void whichTrigger(gr::qtgui::trigger_mode mode);

public slots:
  void getFree() { emit whichTrigger(gr::qtgui::TRIG_MODE_FREE); }
  void getAuto() { emit whichTrigger(gr::qtgui::TRIG_MODE_AUTO); }
  void getNorm() { emit whichTrigger(gr::qtgui::TRIG_MODE_NORM); }
  void getTag()  { emit whichTrigger(gr::qtgui::TRIG_MODE_TAG); }

private:
  QList<QAction *> d_act;
  QActionGroup *d_grp;
};


/********************************************************************/


class TriggerSlopeMenu: public QMenu
{
  Q_OBJECT

public:
  TriggerSlopeMenu(QWidget *parent)
    : QMenu("Slope", parent)
  {
    d_grp = new QActionGroup(this);
    d_act.push_back(new QAction("Positive", this));
    d_act.push_back(new QAction("Negative", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getPos()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getNeg()));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      a->setCheckable(true);
      a->setActionGroup(d_grp);
      addAction(a);
    }
  }

  ~TriggerSlopeMenu()
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
      throw std::runtime_error("TriggerSlopeMenu::getAction: which out of range.\n");
  }

  QAction * getAction(gr::qtgui::trigger_slope slope)
  {
    switch(slope) {
    case gr::qtgui::TRIG_SLOPE_POS:
      return d_act[0];
      break;
    case gr::qtgui::TRIG_SLOPE_NEG:
      return d_act[1];
      break;
    default:
      throw std::runtime_error("TriggerSlopeMenu::getAction: unknown trigger slope.\n");
    }
  }

signals:
  void whichTrigger(gr::qtgui::trigger_slope slope);

public slots:
  void getPos() { emit whichTrigger(gr::qtgui::TRIG_SLOPE_POS); }
  void getNeg() { emit whichTrigger(gr::qtgui::TRIG_SLOPE_NEG); }

private:
  QList<QAction *> d_act;
  QActionGroup *d_grp;
};


/********************************************************************/


class TriggerChannelMenu: public QMenu
{
  Q_OBJECT

public:
  TriggerChannelMenu(int nchans, QWidget *parent)
    : QMenu("Channel", parent)
  {
    d_grp = new QActionGroup(this);
    for(int i = 0; i < nchans; i++) {
      d_act.push_back(new QAction(QString().setNum(i), this));
      d_act[i]->setCheckable(true);
      d_act[i]->setActionGroup(d_grp);

      addAction(d_act[i]);
      connect(d_act[i], SIGNAL(triggered()), this, SLOT(getChannel()));
    }
  }

  ~TriggerChannelMenu()
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
      throw std::runtime_error("TriggerChannelMenu::getAction: which out of range.\n");
  }


signals:
  void whichTrigger(int n);

public slots:
  void getChannel()
  {
    QAction *a = d_grp->checkedAction();
    int which = a->text().toInt();
    emit whichTrigger(which);
  }

private:
  QList<QAction *> d_act;
  QActionGroup *d_grp;
};


/********************************************************************/


class NumberLayoutMenu: public QMenu
{
  Q_OBJECT

public:
  NumberLayoutMenu(QWidget *parent)
    : QMenu("Layout", parent)
  {
    d_grp = new QActionGroup(this);
    d_act.push_back(new QAction("Horizontal", this));
    d_act.push_back(new QAction("Vertical", this));
    d_act.push_back(new QAction("None", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getHoriz()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getVert()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getNone()));

    QListIterator<QAction*> i(d_act);
    while(i.hasNext()) {
      QAction *a = i.next();
      a->setCheckable(true);
      a->setActionGroup(d_grp);
      addAction(a);
    }
  }

  ~NumberLayoutMenu()
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
      throw std::runtime_error("NumberLayoutMenu::getAction: which out of range.\n");
  }

  QAction * getAction(gr::qtgui::graph_t layout)
  {
    switch(layout) {
    case gr::qtgui::NUM_GRAPH_HORIZ:
      return d_act[0];
      break;
    case gr::qtgui::NUM_GRAPH_VERT:
      return d_act[1];
      break;
    case gr::qtgui::NUM_GRAPH_NONE:
      return d_act[1];
      break;
    default:
      throw std::runtime_error("NumberLayoutMenu::getAction: unknown layout type.\n");
    }
  }

signals:
  void whichTrigger(gr::qtgui::graph_t layout);

public slots:
  void getHoriz() { emit whichTrigger(gr::qtgui::NUM_GRAPH_HORIZ); }
  void getVert() { emit whichTrigger(gr::qtgui::NUM_GRAPH_VERT); }
  void getNone() { emit whichTrigger(gr::qtgui::NUM_GRAPH_NONE); }

private:
  QList<QAction *> d_act;
  QActionGroup *d_grp;
};


/********************************************************************/


class NumberColorMapMenu: public QMenu
{
  Q_OBJECT

public:
  NumberColorMapMenu(int which, QWidget *parent)
    : QMenu("Color Map", parent), d_which(which)
  {
    d_grp = new QActionGroup(this);

    d_act.push_back(new QAction("Black", this));
    d_act.push_back(new QAction("Blue-Red", this));
    d_act.push_back(new QAction("White Hot", this));
    d_act.push_back(new QAction("Black Hot", this));
    d_act.push_back(new QAction("Black-Red", this));
    d_act.push_back(new QAction("Other", this));

    connect(d_act[0], SIGNAL(triggered()), this, SLOT(getBlack()));
    connect(d_act[1], SIGNAL(triggered()), this, SLOT(getBlueRed()));
    connect(d_act[2], SIGNAL(triggered()), this, SLOT(getWhiteHot()));
    connect(d_act[3], SIGNAL(triggered()), this, SLOT(getBlackHot()));
    connect(d_act[4], SIGNAL(triggered()), this, SLOT(getBlackRed()));
    connect(d_act[5], SIGNAL(triggered()), this, SLOT(getOther()));

     QListIterator<QAction*> i(d_act);
     while(i.hasNext()) {
       QAction *a = i.next();
       a->setCheckable(true);
       a->setActionGroup(d_grp);
       addAction(a);
     }

     d_max_value = QColor("black");
     d_min_value = QColor("black");
   }

   ~NumberColorMapMenu()
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
  void whichTrigger(int which,
		    const QColor &min_color,
		    const QColor &max_color);

 public slots:
  void getBlack() { emit whichTrigger(d_which, QColor("black"), QColor("black")); }
  void getBlueRed() { emit whichTrigger(d_which, QColor("blue"), QColor("red")); }
  void getWhiteHot() { emit whichTrigger(d_which, QColor("black"), QColor("white")); }
  void getBlackHot() { emit whichTrigger(d_which, QColor("white"), QColor("black")); }
  void getBlackRed() { emit whichTrigger(d_which, QColor("black"), QColor("red")); }
  void getOther()
  {
    QMessageBox::information(this, "Set low and high intensities",
       "In the next windows, select the low and then the high intensity colors.",
       QMessageBox::Ok);
    d_min_value = QColorDialog::getColor(d_min_value, this);
    d_max_value = QColorDialog::getColor(d_max_value, this);

    emit whichTrigger(d_which, d_min_value, d_max_value);
  }

private:
  QActionGroup *d_grp;
  QList<QAction *> d_act;
  QColor d_max_value, d_min_value;
  int d_which;
};


/********************************************************************/


class PopupMenu: public QAction
{
  Q_OBJECT

public:
  PopupMenu(QString desc, QWidget *parent)
    : QAction(desc, parent)
  {
    d_diag = new QDialog(parent);
    d_diag->setWindowTitle(desc);
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

  ~PopupMenu()
  {}

  void setText(QString s)
  {
    d_text->setText(s);
  }

signals:
  void whichTrigger(const QString data);

public slots:
  void getTextDiag()
  {
    d_diag->show();
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


class ItemFloatAct: public QAction
{
  Q_OBJECT

public:
  ItemFloatAct(int which, QString title, QWidget *parent)
    : QAction(title, parent), d_which(which)
  {
    d_diag = new QDialog(parent);
    d_diag->setWindowTitle(title);
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

  ~ItemFloatAct()
  {}

  void setText(float f)
  {
    d_text->setText(QString("%1").arg(f));
  }


signals:
  void whichTrigger(int which, float data);

public slots:
  void getTextDiag()
  {
    d_diag->show();
  }

private slots:
  void getText()
  {
    emit whichTrigger(d_which, d_text->text().toFloat());
    d_diag->accept();
  }

private:
  int d_which;
  QDialog *d_diag;
  QLineEdit *d_text;
};



/********************************************************************/


#endif /* FORM_MENUS_H */
