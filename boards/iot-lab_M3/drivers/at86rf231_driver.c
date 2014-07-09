/**
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; version 2.1 of the License, or (at your option) any
 * later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * @file   at86rf231_driver.c
 * @author Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#include <stdio.h>
#include <stddef.h>

#include "cpu.h"
#include "sched.h"
#include "vtimer.h"

#include "arch/thread_arch.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "spi.h"
#include "board.h"

#include "at86rf231.h"
#include "at86rf231_spi.h"
#include "at86rf231_spi1.h"

/*
SPI1
  SCLK : PA5
  MISO : PA6
  MOSI : PA7
  CS : PA4

GPIO
  IRQ0 : PC4 : Frame buff empty indicator
  DIG2 : ? : RX Frame Time stamping XXX : NOT USED
  Reset : PC1 : active low, enable chip
  SLEEP : PA2 : control sleep, tx & rx state
*/

inline static void RESET_CLR(void)
{
    SPI_0_RESET_PORT->BRR = (1 << SPI_0_RESET_PIN);
}
inline static void RESET_SET(void)
{
    SPI_0_RESET_PORT->BSRR = (1 << SPI_0_RESET_PIN);
}
inline static void CSn_SET(void)
{
    SPI_0_CS_PORT->BSRR = (1 << SPI_0_CS_PIN);
}
inline static void CSn_CLR(void)
{
    SPI_0_CS_PORT->BRR = (1 << SPI_0_CS_PIN);
}
inline static void SLEEP_CLR(void)
{
    SPI_0_SLEEP_PORT->BRR = (1 << SPI_0_SLEEP_PIN);
}

uint8_t at86rf231_get_status(void)
{
    return at86rf231_reg_read(AT86RF231_REG__TRX_STATUS)
           & AT86RF231_TRX_STATUS_MASK__TRX_STATUS;
}


extern void at86rf231_rx_irq(void);
static
void enable_exti_interrupt(void)
{
    gpio_irq_enable(SPI_0_IRQ0_GPIO);
}

static
void disable_exti_interrupt(void)
{
    gpio_irq_disable(SPI_0_IRQ0_GPIO);
}

void at86rf231_gpio_spi_interrupts_init(void)
{
    /* set up GPIO pins */
    /* SCLK and MOSI*/
    GPIOA->CRL &= ~(0xf << (5 * 4));
    GPIOA->CRL |= (0xb << (5 * 4));
    GPIOA->CRL &= ~(0xf << (7 * 4));
    GPIOA->CRL |= (0xb << (7 * 4));
    /* MISO */
    gpio_init_in(SPI_0_MISO_GPIO, GPIO_NOPULL);

    /* SPI1 init */
    at86rf231_spi1_init();

    spi_poweron(SPI_0);

    /* IRQ0 */
    gpio_init_in(SPI_0_IRQ0_GPIO, GPIO_NOPULL);
    gpio_init_int(SPI_0_IRQ0_GPIO, GPIO_NOPULL, GPIO_RISING, at86rf231_rx_irq);

    /* Connect EXTI4 Line to PC4 pin */
    enable_exti_interrupt();

    /* CS */
    gpio_init_out(SPI_0_CS_GPIO, GPIO_NOPULL);
    /* SLEEP */
    gpio_init_out(SPI_0_SLEEP_GPIO, GPIO_NOPULL);
    /* RESET */
    gpio_init_out(SPI_0_RESET_GPIO, GPIO_NOPULL);

}

void at86rf231_reset(void)
{
    /* force reset */
    RESET_CLR();
    CSn_SET();
    SLEEP_CLR();

    vtimer_usleep(AT86RF231_TIMING__RESET);

    RESET_SET();

    /* Wait until TRX_OFF is entered */
    vtimer_usleep(AT86RF231_TIMING__RESET_TO_TRX_OFF);

    /* Send a FORCE TRX OFF command */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__FORCE_TRX_OFF);

    /* Wait until TRX_OFF state is entered from P_ON */
    vtimer_usleep(AT86RF231_TIMING__SLEEP_TO_TRX_OFF);

    /* busy wait for TRX_OFF state */
    uint8_t status;
    uint8_t max_wait = 100;   // TODO : move elsewhere, this is in 10us

    do {
        status = at86rf231_get_status();

        vtimer_usleep(10);

        if (!--max_wait) {
            printf("at86rf231 : ERROR : could not enter TRX_OFF mode\n");
            break;
        }
    }
    while ((status & AT86RF231_TRX_STATUS_MASK__TRX_STATUS) != AT86RF231_TRX_STATUS__TRX_OFF);
}

void at86rf231_spi_select(void)
{
    CSn_CLR();
}
void at86rf231_spi_unselect(void)
{
    CSn_SET();
}

void at86rf231_enable_interrupts(void)
{
    enable_exti_interrupt();
}
void at86rf231_disable_interrupts(void)
{
    disable_exti_interrupt();
}

__attribute__((naked))
void isr_exti4(void)
{
    ISR_ENTER();
    if (EXTI->IMR & (1<<GPIO_11_EXTI_LINE) && EXTI->PR & (1<<GPIO_11_EXTI_LINE)) {
        EXTI->PR = (1<<GPIO_11_EXTI_LINE);

        at86rf231_rx_irq();

        if (sched_context_switch_request) {
            /* scheduler */
            thread_yield();
        }
    }

    ISR_EXIT();
}