/*
 * avrdude - A Downloader/Uploader for AVR device programmers
 * Copyright (C) 2003-2004  Theodore A. Roth  <troth@openavr.org>
 * Copyright (C) 2005 Johnathan Corgan <jcorgan@aeinet.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
 */

/*
 * avrdude interface for serial programming via FTDI bit bang mode operation. 
 */

#include "ac_cfg.h"
#include "avr.h"
#include "pgm.h"
#include "ftbb.h"
#include <string.h>

#if FTDI_SUPPORT
#define FTBB_DEBUG 0

#if HAVE_LIBFTD2XX
#include <ftd2xx.h>
#elif HAVE_FTDI_H
#include <ftdi.h>
#endif

#define RESET   (1<<(pgm->pinno[PIN_AVR_RESET]-1))
#define SCK	(1<<(pgm->pinno[PIN_AVR_SCK]-1))
#define MOSI	(1<<(pgm->pinno[PIN_AVR_MOSI]-1))
#define MISO	(1<<(pgm->pinno[PIN_AVR_MISO]-1))
#define FTDDR	(MOSI|SCK|RESET)

#if HAVE_LIBFTD2XX
static FT_STATUS status;
static FT_HANDLE handle;
#elif HAVE_FTDI_H
static struct ftdi_context device;
static int status;
#endif

static unsigned char txbits;

/* Send 8 bits over SPI bus with per-bit read back
 *
 */
static unsigned char ftbb_txrx(PROGRAMMER *pgm, unsigned char val)
{
    int i;
    unsigned char bits;
    unsigned char res;    

#if HAVE_LIBFTD2XX
    DWORD written;
#endif /* HAVE_LIBFTD2XX */

    res = 0;
    bits = 0;
    for (i = 0; i < 8; i++) {  // For each bit
        // Set up data on low phase of SCK
        txbits &= ~SCK;

        // Set MOSI to high bit of transmit data
        if (val & 0x80)
            txbits |= MOSI;
        else
            txbits &= ~MOSI;

#if HAVE_LIBFTD2XX
        FT_Write(handle, &txbits, 1, &written);
        // Clock just fell, read previous input bit and shift in
        FT_GetBitMode(handle, &bits);
#elif HAVE_FTDI_H
        ftdi_write_data(&device, &txbits, 1);
        // Clock just fell, read previous input bit and shift in
	ftdi_read_pins(&device, &bits);        
#endif /* HAVE_LIBFTD2XX */
        res = (res << 1) | (bits & MISO);

        // Now raise SCK to latch data in AVR
        txbits |= SCK;

#if HAVE_LIBFTD2XX
        FT_Write(handle, &txbits, 1, &written);
#elif HAVE_FTDI_H
	ftdi_write_data(&device, &txbits, 1);
#endif /* HAVE_LIBFTD2XX */
        // Move on to next bit in transmit data
        val = val << 1;
    }

    return res;
}

/* Generic programmer command function for pgm->cmd
 *
 * Sends four bytes of command in sequence and collects responses
 *
 */
static int ftbb_cmd(PROGRAMMER *pgm, unsigned char cmd[4], unsigned char res[4])
{
    int i;
    
#if FTBB_DEBUG
    printf("CMD: %02X%02X%02X%02X  ", cmd[0], cmd[1], cmd[2], cmd[3]);
#endif

    for (i = 0; i < 4; i++) {
        res[i] = ftbb_txrx(pgm, cmd[i]);
    }
    
#if FTBB_DEBUG
    printf("RES: %02X%02X%02X%02X\n", res[0], res[1], res[2], res[3]);
#endif

    return 0;
}

/* Programmer initialization command for pgm->initialize
 *
 * Pulse RESET with SCK low, then send SPI start programming command
 *
 */
static int ftbb_initialize(PROGRAMMER *pgm, AVRPART *p)
{
#if HAVE_LIBFTD2XX
    DWORD written;
#endif /* HAVE_LIBFTD2XX */

    // Atmel documentation says to raise RESET for 2 cpu clocks while sclk is low
    // then lower RESET and wait 20 ms.
    txbits |= RESET;

#if HAVE_LIBFTD2XX
    FT_Write(handle, &txbits, 1, &written);
#elif HAVE_FTDI_H
    ftdi_write_data(&device, &txbits, 1);
#endif /* HAVE_LIBFTD2XX */

    usleep(1000); // 2 AVR cpu clocks at any realistic clock rate
    txbits &= ~RESET;

#if HAVE_LIBFTD2XX
    FT_Write(handle, &txbits, 1, &written);
#elif HAVE_FTDI_H
    ftdi_write_data(&device, &txbits, 1);
#endif /* HAVE_LIBFTD2XX */

    usleep(20000);

    pgm->program_enable(pgm, p);
    return 0;
}

