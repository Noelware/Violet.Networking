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

#pragma once

#include <violet/Container/Optional.h>
#include <violet/Container/Result.h>
#include <violet/Violet.h>

#include "absl/numeric/int128.h"

#include <functional>

namespace violet::net::ip {

struct InvalidV6AddressError;

struct VIOLET_API AddrV6 final {
    constexpr VIOLET_IMPLICIT AddrV6() noexcept
        : AddrV6(0, 0, 0, 0, 0, 0, 0, 0)
    {
    }

    constexpr VIOLET_IMPLICIT AddrV6(Array<UInt8, 16> bytes)
        : n_bytes(bytes)
    {
    }

    constexpr VIOLET_IMPLICIT AddrV6(absl::uint128 value) noexcept
    {
        for (Int32 i = 0; i < 16; ++i) {
            this->n_bytes[i] = static_cast<UInt8>((value >> ((15 - i) * 8)) & 0xFF);
        }
    }

    constexpr VIOLET_IMPLICIT AddrV6(UInt16 first, UInt16 second, UInt16 third, UInt16 fourth, UInt16 fifth,
        UInt16 sixth, UInt16 seventh, UInt16 eighth) noexcept
        : n_bytes({
              static_cast<UInt8>((first >> 8) & 0xFF),
              static_cast<UInt8>(first & 0xFF),
              static_cast<UInt8>((second >> 8) & 0xFF),
              static_cast<UInt8>(second & 0xFF),
              static_cast<UInt8>((third >> 8) & 0xFF),
              static_cast<UInt8>(third & 0xFF),
              static_cast<UInt8>((fourth >> 8) & 0xFF),
              static_cast<UInt8>(fourth & 0xFF),
              static_cast<UInt8>((fifth >> 8) & 0xFF),
              static_cast<UInt8>(fifth & 0xFF),
              static_cast<UInt8>((sixth >> 8) & 0xFF),
              static_cast<UInt8>(sixth & 0xFF),
              static_cast<UInt8>((seventh >> 8) & 0xFF),
              static_cast<UInt8>(seventh & 0xFF),
              static_cast<UInt8>((eighth >> 8) & 0xFF),
              static_cast<UInt8>(eighth & 0xFF),
          })
    {
    }

    constexpr static auto Localhost() noexcept -> AddrV6
    {
        return { 0, 0, 0, 0, 0, 0, 0, 1 };
    }

    static auto FromStr(Str input) noexcept -> Result<AddrV6, InvalidV6AddressError>;

    [[nodiscard]] constexpr auto Loopback() const noexcept -> bool
    {
        for (Int32 i = 0; i < 15; ++i) {
            if (this->n_bytes[i] != 0) {
                return false;
            }
        }

        return this->n_bytes[15] == 1;
    }

