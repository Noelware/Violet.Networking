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
#include <violet/Networking/SocketAddress.h>

// NOLINTBEGIN(google-build-using-namespace)
using namespace violet::net;
using namespace violet;
// NOLINTEND(google-build-using-namespace)

TEST(SocketAddress, ConstructFromV4)
{
    socket::AddrV4 v4(ip::AddrV4(192, 168, 1, 1), 8080);
    auto address = SocketAddress::V4(v4);

    ASSERT_EQ(address.TypeOf(), SocketAddress::Type::V4);
    ASSERT_EQ(address.AsV4().Unwrap(), v4);
}

TEST(SocketAddress, ConstructFromV6)
{
    socket::AddrV6 v6(ip::AddrV6::Localhost(), 8080);
    auto address = SocketAddress::V6(v6);

    ASSERT_EQ(address.TypeOf(), SocketAddress::Type::V6);
    ASSERT_EQ(address.AsV6().Unwrap(), v6);
}

TEST(SocketAddress, ToStringV4)
{
    SocketAddress addr = SocketAddress::V4(socket::AddrV4(ip::AddrV4(127, 0, 0, 1), 80));
    EXPECT_EQ(addr.ToString(), "127.0.0.1:80");
}

TEST(SocketAddress, ToStringV6)
{
    SocketAddress addr = SocketAddress::V6(socket::AddrV6(ip::AddrV6::Localhost(), 80));
    EXPECT_EQ(addr.ToString(), "[::1]:80");
}

TEST(SocketAddress, EqualitySameVariant)
{
    // NOLINTBEGIN(readability-identifier-length)
    SocketAddress a = SocketAddress::V4(socket::AddrV4(ip::AddrV4(10, 0, 0, 1), 80));
    SocketAddress b = SocketAddress::V4(socket::AddrV4(ip::AddrV4(10, 0, 0, 1), 80));
    SocketAddress c = SocketAddress::V4(socket::AddrV4(ip::AddrV4(10, 0, 0, 1), 443));
    // NOLINTEND(readability-identifier-length)

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);

    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
}

TEST(SocketAddress, EqualityDifferentVariants)
{
    SocketAddress v4 = SocketAddress::V4(socket::AddrV4(ip::AddrV4(127, 0, 0, 1), 80));
    SocketAddress v6 = SocketAddress::V6(socket::AddrV6(ip::AddrV6::Localhost(), 80));

    EXPECT_FALSE(v4 == v6);
    EXPECT_TRUE(v4 != v6);
}

TEST(SocketAddress, OrderingSameVariant)
{
    // NOLINTBEGIN(readability-identifier-length)
    SocketAddress a = SocketAddress::V4(socket::AddrV4(ip::AddrV4(10, 0, 0, 1), 80));
    SocketAddress b = SocketAddress::V4(socket::AddrV4(ip::AddrV4(10, 0, 0, 1), 443));
    SocketAddress c = SocketAddress::V4(socket::AddrV4(ip::AddrV4(10, 0, 0, 2), 80));
    // NOLINTEND(readability-identifier-length)

    EXPECT_TRUE(a < b); // port compare
    EXPECT_TRUE(b < c); // IP compare
    EXPECT_TRUE(c > a);
}

TEST(SocketAddress, OrderingDifferentVariants)
{
    SocketAddress v4 = SocketAddress::V4(socket::AddrV4(ip::AddrV4(255, 255, 255, 255), 65535));
    SocketAddress v6 = SocketAddress::V6(socket::AddrV6(ip::AddrV6::Localhost(), 1));

    // Design: V4 < V6
    EXPECT_TRUE(v4 < v6);
    EXPECT_TRUE(v6 > v4);
}

TEST(SocketAddress, FromStrValidV4)
{
    auto res = SocketAddress::FromStr("192.168.1.1:8080");
    ASSERT_TRUE(res.Ok());

    EXPECT_EQ(res->TypeOf(), SocketAddress::Type::V4);
    EXPECT_EQ(res->ToString(), "192.168.1.1:8080");
}

TEST(SocketAddress, FromStrValidV6)
{
    auto res = SocketAddress::FromStr("[::1]:8080");
    ASSERT_TRUE(res.Ok());

    EXPECT_EQ(res->TypeOf(), SocketAddress::Type::V6);
    EXPECT_EQ(res->ToString(), "[::1]:8080");
}

TEST(SocketAddress, FromStrInvalidIP)
{
    auto res = SocketAddress::FromStr("invalid:80");
    EXPECT_FALSE(res.Ok());
}

TEST(SocketAddress, FromStrInvalidPort)
{
    auto res = SocketAddress::FromStr("127.0.0.1:notaport");
    EXPECT_FALSE(res.Ok());
}

TEST(SocketAddress, FromStrMissingPort)
{
    auto res = SocketAddress::FromStr("127.0.0.1");
    EXPECT_TRUE(res.Ok());
}

TEST(SocketAddress, FromStrMissingPortV6)
{
    auto res = SocketAddress::FromStr("[::1]");
    EXPECT_TRUE(res.Ok());
}
