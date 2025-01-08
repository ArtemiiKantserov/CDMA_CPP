#ifndef _t33nsy_ENCODE
#define _t33nsy_ENCODE

#include <cstring>
#include <string>

/**
 * @file encode.hpp
 * @brief функция кодирования строки
 * @param space массив для хранения символов
 * @param str строка для кодирования
 * @param length длина строки
 * @param alphabet алфавит для кодирования
 * @param key_size размер ключа
 * @return кодированная строка
 */
auto encode(char *space, const char *str, int length, char *alphabet,
            int key_size) -> void;

/**
 * @brief функция генерации кодированной буквы
 * @param space массив для хранения символов
 * @param val значение для кодирования (буква)
 * @param key уникальный код
 * @param neg_key уникальный код, умноженный на -1
 * @param key_size размер ключа
 * @return кодированная буква
 */
auto generate_coded_char(char *space, char val, char *key, char *neg_key,
                         int key_size) -> void;

/**
 * @brief функция генерации всех кодированных букв (алфавита для пользователя)
 * @param space массив для хранения символов
 * @param key уникальный код
 * @param neg_key уникальный код, умноженный на -1
 * @param key_size размер ключа
 * @return алфавит всех букв, закодированный для пользоавтеля
 */
auto generate_all_chars(char *space, char *key, char *neg_key,
                        int key_size) -> void;

/**
 * @brief функция декодирования строки
 * @param coded строка для декодирования
 * @param length длина строки
 * @param key уникальный код
 * @param key_size размер ключа
 * @return декодированная строка
 */
auto decode(char *coded, int length, char *key, int key_size) -> char *;

/**
 * @brief функция поиска разницы между двумя строками
 * @param a первая строка
 * @param b вторая строка
 * @param packet_size размер пакета
 * @return разница между двумя строками
 */
auto find_diff(char *a, char *b, int packet_size) -> int;

/**
 * @brief функция умножения матрицы на вектор
 * @param a матрица
 * @param key вектор
 * @param key_size размер ключа
 * @return результат умножения
 */
auto matrix_multiply(char *a, char *key, int key_size) -> char;

#endif /* _t33nsy_ENCODE */
