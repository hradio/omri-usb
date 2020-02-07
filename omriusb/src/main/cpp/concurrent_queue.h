/*
 * Copyright (C) 2018 IRT GmbH
 *
 * Author:
 *  Fabian Sattler
 *
 * This file is a part of IRT DAB library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

#ifndef CONCURRENT_QUEUE
#define CONCURRENT_QUEUE

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

template <typename T>
class ConcurrentQueue {

public:
    T pop() {
        std::unique_lock<std::mutex> mlock(m_mutex);
        //effective against spurious wakes
        while (m_queue.empty()) {
            m_cond.wait(mlock);
        }

        auto item = m_queue.front();
        m_queue.pop();
        return item;
    }

    void pop(T& item) {
        std::unique_lock<std::mutex> mlock(m_mutex);
        while (m_queue.empty()) {
            m_cond.wait(mlock);
        }

        item = m_queue.front();
        m_queue.pop();
    }

    //tries to pop an element but unlocks after timeout
    bool tryPop(T& item, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> mlock(m_mutex);

        //lambda inside for predicate
        if(!m_cond.wait_for(mlock, timeout, [this] {return !m_queue.empty(); })) {
            return false;
        }

        item = m_queue.front();
        m_queue.pop();
        return true;
    }

    void push(const T& item) {
        std::unique_lock<std::mutex> mlock(m_mutex);
        m_queue.push(item);
        //unlock manually before notifying
        mlock.unlock();
        m_cond.notify_one();
    }

    void push(T&& item) {
        //the item will be std::moved to the queue
        std::unique_lock<std::mutex> mlock(m_mutex);
        m_queue.push(std::move(item));
        //unlock manually before notifying
        mlock.unlock();
        m_cond.notify_one();
    }

    int getSize() {
        std::unique_lock<std::mutex> mlock(m_mutex);
        return m_queue.size();
    }

    void getSize(int& size) {
        std::unique_lock<std::mutex> mlock(m_mutex);
        size = m_queue.size();
        mlock.unlock();
    }

    bool isEmpty() {
        std::unique_lock<std::mutex> mlock(m_mutex);
        return m_queue.empty();
    }

    void clear() {
        std::unique_lock<std::mutex> mlock(m_mutex);
        std::queue<T> empty;
        std::swap(m_queue, empty);
        mlock.unlock();
        //make sure every waiting thread gets its fair share
        m_cond.notify_all();
    }

public:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

#endif // CONCURRENT_QUEUE

