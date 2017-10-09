#ifndef _CONFIG_H
#define _CONFIG_H

#define ISP_PORT GPIOA
#define ISP_RCC RCC_GPIOA

#define ISP_MOSI GPIO7
#define ISP_MISO GPIO6
#define ISP_SCK  GPIO5
#define ISP_RST GPIO8

//define this to 1, if blue pill has wrong pull up at USB D+ line
#define USBDPLUS_WRONG_PULLUP 0


#endif
