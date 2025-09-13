# 🍅 Tomato Server

[![C++](https://img.shields.io/badge/C++-17-blue?logo=c%2B%2B)](https://isocpp.org/)  
[![Boost](https://img.shields.io/badge/Boost-1.82-lightgrey?logo=boost)](https://www.boost.org/)  
[![MySQL](https://img.shields.io/badge/MySQL-8.0-blue?logo=mysql)](https://www.mysql.com/)  
[![License](https://img.shields.io/badge/License-MIT-green)](LICENSE)

---

## 📖 Overview

**Tomato Server** is a modern, modular TCP server framework written in **C++17**, designed for high-performance network
applications.  
It emphasizes clean architecture, asynchronous operations, and easy extensibility.

Key features:

- Multi-threaded, asynchronous network communication using **Boost.Asio**
- Thread-safe **MySQL connection pool** for database operations
- Modular protocol layer supporting **JSON**, **Protobuf**, and **FlatBuffers**
- Flexible message dispatcher and logic processor
- Centralized logging using **Boost.Log**
- Configuration via **YAML** files

---

## 📁 Project Structure

```txt
tomato_server/
├─ CMakeLists.txt
├─ README.md
├─ config.yaml
├─ include/
│   ├─ IMessage.h
│   ├─ JsonMessage.h
│   ├─ ProtoMessage.h
│   ├─ FlatMessage.h
│   ├─ ProtocolParser.h
│   ├─ MessageDispatcher.h
│   ├─ Session.h
│   ├─ Server.h
│   ├─ MySQLPool.h
│   ├─ LogicProcessor.h
│   ├─ Logger.h
│   └─ ConfigLoader.h
├─ src/
├─ cpp/
└─ third_party/
```

---

## 🔹 Architecture

```txt
+------------------+
|      Network     |
|  (Server/Session)|
+------------------+
         |
         v
+------------------+
|     Protocol     |
| (Parser/Message) |
+------------------+
         |
         v
+------------------+
|       Logic      |
| (Dispatcher/Logic)|
+------------------+
         |
         v
+------------------+
|     Database     |
|   (MySQL Pool)   |
+------------------+
```

- **Network Layer**: Manages TCP connections and reads/writes asynchronously
- **Protocol Layer**: Handles packet parsing, sticky packet splitting, and message serialization/deserialization
- **Logic Layer**: Dispatches messages to handlers, processes business logic
- **Database Layer**: Provides thread-safe MySQL connection pool and async queries

---

## ⚡ Technology Stack

| Component                  | Purpose                                        |
|----------------------------|------------------------------------------------|
| **C++17**                  | Modern language features, smart pointers, RAII |
| **Boost.Asio**             | High-performance asynchronous network I/O      |
| **Boost.Log**              | Thread-safe logging                            |
| **MySQL**                  | Persistent data storage with connection pool   |
| **YAML-CPP**               | Configuration management                       |
| **Protobuf / FlatBuffers** | Efficient message serialization                |
| **CMake**                  | Build system, dependency management            |

---

## 🚀 Quick Start

1. **Install dependencies** (Boost, MySQL, YAML-CPP, etc.)
2. **Build the server**:

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

3. Run the server:

`./tomato_server`

4. Configure the server via config.yaml

⸻

🔧 Notes
• Fully thread-safe design using std::mutex and modern C++ concurrency features
• Asynchronous database operations via std::future / std::promise
• Protocol layer supports JSON, Protobuf, FlatBuffers interchangeably
• Easily extendable: add new message types or business logic without touching network or database code

```mermaid
sequenceDiagram
autonumber
participant Client
participant Gateway
participant ThreadPool as Gateway ThreadPool
participant EventQueue as Gateway EventQueue
participant AuthServer
participant GameServer
participant DBServer

    %% 客户端登录流程
    Client->>Gateway: 发送登录请求(username/password)
    Gateway->>ThreadPool: 分配线程处理请求
    ThreadPool->>EventQueue: 将请求放入事件队列
    EventQueue->>AuthServer: 异步验证用户信息
    AuthServer-->>EventQueue: 返回验证结果
    EventQueue->>ThreadPool: 分发处理结果
    ThreadPool-->>Gateway: 返回登录结果
    Gateway-->>Client: 登录成功或失败

    %% 游戏操作流程
    Client->>Gateway: 发送游戏操作请求
    Gateway->>ThreadPool: 分配线程处理请求
    ThreadPool->>EventQueue: 将请求放入事件队列
    EventQueue->>GameServer: 处理游戏逻辑
    GameServer->>DBServer: 查询/更新数据
    DBServer-->>GameServer: 返回查询/更新结果
    GameServer-->>EventQueue: 游戏逻辑处理完成
    EventQueue->>ThreadPool: 分发处理结果
    ThreadPool-->>Gateway: 返回操作结果
    Gateway-->>Client: 返回操作结果

    %% 异步事件示意
    Note over EventQueue, ThreadPool: 事件队列解耦线程池与逻辑处理
    Note over GameServer, DBServer: 数据库操作可异步或批量处理
    Note over Gateway, Client: 支持多客户端并发连接
```

```mermaid
%% =================== 类结构 ===================
classDiagram
    class Server {
        +start()
        +stop()
        -acceptConnections()
        -handleClient(ClientConnection)
        -heartbeatMonitor()
    }

    class ClientConnection {
        +send(Message)
        +receive() Message
        -socket
        -lastActiveTime
        -clientId
    }

    class ThreadPool {
        +enqueue(Task)
        -workers
        -taskQueue
    }

    class Task {
        +execute()
    }

    class EventQueue {
        +push(Event)
        +pop() Event
    }

    class GameLogicProcessor {
        +processEvent(Event)
        -handleGameState(Event)
        -validateAction(Event)
    }

    class DBServer {
        +executeQuery(SQL)
        +executeAsync(SQL)
        -connectionPool
    }

    class Logger {
        +logInfo(string)
        +logError(string)
    }

    class MetricsCollector {
        +collectMetric(Metric)
        +exportMetrics()
    }

    class Message {
        +header
        +body
        +serialize()
        +deserialize()
    }

    class ClientManager {
        +registerClient(ClientConnection)
        +removeClient(ClientConnection)
        +getClientById(id)
    }

    Server --> ThreadPool : uses
    Server --> EventQueue : produces
    Server --> ClientManager : manages
    ThreadPool --> Task : executes
    EventQueue --> GameLogicProcessor : consumes
    GameLogicProcessor --> DBServer : queries
    GameLogicProcessor --> Logger : logs
    DBServer --> Logger : logs
    Server --> MetricsCollector : reports
    GameLogicProcessor --> MetricsCollector : reports
    ClientConnection --> Message : send/receive
```
```mermaid
%% =================== 时序图 ===================
sequenceDiagram
    participant Client
    participant Server
    participant ThreadPool
    participant EventQueue
    participant GameLogicProcessor
    participant DBServer
    participant Logger
    participant MetricsCollector
    participant ClientManager

    Client->>Server: connect()
    Server->>ClientManager: registerClient()
    Server->>ThreadPool: enqueue(handleClient)
    ThreadPool->>Server: runTask(handleClient)
    Server->>EventQueue: push(clientMessage)
    EventQueue->>GameLogicProcessor: pop()
    GameLogicProcessor->>DBServer: executeAsync(query)
    DBServer-->>GameLogicProcessor: queryResult
    GameLogicProcessor->>Client: send(response)
    GameLogicProcessor->>Logger: logInfo(event)
    GameLogicProcessor->>MetricsCollector: reportMetric(event)
    DBServer->>Logger: logInfo(query)
    MetricsCollector->>Server: exportMetrics()
    Server->>Client: send(heartbeat)
    Client-->>Server: heartbeat_ack
```



⸻

📜 License

MIT License © 2025

