#ifndef _t33nsy_ENCODE
#define _t33nsy_ENCODE

#include <cstring>
#include <string>

auto encode(char *space, const char *str, int length, char *alphabet,
            int key_size) -> void;

auto generate_coded_char(char *space, char val, char *key, char *neg_key,
                         int key_size) -> void;

auto generate_all_chars(char *space, char *key, char *neg_key,
                        int key_size) -> void;

auto decode(char *coded, int length, char *key, int key_size) -> char *;

auto find_diff(char *a, char *b, int packet_size) -> int;

auto matrix_multiply(char *a, char *key, int key_size) -> char;

#endif /* _t33nsy_ENCODE */
