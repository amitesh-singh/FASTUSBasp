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

//#define DEBUG

#include <libopencm3/cm3/common.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include <stdio.h>
#include <stdlib.h>
#include <libopencm3/usb/usbd.h>

#ifdef DEBUG
#include "usart1.h"
#endif

#include "sleep.h"

#include "usbdevice.h"

int
main()
{
   //set STM32 to 48 MHz
   rcc_clock_setup_in_hsi_out_48mhz();
   inbuilt_led_init();

#ifdef DEBUG
   usart1_setup();
#endif

   clock_setup();

   //This is required if proper pullup is not present at D+ line.
   // This is must for chinese stm32f103c8t6 aka "blue pill"
   rcc_periph_clock_enable(RCC_GPIOA);
   gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
                 GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
   gpio_clear(GPIOA, GPIO12);
   msleep(5);

   // USB initialization
   usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev_descr, &config,
                        usb_strings, 2, usbd_control_buffer,
                        sizeof(usbd_control_buffer));
   usbd_register_set_config_callback(usbd_dev, set_config);

#ifdef DEBUG
   usart1_println("usb init");
#endif

   while(1)
     {
        usbd_poll(usbd_dev);
     }
}
