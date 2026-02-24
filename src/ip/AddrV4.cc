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

#include <violet/Networking/IP/AddrV4.h>

#include <charconv>

using violet::Err;
using violet::Result;
using violet::Str;
using violet::String;
using violet::net::ip::AddrV4;
using violet::net::ip::InvalidV4AddressError;

auto InvalidV4AddressError::ToString() const noexcept -> String
{
    String suffix;
    switch (this->n_kind) {
    case kind_t::kExceededOctetLimit:
        suffix = "exceeded number of octets needed";
        break;

    case kind_t::kFailedIntegralParsing:
        suffix = std::format("failed to parse integral value: {}", this->n_errorCode->message());
        break;

    case kind_t::kMaxOctetNumber:
        suffix = "max octet number (>255)";
        break;

    case kind_t::kNotAtleast4Octets:
        suffix = "4 octets are required to be a valid address";
        break;

    default:
        VIOLET_UNREACHABLE();
    }

    return std::format("invalid IPv4 address: {}", suffix);
}

auto AddrV4::FromStr(Str input) noexcept -> Result<AddrV4, InvalidV4AddressError>
{
    Array<UInt8, 4> octets;
    UInt octetIndex = 0;
    UInt start = 0;

    for (UInt i = 0; i <= input.size(); ++i) {
        if (i == input.size() || input[i] == '.') {
            if (octetIndex >= 4) {
                return Err(InvalidV4AddressError::exceededOctetLimit());
            }

            auto str = input.substr(start, i - start);
            UInt32 value = 0;

            auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
            if (ec != std::errc{}) {
                return Err(InvalidV4AddressError::failedIntegralParsing(ec));
            }

            if (value > 255) {
                return Err(InvalidV4AddressError::maxOctetNumber());
            }

            octets[octetIndex++] = static_cast<UInt8>(value);
            start = i + 1;
        }
    }

    if (octetIndex != 4) {
        return Err(InvalidV4AddressError::notAtleast4Octets());
    }

    return AddrV4(octets[0], octets[1], octets[2], octets[3]);
}

auto AddrV4::ToString() const noexcept -> String
{
    return std::format("{}.{}.{}.{}", this->n_bytes[0], this->n_bytes[1], this->n_bytes[2], this->n_bytes[3]);
}
