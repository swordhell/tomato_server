// include/MessageDispatcher.h
#pragma once
#include "IMessage.h"
#include <functional>
#include <unordered_map>
#include <string>
#include <memory>

class MessageDispatcher {
public:
    using Handler = std::function<void(std::shared_ptr<IMessage>)>;

    void registerHandler(const std::string& type, Handler handler) {
        handlers_[type] = std::move(handler);
    }

    void dispatch(std::shared_ptr<IMessage> msg) {
        auto it = handlers_.find(msg->type());
        if (it != handlers_.end()) {
            it->second(msg);
        }
    }

private:
    std::unordered_map<std::string, Handler> handlers_;
};