#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "bpsk.hpp"
#include "encode.hpp"
#include "hadamard.hpp"

#define PACKET_SIZE 1024
#define BYTE_SIZE 8
#define CHARS_NUM 256

int main() {
  srand(time(0));
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

  // очищаем файлы для получения
  std::vector<std::ofstream> output_files(users_num);

  // закидываем все файлы, куда будут записаны переданный данные в массив
  for (int i = 0; i < users_num; ++i) {
    files_for_output.push_back(output_path + std::to_string(i + 1) + ".txt");
  }
  std::sort(files_for_output.begin(), files_for_output.end());
  for (int i = 0; i < users_num; ++i) {
    output_files[i] = std::ofstream(files_for_output[i], std::ios_base::out);
    if (output_files[i].fail()) {
      std::cout << "Error: can't open file " << files_for_output[i] << ".txt\n";
    }
  }

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
  char *alphabet_for_all_users =
      new char[(users_num + 1) * CHARS_NUM * BYTE_SIZE * number];
  for (int i = 0; i < users_num + 1; ++i) {
    generate_all_chars(
        alphabet_for_all_users + i * CHARS_NUM * BYTE_SIZE * number,
        hadamard[i], negative_hadamard[i], number);
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

  // будем записывать время и операции в файл latest.log
  std::ofstream log("../latest.log", std::ios::out);

  // количество памяти необходимое под модулированный сигнал
  int space_needed =
      (int)(bit_time * carrier_freq * BYTE_SIZE * PACKET_SIZE * number);

  // выделяем память под модулированный сигнал
  double *space_for_modulation = new double[space_needed];

  char *space_for_encoding = new char[PACKET_SIZE * number * BYTE_SIZE];
  // char ***space_for_encoding = new char **[PACKET_SIZE];
  char *demodulated = new char[PACKET_SIZE * number * BYTE_SIZE];
  double *modulated = new double[space_needed];

  // цикл до конца всех файлов
  while (true) {
    int n = 0, iterative_num = 0;
    bool end = false;
    std::fill(modulated, modulated + space_needed, 0.0);
    // массив для проверки корректности передачи пакетов
    char **got_message = new char *[users_num];

    for (int i = 0; i < users_num; ++i) {
      got_message[i] = new char[PACKET_SIZE];
      std::fill(got_message[i], got_message[i] + PACKET_SIZE, 0);
      std::fill(ether, ether + PACKET_SIZE, 0);

      // если файл кончился
      if (files[i].eof()) {
        output_files[i].close();
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

        time(&rawtime);
        current_time = asctime(localtime(&rawtime));
        current_time.pop_back();
        std::cout << current_time << ": Added 1 KB packet from user " << i
                  << " to ether\n";
        log << current_time << ": Added 1 KB packet from user " << i
            << " to ether\n";

        // кодируем пакет
        encode(space_for_encoding, ether, PACKET_SIZE,
               alphabet_for_all_users + i * CHARS_NUM * BYTE_SIZE * number,
               number);

        time(&rawtime);
        current_time = asctime(localtime(&rawtime));
        current_time.pop_back();
        log << current_time << ": Message from user " << i << " encoded\n";

        // bpsk
        bpsk_modulation(space_for_modulation, space_for_encoding, carrier_wave,
                        number, BYTE_SIZE, -0.15, 0.15);
        interfere(modulated, space_for_modulation, space_needed);

        time(&rawtime);
        current_time = asctime(localtime(&rawtime));
        current_time.pop_back();
        log << current_time << ": Message from user " << i << " modulated\n";
      }
      if (i == users_num - 1 && iterative_num % 2 == 0) {
        // если количество пользователей четное, необходимо забить канал еще
        // одним пустым "пользователем", чтобы не происходило возможного
        // зануления сигнала
        std::fill(ether, ether + PACKET_SIZE, 0);
        encode(space_for_encoding, ether, PACKET_SIZE,
               alphabet_for_all_users + i * CHARS_NUM * BYTE_SIZE * number,
               number);
        bpsk_modulation(space_for_modulation, space_for_encoding, carrier_wave,
                        number, BYTE_SIZE, -0.15, 0.15);
        interfere(modulated, space_for_modulation, space_needed);
      }
    }

    bpsk_demodulation(demodulated, modulated, semi_carrier_wave_sum, BYTE_SIZE,
                      PACKET_SIZE, number);

    for (int i = 0; i < output_files.size(); ++i) {
      // если файл уже закончился
      if (ended.find(i) != ended.end()) continue;

      // debpsk
      time(&rawtime);
      current_time = asctime(localtime(&rawtime));
      current_time.pop_back();
      log << current_time << ": Message from user " << i << " demodulated\n";

      // декодируем пакет
      char *decoded = decode(demodulated, PACKET_SIZE, hadamard[i], number);
      time(&rawtime);
      current_time = asctime(localtime(&rawtime));
      current_time.pop_back();
      log << current_time << ": Message from user " << i << " decoded\n";

      // записываем получателю в файл
      if (output_files[i].is_open()) output_files[i] << decoded;
      time(&rawtime);
      current_time = asctime(localtime(&rawtime));
      current_time.pop_back();
      std::cout << current_time << ": Receiver " << i
                << " got message from user " << i << "\n";
      log << current_time << ": Receiver " << i << " got message from user "
          << i << "\n";

      int error = find_diff(decoded, got_message[i], PACKET_SIZE);
      if (error > 0) {
        ++errors[i].second;
        log << "Packet has difference\n";
      }
      ++errors[i].first;
      delete[] decoded;
    }

    for (int i = 0; i < users_num; ++i) {
      delete[] got_message[i];
    }
    delete[] got_message;

    if (end) {
      break;
    }
  }
  time(&end_time);
  std::cout << "Total elapsed time: " << difftime(end_time, start_time)
            << " sec\n";

  delete[] t;
  delete[] space_for_encoding;
  delete[] space_for_modulation;
  delete[] alphabet_for_all_users;
  delete[] ether;
  delete[] modulated;
  std::ofstream graph("../errors.csv", std::ios::out);
  graph << "Packets, kilobytes;Errors, packets\n";
  for (int i = 0; i < users_num; ++i) {
    graph << errors[i].first << ";" << errors[i].second << "\n";
  }
  std::cout << "Sender --> Receiver\n";
  for (int i = 0; i < users_num; ++i) {
    std::cout << files_for_input[i] << " --> " << files_for_output[i]
              << " Code: ";
    for (int j = 0; j < number; ++j) {
      std::cout << std::setw(2) << (int)hadamard[i][j] << " ";
    }
    std::cout << "\n";
  }
  for (int i = 0; i < number; ++i) {
    delete[] hadamard[i];
    delete[] negative_hadamard[i];
  }
  delete[] hadamard;
  delete[] negative_hadamard;
  return 0;
}