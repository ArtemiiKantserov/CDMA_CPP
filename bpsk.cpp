#include "bpsk.hpp"

#define M_PI 3.14159265358979323846

auto linspace(double start, double end, int count, bool endpoint) -> double* {
  double step = (static_cast<double>(fabs(end - start))) / (count - 1);
  if (!endpoint) {
    --count;
  }
  double* arr = new double[count];
  for (int i = 0; i < count; ++i) {
    arr[i] = static_cast<double>(start) + i * step;
  }
  return arr;
}

auto generate_carrier_wave(int count, double* arr,
                           int carrier_freq) -> double* {
  double* carrier_wave = new double[count];
  for (int i = 0; i < count; ++i) {
    carrier_wave[i] = sin(2 * M_PI * carrier_freq * arr[i]);
  }
  return carrier_wave;
}

auto noise(double min, double max) -> double {
  return (double)(rand()) / RAND_MAX * (max - min) + min;
}

auto bpsk_modulation(double* space, char* bits, double* carrier_wave,
                     int len_hadamard, int len_wave, double min_noise,
                     double max_noise) -> void {
  int index = 0;
  for (int i = 0; i < 1024; ++i) {
    for (int j = 0; j < 8; ++j) {
      for (int k = 0; k < len_hadamard; ++k) {
        if (bits[i * 8 * len_hadamard + j * len_hadamard + k] == 1) {
          for (int l = 0; l < len_wave; ++l) {
            space[index++] = (carrier_wave[l] + noise(min_noise, max_noise));
          }
        } else {
          for (int l = 0; l < len_wave; ++l) {
            space[index++] = -(carrier_wave[l] + noise(min_noise, max_noise));
          }
        }
      }
    }
  }
}

auto interfere(double* first, double* second, int len) -> void {
  for (int i = 0; i < len; ++i) {
    first[i] += second[i];
  }
}

auto bpsk_demodulation(char* demodulated, double* signal,
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