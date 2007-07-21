/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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

#ifndef _USRP_CONFIG_H_
#define _USRP_CONFIG_H_

/*
 * ----------------------------------------------------------------
 * USRP Rx configurations.
 *
 * For now this is a placeholder, but will eventually specify the
 * mapping from A/D outputs to DDC inputs (I & Q).
 *
 * What's implemented today is a single DDC that has its I input
 * connected to ADC0 and its Q input connected to ADC1
 * ----------------------------------------------------------------
 */

#define	USRP_RX_CONFIG_DEFAULT	0

/*!
 * given a usrp_rx_config word, return the number of I & Q streams that
 * are interleaved on the USB.
 */

int usrp_rx_config_stream_count (unsigned int usrp_rx_config);

/*
 * USRP Tx configurations.
 *
 * For now this is a placeholder, but will eventually specify the
 * mapping from DUC outputs to D/A inputs.
 *
 * What's implemented today is a single DUC that has its I output
 * connected to DAC0 and its Q output connected to DAC1
 */

#define	USRP_TX_CONFIG_DEFAULT	0

/*!
 * given a usrp_tx_config word, return the number of I & Q streams that
 * are interleaved on the USB.
 */

int usrp_tx_config_stream_count (unsigned int usrp_tx_config);


#endif /* _USRP_CONFIG_H_ */
