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

#include <violet/Networking/URL.h>

#include <print>

#define eprintln(...) ::std::println(std::cerr, ##__VA_ARGS__)

// NOLINTBEGIN(google-build-using-namespace)
using namespace violet;
using namespace violet::net;
// NOLINTEND(google-build-using-namespace)

namespace {

void printUrl(violet::net::Url& url) noexcept
{
    std::println("=+= URL: {} =+=", url.ToString());
    std::println("|> Scheme:         {}", url.Scheme());
    std::println("|> Special Scheme: {}", url.Special() ? "yes" : "no");
    std::println("|> Has Authority:  {}", url.HasAuthority() ? "yes" : "no");
    std::println("|> Authority:      {}", url.Authority());
    std::println("|> Username:       {}", url.Username());
    std::println("|> Password:       {}", url.Password());
    std::println("|> Has Host:       {}", url.HasHost() ? "yes" : "no");
    std::println("|> Domain:         {}", url.Domain());
    std::println("|> Path:           {}", url.Path());
    std::println("|> Query:          {}", url.Query());
    std::println("|> Fragment:       {}", url.Fragment());
    std::println("|> Port:           {}", url.Port());
    std::println("|> Port or Known:  {}", url.PortOrKnownDefault());
}

} // namespace

auto main(int argc, char** argv) -> int
{
    CStr url = "https://noelware.org";
    if (argc > 1) {
        url = argv[1];
    }

    auto uri = Url::Parse(url);
    if (uri.Err()) {
        eprintln("failed to parse url `{}`: {}", url, uri.Error());
        return 1;
    }

    printUrl(uri.Value());

    Array<const char*, 2> params = { "hello=world", "fluff=true" };
    auto uriWithParams = Url::ParseWithParams(url, params);
    if (uriWithParams.Err()) {
        eprintln("failed to parse url `{}`: {}", url, uriWithParams.Error());
        return 1;
    }

    printUrl(uriWithParams.Value());
    return 0;
}
