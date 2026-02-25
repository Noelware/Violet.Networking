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

#include <violet/Networking/IP/AddrV6.h>

#include <charconv>
#include <sstream>

using violet::net::ip::AddrV6;
using violet::net::ip::InvalidV6AddressError;

using violet::Err;
using violet::Result;
using violet::Str;
using violet::UInt;
using violet::UInt16;
using violet::net::ip::AddrV6;

auto InvalidV6AddressError::ToString() const noexcept -> String
{
    String suffix;
    switch (this->n_kind) {
    case kind_t::kInvalidNumberOfParts:
        suffix = "invalid number of parts";
        break;

    case kind_t::kFailedIntegralParsing:
        suffix = std::format("failed to parse integral value: {}", this->n_errorCode->message());
        break;

    case kind_t::kPartTooLarge:
        suffix = "part too large (>65535)";
        break;

    case kind_t::kMultipleColon:
        suffix = "multiple `::` was found";
        break;

    default:
        VIOLET_UNREACHABLE();
    }

    return std::format("invalid IPv6 address: {}", suffix);
}

auto AddrV6::FromStr(Str input) noexcept -> Result<AddrV6, InvalidV6AddressError>
{
    if (input.empty()) {
        return Err(InvalidV6AddressError::invalidNumberOfParts());
    }

    Array<UInt16, 8> hextets{};
    Vec<UInt16> headHextets;
    Vec<UInt16> tailHextets;
    bool seenIPv4 = false;

    // Single '::' position
    UInt doubleColonPos = input.find("::");
    if (doubleColonPos != Str::npos && input.find("::", doubleColonPos + 2) != Str::npos) {
        return Err(InvalidV6AddressError::multipleDoubleColon());
    }

    auto parseHextet = [](Str part) -> Result<UInt16, InvalidV6AddressError> {
        if (part.empty() || part.size() > 4) {
            return Err(InvalidV6AddressError::invalidIntegral(std::errc::invalid_argument));
        }

        UInt32 value = 0;
        auto [ptr, ec] = std::from_chars(part.data(), part.data() + part.size(), value, 16);

        if (ec != std::errc{} || ptr != part.data() + part.size()) {
            return Err(InvalidV6AddressError::invalidIntegral(ec));
        }

        if (value > 0xFFFF) {
            return Err(InvalidV6AddressError::partTooLarge());
        }

        return static_cast<UInt16>(value);
    };

    auto parseIPv4 = [&](Str part) -> Result<violet::Pair<UInt16, UInt16>, InvalidV6AddressError> {
        if (seenIPv4) {
            return Err(InvalidV6AddressError::invalidNumberOfParts());
        }

        seenIPv4 = true;

        UInt32 octets[4]{};
        size_t start = 0;
        for (UInt i = 0; i < 4; ++i) {
            if (start >= part.size()) {
                return Err(InvalidV6AddressError::invalidIntegral(std::errc::invalid_argument));
            }

            size_t end = part.find('.', start);
            if (end == Str::npos) {
                end = part.size();
            }

            UInt32 value = 0;
            auto [ptr, ec] = std::from_chars(part.data() + start, part.data() + end, value, 10);
            if (ec != std::errc{} || value > 255 || ptr != part.data() + end) {
                return Err(InvalidV6AddressError::invalidIntegral(ec));
            }

            octets[i] = value;
            start = end + 1;
        }

        if (start - 1 != part.size()) {
            return Err(InvalidV6AddressError::invalidNumberOfParts());
        }

        return std::make_pair(
            static_cast<UInt16>((octets[0] << 8) | octets[1]), static_cast<UInt16>((octets[2] << 8) | octets[3]));
    };

    auto parseRange = [&](Str range, Vec<UInt16>& target) -> Result<void, InvalidV6AddressError> {
        size_t start = 0;
        while (start <= range.size()) {
            size_t end = range.find(':', start);
            if (end == Str::npos) {
                end = range.size();
            }

            Str part = range.substr(start, end - start);
            if (part.empty()) {
                return Err(InvalidV6AddressError::invalidNumberOfParts());
            }

            if (part.find('.') != Str::npos) {
                if (end != range.size()) {
                    return Err(InvalidV6AddressError::invalidNumberOfParts());
                }

                auto pair = VIOLET_TRY(parseIPv4(part));
                target.push_back(pair.first);
                target.push_back(pair.second);
                return {};
            }

            UInt16 value = VIOLET_TRY(parseHextet(part));
            target.push_back(value);
            if (end == range.size()) {
                break;
            }

            start = end + 1;
        }
        return {};
    };

    if (doubleColonPos != Str::npos) {
        Str head = input.substr(0, doubleColonPos);
        Str tail = input.substr(doubleColonPos + 2);

        if (!head.empty()) {
            VIOLET_TRY_VOID(parseRange(head, headHextets));
        }

        if (!tail.empty()) {
            VIOLET_TRY_VOID(parseRange(tail, tailHextets));
        }

        if (headHextets.size() + tailHextets.size() > 8) {
            return Err(InvalidV6AddressError::invalidNumberOfParts());
        }

        // Copy head
        for (size_t i = 0; i < headHextets.size(); ++i) {
            hextets[i] = headHextets[i];
        }

        // Fill zeros
        size_t zeros = 8 - (headHextets.size() + tailHextets.size());
        for (size_t i = 0; i < zeros; ++i) {
            hextets[headHextets.size() + i] = 0;
        }

        // Copy tail at the end
        for (size_t i = 0; i < tailHextets.size(); ++i) {
            hextets[headHextets.size() + zeros + i] = tailHextets[i];
        }
    } else {
        Vec<UInt16> all;
        VIOLET_TRY_VOID(parseRange(input, all));

        if (all.size() != 8) {
            return Err(InvalidV6AddressError::invalidNumberOfParts());
        }

        for (size_t i = 0; i < 8; ++i) {
            hextets[i] = all[i];
        }
    }

    return AddrV6(hextets[0], hextets[1], hextets[2], hextets[3], hextets[4], hextets[5], hextets[6], hextets[7]);
}

