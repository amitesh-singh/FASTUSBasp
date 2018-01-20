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

#include "pin.h"

uint8_t pin::_getNativePinMode(PinMode pm)
{
  if (pm == PinMode::INPUT)
  {
    return GPIO_MODE_INPUT;
  }
  else if (pm == PinMode::OUTPUT_10MHZ)
  {
    return GPIO_MODE_OUTPUT_10_MHZ;
  }
  else if (pm == PinMode::OUTPUT_2MHZ)
  {
    return GPIO_MODE_OUTPUT_2_MHZ;
  }
  else if (pm == PinMode::OUTPUT_50MHZ)
  {
    return GPIO_MODE_OUTPUT_50_MHZ;
  }

  return GPIO_MODE_INPUT;
}

uint8_t pin::_getNativePinConfig(PinConfig pc)
{
  if (pc == PinConfig::INPUT_ANALOG)
  {
    return GPIO_CNF_INPUT_ANALOG;
  }
  else if (pc == PinConfig::INPUT_FLOAT)
  {
    return GPIO_CNF_INPUT_FLOAT;
  }
  else if (pc == PinConfig::INPUT_PULLUPDOWN)
  {
    return GPIO_CNF_INPUT_PULL_UPDOWN;
  }
  else if (pc == PinConfig::OUTPUT_PUSHPULL)
  {
    return GPIO_CNF_OUTPUT_PUSHPULL;
  }
  else if (pc == PinConfig::OUTPUT_ALTFPUSHPULL)
  {
    return GPIO_CNF_OUTPUT_ALTFN_PUSHPULL;
  }
  else if (pc == PinConfig::OUTPUT_ALTFOPENDRAIN)
  {
    return GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN;
  }

  return GPIO_CNF_INPUT_ANALOG;
}

pin::pin(volatile uint32_t pinBaseAddr, uint32_t port): _pinBaseAddr(pinBaseAddr), _port(port) {}

void pin::assign(volatile uint32_t pinBaseAddr, uint32_t port)
{
  _pinBaseAddr = pinBaseAddr;
  _port = port;
}

void pin::setMode(PinMode pm, PinConfig pc)
{
  gpio_set_mode(_pinBaseAddr, _getNativePinMode(pm), _getNativePinConfig(pc), _port);
}

void pin::on()
{
  gpio_set(_pinBaseAddr, _port);
}

void pin::off()
{
  gpio_clear(_pinBaseAddr, _port);
}

void pin::toggle()
{
  gpio_toggle(_pinBaseAddr, _port);
}

bool pin::get()
{
  return gpio_get(_pinBaseAddr, _port);
}