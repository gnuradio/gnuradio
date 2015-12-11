/*
 * Copyright 2014,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

namespace cpp GNURadio
namespace py GNURadio
namespace java org.gnuradio.controlport

struct complex {
  1: double re;
  2: double im;
}

typedef list<bool>    VectorB
typedef binary        VectorC
typedef list<i16>     VectorT
typedef list<i32>     VectorI
typedef list<i64>     VectorL
typedef list<double>  VectorF
typedef list<double>  VectorD
typedef list<string>  VectorS
typedef list<complex> VectorZ

enum BaseTypes { BOOL, BYTE, SHORT, INT, LONG, DOUBLE, STRING, COMPLEX,
                 F32VECTOR, F64VECTOR, S64VECTOR, S32VECTOR, S16VECTOR,
                 S8VECTOR, C32VECTOR }

struct KnobBase {
  1: bool a_bool;
  2: byte a_byte;
  3: i16 a_short;
  4: i32 a_int;
  5: i64 a_long;
  6: double a_double;
  7: string a_string;
  8: complex a_complex;
  9: VectorF a_f32vector;
  10: VectorD a_f64vector;
  11: VectorL a_s64vector;
  12: VectorI a_s32vector;
  13: VectorT a_s16vector;
  14: VectorC a_s8vector;
  15: VectorZ a_c32vector;
}

struct Knob {
  1: BaseTypes type;
  2: KnobBase value;
}

enum KnobType { KNOBBOOL, KNOBCHAR, KNOBINT, KNOBDOUBLE, KNOBSTRING,
                KNOBLONG, KNOBVECBOOL, KNOBVECCHAR, KNOBVECINT,
                KNOBVECDOUBLE, KNOBVECSTRING, KNOBVECLONG, KNOBSHORT}

const i32 DISPNULL = 0x0000
const i32 DISPTIME = 0x0001
const i32 DISPXY   = 0x0002
const i32 DISPPSD  = 0x0004
const i32 DISPSPEC = 0x0008
const i32 DISPRAST = 0x0010
const i32 DISPOPTCPLX    = 0x0100
const i32 DISPOPTLOG     = 0x0200
const i32 DISPOPTSTEM    = 0x0400
const i32 DISPOPTSTRIP   = 0x0800
const i32 DISPOPTSCATTER = 0x1000

struct KnobProp {
  1: KnobType    type,
  2: string      units,
  3: string      description,
  4: i32         display,
  5: Knob        min,
  6: Knob        max,
  7: Knob        defaultvalue
}

typedef list<string> KnobIDList
typedef map<string, Knob> KnobMap
typedef map<string, KnobProp> KnobPropMap
typedef map<string, string> WaveformArgMap

service StreamReceiver {
        void push(1:VectorC data);
}

service ControlPort {
        void setKnobs(1:KnobMap knobs);
        KnobMap getKnobs(1:KnobIDList knobs);
        KnobMap getRe(1:KnobIDList knobs);
        KnobPropMap properties(1:KnobIDList knobs);
        void postMessage(1:string blk_alias, 2:string port, 3:binary msg);
        void shutdown();
}
