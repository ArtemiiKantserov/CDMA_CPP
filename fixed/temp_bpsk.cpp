#include "temp_bpsk.hpp"

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

// 1024*[8*[адамар]] - bits
auto bpsk_modulation(char*** bits, double* carrier_wave, int len_hadamard,
                     int len_wave) -> double* {
  double* moduleted_signal = new double[len_wave * 8 * 1024 * len_hadamard];
  int index = 0;
  for (int i = 0; i < 1024; ++i) {
    for (int j = 0; j < 8; ++j) {
      for (int k = 0; k < len_hadamard; ++k) {
        if (bits[i][j][k] == 1) {
          for (int l = 0; l < len_wave; ++l) {
            moduleted_signal[index++] = carrier_wave[l];
          }
        } else {
          for (int l = 0; l < len_wave; ++l) {
            moduleted_signal[index++] = -carrier_wave[l];
          }
        }
      }
    }
  }
  return moduleted_signal;
}

auto noise(double min, double max) -> double {
  return (double)(rand()) / RAND_MAX * (max - min) + min;
}

auto bpsk_modulation(double* space, char*** bits, double* carrier_wave,
                     int len_hadamard, int len_wave) -> void {
  int index = 0;
  for (int i = 0; i < 1024; ++i) {
    for (int j = 0; j < 8; ++j) {
      for (int k = 0; k < len_hadamard; ++k) {
        if (bits[i][j][k] == 1) {
          for (int l = 0; l < len_wave; ++l) {
            space[index++] = (carrier_wave[l] + noise(-0.1, 0.1));
          }
        } else {
          for (int l = 0; l < len_wave; ++l) {
            space[index++] = -(carrier_wave[l] + noise(-0.1, 0.1));
          }
        }
      }
    }
  }
}

auto bpsk_modulation(double* space, char*** bits, double* carrier_wave,
                     int len_hadamard, int len_wave, double min_noise,
                     double max_noise) -> void {
  int index = 0;
  for (int i = 0; i < 1024; ++i) {
    for (int j = 0; j < 8; ++j) {
      for (int k = 0; k < len_hadamard; ++k) {
        if (bits[i][j][k] == 1) {
          for (int l = 0; l < len_wave; ++l) {
            space[index++] = (carrier_wave[l] + noise(-0.1, 0.1));
          }
        } else {
          for (int l = 0; l < len_wave; ++l) {
            space[index++] = -(carrier_wave[l] + noise(-0.1, 0.1));
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

auto add_subcarrier(double* first, double* second, int len) -> void {
  for (int i = 0; i < len; ++i) {
    first[i] += second[i];
  }
}

auto create_subcarrier(double* carrier_wave, int len,
                       int hadamard_size) -> double* {
  double* subcarrier = new double[len * 8 * hadamard_size * 1024];
  for (int i = 0; i < len * 8 * hadamard_size * 1024; ++i) {
    subcarrier[i] = -carrier_wave[i % len];
  }
  return subcarrier;
}