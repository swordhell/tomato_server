// include/IMessage.h
#pragma once
#include <vector>
#include <string>

class IMessage {
public:
    virtual ~IMessage() = default;

    // 从字节数组解析
    virtual bool deserialize(const std::vector<uint8_t>& data) = 0;

    // 转为字节数组发送
    virtual std::vector<uint8_t> serialize() const = 0;

    // 可选：消息类型标识
    virtual std::string type() const = 0;
};