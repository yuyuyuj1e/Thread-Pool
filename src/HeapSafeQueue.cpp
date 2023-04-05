/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2023-04-05 14:53:08
 * @last_edit_time: 2023-04-05 16:08:28
 * @file_path: /Thread-Pool/src/HeapSafeQueue.cpp
 * @description: 基于堆结构的优先级队列源文件
 */

#include "HeapSafeQueue.h"

/** 
 * @description: 向上调整，用于向堆中插入一个数据，全局
 * @description: 从 start 开始，自下向上比较；
 * @description: 如果子节点小于父节点，则相互交换，直到（条件一）子节点大于父节点，（条件二）或者子节点成为根节点（index == 0）
 * @param {int} start: 子节点下标
 */
void HeapSafeQueue::siftUp(int start) {
	/*
	 *	1. 下标为 i 的节点的父节点下标：(i - 1) / 2 【向下取整】
   	 *	2. 下标为 i 的节点的左孩子下标：i * 2 + 1
   	 *	3. 下标为 i 的节点的右孩子下标：i * 2 + 2
	 */
	int son = start, parent = (son - 1) / 2;
 
	while (son > 0) {
		if (m_queue[parent].second < m_queue[son].second) {
			break;
		}
		else {
			// 交换父子节点
			std::pair<std::function<void()>, int> temp = m_queue[son];
			m_queue[son] = m_queue[parent];
			m_queue[parent] = temp;

			// 获取下一轮父子节点下标
			son = parent;  // 子节点(本节点)新下标
			parent = (son - 1) / 2;  // 父节点(下次循环的父节点)的新下标
		}
	}
}


/** 
 * @description: 向下调整，用于重构推结构，局部
 * @description: 从根节点开始，向下比较
 * @description: 直到，成为叶子节点或者子节点都比自己小为止
 * @param {int} start: 起始节点下标
 * @param {int} end: 结束节点下标
 */
void HeapSafeQueue::siftDown(int start, int end) {
	int parent = start;
	int son = 2 * parent + 1;  // lson: i * 2 + 1，rson: i * 2 + 2
 
	while (son <= end) {
		// 让 son 指向更小的子节点
		if (son < end && m_queue[son + 1].second < m_queue[son].second) {
			son++;
		}
 
		// 两个子节点都比父节点的大
		if (m_queue[son].second > m_queue[parent].second) {
			break;  
		}
		else {
			// 交换节点
			std::pair<std::function<void()>, int> temp = m_queue[son];
			m_queue[son] = m_queue[parent];
			m_queue[parent] = temp;

			// 更新下标
			parent = son;  // 父节点(本节点)的新下标
			son = 2 * son + 1;  // 子节点(下次循环的子节点)的新下标
		}
	}
}


/**
 * @description: 向任务队列添加任务
 * @param {std::function<void()>&} task: 任务函数
 * @param {size_t} priority: 任务优先级
 */
void HeapSafeQueue::taskEnqueue(std::function<void()> &task, size_t priority) {
	std::unique_lock<std::mutex> lock(m_mutex);

	std::pair<std::function<void()>, int> priority_task(task, priority);  // 将任务与优先级打包
	m_queue.emplace_back(priority_task);  // 放入任务队列
    siftUp(m_queue.size() - 1);  // 向上调整

	std::cout << "任务已提交，当前任务数量为: " << m_queue.size() << std::endl;
}


/**
 * @description: 是否可以从任务队列取出任务，如可以取出任务
 * @param {std::function<void()>&} task: 获取任务函数的空函数
 * @return {bool} true/false
 */
bool HeapSafeQueue::taskDequeue(std::function<void()> &task) {
	std::unique_lock<std::mutex> lock(m_mutex);  // 任务队列上锁

	if (m_queue.empty())
		return false;

	task = std::move(m_queue[0].first);  // 取出队首元素，返回队首元素值，并进行右值引用
    std::cout << "任务优先级为：" << m_queue[0].second << std::endl;

    m_queue[0] = m_queue[m_queue.size() - 1];  // 将最后一个元素，放到堆顶；注意，此时堆的特性已经被破坏，需要重新维护
	m_queue.erase(m_queue.end() - 1);  // 弹出任务
    
    siftDown(0, m_queue.size() - 1);
	return true;
}