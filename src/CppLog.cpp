/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2023-03-15 09:22:13
 * @last_edit_time: 2023-03-31 20:50:22
 * @file_path: /Thread-Pool/src/CppLog.cpp
 * @description: 日志模块源文件
 */

#include "CppLog.h"
#include "json/json.h"
#include <string>
#include <chrono>
#include <sys/stat.h>
#include <iostream>


CppLog* CppLog::m_log = new CppLog("../conf/log.json");

/**
 * @description: 默认构造函数，使用委托构造函数
 */
CppLog::CppLog() : CppLog("../conf/log.json") {
    
}

/**
 * @description: 日志模块对象初始化函数
 * @param {string} config_path: 配置文件存放路径
 */
CppLog::CppLog(const std::string config_path) { 
    parseConfig(config_path);
    m_thread = nullptr;
}


/**
 * @description: CppLog 对象析构函数，会调用成员函数 close()
 */
CppLog::~CppLog() {
    closeLog();
}


/** 
 * @description: 关闭日志文件
 */
void CppLog::closeLog() {
    if (m_fp) {
        m_fp.close();
    }
}


/**
 * @description: 打开日志文件
 * @return {bool}: 日志文件打开成功返回 true， 失败返回 false
 */
bool CppLog::open() {
    /* 打开日志文件 */
    if (m_fp.is_open()) {  // 当第一次打开，或者执行过备份后会关闭 fp，此时再重写打开文件
        return false;
    }

    /* 打开日志文件 */
    std::string full_path = m_config->m_path + "/" + m_config->m_name;

    if (m_config->m_mode == LogMode::ADDTO) {
        m_fp.open(full_path, std::ofstream::app);
    }
    else if (m_config->m_mode == LogMode::WRITEONLY) {
        m_fp.open(full_path, std::ofstream::out);
    }


    if (!m_fp.is_open()) {  // 打开失败
        return false;
    }
    return true;
}


/**
 * @description: 备份日志文件
 * @return {bool}: 需要进行备份时返回 true，否则返回 false
 */
bool CppLog::backup() {
    /* 判断是否需要进行备份 */
    if (m_config->m_backup == false) {
        return false;
    }

    /* 获取文件大小 */
	struct stat stat_buf;  // 存储文件(夹)信息的结构体，有文件大小和创建时间、访问时间、修改时间等
    std::string full_path = m_config->m_path + "/" + m_config->m_name; 
	stat(full_path.c_str(), &stat_buf);  // 提供文件名字符串，获得文件属性结构体
	size_t file_size = stat_buf.st_size;  // 获取文件大小

    /* 备份文件 */
    if (file_size >= m_config->m_max_size * 1024 * 1024) {
        /* 配置时间 */
        TimeFormat ft = m_config->m_time_format;
        setTimeFormat(TimeFormat::FULLA);
        std::string now_t = getCurrentTime();
        setTimeFormat(ft);

        /* 重命名 */
        m_fp.close();
        std::string new_name = full_path + " " + now_t;
        rename(full_path.c_str(), new_name.c_str());
    }
    else return false;

    return true;
}


/**
 * @description: 获取指定时间格式的当前时间字符串
 * @return {std::string}: 指定时间格式的字符串
 */
std::string CppLog::getCurrentTime() {
    /* 获取当前时间戳 */
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t now_t = std::chrono::system_clock::to_time_t(now);

    /* 将当前时间戳转换为 string */
    struct tm now_st = *localtime (&now_t);
    now_st.tm_year = now_st.tm_year + 1900;
    ++now_st.tm_mon;
    char ctime[20];

   
    if (m_config->m_time_format == TimeFormat::FULLB) {
        snprintf(ctime, 20, "%04d/%02d/%02d %02d:%02d:%02d"
            , now_st.tm_year, now_st.tm_mon, now_st.tm_mday
            , now_st.tm_hour, now_st.tm_min, now_st.tm_sec
        );
    }
    else if (m_config->m_time_format == TimeFormat::YMDA) {
        snprintf(ctime, 11, "%04d-%02d-%02d"
            , now_st.tm_year, now_st.tm_mon, now_st.tm_mday
        );
    }
    else if (m_config->m_time_format == TimeFormat::YMDB) {
        snprintf(ctime, 11, "%04d/%02d/%02d"
            , now_st.tm_year, now_st.tm_mon, now_st.tm_mday
        );
    }
    else if (m_config->m_time_format == TimeFormat::TIMEONLY) {
        snprintf(ctime, 9, "%02d:%02d:%02d"
            , now_st.tm_hour, now_st.tm_min, now_st.tm_sec
        );
    }
    else {
        snprintf(ctime, 20, "%04d-%02d-%02d %02d:%02d:%02d"
            , now_st.tm_year, now_st.tm_mon, now_st.tm_mday
            , now_st.tm_hour, now_st.tm_min, now_st.tm_sec
        );
    }

    return std::string(ctime);
}


