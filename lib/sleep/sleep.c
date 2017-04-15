#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include "sleep.h"

static volatile uint32_t system_ms = 0;

__attribute__ ((used)) void sys_tick_handler(void)
{
  ++system_ms;
}

void clock_setup(void)
{
  //72/8 = 9 MHz
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
  // 9e6/9000 (N) = 1000 = 1 ms; N - 1 = is the argument 
  systick_set_reload(8999);
  systick_interrupt_enable();

  // start counting
  systick_counter_enable();
}

//This is a blocking call.
void msleep(uint32_t delay_ms)
{
  uint32_t wakeme = system_ms + delay_ms;
  while (wakeme > system_ms);
}