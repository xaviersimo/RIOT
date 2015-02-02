/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf231
 * @{
 *
 * @file
 * @brief       Register access functions for the AT86RF231 device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include "at86rf231_spi.h"
#include "at86rf231.h"
#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG (1)
#include "debug.h"


void at86rf231_reg_write(uint8_t addr, uint8_t value)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(AT86RF231_SPI);
    /* Start the SPI transfer */
    gpio_clear(AT86RF231_CS);
    /* write to register */
    int transfered = spi_transfer_reg(AT86RF231_SPI, AT86RF231_ACCESS_REG | AT86RF231_ACCESS_WRITE | addr, value, 0);

    //DEBUG("WRITE ha retornat transfered: %d\n", transfered);
    /* End the SPI transfer */
    gpio_set(AT86RF231_CS);
    /* Release the bus for other threads. */
    spi_release(AT86RF231_SPI);
}

uint8_t at86rf231_reg_read(uint8_t addr)
{

    /* Acquire exclusive access to the bus. */
    spi_acquire(AT86RF231_SPI);
    /* Start the SPI transfer */
    gpio_clear(AT86RF231_CS);

    /* read from register */
    int ret = spi_transfer_reg(AT86RF231_SPI, AT86RF231_ACCESS_REG | AT86RF231_ACCESS_READ | addr, 0, &value);


    DEBUG("READ ha retornat: %d\n", ret);
    /* End the SPI transfer */
    gpio_set(AT86RF231_CS);
    /* Release the bus for other threads. */
    spi_release(AT86RF231_SPI);
    return (uint8_t)value;
}

void at86rf231_read_fifo(uint8_t *data, radio_packet_length_t length)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(AT86RF231_SPI);
    /* Start the SPI transfer */
    gpio_clear(AT86RF231_CS);
    /* Read a number of bytes from the devices frame buffer */
    spi_transfer_regs(AT86RF231_SPI, AT86RF231_ACCESS_FRAMEBUFFER | AT86RF231_ACCESS_READ,
                      0, (char*)data, length);
    /* End the SPI transfer */
    gpio_set(AT86RF231_CS);
    /* Release the bus for other threads. */
    spi_release(AT86RF231_SPI);
}

void at86rf231_write_fifo(const uint8_t *data, radio_packet_length_t length)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(AT86RF231_SPI);
    /* Start the SPI transfer */
    gpio_clear(AT86RF231_CS);
    /* Send Frame Buffer Write access */
    spi_transfer_regs(AT86RF231_SPI, AT86RF231_ACCESS_FRAMEBUFFER | AT86RF231_ACCESS_WRITE,
                      (char*)data, 0, length);
    /* End the SPI transfer */
    gpio_set(AT86RF231_CS);
    /* Release the bus for other threads. */
    spi_release(AT86RF231_SPI);
}

uint8_t at86rf231_get_status(void)
{
    return (at86rf231_reg_read(AT86RF231_REG__TRX_STATUS)
            & AT86RF231_TRX_STATUS_MASK__TRX_STATUS);
}