/**
 * @description: 带时间写入日志
 * @param {string} str: 写入日志的内容
 */
void CppLog::writeWithTime(const std::string str) {
    backup();
    open();
    
    std::string now_t = getCurrentTime();
    while (now_t.length() != 20) now_t += " ";

    m_fp << now_t << " --->  " << str << '\n';

    // flush 不关闭文件流的情况下，情况缓冲区，将内容写入文件
    m_fp.flush();
}

/**
 * @description: 不带时间写入日志
 * @param {string} str: 写入日志的内容
 */
void CppLog::write(const std::string str) {
    open();
    backup();
    m_fp << str << '\n';
}


/**
 * @description: 日志线程工作函数
 */
void CppLog::working() {
    while (m_start || !m_taskQ.empty()) {
        while (!m_taskQ.empty()) {
            m_mutex.lock();  // 加锁，保护共享资源
            std::pair<std::string, int> pair = m_taskQ.front();
            m_taskQ.pop();
            m_mutex.unlock();  // 解锁
            if (pair.second > 0) {  // 如果标志大于 0，调用带时间的
                writeWithTime(pair.first);
            }
            else {
                write(pair.first);
            }
        }
    }
}


/**
 * @description: 外部调用，向任务队列添加任务
 * @param {string} str: 需要记录的日志内容，默认值为 1
 * @param {int} flag: 是否记录时间，当数值给定数值大于 0 时记录时间，否则不记录时间，默认记录时间
 */
void CppLog::addTask(std::string str, int flag) {
    m_mutex.lock();
    m_taskQ.push(std::make_pair(str, flag));  // 将任务加入工作队列中
    m_mutex.unlock();
}


/** 
 * @description: 解析 Json 配置文件
 * @param {string} config_path: 配置文件路径
 * @return {bool} 成功返回 true, 失败返回 false
 */
bool CppLog::parseConfig(std::string config_path) {
	std::ifstream ifs(config_path);

	if (!ifs) {
        perror("open json fail");
        return false;
    }

	// 反序列化 -> Value 对象
    Json::Value root;
    Json::Reader rd;
    rd.parse(ifs, root);

    //  从 Value 对象对象读取数据
    if (!root.isObject()) {
        return false;
    }

    m_config = new LogConfig;
    m_config->m_path = root["log_path"].asString();
    m_config->m_name = root["log_name"].asString();

    if (root["open_mode"].asString() == "ADDTO") {
        m_config->m_mode = LogMode::ADDTO;
    }
    else {
        m_config->m_mode = LogMode::WRITEONLY;
    }

    if (root["time_format"].asString() == "TIMEONLY") {
        m_config->m_time_format = TimeFormat::TIMEONLY;
    }
    else if (root["time_format"].asString() == "FULLB") {
        m_config->m_time_format = TimeFormat::FULLB;
    }
    else if (root["time_format"].asString() == "YMDA") {
        m_config->m_time_format = TimeFormat::YMDA;
    }
    else if (root["time_format"].asString() == "YMDB") {
        m_config->m_time_format = TimeFormat::YMDB;
    }
    else {
        m_config->m_time_format = TimeFormat::FULLA;
    }

    m_config->m_backup = root["backup"].asBool();
    m_config->m_max_size = root["max_log_size"].asInt();

    return true;
}


/** 
 * @description: 日志文件启动函数
 */
void CppLog::run() {
    m_start = true;  // 启动日志类
    m_thread = new std::thread(&CppLog::working, this);  // 构造线程
}