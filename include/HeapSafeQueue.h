/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2023-04-05 14:52:59
 * @last_edit_time: 2023-04-05 15:22:14
 * @file_path: /Thread-Pool/include/HeapSafeQueue.h
 * @description: 基于堆结构的优先级队列头文件
 */

#pragma once
#include <vector>
#include <mutex>
#include <functional>
#include <iostream>

class HeapSafeQueue {
private:
	std::vector<std::pair<std::function<void()>, int>> m_queue;  // 任务队列
	std::mutex m_mutex;  // 任务队列互斥锁

    void siftUp(int);  // 向上调整
    void siftDown(int, int);  // 向下调整
public:
	HeapSafeQueue() { 
        m_queue.clear();
    }
	~HeapSafeQueue() = default;

	/* 成员函数 */
	inline bool empty();  // 队列是否为空
	inline size_t size();  // 任务队列大小
    
	void taskEnqueue(std::function<void()> &, size_t);  // 添加任务
	bool taskDequeue(std::function<void()> &);  // 取出任务
};


/**
 * @description: 判断任务队列是否为空
 * @return {bool} m_safe_queue.empty()
 */
bool HeapSafeQueue::empty() {
	std::unique_lock<std::mutex> lock(m_mutex);  // 任务队列上锁

	return m_queue.empty();
}


/**
 * @description: 获取任务队列大小
 * @return {size_t} m_safe_queue.size()
 */
size_t HeapSafeQueue::size() {
	std::unique_lock<std::mutex> lock(m_mutex);  // 任务队列上锁

	return m_queue.size();
}
