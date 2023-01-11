/**
 * @file max7219.h
 * @author Stulov Tikhon (aka Kudesnick) (kudesnick@inbox.ru)
 * @brief Интерфейс драйвера max7219
 * @version 0.1
 * @date 07.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 * @see https://www.analog.com/en/products/max7219.html
 */

#pragma once

#include <stdint.h>

#define MATRX_CNT (4) ///< Количество матриц в линейке

#define STR_CNT (8) ///< Количество строк в матрице

/// Управляющие команды MAX7219 SPI LED Driver
typedef enum : uint32_t
{
    MAX7219_DECODE_MODE = 0x09, ///< Режим декодирования
    ///< (позволяет подключать знакосинтезирующие дисплеи)
    MAX7219_BRIGHTNESS  = 0x0a, ///< Уровень яркости
    MAX7219_SCAN_LIMIT  = 0x0b, ///< Количество строк подключенных к матрице
    MAX7219_SHUTDOWN    = 0x0c, ///< Режим работы (вкл./выкл.)
    MAX7219_TEST        = 0x0f, ///< Тест (зажечь все сегменты индикатора)
} max7219_cmd_t;

/// @brief Отправить команду на все матрицы
/// @param cmd код команды
/// @param data аргумент
void max7219_send_cmd(const max7219_cmd_t cmd, const uint32_t data);

/// @brief Записать строку в матрицу
/// @param str Номер строки 1..8
/// @param data Данные (8*4 бит)
void max7219_send_data(const uint32_t str, const uint32_t data);
