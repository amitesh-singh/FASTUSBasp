/*
 * FASTUSBasp v2 - Fastest programmer for AVR
 * Copyright (C) 2018  Amitesh Singh <singh.amitesh@gmail.com>
 *
 * Most of the code is copied from official USBasp firmware code.
 * Copyright (C) 2011 Thomas Fischl <tfischl@gmx.de>
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

#include "pin.h"
#include "rcc.h"

extern "C"
{
  #include <libopencm3/stm32/spi.h>
}

#include "config.h"

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

// isp functions declaration
void spi_setup(void);
uint8_t transmit(uint8_t send_byte);

void isp_connect(void);
void isp_disconnect(void);
uint8_t enter_into_prog_mode(void);

uint8_t read_flash(uint32_t address);
uint8_t write_flash(uint32_t address, uint8_t data, uint8_t pollmode);
uint8_t flush_page(uint32_t address, uint8_t pollvalue);

uint8_t read_eeprom(uint32_t address);
uint8_t write_eeprom(uint32_t address, uint8_t data);
void set_ack_option(uint8_t option);

// variables declaration
static uint8_t isp_hiaddr;
uint8_t prog_sck = 0;
int8_t prescalar_baudrate = -1;

pin mosi, miso, sck, rst;

void
isp_connect(void)
{
   rst.assign(ISP_RST_PORT, ISP_RST);
   rst.setMode(PinMode::OUTPUT_2MHZ, PinConfig::OUTPUT_PUSHPULL);

   spi_setup();

   //reset device
   rst.off();
   sck.off();

   time::delay(3);
   rst.on();

   time::delay(3);
   rst.off();

   isp_hiaddr = 0;
}

void
isp_disconnect(void)
{
   rst.setMode(PinMode::INPUT, PinConfig::INPUT_FLOAT);
   rst.off();

   mosi.setMode(PinMode::INPUT, PinConfig::INPUT_FLOAT);
   mosi.off();

   sck.setMode(PinMode::INPUT, PinConfig::INPUT_FLOAT);
   sck.off();

   spi_disable(SPI1);
}

void
spi_setup(void)
{
   mosi.assign(ISP_PORT, ISP_MOSI);
   mosi.setMode(PinMode::OUTPUT_2MHZ, PinConfig::OUTPUT_ALTFPUSHPULL);

   miso.assign(ISP_PORT, ISP_MISO);
   miso.setMode(PinMode::INPUT, PinConfig::INPUT_FLOAT);

   sck.assign(ISP_PORT, ISP_SCK);
   sck.setMode(PinMode::OUTPUT_2MHZ, PinConfig::OUTPUT_ALTFPUSHPULL);

   // works for >= SPI_CR1_BAUDRATE_FPCLK_DIV_16
   spi_init_master(ISP_BUS, prog_sck,
                   SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                   SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT,
                   SPI_CR1_MSBFIRST);
   if (prescalar_baudrate >= 0)
     spi_set_baudrate_prescaler(ISP_BUS, prescalar_baudrate);

   spi_enable_software_slave_management(ISP_BUS);
   spi_set_nss_high(ISP_BUS);
   spi_enable(ISP_BUS);
}

uint8_t
transmit(uint8_t send_byte)
{
   return spi_xfer(ISP_BUS, send_byte);
}

uint8_t
enter_into_prog_mode(void)
{
   uint8_t check;
   uint8_t count = 32;

   while (count--)
     {
        transmit(0xAC);
        transmit(0x53);
        check = transmit(0);
        transmit(0);

        if (check == 0x53)
          {
             return 0; //device does reply SUCCESS
          }

        time::delay(3);

        rst.on();
        time::delay(3);

        rst.off();
        time::delay(3);
     }

   //error: device does not reply.
   return 1;
}

static void
update_extended(uint32_t address)
{
   uint8_t curr_hiaddr;

   curr_hiaddr = (address >> 17);

   if(isp_hiaddr != curr_hiaddr)
     {
        isp_hiaddr = curr_hiaddr;
        transmit(0x4D);
        transmit(0x00);
        transmit(isp_hiaddr);
        transmit(0x00);
     }
}

uint8_t
read_flash(uint32_t address)
{
   update_extended(address);

   transmit(0x20 | ((address & 1) << 3));
   transmit(address >> 9);
   transmit(address >> 1);

   return transmit(0);
}

uint8_t
write_flash(uint32_t address, uint8_t data, uint8_t pollmode)
{
   update_extended(address);

   transmit(0x40 | ((address & 1) << 3));
   transmit(address >> 9);
   transmit(address >> 1);
   transmit(data);

   if (pollmode == 0)
     return 0;

   if (data == 0x7F)
     {
        time::delay(5);
        return 0;
     }
   else
     {
        uint8_t retries = 30;
        while (retries != 0)
          {
             if (read_flash(address) != 0x7F)
               {
                  return 0;
               };

             time::delay(3);
             retries--;
          }
     }
   return 1;
}

uint8_t
flush_page(uint32_t address, uint8_t pollvalue)
{
   update_extended(address);

   transmit(0x4C);
   transmit(address >> 9);
   transmit(address >> 1);
   transmit(0);

   if (pollvalue == 0xFF)
     {
        time::delay(5);
        return 0;
     }
   else
     {
        uint8_t retries = 30;
        while (retries != 0)
          {
             if (read_flash(address) != 0xFF)
               {
                  return 0;
               };

             time::delay(3);
             retries--;
          }
     }

   return 1;
}

uint8_t
read_eeprom(uint32_t address)
{
   transmit(0xA0);
   transmit(address >> 8);
   transmit(address);

   return transmit(0);
}

uint8_t
write_eeprom(uint32_t address, uint8_t data)
{
   transmit(0xC0);
   transmit(address >> 8);
   transmit(address);
   transmit(data);

   time::delay(10);

   return 0;
}

void
set_ack_option(uint8_t option)
{
   prescalar_baudrate = -1;
   prog_sck = SPI_CR1_BAUDRATE_FPCLK_DIV_16;
   //TODO: use spi_set_baudrate_prescaler() to support 
   // more speed options: SPI_CR1_BR_FPCLK_DIV_2/4...
   if (option == USBASP_ISP_SCK_AUTO)
     {
        prog_sck = SPI_CR1_BAUDRATE_FPCLK_DIV_16;
     }
   else if (option == USBASP_ISP_SCK_1500)
     {
        prog_sck = SPI_CR1_BAUDRATE_FPCLK_DIV_32;
     }
   else if (option == USBASP_ISP_SCK_750)
     {
        prog_sck = SPI_CR1_BAUDRATE_FPCLK_DIV_64;
     }
   else if (option == USBASP_ISP_SCK_375)
     {
        prog_sck = SPI_CR1_BAUDRATE_FPCLK_DIV_128;
     }
   else if (option == USBASP_ISP_SCK_187_5)
     {
        prog_sck = SPI_CR1_BAUDRATE_FPCLK_DIV_256;
     }
   else if (option == USBASP_ISP_SCK_93_75)
     {
        prog_sck = SPI_CR1_BAUDRATE_FPCLK_DIV_256;
        prescalar_baudrate = SPI_CR1_BR_FPCLK_DIV_2;
     }
   /*
      else if (option == USBASP_ISP_SCK_32)
      {

      }
      else if (option == USBASP_ISP_SCK_16)
      {

      }
      else if (option == USBASP_ISP_SCK_8)
      {

      }
      else if (option == USBASP_ISP_SCK_4)
      {

      }
      else if (option == USBASP_ISP_SCK_2)
      {

      }
      else if (option == USBASP_ISP_SCK_1)
      {

      }
      else if (option == USBASP_ISP_SCK_0_5)
      {

      }
      */
   else
     prog_sck = SPI_CR1_BAUDRATE_FPCLK_DIV_16;
}
