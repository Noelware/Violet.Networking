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

TEST(UrlDeathTest, DeathOnNullHandle)
{
    Url url;

#define DEATHTEST(method)                                                                                              \
    EXPECT_DEATH(                                                                                                      \
        { (void)url.method(); }, "url handle is not valid")                                                            \
        << "method `violet::net::Url::" << #method << "' didn't fail as expected"

    DEATHTEST(ToString);
    DEATHTEST(Scheme);
    DEATHTEST(Special);
    DEATHTEST(HasAuthority);
    DEATHTEST(Username);
    DEATHTEST(Password);
    DEATHTEST(Port);
    DEATHTEST(PortOrKnownDefault);
    DEATHTEST(HasHost);
    DEATHTEST(Host);
    DEATHTEST(Path);
    DEATHTEST(Query);
    DEATHTEST(Fragment);

#undef DEATHTEST
}
