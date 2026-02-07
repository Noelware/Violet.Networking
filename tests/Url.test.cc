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
#include <violet/Networking/URL.h>

using namespace violet; // NOLINT(google-build-using-namespace)
using namespace violet::net; // NOLINT(google-build-using-namespace)

TEST(RustyURLs, URLParseCorrectly)
{
    auto url = Url::Parse("https://floofy.dev");
    ASSERT_TRUE(url) << "failed to parse url `https://floofy.dev': " << url.Error();
    ASSERT_EQ(url->ToString(), "https://floofy.dev/");
}

TEST(RustyURLs, URLJoinCorrectly)
{
    auto url = Url::Parse("https://floofy.dev");
    ASSERT_TRUE(url) << "failed to parse url `https://floofy.dev': " << url.Error();

    auto url2 = url->Join("/oss/zenful");
    ASSERT_TRUE(url2) << "failed to join url with `/oss/zenful': " << url2.Error();
    ASSERT_EQ(url2->ToString(), "https://floofy.dev/oss/zenful");
}

TEST(RustyUrls, URLParseWithParams)
{
    CStr params[] = { "hello=world", "weow=fluff" };

    auto url = Url::ParseWithParams("https://api.noelware.org", params);
    ASSERT_TRUE(url) << "failed to parse url `https://api.noelware.org' with `?hello=world&weow=fluff`: "
                     << url.Error();

    ASSERT_EQ(url->ToString(), "https://api.noelware.org/?hello=world&weow=fluff");

    auto url2 = Url::ParseWithParams("https://api.floofy.dev/weow?fluff=true", params);
    ASSERT_TRUE(url2) << "failed to parse url `https://api.floofy.dev/weow?fluff=true' with `&hello=world&weow=fluff`: "
                      << url2.Error();

    ASSERT_EQ(url2->ToString(), "https://api.floofy.dev/weow?fluff=true&hello=world&weow=fluff");
}
