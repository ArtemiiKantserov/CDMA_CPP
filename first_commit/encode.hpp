#ifndef _t33nsy_ENCODE
#define _t33nsy_ENCODE

#include <string>
auto encode(const char *str, int length, char ***alphabet,
            int key_size) -> char ***;

auto generate_coders(char *key, int key_size) -> char **;

auto generate_coded_char(char val, char *key, char *neg_key,
                         int key_size) -> char **;

auto generate_all_chars(char *key, char *neg_key, int key_size) -> char ***;

auto decode(char ***coded, int length, char *key, int key_size) -> char *;

auto matrix_multiply(char **a, char *key, int key_size) -> char;
#endif /* _t33nsy_ENCODE */
