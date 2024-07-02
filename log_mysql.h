/*
 * @Description: 
 * @Author: zy
 * @Date: 2024-06-26 15:34:50
 * @LastEditTime: 2024-06-28 16:17:39
 * @LastEditors: zy
 */
#ifndef _LOG_MYSQL_H_
#define _LOG_MYSQL_H_

#include <bits/stdc++.h>
#include <mysql/mysql.h>

class Log_Mysql
{
 private:
  const char* m_host;           //主机地址
  const char* m_username;       //用户名
  const char* m_password;       //密码
  const char* m_database;       //连接的数据库名称
  unsigned int m_port;          //端口号，默认3306
  const char* m_unix_socket;    //unix连接标识，一般默认NULL
  unsigned long m_client_flag;  //客户端连接标识，一般默认为0

 private:
  MYSQL m_Mysql;                     //mysql连接句柄
  MYSQL_RES* m_SelectResult = NULL;  //结果集

 public:
  Log_Mysql();
  ~Log_Mysql();

 public:
  //初始化数据库
  bool my_Init();
  //释放数据库资源
  void my_FreeConnect();
  //sql执行语句
  bool my_RunSql(const char* sql);
  //sql查询语句
  bool my_SelectData(const char* sql);
  //打印结果  
  void my_PrintData();

 public:
  //密码强度检验
  bool my_PasswordCheck(std::string& pw);
  //密码加密

 private:
  int m_password_level = 1; //[0 1:长度要大于8 2：大小写字母特殊符号三占二]

 public:
  //注册验证
  bool my_Register(std::string& un, std::string& pw);
  //登录验证
  bool my_Login(std::string& un, std::string& pw);
  //注销用户
  bool my_LogoutUser(std::string& un, std::string& pw);

public:
  //密码加密
  std::string my_EncryptPasswd(std::string &pw);
  //密码解密
  std::string my_DecryptPasswd(std::string &pw);
  //生成密码表(整个周期只能生成一次)
  //bool my_GeneratePasswd();
};

#endif