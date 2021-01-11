#ifndef HEARTBEAT_APP_CONSUMER_HPP
#define HEARTBEAT_APP_CONSUMER_HPP

#include <array>
#include <thread>
#include <iostream>
#include <optional>

#include "fft_function.hpp"
#include "circular_buffer.hpp"

#define N    2048        /* N-point FFT, iFFT */

class consumer {
public:
    template<typename Type, std::size_t Capacity>
    consumer(circular_buffer<Type, Capacity> &buff) :
            running(true) {
        thread.emplace(
                [&]() {
                    int offset = 0;
                    int k, m, minIdx, maxIdx, max_size;
                    std::array<complex, N> array{}, scratch{};
                    std::array<float, N> abs{};
                    while (running) {
                        Type element = buff.pop();
                        int size = buff.size();
                        if (size > max_size) max_size = size;
                        array[offset].Re = element;
                        array[offset].Im = 0;
                        offset++;
                        if (offset >= N) {
                            //auto start = std::chrono::steady_clock::now();
                            fft(array.data(), N, scratch.data());
                            offset = 0;
                            for (k = 0; k < N; k++) {
                                abs[k] = (50.0 / N) * ((array[k].Re * array[k].Re) + (array[k].Im * array[k].Im));
                            }

                            minIdx = (N / 2) / 50;
                            maxIdx = (N * 3) / 50;
                            m = minIdx;
                            for (k = minIdx; k < (maxIdx); k++) {
                                if (abs[k] > abs[m])
                                    m = k;
                            }
                            std::cout << "Computed Value: " << std::to_string((m) * 60 * 50 / N) << "bpm" << std::endl;

                            /* std::cout << "Max size: " << std::to_string(max_size) << std::endl;
                            auto now = std::chrono::steady_clock::now();
                            auto duration = std::chrono::duration_cast<std::chrono::microseconds>
                                    (now - start);
                            start = now;
                            std::cout << duration.count() << std::endl;
                            max_size = 0; */
                            offset = 0;
                        }
                    }
                });
    }

    ~consumer() {
        if (thread->joinable())
            thread->join();
    }

private:
    bool running;
    std::optional<std::thread> thread;

};

#endif //HEARTBEAT_APP_CONSUMER_HPP