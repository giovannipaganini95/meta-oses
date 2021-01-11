#ifndef HEARTHBEAT_APP_PRODUCER_HPP
#define HEARTHBEAT_APP_PRODUCER_HPP

#include <thread>
#include <optional>
#include <iostream>

#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "circular_buffer.hpp"

#define DEVICE_NAME "/dev/ppg_dev"

class producer {
public:
    template<typename Type, std::size_t Capacity>
    producer(int sleep_us, circular_buffer<Type, Capacity> &buff) :
            fd(-1), running(true) {
        if ((fd = open(DEVICE_NAME, O_RDWR)) < 0) {
            std::cout << "Cannot open " << DEVICE_NAME << std::endl;
            exit(-1);
        }
        thread.emplace([&, sleep_us]() {
            int tmp_data;
            while (running) {
                if (read(fd, &tmp_data, 4) != 4) {
                    std::cout << "Read error" << std::endl;
                } else {
                    buff.push(tmp_data);
                }
                std::this_thread::sleep_for(std::chrono::microseconds(sleep_us));
            }
        });
    }

    ~producer() {
        if (thread->joinable())
            thread->join();
        close(fd);
    }

private:
    int fd;
    bool running;
    std::optional<std::thread> thread;
};

#endif //HEARTHBEAT_APP_PRODUCER_HPP