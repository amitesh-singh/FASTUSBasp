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

#ifndef __STM32_FAST_USB_DEVICE_H
#define __STM32_FAST_USB_DEVICE_H

#define USBASP_FUNC_CONNECT        1
#define USBASP_FUNC_DISCONNECT     2
#define USBASP_FUNC_TRANSMIT       3
#define USBASP_FUNC_READFLASH      4
#define USBASP_FUNC_ENABLEPROG     5
#define USBASP_FUNC_WRITEFLASH     6
#define USBASP_FUNC_READEEPROM     7
#define USBASP_FUNC_WRITEEEPROM    8
#define USBASP_FUNC_SETLONGADDRESS 9
#define USBASP_FUNC_SETISPSCK      10

#define USBASP_FUNC_GETCAPABILITIES 127

// programming state
#define PROG_STATE_IDLE         0
#define PROG_STATE_WRITEFLASH   1
#define PROG_STATE_READFLASH    2
#define PROG_STATE_READEEPROM   3
#define PROG_STATE_WRITEEEPROM  4

// Block mode flags
#define PROG_BLOCKFLAG_FIRST    1
#define PROG_BLOCKFLAG_LAST     2

#include "spi.h"

static uint8_t prog_new_mode = 0;
static uint32_t prog_address;
static uint16_t prog_nbytes = 0;
static uint8_t prog_state = PROG_STATE_IDLE;
static uint16_t prog_pagesize;
static uint8_t prog_blockflags;
static uint8_t prog_pagecounter;

static const char *usb_strings[] =
{
   "ami",
   "FASTUSBasp",
};

static usbd_device *usbd_dev;

const struct usb_device_descriptor dev_descr = {
     .bLength = USB_DT_DEVICE_SIZE,
     .bDescriptorType = USB_DT_DEVICE,
     .bcdUSB = 0x0200,
     .bDeviceClass = 0xFF, // custom class 
     .bDeviceSubClass = 0,
     .bDeviceProtocol = 0,
     .bMaxPacketSize0 = 64,
     .idVendor =  0x16C0,
     .idProduct = 0x05DC,
     .bcdDevice = 0x0200,
     .iManufacturer = 1,
     .iProduct = 2,
     .iSerialNumber = 0,
     .bNumConfigurations = 1,
};

const struct usb_interface_descriptor iface = {
     .bLength = USB_DT_INTERFACE_SIZE,
     .bDescriptorType = USB_DT_INTERFACE,
     .bInterfaceNumber = 0,
     .bAlternateSetting = 0,
     .bNumEndpoints = 0,
     .bInterfaceClass = 0,
     .bInterfaceSubClass = 0,
     .bInterfaceProtocol = 0,
     .iInterface = 0,
     .extra = NULL,
     .extralen = 0,
};

const struct usb_interface ifaces[] = {{
     .num_altsetting = 1,
     .altsetting = &iface,
}};

const struct usb_config_descriptor config = {
     .bLength = USB_DT_CONFIGURATION_SIZE,
     .bDescriptorType = USB_DT_CONFIGURATION,
     .wTotalLength = 0,
     .bNumInterfaces = 1,
     .bConfigurationValue = 1,
     .iConfiguration = 0,
     .bmAttributes = 0x80,
     .bMaxPower = 0x32,

     .interface = ifaces,
};

uint16_t
read_usb(uint8_t* data, uint16_t len)
{
   uint8_t i;

   // check if programmer is in correct read state
   if ((prog_state != PROG_STATE_READFLASH)
       && (prog_state != PROG_STATE_READEEPROM))
     {
        return 0xff;
     }

   // fill packet ISP mode
   for (i = 0; i < len; i++)
     {
        if (prog_state == PROG_STATE_READFLASH)
          {
             data[i] = read_flash(prog_address);
          }
        else
          {
             data[i] = read_eeprom(prog_address);
          }
        prog_address++;
     }

   // last packet?
   if (len < 8)
     {
        prog_state = PROG_STATE_IDLE;
     }

   return len;
}


uint16_t
write_usb(uint8_t* data, uint16_t len)
{
   uint8_t retVal = 0;
   uint8_t i;

   /* check if programmer is in correct write state */
   if ((prog_state != PROG_STATE_WRITEFLASH)
       && (prog_state != PROG_STATE_WRITEEEPROM))
     {
        return 0xff;
     }

   for (i = 0; i < len; i++) {

        if (prog_state == PROG_STATE_WRITEFLASH)
          {
             /* Flash */
             if (prog_pagesize == 0)
               {
                  /* not paged */
                  write_flash(prog_address, data[i], 1);
               }
             else
               {
                  /* paged */
                  write_flash(prog_address, data[i], 0);
                  prog_pagecounter--;
                  if (prog_pagecounter == 0)
                    {
                       flush_page(prog_address, data[i]);
                       prog_pagecounter = prog_pagesize;
                    }
              }
          }
        else
          {
             // EEPROM 
             write_eeprom(prog_address, data[i]);
          }

        prog_nbytes--;

        if (prog_nbytes == 0)
          {
             prog_state = PROG_STATE_IDLE;
             if ((prog_blockflags & PROG_BLOCKFLAG_LAST)
                 && (prog_pagecounter != prog_pagesize))
               {
                  /* last block and page flush pending, so flush it now */
                  flush_page(prog_address, data[i]);
               }

             retVal = 1; // Need to return 1 when no more data is to be received
          }

        prog_address++;
   }

   return retVal;
}

