#include <string.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "encode.hpp"
#include "hadamard.hpp"
#include "temp_bpsk.hpp"
#include "temp_debpsk.hpp"

int main() {
  std::srand(std::time(nullptr));
  // std::string path = "messages/", output_path = "received_messages/message";
  std::string path = "../messages/",
              output_path = "../received_messages/message";
  std::vector<std::ifstream> files;

  // проходимся по директории messages и открываем все файлы на считывание
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    files.push_back(std::ifstream(entry.path()));
  }
  int number = files.size(), users_num = files.size();

  // очищаем файлы для получения
  std::vector<std::ofstream> output_files(users_num);

  // закидываем все файлы, куда будут записаны переданный данные в массив
  for (int i = 0; i < users_num; ++i) {
    output_files[i] = std::ofstream(output_path + std::to_string(i) + ".txt",
                                    std::ios_base::out);
    if (output_files[i].fail()) {
      std::cout << "Error: can't open file " << output_path + std::to_string(i)
                << ".txt\n";
    }
  }

  // задаем вторую несущую (пустого отправителя) для случаев, когда
  // пользователей четное число и может произойти зануление в bpsk
  bool flag_for_subcarrier = false;
  if (users_num % 2 == 0) {
    ++users_num;
    ++number;
    flag_for_subcarrier = true;
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
  generate_negative_hadamard(negative_hadamard, number, 0, 0, 1);

  char ****alphabet_for_all_users = new char ***[users_num];
  for (int i = 0; i < users_num; ++i) {
    alphabet_for_all_users[i] =
        generate_all_chars(hadamard[i], negative_hadamard[i], number);
  }

  // создадим переменную для времени в данный момент
  time_t rawtime, start_time, end_time;
  struct tm *timeinfo;

  // текущая дата, выраженная в секундах
  time(&rawtime);
  time(&start_time);

  // текущая дата, представленная в нормальной форме
  timeinfo = localtime(&rawtime);
  std::string current_time = asctime(timeinfo);
  std::cout << "Current time: " << current_time;

  // calloc для инициализации \0
  char *ether = new char[1024];
  std::fill(ether, ether + 1024, 0);
  // char *ether = (char *)calloc(1024, sizeof(char));

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

  // будем записывать время и операции в файл latest.log
  std::ofstream log("../latest.log", std::ios::out);

  // выделяем память под модулированный сигнал
  double *space_for_modulation =
      new double[(int)(bit_time * carrier_freq * 8 * 1024 * number)];
  std::fill(
      space_for_modulation,
      space_for_modulation + (int)(bit_time * carrier_freq * 8 * 1024 * number),
      0);

  char ***space_for_encoding = new char **[1024];
  double *modulated =
      new double[(int)(bit_time * carrier_freq * 8 * 1024 * number)];

  // цикл до конца всех файлов
  while (true) {
    int n = 0;
    bool end = false;
    std::fill(modulated,
              modulated + (int)(bit_time * carrier_freq * 8 * 1024 * number),
              0);
    // массив для проверки корректности передачи пакетов
    char **got_message = new char *[users_num];

    for (int i = 0; i < users_num; ++i) {
      got_message[i] = new char[1024];
      std::fill(got_message[i], got_message[i] + 1024, 0);
      if (i == users_num - 1 && flag_for_subcarrier) {
        encode(space_for_encoding, ether, 1024, alphabet_for_all_users[i],
               number);
        bpsk_modulation(space_for_modulation, space_for_encoding, carrier_wave,
                        number, 8);
        interfere(modulated, space_for_modulation,
                  (int)(bit_time * carrier_freq * 8 * 1024 * number));
        std::fill(ether, ether + 1024, 0);
      }

      // если файл кончился
      else if (files[i].eof()) {
        output_files[i].close();
        ended.insert(i);
        ++n;

        // все файлы кончились
        if (n == users_num - static_cast<int>(flag_for_subcarrier)) {
          if (i == users_num - 1 && flag_for_subcarrier) {
            encode(space_for_encoding, ether, 1024, alphabet_for_all_users[i],
                   number);
            bpsk_modulation(space_for_modulation, space_for_encoding,
                            carrier_wave, number, 8);
            interfere(modulated, space_for_modulation,
                      (int)(bit_time * carrier_freq * 8 * 1024 * number));
            std::fill(ether, ether + 1024, 0);
          }
          end = true;
          break;
        }
      } else {
        // считываем пакет размером 1 Кб
        files[i].read(ether, 1024);

        strcpy(got_message[i], ether);

        time(&rawtime);
        current_time = asctime(localtime(&rawtime));
        current_time.pop_back();
        std::cout << current_time << ": To ether added 1 KB packet from user"
                  << i << "\n";
        log << current_time << ": To ether added 1 KB packet from user " << i
            << "\n";

        // кодируем пакет
        encode(space_for_encoding, ether, 1024, alphabet_for_all_users[i],
               number);
        time(&rawtime);
        current_time = asctime(localtime(&rawtime));
        current_time.pop_back();
        log << current_time << ": Message from user " << i << " was encoded\n";

        // bpsk
        bpsk_modulation(space_for_modulation, space_for_encoding, carrier_wave,
                        number, 8);
        interfere(modulated, space_for_modulation,
                  (int)(bit_time * carrier_freq * 8 * 1024 * number));

        time(&rawtime);
        current_time = asctime(localtime(&rawtime));
        current_time.pop_back();
        log << current_time << ": Message from user " << i
            << " was modulated\n";

        std::fill(ether, ether + 1024, 0);
      }
    }
    if (end) {
      break;
    }

    char ***demodulated = bpsk_demodulation(
        modulated, carrier_wave, semi_carrier_wave_sum, 8, 1024, number);
    for (int i = 0; i < output_files.size(); ++i) {
      // если файл уже закончился
      if (ended.find(i) != ended.end()) continue;

      // debpsk
      time(&rawtime);
      current_time = asctime(localtime(&rawtime));
      current_time.pop_back();
      log << current_time << ": Message from user " << i
          << " was demodulated\n";

      // декодируем пакет
      char *decoded = decode(demodulated, 1024, hadamard[i], number);
      time(&rawtime);
      current_time = asctime(localtime(&rawtime));
      current_time.pop_back();
      log << current_time << ": Message from user " << i << " was decoded\n";

      // записываем получателю в файл
      if (output_files[i].is_open()) output_files[i] << decoded;
      time(&rawtime);
      current_time = asctime(localtime(&rawtime));
      current_time.pop_back();
      std::cout << current_time << ": Receiver " << i
                << " got message from user " << i << "\n";
      log << current_time << ": Receiver " << i << " got message from user "
          << i << "\n";
      int error = find_diff(decoded, got_message[i], 1024);
      log << "Packet has " << error << " bits different\n";
      errors[i].second += error;
      ++errors[i].first;
      free(decoded);
    }

    for (int i = 0; i < users_num; ++i) {
      delete[] got_message[i];
    }
    for (int i = 0; i < 1024; ++i) {
      for (int j = 0; j < 8; ++j) {
        delete[] demodulated[i][j];
      }
      delete[] demodulated[i];
    }
    delete[] demodulated;
    delete[] got_message;
  }
  time(&end_time);
  std::cout << "Total elapsed time: " << difftime(end_time, start_time)
            << " sec\n";

  // FIX
  //  for (int i = 0; i < users_num; ++i) {
  //    for (int j = 0; j < 256; ++j) {
  //      for (int k = 0; k < 8; ++k) {
  //        delete[] alphabet_for_all_users[i][j][k];
  //      }
  //      delete[] alphabet_for_all_users[i][j];
  //    }
  //    delete[] alphabet_for_all_users[i];
  //  }
  //  delete[] alphabet_for_all_users;

  for (int i = 0; i < number; ++i) {
    delete[] hadamard[i];
    delete[] negative_hadamard[i];
  }
  delete[] hadamard;
  delete[] negative_hadamard;
  delete[] t;
  delete[] carrier_wave;
  delete[] ether;
  delete[] modulated;
  std::ofstream graph("../errors.csv", std::ios::out);
  // std::ofstream graph("errors.csv", std::ios::out);
  graph << "Packets;Errors\n";
  for (int i = 0; i < users_num - static_cast<int>(flag_for_subcarrier); ++i) {
    graph << errors[i].first << ";" << errors[i].second << "\n";
  }
  return 0;
}