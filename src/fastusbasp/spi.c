/*
 * FASTUSBasp - Fastest programmer for AVR
 * Copyright (C) 2017  Amitesh Singh <singh.amitesh@gmail.com>
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

#include "usart1.h"
#include "spi.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>
#include "sleep.h"

#define ISP_PORT GPIOA
#define ISP_RCC RCC_GPIOA

#define ISP_MOSI GPIO7
#define ISP_MISO GPIO6
#define ISP_SCK  GPIO5
#define ISP_RST GPIO8


static uint8_t isp_hiaddr;
//extern uint8_t prog_sck;
uint8_t prog_sck = 0;
int8_t prescalar_baudrate = -1;

//! ------- INBUILT LED code

#define INBUILT_LED GPIO13
#define INBUILT_LED_PORT GPIOC
#define INBUILT_LED_RCC RCC_GPIOC

void
inbuilt_led_init(void)
{
   rcc_periph_clock_enable(INBUILT_LED_RCC);
   gpio_set_mode(INBUILT_LED_PORT, GPIO_MODE_OUTPUT_2_MHZ,
                 GPIO_CNF_OUTPUT_PUSHPULL, INBUILT_LED);
   //switch off GPIO13 LED
   gpio_set(GPIOC, INBUILT_LED);
}

void
inbuilt_led_green(bool val)
{
   if (val) gpio_clear(INBUILT_LED_PORT, INBUILT_LED);
   else gpio_set(INBUILT_LED_PORT, INBUILT_LED);
}
//! ------ INBUILT_LED Code ends

void
isp_connect(void)
{
   //led green on
   inbuilt_led_green(1);

   rcc_periph_clock_enable(RCC_GPIOA);
   gpio_set_mode(ISP_PORT, GPIO_MODE_OUTPUT_10_MHZ,
                 GPIO_CNF_OUTPUT_PUSHPULL, ISP_RST);

   spi_setup();

   //reset device
   gpio_clear(ISP_PORT, ISP_RST);
   gpio_clear(ISP_PORT, ISP_SCK);

   msleep(3);
   gpio_set(ISP_PORT, ISP_RST);
   msleep(3);
   gpio_clear(ISP_PORT, ISP_RST);
   isp_hiaddr = 0;
}

void
isp_disconnect(void)
{
   gpio_set_mode(ISP_PORT, GPIO_MODE_INPUT,
                 GPIO_CNF_INPUT_FLOAT, ISP_MOSI | ISP_SCK | ISP_RST);

   //led green off
   inbuilt_led_green(0);
}

void
spi_setup(void)
{
   //GPIOs: SS=PA4, SCK=PA5, MISO=PA6 and MOSI=PA7  
   rcc_periph_clock_enable(RCC_SPI1);
   gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ,
                 GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                 ISP_SCK | ISP_MOSI);

   gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                 ISP_MISO);
   spi_reset(SPI1);

   // works for >= SPI_CR1_BAUDRATE_FPCLK_DIV_16
   spi_init_master(SPI1, prog_sck,
                   SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                   SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT,
                   SPI_CR1_MSBFIRST);
   if (prescalar_baudrate >= 0)
     spi_set_baudrate_prescaler(SPI1, prescalar_baudrate);

   spi_enable_software_slave_management(SPI1);
   //spi_disable_crc(SPI1);
   spi_set_nss_high(SPI1);
   spi_enable(SPI1);
}

uint8_t
transmit(uint8_t send_byte)
{
   return spi_xfer(SPI1, send_byte);
}

uint8_t
enter_into_prog_mode(void)
{
   uint8_t check;
   uint8_t count = 32;

#ifdef DEBUG
   char buf[10];
#endif

   while (count--)
     {
        transmit(0xAC);
        transmit(0x53);
        check = transmit(0);
        transmit(0);

#ifdef DEBUG
        usart1_print("check: ");
        sprintf(buf, "%d", check);

        usart1_println(buf);
#endif

        if (check == 0x53)
          {
             return 0; //device does reply SUCCESS
          }

        msleep(3);
        gpio_set(ISP_PORT, ISP_RST);
        msleep(3);
        gpio_clear(ISP_PORT, ISP_RST);
        msleep(3);
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
        msleep(5);
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

             msleep(3);
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
        msleep(5);
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

             msleep(3);
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

   msleep(10);

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
