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
#include <violet/Networking/HTTP/Body.h>

// NOLINTBEGIN(google-build-using-namespace)
using namespace violet::net::http;
using namespace violet;
// NOLINTEND(google-build-using-namespace)

TEST(Body, DefaultShouldBeEmpty)
{
    Body body;

    EXPECT_TRUE(body.Empty());
    EXPECT_FALSE(body.ContentType());
    EXPECT_TRUE(body.Data().empty());
}

TEST(Body, TextBodyHasCorrectContentType)
{
    auto body = Body::Text("hello, world!");
    EXPECT_FALSE(body.Empty());

    auto ct = body.ContentType();
    ASSERT_TRUE(ct);
    EXPECT_EQ(*ct, "text/plain");
}

TEST(Body, TextBodyDataMatchesInput)
{
    String text = "hello, world";
    auto length = text.size();
    auto body = Body::Text(text);

    auto data = body.Data();
    ASSERT_EQ(data.size(), length);
    EXPECT_EQ(String(reinterpret_cast<CStr>(data.data()), data.size()), "hello, world");
}

TEST(Body, BytesBodyUsesDefaultContentType)
{
    Vec<UInt8> bytes = { 0xDE, 0xAD, 0xBE, 0xEF };
    auto body = Body::Bytes(std::move(bytes));

    EXPECT_FALSE(body.Empty());

    auto ct = body.ContentType();
    ASSERT_TRUE(ct.HasValue());
    EXPECT_EQ(*ct, "application/octet-stream");
}

TEST(Body, BytesBodyUsesCustomContentType)
{
    Vec<UInt8> bytes = { 0x01, 0x02, 0x03 };
    auto body = Body::Bytes(std::move(bytes), "image/png");

    auto ct = body.ContentType();
    ASSERT_TRUE(ct.HasValue());
    EXPECT_EQ(*ct, "image/png");
}

TEST(Body, BytesBodyDataMatchesInput)
{
    Vec<UInt8> bytes = { 0xDE, 0xAD, 0xBE, 0xEF };
    auto expected = bytes;
    auto body = Body::Bytes(std::move(bytes));

    auto data = body.Data();
    ASSERT_EQ(data.size(), expected.size());

    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(data[i], expected[i]);
    }
}

TEST(Body, EmptyBytesBodyIsNotEmpty)
{
    Vec<UInt8> bytes;
    auto body = Body::Bytes(VIOLET_MOVE(bytes));

    EXPECT_TRUE(body.Empty());
    EXPECT_TRUE(body.Data().empty());
}

namespace {

struct JsonCodec final {
    constexpr static auto ContentType() noexcept -> Str
    {
        return "application/json";
    }

    static auto Encode(String&& str) -> Vec<UInt8>
    {
        auto string = VIOLET_MOVE(str);
        return { reinterpret_cast<const UInt8*>(string.data()),
            reinterpret_cast<const UInt8*>(string.data()) + string.size() };
    }
};

} // namespace

TEST(Body, CodecSetsCorrectContentType)
{
    auto body = Body::From<JsonCodec, String>(R"({"key":"value"})");
    EXPECT_FALSE(body.Empty());

    auto ct = body.ContentType();
    ASSERT_TRUE(ct);
    EXPECT_EQ(*ct, "application/json");
}

TEST(Body, CodecDataMatchesEncoded)
{
    String json = R"({"key":"value"})";
    auto size = json.size();

    auto body = Body::From<JsonCodec, String>(VIOLET_MOVE(json));
    EXPECT_FALSE(body.Empty());

    auto data = body.Data();
    ASSERT_EQ(data.size(), size);
    EXPECT_EQ(String(reinterpret_cast<CStr>(data.data()), data.size()), R"({"key":"value"})");
}
