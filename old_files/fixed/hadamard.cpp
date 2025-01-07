#include "hadamard.hpp"

auto generate_hadamard(char **H, int size, int i, int j, char h) -> void {
  if (size == 1)
    H[i][j] = h;
  else {
    generate_hadamard(H, size / 2, i, j, h);
    generate_hadamard(H, size / 2, i, size / 2 + j, h);
    generate_hadamard(H, size / 2, size / 2 + i, j, h);
    generate_hadamard(H, size / 2, size / 2 + i, size / 2 + j, -h);
  }
}