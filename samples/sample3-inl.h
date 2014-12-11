// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// A sample program demonstrating using Google C++ testing framework.
// 这个例子用于展示使用C++ test框架
// Author: wan@google.com (Zhanyong Wan)

#ifndef GTEST_SAMPLES_SAMPLE3_INL_H_
#define GTEST_SAMPLES_SAMPLE3_INL_H_

#include <stddef.h>


// Queue is a simple queue implemented as a singled-linked list.
// 这是一个由单链表构建的队列
// The element type must support copy constructor.
// 队列中的元素必须支持拷贝构造函数
template <typename E>  // E is the element type
class Queue;

// QueueNode is a node in a Queue, which consists of an element of
// type E and a pointer to the next node.
//
// 队列节点由类型为E的元素，以及指向下一个节点的指针所构成
template <typename E>  // E is the element type
class QueueNode {
  friend class Queue<E>;

 public:
  // Gets the element in this node.
  // 获取节点中元素的值
  const E& element() const { return element_; }

  // Gets the next node in the queue.
  // 获取下一个节点
  QueueNode* next() { return next_; }
  const QueueNode* next() const { return next_; }

 private:
  // Creates a node with a given element value.  The next pointer is
  // set to NULL.
  // 对于给定的值生成一个节点，其指向下个节点的指针被设为空
  explicit QueueNode(const E& an_element) : element_(an_element), next_(NULL) {}

  // We disable the default assignment operator and copy c'tor.
  // 我们使默认的复制构造函数以及拷贝工作函数无效
  const QueueNode& operator = (const QueueNode&);
  QueueNode(const QueueNode&);

  E element_;
  QueueNode* next_;
};

template <typename E>  // E is the element type.E是元素的类型
class Queue {
 public:
  // Creates an empty queue.
  // 生成一个空的队列
  Queue() : head_(NULL), last_(NULL), size_(0) {}

  // D'tor.  Clears the queue.
  ~Queue() { Clear(); }

  // Clears the queue.
  // 清空队列
  void Clear() {
    if (size_ > 0) {
      // 1. Deletes every node.
      // 删除每一个节点
      QueueNode<E>* node = head_;
      QueueNode<E>* next = node->next();
      for (; ;) {
        delete node;
        node = next;
        if (node == NULL) break;
        next = node->next();
      }

      // 2. Resets the member variables.
      // 重置成员变量
      head_ = last_ = NULL;
      size_ = 0;
    }
  }

  // Gets the number of elements.
  // 获取队列中元素的数目
  size_t Size() const { return size_; }

  // Gets the first element of the queue, or NULL if the queue is empty.
  // 获取队列中的第一个元素，如果队列为空，则返回NULL
  QueueNode<E>* Head() { return head_; }
  const QueueNode<E>* Head() const { return head_; }

  // Gets the last element of the queue, or NULL if the queue is empty.
  // 获取队列中的最后一个元素，如果队列为空，则返回NULL
  QueueNode<E>* Last() { return last_; }
  const QueueNode<E>* Last() const { return last_; }

  // Adds an element to the end of the queue.  A copy of the element is
  // created using the copy constructor, and then stored in the queue.
  // Changes made to the element in the queue doesn't affect the source
  // object, and vice versa.
  //
  // 向队列中加入一个元素。这个元素由拷贝构造函数生成，并被存储到队列中，对队列中元素的改变，不会影响到原先的对象，反之亦然
  void Enqueue(const E& element) {
    QueueNode<E>* new_node = new QueueNode<E>(element);

    if (size_ == 0) {
      head_ = last_ = new_node;
      size_ = 1;
    } else {
      last_->next_ = new_node;
      last_ = new_node;
      size_++;
    }
  }

  // Removes the head of the queue and returns it.  Returns NULL if
  // the queue is empty.
  // 将队列的头元素移除，并返回它的值，如果为空，则返回NULL
  E* Dequeue() {
    if (size_ == 0) {
      return NULL;
    }

    const QueueNode<E>* const old_head = head_;
    head_ = head_->next_;
    size_--;
    if (size_ == 0) {
      last_ = NULL;
    }

    E* element = new E(old_head->element());
    delete old_head;

    return element;
  }

  // Applies a function/functor on each element of the queue, and
  // returns the result in a new queue.  The original queue is not
  // affected.
  //
  // 将函数应用于队列中的元素，并将产生的结果存入新的队列中，原始的队列不受影响
  template <typename F>
  Queue* Map(F function) const {
    Queue* new_queue = new Queue();
    for (const QueueNode<E>* node = head_; node != NULL; node = node->next_) {
      new_queue->Enqueue(function(node->element()));
    }

    return new_queue;
  }

 private:
  QueueNode<E>* head_;  // The first node of the queue. 队列的头元素
  QueueNode<E>* last_;  // The last node of the queue.  队列的尾元素
  size_t size_;  // The number of elements in the queue.队列中元素的个数

  // We disallow copying a queue.
  // 我们不允许对队列进行拷贝
  Queue(const Queue&);
  const Queue& operator = (const Queue&);
};

#endif  // GTEST_SAMPLES_SAMPLE3_INL_H_
