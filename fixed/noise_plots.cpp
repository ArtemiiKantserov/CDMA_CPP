#include <string.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "encode.hpp"
#include "hadamard.hpp"
#include "temp_bpsk.hpp"
#include "temp_debpsk.hpp"

#define PACKET_SIZE 1024

double iterate(double noise_lower_limit, double noise_upper_limit) {
  std::string path = "../messages/",
              output_path = "../received_messages/message";
  std::vector<std::ifstream> files;
  std::vector<std::string> files_for_input, files_for_output;

  // проходимся по директории messages и открываем все файлы на считывание
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    files.push_back(std::ifstream(entry.path()));
    files_for_input.push_back(entry.path().string());
  }
  int number = files.size(), users_num = files.size();

  // задаем вторую несущую (пустого отправителя) для случаев, когда
  // пользователей четное число и может произойти зануление в bpsk
  if (users_num % 2 == 0) {
    ++number;
  }

  // быстрое определение ближайшей степени двойки для построения матрицы
  // адамара
  --number;
  for (unsigned k = 0; k <= 4; ++k) {
    number |= number >> (1 << k);
  }
  ++number;

  // выделяем память под матрицу адамара
  char **hadamard = new char *[number],
       **negative_hadamard = new char *[number];
  for (int i = 0; i < number; ++i) {
    hadamard[i] = new char[number];
    negative_hadamard[i] = new char[number];
  }

  // создаем вектора для расчета количества ошибок
  std::vector<std::pair<long long, long long>> errors(users_num, {0, 0});

  // генерируем матрицу адамара и матрицу адамара, умноженную на -1
  generate_hadamard(hadamard, number, 0, 0, 1);
  generate_hadamard(negative_hadamard, number, 0, 0, -1);

  // создаем алфавит для пользователей
  char ****alphabet_for_all_users = new char ***[users_num + 1];
  for (int i = 0; i < users_num + 1; ++i) {
    alphabet_for_all_users[i] =
        generate_all_chars(hadamard[i], negative_hadamard[i], number);
  }

  // calloc для инициализации \0
  char *ether = new char[PACKET_SIZE];
  std::fill(ether, ether + PACKET_SIZE, 0);

  // генерация несущей с частотой carrier_freq и временем одного сигнала
  // bit_time
  int carrier_freq = 80;
  double bit_time = 0.1;
  double *t = linspace(0, bit_time, bit_time * carrier_freq, true);
  double *carrier_wave =
      generate_carrier_wave(bit_time * carrier_freq, t, carrier_freq);
  double semi_carrier_wave_sum = 0;
  for (int i = 0; i < bit_time * carrier_freq / 2; ++i) {
    semi_carrier_wave_sum += carrier_wave[i];
  }

  // множество для проверки конца передачи
  std::set<int> ended;

  // выделяем память под модулированный сигнал
  double *space_for_modulation =
      new double[(int)(bit_time * carrier_freq * 8 * PACKET_SIZE * number)];
  std::fill(space_for_modulation,
            space_for_modulation +
                (int)(bit_time * carrier_freq * 8 * PACKET_SIZE * number),
            0);

  char ***space_for_encoding = new char **[PACKET_SIZE];
  double *modulated =
      new double[(int)(bit_time * carrier_freq * 8 * PACKET_SIZE * number)];

  // цикл до конца всех файлов
  while (true) {
    int n = 0, iterative_num = 0;
    bool end = false;
    std::fill(
        modulated,
        modulated + (int)(bit_time * carrier_freq * 8 * PACKET_SIZE * number),
        0.0);
    // массив для проверки корректности передачи пакетов
    char **got_message = new char *[users_num];

    for (int i = 0; i < users_num; ++i) {
      got_message[i] = new char[PACKET_SIZE];
      std::fill(got_message[i], got_message[i] + PACKET_SIZE, 0);
      std::fill(ether, ether + PACKET_SIZE, 0);

      // если файл кончился
      if (files[i].eof()) {
        ended.insert(i);
        // все файлы кончились
        if (ended.size() == users_num) {
          end = true;
        }
      } else {
        // считываем пакет размером 1 Кб
        files[i].read(ether, PACKET_SIZE);
        strcpy(got_message[i], ether);
        ++iterative_num;

        // кодируем пакет
        encode(space_for_encoding, ether, PACKET_SIZE,
               alphabet_for_all_users[i], number);

        // bpsk
        bpsk_modulation(space_for_modulation, space_for_encoding, carrier_wave,
                        number, 8, noise_lower_limit, noise_upper_limit);
        interfere(modulated, space_for_modulation,
                  (int)(bit_time * carrier_freq * 8 * PACKET_SIZE * number));
      }
      if (i == users_num - 1 && iterative_num % 2 == 0) {
        // если количество пользователей четное, необходимо забить канал еще
        // одним пустым "пользователем", чтобы не происходило возможного
        // зануления сигнала
        std::fill(ether, ether + PACKET_SIZE, 0);
        encode(space_for_encoding, ether, PACKET_SIZE,
               alphabet_for_all_users[users_num], number);
        bpsk_modulation(space_for_modulation, space_for_encoding, carrier_wave,
                        number, 8, noise_lower_limit, noise_upper_limit);
        interfere(modulated, space_for_modulation,
                  (int)(bit_time * carrier_freq * 8 * PACKET_SIZE * number));
      }
    }

    char ***demodulated = bpsk_demodulation(
        modulated, carrier_wave, semi_carrier_wave_sum, 8, PACKET_SIZE, number);
    for (int i = 0; i < users_num; ++i) {
      // если файл уже закончился
      if (ended.find(i) != ended.end()) continue;

      // декодируем пакет
      char *decoded = decode(demodulated, PACKET_SIZE, hadamard[i], number);

      // записываем получателю в файл
      int error = find_diff(decoded, got_message[i], PACKET_SIZE);
      if (error > 0) {
        ++errors[i].second;
      }
      ++errors[i].first;
      free(decoded);
    }

    for (int i = 0; i < users_num; ++i) {
      delete[] got_message[i];
    }
    for (int i = 0; i < PACKET_SIZE; ++i) {
      for (int j = 0; j < 8; ++j) {
        delete[] demodulated[i][j];
      }
      delete[] demodulated[i];
    }
    delete[] demodulated;
    delete[] got_message;

    if (end) {
      break;
    }
  }

  delete[] t;
  // delete[] carrier_wave;
  delete[] ether;
  delete[] modulated;
  long long max_error = 0, max_packets = 0;
  for (int i = 0; i < users_num; ++i) {
    max_error = std::max(max_error, errors[i].second);
    max_packets = std::max(max_packets, errors[i].first);
  }
  for (int i = 0; i < number; ++i) {
    delete[] hadamard[i];
    delete[] negative_hadamard[i];
  }
  delete[] hadamard;
  delete[] negative_hadamard;
  delete[] space_for_encoding;
  delete[] space_for_modulation;
  delete[] alphabet_for_all_users;
  return static_cast<double>(max_error) / max_packets;
}

int main(int argc, char *argv[]) {
  double noise_lower_limit, noise_upper_limit, step;
  int number_of_tests;
  if (argc == 5) {
    noise_lower_limit = atof(argv[1]);  // нижняя граница
    noise_upper_limit = atof(argv[2]);  // верхняя граница
    step = atof(argv[3]);               // шаг
    number_of_tests = atoi(argv[4]);    // количество тестов
  } else {
    return 0;
  }
  std::ofstream graph("../noise_errors.csv", std::ios::out);
  srand(time(0));
  for (double i = 0; noise_lower_limit + i < noise_upper_limit - i; i += step) {
    double mean = 0;
    for (int j = 0; j < number_of_tests; ++j) {
      mean += iterate(noise_lower_limit + i, noise_upper_limit - i);
      std::cout << j << " ";
    }
    mean /= (double)number_of_tests;
    std::cout << mean << std::endl;
    std::cout << noise_upper_limit - noise_lower_limit - i * 2 << std::endl;
    graph << mean << ";" << noise_upper_limit - noise_lower_limit - i * 2
          << "\n";
  }
  return 0;
}