// src/ProtocolParser.cpp
#include "ProtocolParser.h"
#include <cstring>

void ProtocolParser::appendData(const std::vector<uint8_t>& data) {
    buffer_.insert(buffer_.end(), data.begin(), data.end());
}

std::queue<std::vector<uint8_t>> ProtocolParser::parse() {
    std::queue<std::vector<uint8_t>> messages;
    size_t offset = 0;

    while (buffer_.size() - offset >= sizeof(uint32_t)) {
        uint32_t len;
        std::memcpy(&len, buffer_.data() + offset, sizeof(uint32_t));

        if (buffer_.size() - offset >= sizeof(uint32_t) + len) {
            std::vector<uint8_t> msg(len);
            std::memcpy(msg.data(), buffer_.data() + offset + sizeof(uint32_t), len);
            messages.push(std::move(msg));
            offset += sizeof(uint32_t) + len;
        } else {
            break;
        }
    }

    if (offset > 0) buffer_.erase(buffer_.begin(), buffer_.begin() + offset);
    return messages;
}