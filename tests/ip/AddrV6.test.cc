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
#include <violet/Networking/IP/AddrV6.h>

// NOLINTBEGIN(google-build-using-namespace)
using namespace violet::net::ip;
using namespace violet;
// NOLINTEND(google-build-using-namespace)

TEST(AddrV6, Defaults)
{
    AddrV6 v6;
    ASSERT_TRUE(v6.Unspecified());
    ASSERT_FALSE(v6.Loopback());
    ASSERT_FALSE(v6.Multicast());
}

TEST(AddrV6, LoopbackAndUnspecified)
{
    auto loop = AddrV6::Localhost();
    ASSERT_TRUE(loop.Loopback());
    EXPECT_FALSE(loop.Unspecified());

    AddrV6 unspec;
    ASSERT_TRUE(unspec.Unspecified());
    EXPECT_FALSE(unspec.Loopback());
}

TEST(AddrV6, Multicast)
{
    Array<UInt8, 16> multicast_{};
    multicast_[0] = 0xFF;

    AddrV6 multicast(multicast_);
    ASSERT_TRUE(multicast.Multicast());
    EXPECT_FALSE(multicast.Unicast());
}

TEST(AddrV6, IPv4MappedDetection)
{
    Array<UInt8, 16> mapped{};
    mapped[10] = 0xFF;
    mapped[11] = 0xFF;
    mapped[12] = 192;
    mapped[13] = 0;
    mapped[14] = 2;
    mapped[15] = 128;

    AddrV6 ip4(mapped);
    EXPECT_TRUE(ip4.IPv4Mapped());
    EXPECT_EQ(ip4.ToString(), "::ffff:192.0.2.128");
}

TEST(AddrV6, RFC5952Formatting)
{
    AddrV6 addr(0x2001, 0x0db8, 0, 0, 0, 0, 0, 1);
    EXPECT_EQ(addr.ToString(), "2001:db8::1");

    AddrV6 addr2(0, 0, 0, 0, 0, 0, 0, 1);
    EXPECT_EQ(addr2.ToString(), "::1");

    AddrV6 addr3(0xfe80, 0, 0, 0, 0x0202, 0xb3ff, 0xfe1e, 0x8329);
    EXPECT_EQ(addr3.ToString(), "fe80::202:b3ff:fe1e:8329");
}

TEST(AddrV6, FromStrValid)
{
    auto res = AddrV6::FromStr("2001:db8::1");
    ASSERT_TRUE(res.Ok()) << "failed to parse: " << res.Error();
    EXPECT_EQ(res->ToString(), "2001:db8::1");

    auto res2 = AddrV6::FromStr("::ffff:192.0.2.128");
    ASSERT_TRUE(res2.Ok()) << "failed to parse: " << res2.Error();
    EXPECT_TRUE(res2->IPv4Mapped());
    EXPECT_EQ(res2->ToString(), "::ffff:192.0.2.128");
}

TEST(AddrV6, FromStrInvalid)
{
    auto res = AddrV6::FromStr("2001:db8:::1");
    ASSERT_TRUE(res.Err());

    auto res2 = AddrV6::FromStr("12345::1");
    ASSERT_TRUE(res2.Err());

    auto res3 = AddrV6::FromStr("1:2:3:4:5:6:7"); // only 7 hextets
    ASSERT_TRUE(res3.Err());
}

// TEST(AddrV6, UnicastVariants) {
//     AddrV6 linkLocal(0xfe80,0,0,0,0,0,0,1);
//     EXPECT_TRUE(linkLocal.Unicast());
//     EXPECT_TRUE(linkLocal.UnicastLinkLocal());
//     EXPECT_FALSE(linkLocal.UnicastGlobal());

//     AddrV6 uniqueLocal(0xfc00,0,0,0,0,0,0,1);
//     EXPECT_TRUE(uniqueLocal.IsUnicast());
//     EXPECT_TRUE(uniqueLocal.IsUniqueLocal());
//     EXPECT_FALSE(uniqueLocal.IsUnicastGlobal());

//     AddrV6 global(0x2001,0xdb8,1,0,0,0,0,1);
//     EXPECT_TRUE(global.IsUnicastGlobal());
//     EXPECT_FALSE(global.IsLinkLocal());
//     EXPECT_FALSE(global.IsUniqueLocal());
// }

TEST(AddrV6, BenchmarkingAndDocumentation)
{
    AddrV6 bench(0x2001, 0x0002, 0, 0, 0, 0, 0, 1);
    ASSERT_TRUE(bench.Benchmarking());

    AddrV6 doc(0x2001, 0x0db8, 0, 0, 0, 0, 0, 1);
    ASSERT_TRUE(doc.Documentation());
}

