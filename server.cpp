#include "cpp-httplib/httplib.h"
#include "shortener.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h> // 引入异步头文件

int main() {
    // 1. 初始化异步日志队列，队列大小 8192，后台开启 1 个线程写文件
    spdlog::init_thread_pool(8192, 1);
    
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("server.log", true); 
    
    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
    
    // 2. 创建异步 logger
    auto logger = std::make_shared<spdlog::async_logger>(
        "async_logger", sinks.begin(), sinks.end(), 
        spdlog::thread_pool(), spdlog::async_overflow_policy::block
    );
    
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);

    // 3. 启动 Web 服务
    httplib::Server svr;
    svr.Get("/create", handle_create);
    svr.Get(R"(/(\d+))", handle_redirect);

    spdlog::info("=====================================");
    spdlog::info("Server started at http://localhost:8080 (Async Log ON)");
    spdlog::info("=====================================");
    
    svr.listen("0.0.0.0", 8080);
    return 0;
}