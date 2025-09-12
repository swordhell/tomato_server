cmake_minimum_required(VERSION 3.16)
project(tomato_server VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(boost_asio REQUIRED CONFIG)
find_package(unofficial-libmysql REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log log_setup regex thread filesystem date_time)


# 默认构建类型 (Debug/Release/RelWithDebInfo/MinSizeRel)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# 编译器警告和优化选项
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    message(STATUS "Using GCC/Clang family compiler")

    add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wconversion
            -Wsign-conversion
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
    )

    # Debug: 关闭优化, 开启调试符号
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")

    # Release: 开启优化, 关闭断言
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

elseif (MSVC)
    message(STATUS "Using MSVC compiler")

    add_compile_options(
            /W4            # 高等级警告
            /permissive-   # 更严格的标准
            /Zc:__cplusplus
    )
    # MSVC 默认会自动处理 Debug/Release 优化参数
endif()

include_directories(
        ${PROJECT_SOURCE_DIR}/include
)
# 递归查找所有源文件（.cpp）
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        src/*.cpp
)

# 递归查找所有头文件（.h）
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        include/*.h
)

# 生成可执行文件
add_executable(${PROJECT_NAME}
        main.cpp
        ${SOURCES}
        ${HEADERS}
        include/IMessage.h
        src/ProtocolParser.cpp
        include/ProtocolParser.h
)

# -------------------- 链接库 --------------------
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::asio pthread unofficial::libmysql::libmysql )
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp::yaml-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::log)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Boost::log
        Boost::log_setup
        Boost::regex
        Boost::filesystem
        Boost::date_time
)
if(APPLE)
    message(STATUS "Configuring for macOS")
    target_link_libraries(${PROJECT_NAME} PRIVATE resolv)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Configuring for Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE dl resolv)
endif()
