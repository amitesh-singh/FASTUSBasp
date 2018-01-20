/*
 * FASTUSBasp v2 - Fastest programmer for AVR
 * Copyright (C) 2018  Amitesh Singh <singh.amitesh@gmail.com>
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

#ifndef _RCC_H
#define _RCC_H

extern "C"
{
  #include <libopencm3/stm32/rcc.h>
}

//We are not supporting HSI clock
// This is only to support HSE clock with 8MHz XTAL
enum class ClockSpeed
{
  CLOCK8MHZ,
  CLOCK12MHZ,
  CLOCK16MHZ,
  CLOCK24MHZ,
  CLOCK48MHZ,
  CLOCK72MHZ,
  //overclocked MCU speed
  CLOCK128MHZ,
};

//Refer to clock tree in STM32F103 DS to have clear picture.

class RCC
{
public:
  static void enable(rcc_periph_clken);
  static void disable(rcc_periph_clken);
  //this sets the chip at 72MHz using hse 8 Mhz
  static void defaultClockSetup();
  //use RCC::_clkSpeed to know the set clock speed of MCU
  static ClockSpeed _clkSpeed;
};


#endif