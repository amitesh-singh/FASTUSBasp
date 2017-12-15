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
#include <libopencm3/stm32/flash.h>
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <libopencm3/usb/usbd.h>

#ifdef DEBUG
#include "usart1.h"
#endif

#include "sleep.h"
#include "usbdevice.h"

void rcc_clock_setup_in_hse_8mhz_out_48mhz(void)
{
   /* Enable internal high-speed oscillator. */
   rcc_osc_on(RCC_HSI);
   rcc_wait_for_osc_ready(RCC_HSI);

   /* Select HSI as SYSCLK source. */
   rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSICLK);

   /* Enable external high-speed oscillator 8MHz. */
   rcc_osc_on(RCC_HSE);
   rcc_wait_for_osc_ready(RCC_HSE);
   rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSECLK);

   /*
    * Set prescalers for AHB, ADC, ABP1, ABP2.
    */
   rcc_set_hpre(RCC_CFGR_HPRE_SYSCLK_NODIV);    /* Set. 48MHz Max. 72MHz */
   rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV4);  /* Set. 12MHz Max. 14MHz */
   rcc_set_ppre1(RCC_CFGR_PPRE1_HCLK_DIV2);    /* Set. 24MHz Max. 36MHz */
   rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_NODIV);    /* Set. 48MHz Max. 72MHz */
   rcc_set_usbpre(RCC_CFGR_USBPRE_PLL_CLK_NODIV); /* Set 48Mhz, Max: 48Mhz */

   /*
    * Sysclk runs with 24MHz -> 0 waitstates.
    * 0WS from 0-24MHz
    * 1WS from 24-48MHz
    * 2WS from 48-72MHz
    */
   flash_set_ws(FLASH_ACR_LATENCY_1WS);

   /*
    * Set the PLL multiplication factor to 3.
    * 8MHz (external) * 6 (multiplier) = 48MHz
    */
   rcc_set_pll_multiplication_factor(RCC_CFGR_PLLMUL_PLL_CLK_MUL6);

   /* Select HSE as PLL source. */
   rcc_set_pll_source(RCC_CFGR_PLLSRC_HSE_CLK);

   /*
	 * External frequency undivided before entering PLL
	 * (only valid/needed for HSE).
	 */
   rcc_set_pllxtpre(RCC_CFGR_PLLXTPRE_HSE_CLK);

   /* Enable PLL oscillator and wait for it to stabilize. */
   rcc_osc_on(RCC_PLL);
   rcc_wait_for_osc_ready(RCC_PLL);

   /* Select PLL as SYSCLK source. */
   rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_PLLCLK);

   /* Set the peripheral clock frequencies used */
   rcc_ahb_frequency = 48000000;
   rcc_apb1_frequency = 24000000;
   rcc_apb2_frequency = 48000000;
}

int
main()
{
   //set STM32 to 48 MHz using ext. clock (HSE)
   rcc_clock_setup_in_hse_8mhz_out_48mhz();


   // set up all the clocks here
   rcc_periph_clock_enable(ISP_PORT_RCC);
   rcc_periph_clock_enable(ISP_RST_RCC);
   rcc_periph_clock_enable(INBUILT_LED_RCC);
   rcc_periph_clock_enable(ISP_BUS_RCC);

   inbuilt_led_init();

#ifdef DEBUG
   usart1_setup();
#endif

   clock_setup();

   //This is required if proper pullup is not present at D+ line.
   // This is must for chinese stm32f103c8t6 aka "blue pill"
   // set USBDPLUS_WRONG_PULLUP to 1 if blue pill has wrong pullup at D+ line
   // This code is disabled by default.
#if USBDPLUS_WRONG_PULLUP == 1
   gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
                 GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
   gpio_clear(GPIOA, GPIO12);
   msleep(5);
#endif

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
