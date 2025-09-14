# 📘 FlatBuffers 概述文档

1. 背景

在高性能系统（如游戏引擎、金融交易所撮合、实时推送服务器）中，传统的数据交换格式（JSON、XML）存在以下问题：
•	解析开销大：需要序列化 / 反序列化。
•	内存占用高：反序列化会生成大量临时对象。
•	延迟敏感：每一次内存拷贝都会造成性能瓶颈。

为解决这些问题，Google 开发了 FlatBuffers —— 一种高效、零拷贝的序列化库。

⸻

2. 什么是 FlatBuffers

FlatBuffers 是 Google 出品的 跨平台序列化库，它的特点是：
•	零拷贝访问：无需反序列化，直接在二进制缓冲区上访问数据。
•	跨语言支持：C++, Go, Rust, Java, Python, C#, Swift 等。
•	向后兼容：数据结构升级不会破坏旧版本兼容性。
•	轻量级：适合资源受限的系统（嵌入式 / 移动端）。

⸻

3. 工作原理

与 Protobuf 相比，FlatBuffers 不需要先反序列化为对象，而是 通过偏移量（offset）直接读取内存中的数据：
1.	定义 schema（类似 Protobuf 的 .proto 文件）。
2.	编译 schema → 生成代码。
3.	构建数据 → 写入 FlatBuffer（一次性）。
4.	读取数据 → 直接在 buffer 上访问字段，无需拷贝。

⸻

4. 优势

🚀 性能优势
•	零拷贝：数据无需反序列化。
•	低延迟：直接访问内存指针。
•	低内存占用：只存一份 buffer，无需额外对象。

⚡ 实用性优势
•	跨平台、跨语言，适合分布式系统。
•	兼容性强，可增量升级 schema。
•	适用于高频场景：实时推送、行情系统、游戏同步。

⸻

5. 与 Protobuf 对比

特性	Protobuf	FlatBuffers
序列化速度	快	快（一次性写入）
反序列化速度	慢（需解码）	极快（零拷贝）
内存占用	高	低
消息体积	小（更紧凑）	略大（offset 额外开销）
API 生态	成熟，gRPC 默认支持	生态小，需额外集成
使用复杂度	简单直观	偏底层，需理解 offset
典型场景	微服务通信	高性能系统，嵌入式、游戏、金融


⸻

6. 应用场景
   •	游戏服务器：玩家状态同步，帧数据传输。
   •	交易所系统：撮合引擎、行情推送（tick-level 数据）。
   •	IoT 系统：传感器数据流，实时处理。
   •	嵌入式设备：资源受限环境，减少内存占用。

⸻

7. 示例代码（C++）

Schema (trade.fbs)：

namespace Exchange;

table Trade {
id:string;
price:double;
volume:double;
ts:ulong;
}

root_type Trade;

写入数据：

flatbuffers::FlatBufferBuilder builder(1024);

auto id = builder.CreateString("BTCUSDT");
Exchange::TradeBuilder trade(builder);
trade.add_id(id);
trade.add_price(45678.90);
trade.add_volume(0.1234);
trade.add_ts(1694512345);
auto tradeOffset = trade.Finish();

builder.Finish(tradeOffset);
uint8_t* buf = builder.GetBufferPointer();
int size = builder.GetSize();

// buf 可直接发送到网络

读取数据：

auto trade = Exchange::GetTrade(buf);
std::cout << "ID: " << trade->id()->str() << "\n";
std::cout << "Price: " << trade->price() << "\n";


⸻

8. 总结
   •	FlatBuffers 是 Google 提供的高性能序列化解决方案。
   •	适合 低延迟、高吞吐量 的系统（游戏、金融、IoT）。
   •	相比 Protobuf，性能更好但生态略弱。
   •	可在 内部通信 采用 FlatBuffers，对外接口 采用 Protobuf/gRPC，兼顾性能与生态。

⸻

* [https://flatbuffers.dev/](https://flatbuffers.dev/)
