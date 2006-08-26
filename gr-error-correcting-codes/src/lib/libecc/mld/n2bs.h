/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef INCLUDED_N2BS_H
#define INCLUDED_N2BS_H

#include <string>

std::string n2bs (char number, unsigned char digits);
std::string n2bs (short number, unsigned char digits);
std::string n2bs (int number, unsigned char digits);
std::string n2bs (long number, unsigned char digits);
std::string n2bs (long long number, unsigned char digits);
std::string n2bs (unsigned char number, unsigned char digits);
std::string n2bs (unsigned short number, unsigned char digits);
std::string n2bs (unsigned int number, unsigned char digits);
std::string n2bs (unsigned long number, unsigned char digits);
std::string n2bs (unsigned long long number, unsigned char digits);

inline std::string n2bs (char number, char digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (short number, char digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (int number, char digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long number, char digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long long number, char digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned char number, char digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned short number, char digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned int number, char digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long number, char digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long long number, char digits)
{return (n2bs (number, (unsigned char) digits));};

inline std::string n2bs (char number, unsigned short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (short number, unsigned short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (int number, unsigned short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long number, unsigned short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long long number, unsigned short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned char number, unsigned short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned short number, unsigned short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned int number, unsigned short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long number, unsigned short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long long number, unsigned short digits)
{return (n2bs (number, (unsigned char) digits));};

inline std::string n2bs (char number, short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (short number, short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (int number, short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long number, short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long long number, short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned char number, short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned short number, short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned int number, short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long number, short digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long long number, short digits)
{return (n2bs (number, (unsigned char) digits));};

inline std::string n2bs (char number, unsigned int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (short number, unsigned int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (int number, unsigned int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long number, unsigned int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long long number, unsigned int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned char number, unsigned int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned short number, unsigned int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned int number, unsigned int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long number, unsigned int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long long number, unsigned int digits)
{return (n2bs (number, (unsigned char) digits));};

inline std::string n2bs (char number, int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (short number, int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (int number, int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long number, int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long long number, int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned char number, int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned short number, int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned int number, int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long number, int digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long long number, int digits)
{return (n2bs (number, (unsigned char) digits));};

inline std::string n2bs (char number, unsigned long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (short number, unsigned long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (int number, unsigned long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long number, unsigned long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long long number, unsigned long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned char number, unsigned long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned short number, unsigned long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned int number, unsigned long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long number, unsigned long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long long number, unsigned long digits)
{return (n2bs (number, (unsigned char) digits));};

inline std::string n2bs (char number, long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (short number, long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (int number, long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long number, long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long long number, long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned char number, long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned short number, long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned int number, long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long number, long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long long number, long digits)
{return (n2bs (number, (unsigned char) digits));};

inline std::string n2bs (char number, unsigned long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (short number, unsigned long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (int number, unsigned long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long number, unsigned long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long long number, unsigned long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned char number, unsigned long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned short number, unsigned long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned int number, unsigned long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long number, unsigned long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long long number, unsigned long long digits)
{return (n2bs (number, (unsigned char) digits));};

inline std::string n2bs (char number, long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (short number, long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (int number, long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long number, long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (long long number, long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned char number, long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned short number, long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned int number, long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long number, long long digits)
{return (n2bs (number, (unsigned char) digits));};
inline std::string n2bs (unsigned long long number, long long digits)
{return (n2bs (number, (unsigned char) digits));};

#endif /* INCLUDED_N2BS_H */
