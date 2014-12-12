// Copyright 2008 Google Inc.
// All Rights Reserved.
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

// This sample shows how to test common properties of multiple
// implementations of the same interface (aka interface tests).
//
// 这个例子展示如何测试相同接口不同实现的公共性质

// The interface and its implementations are in this header.
// 接口和其实现都在头文件中
#include "prime_tables.h"

#include "gtest/gtest.h"

// First, we define some factory functions for creating instances of
// the implementations.  You may be able to skip this step if all your
// implementations can be constructed the same way.
//
// 首先，我们定义一些工厂函数来产生一些实现的实例，你也可以跳过这一步，
// 如果你的所有实现都以相同的方法构建


//创建质数表,这个是模板函数，不太清楚可以百度一下
template <class T>
PrimeTable* CreatePrimeTable();

//模板特例，类型OnTheFlyPrimeTable
template <>
PrimeTable* CreatePrimeTable<OnTheFlyPrimeTable>() {
  return new OnTheFlyPrimeTable;
}

//模板特例，类型PrecalculatedPrimeTable
template <>
PrimeTable* CreatePrimeTable<PreCalculatedPrimeTable>() {
  return new PreCalculatedPrimeTable(10000);
}

// Then we define a test fixture class template.
// 然后我们定义一个测试固件类模板
template <class T>
class PrimeTableTest : public testing::Test {
 protected:
  // The ctor calls the factory function to create a prime table
  // implemented by T.
  //
  // 构造函数调用工厂函数来生成类型为T的质数表，根据T的类型来调用
  // 普通的模板函数，还是特例的模板函数
  PrimeTableTest() : table_(CreatePrimeTable<T>()) {}

  virtual ~PrimeTableTest() { delete table_; }

  // Note that we test an implementation via the base interface
  // instead of the actual implementation class.  This is important
  // for keeping the tests close to the real world scenario, where the
  // implementation is invoked via the base interface.  It avoids
  // got-yas where the implementation class has a method that shadows
  // a method with the same name (but slightly different argument
  // types) in the base interface, for example.
  //
  // 需要注意的是，我们测试方法的实现是通过基类接口（准确的说是指的虚函数）
  // ，而不是实际已实现的类。现实场景中，实现（准确的说是对虚函数实现重载）
  // 是通过基本的接口来调用的，所以将我们的测试与现实中的测试场景尽可能的保
  // 持一致非常重要。
  //
  // 采用这样的方式避免了这样的情况：实现类中有一个方法，而这个方法与基类中
  // 的某方法同名，这将覆盖掉基类的方法。
  //
  PrimeTable* const table_;
};

#if GTEST_HAS_TYPED_TEST

using testing::Types;

// Google Test offers two ways for reusing tests for different types.
// The first is called "typed tests".  You should use it if you
// already know *all* the types you are gonna exercise when you write
// the tests.
//
// Google Test为重用不同参数类型的测试提供了两种方法，第一种方法被称为
// 类型测试。使用这种方法的前提是，你在写测试之前已经知道了所有可能涉及
// 到的类型。
//

// To write a typed test case, first use
//
//   TYPED_TEST_CASE(TestCaseName, TypeList);
//
// to declare it and specify the type parameters.  As with TEST_F,
// TestCaseName must match the test fixture name.
//
// 写一个类型测试，首先使用
// .......
// 声明它，并指定类型参数，与TEST_F相同，测试用例集名称必须与测试固件名称
// 匹配。

// The list of types we want to test.
// 列举需要测试的类型参数
typedef Types<OnTheFlyPrimeTable, PreCalculatedPrimeTable> Implementations;

TYPED_TEST_CASE(PrimeTableTest, Implementations);

