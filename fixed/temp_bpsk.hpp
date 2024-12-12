#ifndef _t33nsy_TEMP_BPSK
#define _t33nsy_TEMP_BPSK

#include <random>

auto linspace(double start, double end, int count, bool endpoint) -> double*;

auto generate_carrier_wave(int count, double* arr, int carrier_freq) -> double*;

// auto noise_modulation() -> double;

auto bpsk_modulation(char*** bits, double* carrier_wave, int len_hadamard,
                     int len_wave) -> double*;

auto bpsk_modulation(double* space, char*** bits, double* carrier_wave,
                     int len_hadamard, int len_wave) -> void;

auto interfere(double* first, double* second, int len) -> void;

auto add_subcarrier(double* first, double* second, int len) -> void;

auto create_subcarrier(double* carrier_wave, int len,
                       int hadamard_size) -> double*;

auto noise(double min, double max) -> double;

#endif /* _t33nsy_TEMP_BPSK */
