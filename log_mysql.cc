/*
 * @Description: 
 * @Author: zy
 * @Date: 2024-06-26 16:39:52
 * @LastEditTime: 2024-07-02 11:02:18
 * @LastEditors: zy
 */
#include "log_mysql.h"
#include <bits/stdc++.h>
#include <mysql/mysql.h>

const int max_size = 100; //密码表大小
//const std::string file_name = "passwd.txt" ;
const std::string file_name = "/home/cidi/vscodeWorkspace/temp/wb_password_test/passwd_mysql/passwd.txt" ;

std::string passwd[max_size] ;


//此处构造函数直接写数据库信息，后续迁移到服务器可以改成读配置文件
Log_Mysql::Log_Mysql() {
  m_host = "localhost";
  m_username = "root";
  m_password = "zy1029343447";
  m_database = "passwd_test";
  m_port = 3306;
  m_unix_socket = NULL;
  m_client_flag = 0;

  my_Init();
}

Log_Mysql::~Log_Mysql() {
  my_FreeConnect();
}

bool getPasswd();
bool Log_Mysql::my_Init() {
  //打开密码表
  if(!getPasswd()) {
    std::cerr << "打开密码表出现错误，加密失败 " << std::endl;
    return false;
  }
  if(!mysql_init(&m_Mysql)) {
    std::cerr << "MySQL initialization failed " <<std::endl;
    std::cerr << mysql_error(&m_Mysql) <<std::endl;
    return false;
  }
  if(!mysql_real_connect(&m_Mysql, m_host, m_username, m_password,
      m_database, m_port, m_unix_socket, m_client_flag)) {
        std::cerr << "MySQL connection failed " <<std::endl;
        std::cerr << mysql_error(&m_Mysql) <<std::endl;
        return false;
  }
  if(mysql_set_character_set(&m_Mysql,"utf8")) {
    std::cerr << "设置字符集失败 " << std::endl;
    std::cerr << mysql_error(&m_Mysql) <<std::endl;
    return false; 
  }
  return true;
}

bool Log_Mysql::my_Register(std::string& un, std::string& pw) {
  //查询判断是否有该用户名
  std::string sql = "select * from login_test";

  std::string raw_pw = pw;
  pw = my_EncryptPasswd(pw);
  
  if(pw == "ERROR") return false;

  if(mysql_real_query(&m_Mysql, sql.c_str(), sql.length())) {
    std::cerr << "MySQL query failed " <<std::endl;
    std::cerr << mysql_error(&m_Mysql) <<std::endl;
  }

  m_SelectResult = mysql_store_result(&m_Mysql);
  MYSQL_ROW row;
  while(row = mysql_fetch_row(m_SelectResult)) {
    if(row[1]==un) {
      std::cerr << "The user is registered " <<std::endl;
      std::cerr << mysql_error(&m_Mysql) <<std::endl;
      return false;
    }
  }
  
  sql = "insert into login_test(username,password,raw_passwd) values(\"" 
  + un + "\",\"" + pw + "\",\"" + raw_pw + "\")";
 

  if(mysql_real_query(&m_Mysql, sql.c_str(), sql.length())) {
    std::cerr << "Execute sql failed " <<std::endl;
    std::cerr << mysql_error(&m_Mysql) <<std::endl;
    return false;
  }

  std::cout << "User registration successful " <<std::endl;
  
  return true;

}


bool Log_Mysql::my_Login(std::string& un, std::string& pw) {
  std::string sql = "select * from login_test";

  //查询
  if(mysql_real_query(&m_Mysql, sql.c_str(),sql.length())) {
    std::cerr << "Query sql failed " <<std::endl;
    std::cerr << mysql_error(&m_Mysql) <<std::endl;
    return false;
  }
  m_SelectResult = mysql_store_result(&m_Mysql);
  if(!m_SelectResult) {
    std::cout << "No such user " <<std::endl;
    return false;
  }

  MYSQL_ROW row;
  while(row = mysql_fetch_row(m_SelectResult)) {
    std::string decrypted_pw = std::string(row[2]);
    decrypted_pw = my_DecryptPasswd(decrypted_pw);
    if(decrypted_pw == "ERROR") return false;std::string new_pw = my_EncryptPasswd(pw);
  // std::cout << "new_pw" << new_pw <<std::endl;
    if(row[1] == un && decrypted_pw == pw) return true;
  }

  std::cout << "Wrong account or password! " <<std::endl;
  return false;
}

bool Log_Mysql::my_LogoutUser(std::string& un, std::string& pw) {
  
  if(!my_Login(un,pw)) {
    return false;
  }
  
  std::string sql = "delete from login_test where username=\"" + un + "\"";
  if(mysql_real_query(&m_Mysql, sql.c_str(), sql.length())) {
    std::cerr << "Delete sql failed " <<std::endl;
    std::cerr << mysql_error(&m_Mysql) <<std::endl;
    return false;
  }
  return true;
}


