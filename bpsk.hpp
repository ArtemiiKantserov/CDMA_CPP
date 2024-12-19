#ifndef _t33nsy_BPSK
#define _t33nsy_BPSK

auto bpsk_demodulation(char* demodulated, double* signal,
                       double semi_carrier_wave_sum, int samples_per_bit,
                       int packet_size, int key_length) -> void;

#include <random>

auto linspace(double start, double end, int count, bool endpoint) -> double*;

auto generate_carrier_wave(int count, double* arr, int carrier_freq) -> double*;

auto bpsk_modulation(double* space, char* bits, double* carrier_wave,
                     int len_hadamard, int len_wave, double min_noise,
                     double max_noise) -> void;

auto interfere(double* first, double* second, int len) -> void;

auto noise(double min, double max) -> double;
#endif /* _t33nsy_BPSK */
