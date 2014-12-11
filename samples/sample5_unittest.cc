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
//
// Author: wan@google.com (Zhanyong Wan)

// This sample teaches how to reuse a test fixture in multiple test
// cases by deriving sub-fixtures from it.
// 
// 这个例子主要用于展示，如何通过从测试固件中派生子固件，在多重测试中重用
// 测试固件。
//
// When you define a test fixture, you specify the name of the test
// case that will use this fixture.  Therefore, a test fixture can
// be used by only one test case.
//
// 当你定义一个测试固件时，你确定这个使用测试固件的测试用例集的名称，因此
// 一个测试固件只能够被一个测试用例集所使用
//
// Sometimes, more than one test cases may want to use the same or
// slightly different test fixtures.  For example, you may want to
// make sure that all tests for a GUI library don't leak important
// system resources like fonts and brushes.  In Google Test, you do
// this by putting the shared logic in a super (as in "super class")
// test fixture, and then have each test case use a fixture derived
// from this super fixture.
//
// 有些时候，不止一个测试用例集需要使用相同或者有细微差异的测试固件。例如
// 你想确定，针对一个GUI库的所有测试用例集都没有泄漏重要的系统资源，如字体
// 和话刷。在Google Test中，你可以把共享的逻辑放在一个超级测试固件中，然后
// 每一个测试用例集所使用的测试固件都可以从这个超级测试固件类中派生。

#include <limits.h>
#include <time.h>
#include "sample3-inl.h"
#include "gtest/gtest.h"
#include "sample1.h"

// In this sample, we want to ensure that every test finishes within
// ~5 seconds.  If a test takes longer to run, we consider it a
// failure.
//
// We put the code for timing a test in a test fixture called
// "QuickTest".  QuickTest is intended to be the super fixture that
// other fixtures derive from, therefore there is no test case with
// the name "QuickTest".  This is OK.
//
// Later, we will derive multiple test fixtures from QuickTest.
//
// 在这里例子中，我们需要确保每一个测试都在5s中结束。如果一个测试运行的时间
// 长于5s，我们就认为这个测试失败了
//
// 我们在测试固件中放置计算测试花费时间的代码，并将其称为QuickTest。QuickTest
// 作为超级测试固件而存在，而后续的测试固件都从其派生，所以并没有测试用例集所
// 绑定的测试固件被命名为QuickTest,之后我们将从QuickTest类中派生出多个测试固件
//
class QuickTest : public testing::Test {
 protected:
  // Remember that SetUp() is run immediately before a test starts.
  // This is a good place to record the start time.
  // 记住SetUp()函数，在一个测试开始之前执行，所以在其内部可以用来记录开始时间
  //
  virtual void SetUp() {
    start_time_ = time(NULL);
  }

  // TearDown() is invoked immediately after a test finishes.  Here we
  // check if the test was too slow.
  //
  // TearDown()函数在一个测试结束之后被调起，所以在这里我们检测一个测试是否太慢
  //
  virtual void TearDown() {
    // Gets the time when the test finishes
    const time_t end_time = time(NULL);

    // Asserts that the test took no more than ~5 seconds.  Did you
    // know that you can use assertions in SetUp() and TearDown() as
    // well?
    //
    // 下面是断言的结果
    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long.";
  }

  // The UTC time (in seconds) when the test starts
  // 超级固件定义的开始时间成员变量
  time_t start_time_;
};


// We derive a fixture named IntegerFunctionTest from the QuickTest
// fixture.  All tests using this fixture will be automatically
// required to be quick.
//
// 我们从QuickTest超级固件中派生出一个名为IntergerFunctionTest的测试固件
// 所有使用这个测试固件的测试，都将自动计算测试运行所话时间
//
class IntegerFunctionTest : public QuickTest {
  // We don't need any more logic than already in the QuickTest fixture.
  // Therefore the body is empty.
  // 由于不需要添加新的测试逻辑，所以此函数体为空
  //
};


