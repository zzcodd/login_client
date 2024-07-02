#include "log_mysql.h"
#include <bits/stdc++.h>

#include "mylog.h"

bool log_Register();
bool log_Login();
bool log_Logout();
bool change_UserName();
bool change_Passwd();

Log_Mysql my;



int main() {

  LogManager::init();

  int flag = 1;
  bool isLogin = false;
  while(flag) {
    std::cout << "====================================" << std::endl;

    int index;
    std::cout << "1-注册  " <<std::endl 
        << "2-登录  " << std::endl
        << "3-注销  " << std::endl
        << "4-修改用户名  " << std::endl
        << "5-修改密码  " << std::endl
        << "其他键以退出  " <<std::endl
        << "请输入您想执行的操作: " ;
    std::cin >> index;
    switch(index) {
      case 1: {
        log_Register();
        break;
      }
      case 2: {
        if(log_Login()) {
          isLogin = true;
          flag = 0;
        }
        break;
      }
      case 3: {
        if(log_Logout()) {
          flag = 0;
        }
        break;
      }
      case 4: {
        change_UserName();
        break;
      }
      case 5: {
        change_Passwd();
        break;
      }
      default: {
        flag = 0;
        break;
      }
    }
  }
  if(isLogin) {
    std::cout << "已登录 " <<std::endl;
  } else {
    std::cout << "无操作执行 " <<std::endl;
  }


  return 0;
}

//注册
bool log_Register() {
  std::string username,password,re_password;
  std::cout << "注册时:不要包含无意义的字符 " <<std::endl;
  std::cout << "请输入您的用户名: " ;
  std::cin >> username;
  label:
    std::cout << "请设置您的密码: " ;
    std::cin >> password;
    std::cout << "请再次输入您的密码: " ;
    std::cin >> re_password;
    if(password != re_password) {
      std::cout << "您两次输入的密码不一致! " <<std::endl;
      return false;
    }

  if(!my.my_PasswordCheck(password)) {
    std::cout << "密码强度不符合要求 " <<std::endl;
    goto label;
  }
 
  if(my.my_Register(username, password)) {
    std::cout << "注册成功！ " <<std::endl;
    LogManager::logUserActivity(username,"注册","成功");
  } else {
    std::cout << "注册失败! " <<std::endl;
    LogManager::logUserActivity(username,"注册","失败");
    return false;
  }

  return true;
}

//登录
bool log_Login() {
  std::string username, password;
  std::cout << "登录时:不要包含无意义的字符 " <<std::endl;
  std::cout << "请输入您的用户名: " ;
  std::cin >> username;
  std::cout << "请输入您的密码: " ;
  std::cin >> password;

  if(my.my_Login(username,password)) {
    std::cout << "登录成功！ " <<std::endl;
    LogManager::logUserActivity(username,"登录","成功");
    return true;
  }
  
  LogManager::logUserActivity(username,"登录","失败");
  return false;
}

//注销
bool log_Logout() {
  std::string username, password;
  std::cout << "请输入你需要注销的用户名: " ;
  std::cin >> username;
  std::cout << "请输入你需要注销的账户密码: " ;
  std::cin >> password;

  LogManager::logSecurityEvent("注销用户",username);
  if(my.my_LogoutUser(username, password)) {
    std::cout << "注销用户成功！" << std::endl;
    LogManager::logUserActivity(username,"注销","成功");

    return true;
  }
  LogManager::logUserActivity(username,"注销","失败");
  return false;
}

//修改用户名或密码
bool change_UserName() {
  std::string username, password;
  std::cout << "请输入你需要修改的用户名: " ;
  std::cin >> username;
  std::cout << "请输入你需要修改用户名的账户密码: " ;
  std::cin >> password; 

  if(my.my_ChangeUserName(username, password)) {
    std::cout << "修改用户名成功！ " << std::endl;
    LogManager::logUserActivity(username,"修改用户名","成功");
    return true;
  }
  LogManager::logUserActivity(username,"修改用户名","失败");
  return false;
}

bool change_Passwd() {
  std::string username, password;
  std::cout << "请输入你需要修改密码的用户名: " ;
  std::cin >> username;
  std::cout << "请输入你需要修改的账户密码: " ;
  std::cin >> password; 

  if(my.my_ChangePasswd(username, password)) {
    std::cout << "修改密码成功！ " << std::endl;
    LogManager::logUserActivity(username,"修改密码","成功");
    return true;
  }
  LogManager::logUserActivity(username,"修改密码","失败");
  return false;
}