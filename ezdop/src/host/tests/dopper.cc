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
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <cstdio>
#include <ezdop.h>

int chunks = 3600;
const static int samples = 250;
int rate = 250;

static complex<float> buffer[samples];

int main(int argc, char *argv)
{
    ezdop *dop = new ezdop();

    printf("Initializing EZDOP...");
    if (dop->init())
        printf("done.\n");
    else
        printf("failed.\n");
    printf("EZDOP reports %s.\n", dop->is_online() ? "online" : "offline");
    
    printf("Setting EZDOP rate...");
    if (dop->set_rate(rate))
	printf("done.\n");
    else
	printf("failed.\n");

    printf("Telling EZDOP to rotate...");
    if (dop->rotate())
	printf("done.\n");
    else
	printf("failed.\n");

    printf("Telling EZDOP to stream...");
    if (dop->stream())
	printf("done.\n");
    else
	printf("failed.\n");

    float volume;

    for (int i = 0; i < chunks; i++) {
        printf("Asking EZDOP for %i samples...", samples);
        int rd = dop->read_iq(buffer, samples, volume);
	printf("got %i --- ", rd);
	if (rd != samples)
	    printf("*****\n");
	
	complex<float> average = complex<float>(0.0, 0.0);
	for (int j = 0; j < rd; j++) {
	    average += buffer[j];
	}

	float I = average.real()/rd;
	float Q = average.imag()/rd;
	float M = std::sqrt(I*I+Q*Q);
	float dbfs = 20*std::log(M/1.4142135);
		
	printf("I=%f Q=%f M=%f dbfs=%f\n", I, Q, M, dbfs);	
    }

    printf("Telling EZDOP to stop streaming...");
    if (dop->stop_streaming())
	printf("done.\n");
    else
	printf("failed.\n");

    printf("Telling EZDOP to stop stop rotating...");
    if (dop->stop_rotating())
        printf("done.\n");
    else
        printf("failed.\n");

    printf("Releasing EZDOP...");
    if (dop->finish())
        printf("done.\n");
    else
        printf("failed.\n");

    delete dop;
}
