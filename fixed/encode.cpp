#include "encode.hpp"

#include <bit>
#include <iostream>
auto encode(const char* str, int length, char*** alphabet,
            int key_size) -> char*** {
  char*** encoded = new char**[length];
  for (int i = 0; i < length; ++i) {
    encoded[i] = alphabet[str[i] + 128];
  }
  return encoded;
}

auto encode(char*** space, const char* str, int length, char*** alphabet,
            int key_size) -> void {
  for (int i = 0; i < length; ++i) {
    space[i] = alphabet[str[i] + 128];
  }
}

auto generate_coded_char(char val, char* key, char* neg_key,
                         int key_size) -> char** {
  char** coded_char = new char*[8];
  for (int i = 0; i < 8; i++) {
    coded_char[i] = new char[key_size];
  }
  for (int i = 0, j = 7; i < 8; ++i, --j) {
    if ((val >> i) & 1u) {
      coded_char[j] = key;
    } else {
      coded_char[j] = neg_key;
    }
  }
  return coded_char;
}

auto generate_all_chars(char* key, char* neg_key, int key_size) -> char*** {
  char*** all_chars = new char**[256];
  for (int i = 0; i < 256; ++i) {
    all_chars[i] = generate_coded_char(i - 128, key, neg_key, key_size);
  }
  return all_chars;
}

auto decode(char*** coded, int length, char* key, int key_size) -> char* {
  char* decoded = (char*)calloc(length, sizeof(char));
  for (int i = 0; i < length; ++i) {
    decoded[i] = matrix_multiply(coded[i], key, key_size);
  }
  return decoded;
}

auto matrix_multiply(char** a, char* key, int key_size) -> char {
  char result = 0;
  for (int i = 0; i < 8; ++i) {
    int temp = 0;
    for (int k = 0; k < key_size; ++k) {
      temp += a[i][k] * key[k];
    }
    if (temp > 0) {
      result += 1 << (7 - i);
    }
  }
  return result;
}

auto clear_encoded(char*** encoded, int size) -> void {
  for (int i = 0; i < size; ++i) {
    delete[] encoded[i];
  }
  delete[] encoded;
}

auto find_diff(char* a, char* b, int packet_size) -> int {
  int diff = 0;
  for (int i = 0; i < packet_size; ++i) {
    // char xorred = a[i] ^ b[i];
    // for (int j = 0; j < 8; ++j) {
    //   diff += ((a[i] >> j) & 1u) ^ ((b[i] >> j) & 1u);
    // }
    diff += std::__popcount(a[i] ^ b[i]);
  }
  return diff;
}