#ifndef _SERIAL_H
#define _SERIAL_H

extern "C"
{
#include <libopencm3/stm32/usart.h>
}

#include "pin.h"
#include "serialcommon.h"
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

//XXX: add recv functions

class serial
{
   SerialType _serialType;
   pin _rx, _tx;
   uint32_t _nativeUSART;
   SerialMode _serialMode;

 public:
   serial():_serialMode(SerialMode::TX)
   {
   }
   void setSerialMode(SerialMode sm)
     {
        _serialMode = sm;
     }

   void begin(SerialType serialType = SerialType::Serial1, uint32_t usart = 9600);

   void print(const char *p);
   void println(const char *p);
   void printint(int);

   inline void write(uint8_t d)
   {
     usart_send_blocking(_nativeUSART, d);
   }
   
   inline void write(const uint8_t *p, uint16_t len)
   {
      for (uint16_t i = 0; i < len; ++i)
      {
        usart_send_blocking(_nativeUSART, *p);
        ++p;
      }
    }
   inline void send(uint8_t d)
   {
     usart_send(_nativeUSART, d);
   }

   inline void send(uint8_t *d, int len)
   {
     uint8_t *p = d;

     for (int i = 0; i < len; ++i)
     {
        usart_send(_nativeUSART, *p);
        p++;
     }
   }
   
   uint16_t recv();

   inline bool isReadReady()
   {
      return usart_get_flag(_nativeUSART, USART_SR_RXNE);
   }

   inline void setBaudrate(uint32_t baud)
   {
      usart_set_baudrate(_nativeUSART, baud);
   }

   inline void setDatabits(int bits)
   {
      usart_set_databits(_nativeUSART, bits);
   }

   inline void setStopbits(uint32_t bits)
   {
     usart_set_stopbits(_nativeUSART, bits);
   }

   inline void setParity(uint32_t parity)
   {
     usart_set_parity(_nativeUSART, parity);
   }
};

#endif
