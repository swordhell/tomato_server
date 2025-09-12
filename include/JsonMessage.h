// include/JsonMessage.h
#pragma once
#include "IMessage.h"
#include <nlohmann/json.hpp>

class JsonMessage : public IMessage {
public:
    JsonMessage() = default;
    explicit JsonMessage(const nlohmann::json& j) : json_(j) {}

    bool deserialize(const std::vector<uint8_t>& data) override {
        try {
            json_ = nlohmann::json::parse(data.begin(), data.end());
            return true;
        } catch (...) {
            return false;
        }
    }

    std::vector<uint8_t> serialize() const override {
        auto s = json_.dump();
        return std::vector<uint8_t>(s.begin(), s.end());
    }

    std::string type() const override { return "json"; }

    nlohmann::json& get() { return json_; }

private:
    nlohmann::json json_;
};