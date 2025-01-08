#include "hadamard.hpp"

auto generate_hadamard(char **H, int size, int i, int j, char h) -> void {
  // проверка на то, что размер - степень двойки
  if (!(size > 0 && (size & (size - 1)) == 0)) throw "Invalid size";
  if (size == 1)
    // условие выхода из рекурсии
    H[i][j] = h;
  else {
    // вызываем рекурсивно на каждые 4 блока
    generate_hadamard(H, size / 2, i, j, h);
    generate_hadamard(H, size / 2, i, size / 2 + j, h);
    generate_hadamard(H, size / 2, size / 2 + i, j, h);
    generate_hadamard(H, size / 2, size / 2 + i, size / 2 + j, -h);
  }
}