#include "LogicProcessor.h"

LogicProcessor::LogicProcessor(size_t thread_count) : stop_(false) {
    for (size_t i = 0; i < thread_count; ++i) {
        workers_.emplace_back([this] { this->workerThread(); });
    }
}

LogicProcessor::~LogicProcessor() {
    {
        std::unique_lock<std::mutex> lock(mtx_);
        stop_ = true;
        cv_.notify_all();
    }
    for (auto& t : workers_) t.join();
}

void LogicProcessor::post(Task task) {
    {
        std::unique_lock<std::mutex> lock(mtx_);
        tasks_.push(std::move(task));
    }
    cv_.notify_one();
}

void LogicProcessor::workerThread() {
    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this]{ return stop_ || !tasks_.empty(); });
            if (stop_ && tasks_.empty()) return;
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        task();
    }
}