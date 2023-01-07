/**
 * @file spi.c
 * @headerfile csp.h
 * @author Stulov Tikhon (aka Kudesnick) (kudesnick@inbox.ru)
 * @brief Реализация интерфейса доступа к аппаратным функциям МК
 * @details Использует драйвер SPI из бибилотеки CMSIS
 * @version 0.1
 * @date 07.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 * @see https://www.keil.com/pack/doc/CMSIS/Driver/html/group__spi__interface__gr.html
 * @see https://arm-software.github.io/CMSIS_5/Driver/html/index.html
 */

#include "RTE_Components.h"
#include CMSIS_device_header

#include "Driver_SPI.h"

/// SPI Driver external
extern ARM_DRIVER_SPI Driver_SPI1;

/// SPI Driver pointer
ARM_DRIVER_SPI *SPIdrv = &Driver_SPI1;

void csp_delay(const uint8_t del)
{
    for (volatile uint32_t i = 0xFFF * del; i != 0; i--);
}

void csp_spi_init()
{
    /* Initialize the SPI driver */
    SPIdrv->Initialize(NULL);
    /* Power up the SPI peripheral */
    SPIdrv->PowerControl(ARM_POWER_FULL);
    /* Configure the SPI to Master */
    SPIdrv->Control(0
                    | ARM_SPI_MODE_MASTER
                    | ARM_SPI_CPOL0_CPHA0
                    | ARM_SPI_MSB_LSB
                    | ARM_SPI_SS_MASTER_SW
                    | ARM_SPI_DATA_BITS(8),
                    1000000);

    SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
}

void csp_spi_nss_active()
{
    SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
}

void csp_spi_nss_inactive()
{
    SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
}

void csp_spi_send(const uint8_t *data, const uint8_t len)
{
    SPIdrv->Send(data, len);
    while (SPIdrv->GetStatus().busy);
}
