#include "temp_bpsk.hpp"

#include <iostream>
#include <random>

auto linspace(double start, double end, int count, bool endpoint) -> double* {
  double step = (static_cast<double>(fabs(end - start))) / (count - 1);
  // std::cout << step << std::endl;
  if (!endpoint) {
    --count;
  }
  double* arr = new double[count];
  for (int i = 0; i < count; ++i) {
    arr[i] = static_cast<double>(start) + i * step;
    // printf("%lf\n", arr[i]);
  }
  return arr;
}

auto generate_carrier_wave(int count, double* arr,
                           int carrier_freq) -> double* {
  double* carrier_wave = new double[count];
  for (int i = 0; i < count; ++i) {
    carrier_wave[i] = sin(2 * M_PI * carrier_freq * arr[i]);
    // printf("%lf %lf\n", arr[i], carrier_wave[i]);
  }
  return carrier_wave;
}

std::mt19937 gen(std::random_device{}());
std::uniform_real_distribution<> dis(-0, 0);

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
            // moduleted_signal[index++] += dis(gen);
          }
        } else {
          for (int l = 0; l < len_wave; ++l) {
            moduleted_signal[index++] = -carrier_wave[l];
            // moduleted_signal[index++] += dis(gen);
          }
        }
      }
    }
  }
  return moduleted_signal;
}

auto interfere(double* first, double* second, int len) -> void {
  for (int i = 0; i < len; ++i) {
    first[i] += second[i];
  }
}
