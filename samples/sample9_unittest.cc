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
// an alternative console output and how to use the UnitTest reflection API
// to enumerate test cases and tests and to inspect their results.
//
// 这个例子展示了，如何使用Google Test 监听API来实现另一种控制台输出，以及如何
// 使用单元测试映射API来枚举测试用例集和测试用例来探测它们的结果
//

#include <stdio.h>

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

// Provides alternative output mode which produces minimal amount of
// information about tests.
//
// 提供另一种输出模式，这种模式产生关于测试用例的最少信息
//
class TersePrinter : public EmptyTestEventListener {
 private:
  // Called before any test activity starts.
  // 在测试活动开始前被调用
  virtual void OnTestProgramStart(const UnitTest& /* unit_test */) {}

  // Called after all test activities have ended.
  // 在所有测试活动结束后被调用
  virtual void OnTestProgramEnd(const UnitTest& unit_test) {
    fprintf(stdout, "TEST %s\n", unit_test.Passed() ? "PASSED" : "FAILED");
    fflush(stdout);
  }

  // Called before a test starts.
  // 在一个测试开始前被调用
  virtual void OnTestStart(const TestInfo& test_info) {
    fprintf(stdout,
            "*** Test %s.%s starting.\n",
            test_info.test_case_name(),
            test_info.name());
    fflush(stdout);
  }

  // Called after a failed assertion or a SUCCEED() invocation.
  // 在测试成功或者失败后被调用
  virtual void OnTestPartResult(const TestPartResult& test_part_result) {
    fprintf(stdout,
            "%s in %s:%d\n%s\n",
            test_part_result.failed() ? "*** Failure" : "Success",
            test_part_result.file_name(),
            test_part_result.line_number(),
            test_part_result.summary());
    fflush(stdout);
  }

  // Called after a test ends.
  // 在测试结束后被调用
  virtual void OnTestEnd(const TestInfo& test_info) {
    fprintf(stdout,
            "*** Test %s.%s ending.\n",
            test_info.test_case_name(),
            test_info.name());
    fflush(stdout);
  }
};  // class TersePrinter


// 现在测试开始
TEST(CustomOutputTest, PrintsMessage) {
  printf("Printing something from the test body...\n");
}

// 人为的制造测试成功的例子
TEST(CustomOutputTest, Succeeds) {
  SUCCEED() << "SUCCEED() has been invoked from here";
}

// 普通测试，用于测试输出过程
TEST(CustomOutputTest, Fails) {
  EXPECT_EQ(1, 2)
      << "This test fails in order to demonstrate alternative failure messages";
}

}  // namespace

int main(int argc, char **argv) {
  InitGoogleTest(&argc, argv);

  bool terse_output = false;
  if (argc > 1 && strcmp(argv[1], "--terse_output") == 0 )
    terse_output = true;
  else
    printf("%s\n", "Run this program with --terse_output to change the way "
           "it prints its output.");

  UnitTest& unit_test = *UnitTest::GetInstance();

  // If we are given the --terse_output command line flag, suppresses the
  // standard output and attaches own result printer.
  //
  // 如果在运行的时候使用 --terse_output命令行标志，这将限制标准输出，并添加我们
  // 自己的结果打印机
  //
  if (terse_output) {
    TestEventListeners& listeners = unit_test.listeners();

    // Removes the default console output listener from the list so it will
    // not receive events from Google Test and won't print any output. Since
    // this operation transfers ownership of the listener to the caller we
    // have to delete it as well.
    //
    // 移除默认的控制台输出监听器，这将导致其不会接受到GoogleTest所发出的事件消息，
    // 并且也不会打印出任何结果。由于这个操作将监听者的拥有权转移到调用者，所以我们
    // 也要将其删除。
    //
    delete listeners.Release(listeners.default_result_printer());

    // Adds the custom output listener to the list. It will now receive
    // events from Google Test and print the alternative output. We don't
    // have to worry about deleting it since Google Test assumes ownership
    // over it after adding it to the list.
    //
    // 向列表中添加自定义的输出监听器，它将接受Google Test所发出的事件消息，并打印输出。
    // 我们不需要担心删除它，原因在于现在监听者的拥有权并不在调用者身上，而是在GoogleTest.
    //
    listeners.Append(new TersePrinter);
  }
  int ret_val = RUN_ALL_TESTS();

  // This is an example of using the UnitTest reflection API to inspect test
  // results. Here we discount failures from the tests we expected to fail.
  //
  // 下面是测试的例子
  int unexpectedly_failed_tests = 0;
  for (int i = 0; i < unit_test.total_test_case_count(); ++i) {
    const TestCase& test_case = *unit_test.GetTestCase(i);
    for (int j = 0; j < test_case.total_test_count(); ++j) {
      const TestInfo& test_info = *test_case.GetTestInfo(j);
      // Counts failed tests that were not meant to fail (those without
      // 'Fails' in the name).
      //
      // 计算本不应该失败的测试失败的数目
      //
      if (test_info.result()->Failed() &&
          strcmp(test_info.name(), "Fails") != 0) {
        unexpectedly_failed_tests++;
      }
    }
  }

  // Test that were meant to fail should not affect the test program outcome.
  if (unexpectedly_failed_tests == 0)
    ret_val = 0;

  return ret_val;
}
