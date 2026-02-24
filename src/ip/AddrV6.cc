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

    default:
        VIOLET_UNREACHABLE();
    }

    return std::format("invalid IPv6 address: {}", suffix);
}

auto AddrV6::FromStr(Str input) noexcept -> Result<AddrV6, InvalidV6AddressError>
{
    Array<UInt16, 8> hextets{};
    UInt doubleColonPos = input.find("::");

    auto parseHextet = [](Str part) -> Result<UInt16, InvalidV6AddressError> {
        UInt32 value = 0;
        auto [ptr, ec] = std::from_chars(part.data(), part.data() + part.size(), value, 16);
        if (ec != std::errc{}) {
            return Err(InvalidV6AddressError::invalidIntegral(ec));
        }

        if (value > 0xFFFF) {
            return Err(InvalidV6AddressError::partTooLarge());
        }

        return static_cast<UInt16>(value);
    };

    Vec<UInt16> headHextets;
    Vec<UInt16> tailHextets;

    if (doubleColonPos != Str::npos) {
        Str head = input.substr(0, doubleColonPos);
        Str tail = input.substr(doubleColonPos + 2);

        // Parse head `::`
        if (!head.empty()) {
            std::stringstream ss(head);
            String part;

            while (std::getline(ss, part, ':')) {
                headHextets.push_back(VIOLET_TRY(parseHextet(part)));
            }
        }

        // Parse tail `::`
        if (!tail.empty()) {
            std::stringstream ss(tail);
            String part;

            while (std::getline(ss, part, ':')) {
                // Check if `part` contains a IPv4-mapped address
                // as the last segment
                if (part.find('.') != Str::npos) {
                    // TODO(@auguwu/Noel): switch to violet::strings::SplitN<4>(part, '.');
                    // once Noelware.Violet 26.03.08 is released
                    UInt32 first = 0, second = 0, third = 0, fourth = 0; // NOLINT(readability-isolate-declaration)
                    if (std::sscanf(part.c_str(), "%u.%u.%u.%u", &first, &second, &third, &fourth) != 4) {
                        return Err(InvalidV6AddressError::invalidIntegral(std::errc::invalid_argument));
                    }

                    if (first > 255 || second > 255 || third > 255 || fourth > 255) {
                        return Err(InvalidV6AddressError::partTooLarge());
                    }

                    tailHextets.push_back(static_cast<UInt16>((first << 8) | second));
                    tailHextets.push_back(static_cast<UInt16>((third << 8) | fourth));
                } else {
                    tailHextets.push_back(VIOLET_TRY(parseHextet(part)));
                }
            }
        }

        if (headHextets.size() + tailHextets.size() > 8) {
            return Err(InvalidV6AddressError::invalidNumberOfParts());
        }

        UInt index = 0;
        for (auto head: headHextets) {
            hextets[index++] = head;
        }

        UInt zerosToFill = 8 - (headHextets.size() + tailHextets.size());
        for (UInt i = 0; i < zerosToFill; ++i) {
            hextets[index++] = 0;
        }

        for (auto tail: tailHextets) {
            hextets[index++] = tail;
        }
    } else {
        std::stringstream ss(input);
        String part;
        UInt index = 0;

        while (std::getline(ss, part, ':')) {
            // Check if `part` contains a IPv4-mapped address
            // as the last segment
            if (part.find('.') != Str::npos) {
                // TODO(@auguwu/Noel): switch to violet::strings::SplitN<4>(part, '.');
                // once Noelware.Violet 26.03.08 is released
                UInt32 first = 0, second = 0, third = 0, fourth = 0; // NOLINT(readability-isolate-declaration)
                if (std::sscanf(part.c_str(), "%u.%u.%u.%u", &first, &second, &third, &fourth) != 4) {
                    return Err(InvalidV6AddressError::invalidIntegral(std::errc::invalid_argument));
                }

                if (first > 255 || second > 255 || third > 255 || fourth > 255) {
                    return Err(InvalidV6AddressError::partTooLarge());
                }

                hextets[index++] = static_cast<UInt16>((first << 8) | second);
                hextets[index++] = static_cast<UInt16>((third << 8) | fourth);
            } else {
                if (index >= 8) {
                    return Err(InvalidV6AddressError::invalidNumberOfParts());
                }

                hextets[index++] = VIOLET_TRY(parseHextet(part));
            }
        }

        if (index != 8) {
            return Err(InvalidV6AddressError::invalidNumberOfParts());
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
