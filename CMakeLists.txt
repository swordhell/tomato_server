cmake_minimum_required(VERSION 3.16)
project(tomato_server VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(boost_asio REQUIRED CONFIG)
find_package(unofficial-libmysql REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system log log_setup regex thread filesystem date_time)


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
add_executable(tomato_server
        main.cpp
        ${SOURCES}
        ${HEADERS}
)

# -------------------- 链接库 --------------------
target_link_libraries(tomato_server PRIVATE Boost::asio pthread unofficial::libmysql::libmysql )
target_link_libraries(tomato_server PRIVATE yaml-cpp::yaml-cpp)
target_link_libraries(tomato_server PRIVATE Boost::log)
target_link_libraries(tomato_server
        PRIVATE
        Boost::system
        Boost::log
        Boost::log_setup
        Boost::regex
        Boost::thread
        Boost::filesystem
        Boost::date_time
)
