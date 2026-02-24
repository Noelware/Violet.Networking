// 🌺💜 Violet.Networking: C++20 library that provides networking primitives
// Copyright (c) 2026 Noelware, LLC. <team@noelware.org>, et al.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <gtest/gtest.h>
#include <violet/Networking/IPAddress.h>

// NOLINTBEGIN(google-build-using-namespace)
using namespace violet::net;
using namespace violet;
// NOLINTEND(google-build-using-namespace)

TEST(IPAddress, ConstructFromV4)
{
    ip::AddrV4 v4(192, 168, 1, 1);
    auto ip = IPAddress::V4(v4);

    ASSERT_EQ(ip.TypeOf(), IPAddress::Type::V4);
    ASSERT_EQ(ip.AsV4().Unwrap(), v4);
}

TEST(IPAddress, ConstructFromV6)
{
    auto v6 = ip::AddrV6::Localhost();
    auto ip = IPAddress::V6(v6);

    ASSERT_EQ(ip.TypeOf(), IPAddress::Type::V6);
    ASSERT_EQ(ip.AsV6().Unwrap(), v6);
}

TEST(IPAddress, ToStringV4)
{
    IPAddress ip = IPAddress::V4(ip::AddrV4::Localhost());
    ASSERT_EQ(ip.ToString(), "127.0.0.1");
}

TEST(IPAddress, ToStringV6)
{
    IPAddress ip = IPAddress::V6(ip::AddrV6::Localhost());
    ASSERT_EQ(ip.ToString(), "::1");
}

TEST(IPAddress, EqualityOfSameVariants)
{
    // NOLINTBEGIN(readability-identifier-length)
    IPAddress a = IPAddress::V4(ip::AddrV4(10, 0, 0, 1));
    IPAddress b = IPAddress::V4(ip::AddrV4(10, 0, 0, 1));
    IPAddress c = IPAddress::V4(ip::AddrV4(10, 0, 0, 2));
    // NOLINTEND(readability-identifier-length)

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);

    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
}

TEST(IPAddress, EqualityOfDifferentVariants)
{
    IPAddress v4 = IPAddress::V4(ip::AddrV4(127, 0, 0, 1));
    IPAddress v6 = IPAddress::V6(ip::AddrV6::Localhost());

    EXPECT_FALSE(v4 == v6);
    EXPECT_TRUE(v4 != v6);
}

TEST(IPAddress, OrderingOfSameVariantsV4)
{
    // NOLINTBEGIN(readability-identifier-length)
    IPAddress a = IPAddress::V4(ip::AddrV4(10, 0, 0, 1));
    IPAddress b = IPAddress::V4(ip::AddrV4(10, 0, 0, 2));
    // NOLINTEND(readability-identifier-length)

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
}

TEST(IPAddress, OrderingOfSameVariantsV6)
{
    ip::AddrV6 a6(0, 0, 0, 0, 0, 0, 0, 1);
    ip::AddrV6 b6(0, 0, 0, 0, 0, 0, 0, 2);

    // NOLINTBEGIN(readability-identifier-length)
    IPAddress a = IPAddress::V6(a6);
    IPAddress b = IPAddress::V6(b6);
    // NOLINTEND(readability-identifier-length)

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
}

TEST(IPAddress, OrderingOfDifferentVariants)
{
    IPAddress v4 = IPAddress::V4(ip::AddrV4(255, 255, 255, 255));
    IPAddress v6 = IPAddress::V6(ip::AddrV6::Localhost());

    // By design: V4 < V6
    EXPECT_TRUE(v4 < v6);
    EXPECT_TRUE(v6 > v4);
}

TEST(IPAddress, FromStrV4)
{
    auto res = IPAddress::FromStr("192.168.1.1");
    ASSERT_TRUE(res.Ok());

    EXPECT_EQ(res->TypeOf(), IPAddress::Type::V4);
    EXPECT_EQ(res->ToString(), "192.168.1.1");
}

TEST(IPAddress, FromStrV6)
{
    auto res = IPAddress::FromStr("::1");
    ASSERT_TRUE(res.Ok());

    EXPECT_EQ(res->TypeOf(), IPAddress::Type::V6);
    EXPECT_EQ(res->ToString(), "::1");
}

TEST(IPAddress, FromStrInvalid)
{
    auto res = IPAddress::FromStr("not_an_ip");
    EXPECT_FALSE(res.Ok());
}
