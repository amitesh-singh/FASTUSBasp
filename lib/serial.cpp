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

#include "serial.h"

#include "pin.h"
#include "rcc.h"

//Note:
// In Stm32f1, USART use abp2 frequency for USART1 
// and it uses abp1 frequency for other USARTs (2 and 3).
//
// Only USART1 can run at the maximum speed of 4.5Mbits/s. Other USART are limited to 2.25Mbits/s.
void serial::begin(SerialType _st, uint32_t usart)
{
   //RCC::enable(RCC_AFIO);

   //Tested and works
   if (_st == SerialType::Serial1)
     {
        _nativeUSART = USART1;
        RCC::enable(RCC_GPIOA);
        RCC::enable(RCC_USART1);

        if (_serialMode == SerialMode::RX || _serialMode == SerialMode::TX_RX)
          {
             _rx.assign(GPIOA, GPIO10);
             _rx.setMode(PinMode::INPUT, PinConfig::INPUT_FLOAT);
          }
        if (_serialMode == SerialMode::TX || _serialMode == SerialMode::TX_RX)
          {
             _tx.assign(GPIOA, GPIO9);
             _tx.setMode(PinMode::OUTPUT_2MHZ, PinConfig::OUTPUT_ALTFPUSHPULL);
          }
     }
   else if (_st == SerialType::Serial2)
     {
       //Tested and it works
        _nativeUSART = USART2;
        RCC::enable(RCC_GPIOA);
        RCC::enable(RCC_USART2);

        if (_serialMode == SerialMode::RX || _serialMode == SerialMode::TX_RX)
          {
             _rx.assign(GPIOA, GPIO3);
             _rx.setMode(PinMode::INPUT, PinConfig::INPUT_FLOAT);
          }
        if (_serialMode == SerialMode::TX || _serialMode == SerialMode::TX_RX)
          {
             _tx.assign(GPIOA, GPIO2);
             _tx.setMode(PinMode::OUTPUT_2MHZ, PinConfig::OUTPUT_ALTFPUSHPULL);
          }

     }
   else if (_st == SerialType::Serial3)
     {
        //Tested & it works.
        _nativeUSART = USART3;
        RCC::enable(RCC_GPIOB);
        RCC::enable(RCC_USART3);

        if (_serialMode == SerialMode::RX || _serialMode == SerialMode::TX_RX)
          {
             //GPIO_USART3_RX
             _rx.assign(GPIOB, GPIO11);
             _rx.setMode(PinMode::INPUT, PinConfig::INPUT_FLOAT);
          }
        if (_serialMode == SerialMode::TX || _serialMode == SerialMode::TX_RX)
          {
             //GPIO_USART3_TX
             _tx.assign(GPIOB, GPIO10);
             _tx.setMode(PinMode::OUTPUT_2MHZ, PinConfig::OUTPUT_ALTFPUSHPULL);
          }
     }
   //disable the USAST before setting it.
   usart_disable(_nativeUSART);

   usart_set_baudrate(_nativeUSART, usart);
   usart_set_databits(_nativeUSART, 8);
   usart_set_stopbits(_nativeUSART, USART_STOPBITS_1);

   if (_serialMode == SerialMode::TX)
     usart_set_mode(_nativeUSART, USART_MODE_TX);
   else if (_serialMode == SerialMode::TX_RX)
     usart_set_mode(_nativeUSART, USART_MODE_TX_RX);
   else
     usart_set_mode(_nativeUSART, USART_MODE_RX);

   usart_set_parity(_nativeUSART, USART_PARITY_NONE);
   usart_set_flow_control(_nativeUSART, USART_FLOWCONTROL_NONE);

   /* Finally enable the USART. */
   usart_enable(_nativeUSART);
}

void serial::print(const char *s)
{
   while (*s != 0)
     {
        usart_send_blocking(_nativeUSART, *s);
        s++;
     }
}

void serial::println(const char *s)
{
   print(s);
   usart_send_blocking(_nativeUSART, '\r');
   usart_send_blocking(_nativeUSART, '\n');
}

uint16_t serial::recv()
{
  return usart_recv_blocking(_nativeUSART);
}

#include <cstdio>

void serial::printint(int x)
{
  char buf[10];

  sprintf(buf, "%d", x);
  
  print(buf);
}
