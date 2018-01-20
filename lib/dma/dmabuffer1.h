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

#ifndef _DMA_BUFFER_1_H
#define _DMA_BUFFER_1_H

#include "dmabuffer.h"

dmabuffer<DMA1, DMA_CHANNEL1> dma1Ch1Buffer;

extern "C" void dma1_channel1_isr(void)
{
   if ((DMA1_ISR &DMA_ISR_TCIF1) != 0)
     {
        DMA1_IFCR |= DMA_IFCR_CTCIF1;

        dma1Ch1Buffer.transferred = true;
        if (dma1Ch1Buffer._dma_cb) dma1Ch1Buffer._dma_cb();
     }

   dma_disable_channel(DMA1, DMA_CHANNEL1);
   //we need this below line if we want enable and disable dma complete interrupt 
   // on every buffer copy

   //dma_disable_transfer_complete_interrupt(DMA1, DMA_CHANNEL1);

   //DMA TCIF is an event flag and means trasnfer is completed.
   dma_clear_interrupt_flags(DMA1, DMA_CHANNEL1, DMA_TCIF);
}

#endif
