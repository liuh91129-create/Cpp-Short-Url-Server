#include "shortener.h"
#include <iostream>
#include <mutex>
#include <spdlog/spdlog.h> // 引入日志库

// 连接本地 Redis
auto redis = sw::redis::Redis("tcp://127.0.0.1:6379");
std::mutex redis_mutex; 

void handle_create(const httplib::Request& req, httplib::Response& res) {
    if (!req.has_param("url")) {
        spdlog::warn("IP: {} 请求创建短链失败: 缺少 url 参数", req.remote_addr);
        res.status = 400;
        res.set_content("Error: missing url parameter", "text/plain; charset=utf-8");
        return;
    }

    std::string ip = req.remote_addr;
    std::string limit_key = "limit:" + ip;
    
    std::lock_guard<std::mutex> lock(redis_mutex);
    try {
        long long count = redis.incr(limit_key); 
        if (count == 1) {
            redis.expire(limit_key, 60); 
        }
        
        if (count > 5) {
            spdlog::warn("IP: {} 触发限流拦截！", ip); // 记录被限流的 IP
            res.status = 429; 
            res.set_content("Too Many Requests! 请求太频繁，请1分钟后再试。", "text/plain; charset=utf-8");
            return;
        }
    } catch (const std::exception& e) {
        spdlog::error("Redis 限流发生异常: {}", e.what());
        res.status = 500;
        res.set_content("Redis Error", "text/plain; charset=utf-8");
        return;
    }

    std::string long_url = req.get_param_value("url");

    if (long_url.find("http://") != 0 && long_url.find("https://") != 0) {
        long_url = "http://" + long_url;
    }

    try {
        long long id = redis.incr("short_url_id"); 
        std::string short_code = std::to_string(id);
        
        redis.set(short_code, long_url);
        
        spdlog::info("IP: {} 成功创建短链 [{}] -> {}", ip, short_code, long_url); // 记录成功日志
        res.set_content("Success! Your short URL is: http://localhost:8080/" + short_code, "text/plain; charset=utf-8");
    } catch (const std::exception& e) {
        spdlog::error("创建短链发生 Redis 异常: {}", e.what());
        res.status = 500;
        res.set_content("Redis Error: " + std::string(e.what()), "text/plain; charset=utf-8");
    }
}

void handle_redirect(const httplib::Request& req, httplib::Response& res) {
    std::string code = req.matches[1];
    
    std::lock_guard<std::mutex> lock(redis_mutex);
    
    try {
        auto long_url = redis.get(code);
        if (long_url) {
            spdlog::info("IP: {} 访问短链 [{}] 跳转至 {}", req.remote_addr, code, *long_url);
            res.set_redirect(*long_url);
        } else {
            spdlog::warn("IP: {} 访问短链 [{}] 失败: 未找到", req.remote_addr, code);
            res.status = 404;
            res.set_content("Short URL not found", "text/plain; charset=utf-8");
        }
    } catch (const std::exception& e) {
        spdlog::error("访问短链发生 Redis 异常: {}", e.what());
        res.status = 500;
        res.set_content("Redis Error: " + std::string(e.what()), "text/plain; charset=utf-8");
    }
}