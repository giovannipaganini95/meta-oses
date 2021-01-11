#ifndef HEARTBEAT_APP_CIRCULAR_BUFFER_HPP
#define HEARTBEAT_APP_CIRCULAR_BUFFER_HPP

#include <array>
#include <mutex>
#include <condition_variable>

template<typename Type, std::size_t Capacity>
class circular_buffer {
public:

    circular_buffer() : full_(false), head_(0), tail_(0) {};

    [[nodiscard]] constexpr std::size_t capacity() const {
        return Capacity;
    }

    std::size_t size() {
        std::lock_guard lg{mtx_};
        if (full_) return Capacity;
        else if (tail_ >= head_) return (tail_ - head_);
        else return (head_ - tail_);
    }

    bool empty() {
        std::lock_guard lg{mtx_};
        return (!full_ && tail_ == head_);
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mtx_);
        head_ = tail_;
        full_ = false;
    }

    void push(Type data) {
        std::lock_guard lg{mtx_};
        buffer_[tail_] = data;
        tail_ = (tail_ + 1) % Capacity;
        full_ = tail_ == head_;
        cv_.notify_one();
    }

    Type pop() {
        std::unique_lock ul{mtx_};
        cv_.wait(ul, [this]() { return (full_ || head_ != tail_); });
        auto tmp = buffer_[head_];
        full_ = false;
        head_ = (head_ + 1) % Capacity;
        return tmp;
    }

private:
    std::mutex mtx_;
    bool full_;
    std::size_t head_;
    std::size_t tail_;

    std::condition_variable cv_;
    std::array<Type, Capacity> buffer_;
};

#endif //HEARTBEAT_APP_CIRCULAR_BUFFER_HPP