// Then use TYPED_TEST(TestCaseName, TestName) to define a typed test,
// similar to TEST_F.
//
// 使用TYPED_TEST 来定义类型测试，与TEST_F相类似
//
TYPED_TEST(PrimeTableTest, ReturnsFalseForNonPrimes) {
  // Inside the test body, you can refer to the type parameter by
  // TypeParam, and refer to the fixture class by TestFixture.  We
  // don't need them in this example.
  //
  // 在测试函数体内，你可以通过TypeParam来查阅类型参数，以及通过TestFixture
  // 来查阅固件类，在这个例子中不需要这么做

  // Since we are in the template world, C++ requires explicitly
  // writing 'this->' when referring to members of the fixture class.
  // This is something you have to learn to live with.
  //
  // 由于我们在模板的世界中，C++需要显式的使用this->来引用固件类中的成员
  //
  EXPECT_FALSE(this->table_->IsPrime(-5));
  EXPECT_FALSE(this->table_->IsPrime(0));
  EXPECT_FALSE(this->table_->IsPrime(1));
  EXPECT_FALSE(this->table_->IsPrime(4));
  EXPECT_FALSE(this->table_->IsPrime(6));
  EXPECT_FALSE(this->table_->IsPrime(100));
}

TYPED_TEST(PrimeTableTest, ReturnsTrueForPrimes) {
  EXPECT_TRUE(this->table_->IsPrime(2));
  EXPECT_TRUE(this->table_->IsPrime(3));
  EXPECT_TRUE(this->table_->IsPrime(5));
  EXPECT_TRUE(this->table_->IsPrime(7));
  EXPECT_TRUE(this->table_->IsPrime(11));
  EXPECT_TRUE(this->table_->IsPrime(131));
}

TYPED_TEST(PrimeTableTest, CanGetNextPrime) {
  EXPECT_EQ(2, this->table_->GetNextPrime(0));
  EXPECT_EQ(3, this->table_->GetNextPrime(2));
  EXPECT_EQ(5, this->table_->GetNextPrime(3));
  EXPECT_EQ(7, this->table_->GetNextPrime(5));
  EXPECT_EQ(11, this->table_->GetNextPrime(7));
  EXPECT_EQ(131, this->table_->GetNextPrime(128));
}

// That's it!  Google Test will repeat each TYPED_TEST for each type
// in the type list specified in TYPED_TEST_CASE.  Sit back and be
// happy that you don't have to define them multiple times.
//
// 就是这样，Google Test将会对每一个在TYPED_TEST_CASE中确定的类型参数执行TYPED_TEST,
// 下面的不用翻译了吧

#endif  // GTEST_HAS_TYPED_TEST

#if GTEST_HAS_TYPED_TEST_P

using testing::Types;

// Sometimes, however, you don't yet know all the types that you want
// to test when you write the tests.  For example, if you are the
// author of an interface and expect other people to implement it, you
// might want to write a set of tests to make sure each implementation
// conforms to some basic requirements, but you don't know what
// implementations will be written in the future.
//
// 然而有些时候，当你编写测试时，你并不知道所有的类型参数。例如，如果你是一个接口的
// 设计者,并期望其他人来实现它。或者你也可能想写一系列测试并确保每一个实现都遵从相同
// 的基本要求（类似的是上面模板的例子），但是你并不知道这些实现在将来会被写成什么样子。
//
// How can you write the tests without committing to the type
// parameters?  That's what "type-parameterized tests" can do for you.
// It is a bit more involved than typed tests, but in return you get a
// test pattern that can be reused in many contexts, which is a big
// win.  Here's how you do it:
//
// 你要怎样写测试用例而不需要遵从类型参数呢？（就是在类型参数暂且不知道的情况下）。这
// 就是类型参数化测试可以为你做的。这比类型测试要稍微复杂一些，但是反过来你可以得到一
// 个测试模板，而这个模板可以在多个应用场景中被重复使用，这算是一个一劳永逸的办法了。
// 下面是具体的方法：
//

// First, define a test fixture class template.  Here we just reuse
// the PrimeTableTest fixture defined earlier:
//
// 首先，定义一个测试固件类模板，在这里我们还是使用之前定义的PrimeTableTest测试固件

template <class T>
class PrimeTableTest2 : public PrimeTableTest<T> {
};

// Then, declare the test case.  The argument is the name of the test
// fixture, and also the name of the test case (as usual).  The _P
// suffix is for "parameterized" or "pattern".
//
// 然后，声明测试用例集，其参数是测试固件的名字，同时也是测试用集的名字。_P后缀，表
// 示的是参数化，或者模式
TYPED_TEST_CASE_P(PrimeTableTest2);

