/**
 * @file main.c
 * @author Stulov Tikhon (aka Kudesnick) (kudesnick@inbox.ru)
 * @brief Демонстрация работы с МС max7219
 * @details Анимированный вывод "середечек" на матрицу 8x32 точки
 * @version 0.1
 * @date 07.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "stm32f10x.h"
#include "csp.h"
#include "max7219.h"

#if(1)
/// @brief Побитное зеркалирование 4-байтного слова
/// @example 0x80000010 -> 0x08000001
#define RBIT(dw) __RBIT(dw)
#else
uint32_t RBIT(uint32_t in)
{
    uint32_t result = 0;
    
    for (uint32_t i = 32; i; i--)
    {
        result <<= 1;
        result |= (in & 1);
        in >>= 1;
    }

    return result;
}
#endif

/// Структура строки изображения
typedef union
{
    uint32_t dw;
    uint16_t w[2];
    uint8_t  b[4];
} buf_str_t;

/// Половинка "сердца"
const uint8_t heart[STR_CNT] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x08, 0x88, 0x70};

/// Буфер изображения
buf_str_t img_buf[STR_CNT];

/// Загрузить изображение
static void load_img()
{
    for (uint32_t i = 0; ++i <= STR_CNT;)
    {
        const uint32_t tmp = img_buf[i-1].dw;
        max7219_send_data(i, tmp | RBIT(tmp));
    }
    csp_delay(16);
}

/// @brief Эффект пульсации (изменение яркости)
/// @param cnt Количество пульсаций
static void pulse(const uint32_t cnt)
{
    for (uint32_t i = cnt; i-- > 0;)
    {
        for (uint32_t i = 0; ++i < 0x20;)
        {
            max7219_send_cmd(MAX7219_BRIGHTNESS, i ^ (0xF * (i >> 4)));
            csp_delay(2 << (i >> 4));
        }
        csp_delay(200);
    }
}

/** @defgroup Animation Функции анимации
 *  @{ ********************************************************************************************/

/// Сигнатура функции преобразования строки изображения
typedef void(*step_t)(const uint32_t, const uint32_t);

/// @brief Применение функции преобразования к изображению
/// @param j Количество кадров анимации
/// @param fn_p Функция преобразования строки изображения
void step(int8_t j, step_t fn_p)
{
    for (; j >= 0; j--)
    {
        for (int32_t i = STR_CNT; --i >= 0;)
        {
            fn_p(i, j);
        }

        load_img();
    }
}

/// @brief Функция преобразования "одно сердце"
/// @param i номер строки
/// @param j номер кадра
void one_heart(const uint32_t i, const uint32_t j)
{
    img_buf[i].dw = (uint16_t)heart[i] << 8 >> j;
}

/// @brief Функция преобразования "два сердца"
/// @param i номер строки
/// @param j номер кадра
void dbl_heart(const uint32_t i, const uint32_t j)
{
    img_buf[i].dw = ((uint32_t)heart[i] >> j) | (uint32_t)heart[i] << (16 - j);
}

/// @brief Функция преобразования "слияние сердец"
/// @param i номер строки
/// @param j номер кадра
void uni_heart(const uint32_t i, const uint32_t j)
{
    (void)j;

    img_buf[i].w[0] <<= 1;
    img_buf[i].w[1] >>= 1;
}

/** @} ********************************************************************************************/

/// Инициализация
__STATIC_FORCEINLINE void init()
{
    csp_spi_init();

    csp_spi_nss_inactive();

    // Base initialisation of MAX7219
    max7219_send_cmd(MAX7219_TEST, 0x00); // 1 - on, 0 - off
    max7219_send_cmd(MAX7219_SCAN_LIMIT, STR_CNT - 1);
    max7219_send_cmd(MAX7219_BRIGHTNESS, 0x00); // 0x00..0x0F
    max7219_send_cmd(MAX7219_DECODE_MODE, 0x00); // 0 - raw data
    max7219_send_cmd(MAX7219_SHUTDOWN, 0x01); // 1 - active mode, 0 - inactive mode
}

/// @brief Основная функция
/// @return Нафиг не нужен, но против стандарта не попрешь
int main()
{
    init();

    step(15, one_heart);
    pulse(1);

    step(7, dbl_heart);
    pulse(2);

    step(7, uni_heart);

    // Заливка контурного "сердца"
    for (uint32_t j = 0; ++j < 7;)
    {
        img_buf[j].dw |= img_buf[j - 1].dw;

        load_img();
    }

    // Бесконечно
    for (;; pulse(1));

    return 0;
}
