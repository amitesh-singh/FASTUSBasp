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

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include "sleep.h"

static volatile uint32_t system_ms = 0;

__attribute__ ((used)) void sys_tick_handler(void)
{
  ++system_ms;
}

void clock_setup(void)
{
  //48/8 = 6 MHz
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
  // 6e6/6000 (N) = 1000 = 1 ms; N - 1 = is the argument 
  systick_set_reload(5999);
  systick_interrupt_enable();

  // start counting
  systick_counter_enable();
}

//This is a blocking call.
void msleep(uint32_t delay_ms)
{
  uint32_t wakeme = system_ms + delay_ms;
  while (wakeme > system_ms);
}