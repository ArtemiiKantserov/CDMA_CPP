#ifndef _t33nsy_HADAMARD
#define _t33nsy_HADAMARD

/**
 * @file hadamard.hpp
 * @brief Генерация матрицы Адамара
 * @param H место для матрицы
 * @param size размер матрицы (степень двойки)
 * @param i номер строки
 * @param j номер столбца
 * @param h константа, на которую умножается матрица
 */
auto generate_hadamard(char **H, int size, int i, int j, char h) -> void;

#endif /* _t33nsy_HADAMARD */
