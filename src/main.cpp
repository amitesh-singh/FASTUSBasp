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
#include "pin.h"
#include "delay.h"
#include "fastserial.h"

#include "usb/descriptor.h"
#include "dma/dmabuffer1.h"
#include <string.h>
#include "queue.h"

#if USBDPLUS_WRONG_PULLUP == 1

#include "usb/util.h"

#endif

extern "C"
{
   #include <libopencm3/usb/cdc.h>
}

#ifdef DEBUG
#include "serial.h"
#endif

//avrdude commands on control ep
#define USBASP_FUNC_CONNECT          1
#define USBASP_FUNC_DISCONNECT       2
#define USBASP_FUNC_TRANSMIT         3
#define USBASP_FUNC_READFLASH        4
#define USBASP_FUNC_ENABLEPROG       5
#define USBASP_FUNC_WRITEFLASH       6
#define USBASP_FUNC_READEEPROM       7
#define USBASP_FUNC_WRITEEEPROM      8
#define USBASP_FUNC_SETLONGADDRESS   9
#define USBASP_FUNC_SETISPSCK        10
#define USBASP_FUNC_GETCAPABILITIES  127

// programming state
#define PROG_STATE_IDLE         0
#define PROG_STATE_WRITEFLASH   1
#define PROG_STATE_READFLASH    2
#define PROG_STATE_READEEPROM   3
#define PROG_STATE_WRITEEEPROM  4

// Block mode flags
#define PROG_BLOCKFLAG_FIRST    1
#define PROG_BLOCKFLAG_LAST     2

static uint8_t prog_new_mode = 0;
static uint32_t prog_address;
static uint16_t prog_nbytes = 0;
static uint8_t prog_state = PROG_STATE_IDLE;
static uint16_t prog_pagesize;
static uint8_t prog_blockflags;
static uint8_t prog_pagecounter;

#include "isp.h"

static const char *usb_strings[] =
{
   "http://amitesh-singh.github.io", //manufacturer
   "fastusbasp", //product
   "AARAV" // Serial number
};

const static auto dev_desc = usb::device_desc(
   0x0200,
   0, // We will define this later in interface classes
   0,
   0,
   64,
   0x16c0,
   0x05dc,
   0x0104,
   1,
   2,
   3,
   1
   );

static const struct {
     struct usb_cdc_header_descriptor header;
     struct usb_cdc_call_management_descriptor call_mgmt;
     struct usb_cdc_acm_descriptor acm;
     struct usb_cdc_union_descriptor cdc_union;
} __attribute__((packed)) cdcacm_functional_descriptors = {
     .header = {
          .bFunctionLength = sizeof(struct usb_cdc_header_descriptor),
          .bDescriptorType = CS_INTERFACE,
          .bDescriptorSubtype = USB_CDC_TYPE_HEADER,
          .bcdCDC = 0x0110,
     },
     .call_mgmt = {
          .bFunctionLength =
             sizeof(struct usb_cdc_call_management_descriptor),
          .bDescriptorType = CS_INTERFACE,
          .bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,
          .bmCapabilities = 0,
          .bDataInterface = 1,
     },
     .acm = {
          .bFunctionLength = sizeof(struct usb_cdc_acm_descriptor),
          .bDescriptorType = CS_INTERFACE,
          .bDescriptorSubtype = USB_CDC_TYPE_ACM,
          .bmCapabilities = 0,
     },
     .cdc_union = {
          .bFunctionLength = sizeof(struct usb_cdc_union_descriptor),
          .bDescriptorType = CS_INTERFACE,
          .bDescriptorSubtype = USB_CDC_TYPE_UNION,
          .bControlInterface = 0,
          .bSubordinateInterface0 = 1,
     }
};

/*
 * This notification endpoint isn't implemented. According to CDC spec it's
 * optional, but its absence causes a NULL pointer dereference in the
 * Linux cdc_acm driver
 */
static const auto comm_endp_desc = usb::endpoint_desc(
   0x83,
   USB_ENDPOINT_ATTR_INTERRUPT,
   16,
   255
   );
static const usb_endpoint_descriptor comm_endp[] = {comm_endp_desc};

static const usb_interface_descriptor comm_iface[] =
{
   usb::interface_desc(0, 0, 1,
                       USB_CLASS_CDC, USB_CDC_SUBCLASS_ACM, USB_CDC_PROTOCOL_AT,
                       0, comm_endp,
                       &cdcacm_functional_descriptors, sizeof(cdcacm_functional_descriptors))
};

static const auto ep1_out = usb::endpoint_desc(
   0x01,
   USB_ENDPOINT_ATTR_BULK,
   64,
   1);

static const auto ep1_in = usb::endpoint_desc(
   0x82,
   USB_ENDPOINT_ATTR_BULK,
   64,
   1);

static const usb_endpoint_descriptor data_endp[] = {ep1_out, ep1_in};

