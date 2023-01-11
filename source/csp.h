/**
 * @file csp.h
 * @author Stulov Tikhon (aka Kudesnick) (kudesnick@inbox.ru)
 * @brief Интерфейс доступа к аппаратным функциям МК
 * @version 0.1
 * @date 07.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <stdint.h>

/// @brief Синхронная задержка
/// @param del Длительность задержки в условных единицах (примерно 7 мс)
void csp_delay(const uint32_t del);

/// @brief Инициализация SPI модуля
/// @details MODE_MASTER, CPOL0, CPHA0, MSB_LSB, DATA_8_BITS, SS software control
void csp_spi_init();

/// @brief активация линии SS (низкий уровень)
void csp_spi_nss_active();

/// @brief деактивация линии SS (высокий уровень)
void csp_spi_nss_inactive();

/// @brief Отправка цепочки данных по SPI
/// @param data Данные
void csp_spi_send(const uint32_t data);
