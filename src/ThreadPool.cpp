/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2023-03-13 09:58:13
 * @last_edit_time: 2023-03-29 19:42:20
 * @file_path: /Thread-Pool/src/ThreadPool.cpp
 * @description: 线程池模块源文件
 */


#include "ThreadPool.h"

/*
***************************线程池的实现***************************
*/

/**
 * @description: 默认构造函数，线程数量为可用硬件实现支持的并发线程数
 * @description: 通过委托构造函数
 */
ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency(), ThreadPoolWorkMode::FIXED_THREAD)
{ }


/**
 * @description: 含参构造函数
 * @param {size_t} n_threads: 最低线程数量
 * @param {ThreadPoolWorkMode} work_mode: 线程池工作模式
 */
ThreadPool::ThreadPool(const size_t n_threads, ThreadPoolWorkMode work_mode)
	: m_max_task(2 * n_threads)
	, m_max_threshold(
		work_mode == ThreadPoolWorkMode::FIXED_THREAD ? 
			(n_threads < std::thread::hardware_concurrency() ? n_threads : std::thread::hardware_concurrency()) : 
			(2 * n_threads < std::thread::hardware_concurrency() ? 2 * n_threads : std::thread::hardware_concurrency()))
	, m_min_threshold(
		work_mode == ThreadPoolWorkMode::FIXED_THREAD ? 
			(n_threads < std::thread::hardware_concurrency() ? n_threads : std::thread::hardware_concurrency()) : 
			(n_threads < std::thread::hardware_concurrency() ? n_threads : std::thread::hardware_concurrency()))
	, m_mode(work_mode)
	, m_thread_amount(0)
{
	std::cout << "线程池初始配置如下: " << std::endl;
	if (m_mode == ThreadPoolWorkMode::FIXED_THREAD)
		std::cout << "线程池工作模式: FIXED_THREAD" << std::endl;
	else
		std::cout << "线程池工作模式: MUTABLE_THREAD" << std::endl;
	std::cout << "线程数量: " << m_min_threshold << '\n'
		<< "线程上限: " << m_max_threshold << '\n'
		<< "线程下限: " << m_min_threshold << '\n'
		<< "任务队列长度: " << m_max_task << '\n'
		<< "任务优先级: " << m_priority_level << '\n'
		<< "任务提交时限: 3 秒\n"
		<< std::endl;

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
		std::cout << "线程池已准备关闭，请勿继续提交任务" << std::endl;
    }

	// 唤醒所有被当前条件变量阻塞的线程
	m_queue_not_empty.notify_all();

	// 等待所有线程结束工作
	for(std::unordered_map<int, std::thread>::iterator it = m_threads.begin(); it != m_threads.end(); ++it) {
		it->second.join();
	}

	std::cout << "线程池已关闭" << std::endl;
}



/**
 * @description: 初始化线程池
 */
void ThreadPool::initThreadPool() {
	m_start = true;
	for (int i = 0; i < m_min_threshold; ++i) {
		// std::thread 调用类的成员函数需要传递类的一个对象作为参数， 由于是 operator() 下面两种写法都可以，如果是类内部，传入 this 指针即可
		// m_threads[i] = std::thread(Worker(this, i));  // 分配工作线程
		m_threads[m_thread_id] = std::thread(&Worker::operator(), Worker(this, m_thread_id));  // 指定线程所执行的函数
		m_thread_id++;
		m_thread_amount++;
	}
}

