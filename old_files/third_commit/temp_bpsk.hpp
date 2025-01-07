#ifndef _t33nsy_TEMP_BPSK
#define _t33nsy_TEMP_BPSK
auto linspace(double start, double end, int count, bool endpoint) -> double*;

auto generate_carrier_wave(int count, double* arr, int carrier_freq) -> double*;

// auto noise_modulation() -> double;

auto bpsk_modulation(char*** bits, double* carrier_wave, int len_hadamard,
                     int len_wave) -> double*;

auto interfere(double* first, double* second, int len) -> void;

#endif /* _t33nsy_TEMP_BPSK */
