/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2023-03-13 09:58:13
 * @last_edit_time: 2023-03-31 20:55:31
 * @file_path: /Thread-Pool/src/ThreadPool.cpp
 * @description: 线程池模块源文件
 */


#include "ThreadPool.h"
#include <fstream>
#include "json/json.h"

/**
 * @description: 默认构造函数，使用通过委托构造函数
 */
ThreadPool::ThreadPool() : ThreadPool("../conf/threadpool.json") {
	
}


/**
 * @description: 含参构造函数
 * @param {size_t} n_threads: 最低线程数量
 * @param {ThreadPoolWorkMode} work_mode: 线程池工作模式
 */
ThreadPool::ThreadPool(const std::string config_path): m_thread_amount(0) {
	m_log->run();
	parseConfig(config_path);

#ifdef DEBUG
	std::cout << "线程池初始配置如下: " << std::endl;
	if (m_config->m_mode == ThreadPoolWorkMode::FIXED_THREAD)
		std::cout << "线程池工作模式: FIXED_THREAD" << std::endl;
	else
		std::cout << "线程池工作模式: MUTABLE_THREAD" << std::endl;
	std::cout << "线程数量: " << m_config->m_min_threshold << '\n'
		<< "线程上限: " << m_config->m_max_threshold << '\n'
		<< "线程下限: " << m_config->m_min_threshold << '\n'
		<< "任务队列长度: " << m_config->m_max_task << '\n'
		<< "任务优先级: " << m_config->m_priority_level << '\n'
		<< "任务提交时限: 3 秒\n"
		<< std::endl;
#else
	std::string task = "线程池初始配置如下 ---------> ";
	
	if (m_config->m_mode == ThreadPoolWorkMode::FIXED_THREAD)
		task += "线程池工作模式: FIXED_THREAD";
	else
		task += "线程池工作模式: MUTABLE_THREAD";

	m_log->addTask(task);
#endif
	// 初始化线程池
	initThreadPool();
}


/**
 * @description: 析构函数，关闭线程池
 */
ThreadPool::~ThreadPool() {
	close();
}


/**
 * @description: 等待线程完成工作，然后关闭线程池
 */
void ThreadPool::close() {
	// 判断
	if (!m_start) {
		return ;
	}

	{
        std::unique_lock<std::mutex> lock(m_mutex);
        m_start = false;
    }

#ifdef DEBUG
	std::cout << "线程池已准备关闭，请勿继续提交任务" << std::endl;
#else
	m_log->addTask("线程池已准备关闭，请勿继续提交任务");
#endif


	// 唤醒所有被当前条件变量阻塞的线程
	m_queue_not_empty.notify_all();

	// 等待所有线程结束工作
	for(std::unordered_map<int, std::thread>::iterator it = m_threads.begin(); it != m_threads.end(); ++it) {
		it->second.join();
	}

#ifdef DEBUG
	std::cout << "线程池已关闭" << std::endl;
#else
	m_log->addTask("线程池已关闭");
#endif

	m_log->close();
}



/**
 * @description: 初始化线程池
 */
void ThreadPool::initThreadPool() {
	m_start = true;
	for (int i = 0; i < m_config->m_min_threshold; ++i) {
		// std::thread 调用类的成员函数需要传递类的一个对象作为参数， 由于是 operator() 下面两种写法都可以，如果是类内部，传入 this 指针即可
		// m_threads[i] = std::thread(Worker(this, i));  // 分配工作线程
		m_threads[m_thread_id] = std::thread(&Worker::operator(), Worker(this, m_thread_id));  // 指定线程所执行的函数
		m_thread_id++;
		m_thread_amount++;
	}
}


/** 
 * @description: 解析 Json 配置文件
 * @param {string} config_path: 配置文件路径
 * @return {bool} 成功返回 true, 失败返回 false
 */
bool ThreadPool::parseConfig(std::string config_path) {
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

    int max = root["max_threads"].asInt();
    int min = root["min_threads"].asInt();
    int hardware_size = std::thread::hardware_concurrency();

    m_config = new ThreadPoolConfig;
    if (root["FIXED_THREAD"].asBool()) {
        m_config->m_mode = ThreadPoolWorkMode::FIXED_THREAD;

        m_config->m_max_threshold = m_config->m_min_threshold = max < hardware_size ? max : hardware_size;
    }
    else {
        m_config->m_mode = ThreadPoolWorkMode::MUTABLE_THREAD;

        m_config->m_max_threshold = max < hardware_size ? max : hardware_size;
        m_config->m_min_threshold = min < hardware_size ? min : hardware_size;
    }
    m_config->m_timeout = std::chrono::milliseconds(root["timeout"].asInt());
    m_config->m_priority_level = root["priority_level"].asInt();

    m_config->m_max_task = root["max_task"].asInt();

    return true;
}