TEST(AddrV6, UInt128Conversion)
{
    AddrV6 a1(0x2001, 0xdb8, 0, 0, 0, 0, 0, 1);
    absl::uint128 value = a1.AsUInt128();

    AddrV6 b1(value);
    EXPECT_EQ(a1.ToString(), b1.ToString());
}

namespace {

struct AddrV6RFC: public testing::Test {
    static void ExpectSuccess(Str input, Array<UInt16, 8> expected) noexcept
    {
        auto result = AddrV6::FromStr(input);
        ASSERT_TRUE(result) << "failed to parse input `" << input << "': " << result.Error();

        auto ipv6 = result.Unwrap();
        auto hextets = ipv6.Hextets();

        Array<UInt16, 8> actual;
        for (size_t i = 0; i < 8; ++i) {
            actual[i] = (hextets[i * 2] << 8) | hextets[(i * 2) + 1];
        }

        for (UInt i = 0; i < 8; ++i) {
            EXPECT_EQ(actual[i], expected[i]) << "Mismatch at index " << i << ": got "
                                              << static_cast<unsigned>(hextets[i]) << ", expected " << expected[i];
        }
    }

    static void ExpectFailure(Str input) noexcept
    {
        auto result = AddrV6::FromStr(input);
        ASSERT_FALSE(result) << "input `" << input << "' succeeded";
    }
};

} // namespace

TEST_F(AddrV6RFC, FullEightHextets)
{
    ExpectSuccess(
        "2001:0db8:0000:0000:0000:ff00:0042:8329", { 0x2001, 0x0db8, 0x0000, 0x0000, 0x0000, 0xff00, 0x0042, 0x8329 });
}

TEST_F(AddrV6RFC, ZeroCompressionMiddle)
{
    ExpectSuccess("2001:db8::ff00:42:8329", { 0x2001, 0x0db8, 0x0000, 0x0000, 0x0000, 0xff00, 0x0042, 0x8329 });
}

TEST_F(AddrV6RFC, Loopback)
{
    ExpectSuccess("::1", { 0, 0, 0, 0, 0, 0, 0, 1 });
}

TEST_F(AddrV6RFC, Unspecified)
{
    ExpectSuccess("::", { 0, 0, 0, 0, 0, 0, 0, 0 });
}

TEST_F(AddrV6RFC, LeadingCompression)
{
    ExpectSuccess("::ffff:192.168.0.1", { 0, 0, 0, 0, 0, 0xffff, 0xc0a8, 0x0001 });
}

TEST_F(AddrV6RFC, IPv4Mapped)
{
    ExpectSuccess("0:0:0:0:0:ffff:192.168.0.1", { 0, 0, 0, 0, 0, 0xffff, 0xc0a8, 0x0001 });
}

TEST_F(AddrV6RFC, UpperLowerHex)
{
    ExpectSuccess("2001:DB8:0:0:8:800:200C:417A", { 0x2001, 0x0db8, 0, 0, 0x0008, 0x0800, 0x200c, 0x417a });
}

TEST_F(AddrV6RFC, MultipleDoubleColon)
{
    ExpectFailure("2001::db8::1");
}

TEST_F(AddrV6RFC, TooManyParts)
{
    ExpectFailure("1:2:3:4:5:6:7:8:9");
}

TEST_F(AddrV6RFC, TooFewPartsWithoutCompression)
{
    ExpectFailure("1:2:3:4:5:6:7");
}

TEST_F(AddrV6RFC, HextetTooLong)
{
    ExpectFailure("12345::1");
}

TEST_F(AddrV6RFC, InvalidHex)
{
    ExpectFailure("2001:db8::zzzz");
}

TEST_F(AddrV6RFC, IPv4OutOfRange)
{
    ExpectFailure("::ffff:256.1.1.1");
}

TEST_F(AddrV6RFC, IPv4TooFewOctets)
{
    ExpectFailure("::ffff:192.168.1");
}

TEST_F(AddrV6RFC, IPv4TooManyOctets)
{
    ExpectFailure("::ffff:1.2.3.4.5");
}

TEST_F(AddrV6RFC, IPv4NotLast)
{
    ExpectFailure("::ffff:192.168.0.1:1234");
}

TEST_F(AddrV6RFC, LoneColonPrefix)
{
    ExpectFailure(":1:2:3:4:5:6:7");
}

TEST_F(AddrV6RFC, LoneColonSuffix)
{
    ExpectFailure("1:2:3:4:5:6:7:");
}

TEST_F(AddrV6RFC, EmptyString)
{
    ExpectFailure("");
}

TEST_F(AddrV6RFC, OnlySingleColon)
{
    ExpectFailure(":");
}
