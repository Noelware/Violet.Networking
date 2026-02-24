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
#include <violet/Networking/IP/AddrV4.h>

// NOLINTBEGIN(google-build-using-namespace)
using namespace violet::net::ip;
using namespace violet;
// NOLINTEND(google-build-using-namespace)

TEST(AddrV4, ParseValidAddresses)
{
    auto first = AddrV4::FromStr("192.168.1.1");
    ASSERT_TRUE(first);
    EXPECT_EQ(first->ToString(), "192.168.1.1");

    auto second = AddrV4::FromStr("0.0.0.0");
    ASSERT_TRUE(second);
    EXPECT_EQ(second->ToString(), "0.0.0.0");

    auto third = AddrV4::FromStr("255.255.255.255");
    ASSERT_TRUE(third);
    EXPECT_EQ(third->ToString(), "255.255.255.255");
}

TEST(AddrV4, ParseInvalidAddresses)
{
    EXPECT_FALSE(AddrV4::FromStr("256.0.0.1"));
    EXPECT_FALSE(AddrV4::FromStr("192.168.1"));
    EXPECT_FALSE(AddrV4::FromStr("192.168.1.1.1"));
    EXPECT_FALSE(AddrV4::FromStr("abc.def.ghi.jkl"));
}

TEST(AddrV4, UInt32Conversion)
{
    UInt32 num = 0xC0A80101; // 192.168.1.1
    auto ip = AddrV4::FromUInt32(num);
    ASSERT_EQ(ip.AsUInt32(), num);
    ASSERT_EQ(ip.ToString(), "192.168.1.1");
}

TEST(AddrV4, SpecialAddresses)
{
    auto broadcast = AddrV4::Broadcast();
    EXPECT_TRUE(broadcast.IsBroadcast());
    EXPECT_FALSE(broadcast.Unspecified());

    auto unspec = AddrV4::FromUInt32(0);
    EXPECT_TRUE(unspec.Unspecified());

    auto loopback = AddrV4::FromStr("127.1.2.3").Unwrap();
    EXPECT_TRUE(loopback.Loopback());
    EXPECT_FALSE(loopback.Private());

    auto private1 = AddrV4::FromStr("10.0.0.1").Unwrap();
    EXPECT_TRUE(private1.Private());

    auto private2 = AddrV4::FromStr("172.16.5.1").Unwrap();
    EXPECT_TRUE(private2.Private());

    auto private3 = AddrV4::FromStr("192.168.1.1").Unwrap();
    EXPECT_TRUE(private3.Private());

    auto link = AddrV4::FromStr("169.254.1.1").Unwrap();
    EXPECT_TRUE(link.LinkLocal());

    auto multicast = AddrV4::FromStr("224.0.0.1").Unwrap();
    EXPECT_TRUE(multicast.Multicast());
}

TEST(AddrV4, GlobalAddresses)
{
    auto ip = AddrV4::FromStr("8.8.8.8").Unwrap();
    EXPECT_TRUE(ip.Global());
    EXPECT_FALSE(ip.Private());
    EXPECT_FALSE(ip.Loopback());
}

TEST(AddrV4, OctetArrayConversion)
{
    auto ip = AddrV4::FromStr("192.168.1.1").Unwrap();
    auto octs = ip.Octets();
    EXPECT_EQ(octs[0], 192);
    EXPECT_EQ(octs[1], 168);
    EXPECT_EQ(octs[2], 1);
    EXPECT_EQ(octs[3], 1);

    auto arr = static_cast<Array<UInt8, 4>>(ip);
    EXPECT_EQ(arr, octs);
}

TEST(AddrV4, Localhost)
{
    auto loop = AddrV4::Localhost();
    ASSERT_TRUE(loop.Loopback());
    ASSERT_EQ(loop.ToString(), "127.0.0.1");
}
