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

#include "rcc.h"

extern "C"
{
   #include <libopencm3/stm32/flash.h>
}

ClockSpeed RCC::_clkSpeed;

void RCC::enable(rcc_periph_clken clk)
{
   rcc_periph_clock_enable(clk);
}

void RCC::disable(rcc_periph_clken clk)
{
   rcc_periph_clock_disable(clk);
}

void RCC::defaultClockSetup()
{
   _clkSpeed = ClockSpeed::CLOCK72MHZ;

   rcc_clock_setup_in_hse_8mhz_out_72mhz();
}