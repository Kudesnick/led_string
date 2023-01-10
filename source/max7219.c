/**
 * @file max7219.c
 * @author Stulov Tikhon (aka Kudesnick) (kudesnick@inbox.ru)
 * @brief Интерфейс драйвера max7219
 * @details Функционал драйвера ограничен работой с матрицей 8 строк на 32 столбца, что еквивалентно
 * четырем последовательно соединенным микросхемам max7219. Управляющие команды применяются ко всем
 * микросхемам одновременно, запись данных производится построчно.
 * @version 0.1
 * @date 07.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "max7219.h"
#include "csp.h"

void max7219_send_cmd(const max7219_cmd_t cmd, const uint8_t data)
{
    const uint8_t send[] = {cmd, data};

    csp_spi_nss_active();

    for (uint8_t i = 0; i < MATRX_CNT; i++)
    {
        csp_spi_send(send, sizeof(send));
    }

    csp_spi_nss_inactive();
}

void max7219_send_data(const uint8_t str, const uint32_t data)
{
    csp_spi_nss_active();

    for (int8_t i = MATRX_CNT - 1; i >= 0; i--)
    {
        const uint8_t send[] = {str, ((uint8_t *)&data)[i]};

        csp_spi_send(send, sizeof(send));
    }

    csp_spi_nss_inactive();
}
