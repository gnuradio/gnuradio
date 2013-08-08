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

[["python:package:gnuradio.ctrlport"]]

#ifndef GNURADIO_DEBUG
#define GNURADIO_DEBUG

module GNURadio {

struct complex {
  float re;
  float im;
};

class Knob {};
class KnobB extends Knob { bool   value; };
class KnobC extends Knob { byte   value; };
class KnobI extends Knob { int 	  value; };
class KnobT extends Knob { short  value; };
class KnobF extends Knob { float  value; };
class KnobD extends Knob { double value; };
class KnobL extends Knob { long   value; };
class KnobS extends Knob { string value; };
class KnobZ extends Knob { complex value; };

sequence<bool>   VectorB; sequence<byte>   VectorC; 
sequence<int>    VectorI; sequence<float>  VectorF;
sequence<double> VectorD; sequence<string> VectorS;
sequence<long> 	 VectorL; sequence<short>  VectorT;

class KnobVecB extends Knob { VectorB value; }; 
class KnobVecC extends Knob { VectorC value; }; 
class KnobVecI extends Knob { VectorI value; }; 
class KnobVecT extends Knob { VectorT value; }; 
class KnobVecF extends Knob { VectorF value; }; 
class KnobVecD extends Knob { VectorD value; }; 
class KnobVecL extends Knob { VectorL value; }; 
class KnobVecS extends Knob { VectorS value; }; 

enum KnobType { KNOBBOOL, 	KNOBCHAR, 	KNOBINT, 	KNOBFLOAT, 
		KNOBDOUBLE, 	KNOBSTRING, 	KNOBLONG, 	KNOBVECBOOL, 
		KNOBVECCHAR, 	KNOBVECINT,	KNOBVECFLOAT, 	KNOBVECDOUBLE, 
		KNOBVECSTRING, 	KNOBVECLONG,    KNOBSHORT};

const int DISPNULL = 0x0000;
const int DISPTIME = 0x0001;
const int DISPXY   = 0x0002;
const int DISPPSD  = 0x0004;
const int DISPSPEC = 0x0008;
const int DISPRAST = 0x0010;
const int DISPOPTCPLX    = 0x0100;
const int DISPOPTLOG     = 0x0200;
const int DISPOPTSTEM    = 0x0400; 
const int DISPOPTSTRIP   = 0x0800;
const int DISPOPTSCATTER = 0x1000;

struct KnobProp {
	KnobType    type;
	string      units;
	string      description;
        int         display;
	Knob        min;
	Knob        max;
	Knob        defaultvalue;
};

sequence<string> 		KnobIDList;
dictionary<string, Knob> 	KnobMap;
dictionary<string, KnobProp> 	KnobPropMap;
dictionary<string, string>	WaveformArgMap;

interface StreamReceiver {
    void 			push(VectorC 	data);
};

interface ControlPort {
    void 			set(KnobMap knobs);
    idempotent  KnobMap 	get(KnobIDList knobs);
    idempotent  KnobMap 	getRe(KnobIDList knobs);
    idempotent  KnobPropMap 	properties(KnobIDList knobs);
    void 			shutdown();
};

};

#endif
