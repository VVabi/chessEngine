/*
 * threadSafeQueue.hpp
 *
 *  Created on: Sep 7, 2016
 *      Author: vabi
 */



#include <mutex>
#include <queue>
#include <tr1/memory>


template<class T> class ThreadSafeQueue{
	private:
		std::queue<T> storage;
		std::mutex    queue_mutex;
	public:

		void push(T t){
			queue_mutex.lock();
			storage.push(std::move(t));
			queue_mutex.unlock();
		}

	/*	T pop(){
			queue_mutex.lock();

			queue_mutex.unlock();
			return t;
		}*/

		ThreadSafeQueue(){
			storage = std::queue<T>();
		}

		ThreadSafeQueue(const ThreadSafeQueue& that) = delete;
		ThreadSafeQueue(const ThreadSafeQueue&& that) = delete;
};
