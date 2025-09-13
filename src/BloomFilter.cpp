#include "BloomFilter.h"
#include <cmath>
#include <fstream>

BloomFilter::BloomFilter(size_t num_bits, size_t num_hashes)
    : num_bits_(num_bits), num_hashes_(num_hashes), bitset_((num_bits + 7) / 8, 0) { }

void BloomFilter::add(const std::string &item) {
    size_t h1_val = hash1(item);
    size_t h2_val = hash2(item);

    for (size_t i = 0; i < num_hashes_; ++i) {
        size_t pos = (h1_val + i * h2_val) % num_bits_;
        setBit(pos);
    }
}

bool BloomFilter::contains(const std::string &item) const {
    size_t h1_val = hash1(item);
    size_t h2_val = hash2(item);

    for (size_t i = 0; i < num_hashes_; ++i) {
        size_t pos = (h1_val + i * h2_val) % num_bits_;
        if (!getBit(pos)) return false;
    }
    return true;
}

bool BloomFilter::testAndAdd(const std::string &item) {
    bool exists = contains(item);
    if (!exists) add(item);
    return exists;
}

size_t BloomFilter::hash1(const std::string &item) const {
    return std::hash<std::string>{}(item);
}

size_t BloomFilter::hash2(const std::string &item) const {
    // 简单第二哈希，可改为更复杂的FNV等
    size_t hash = 0xcbf29ce484222325;
    for (char c : item) {
        hash ^= static_cast<size_t>(static_cast<unsigned char>(c));
        hash *= 0x100000001b3;
    }
    return hash;
}

void BloomFilter::setBit(size_t pos) {
    bitset_[pos / 8] |= (1 << (pos % 8));
}

bool BloomFilter::getBit(size_t pos) const {
    return bitset_[pos / 8] & (1 << (pos % 8));
}

bool BloomFilter::save(const std::string &filename) const {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) return false;
    ofs.write(reinterpret_cast<const char*>(bitset_.data()),  static_cast<std::streamsize>(bitset_.size()));
    ofs.write(reinterpret_cast<const char*>(&num_bits_), sizeof(num_bits_));
    ofs.write(reinterpret_cast<const char*>(&num_hashes_), sizeof(num_hashes_));
    return true;
}

bool BloomFilter::load(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) return false;

    std::vector<uint8_t> new_bitset((num_bits_ + 7) / 8, 0);
    ifs.read(reinterpret_cast<char*>(new_bitset.data()),  static_cast<std::streamsize>(new_bitset.size()));
    ifs.read(reinterpret_cast<char*>(&num_bits_), sizeof(num_bits_));
    ifs.read(reinterpret_cast<char*>(&num_hashes_), sizeof(num_hashes_));
    bitset_ = std::move(new_bitset);
    return true;
}