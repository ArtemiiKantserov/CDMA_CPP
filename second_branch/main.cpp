#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "encode.hpp"
#include "hadamard.hpp"
#include "temp_bpsk.hpp"
#include "temp_debpsk.hpp"

int main() {
  std::string path = "../messages/",
              output_path = "../received_messages/message";
  std::vector<std::ifstream> files;

  // проходимся по директории messages и открываем все файлы на считывание
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    files.push_back(std::ifstream(entry.path()));
  }
  int number = files.size(), users_num = files.size();

  // закидываем все файлы, куда будут записаны переданный данные в массив
  std::vector<std::ofstream> output_files(users_num);
  for (int i = 0; i < users_num; ++i) {
    output_files[i] = std::ofstream(output_path + std::to_string(i) + ".txt",
                                    std::ios_base::app);
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

  // генерируем матрицу адамара и матрицу адамара, умноженную на -1
  generate_hadamard(hadamard, number, 0, 0, 1);
  generate_negative_hadamard(negative_hadamard, number, 0, 0, 1);

  char ****alphabet_for_all_users = new char ***[users_num];
  for (int i = 0; i < users_num; ++i) {
    alphabet_for_all_users[i] =
        generate_all_chars(hadamard[i], negative_hadamard[i], number);
  }

  // создадим переменную для времени в данный момент
  time_t rawtime;
  struct tm *timeinfo;
  // текущая дата, выраженная в секундах
  time(&rawtime);
  // текущая дата, представленная в нормальной форме
  timeinfo = localtime(&rawtime);
  std::string current_time = asctime(timeinfo);
  std::cout << "Current time: " << current_time;

  // calloc для инициализации \0
  char *ephir = (char *)calloc(1024, sizeof(char));
  // цикл до конца всех файлов
  double *t = linspace(0, 0.1, 8, true);
  double *carrier_wave = generate_carrier_wave(8, t, 80);
  double semi_carrier_wave_sum = 0;
  for (int i = 0; i < 4; ++i) {
    semi_carrier_wave_sum += carrier_wave[i];
    std::cout << semi_carrier_wave_sum << "\n";
  }
  std::vector<std::string> test = {"kotak", "helo", "nigga"};
  double *modulated = nullptr;
  for (int i = 0; i < test.size(); ++i) {
    memmove(ephir, test[i].c_str(), test[i].size());
    char ***encoded = encode(ephir, 1024, alphabet_for_all_users[i], number);
    if (modulated == nullptr) {
      modulated = bpsk_modulation(encoded, carrier_wave, number, 8);
    } else {
      double *temp = bpsk_modulation(encoded, carrier_wave, number, 8);
      interfere(modulated, temp, 8 * 8 * 1024 * number);
    }
    ephir = (char *)calloc(1024, sizeof(char));
  }
  fix_after_interfere(modulated, carrier_wave, semi_carrier_wave_sum, 8,
                      8 * 8 * 1024 * number);
  for (int i = 0; i < test.size(); ++i) {
    std::cout << decode(
                     bpsk_demodulation(modulated, carrier_wave,
                                       semi_carrier_wave_sum, 8, 1024, number),
                     1024, hadamard[i], number)
              << "\n";
  }
  // while (true) {
  //   int n = 0;
  //   bool end = false;
  //   double *modulated = nullptr, *temp = nullptr;
  //   for (int i = 0; i < users_num; ++i) {
  //     // если файл кончился
  //     if (files[i].eof()) {
  //       ++n;
  //       // все файлы кончились
  //       if (n == users_num) {
  //         end = true;
  //         break;
  //       }
  //     } else {
  //       // считываем пакет размером 1 Кб
  //       files[i].read(ephir, 1024);
  //       current_time = asctime(localtime(&rawtime));
  //       current_time.pop_back();
  //       std::cout << current_time << ": To ephir added 1 KB packet from user"
  //                 << i << "\n";

  //       // кодируем пакет
  //       char ***encoded =
  //           encode(ephir, 1024, alphabet_for_all_users[i], number);
  //       current_time = asctime(localtime(&rawtime));
  //       current_time.pop_back();
  //       std::cout << current_time << ": Message from user " << i
  //                 << " was encoded\n";

  //       // bpsk
  //       if (modulated == nullptr) {
  //         modulated = bpsk_modulation(encoded, carrier_wave, number, 8);
  //       } else {
  //         temp = bpsk_modulation(encoded, carrier_wave, number, 8);
  //         interfere(modulated, temp, 8 * 8 * 1024 * number);
  //       }

  //       current_time = asctime(localtime(&rawtime));
  //       current_time.pop_back();
  //       std::cout << current_time << ": Message from user " << i
  //                 << " was modulated\n";
  //       free(ephir);
  //       ephir = (char *)calloc(1024, sizeof(char));
  //       free(encoded);
  //     }
  //   }
  //   if (end) break;
  //   char ***demodulated =
  //       bpsk_demodulation(modulated, semi_carrier_wave_sum, 8, 1024, number);
  //   for (int i = 0; i < users_num; ++i) {
  //     // debpsk
  //     current_time = asctime(localtime(&rawtime));
  //     current_time.pop_back();
  //     std::cout << current_time << ": Message from user " << i
  //               << " was demodulated\n";

  //     // декодируем пакет
  //     char *decoded = decode(demodulated, 1024, hadamard[i], number);
  //     // std::cout << decoded << " " << i << "\n";
  //     current_time = asctime(localtime(&rawtime));
  //     current_time.pop_back();
  //     std::cout << current_time << ": Message from user " << i
  //               << " was decoded\n";

  //     // записываем получателю в файл
  //     output_files[i] << decoded;
  //     current_time = asctime(localtime(&rawtime));
  //     current_time.pop_back();
  //     std::cout << current_time << ": Receiver " << i
  //               << " got message from user " << i << "\n";
  //     free(decoded);
  //   }
  // }

  free(ephir);
  return 0;
}