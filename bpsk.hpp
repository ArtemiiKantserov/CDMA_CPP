#ifndef _t33nsy_BPSK
#define _t33nsy_BPSK
#include <random>

/**
 * @file bpsk.hpp
 * @brief BPSK демодуляция
 * @param demodulated память для демодулированной строки
 * @param signal сигнал для демодулирования
 * @param semi_carrier_wave_sum сумма половины амплитуды несущей волны
 * @param samples_per_bit ширина сигнала
 * @param packet_size размер пакета
 * @param key_length длина ключа
 */
auto bpsk_demodulation(char* demodulated, double* signal,
                       double semi_carrier_wave_sum, int samples_per_bit,
                       int packet_size, int key_length) -> void;

/**
 * @brief аналог функции linspace из numpy
 * @param start начало интервала
 * @param end конец интервала
 * @param count количество интервалов
 * @param endpoint включается или не включается конечная точка
 */
auto linspace(double start, double end, int count, bool endpoint) -> double*;

/**
 * @brief генератор несущей волны
 * @param count количество точек несущей волны
 * @param arr массив точек
 * @param carrier_freq частота несущей волны
 * @return массив точек несущей волны
 */
auto generate_carrier_wave(int count, double* arr, int carrier_freq) -> double*;

/**
 * @brief BPSK модуляция сигнала
 * @param space память для модуляции
 * @param bits значения, которые необходимо модулировать
 * @param carrier_wave массив точек несущей волны
 * @param len_hadamard длина строки матрицы Адамара
 * @param len_wave длина массива точек несущей волны
 * @param min_noise минимальное значение генерируемого шума
 * @param max_noise максимальное значение генерируемого шума
 */
auto bpsk_modulation(double* space, char* bits, double* carrier_wave,
                     int len_hadamard, int len_wave, double min_noise,
                     double max_noise) -> void;

/**
 * @brief интерференция волн
 * @param first первая волна
 * @param second вторая волна
 * @param len длина волн
 */
auto interfere(double* first, double* second, int len) -> void;

/**
 * @brief генератор шума
 * @param min минимальное значение генерируемого шума
 * @param max максимальное значение генерируемого шума
 * @return генерируемое значение шума
 */
auto noise(double min, double max) -> double;
#endif /* _t33nsy_BPSK */
