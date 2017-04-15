#include <libopencm3/cm3/common.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "usart1.h"

void usart1_setup(void)
{
   rcc_periph_clock_enable(RCC_GPIOA);
   rcc_periph_clock_enable(RCC_USART1);

   // GPIO_USART1_TX - GPIO9
   gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
                 GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
   // Also: GPIO_USART1_RX - > GPIO10
   gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                 GPIO_USART1_RX);

   /* Setup UART parameters. */
   usart_set_baudrate(USART1, 9600);
   usart_set_databits(USART1, 8);
   usart_set_stopbits(USART1, USART_STOPBITS_1);
   usart_set_mode(USART1, USART_MODE_TX_RX); //USART_MODE_TX);
   usart_set_parity(USART1, USART_PARITY_NONE);
   usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

   /* Finally enable the USART. */
   usart_enable(USART1);
}

void usart1_print(const char *s)
{
  while (*s != 0)
     {
        usart_send_blocking(USART1, *s);
        s++;
     } 
}

void usart1_println(const char *s)
{
  usart1_print(s);
  usart_send_blocking(USART1, '\r');
  usart_send_blocking(USART1, '\n');
}