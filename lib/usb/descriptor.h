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

#ifndef _USB_DESCRIPTOR_H
#define _USB_DESCRIPTOR_H

extern "C"
{
   #include <libopencm3/usb/usbd.h>
}

//this defines the list of usb class codes 
// Refer: http://www.usb.org/developers/defined_class

//Device class is unspecified, interface descriptors are used to determine needed drivers
#define USB_LET_INTERFACE_HANDLE_CLASS 0x00

// Speaker, microphone, sound card, MIDI
#define USB_AUDIO_CLASS 0x01

//Modem, Ethernet adapter, Wi-Fi adapter, RS232 serial adapter. Used together with class 0Ah (below)
#define USB_CDC_CLASS 0x02

//Keyboard, mouse, joystick (HID)
#define USB_HID_CLASS 0x03

// Force feedback joystick (PID)
#define USB_PHYSICAL_CLASS 0x05

// (PTP/MTP) Webcam, scanner
#define USB_IMAGE_CLASS 0x06

// Laser printer, inkjet printer, CNC machine
#define USB_PRINTER_CLASS 0x07

//USB flash drive, memory card reader, digital audio player, digital camera, external drive
#define USB_MASSSTORAGE_CLASS 0x08

// Full bandwidth hub
#define USB_HUB_CLASS 0x09

// Used together with class 02h (above)
#define USB_CDCDATA_CLASS 0x0A

// USB smart card reader
#define USB_SMARTCARD_CLASS 0x0B

// Fingerprint reader
#define USB_CONTENTSECURITY_CLASS 0x0D

// webcam
#define USB_VIDEO_CLASS 0x0E

// Pulse monitor (watch)
#define USB_PERSONALHEALTHCARE_CLASS 0x0F

// Pulse monitor (watch) (AV)
#define USB_AUDIOVIDEO_CLASS 0x10

// Describes USB Type-C alternate modes supported by device
#define USB_BILLBOARD_CLASS 0x11

#define USB_TYPECBRIDGE_CLASS 0x12

//USB compliance testing device
#define USB_SIAGNOSTIC_CLASS 0xDC

//Bluetooth adapter, Microsoft RNDIS
#define USB_WIRELESS_CONTROLL 0xE0

// ActiveSync device
#define USB_MISC_CLASS 0xEF

// IrDA Bridge, Test & Measurement Class (USBTMC),USB DFU (Device Firmware Upgrade)
#define USB_APPSPECIFIC_CLASS 0xFE

// Indicates that a device needs vendor-specific drivers
#ifndef USB_VENDOR_CLASS
#define USB_VENDOR_CLASS 0xFF
#endif

namespace usb
{
   constexpr struct usb_device_descriptor
      device_desc(uint16_t bcdUSB,
                  uint8_t bDeviceClass,
                  uint8_t bDeviceSubClass,
                  uint8_t bDeviceProtocol,
                  uint8_t bMaxPacketSize0,
                  uint16_t idVendor,
                  uint16_t idProduct,
                  uint16_t bcdDevice,
                  uint8_t iManufacturer,
                  uint8_t iProduct,
                  uint8_t iSerialNumber,
                  uint8_t bNumConfigurations
                 )
        {
           return {
                /*
                  Size of the Descriptor in Bytes (18 bytes)
                */
        	    USB_DT_DEVICE_SIZE,
                /*
                  Device Descriptor (0x01)
                */
                USB_DT_DEVICE, //1
                /*
                  USB Specification Number which device complies too
                */
                bcdUSB,
                /*
                  - Class Code (Assigned by USB Org)
                  - If equal to Zero, each interface specifies it’s own class code
                  - If equal to 0xFF, the class code is vendor specified.
                  - Otherwise field is valid Class Code.
                */
                bDeviceClass,
                /*
                  Subclass Code (Assigned by USB Org)
                */
                bDeviceSubClass,
                /*
                  Protocol Code (Assigned by USB Org)
                */
                bDeviceProtocol,
                /*
                  Maximum Packet Size for Zero Endpoint. Valid Sizes are 8, 16, 32, 64
                */
                bMaxPacketSize0,
                /*
                  Vendor ID (Assigned by USB Org)
                */
                idVendor,
                /*
                  Product ID (Assigned by Manufacturer)
                */
                idProduct,
                /*
                  Device Release Number
                */
                bcdDevice,
                /*
                  Index of Manufacturer String Descriptor
                */
                iManufacturer,
                /*
                  Index of Product String Descriptor
                */
                iProduct,
                /*
                  Index of Serial Number String Descriptor
                */
                iSerialNumber,
                /*
                  Number of Possible Configurations
                */
                bNumConfigurations
           };
        }

