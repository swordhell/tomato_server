// include/ProtocolParser.h
#pragma once
#include <vector>
#include <queue>
#include <cstdint>

class ProtocolParser {
public:
    ProtocolParser() = default;

    void appendData(const std::vector<uint8_t>& data);

    // 返回已解析完整消息
    std::queue<std::vector<uint8_t>> parse();

private:
    std::vector<uint8_t> buffer_;
};