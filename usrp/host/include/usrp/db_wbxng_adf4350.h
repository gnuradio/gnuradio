/*
 * Copyright 2009 Ettus Research LLC
 */

#ifndef ADF4350_H
#define ADF4350_H

#include <usrp/db_wbxng_adf4350_regs.h>
#include <usrp/db_base.h>
#include <stdint.h>

typedef uint64_t freq_t;
class adf4350_regs;

class adf4350
{
public:
    adf4350(usrp_basic_sptr _usrp, int _which, int _spi_enable);
    ~adf4350();
    void _update();
    bool _get_locked();
    void _enable(bool enable);
    void _write(uint8_t addr, uint32_t data);
    bool _set_freq(freq_t freq);
    freq_t _get_freq();
    freq_t _get_max_freq();
    freq_t _get_min_freq();

protected:
    usrp_basic_sptr d_usrp;
    int d_which;
    int d_spi_enable;
    int d_spi_format;
    adf4350_regs *d_regs;
};

#endif /* ADF4350_H */
