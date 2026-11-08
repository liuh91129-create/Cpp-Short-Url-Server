#pragma once
#include "cpp-httplib/httplib.h"
#include <sw/redis++/redis++.h>   // 🌟 核心就是缺了这一行！必须让编译器认识 Redis

// 声明两个处理函数
void handle_create(const httplib::Request& req, httplib::Response& res);
void handle_redirect(const httplib::Request& req, httplib::Response& res);