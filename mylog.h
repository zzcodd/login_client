/*
 * @Description: 
 * @Author: zy
 * @Date: 2024-07-02 09:28:02
 * @LastEditTime: 2024-07-02 14:28:57
 * @LastEditors: zy
 */
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <bits/stdc++.h>

#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
// 我们将设计一个日志管理类`LogManager`，该类包含三个主要功能：
// 1. **记录用户活动**：包括用户注册和登录。
// 2. **记录系统错误**：包括详细的错误描述和堆栈跟踪信息。
// 3. **记录安全事件**：包括多次失败的登录尝试等。

class LogManager {

 public:
  //初始化日志系统
  static void init() {
    try{
      auto logger = spdlog::basic_logger_mt("zy_logger","log_register_server.log");
      spdlog::set_default_logger(logger);
      spdlog::set_pattern("[%Y-%m-%d %H:%M:%S][%l]%v");
      spdlog::flush_every(std::chrono::seconds(5));// 定期为所有注册的logger隔5秒刷新
      
    }catch(const spdlog::spdlog_ex& ex) {
      std::cerr << "日志系统初始化失败 " << ex.what() << std::endl;
    }
  }
  //记录用户活动
  static void logUserActivity(const std::string& userId,const std::string& activity, const std::string& status) {
    std::string ip = getIpAddress();
    spdlog::info("UserId: {} | Activity: {} | status: {} | IP: {}",userId, activity, status, ip);
  }
  //记录系统错误
  static void logError(const std::string& error) {
    spdlog::error("Error: {}", error);
  }
  //记录安全事件
  static void logSecurityEvent(const std::string& userId,const std::string& activity) {

    std::string ip = getIpAddress();
    spdlog::warn("UserId:{} | Activity:{} | IP:{}",userId,activity,ip);
  }

 private:

  static std::string getIpAddress() {
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    void *tmpAddrPtr = NULL;
    std::string localIp = "Unknown";

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            // You can add conditions to select specific interface
            if (std::string(ifa->ifa_name) == "enp0s31f6" || std::string(ifa->ifa_name) == "lo") {
                localIp = addressBuffer;
                break; // Stop after finding the first valid IP
            }
        }
    }
    if (ifAddrStruct != NULL) freeifaddrs(ifAddrStruct);
    return localIp;
  }



};