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

#include <violet/Networking/IPAddress.h>

using violet::Err;
using violet::net::IPAddress;
using violet::net::ParseIPAddressError;

auto IPAddress::FromStr(Str input) noexcept -> Result<IPAddress, ParseIPAddressError>
{
    if (auto v4 = ip::AddrV4::FromStr(input); v4.Ok()) {
        return IPAddress::V4(v4.Value());
    }

    if (auto v6 = ip::AddrV6::FromStr(input); v6.Ok()) {
        return IPAddress::V6(v6.Value());
    }

    return Err(ParseIPAddressError{});
}

auto IPAddress::ToString() const noexcept -> String
{
    if (auto v4 = this->AsV4()) {
        return v4.ToString();
    }

    if (auto v6 = this->AsV6()) {
        return v6->ToString();
    }

    VIOLET_UNREACHABLE();
}
