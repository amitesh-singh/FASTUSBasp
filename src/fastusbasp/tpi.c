#include "tpi.h"
#include "config.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include "sleep.h"

#define TPI_DATA ISP_MOSI
#define TPI_CLK ISP_SCK
#define TPI_RST ISP_RST 

void tpi_clk()
{
  msleep(1); //should be 10 uS
  //toggle CLK pin
  gpio_toggle(GPIOA, TPI_CLK);
}

void tpi_init()
{
  rcc_periph_clock_enable(RCC_GPIOA);
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ,
                            GPIO_CNF_OUTPUT_PUSHPULL,
                            TPI_RST | TPI_CLK | TPI_DATA);
  
  msleep(128);
   
  for (uint8_t i  = 0; i < 33; ++i)
  {
    tpi_clk();
  } 
}

void tpi_break()
{
  gpio_clear(GPIOA, TPI_DATA);
  for (uint8_t i  = 0; i < 24; ++i)
  {
    tpi_clk();
  } 

  gpio_set(GPIOA, TPI_DATA);
}

void tpi_send(uint8_t b)
{
  gpio_clear(GPIOA, TPI_DATA);

  tpi_clk();
  tpi_clk();

  uint8_t parity = 0;

  for(uint8_t i = 0; i < 8; ++i) 
  {
		if(b & 1)
    {
			//TPI_PORT |= (1 << TPI_DATA);
			gpio_set(GPIOA, TPI_DATA);
      parity ^= 1;
		}
    else
    {
			//TPI_PORT &= ~(1 << TPI_DATA);
		  gpio_clear(GPIOA, TPI_DATA);
    }
		b >>= 1;

		tpi_clk();
    tpi_clk(); 
 } 

if (parity)
{
  gpio_set(GPIOA, TPI_DATA);
}
else 
{
  gpio_clear(GPIOA, TPI_DATA);
}
tpi_clk();
tpi_clk(); 

gpio_set(GPIOA, TPI_DATA);
tpi_clk(); tpi_clk();
tpi_clk(); tpi_clk();
}

uint8_t tpi_recv()
{
  //TPI_DDR &= ~(1 << TPI_DATA);
	//gpio_clear(GPIOA, TPI_DATA);
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                            GPIO_CNF_INPUT_FLOAT, 
                            TPI_DATA);
  
  tpi_clk();

	//while(TPI_PIN & (1 << TPI_DATA)) {
	while (gpio_read(GPIOA, TPI_DATA))
  {	
    tpi_clk(); tpi_clk();
	}

	uint8_t ret = 0;
	uint8_t parity = 0;

	for(uint8_t i = 0; i < 8; ++i)
  {
		tpi_clk();
    tpi_clk();
		ret >>= 1;
		//if(TPI_PIN & (1 << TPI_DATA)) {
			while (gpio_read(GPIOA, TPI_DATA))
      {
    	ret |= 0x80;
			parity ^= 1;
		}
	}

	tpi_clk(); tpi_clk();

	if(!!gpio_read(GPIOA, TPI_DATA)) != parity) {
		//printf_P(PSTR("parity error\n"));
	}

	tpi_clk(); tpi_clk();
	if(!(gpio_read(GPIOA, TPI_DATA))) {
		//printf_P(PSTR("stop bit error\n"));
	}

	tpi_clk(); tpi_clk();
	if(!(gpio_read(GPIOA, TPI_DATA))) {
		//printf_P(PSTR("stop bit error\n"));
	}

	tpi_clk();
	tpi_clk(); tpi_clk();
//	TPI_DDR |= 1 << TPI_DATA;

gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ,
                            GPIO_CNF_OUTPUT_PUSHPULL, TPI_DATA);

return ret;
  
}

void tpi_write_css(uint8_t addr, uint8_t value)
 {
	tpi_send(0xC0 | addr);
	tpi_send(value);
}

uint8_t tpi_read_css(uint8_t addr)
{
	tpi_send(0x80 | addr);
	return tpi_receive();
}

void tpi_write_io(uint8_t addr, uint8_t value)
{
	tpi_send(0x90 | (addr & 0x0F) | ((addr & 0x30) << 1));
	tpi_send(value);
}

uint8_t tpi_read_io(uint8_t addr)
{
	tpi_send(0x10 | (addr & 0x0F) | ((addr & 0x30) << 1));
	return tpi_receive();
}

void tpi_set_pointer(uint16_t addr)
{
	tpi_send(0x68);
	tpi_send(addr & 0xFF);
	tpi_send(0x69);
	tpi_send(addr >> 8);
}

uint8_t tpi_read_data(void)
{
	tpi_send(0x24);
	return tpi_receive();
}

void tpi_write_data(uint8_t value)
{
	tpi_send(0x64);
	tpi_send(value);
}

void tpi_write_block(uint16_t addr, const uint8_t *sptr, uint8_t len)
{
   tpi_set_pointer(addr);
  

}

void tpi_read_block(uint16_t addr, uint8_t *dptr, uint8_t len)
{
  tpi_set_pointer(addr);

  for (uint8_t i = 0; i < len; ++i)
  {
    dptr[i] = tpi_read_data();
  }

}