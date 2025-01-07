#include <iomanip>
#include <iostream>

#include "encode.hpp"
#include "hadamard.hpp"

int main() {
  int number;
  std::cout << "Enter a number of users: ";
  std::cin >> number;

  // быстрое определение ближайшей степени двойки для построения матрицы адамара
  --number;
  for (unsigned k = 0; k <= 4; ++k) {
    number |= number >> (1 << k);
  }
  ++number;

  // выделяем память под матрицу адамара
  char **hadamard = new char *[number],
       **negative_hadamard = new char *[number];
  for (int i = 0; i < number; ++i) {
    hadamard[i] = new char[number];
    negative_hadamard[i] = new char[number];
  }

  // генерируем матрицу адамара и матрицу адамара, умноженную на -1
  generate_hadamard(hadamard, number, 0, 0, 1);
  generate_negative_hadamard(negative_hadamard, number, 0, 0, 1);
  // char ***coded_char_array =
  //     generate_all_chars(hadamard[1], negative_hadamard[1], number);
  // char *test = "kotak";
  // char ***encoded = encode(test, 5, coded_char_array, number);
  // char *decoded = decode(encoded, 5, hadamard[1], number);
  // std::cout << decoded << "\n";

  char *ephir = new char[1024];
  for (int i = 0; i < number; ++i) {
    char ***coded_char_array =
        generate_all_chars(hadamard[i], negative_hadamard[i], number);
    std::string str;
    std::cin >> str;
    while (!str.empty()) {
      ephir = (char *)str.substr(0, std::min(1024,
      (int)str.size())).c_str(); std::cout << "To ephir added: " << ephir
      << "\n"; char ***encoded = encode(ephir, std::min(1024,
      (int)str.size()),
                               coded_char_array, number);
      char *decoded =
          decode(encoded, std::min(1024, (int)str.size()), hadamard[i],
          number);
      std::cout << "User " << i << " got: " << decoded << "\n";
      str = str.substr(std::min(1024, (int)str.size()));
    }
    std::cout << "\n";
  }
  // for (int i = 0; i < 8; ++i) {
  //   for (int j = 0; j < number; ++j) {
  //     std::cout << std::setw(3) << (int)coded_char_array[128 +
  //     'h'][i][j];
  //   }
  //   std::cout << "\n";
  // }
  return 0;
}