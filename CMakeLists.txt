cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0167 NEW)
project(tomato_server VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------- 编译器选项 ----------------
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    add_compile_options(
            -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor
            -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual
            -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion
            -Wformat=2
    )
elseif(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
endif()

# ---------------- 输出目录 ----------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
link_directories(${CMAKE_BINARY_DIR}/lib)
# ---------------- 查找依赖 ----------------
find_package(boost_asio REQUIRED CONFIG)
find_package(unofficial-libmysql REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log log_setup regex thread filesystem date_time)

# ---------------- 递归查找源文件 ----------------
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.h)

# ---------------- 可执行文件 ----------------

add_executable(${PROJECT_NAME} main.cpp ${SOURCES} ${HEADERS})
# 顶层 include 目录
target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/mysql_pool/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Boost::asio Boost::log_setup Boost::regex
        spdlog::spdlog fmt::fmt yaml-cpp::yaml-cpp
        unofficial::libmysql::libmysql
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE resolv)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl resolv)
endif()

# ---------------- 单元测试 -----------