/* Programmer status display for pgm->display
 *
 * Not-implemented
 *
 */
static void ftbb_display(PROGRAMMER *pgm, char *p)
{
#if FTBB_DEBUG
    printf("ftbb: display called with: %s\n", p);
#endif

    printf("FTBB: RESET mapped to pinno %d\n", pgm->pinno[PIN_AVR_RESET]);
    printf("FTBB: SCK   mapped to pinno %d\n", pgm->pinno[PIN_AVR_SCK]);
    printf("FTBB: MOSI  mapped to pinno %d\n", pgm->pinno[PIN_AVR_MOSI]);
    printf("FTBB: MISO  mapped to pinno %d\n", pgm->pinno[PIN_AVR_MISO]);
}  

/* Programmer enable command for pgm->enable
 *
 * Lowers SCK and RESET in preparation for serial programming
 *
 */
static void ftbb_enable(PROGRAMMER *pgm)
{
#if HAVE_LIBFTD2XX
    DWORD written;
#endif /* HAVE_LIBFTD2XX */

    // Lower SCK & RESET
    txbits &= ~SCK;
    txbits &= ~RESET;

#if HAVE_LIBFTD2XX
    FT_Write(handle, &txbits, 1, &written);
#elif HAVE_FTDI_H
    ftdi_write_data(&device, &txbits, 1);
#endif /* HAVE_LIBFTD2XX */
}

/* Programmer disable command for pgm->disable
 *
 * Raises RESET to return to normal chip operation
 *
 */
static void ftbb_disable(PROGRAMMER *pgm)
{
#if HAVE_LIBFTD2XX
    DWORD written;
#endif /* HAVE_LIBFTD2XX */  
    // Raise RESET to return to normal mode
    txbits |= RESET;

#if HAVE_LIBFTD2XX
    FT_Write(handle, &txbits, 1, &written);
#elif HAVE_FTDI_H
    ftdi_write_data(&device, &txbits, 1);
#endif /* HAVE_LIBFTD2XX */
}

/* Programmer programming mode enable function for pgm->program_enable
 *
 * Starts SPI programming mode 
 *
 */
static int ftbb_program_enable(PROGRAMMER *pgm, AVRPART *p)
{
    unsigned char cmd[4];
    unsigned char res[4];
  
    if (p->op[AVR_OP_PGM_ENABLE] == NULL) {
        fprintf(stderr, "program enable instruction not defined for part \"%s\"\n", p->desc);
        return -1;
    }

    memset(cmd, 0, sizeof(cmd));
    avr_set_bits(p->op[AVR_OP_PGM_ENABLE], cmd);
    pgm->cmd(pgm, cmd, res);

    return 0;
}

/* Progammer erase function for pgm->erase
 *
 * Sends chip erase command and sleeps the chip erase delay
 *
 */ 
static int ftbb_chip_erase(PROGRAMMER *pgm, AVRPART *p)
{
    unsigned char cmd[4];
    unsigned char res[4];

    if (p->op[AVR_OP_CHIP_ERASE] == NULL) {
        fprintf(stderr, "chip erase instruction not defined for part \"%s\"\n", p->desc);
        return -1;
    }

    memset(cmd, 0, sizeof(cmd));
    avr_set_bits(p->op[AVR_OP_CHIP_ERASE], cmd);
    pgm->cmd(pgm, cmd, res);
    usleep(p->chip_erase_delay);

    return 0;
}


/* Parse routine for device name
 *
 * FFFF:FFFF:0
 *
 */
int ftbb_parse_name(char *name, int *vid, int *pid, int *ifc)
{
    printf("name=%s\n", name);
    return 0;
}

/* Programmer open command for pgm->open
 *
 * Opens FTD2XX device specified by 'name', performs a chip reset, then
 * sets the baudrate and bit bang mode
 *
 */
