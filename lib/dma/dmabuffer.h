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

#ifndef _DMA_BUFFER_H
#define _DMA_BUFFER_H

#include "rcc.h"

extern "C"
{
  #include <libopencm3/cm3/nvic.h>
  #include <libopencm3/stm32/dma.h>
}

typedef void (*dmabuffer_cb)();

template<uint32_t dma, uint8_t channel>
class dmabuffer
{
   void _enableNvicIrq()
     {
        if (dma == DMA1 && channel == DMA_CHANNEL1)
          nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);
        else if (dma == DMA1 && channel == DMA_CHANNEL7)
          nvic_enable_irq(NVIC_DMA1_CHANNEL7_IRQ);

        //XXX: write for others
     }

   uint32_t _memsize = DMA_CCR_MSIZE_8BIT;

 public:
   dmabuffer_cb _dma_cb;

   // Set the callback which gets executed in ISR
   inline void setCb(dmabuffer_cb dmacb)
     {
        _dma_cb = dmacb;
     }

   //This set the memory size for both src and dst
   //for char copy, use DMA_CCR_MSIZE_8BIT 
   inline void setMemorySize(uint32_t memsize)
     {
        _memsize = memsize;
     }

   //this checks if dma copy mem2mem is done
   volatile bool transferred;

   // call it before doing dma operation
   void begin()
     {
        RCC::enable(RCC_DMA1);

        _enableNvicIrq();

        //reset the channel
        dma_channel_reset(dma, channel);
        //disable the channel before changing the dma settings
        dma_disable_channel(dma, channel);

        //enable mem2mem 
        dma_enable_mem2mem_mode(dma, channel);
        dma_set_priority(dma, channel, DMA_CCR_PL_HIGH);
        //32bit wide transfer for source and destination
        // dma_set_memory_size(dma, channel, DMA_CCR_MSIZE_8BIT);
        // dma_set_peripheral_size(dma, channel, DMA_CCR_MSIZE_8BIT);
        dma_set_memory_size(dma, channel, _memsize);
        dma_set_peripheral_size(dma, channel, _memsize);

        //After every 32bits we have to increase the address because
        // we use RAM
        dma_enable_memory_increment_mode(dma, channel);
        dma_enable_peripheral_increment_mode(dma, channel);

        //we define the source as peripherals
        dma_set_read_from_peripheral(dma, channel);

        //enable interrupt on transfer complete.
        dma_enable_transfer_complete_interrupt(dma, channel);
        transferred = false;
     }

   void copy(void *dst, const void *src, uint16_t len)
     {
        transferred = false;
        dma_set_number_of_data(dma, channel, len);
        dma_set_peripheral_address(dma, channel, uint32_t (src));
        dma_set_memory_address(dma, channel, uint32_t (dst));
        dma_enable_channel(dma, channel);
     }

   //This disables the dma@channel
   inline void disable()
     {
        dma_disable_channel(dma, channel);
     }
};


#endif