    [[nodiscard]] constexpr auto Unspecified() const noexcept -> bool
    {
        for (auto byte: this->n_bytes) {
            if (byte != 0) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] constexpr auto Multicast() const noexcept -> bool
    {
        return this->n_bytes[0] == 0xFF;
    }

    [[nodiscard]] constexpr auto Unicast() const noexcept -> bool
    {
        return !Multicast() && !Unspecified();
    }

    [[nodiscard]] constexpr auto UnicastGlobal() const noexcept -> bool
    {
        return Unicast() && !LinkLocal() && !UniqueLocal();
    }

    [[nodiscard]] constexpr auto LinkLocal() const noexcept -> bool
    {
        return (this->n_bytes[0] == 0xFE) && ((this->n_bytes[1] & 0xC0) == 0x80);
    }

    [[nodiscard]] constexpr auto UniqueLocal() const noexcept -> bool
    {
        return (this->n_bytes[0] & 0xFE) == 0xFC;
    }

    [[nodiscard]] constexpr auto IPv4Mapped() const noexcept -> bool
    {
        for (Int32 i = 0; i < 10; ++i) {
            if (this->n_bytes[i] != 0) {
                return false;
            }
        }

        return this->n_bytes[10] == 0xFF && this->n_bytes[11] == 0xFF;
    }

    [[nodiscard]] constexpr auto Documentation() const noexcept -> bool
    {
        return this->n_bytes[0] == 0x20 && this->n_bytes[1] == 0x01 && this->n_bytes[2] == 0x0d
            && this->n_bytes[3] == 0xb8;
    }

    [[nodiscard]] constexpr auto Benchmarking() const noexcept -> bool
    {
        return this->n_bytes[0] == 0x20 && this->n_bytes[1] == 0x01 && this->n_bytes[2] == 0x00
            && this->n_bytes[3] == 0x02;
    }

    [[nodiscard]] constexpr auto AsUInt128() const noexcept -> absl::uint128
    {
        absl::uint128 result = 0;
        auto octets = this->Hextets();

        for (Int32 i = 0; i < 16; ++i) {
            result |= absl::uint128(octets[i]) << ((15 - i) * 8);
        }

        return result;
    }

    [[nodiscard]] constexpr auto Hextets() const noexcept -> Array<UInt8, 16>
    {
        return this->n_bytes;
    }

    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const AddrV6& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

    constexpr VIOLET_EXPLICIT operator Array<UInt8, 16>() const noexcept
    {
        return this->Hextets();
    }

    constexpr VIOLET_EXPLICIT operator absl::uint128() const noexcept
    {
        return this->AsUInt128();
    }

    constexpr friend auto operator==(const AddrV6& lhs, const AddrV6& rhs) noexcept -> bool
    {
        return lhs.AsUInt128() == rhs.AsUInt128();
    }

    constexpr friend auto operator!=(const AddrV6& lhs, const AddrV6& rhs) noexcept -> bool
    {
        return !(lhs == rhs);
    }

    constexpr friend auto operator<=>(const AddrV6& lhs, const AddrV6& rhs) noexcept -> std::strong_ordering
    {
        return lhs.AsUInt128() <=> rhs.AsUInt128();
    }

    constexpr friend auto operator&(const AddrV6& lhs, const AddrV6& rhs) noexcept -> AddrV6
    {
        return lhs.AsUInt128() & rhs.AsUInt128();
    }

    constexpr friend auto operator|(const AddrV6& lhs, const AddrV6& rhs) noexcept -> AddrV6
    {
        return lhs.AsUInt128() | rhs.AsUInt128();
    }

    constexpr friend auto operator^(const AddrV6& lhs, const AddrV6& rhs) noexcept -> AddrV6
    {
        return lhs.AsUInt128() ^ rhs.AsUInt128();
    }

    constexpr friend auto operator~(const AddrV6& ip) noexcept -> AddrV6
    {
        return ~ip.AsUInt128();
    }

    constexpr friend auto operator&=(AddrV6& lhs, const AddrV6& rhs) noexcept -> AddrV6&
    {
        lhs = lhs.AsUInt128() & rhs.AsUInt128();
        return lhs;
    }

    constexpr friend auto operator|=(AddrV6& lhs, const AddrV6& rhs) noexcept -> AddrV6&
    {
        lhs = lhs.AsUInt128() | rhs.AsUInt128();
        return lhs;
    }

    constexpr friend auto operator^=(AddrV6& lhs, const AddrV6& rhs) noexcept -> AddrV6&
    {
        lhs = lhs.AsUInt128() ^ rhs.AsUInt128();
        return lhs;
    }

private:
    Array<UInt8, 16> n_bytes{};
};

/// Represents an error returned when parsing an invalid IPv4 address.
struct VIOLET_API InvalidV6AddressError final {
    /// Returns a string description of the error.
    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const InvalidV6AddressError& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

private:
    friend auto AddrV6::FromStr(Str input) noexcept -> Result<AddrV6, InvalidV6AddressError>;
    constexpr VIOLET_IMPLICIT InvalidV6AddressError() noexcept = default;

    enum struct kind_t : violet::UInt8 {
        kInvalidNumberOfParts = 0,
        kFailedIntegralParsing = 1,
        kPartTooLarge = 2,
        kMultipleColon = 3,

        kMax = std::numeric_limits<std::underlying_type_t<kind_t>>::max()
    };

    static auto invalidNumberOfParts() noexcept -> InvalidV6AddressError
    {
        InvalidV6AddressError error;
        error.n_kind = kind_t::kInvalidNumberOfParts;

        return error;
    }

    static auto invalidIntegral(std::errc code) noexcept -> InvalidV6AddressError
    {
        InvalidV6AddressError error;
        error.n_kind = kind_t::kFailedIntegralParsing;
        error.n_errorCode = Some<std::error_code>(static_cast<Int32>(code), std::generic_category());

        return error;
    }

    static auto partTooLarge() noexcept -> InvalidV6AddressError
    {
        InvalidV6AddressError error;
        error.n_kind = kind_t::kPartTooLarge;

        return error;
    }

    static auto multipleDoubleColon() noexcept -> InvalidV6AddressError
    {
        InvalidV6AddressError error;
        error.n_kind = kind_t::kMultipleColon;

        return error;
    }

    Optional<std::error_code> n_errorCode;
    kind_t n_kind = kind_t::kMax;
};

} // namespace violet::net::ip

// VIOLET_FORMATTER(violet::net::ip::InvalidV6AddressError);
VIOLET_FORMATTER(violet::net::ip::AddrV6);

template<>
struct std::hash<violet::net::ip::AddrV6> final {
    auto operator()(const violet::net::ip::AddrV6& addr) const noexcept -> violet::UInt
    {
        auto value = addr.AsUInt128();
        return std::hash<violet::UInt64>{}(absl::Uint128High64(value))
            ^ (std::hash<violet::UInt64>{}(absl::Uint128Low64(value)) << 1);
    }
};
