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

#include "delay.h"
#include "rcc.h"

static volatile uint32_t system_ms = 0;

extern "C" __attribute__ ((used)) void sys_tick_handler(void)
{
   ++system_ms;
}

void time::enable(TimeFactor tf)
{
   uint32_t val = 9000 - 1;

   systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

   //TODO: Add support for other clock speed
   if (RCC::_clkSpeed == ClockSpeed::CLOCK72MHZ)
     {
        // 9e6/9000 (N) = 1000 = 1 ms; N - 1 = is the argument 
        if (tf == TimeFactor::MILLISECONDS)
          val = 9000 - 1;
        else
          val = 8;
     }
   else if (RCC::_clkSpeed == ClockSpeed::CLOCK128MHZ)
     {
        if (tf == TimeFactor::MILLISECONDS)
          val = 16000 - 1;
        else
          val = 15;
     }
   else if (RCC::_clkSpeed == ClockSpeed::CLOCK48MHZ)
     {
        if (tf == TimeFactor::MILLISECONDS)
          val = 6000 - 1;
        else
          val = 5;
     }
   else if (RCC::_clkSpeed == ClockSpeed::CLOCK8MHZ)
     {
        //1e6/1000 = 1000, 
        if (tf == TimeFactor::MILLISECONDS)
          val = 1000 - 1;
        else
          //TODO: implement it later
          val = 0; 
     }
   else if (RCC::_clkSpeed == ClockSpeed::CLOCK16MHZ)
     {
        if (tf == TimeFactor::MILLISECONDS)
          val = 2000 - 1;
        else
          val = 1;
     }
   else if (RCC::_clkSpeed == ClockSpeed::CLOCK12MHZ)
     {
        systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
        //12e6/12000 = 1000 
        if (tf == TimeFactor::MILLISECONDS)
          {
             val = 12000 - 1;
          }
        else
          val = 12 - 1;
     }

   systick_set_reload(val);
   systick_interrupt_enable();

   // start counting
   systick_counter_enable();
}

void time::delay(uint32_t delay_ms)
{
   uint32_t wakeme = system_ms + delay_ms;
   while (wakeme > system_ms);
   // reset system ms = 0;
   //i think we would be fine in most of the cases.
   system_ms = 0;
}

void time::udelay(uint32_t delay_ms)
{
   uint32_t wakeme = system_ms + delay_ms;
   while (wakeme > system_ms);

   system_ms = 0;
}

void time::disable()
{
   systick_interrupt_disable();
}