static int ftbb_open(PROGRAMMER *pgm, char *name)
{
    int vid, pid, ifc;
    ftbb_parse_name(name, &vid, &pid, &ifc);
    
#if HAVE_LIBFTD2XX
    int devnum = 0;
    if (strcmp(name, "ft0")) {
        fprintf(stderr, "ERROR: FTD2XX device selection not yet implemented!\n");
        return -1;
    }

    // Call FTD2XX library to open device
    if ((status = FT_Open(devnum, &handle)) != FT_OK) {
        fprintf(stderr, "Failed to open FTD2XX device #%d.\n", devnum);
        return -1;        
    }

    // Reset chipset
    if ((status = FT_ResetDevice(handle)) != FT_OK) {
        fprintf(stderr, "Failed to reset chipset for FTD2XX device #%d.\n", devnum);
        return -1;
    }

    // Set baud rate for bit bang interface
    if ((status = FT_SetBaudRate(handle, pgm->baudrate)) != FT_OK) {
        fprintf(stderr, "Failed to set baud rate for FTD2XX device #%d.\n", devnum);
        return -1;
    }

    // Set bit bang direction and mode
    if ((status = FT_SetBitMode(handle, FTDDR, 1)) != FT_OK) {
        fprintf(stderr, "Failed to set bit bang mode for FTD2XX device #%d.\n", devnum);
        return -1;
    }
#elif HAVE_FTDI_H
    // Open device via FTDI library *** FIXME *** hardcoded VID and PID
    if (ftdi_usb_open(&device, EZDOP_VENDORID, EZDOP_PRODUCTID)) {
	fprintf(stderr, "ftdi_usb_open: %s", device.error_str);
	return -1;
    }

    // Reset FTDI chipset
    if (ftdi_usb_reset(&device)) {
        fprintf(stderr, "ftdi_usb_reset: %s", device.error_str);
	return -1;
    }

    // Set FTDI chipset baudrate for bitbang
    if (ftdi_set_baudrate(&device, pgm->baudrate)) {
	fprintf(stderr, "ftdi_set_baudrate: %s", device.error_str);
	return -1;
    }

    // Enable bitbang
    if (ftdi_enable_bitbang(&device, FTDDR)) {
        fprintf(stderr, "ftdi_enable_bitbang: %s", device.error_str);
	return -1;
    }

    // Minimum chunk size for reads to reduce latency
    if (ftdi_read_data_set_chunksize(&device, 256)) {
        fprintf(stderr, "ftdi_read_data_set_chunksize: %s", device.error_str);
	return -1;
    }
#endif /* HAVE_LIBFTD2XX */

    return 0;
}

/* Programmer close function for pgm->close
 *
 * Releases FTD2XX device
 *
 */
static void ftbb_close(PROGRAMMER *pgm)
{
#if HAVE_LIBFTD2XX
    FT_Close(handle);
#elif HAVE_FTDI_H
    ftdi_deinit(&device);
#endif
}
#endif /* FTDI_SUPPORT */

/* Programmer initialization command for startup.
 *
 * Sets appropriate function pointers in structure
 * Tests FTD2XX interface by enumerating number of devices
 *
 */
void ftbb_initpgm (PROGRAMMER *pgm)
{
#if FTDI_SUPPORT

#if HAVE_LIBFTD2XX
    DWORD num_devices;
#endif

    strcpy(pgm->type, "ftbb");
    pgm->initialize = ftbb_initialize;
    pgm->display = ftbb_display;
    pgm->enable = ftbb_enable;
    pgm->disable = ftbb_disable;
    pgm->program_enable = ftbb_program_enable;
    pgm->chip_erase = ftbb_chip_erase;  
    pgm->cmd = ftbb_cmd;
    pgm->open = ftbb_open;
    pgm->close = ftbb_close;  

#if HAVE_LIBFTD2XX
    if ((status = FT_ListDevices(&num_devices, NULL, FT_LIST_NUMBER_ONLY)) != FT_OK)
        fprintf(stderr, "Failed to initialize FTD2XX interface. (%li)\n", status);
    else
        printf("%lu FTD2XX device(s) found.\n", num_devices);
#elif HAVE_FTDI_H
    if ((status = ftdi_init(&device)) < 0)
        fprintf(stderr, "Failed to initialize FTDI interface. (%i)\n", status);
#endif     

    txbits = RESET;
#endif /* FTDI_SUPPORT */
}