void Log_Mysql::my_FreeConnect() {
  std::cout << "=====================" <<std::endl;
  std::cout << "Start recycling Mysql connections! " <<std::endl;
  //释放存储的查询结果集合
  mysql_free_result(m_SelectResult);
  //释放Mysql链接资源
  mysql_close(&m_Mysql);
  return;
}

//运行sql语句
bool Log_Mysql::my_RunSql(const char* sql){}
//查询sql数据
bool Log_Mysql::my_SelectData(const char* sql){}
//打印信息
void Log_Mysql::my_PrintData(){}


bool isNullPw(std::string& pw) {
  if(pw.length()==0) return false;
  return true;
}

bool isLengthPw(std::string& pw) {
  return pw.length()>=8? true:false;
}

bool isComplexPw(std::string& pw) {
  //大小写字母，特殊符号三者占其二
  bool hasUpper = false;
  bool hasLower = false;
  bool hasSpecial = false;
  const char* specialChars = "!@#$%^&*()_+-=[]{}|;':\",.<>/?\\";
  for(auto it:pw) {
    if(isupper(it)) hasUpper = true;
    else if(islower(it)) hasLower = true;
    else{
      if(strchr(specialChars,it)!=nullptr) hasSpecial = true;
    }
  }
  
  return (hasUpper && hasLower) || (hasUpper && hasSpecial) || (hasLower && hasSpecial);
}

bool Log_Mysql::my_PasswordCheck(std::string& pw) {
  bool flag = true;
  switch(m_password_level) {
    case 0: {      
      flag = isNullPw(pw);
      break;
    }
    case 1: {
      flag = isNullPw(pw) && isLengthPw(pw);
      break;
    }
    case 2: {
      flag = isNullPw(pw) && isLengthPw(pw) && isComplexPw(pw);
      break;
    }
  }
  return flag;
}

//打开密码表
bool getPasswd() {
  std::ifstream myFile;
  myFile.open(file_name);
  if(!myFile.is_open()) {
    std::cerr << "打开文件失败\n" ;
    return false;
  }
  std::string line;
  while(getline(myFile,line)) {
    for(int i=0;i<max_size;i++) {
      passwd[i] = line;
    }
  }
  myFile.close();
  return true;
}

std::string Log_Mysql::my_EncryptPasswd(std::string &pw) {

  std::ostringstream fin_passwd;
  int length = pw.length();
  int result[100];
  srand(time(0));
  int rule = rand()%max_size;

  for(int i=0; i < length; i++) {
    size_t pos = passwd[rule].find(pw[i]); 
    if(pos == std::string::npos) {
      std::cerr << "加密过程出现错误，加密失败 " << std::endl;
      return "ERROR";
    }else {
      result[i] = static_cast<int>(pos);
    }
  }
  fin_passwd << rule << "." << (rand() % 88 +10) << "_"; 
  fin_passwd << result[0];
  for(int i=1; i<length; i++) {
    if(pw[i]!=0) {
      fin_passwd << "." << result[i];
    }
    else{
      fin_passwd << "_" << result[i+1];
      i++;
    }
  }
  return fin_passwd.str();
}

std::string Log_Mysql::my_DecryptPasswd(std::string &pw) {
  int length = pw.length();
  int rule=0, value=0, index;
  std::ostringstream rawPasswd; 
 
  // 解析规则部分
  for(int i=0; pw[i] != '.'; i++) {
    rule *= 10 + pw[i] - '0';
    index = i;
  } 

  // 解析后续部分
  for(int i = index+5; i < length; i++) {
    if(pw[i] >='0' && pw[i] <= '9') {
      value = value * 10 + (pw[i] -'0');
    } else {
      rawPasswd << passwd[rule][value];
      if(pw[i] == '_') rawPasswd << " ";
      value = 0;
    }
  }

  // 确保处理最后一个值
  if (value > 0) {
      rawPasswd << passwd[rule][value];
  }

  return rawPasswd.str();
}

// 生成打乱的密码表
std::string generate_password(const std::string& characters) {
    std::string shuffled = characters;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffled.begin(), shuffled.end(), g);
    return shuffled;
}

// 对特殊字符进行转义
std::string escape_string(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        if (c == '\\' || c == '\"') {
            escaped += '\\';
        }
        escaped += c;
    }
    return escaped;
}

/*
bool Log_Mysql::my_GeneratePasswd() {
  const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-+={[}]|:;\"'<,>.?/~`\\";
  for(int i=0; i<max_size; i++) {
    passwd[i] = generate_password(characters);
  }

  std::ofstream myFile;
  myFile.open(file_name);
  if(!myFile.is_open()){
    std::cerr << "写入文件失败" << std::endl;
    return false;
  }

  for(std::string temp : passwd){
    myFile <<"\"" <<" "<< escape_string(temp) << "\"" << "," << std::endl;
  }

  myFile.close();
  return true;
}
*/
