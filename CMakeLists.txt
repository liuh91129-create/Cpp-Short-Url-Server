cmake_minimum_required(VERSION 3.10)
project(ShortUrlServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 暴力查找 hiredis 的头文件和库文件
find_path(HIREDIS_INCLUDE_DIR hiredis/hiredis.h)
find_library(HIREDIS_LIBRARY hiredis)

# 2. 暴力查找 redis++ 的头文件和库文件
find_path(REDIS_PLUS_PLUS_INCLUDE_DIR sw/redis++/redis++.h)
find_library(REDIS_PLUS_PLUS_LIBRARY redis++)

# 3. 查找 spdlog
find_package(spdlog REQUIRED)

# 检查是否找齐了
if(NOT HIREDIS_INCLUDE_DIR OR NOT HIREDIS_LIBRARY)
    message(FATAL_ERROR "Could not find hiredis")
endif()
if(NOT REDIS_PLUS_PLUS_INCLUDE_DIR OR NOT REDIS_PLUS_PLUS_LIBRARY)
    message(FATAL_ERROR "Could not find redis++")
endif()

add_executable(server server.cpp shortener.cpp)

# 4. 包含头文件目录
target_include_directories(server PRIVATE 
    ${HIREDIS_INCLUDE_DIR} 
    ${REDIS_PLUS_PLUS_INCLUDE_DIR}
)

# 5. 链接库
target_link_libraries(server PRIVATE 
    ${REDIS_PLUS_PLUS_LIBRARY} 
    ${HIREDIS_LIBRARY} 
    spdlog::spdlog 
    pthread
)