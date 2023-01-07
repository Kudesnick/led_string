/**
 * @file csp.c
 * @headerfile csp.h
 * @author Stulov Tikhon (aka Kudesnick) (kudesnick@inbox.ru)
 * @brief Реализация интерфейса доступа к аппаратным функциям МК
 * @details Реализован посредством прямой записи в регистры. Поддерживает семейство МК stm32f10x
 * @version 0.1
 * @date 07.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 * @warning Особенности реализации модуля предполагают, что все задействованные пины
 * принадлежат одному порту и имеют номера от 0 до 7.
 */

#include "stm32f10x.h"

#define NSS_PORT  GPIOA ///< Адрес порта SS (CS)
#define NSS_PIN   (6)   ///< Номер пина SS (CS)
#define SCK_PORT  GPIOA ///< Адрес порта SCK (CLK)
#define SCK_PIN   (5)   ///< Номер пина SCK (CLK)
#define MOSI_PORT GPIOA ///< Адрес порта MOSI (DIN)
#define MOSI_PIN  (7)   ///< Номер пина MOSI (DIN)

/// Port Mode
typedef enum
{
    GPIO_MODE_INPUT     = 0x00, ///< GPIO is input
    GPIO_MODE_OUT10MHZ  = 0x01, ///< Max output Speed 10MHz
    GPIO_MODE_OUT2MHZ   = 0x02, ///< Max output Speed  2MHz
    GPIO_MODE_OUT50MHZ  = 0x03  ///< Max output Speed 50MHz
} GPIO_MODE;

/// Port Conf
typedef enum
{
    GPIO_OUT_PUSH_PULL  = 0x00, ///< general purpose output push-pull
    GPIO_OUT_OPENDRAIN  = 0x01, ///< general purpose output open-drain
    GPIO_AF_PUSHPULL    = 0x02, ///< alternate function push-pull
    GPIO_AF_OPENDRAIN   = 0x03, ///< alternate function open-drain
    GPIO_IN_ANALOG      = 0x00, ///< input analog
    GPIO_IN_FLOATING    = 0x01, ///< input floating
    GPIO_IN_PULL_DOWN   = 0x02, ///< alternate function push-pull
    GPIO_IN_PULL_UP     = 0x03  ///< alternate function pull up
} GPIO_CONF;

#define CONF_CLR(pin) (0xF << ((pin) << 2))
#define CONF_SET(pin, conf) ((((conf) << 2) | GPIO_MODE_OUT50MHZ) << ((pin) << 2))

#define BITBANDING_ADDR_CALC(ADDR, BYTE)       \
    ((uint32_t)(ADDR) & 0xF0000000UL) +        \
    (((uint32_t)(ADDR) & 0x00FFFFFFUL) << 5) + \
    0x02000000UL + ((BYTE) << 2)
#define BB_REG(ADDR, BYTE) (*(volatile uint32_t *)(BITBANDING_ADDR_CALC(ADDR, BYTE)))

void csp_delay(const uint8_t del)
{
    for (volatile uint32_t i = 0xFFF * del; i != 0; i--);
}

void csp_spi_init()
{
    // GPIO init
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN;
    GPIOA->CRL &= ~(CONF_CLR(NSS_PIN) | CONF_CLR(SCK_PIN) | CONF_CLR(MOSI_PIN));
    GPIOA->CRL |= CONF_SET(NSS_PIN, GPIO_OUT_PUSH_PULL) | CONF_SET(SCK_PIN, GPIO_AF_PUSHPULL) | CONF_SET(MOSI_PIN, GPIO_AF_PUSHPULL);

    // SPI1 init MODE_MASTER, CPOL0, CPHA0, MSB_LSB, DATA_8_BITS, Max speed
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_SPE;
}

void csp_spi_nss_active()
{
    NSS_PORT->BRR = (1UL << NSS_PIN);
}

void csp_spi_nss_inactive()
{
    while (BB_REG(&(SPI1->SR), 7)); // SPI_SR_BSY
    NSS_PORT->BSRR = (1UL << NSS_PIN);
}

void csp_spi_send(const uint8_t *data, const uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        while (!BB_REG(&(SPI1->SR), 1)); // SPI_SR_TXE
        SPI1->DR = data[i];
    }
}
