#ifndef _t33nsy_TEMP_DEBPSK
#define _t33nsy_TEMP_DEBPSK

auto bpsk_demodulation(double* signal, double* carrier_wave,
                       double semi_carrier_wave_sum, int samples_per_bit,
                       int packet_size, int key_length) -> char***;

auto bpsk_demodulation(double* signal, double semi_carrier_wave_sum,
                       int samples_per_bit, int packet_size,
                       int key_length) -> char***;

#endif /* _t33nsy_TEMP_DEBPSK */