/* Buffer to be used for control requests. */
static uint8_t usbd_control_buffer[128];

static int
control_request(usbd_device *dev, struct usb_setup_data *req,
                uint8_t **buf, uint16_t *len,
                void (**complete)(usbd_device *, struct usb_setup_data *))
{
   (void)complete;
   (void)dev;

   if ((req->bmRequestType & 0x7F) != USB_REQ_TYPE_VENDOR)
     return 0;

   (*len) = 0;

   if (req->bRequest == USBASP_FUNC_CONNECT)
     {
        isp_connect();
        prog_new_mode = 0;

#ifdef DEBUG
        usart1_println("FUNC_CONNECT");
#endif
     }
   else if (req->bRequest == USBASP_FUNC_DISCONNECT)
     {
        isp_disconnect();
#ifdef DEBUG
        usart1_println("FUNC_DISCONNECT");
#endif
     }
   else if (req->bRequest == USBASP_FUNC_GETCAPABILITIES)
     {
        (*len) = 0;
        (*buf)[0] = 0;
#ifdef DEBUG
        usart1_println("Get capabilities");
#endif
     }
   else if (req->bRequest == USBASP_FUNC_TRANSMIT)
     {
        (*buf)[0] = transmit(req->wValue & 0xFF);
        (*buf)[1] = transmit(req->wValue >> 8);
        (*buf)[2] = transmit(req->wIndex & 0xFF);
        (*buf)[3] = transmit(req->wIndex >> 8);

        (*len) = 4;
     }
   else if (req->bRequest == USBASP_FUNC_SETISPSCK)
     {
        set_ack_option(req->wValue);

        (*buf)[0] = 0;
        (*len) = 1;
#ifdef DEBUG
        usart1_println("SETISPSCK");
#endif
     }
   else if (req->bRequest == USBASP_FUNC_ENABLEPROG)
     {
#ifdef DEBUG
        usart1_println("USBASP_FUNC_ENABLEPROG");
#endif
        (*buf)[0] = enter_into_prog_mode();
        (*len) = 1;
     }
   else if (req->bRequest == USBASP_FUNC_READFLASH)
     {
        if (!prog_new_mode)
          prog_address = req->wValue;
#ifdef DEBUG
        usart1_println("USBASP_FUNC_READFLASH");
#endif

        prog_nbytes = req->wLength;
        prog_state = PROG_STATE_READFLASH;
        (*len) = read_usb((*buf), prog_nbytes);
     }
   else if (req->bRequest == USBASP_FUNC_WRITEFLASH)
     {
        if (!prog_new_mode)
          prog_address = req->wValue;

#ifdef DEBUG
        usart1_println("USBASP_FUNC_WRITEFLASH");
#endif
        prog_pagesize = req->wIndex & 0xFF;
        prog_blockflags = (req->wIndex >> 8) & 0x0F;
        prog_pagesize += ((req->wIndex >> 8) & 0xF0) << 4;
        if (prog_blockflags & PROG_BLOCKFLAG_FIRST)
          {
             prog_pagecounter = prog_pagesize;
          }
        prog_nbytes = req->wLength;
        prog_state = PROG_STATE_WRITEFLASH;
        (*len) = write_usb((*buf), prog_nbytes);
     }
   else if (req->bRequest == USBASP_FUNC_READEEPROM)
     {
        if (!prog_new_mode)
          prog_address = req->wValue;
        prog_nbytes = req->wLength;
        prog_state = PROG_STATE_READEEPROM;
        (*len) = read_usb((*buf), prog_nbytes);
     }
   else if (req->bRequest == USBASP_FUNC_WRITEEEPROM)
     {
        if (!prog_new_mode)
          prog_address = req->wValue;

        prog_pagesize = 0;
        prog_blockflags = 0;
        prog_nbytes = req->wLength;
        prog_state = PROG_STATE_WRITEEEPROM;
        (*len) = write_usb((*buf), prog_nbytes);
     }
   else if (req->bRequest == USBASP_FUNC_SETLONGADDRESS)
     {
        // set new mode of address delivering (ignore address delivered in commands)
        prog_address = 1;
        // set new address
        prog_address = (req->wIndex << 16) | req->wValue;
        (*len) = 0;
     }

   return 1;
}

static void
set_config(usbd_device *dev, uint16_t wValue)
{
   (void)wValue;

   usbd_register_control_callback(dev, USB_REQ_TYPE_VENDOR,
                                  USB_REQ_TYPE_TYPE,
                                  control_request);
}

#endif
