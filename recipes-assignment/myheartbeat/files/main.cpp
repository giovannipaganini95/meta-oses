#include "producer.hpp"
#include "consumer.hpp"
#include "circular_buffer.hpp"

int main() {
    circular_buffer<unsigned int, 4> buffer;
    producer p(20000, buffer);
    consumer c(buffer);
    return 0;
}
