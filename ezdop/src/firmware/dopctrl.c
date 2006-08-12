/*
 * AE6HO EZ-Doppler firmware for onboard ATmega8 microcontroller
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

/*
 * ARCHITECTURE
 * 
 * Timer0 is 8000 Hz time base
 * PORTD.2 through PORTD.5 are doppler antenna array element enables.
 * PORTB.0 is a diagnostic LED, set low to light up
 * ADC7 is audio input
 * ADC6 is RSSI input - not yet used
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "dopctrl.h"

#define LED 0
#define turn_off_led() 	PORTB |= _BV(LED);
#define turn_on_led()	PORTB &= ~_BV(LED);

#define ANT1 _BV(2)
#define ANT2 _BV(3)
#define ANT3 _BV(4)
#define ANT4 _BV(5)
#define ANTMASK ANT1|ANT2|ANT3|ANT4

#define ADCAIN 7
#define BAUDRATE 250000

/* Assume these are all set to zero in startup code */

uint8_t rotate;         /* Flag to indicate antennas should rotate or not */
uint8_t streaming;      /* Flag to indicate continuous sampling */
uint8_t antennas;       /* Holds shadow copy of PORTD antennas */
uint8_t speed;          /* Holds samples per phase increment */
uint8_t phase;          /* Holds rotation phase (measured in samples */

uint8_t audio_hi;       /* High byte of ADC sample of audio */
uint8_t audio_lo;       /* Low byte of ADC sample of audio */

uint8_t rx;             /* Temporary holds received byte from USART */
uint8_t command;        /* Temporary to hold command when getting operand */
uint8_t cmdbyte;        /* Tracks bytes received in multi-byte commands */

int main(void)
{
	/* Diagnostic port setup */
	DDRB = _BV(LED);						/* PB0 is output */
	turn_off_led();
	
	/* Antenna control port setup */
	speed = 4;							/* Todo: read from EEPROM */
	antennas = ANT1;					/* Start with antenna #1 */
	PORTD = antennas;					/* Set port value */
	DDRD = ANTMASK;						/* Set antenna enables as PORTD outputs */
	
	/* ADC port setup */
	ADMUX = _BV(REFS0)|ADCAIN;			/* AVCC is reference, use ADC for audio input (ADC7) */
	ADCSRA = _BV(ADEN)|_BV(ADIE)|0x07; /* Enable converter, prescale by 128, enable ADC interrupt */
	
	/* USART port setup*/
	UCSRA = 0;							/* Normal asynchronous mode */
	UCSRB = _BV(TXEN)|_BV(RXEN)|_BV(RXCIE); /* Enable transmitter and receiver, and receiver interrupts */
	UCSRC = _BV(URSEL)|_BV(UCSZ1)|_BV(UCSZ0); /* 8N1 format */
	UBRRH = 0;							/* Set baud rate prescaler to 3 */
	UBRRL = 3;							/* To get 250000 bps */
	
	/* Set up 8000 Hz time base */
	timer_enable_int(_BV(TOIE0));		/* Turn on Timer0 output overflow interrupt */
	TCCR0 = _BV(CS01);					/* Clock Timer0 from CLK/8 */
	
	sei();								/* Let 'er rip! */
	return 0;
}

/* Timer0 overflow interrupt handler
 *
 * Creates 8000 Hz time base, or 125us budget
 *
 */
SIGNAL(SIG_OVERFLOW0)
{
	/* Reload Timer0 samples to 8, results in 8000 Hz overflow interrupt */
	TCNT0 = 0x08;

	if (streaming) {
		/* Kick-off an audio sample conversion, will interrupt 104us later */
		ADCSRA |= _BV(ADSC);

		/* Write the first byte of previous sample and enable UDRIE */
		UDR = audio_lo;
		UCSRB |= _BV(UDRIE);
	}

	if (!rotate)	/* Skip rotating antenna if not started */
		return;

	/* Increment antenna phase and see if antenna need to be rotated */
    if (++phase == speed) {
		phase = 0;
		
		/* Sequence antenna array elements */
		antennas >>= 1;
		antennas &= ANTMASK;

		if (!antennas)
			antennas = ANT4;
		PORTD = antennas;
	}
}

/* ADC conversion complete interrupt handler
 *
 * Read value and store. Assume prior sample has been handled.
 *
 */
SIGNAL(SIG_ADC)
{
	audio_lo = ADCL;
	audio_hi = ADCH;
}

/* USART data transmit holding register empty interrupt handler
 * 
 * First byte is always sent from timer interrupt
 * So second byte gets sent here with UDRIE disabled
 *
 */
SIGNAL(SIG_UART_DATA)
{
	/* Write second byte of previous sample and disable UDRIE */
	UDR = audio_hi | (antennas << 2);
	UCSRB &= ~_BV(UDRIE);
}

/* USART receive complete interrupt handler
 *
 * Received bytes are commands, with one or two bytes of operands following
 *
 */
SIGNAL(SIG_UART_RECV)
{
	rx = UDR;

	if (cmdbyte == 0) {
		if (rx == EZDOP_CMD_ROTATE)		/* Start rotation */
			rotate = 1;
		else if (rx == EZDOP_CMD_STOP)       /* Stop rotation */
			rotate = 0;
        else if (rx == EZDOP_CMD_RATE) {     /* Set rotation rate */
            command = rx;
            cmdbyte = 1;
        }
        else if (rx == EZDOP_CMD_STREAM)     /* Stream audio samples */
            streaming = 1;
        else if (rx == EZDOP_CMD_STROFF)     /* Stop streaming */
            streaming = 0;
		else
			turn_on_led();				/* Unknown command */
	}
	else if (cmdbyte == 1) {
        if (command == EZDOP_CMD_RATE) {     /* Operand is number of samples per phase increment */
            speed = rx;
            cmdbyte = 0;
        }
		else
			turn_on_led();				/* Bogus command state */
	}
	else
		turn_on_led();					/* Bogus command state */
}