static const usb_interface_descriptor data_iface[] =
{
   usb::interface_desc(1, 0, 2, USB_CLASS_DATA, 0, 0, 0, data_endp)
};

static const auto cdc_acm_interface_association =
usb::ifaceassoc_desc(0, 2, USB_CLASS_CDC, USB_CDC_SUBCLASS_ACM, USB_CDC_PROTOCOL_AT, 0);

//Vendor class interface -
// defines 0xFF as usb vendor class and interface number which is 2 in ifaces
static const usb_interface_descriptor vendor_iface[] =
{
   usb::interface_desc(2, 0, 0, USB_CLASS_VENDOR, 0, 0, 0, nullptr)
};

static const usb_interface ifaces[] =
{
   // CDC
   usb::interface(nullptr, 1, &cdc_acm_interface_association, comm_iface),
   usb::interface(nullptr, 1, nullptr, data_iface),

   // vendor class
   usb::interface(nullptr, 1, nullptr, vendor_iface)
};

static const auto config = usb::config_desc(3, 1, 0, 0x80, 0x32, ifaces);

static usbd_device *usbd_dev;

static void config_setup(usbd_device *usb, uint16_t wValue);
uint8_t ret;
static pin led;

static uint8_t usbdControlBuf[512];

static void
_serial1_rx_callback(volatile uint8_t &d)
{
   ret = d;
   usbd_ep_write_packet(usbd_dev, 0x82, &ret, 1);
}

static uint8_t usb_serial_databuf[64];
struct txbuf
{
  uint8_t buffer[64];
  uint8_t len;
} __attribute__((__packed__));

static txbuf TXBUF[16];
class dmaqueue: public util::queue<txbuf>
{
 public:
    dmaqueue(int n): util::queue<txbuf>(n, TXBUF) {}
    void push(uint8_t *buf, int len)
      {
         if (_count < _maxitems)
           {
              dma1Ch1Buffer.copy(_data[_back].buffer, buf, len);
              _data[_back].len = len;
              ++_back;
              ++_count;
              if (_back > _maxitems)
                {
                   _back-= (_maxitems + 1);
                }
           }
      }
};

void *__dso_handle = nullptr;
dmaqueue q(16);

#ifdef DEBUG
serial s2;
#endif

int
main()
{
   RCC::defaultClockSetup();

   //set up all the clocks
   RCC::enable(INBUILT_LED_RCC);
   RCC::enable(ISP_PORT_RCC);
   RCC::enable(ISP_RST_RCC);
   RCC::enable(ISP_BUS_RCC);

   //setup onboard led 
   led.assign(INBUILT_LED_PORT, INBUILT_LED);
   led.setMode(PinMode::OUTPUT_2MHZ, PinConfig::OUTPUT_PUSHPULL);
   led.on();

#ifdef DEBUG
   s2.begin(SerialType::Serial2, 9600);
#endif

   time::enable();

   fastserial1.setRxcb(_serial1_rx_callback);

   //This is required if proper pullup is not present at D+ line.
   // This is must for chinese stm32f103c8t6 aka "blue pill"
   // set USBDPLUS_WRONG_PULLUP to 1 if blue pill has wrong pullup at D+ line
   // This code is disabled by default.
#if USBDPLUS_WRONG_PULLUP == 1
     {
        pin usbdplus(GPIOA, GPIO12);
        usbdplus.setMode(PinMode::OUTPUT_50MHZ, PinConfig::OUTPUT_PUSHPULL);
        usbdplus.off();
        time::delay(1);
     }
#endif

   //usb setup code 
   RCC::enable(RCC_USB);

   usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev_desc, &config,
                        usb_strings, 3, usbdControlBuf, sizeof(usbdControlBuf));
   usbd_register_set_config_callback(usbd_dev, config_setup);

   while (1)
     {
        if (!q.empty())
          {
             while (!dma1Ch1Buffer.transferred)
               {
                  __asm__ volatile ("nop");
               }
             fastserial1.write(q.front().buffer, q.front().len);
             while(!fastserial1.transferred)
               {
                  __asm__ volatile ("nop");
               }
             q.pop();
          }

        usbd_poll(usbd_dev);
     }

   return 0;
}

static void
_serial_usb_data_in_cb(usbd_device *usb, uint8_t ep)
{
   (void) ep;
   int len;

   len = usbd_ep_read_packet(usb, 0x01, usb_serial_databuf, 64);
   if (len)
     {
        q.push(usb_serial_databuf, len);
     }
}

static void
usbuart_set_line_coding(struct usb_cdc_line_coding *coding)
{
   fastserial1.setBaudrate(coding->dwDTERate);

   if (coding->bParityType)
     fastserial1.setDatabits(coding->bDataBits + 1);
   else
     fastserial1.setDatabits(coding->bDataBits);

   switch(coding->bCharFormat)
     {
      case 0:
         fastserial1.setStopbits(USART_STOPBITS_1);
         break;
      case 1:
         fastserial1.setStopbits(USART_STOPBITS_1_5);
         break;
      case 2:
         fastserial1.setStopbits(USART_STOPBITS_2);
         break;
     }

   switch(coding->bParityType)
     {
      case 0:
         fastserial1.setParity(USART_PARITY_NONE);
         break;
      case 1:
         fastserial1.setParity(USART_PARITY_ODD);
         break;
      case 2:
         fastserial1.setParity(USART_PARITY_EVEN);
         break;
     }
}

