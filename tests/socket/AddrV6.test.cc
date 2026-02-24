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
#include <violet/Networking/Socket/AddrV6.h>

// NOLINTBEGIN(google-build-using-namespace)
using namespace violet::net;
using namespace violet;
// NOLINTEND(google-build-using-namespace)

TEST(SocketAddrV6, ConstructAndAccess)
{
    ip::AddrV6 ip = ip::AddrV6::Localhost();
    socket::AddrV6 socket(ip, 8080);

    ASSERT_EQ(socket.Address, ip);
    ASSERT_EQ(socket.Port, 8080);
}

TEST(SocketAddrV6, Stringify)
{
    auto address = ip::AddrV6::Localhost();
    socket::AddrV6 socket(address, 80);

    ASSERT_EQ(socket.ToString(), "[::1]:80");
}

TEST(SocketAddrV6, EqualityOperators)
{
    ip::AddrV6 ip1(0, 0, 0, 0, 0, 0, 0, 1);
    ip::AddrV6 ip2(0, 0, 0, 0, 0, 0, 0, 2);

    socket::AddrV6 sock1(ip1, 1234);
    socket::AddrV6 sock2(ip1, 1234);
    socket::AddrV6 sock3(ip1, 5678);
    socket::AddrV6 sock4(ip2, 1234);

    EXPECT_TRUE(sock1 == sock2);
    EXPECT_FALSE(sock1 != sock2);

    EXPECT_FALSE(sock1 == sock3);
    EXPECT_TRUE(sock1 != sock3);

    EXPECT_FALSE(sock1 == sock4);
    EXPECT_TRUE(sock1 != sock4);
}

TEST(SocketAddrV6, ComparisonOperators)
{
    ip::AddrV6 ip1(0, 0, 0, 0, 0, 0, 0, 1);
    ip::AddrV6 ip2(0, 0, 0, 0, 0, 0, 0, 2);

    // NOLINTBEGIN(readability-identifier-length)
    socket::AddrV6 a(ip1, 80);
    socket::AddrV6 b(ip1, 443);
    socket::AddrV6 c(ip2, 80);
    // NOLINTEND(readability-identifier-length)

    EXPECT_TRUE(a < b); // port difference
    EXPECT_TRUE(b < c); // IP difference
    EXPECT_TRUE(c > a);
}
