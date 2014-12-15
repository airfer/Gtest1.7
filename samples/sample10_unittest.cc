// Copyright 2009 Google Inc. All Rights Reserved.
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
//
// Author: vladl@google.com (Vlad Losev)

// This sample shows how to use Google Test listener API to implement
// a primitive leak checker.
//
// 这个例子展示了如何使用GoogleTest监听API来实现最初的内存泄漏检测
//

#include <stdio.h>
#include <stdlib.h>

#include "gtest/gtest.h"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;

namespace {

// We will track memory used by this class.
// 在这个例子中，我们将会使用这个类来追踪内存的消耗情况
//
class Water {
 public:
  // Normal Water declarations go here.

  // 使用allocated变量来控制内存池的分配情况
  //
  // operator new and operator delete help us control water allocation.
  void* operator new(size_t allocation_size) {
    allocated_++;
    return malloc(allocation_size);
  }

  void operator delete(void* block, size_t /* allocation_size */) {
    allocated_--;
    free(block);
  }

  static int allocated() { return allocated_; }

 private:
  static int allocated_;
};

int Water::allocated_ = 0;

// This event listener monitors how many Water objects are created and
// destroyed by each test, and reports a failure if a test leaks some Water
// objects. It does this by comparing the number of live Water objects at
// the beginning of a test and at the end of a test.
//
// 这个事件监听者监听在每个测试中，共有多少资源对象被生成以及多少资源对象被销
// 毁，如果出现资源泄漏就会报告失败。其实现的过程就是通过对比测试开始以及结束
// 资源对象数目变化
class LeakChecker : public EmptyTestEventListener {
 private:
  // Called before a test starts.
  // 在测试开始之前调用
  virtual void OnTestStart(const TestInfo& /* test_info */) {
    initially_allocated_ = Water::allocated();
  }

  // Called after a test ends.
  // 在测试开始之后调用
  virtual void OnTestEnd(const TestInfo& /* test_info */) {
    int difference = Water::allocated() - initially_allocated_;

    // You can generate a failure in any event handler except
    // OnTestPartResult. Just use an appropriate Google Test assertion to do
    // it.
    //
    // 当资源对象数不相等的时候，出错内存泄漏
    printf("Difference: %d\n",difference);
    EXPECT_LE(difference, 0) << "Leaked " << difference << " unit(s) of Water!";
  }

  int initially_allocated_;
};

TEST(ListenersTest, DoesNotLeak) {
  Water* water = new Water;
  delete water;
}

// This should fail when the --check_for_leaks command line flag is
// specified.
// 使用--check_for_leaks命令行进行检测
TEST(ListenersTest, LeaksWater) {
  Water* water = new Water;
  EXPECT_TRUE(water != NULL);
}

}  // namespace

int main(int argc, char **argv) {
  InitGoogleTest(&argc, argv);

  bool check_for_leaks = false;
  if (argc > 1 && strcmp(argv[1], "--check_for_leaks") == 0 )
    check_for_leaks = true;
  else
    printf("%s\n", "Run this program with --check_for_leaks to enable "
           "custom leak checking in the tests.");

  // If we are given the --check_for_leaks command line flag, installs the
  // leak checker.
  if (check_for_leaks) {
      //和sample9，是用同样的方法得到监听器
    TestEventListeners& listeners = UnitTest::GetInstance()->listeners();

    // Adds the leak checker to the end of the test event listener list,
    // after the default text output printer and the default XML report
    // generator
    //
    // 将内存泄漏检测放在测试事件监听列表的末端，在默认测试输出打印机，以及
    // 默认XML报告生成器之后
    //
    // The order is important - it ensures that failures generated in the
    // leak checker's OnTestEnd() method are processed by the text and XML
    // printers *before* their OnTestEnd() methods are called, such that
    // they are attributed to the right test. Remember that a listener
    // receives an OnXyzStart event *after* listeners preceding it in the
    // list received that event, and receives an OnXyzEnd event *before*
    // listeners preceding it.
    //
    // 事件监听器的顺序十分重要，它保证了在OnTestEnd方法中所产生的错误能够在OnTestEnd
    // 方法被调用之前得到处理，这有助于正确的测试。记住，在监听器列表中，顺序很重要。
    // 例如有Listener1和Listener2, Listen1在监听例表中顺序早于Listener2。那么接收OnStart
    // 时间消息是Listner1更早一些，但是接收OnEnd事件消息则是Listener2更早一些。
    //
    // We don't need to worry about deleting the new listener later, as
    // Google Test will do it.
    //
    // 我们不需要担心在之后删除新的监听器，因为GoogleTest会为你做这一点的
    listeners.Append(new LeakChecker);
  }
  return RUN_ALL_TESTS();
}
