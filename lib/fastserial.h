
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

#ifndef _FAST_SERIAL_H
#define _FAST_SERIAL_H

extern "C"
{
   #include <libopencm3/cm3/common.h>
   #include <libopencm3/cm3/nvic.h>
   #include <libopencm3/stm32/usart.h>
   #include <libopencm3/stm32/dma.h>
}

#include "pin.h"
#include "serialcommon.h"

//this implement very efficient and Fast serial 
// we are going to use interrupt for Rx and dma transfer for Tx

typedef void (*fastserial_cb)(volatile uint8_t &d);

//supporting only for usart1 for now :)
template<uint32_t _nativeUSART>
class fastserial
{
   pin _rx, _tx;
   uint32_t _baudrate;

 public:

   fastserial_cb _rx_cb;

   volatile uint8_t transferred = 0;
   volatile uint8_t retRx = 0;

   inline void setRxcb(fastserial_cb rx)
     {
        _rx_cb = rx;
     }

   void end()
     {
        usart_disable(_nativeUSART);
     }

   void rebegin()
     {
        begin(_baudrate);
     }

   void begin(uint32_t baudrate = 9600)
     {
        _baudrate = baudrate;
        RCC::enable(RCC_GPIOA);
        RCC::enable(RCC_DMA1);
        RCC::enable(RCC_USART1);
        nvic_enable_irq(NVIC_USART1_IRQ);

        _rx.assign(GPIOA, GPIO10);
        _rx.setMode(PinMode::INPUT, PinConfig::INPUT_FLOAT);

        _tx.assign(GPIOA, GPIO9);
        _tx.setMode(PinMode::OUTPUT_2MHZ, PinConfig::OUTPUT_ALTFPUSHPULL);

        //disable the USAST before setting it.
        usart_disable(_nativeUSART);

        usart_set_baudrate(_nativeUSART, baudrate);
        usart_set_databits(_nativeUSART, 8);
        usart_set_stopbits(_nativeUSART, USART_STOPBITS_1);
        usart_set_mode(_nativeUSART, USART_MODE_TX_RX);

        usart_set_parity(_nativeUSART, USART_PARITY_NONE);
        usart_set_flow_control(_nativeUSART, USART_FLOWCONTROL_NONE);

        //Rx is handled by interrupt
        usart_enable_rx_interrupt(_nativeUSART);

        /* Finally enable the USART. */
        usart_enable(_nativeUSART);

        //Tx is handled by dma. set it up now
        nvic_set_priority(NVIC_DMA1_CHANNEL4_IRQ, 0);
        nvic_enable_irq(NVIC_DMA1_CHANNEL4_IRQ);
     }

   void write(uint8_t *d, uint16_t len)
     {
        transferred = false;

        //reset dma channel
        dma_channel_reset(DMA1, DMA_CHANNEL4);
        dma_disable_channel(DMA1, DMA_CHANNEL4);

        dma_set_peripheral_address(DMA1, DMA_CHANNEL4, (uint32_t)&USART1_DR);

        dma_set_memory_address(DMA1, DMA_CHANNEL4, (uint32_t)d);
        dma_set_number_of_data(DMA1, DMA_CHANNEL4, len);
        dma_set_read_from_memory(DMA1, DMA_CHANNEL4);
        dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL4);
        dma_set_peripheral_size(DMA1, DMA_CHANNEL4, DMA_CCR_PSIZE_8BIT);
        dma_set_memory_size(DMA1, DMA_CHANNEL4, DMA_CCR_MSIZE_8BIT);
        dma_set_priority(DMA1, DMA_CHANNEL4, DMA_CCR_PL_VERY_HIGH);

        dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL4);

        dma_enable_channel(DMA1, DMA_CHANNEL4);

        usart_enable_tx_dma(_nativeUSART);
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

//declare object as global.
fastserial<USART1> fastserial1;

extern "C" void usart1_isr(void)
{
   /* Check if we were called because of RXNE. */
   if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
       ((USART_SR(USART1) & USART_SR_RXNE) != 0))
     {
        /* Retrieve the data from the peripheral. */
        fastserial1.retRx = usart_recv(USART1);
        //its better to set RxCb and use this callback to play with recieved data
        if (fastserial1._rx_cb) fastserial1._rx_cb(fastserial1.retRx);
     }
   //not interested in Tx since its handled by dma
}

//Tx
extern "C" void dma1_channel4_isr(void)
{
   if ((DMA1_ISR &DMA_ISR_TCIF4) != 0)
     {
        DMA1_IFCR |= DMA_IFCR_CTCIF4;

        fastserial1.transferred = true;
     }

   dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL4);
   usart_disable_tx_dma(DMA1);
   dma_disable_channel(DMA1, DMA_CHANNEL4);
}


#endif
