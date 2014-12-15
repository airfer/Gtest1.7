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
// Author: vladl@google.com (Vlad Losev)

// This sample shows how to test code relying on some global flag variables.
// Combine() helps with generating all possible combinations of such flags,
// and each test is given one combination as a parameter.
//
// 这个例子展示了如何测试依赖全局标志变量的代码。combine（）帮助产生对于全局标志变量的
// 所有可能的组合，每一个测试都给予所有组合中的一个作为参数。

// Use class definitions to test from this header.
//
//
#include "prime_tables.h"

#include "gtest/gtest.h"

#if GTEST_HAS_COMBINE

// Suppose we want to introduce a new, improved implementation of PrimeTable
// which combines speed of PrecalcPrimeTable and versatility of
// OnTheFlyPrimeTable (see prime_tables.h). Inside it instantiates both
// PrecalcPrimeTable and OnTheFlyPrimeTable and uses the one that is more
// appropriate under the circumstances. But in low memory conditions, it can be
// told to instantiate without PrecalcPrimeTable instance at all and use only
// OnTheFlyPrimeTable.
//
// 假设，我们将要引进一种新的、改进的质数表实现，这个质数表的实现结合了PrecalcPrimeTable
// 实现的速度，以及OnTheFlayPrimeTable的灵活性。在这个新的实现中，我们将实例化以上的两种
// 实现，但是在使用时，将根据应用的场景选择其中的一种。在较低内存的情况下，我们将不实例化
// PrecalPrimeTable,而只使用OnTheFlyPrimeTable
//
class HybridPrimeTable : public PrimeTable {
 public:
  HybridPrimeTable(bool force_on_the_fly, int max_precalculated)
      : on_the_fly_impl_(new OnTheFlyPrimeTable),
        precalc_impl_(force_on_the_fly ? NULL :
                          new PreCalculatedPrimeTable(max_precalculated)),
        max_precalculated_(max_precalculated) {}
  virtual ~HybridPrimeTable() {
    delete on_the_fly_impl_;
    delete precalc_impl_;
  }

  virtual bool IsPrime(int n) const {
    if (precalc_impl_ != NULL && n < max_precalculated_)
      return precalc_impl_->IsPrime(n);
    else
      return on_the_fly_impl_->IsPrime(n);
  }

  virtual int GetNextPrime(int p) const {
    int next_prime = -1;
    if (precalc_impl_ != NULL && p < max_precalculated_)
      next_prime = precalc_impl_->GetNextPrime(p);

    return next_prime != -1 ? next_prime : on_the_fly_impl_->GetNextPrime(p);
  }

 private:
  OnTheFlyPrimeTable* on_the_fly_impl_;
  PreCalculatedPrimeTable* precalc_impl_;
  int max_precalculated_;
};

using ::testing::TestWithParam;
using ::testing::Bool;
using ::testing::Values;
using ::testing::Combine;

// To test all code paths for HybridPrimeTable we must test it with numbers
// both within and outside PreCalculatedPrimeTable's capacity and also with
// PreCalculatedPrimeTable disabled. We do this by defining fixture which will
// accept different combinations of parameters for instantiating a
// HybridPrimeTable instance.
//
// 如果要覆盖HybridPrimeTable所有的代码路径，我们必须考虑到PreCalculatedPrimeTable容量大小
// 以及PreCalculatedPrime是否可以使用这个两个因素。我们定义一个测试固件，这个测试固件将接受
// 不同的参数组合用来实例化HybridPrimeTable
//
class PrimeTableTest : public TestWithParam< ::std::tr1::tuple<bool, int> > {
 protected:
  virtual void SetUp() {
    // This can be written as
    //
    // bool force_on_the_fly;
    // int max_precalculated;
    // tie(force_on_the_fly, max_precalculated) = GetParam();
    // 
    // 也可以使用tie()函数来完成下面的工作
    // once the Google C++ Style Guide allows use of ::std::tr1::tie.
    //
    bool force_on_the_fly = ::std::tr1::get<0>(GetParam());
    int max_precalculated = ::std::tr1::get<1>(GetParam());
    table_ = new HybridPrimeTable(force_on_the_fly, max_precalculated);
  }
  virtual void TearDown() {
    delete table_;
    table_ = NULL;
  }
  HybridPrimeTable* table_;
};

