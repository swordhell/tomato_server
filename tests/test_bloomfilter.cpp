#include "gtest/gtest.h"
#include "BloomFilter.h"
#include <string>
#include <cstdio> // for remove

// 测试添加和查询
TEST(BloomFilterTest, AddAndCheck) {
    BloomFilter bf(1000, 3); // 1000 bits, 3 hash functions

    bf.add("apple");
    bf.add("banana");
    bf.add("cherry");

    EXPECT_TRUE(bf.contains("apple"));
    EXPECT_TRUE(bf.contains("banana"));
    EXPECT_TRUE(bf.contains("cherry"));
    EXPECT_FALSE(bf.contains("durian")); // 没添加的元素可能为 false
}

// 测试序列化和反序列化
TEST(BloomFilterTest, SerializeAndDeserialize) {
    BloomFilter bf(1000, 3);

    bf.add("apple");
    bf.add("banana");

    const std::string filename = "bloom_test.bin";
    bf.save(filename);

    BloomFilter bf2(1000, 3);
    bf2.load(filename);

    EXPECT_TRUE(bf2.contains("apple"));
    EXPECT_TRUE(bf2.contains("banana"));
    EXPECT_FALSE(bf2.contains("cherry"));

    std::remove(filename.c_str()); // 清理测试文件
}

// 测试重复添加
TEST(BloomFilterTest, RepeatedAdd) {
    BloomFilter bf(1000, 3);
    bf.add("apple");
    bf.add("apple");

    EXPECT_TRUE(bf.contains("apple"));
}

// 测试大数量元素
TEST(BloomFilterTest, LargeNumberOfElements) {
    BloomFilter bf(10000, 5);

    for (int i = 0; i < 1000; ++i) {
        bf.add("item" + std::to_string(i));
    }

    for (int i = 0; i < 1000; ++i) {
        EXPECT_TRUE(bf.contains("item" + std::to_string(i)));
    }

    EXPECT_FALSE(bf.contains("not_added_item"));
}