// Next, use TYPED_TEST_P(TestCaseName, TestName) to define a test,
// similar to what you do with TEST_F.
//
// 接下俩，将使用TYPED_TEST_P(TestCaseName,TestName)来定义一个测试，这与你在TEST_F中
// 所做的很类似
//
TYPED_TEST_P(PrimeTableTest2, ReturnsFalseForNonPrimes) {
  EXPECT_FALSE(this->table_->IsPrime(-5));
  EXPECT_FALSE(this->table_->IsPrime(0));
  EXPECT_FALSE(this->table_->IsPrime(1));
  EXPECT_FALSE(this->table_->IsPrime(4));
  EXPECT_FALSE(this->table_->IsPrime(6));
  EXPECT_FALSE(this->table_->IsPrime(100));
}

TYPED_TEST_P(PrimeTableTest2, ReturnsTrueForPrimes) {
  EXPECT_TRUE(this->table_->IsPrime(2));
  EXPECT_TRUE(this->table_->IsPrime(3));
  EXPECT_TRUE(this->table_->IsPrime(5));
  EXPECT_TRUE(this->table_->IsPrime(7));
  EXPECT_TRUE(this->table_->IsPrime(11));
  EXPECT_TRUE(this->table_->IsPrime(131));
}

TYPED_TEST_P(PrimeTableTest2, CanGetNextPrime) {
  EXPECT_EQ(2, this->table_->GetNextPrime(0));
  EXPECT_EQ(3, this->table_->GetNextPrime(2));
  EXPECT_EQ(5, this->table_->GetNextPrime(3));
  EXPECT_EQ(7, this->table_->GetNextPrime(5));
  EXPECT_EQ(11, this->table_->GetNextPrime(7));
  EXPECT_EQ(131, this->table_->GetNextPrime(128));
}

// Type-parameterized tests involve one extra step: you have to
// enumerate the tests you defined:
//
// 类型参数化测试还包含一个额外的步骤：你需要枚举你所定义的测试
//
REGISTER_TYPED_TEST_CASE_P(
    PrimeTableTest2,  // The first argument is the test case name.
    // The rest of the arguments are the test names.
    //
    // 第一个参数是测试集的名称，其余的参数是测试的名称
    ReturnsFalseForNonPrimes, ReturnsTrueForPrimes, CanGetNextPrime);

// At this point the test pattern is done.  However, you don't have
// any real test yet as you haven't said which types you want to run
// the tests with.
//
// 到现在为止，测试模板已经完成了。然而，你并不能开始真正的测试，因为你
// 还没有确定测试模板将使用那些类型参数。

// To turn the abstract test pattern into real tests, you instantiate
// it with a list of types.  Usually the test pattern will be defined
// in a .h file, and anyone can #include and instantiate it.  You can
// even instantiate it more than once in the same program.  To tell
// different instances apart, you give each of them a name, which will
// become part of the test case name and can be used in test filters.
//
// 为了将抽象的测试模板装换为真实的测试，你需要使用一系列的类型来实例化
// 测试模板。通常测试模板被定义在.h的文件中，任何人都可以通过#Include来
// 包含并实例化它。你甚至可以在同一程序中实例化它不止一次。为了将不同的
// 实例化区分开来，你需要给它们起一个名字，这个名字将称为测试用例集名称
// 的一部分，也可以被使用在测试过滤器中。

// The list of types we want to test.  Note that it doesn't have to be
// defined at the time we write the TYPED_TEST_P()s.
//
// 下面是我们需要测试的类型。需要注意的是，测试的类型列表不必在写
// TYPED_TEST_P的时候定义，可以在之前，也可以在之后（此例为在之后）
typedef Types<OnTheFlyPrimeTable, PreCalculatedPrimeTable>
    PrimeTableImplementations;
INSTANTIATE_TYPED_TEST_CASE_P(OnTheFlyAndPreCalculated,    // Instance name  实例的名字
                              PrimeTableTest2,             // Test case name 测试集的名字
                         
                              PrimeTableImplementations);  // Type list      类型参数列表

#endif  // GTEST_HAS_TYPED_TEST_P
