#ifndef _CONFIG_H
#define _CONFIG_H

#define INBUILT_LED GPIO13
#define INBUILT_LED_PORT GPIOC
#define INBUILT_LED_RCC RCC_GPIOC

#define ISP_BUS SPI2
#define ISP_BUS_RCC RCC_SPI2

#define ISP_PORT_RCC RCC_GPIOB
#define ISP_PORT GPIOB
#define ISP_MOSI GPIO15
#define ISP_MISO GPIO14
#define ISP_SCK  GPIO13

#define ISP_RST_RCC RCC_GPIOA
#define ISP_RST_PORT GPIOA
#define ISP_RST GPIO8

//define this to 1, if blue pill has wrong pull up at USB D+ line
#define USBDPLUS_WRONG_PULLUP 0

#endif
