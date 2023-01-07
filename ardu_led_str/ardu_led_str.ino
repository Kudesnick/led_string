/**
 * @file ardu_led_str.ino
 * @author Stulov Tikhon (aka Kudesnick) (kudesnick@inbox.ru)
 * @brief Демонстрация работы с МС max7219
 * @details Анимированный вывод "середечек" на матрицу 8x32 точки
 * @version 0.1
 * @date 07.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 * @see https://majicdesigns.github.io/MD_MAX72XX/
 */

#include <MD_MAX72xx.h>

/// Number of devices
#define MAX_DEVICES 4

#define CLK_PIN     13  ///< or SCK
#define DATA_PIN    11  ///< or MOSI
#define CS_PIN      10  ///< or SS

/// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(CS_PIN, MAX_DEVICES);

/// Структура строки изображения
typedef union
{
    uint32_t dw;
    uint16_t w[2];
    uint8_t  b[4];
} buf_str_t;

/** @defgroup Utility Вспомогательные функции
 *  @{ ********************************************************************************************/

uint32_t RBIT(const uint32_t in)
{
    uint32_t result = 0;

    for (uint8_t i = 0; i < 32; i++)
    {
        if (in & ((uint32_t)1 << i))
        {
            result |= (uint32_t)0x80000000 >> i;
        }
    }

    return result;
}

void csp_delay(const uint8_t del)
{
    delay(del * 7);
}

/** @} ********************************************************************************************/

#define STR_CNT (8)

/// Половинка "сердца"
const uint8_t heart[STR_CNT] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x08, 0x88, 0x70};

/// Буфер изображения
buf_str_t img_buf[STR_CNT];

/// Загрузить изображение
void load_img()
{
    for (uint8_t i = 0; i < STR_CNT; i++)
    {
        mx.setRow(3, 3, i, img_buf[i].b[0]);
        mx.setRow(2, 2, i, img_buf[i].b[1]);
        mx.setRow(1, 1, i, img_buf[i].b[2]);
        mx.setRow(0, 0, i, img_buf[i].b[3]);
    }
    csp_delay(16);
}

/// @brief Эффект пульсации (изменение яркости)
/// @param cnt Количество пульсаций
void pulse(const uint8_t cnt)
{
    for (uint8_t j = cnt; j > 0; j--)
    {
        int8_t i = 1;

        for (; i < 0x0F; i++)
        {
            mx.control(MD_MAX72XX::INTENSITY, i);
            csp_delay(2);
        }
        for (; i >= 0x00; i--)
        {
            mx.control(MD_MAX72XX::INTENSITY, i);
            csp_delay(4);
        }
        csp_delay(200);
    }
}

/** @defgroup Animation Функции анимации
 *  @{ ********************************************************************************************/

/// Сигнатура функции преобразования строки изображения
typedef void(*step_t)(const uint8_t, const uint8_t);

// Приходится объявлять здесь из за особенностей сборки в Arduino IDE
void step(int8_t j, step_t fn_p);

/// @brief Применение функции преобразования к изображению
/// @param j Количество кадров анимации
/// @param fn_p Функция преобразования строки изображения
void step(int8_t j, step_t fn_p)
{
    for (; j >= 0; j--)
    {
        for (int8_t i = STR_CNT - 1; i >= 0; i--)
        {
            fn_p(i, j);
        }

        load_img();
    }
}

/// @brief Функция преобразования "одно сердце"
/// @param i номер строки
/// @param j номер кадра
void one_heart(const uint8_t i, const uint8_t j)
{
    uint32_t tmp = (uint16_t)heart[i] << 8 >> j;
    img_buf[i].dw = tmp | RBIT(tmp);
}

/// @brief Функция преобразования "два сердца"
/// @param i номер строки
/// @param j номер кадра
void dbl_heart(const uint8_t i, const uint8_t j)
{
    uint32_t tmp = (uint16_t)heart[i] >> j;
    tmp |= (uint32_t)heart[i] << (16 - j);
    img_buf[i].dw = tmp | RBIT(tmp);
}

/// @brief Функция преобразования "слияние сердец"
/// @param i номер строки
/// @param j номер кадра
void uni_heart(const uint8_t i, const uint8_t j)
{
    (void)j;

    img_buf[i].w[0] <<= 1;
    img_buf[i].w[1] >>= 1;
}

/** @} ********************************************************************************************/

/// Инициализация
void setup()
{
    mx.begin();

    mx.control(MD_MAX72XX::INTENSITY, 0);
}

/// Основная функция
void loop()
{
    // One heart animation
    step(15, one_heart);
    pulse(1);

    // Double heart animation
    step(7, dbl_heart);
    pulse(2);

    // Union heart animation
    step(7, uni_heart);

    // Flood hert animation
    for (uint8_t j = 1; j < 7; j++)
    {
        img_buf[j].dw |= img_buf[j - 1].dw;

        load_img();
    }

    // Infinity pulsation of heart
    for (;; pulse(1));
}
