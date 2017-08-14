#ifndef _TPI_H
#define _TPI_H

#include "stdint.h"

void tpi_clk();

void tpi_init();
void tpi_break();
void tpi_send(uint8_t );
uint8_t tpi_recv();

uint8_t tpi_read_data();
void tpi_write_data(uint8_t b);
void tpi_write_block(uint16_t addr, const uint8_t *sptr, uint8_t len);
void tpi_read_block(uint16_t addr, uint8_t *dptr, uint8_t len);
#endif