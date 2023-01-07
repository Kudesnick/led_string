/**
 * @file startup.c
 * @author Stulov Tikhon (aka Kudesnick) (kudesnick@inbox.ru)
 * @brief Таблица векторов прерываний для МК семейства stm32
 * @details Таблица векторов прерывания расположена в начале FLASH памяти по адресу 0x08000000.
 * первая 4-байтная ячейка - указатель на вершину стека, следующая - вектор сброса. Далее идут
 * вектора прерываний. Данная таблица ограничена указателем стека и вектором сброса. Используется
 * в проектах не подразумевающих использование прерываний.
 * @version 0.1
 * @date 07.01.2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdint.h>

/// Размер стека. Должен быть кратен 8 байтам.
#define STACK_SIZE (0x68)

/// @brief Структура таблицы векторов
typedef volatile const struct
{
    const void *const sp; ///< Указатель на вершину стека
    int (*main)(void); ///< Вектор сброса
} vectors_t;

/// Стек. Должен быть кратен 8 байтам.
volatile uint64_t stack[STACK_SIZE / sizeof(uint64_t)];

/// @brief Указатель на вершину стека.
/// @details Стек растет сверху вниз.
const void *const __initial_sp = (void *)((uint32_t)stack + sizeof(stack));

/// @brief Главная функция
extern int main();

/// @brief Таблица векторов
vectors_t vectors __attribute__((section("reset"))) =
{
    .sp = __initial_sp,
    .main = main,
};
