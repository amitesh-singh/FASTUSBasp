/*
 * FASTUSBasp - Fastest programmer for AVR
 * Copyright (C) 2017  Amitesh Singh <singh.amitesh@gmail.com>
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

#ifndef __STM32_FAST_USBASP_H
#define __STM32_FAST_USBASP_H

#include <libopencm3/stm32/spi.h>

/* ISP SCK speed identifiers */
#define USBASP_ISP_SCK_AUTO   0   // 3 MHz
#define USBASP_ISP_SCK_0_5    1   /* 500 Hz */
#define USBASP_ISP_SCK_1      2   /*   1 kHz */
#define USBASP_ISP_SCK_2      3   /*   2 kHz */
#define USBASP_ISP_SCK_4      4   /*   4 kHz */
#define USBASP_ISP_SCK_8      5   /*   8 kHz */
#define USBASP_ISP_SCK_16     6   /*  16 kHz */
#define USBASP_ISP_SCK_32     7   /*  32 kHz */
#define USBASP_ISP_SCK_93_75  8   /*  93.75 kHz */
#define USBASP_ISP_SCK_187_5  9   /* 187.5  kHz */
#define USBASP_ISP_SCK_375    10  /* 375 kHz   */
#define USBASP_ISP_SCK_750    11  /* 750 kHz   */
#define USBASP_ISP_SCK_1500   12 /* 1.5 MHz   */


void inbuilt_led_init(void);
void inbuilt_led_green(bool val);

void spi_setup(void);
uint8_t transmit(uint8_t send_byte);

void isp_connect(void);
void isp_disconnect(void);
uint8_t enter_into_prog_mode(void);

//void update_extended(uint32_t address);
uint8_t read_flash(uint32_t address);
uint8_t write_flash(uint32_t address, uint8_t data, uint8_t pollmode);
uint8_t flush_page(uint32_t address, uint8_t pollvalue);

uint8_t read_eeprom(uint32_t address);
uint8_t write_eeprom(uint32_t address, uint8_t data);
void set_ack_option(uint8_t option);

#endif
