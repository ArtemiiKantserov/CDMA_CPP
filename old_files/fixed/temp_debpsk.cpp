#include "temp_debpsk.hpp"

auto bpsk_demodulation(double* signal, double semi_carrier_wave_sum,
                       int samples_per_bit, int packet_size,
                       int key_length) -> char*** {
  char*** demodulated = new char**[packet_size];
  for (int i = 0; i < packet_size; ++i) {
    demodulated[i] = new char*[8];
    for (int j = 0; j < 8; ++j) {
      demodulated[i][j] = new char[key_length];
      for (int k = 0; k < key_length; ++k) {
        double ratio = 0, sum1 = 0, sum2 = 0;
        for (int l = 0; l < samples_per_bit / 2; ++l) {
          ratio += signal[i * 8 * samples_per_bit * key_length +
                          j * key_length * samples_per_bit +
                          k * samples_per_bit + l];
        }
        if (ratio / semi_carrier_wave_sum > 0) {
          demodulated[i][j][k] = 1;
        } else {
          demodulated[i][j][k] = -1;
        }
      }
    }
  }
  return demodulated;
}

auto bpsk_demodulation(double* signal, double* carrier_wave,
                       double semi_carrier_wave_sum, int samples_per_bit,
                       int packet_size, int key_length) -> char*** {
  char*** demodulated = new char**[packet_size];
  for (int i = 0; i < packet_size; ++i) {
    demodulated[i] = new char*[8];
    for (int j = 0; j < 8; ++j) {
      demodulated[i][j] = new char[key_length];
      for (int k = 0; k < key_length; ++k) {
        double ratio = 0, sum1 = 0, sum2 = 0;
        for (int l = 0; l < samples_per_bit / 2; ++l) {
          ratio += signal[i * 8 * samples_per_bit * key_length +
                          j * key_length * samples_per_bit +
                          k * samples_per_bit + l];
        }
        if (ratio / semi_carrier_wave_sum > 0) {
          demodulated[i][j][k] = 1;
        } else {
          demodulated[i][j][k] = -1;
        }
      }
    }
  }
  return demodulated;
}

auto bpsk_demodulation(char* demodulated, double* signal, double* carrier_wave,
                       double semi_carrier_wave_sum, int samples_per_bit,
                       int packet_size, int key_length) -> void {
  for (int i = 0; i < packet_size; ++i) {
    for (int j = 0; j < 8; ++j) {
      for (int k = 0; k < key_length; ++k) {
        double ratio = 0, sum1 = 0, sum2 = 0;
        for (int l = 0; l < samples_per_bit / 2; ++l) {
          ratio += signal[i * 8 * samples_per_bit * key_length +
                          j * key_length * samples_per_bit +
                          k * samples_per_bit + l];
        }
        if (ratio / semi_carrier_wave_sum > 0) {
          demodulated[i * 8 * key_length + j * key_length + k] = 1;
        } else {
          demodulated[i * 8 * key_length + j * key_length + k] = -1;
        }
      }
    }
  }
}