auto AddrV6::ToString() const noexcept -> String
{
    // fast path: if a v6 address is ipv4-mapped (`::ffff:a.b.c.d`)
    if (this->IPv4Mapped()) {
        const auto& bytes = this->n_bytes;
        return std::format("::ffff:{}.{}.{}.{}", bytes[12], bytes[13], bytes[14], bytes[15]);
    }

    Array<UInt16, 8> hextets{};
    for (Int32 i = 0; i < 8; ++i) {
        hextets[i] = (static_cast<UInt16>(this->n_bytes[i * 2]) << 8) | this->n_bytes[(i * 2) + 1];
    }

    Int32 bestStart = -1;
    UInt32 bestLength = 0;

    Int32 currStart = -1;
    UInt32 currLength = 0;

    for (Int32 i = 0; i < 8; ++i) {
        if (hextets[i] == 0) {
            if (currStart == -1) {
                currStart = i;
            }

            currLength++;
        } else {
            if (currLength > bestLength) {
                bestStart = currStart;
                bestLength = currLength;
            }

            currStart = -1;
            currLength = 0;
        }
    }

    if (currLength > bestLength) {
        bestStart = currStart;
        bestLength = currLength;
    }

    if (bestLength < 2) {
        bestStart = -1;
    }

    std::ostringstream os;
    for (Int32 i = 0; i < 8; ++i) {
        if (i == bestStart) {
            os << "::";
            i += static_cast<Int32>(bestLength) - 1;

            if (i == 7) {
                break;
            }

            continue;
        }

        if (i != 0 && !(i == bestStart + bestLength)) {
            os << ':';
        }

        os << std::hex << std::nouppercase << hextets[i];
    }

    return os.str();
}
