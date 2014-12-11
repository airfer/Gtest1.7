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
//
// Author: wan@google.com (Zhanyong Wan)


// In this example, we use a more advanced feature of Google Test called
// test fixture.
//
// A test fixture is a place to hold objects and functions shared by
// all tests in a test case.  Using a test fixture avoids duplicating
// the test code necessary to initialize and cleanup those common
// objects for each test.  It is also useful for defining sub-routines
// that your tests need to invoke a lot.
//
// 在这个例子中，我们将会用到Google Test一个更为高级的特性，被称为测试固件。
// 测试固件的用途主要用于存放所有测试集都要使用的对象或者函数。
//
// 使用测试固件避免了重复一些代码，而这些代码对于初始化或者清除测试用例中
// 的对象又是必须的。同时，测试固件对于定义你测试中需要调起的子例程也很有帮助
// <TechnicalDetails>
//
// The tests share the test fixture in the sense of code sharing, not
// data sharing.  Each test is given its own fresh copy of the
// fixture.  You cannot expect the data modified by one test to be
// passed on to another test, which is a bad idea.
//
// The reason for this design is that tests should be independent and
// repeatable.  In particular, a test should not fail as the result of
// another test's failure.  If one test depends on info produced by
// another test, then the two tests should really be one big test.
//
// The macros for indicating the success/failure of a test
// (EXPECT_TRUE, FAIL, etc) need to know what the current test is
// (when Google Test prints the test result, it tells you which test
// each failure belongs to).  Technically, these macros invoke a
// member function of the Test class.  Therefore, you cannot use them
// in a global function.  That's why you should put test sub-routines
// in a test fixture.
//
//技术细节：
//
//这些测试用例测试固件，在一定意义上是指的代码的共享，和不是数据的共享。
//这样设计的原因是，这些测试应该是独立以及可重复的。特别是当一个测试失败时
//不应该影响其他测试的执行。如果一个测试依赖于另一个测试所产生的结果，那么
//这两个测试应该合并为一个大的测试。
//
//指示测试成功(EXPECT_TRUE)或者失败(EXPECT_FAIL)的宏需要知道当前的测试是什
//么(当Goole Test 打印失败的结果时，它告诉你这个失败从属于那个测试),从技术
//上说，这些宏将调用一个Test类的成员函数。因此你不能在全局的函数中使用这些
//宏，这就是你为什么要把测试子历程放入测试固件中的原因。
//
// </TechnicalDetails>

#include "sample3-inl.h"
#include "gtest/gtest.h"

// To use a test fixture, derive a class from testing::Test.
// 为了使用测试固件，须从Test类中派生一个类
//
class QueueTest : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.
             // 你应该将成员变量声明为保护，这样它们将只能被子类访问

  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the varaibles.
  // Otherwise, this can be skipped.
  //
  // 虚函数Setup(),在每一个测试运行之前就被调用，所以如果你需要初始化一些
  // 变量，你应该在这个地方进行定义，不需要可以跳过
  virtual void SetUp() {
    q1_.Enqueue(1);
    q2_.Enqueue(2);
    q2_.Enqueue(3);
  }

  // virtual void TearDown() will be called after each test is run.
  // You should define it if there is cleanup work to do.  Otherwise,
  // you don't have to provide it.
  //
  // virtual void TearDown() {
  // }
  //
  // 虚函数TearDown()在每一个测试运行之后被调用，如果你需要做一些清理工作
  // 可以在这个函数中定义。否则你可以忽略它

  // A helper function that some test uses.
  // 帮助函数用于测试
  static int Double(int n) {
    return 2*n;
  }

  // A helper function for testing Queue::Map().
  // 帮助函数用于测试 Queue::Map()
  //
  void MapTester(const Queue<int> * q) {
    // Creates a new queue, where each element is twice as big as the
    // corresponding one in q.
    // 生成一个新的队列，队列中的元素是原队列元素的两倍
    const Queue<int> * const new_q = q->Map(Double);

    // Verifies that the new queue has the same size as q.
    // 判断元队列与新队列是否有相同的大小
    ASSERT_EQ(q->Size(), new_q->Size());

    // Verifies the relationship between the elements of the two queues.
    // 验证两个队列中，元素的关系
    for ( const QueueNode<int> * n1 = q->Head(), * n2 = new_q->Head();
          n1 != NULL; n1 = n1->next(), n2 = n2->next() ) {
      EXPECT_EQ(2 * n1->element(), n2->element());
    }

    delete new_q;
  }

  // Declares the variables your tests want to use.
  // 声明测试中需要用到的变量
  Queue<int> q0_;
  Queue<int> q1_;
  Queue<int> q2_;
};

// When you have a test fixture, you define a test using TEST_F
// instead of TEST.
// 当你定义了测试固件时，应该使用TEST_F来代替TEST

// Tests the default c'tor.
// 测试默认构造函数
TEST_F(QueueTest, DefaultConstructor) {
  // You can access data in the test fixture here.
  EXPECT_EQ(0u, q0_.Size());
}

// Tests Dequeue().
// 测试出队列
TEST_F(QueueTest, Dequeue) {
  int * n = q0_.Dequeue();
  EXPECT_TRUE(n == NULL);

  n = q1_.Dequeue();
  ASSERT_TRUE(n != NULL);
  EXPECT_EQ(1, *n);
  EXPECT_EQ(0u, q1_.Size());
  delete n;

  n = q2_.Dequeue();
  ASSERT_TRUE(n != NULL);
  EXPECT_EQ(2, *n);
  EXPECT_EQ(1u, q2_.Size());
  delete n;
}

// Tests the Queue::Map() function.
// 测试队列中的Map()函数
TEST_F(QueueTest, Map) {
  MapTester(&q0_);
  MapTester(&q1_);
  MapTester(&q2_);
}
