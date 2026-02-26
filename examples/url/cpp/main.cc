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

// NOLINTBEGIN(google-build-using-namespace)
using namespace violet;
using namespace violet::net;
// NOLINTEND(google-build-using-namespace)

namespace {

#if VIOLET_REQUIRE_STL(202302L)
using std::println;
#define eprintln(...) ::println(std::cerr, ##__VA_ARGS__)
#else
template<typename... Args>
void println(std::format_string<Args...> fmt, Args&&... args)
{
    std::cout << std::format(fmt, VIOLET_FWD(Args, args)...) << '\n';
}

template<typename... Args>
void eprintln(std::format_string<Args...> fmt, Args&&... args)
{
    std::cerr << std::format(fmt, VIOLET_FWD(Args, args)...) << '\n';
}
#endif

void printUrl(violet::net::Url& url) noexcept
{
    println("=+= URL: {} =+=", url.ToString());
    println("|> Scheme:         {}", url.Scheme());
    println("|> Special Scheme: {}", url.Special() ? "yes" : "no");
    println("|> Has Authority:  {}", url.HasAuthority() ? "yes" : "no");
    println("|> Authority:      {}", url.Authority());
    println("|> Username:       {}", url.Username());
    println("|> Password:       {}", url.Password());
    println("|> Has Host:       {}", url.HasHost() ? "yes" : "no");
    println("|> Domain:         {}", url.Domain());
    println("|> Path:           {}", url.Path());
    println("|> Query:          {}", url.Query());
    println("|> Fragment:       {}", url.Fragment());
    println("|> Port:           {}", url.Port());
    println("|> Port or Known:  {}", url.PortOrKnownDefault());
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