TEST_P(PrimeTableTest, ReturnsFalseForNonPrimes) {
  // Inside the test body, you can refer to the test parameter by GetParam().
  // In this case, the test parameter is a PrimeTable interface pointer which
  // we can use directly.
  //
  // 在测试体内，你也可以得到测试参数通过调用GetParam()方法，在这个例子中，测试参数是一个PrimeTable
  // 的指针，我们可以直接使用。----这个我自己也不太理解，感觉不应该是指针才对
  //
  // Please note that you can also save it in the fixture's SetUp() method
  // or constructor and use saved copy in the tests.
  // 请注意，你也可以在Setup()函数或者构造函数中将其保存，并在后续的测试中使用之前保存的副本
  // 

  EXPECT_FALSE(table_->IsPrime(-5));
  EXPECT_FALSE(table_->IsPrime(0));
  EXPECT_FALSE(table_->IsPrime(1));
  EXPECT_FALSE(table_->IsPrime(4));
  EXPECT_FALSE(table_->IsPrime(6));
  EXPECT_FALSE(table_->IsPrime(100));
}

TEST_P(PrimeTableTest, ReturnsTrueForPrimes) {
  EXPECT_TRUE(table_->IsPrime(2));
  EXPECT_TRUE(table_->IsPrime(3));
  EXPECT_TRUE(table_->IsPrime(5));
  EXPECT_TRUE(table_->IsPrime(7));
  EXPECT_TRUE(table_->IsPrime(11));
  EXPECT_TRUE(table_->IsPrime(131));
}

TEST_P(PrimeTableTest, CanGetNextPrime) {
  EXPECT_EQ(2, table_->GetNextPrime(0));
  EXPECT_EQ(3, table_->GetNextPrime(2));
  EXPECT_EQ(5, table_->GetNextPrime(3));
  EXPECT_EQ(7, table_->GetNextPrime(5));
  EXPECT_EQ(11, table_->GetNextPrime(7));
  EXPECT_EQ(131, table_->GetNextPrime(128));
}

// In order to run value-parameterized tests, you need to instantiate them,
// or bind them to a list of values which will be used as test parameters.
// You can instantiate them in a different translation module, or even
// instantiate them several times.
//
// 为了运行值参数化测试，你需要实例化它们或者将其与一系列值的列表绑定，这些值将被用做测试参数
// 你可以在不同的变化模块中实例化它们，甚至可以实例化它们多次
//
// Here, we instantiate our tests with a list of parameters. We must combine
// all variations of the boolean flag suppressing PrecalcPrimeTable and some
// meaningful values for tests. We choose a small value (1), and a value that
// will put some of the tested numbers beyond the capability of the
// PrecalcPrimeTable instance and some inside it (10). Combine will produce all
// possible combinations.
//
// 这里，我们将用一系列的参数来实例化我们的测试。我们必须考虑到所有组合的情况。bool变量表示是否
// 限制PrecalPrimeTable,而其它的值则是对PrecalPrimeTable容量的测试，例如，可以取1表示容量很小
// 也可以取10表示容量很大。经过组合便可以产生大量的可用参数对
//
INSTANTIATE_TEST_CASE_P(MeaningfulTestParameters,           //测试的名字
                        PrimeTableTest,                     //测试固件的名字，test case
                        Combine(Bool(), Values(1,6,10)));   //可能产生的组合

#else

// Google Test may not support Combine() with some compilers. If we
// use conditional compilation to compile out all code referring to
// the gtest_main library, MSVC linker will not link that library at
// all and consequently complain about missing entry point defined in
// that library (fatal error LNK1561: entry point must be
// defined). This dummy test keeps gtest_main linked in.
TEST(DummyTest, CombineIsNotSupportedOnThisPlatform) {}

#endif  // GTEST_HAS_COMBINE
