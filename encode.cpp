#include "encode.hpp"

auto encode(char* space, const char* str, int length, char* alphabet,
            int key_size) -> void {
  for (int i = 0; i < length; ++i) {
    strncpy(space + i * 8 * key_size, alphabet + (str[i] + 128) * 8 * key_size,
            8 * key_size);
  }
}

auto generate_coded_char(char* space, char val, char* key, char* neg_key,
                         int key_size) -> void {
  for (int i = 0, j = 7; i < 8; ++i, --j) {
    if ((val >> i) & 1u) {
      strncpy(space + j * key_size, key, key_size);
    } else {
      strncpy(space + j * key_size, neg_key, key_size);
    }
  }
}

auto generate_all_chars(char* space, char* key, char* neg_key,
                        int key_size) -> void {
  for (int i = 0; i < 256; ++i) {
    generate_coded_char(space + i * 8 * key_size, i - 128, key, neg_key,
                        key_size);
  }
}

auto find_diff(char* a, char* b, int packet_size) -> int {
  int diff = 0;
  for (int i = 0; i < packet_size; ++i) {
    char xorred = a[i] ^ b[i];
    for (int j = 0; j < 8; ++j) {
      diff += (xorred >> j) & 1u;
    }
  }
  return diff;
}

auto matrix_multiply(char* a, char* key, int key_size) -> char {
  char result = 0;
  for (int i = 0; i < 8; ++i) {
    int temp = 0;
    for (int k = 0; k < key_size; ++k) {
      temp += a[i * key_size + k] * key[k];
    }
    if (temp > 0) {
      result += 1 << (7 - i);
    }
  }
  return result;
}

auto decode(char* coded, int length, char* key, int key_size) -> char* {
  char* decoded = new char[length]{0};
  for (int i = 0; i < length; ++i) {
    decoded[i] = matrix_multiply(coded + i * 8 * key_size, key, key_size);
  }
  return decoded;
}