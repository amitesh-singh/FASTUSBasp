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

#ifndef _USB_UTIL_H
#define _USB_UTIL_H

#include "pin.h"
#include "delay.h"

namespace usb
{
   class util
     {
      public:
         // usb dplus hack for blue pills
         static void usbdplushack(uint8_t ms = 1)
           {
              RCC::enable(RCC_GPIOA);
              pin usbdplus(GPIOA, GPIO12);
              usbdplus.setMode(PinMode::OUTPUT_50MHZ, PinConfig::OUTPUT_PUSHPULL);
              usbdplus.off();
              time::delay(ms);
           }
     };
}

#endif

