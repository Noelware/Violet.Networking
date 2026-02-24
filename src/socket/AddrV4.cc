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

#include <violet/Networking/Socket/AddrV4.h>
#include <violet/Strings.h>

#include <charconv>
#include <limits>

using violet::Err;
using violet::net::socket::AddrV4;
using violet::net::socket::ParseV4Error;

auto AddrV4::FromStr(Str input) noexcept -> Result<AddrV4, ParseV4Error>
{
    auto [addr, port] = strings::SplitOnce(input, ':');
    if (!port.HasValue()) {
        auto address = ip::AddrV4::FromStr(addr);
        if (address.Err()) {
            return Err(ParseV4Error::invalidAddress(VIOLET_MOVE(address.Error())));
        }

        return AddrV4{ address.Value(), static_cast<UInt16>(0) };
    }

    auto address = ip::AddrV4::FromStr(addr);
    if (address.Err()) {
        return Err(ParseV4Error::invalidAddress(VIOLET_MOVE(address.Error())));
    }

    UInt64 thePort = 0;
    auto [ptr, ec] = std::from_chars(port->begin(), port->begin() + port->size(), thePort);
    if (ec != std::errc{}) {
        return Err(ParseV4Error::invalidIntegral(ec));
    }

    if (thePort > std::numeric_limits<UInt16>::max()) {
        return Err(ParseV4Error::invalidIntegral(std::errc::invalid_argument));
    }

    return AddrV4{ address.Value(), static_cast<UInt16>(thePort) };
}

auto AddrV4::ToString() const noexcept -> String
{
    return std::format("{}:{}", this->Address, this->Port);
}

auto ParseV4Error::ToString() const noexcept -> String
{
    if (std::holds_alternative<invalid_integral_t>(this->n_value)) {
        return std::get<invalid_integral_t>(this->n_value).ToString();
    }

    if (std::holds_alternative<ip::InvalidV4AddressError>(this->n_value)) {
        return std::get<ip::InvalidV4AddressError>(this->n_value).ToString();
    }

    VIOLET_UNREACHABLE();
}
