#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class LogicProcessor {
public:
    using Task = std::function<void()>;

    explicit LogicProcessor(size_t thread_count);
    ~LogicProcessor();

    void post(Task task);

private:
    void workerThread();

    std::vector<std::thread> workers_;
    std::queue<Task> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_;
};