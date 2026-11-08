#include "cpp-httplib/httplib.h"
#include "shortener.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

int main() {
    // 1. 配置双通道日志：控制台输出 + 文件输出
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("server.log", true); // true表示追加写入
    
    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info); // 设置日志级别为 info

    // 2. 启动 Web 服务
    httplib::Server svr;

    svr.Get("/create", handle_create);
    svr.Get(R"(/(\d+))", handle_redirect);

    spdlog::info("=====================================");
    spdlog::info("Server started at http://localhost:8080");
    spdlog::info("=====================================");
    
    svr.listen("0.0.0.0", 8080);
    return 0;
}