static struct usb_cdc_line_coding _usb_cdc_line_coding_backup;

static int serial_control_request(usbd_device *usb, struct usb_setup_data *req, uint8_t **buf,
                                  uint16_t *len, void (**complete)(usbd_device *,
                                                                   struct usb_setup_data *))
{
   (void) complete;
   (void) usb;
   (void) req;

   switch (req->bRequest)
     {
      case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
           {
              /*
               * This Linux cdc_acm driver requires this to be implemented
               * even though it's optional in the CDC spec, and we don't
               * advertise it in the ACM functional descriptor.
               */
              char local_buf[10];
              usb_cdc_notification *notif = (usb_cdc_notification *)local_buf;

              /* We echo signals back to host as notification. */
              notif->bmRequestType = 0xA1;
              notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
              notif->wValue = 0;
              notif->wIndex = 0;
              notif->wLength = 2;
              local_buf[8] = req->wValue & 3;
              local_buf[9] = 0;
              usbd_ep_write_packet(usbd_dev, 0x82, local_buf, 10);

              return USBD_REQ_HANDLED;
           }
      case USB_CDC_REQ_SET_LINE_CODING:
           {
              if (*len < sizeof(struct usb_cdc_line_coding))
                return USBD_REQ_NOTSUPP;

              struct usb_cdc_line_coding *usbCdcLineCondig =(struct usb_cdc_line_coding *)*buf;
              memcpy(&_usb_cdc_line_coding_backup, usbCdcLineCondig,
                     sizeof(_usb_cdc_line_coding_backup));

              fastserial1.begin(115200);
              dma1Ch1Buffer.begin();
              usbuart_set_line_coding(&_usb_cdc_line_coding_backup);

              return USBD_REQ_HANDLED;
           }
     }
   return USBD_REQ_NEXT_CALLBACK;
}

static uint16_t
read_usb(uint8_t* data, uint16_t len)
{
   uint16_t i;

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


static uint16_t
write_usb(uint8_t* data, uint16_t len)
{
   uint8_t retVal = 0;
   uint16_t i;

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

static int
isp_control_endpoint(usbd_device *usb, struct usb_setup_data *req,
                     uint8_t **buf, uint16_t *len,
                     void (**complete)(usbd_device *, struct usb_setup_data *))
{
   (void) usb;
   (void) req;
   (void) complete;

   if ((req->bmRequestType & 0x7F) != USB_REQ_TYPE_VENDOR)
     return USBD_REQ_NEXT_CALLBACK;
   *len = 0;

   if (req->bRequest == USBASP_FUNC_CONNECT)
     {
        //disable USART
        fastserial1.end();
        led.off(); //PC13 led on
        isp_connect();
        prog_new_mode = 0;
     }
   else if (req->bRequest == USBASP_FUNC_DISCONNECT)
     {
        led.on(); //PC13 led off
        isp_disconnect();
        //enable USART again here.
        fastserial1.rebegin();
        dma1Ch1Buffer.begin();
        usbuart_set_line_coding(&_usb_cdc_line_coding_backup);
     }
   else if (req->bRequest == USBASP_FUNC_GETCAPABILITIES)
     {
        (*len) = 0;
        (*buf)[0] = 0;
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
     }
   else if (req->bRequest == USBASP_FUNC_ENABLEPROG)
     {
        (*buf)[0] = enter_into_prog_mode();
        (*len) = 1;
     }
   else if (req->bRequest == USBASP_FUNC_READFLASH)
     {
        if (!prog_new_mode)
          prog_address = req->wValue;

        prog_nbytes = req->wLength;
        prog_state = PROG_STATE_READFLASH;
        (*len) = read_usb((*buf), prog_nbytes);
     }
   else if (req->bRequest == USBASP_FUNC_WRITEFLASH)
     {
        if (!prog_new_mode)
          prog_address = req->wValue;

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
        prog_new_mode = 1;
        // set new address
        prog_address = (req->wIndex << 16) | req->wValue;
        (*len) = 0;
     }

   return USBD_REQ_HANDLED;
}

static void
config_setup(usbd_device *usb, uint16_t wValue)
{
   (void) usb;
   (void) wValue;

   usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, _serial_usb_data_in_cb);
   usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, nullptr);
   usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, nullptr);

   usbd_register_control_callback(usbd_dev,
                                  USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
                                  USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
                                  serial_control_request);

   usbd_register_control_callback(usbd_dev,
                                  USB_REQ_TYPE_VENDOR,
                                  USB_REQ_TYPE_TYPE,
                                  isp_control_endpoint);
}
