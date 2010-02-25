/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Interface to daughterboard code
 */

#ifndef INCLUDED_DB_H
#define INCLUDED_DB_H

#include <usrp2_types.h>


struct db_base;

/* pointers to daughterboard structures */
extern struct db_base *tx_dboard;
extern struct db_base *rx_dboard;


//! Intermediate tuning information

struct tune_result
{
  //! The RF frequency that corresponds to DC in the IF from the daughterboard
  u2_fxpt_freq_t 	baseband_freq;

  //! The DDC/DUC frequency used to down/up convert to/from the target frequency
  u2_fxpt_freq_t 	dxc_freq;

  //! Any differerence btwn target and actual (typically < 0.01 Hz)
  u2_fxpt_freq_t 	residual_freq;

  //! Is the complex baseband spectrum inverted
  bool			inverted;
};


/*!
 * \brief One-time init at powerup
 *
 *  Sets rx_dboard, tx_dboard and initializes daughterboards.
 */
void 
db_init(void);

/*!
 * \brief Set daughterboard LO offset frequency.
 *
 * \param[in] db is the daughterboard instance
 * \param[in] offset is the amount to add to tuning requests
 * \param[out] success or failure
 */
bool
db_set_lo_offset(struct db_base *db, u2_fxpt_freq_t offset);

/*!
 * \brief Two stage tuning.  Given target_freq, tune LO and DDC/DUC
 *
 * \param[in] db is the daughterboard instance
 * \param[in] target_freq is the freq to translate the complex baseband to/from.
 * \param[out] result provides details of the resulting configuration.
 * 
 */
bool
db_tune(struct db_base *db, u2_fxpt_freq_t target_freq, struct tune_result *result);


/*
 * Set only the DDC frequency
 */
bool
db_set_ddc_freq(u2_fxpt_freq_t dxc_freq, u2_fxpt_freq_t *actual_dxc_freq);

/*
 * Set only the DUC frequency
 */
bool
db_set_duc_freq(u2_fxpt_freq_t dxc_freq, u2_fxpt_freq_t *actual_dxc_freq);


/*!
 * \brief Set gain
 */
bool
db_set_gain(struct db_base *db, u2_fxpt_gain_t gain);

bool
db_set_antenna(struct db_base *db, int ant);

/*!
 * \brief Read the eeprom value from the db, without defaulting to BasicRX/TX
 */
int
read_dboard_eeprom(int i2c_addr);

#endif /* INCLUDED_DB_H */