// Now we can write tests in the IntegerFunctionTest test case.
// 现在在IntegerFunctionTest测试集中，编写测试用例
//
// Tests Factorial()
TEST_F(IntegerFunctionTest, Factorial) {
  // Tests factorial of negative numbers.
  //sleep(6);
  EXPECT_EQ(1, Factorial(-5));
  EXPECT_EQ(1, Factorial(-1));
  EXPECT_GT(Factorial(-10), 0);

  // Tests factorial of 0.
  EXPECT_EQ(1, Factorial(0));

  // Tests factorial of positive numbers.
  EXPECT_EQ(1, Factorial(1));
  EXPECT_EQ(2, Factorial(2));
  EXPECT_EQ(6, Factorial(3));
  EXPECT_EQ(40320, Factorial(8));
}


// Tests IsPrime()
TEST_F(IntegerFunctionTest, IsPrime) {
  // Tests negative input.
  EXPECT_FALSE(IsPrime(-1));
  EXPECT_FALSE(IsPrime(-2));
  EXPECT_FALSE(IsPrime(INT_MIN));

  // Tests some trivial cases.
  EXPECT_FALSE(IsPrime(0));
  EXPECT_FALSE(IsPrime(1));
  EXPECT_TRUE(IsPrime(2));
  EXPECT_TRUE(IsPrime(3));

  // Tests positive input.
  EXPECT_FALSE(IsPrime(4));
  EXPECT_TRUE(IsPrime(5));
  EXPECT_FALSE(IsPrime(6));
  EXPECT_TRUE(IsPrime(23));
}


// The next test case (named "QueueTest") also needs to be quick, so
// we derive another fixture from QuickTest.
//
// The QueueTest test fixture has some logic and shared objects in
// addition to what's in QuickTest already.  We define the additional
// stuff inside the body of the test fixture, as usual.
//
// 同样，可以从超级固件中派生出另一个测试固件用于队列测试，QueueTest固件
// 还有一些其他的逻辑，以及共享的对象除了计算测试时间之外，所以我们像平常
// 一样在测试固件内部进行定义
//
class QueueTest : public QuickTest {
 protected:
  virtual void SetUp() {
    // First, we need to set up the super fixture (QuickTest).
    // 首先需要调用超级固件的setup，这个地方由于不是指针或者引用调用，所以
    // 不涉及多态
    QuickTest::SetUp();

    // Second, some additional setup for this fixture.
    q1_.Enqueue(1);
    q2_.Enqueue(2);
    q2_.Enqueue(3);
  }

  // By default, TearDown() inherits the behavior of
  // QuickTest::TearDown().  As we have no additional cleaning work
  // for QueueTest, we omit it here.
  //
  // 之前已有翻译，这里不再赘述
  //
  // virtual void TearDown() {
  //   QuickTest::TearDown();
  // }

  Queue<int> q0_;
  Queue<int> q1_;
  Queue<int> q2_;
};


// Now, let's write tests using the QueueTest fixture.

// Tests the default constructor.
TEST_F(QueueTest, DefaultConstructor) {
  EXPECT_EQ(0u, q0_.Size());
}

// Tests Dequeue().
TEST_F(QueueTest, Dequeue) {
  int* n = q0_.Dequeue();
  EXPECT_TRUE(n == NULL);

  n = q1_.Dequeue();
  EXPECT_TRUE(n != NULL);
  EXPECT_EQ(1, *n);
  EXPECT_EQ(0u, q1_.Size());
  delete n;

  n = q2_.Dequeue();
  EXPECT_TRUE(n != NULL);
  EXPECT_EQ(2, *n);
  EXPECT_EQ(1u, q2_.Size());
  delete n;
}

// If necessary, you can derive further test fixtures from a derived
// fixture itself.  For example, you can derive another fixture from
// QueueTest.  Google Test imposes no limit on how deep the hierarchy
// can be.  In practice, however, you probably don't want it to be too
// deep as to be confusing.
//
// 如果必要的话，你也可以从已派生的固件进行再次的派生。例如你可以从QueueTest
// 这个固件中再次派生。GoogleTest对你究竟从中派生多少次并没有限制，但是派生的
// 层次太深，则容易使人迷惑
//
//
