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

#include <violet/Networking/Socket/AddrV6.h>
#include <violet/Strings.h>

#include <charconv>
#include <limits>

using violet::Err;
using violet::net::socket::AddrV6;
using violet::net::socket::ParseV6Error;

auto AddrV6::FromStr(Str input) noexcept -> Result<AddrV6, ParseV6Error>
{
    auto stripBrackets = [](Str input) -> Result<Pair<Str, UInt>, ParseV6Error> {
        auto closeBracket = input.find(']');
        if (closeBracket == Str::npos) {
            return Err(ParseV6Error::invalidBracketPlacement());
        }

        return std::make_pair(input.substr(1, closeBracket - 1), closeBracket);
    };

    if (input.front() != '[') {
        return Err(ParseV6Error::invalidBracketPlacement());
    }

    auto [ip, closeBracketIdx] = VIOLET_TRY(stripBrackets(input));
    auto address = ip::AddrV6::FromStr(ip);
    if (address.Err()) {
        return Err(ParseV6Error::invalidAddress(VIOLET_MOVE(address.Error())));
    }

    if (closeBracketIdx + 1 >= input.size()) {
        return AddrV6{ address.Value(), 0 };
    }

    if (input[closeBracketIdx + 1] != ':') {
        return Err(ParseV6Error::invalidBracketPlacement());
    }

    auto port = input.substr(closeBracketIdx + 2);
    if (port.empty()) {
        return AddrV6{ address.Value(), 0 };
    }

    UInt64 thePort = 0;
    auto [ptr, ec] = std::from_chars(port.begin(), port.begin() + port.size(), thePort);
    if (ec != std::errc{}) {
        return Err(ParseV6Error::invalidIntegral(ec));
    }

    if (thePort > std::numeric_limits<UInt16>::max()) {
        return Err(ParseV6Error::invalidIntegral(std::errc::invalid_argument));
    }

    return AddrV6{ address.Value(), static_cast<UInt16>(thePort) };
}

auto AddrV6::ToString() const noexcept -> String
{
    return std::format("[{}]:{}", this->Address, this->Port);
}

auto ParseV6Error::ToString() const noexcept -> String
{
    if (std::holds_alternative<invalid_integral_t>(this->n_value)) {
        return std::get<invalid_integral_t>(this->n_value).ToString();
    }

    if (std::holds_alternative<ip::InvalidV6AddressError>(this->n_value)) {
        return std::get<ip::InvalidV6AddressError>(this->n_value).ToString();
    }

    if (std::holds_alternative<invalid_bracket_placement_t>(this->n_value)) {
        return std::get<invalid_bracket_placement_t>(this->n_value).ToString();
    }

    VIOLET_UNREACHABLE();
}
