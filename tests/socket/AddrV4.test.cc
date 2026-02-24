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
#include <violet/Networking/Socket/AddrV4.h>

// NOLINTBEGIN(google-build-using-namespace)
using namespace violet::net;
using namespace violet;
// NOLINTEND(google-build-using-namespace)

TEST(SocketAddrV4, ConstructAndAccess)
{
    ip::AddrV4 addr(192, 168, 1, 42);
    socket::AddrV4 socket(addr, 8080);

    ASSERT_EQ(socket.Address, addr);
    ASSERT_EQ(socket.Port, 8080);
}

TEST(SocketAddrV4, Stringify)
{
    auto address = ip::AddrV4::Localhost();
    socket::AddrV4 socket(address, 80);

    ASSERT_EQ(socket.ToString(), "127.0.0.1:80");
}

TEST(SocketAddrV4, Equality)
{
    ip::AddrV4 ip1(10, 0, 0, 1);
    ip::AddrV4 ip2(10, 0, 0, 2);

    socket::AddrV4 sock1(ip1, 1234);
    socket::AddrV4 sock2(ip1, 1234);
    socket::AddrV4 sock3(ip1, 5678);
    socket::AddrV4 sock4(ip2, 1234);

    EXPECT_TRUE(sock1 == sock2);
    EXPECT_FALSE(sock1 != sock2);

    EXPECT_FALSE(sock1 == sock3);
    EXPECT_TRUE(sock1 != sock3);

    EXPECT_FALSE(sock1 == sock4);
    EXPECT_TRUE(sock1 != sock4);
}

TEST(SocketAddrV4, Comparison)
{
    ip::AddrV4 ip1(10, 0, 0, 1);
    ip::AddrV4 ip2(10, 0, 0, 2);

    // NOLINTBEGIN(readability-identifier-length)
    socket::AddrV4 a(ip1, 80);
    socket::AddrV4 b(ip1, 443);
    socket::AddrV4 c(ip2, 80);
    // NOLINTEND(readability-identifier-length)

    EXPECT_TRUE(a < b); // port difference
    EXPECT_TRUE(b < c); // IP difference
    EXPECT_TRUE(c > a);
}