   constexpr usb_endpoint_descriptor
      endpoint_desc(uint8_t bEndpointAddress,
                    uint8_t bmAttributes,
                    uint16_t wMaxPacketSize,
                    uint8_t bInterval,
                    const void *extra = nullptr,
                    int extralen = 0)
        {
           return {
                /*
                  Size of Descriptor in Bytes (7 bytes)
                */
        	    USB_DT_INTERFACE_SIZE,
                /*
                  Endpoint Descriptor (0x05)
                */
                USB_DT_ENDPOINT, //5
                /*
                  - Endpoint Address
                  - Bits 0..3b Endpoint Number.
                  - Bits 4..6b Reserved. Set to Zero
                  - Bits 7 Direction 0 = Out, 1 = In (Ignored for Control Endpoints)
                */
                bEndpointAddress,
                 /*
                  - Bits 0..1 Transfer Type
                      00 = Control
                      01 = Isochronous
                      10 = Bulk
                      11 = Interrupt

                  - Bits 2..7 are reserved. If Isochronous endpoint,
                  - Bits 3..2 = Synchronisation Type (Iso Mode)
                      00 = No Synchonisation
                      01 = Asynchronous
                      10 = Adaptive
                      11 = Synchronous

                  - Bits 5..4 = Usage Type (Iso Mode)
                      00 = Data Endpoint
                      01 = Feedback Endpoint
                      10 = Explicit Feedback Data Endpoint
                      11 = Reserved
                */
                bmAttributes,
                /*
                  Maximum Packet Size this endpoint is capable of sending or receiving
                */
                wMaxPacketSize,
                /*
                  Interval for polling endpoint data transfers. Value in frame counts.
                  Ignored for Bulk & Control Endpoints. Isochronous must 
                  equal 1 and field may range from 1 to 255 for interrupt endpoints.
                */
                bInterval,
                /*
                */
                extra,
                /*
                */
                extralen
           };
        }

   constexpr usb_interface_descriptor 
      interface_desc(uint8_t bInterfaceNumber,
                     uint8_t bAlternateSetting,
                     uint8_t bNumEndpoints,
                     uint8_t bInterfaceClass,
                     uint8_t bInterfaceSubClass,
                     uint8_t bInterfaceProtocol,
                     uint8_t iInterface,
                     const struct usb_endpoint_descriptor *endpoint = nullptr,
                     const void *extra = nullptr,
                     int extralen = 0
                    )
        {
           return {
                /*
                  Size of Descriptor in Bytes (9 Bytes)
                */
        	    USB_DT_INTERFACE_SIZE,
                /*
                  Interface Descriptor (0x04)
                */
                USB_DT_INTERFACE, //4
                /*
                  Number of Interface
                */
                bInterfaceNumber,
                /*
                  Value used to select alternative setting
                */
                bAlternateSetting,
                /*
                  Number of Endpoints used for this interface
                */
                bNumEndpoints,
                /*
                  Class Code (Assigned by USB Org)
                */
                bInterfaceClass,
                /*
                  Subclass Code (Assigned by USB Org)
                */
                bInterfaceSubClass,
                /*
                  Protocol Code (Assigned by USB Org)
                */
                bInterfaceProtocol,
                /*
                  Index of String Descriptor Describing this interface
                */
                iInterface,
                /*
                */
                endpoint,
                /*
                */
                extra,
                /*
                */
                extralen
           };
        }

   constexpr usb_interface interface(uint8_t *cur_altsetting,
                                     uint8_t num_altsetting,
                                     const struct usb_iface_assoc_descriptor *iface_assoc,
                                     const struct usb_interface_descriptor *altsetting 
                                    )
     {
        return { cur_altsetting,
             num_altsetting,
             iface_assoc,
             altsetting
        };
     }

   constexpr usb_config_descriptor 
      config_desc(uint8_t bNumInterfaces,
                  uint8_t bConfigurationvalue,
                  uint8_t iConfiguration,
                  uint8_t bmAttributes,
                  uint8_t bMaxPower,
                  const struct usb_interface *interface
                 )
        {
           return {
                /*
                  Size of Descriptor in Bytes
                */
        	   USB_DT_CONFIGURATION_SIZE,
                /*
                  Configuration Descriptor (0x02)
                */
                USB_DT_CONFIGURATION, //2
                /*
                  Total length in bytes of data returned
                */
                0, /** Refer Note 1 **/
                /*
                  Number of Interfaces
                */
                bNumInterfaces,
                /*
                  Value to use as an argument to select this configuration
                */
                bConfigurationvalue,
                /*
                  Index of String Descriptor describing this configuration
                */
                iConfiguration,
                /*
                  - D7 Reserved, set to 1. (USB 1.0 Bus Powered)

                  - D6 Self Powered

                  - D5 Remote Wakeup

                  - D4..0 Reserved, set to 0.
                */
                bmAttributes,
                /*
                  Maximum Power Consumption in 2mA units 
                */
                bMaxPower,
                /*
                */
                interface
           };
        }

   // An interface association allows the device to group a set of interfaces to
   // represent one logical device to be managed by one host driver.
   constexpr usb_iface_assoc_descriptor 
      ifaceassoc_desc(uint8_t bFirstInterface,
                      uint8_t bInterfaceCount,
                      uint8_t bFunctionClass,
                      uint8_t bFunctionSubClass,
                      uint8_t bFunctionProtocol,
                      uint8_t iFunction)
        {
           return {
                // The size of an interface association descriptor: 8
                USB_DT_INTERFACE_ASSOCIATION_SIZE,
                // A value of 11 indicates that this descriptor describes an interface
                // association.
                USB_DT_INTERFACE_ASSOCIATION,
                // The first interface that is part of this group.
                bFirstInterface,
                // The number of included interfaces. This implies that the bundled
                // interfaces must be continugous.
                bInterfaceCount,
                // The class, subclass, and protocol of device represented by this
                // association. In this case a communication device.
                bFunctionClass,
                // Using Abstract Control Model
                bFunctionSubClass,
                // With AT protocol (or Hayes compatible).
                bFunctionProtocol,
                // A string representing this interface. Zero means not provided.
                iFunction
           };
        }
}

/*
Note1:
This should hold the total size of the configuration descriptor 
including all sub interfaces. This is automatically filled in by 
the usb stack in libopencm3.
.wTotalLength = 0,

 */
#endif
