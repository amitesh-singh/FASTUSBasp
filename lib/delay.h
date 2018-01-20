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

#ifndef _DELAY_H
#define _DELAY_H

extern "C"
{
  #include <libopencm3/cm3/nvic.h>
  #include <libopencm3/cm3/systick.h>
  #include <libopencm3/stm32/timer.h>
}

#include "rcc.h"

enum class TimeFactor
{
  MILLISECONDS, //miliseconds
  MICROSECONDS //micro second
};

class time
{
public:
  static void enable(TimeFactor tf = TimeFactor::MILLISECONDS);
  static void delay(uint32_t ms);
  static void udelay(uint32_t us);
  static void disable();
};

#endif
