#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <functional>
#include <cstdint>

class BloomFilter {
public:
    BloomFilter(size_t num_bits, size_t num_hashes);

    // 添加元素
    void add(const std::string &item);

    // 检查元素是否存在
    bool contains(const std::string &item) const;

    // 测试并添加，返回之前是否存在
    bool testAndAdd(const std::string &item);

    // 序列化到文件
    bool save(const std::string &filename) const;

    // 从文件加载
    bool load(const std::string &filename);

private:
    size_t hash1(const std::string &item) const;
    size_t hash2(const std::string &item) const;

    size_t num_bits_;
    size_t num_hashes_;
    std::vector<uint8_t> bitset_;

    void setBit(size_t pos);
    bool getBit(size_t pos) const;
};