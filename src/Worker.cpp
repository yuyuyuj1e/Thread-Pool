/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2023-03-29 19:06:23
 * @last_edit_time: 2023-04-04 17:13:42
 * @file_path: /Thread-Pool/src/Worker.cpp
 * @description: 线程池内部工作类的源文件
 */


#include "ThreadPool.h"

/**
 * @description: 工作线程构造函数
 * @param {ThreadPool} *pool: 工作线程所属线程池
 * @param {int} id: 工作线程 ID
 */
ThreadPool::Worker::Worker(ThreadPool *pool, const int id) 
	: m_pool(pool)
	, m_id(id)
{ }


/**
 * @description: 重载 ()，这里是工作线程的工作函数，提交的函数会在这里执行
 */
void ThreadPool::Worker::operator()() {
	std::function<void()> func;  // 存放真正执行的函数
	bool dequeued = false;  // 是否取出任务

	while (m_pool->m_start || !m_pool->m_queue.empty()) {
		{
			// 线程池加锁
			std::unique_lock<std::mutex> lock(m_pool->m_mutex);

			std::cout << "tid: " << std::this_thread::get_id() << " 正在尝试获取任务" << std::endl;

			// 如果任务队列为空，阻塞当前线程
			if (m_pool->m_queue.empty()) {
				std::cout << "任务队列空，等待任务..." << std::endl;
				if (m_pool->m_config->m_mode == ThreadPoolWorkMode::FIXED_THREAD) {
					m_pool->m_queue_not_empty.wait(lock);  // 等待任务
				}
				else if (m_pool->m_config->m_mode == ThreadPoolWorkMode::MUTABLE_THREAD) {
					
					if (std::cv_status::timeout == m_pool->m_queue_not_empty.wait_for(lock, std::chrono::milliseconds(m_pool->m_config->m_timeout))) {
						
						if (m_pool->m_thread_amount > m_pool->m_config->m_min_threshold) {
							std::cout << "tid:" << std::this_thread::get_id() << " 退出! ---- ";
							m_pool->m_threads[m_id].detach();
							m_pool->m_threads.erase(m_id);
							m_pool->m_thread_amount--;
							std::cout << "剩余线程: " << m_pool->m_thread_amount << std::endl;
							return ;
						}
						else {
							m_pool->m_queue_not_empty.wait(lock);  // 等待任务
						}
					}
				}
			}

			// 取出任务队列中的元素
			dequeued = m_pool->m_queue.taskDequeue(func);
		}

		// 如果成功取出，执行工作函数
		if (dequeued) {
			// 取出一个任务进行通知 通知可以继续提交任务
			m_pool->m_queue_not_full.notify_all();
			std::cout << "tid: " << std::this_thread::get_id() << " 已领取任务，当前任务数量为: " << m_pool->m_queue.safeQueueSize() << "  ----->   " << m_pool->m_threads.size() << std::endl;
			func();
		}
		else {
			std::cout << "tid: " << std::this_thread::get_id() << " 取出任务失败" << std::endl;
		}
	}
}