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

#include <functional>

namespace violet::net::ip {

struct InvalidV4AddressError;

/// Representation of Internet Protocol Version 4 addresses specified in [IETF
/// RFC791](https://tools.ietf.org/html/rfc791) in network-byte order.
struct VIOLET_API AddrV4 final {
    /// Constructs a unspecified IPv4 address (`0.0.0.0`).
    constexpr VIOLET_IMPLICIT AddrV4() noexcept = default;

    /// Constructs an IPv4 address from 4 octets.
    /// @param first first octet (most significant byte).
    /// @param second second octet.
    /// @param third third octet.
    /// @param fourth fourth octet (least significant byte).
    constexpr VIOLET_IMPLICIT AddrV4(UInt8 first, UInt8 second, UInt8 third, UInt8 fourth) noexcept
        : n_bytes({ first, second, third, fourth })
    {
    }

    /// Constructs the loopback address (`127.0.0.1`).
    constexpr static auto Localhost() noexcept -> AddrV4
    {
        return { 127, 0, 0, 1 };
    }

    /// Constructs the broadcast address (`255.255.255.255`).
    constexpr static auto Broadcast() noexcept -> AddrV4
    {
        return { 255, 255, 255, 255 };
    }

    /// Constructs an IPv4 address from a network-byte ordered 32-bit integer.
    constexpr static auto FromUInt32(UInt32 addr) noexcept -> AddrV4
    {
        return { static_cast<UInt8>((addr >> 24) & 0xFF), static_cast<UInt8>((addr >> 16) & 0xFF),
            static_cast<UInt8>((addr >> 8) & 0xFF), static_cast<UInt8>(addr & 0xFF) };
    }

    /// Constructs a IPv4 address from its string representation like `"192.168.0.1"`.
    /// @param input the input to parse
    static auto FromStr(Str input) noexcept -> Result<AddrV4, InvalidV4AddressError>;

    /// Returns **true** if this is a broadcast address (`255.255.255.255`).
    ///
    /// A broadcast address has all octets set to `255` as defined in [IETF RFC919](https://tools.ietf.org/html/rfc919).
    ///
    /// ## Example
    /// ```cpp
    /// #include <violet/Networking/IP/AddrV4.h>
    ///
    /// using IPAddrV4 = violet::net::ip::AddrV4;
    ///
    /// static_assert(IPAddrV4(255, 255, 255, 255).IsBroadcast());
    /// static_assert(!IPAddrV4(236, 168, 10, 65).IsBroadcast());
    /// ```
    [[nodiscard]] constexpr auto IsBroadcast() const noexcept -> bool
    {
        return this->AsUInt32() == 0xFFFFFFFF;
    }

    /// Returns **true** if this IP address is the special 'unspecified' address (`0.0.0.0`).
    ///
    /// ## Example
    /// ```cpp
    /// #include <violet/Networking/IP/AddrV4.h>
    ///
    /// using IPAddrV4 = violet::net::ip::AddrV4;
    ///
    /// static_assert(IPAddrV4{}.Unspecified());
    /// static_assert(!IPAddrV4(45, 22, 13, 197).Unspecified());
    /// ```
    [[nodiscard]] constexpr auto Unspecified() const noexcept -> bool
    {
        return this->AsUInt32() == 0x00000000;
    }

    [[nodiscard]] constexpr auto Loopback() const noexcept -> bool
    {
        return (this->AsUInt32() >> 24) == 127;
    }

    [[nodiscard]] constexpr auto Private() const noexcept -> bool
    {
        auto ip = this->AsUInt32();

        // 10.0.0.0/8
        if ((ip >> 24) == 10) {
            return true;
        }

        // 172.16.0.0/12
        if ((ip >> 20) == 0xAC1) {
            return true;
        }

        // 192.168.0.0/16
        if ((ip >> 16) == 0xC0A8) {
            return true;
        }

        return false;
    }

    [[nodiscard]] constexpr auto LinkLocal() const noexcept -> bool
    {
        return (this->AsUInt32() >> 16) == 0xA9FE;
    }

    [[nodiscard]] constexpr auto Global() const noexcept -> bool
    {
        return !(Unspecified() && Loopback() && Private() && LinkLocal() && Multicast() && IsBroadcast() && Shared()
            && Benchmarking() && Reserved());
    }

    [[nodiscard]] constexpr auto Shared() const noexcept -> bool
    {
        auto ip = this->AsUInt32();
        return (ip >> 24) == 100 && ((ip >> 16) & 0xF) >= 64 && ((ip >> 16) & 0xF) <= 127; // 100.64.0.0/10
    }

    [[nodiscard]] constexpr auto Benchmarking() const noexcept -> bool
    {
        auto ip = this->AsUInt32();
        return (ip >> 24) == 198 && ((ip >> 16) & 0xFF) == 18; // 198.18.0.0/15
    }

    [[nodiscard]] constexpr auto Reserved() const noexcept -> bool
    {
        auto ip = this->AsUInt32();
        return (ip >> 24) >= 240 && (ip >> 24) <= 255;
    }

    [[nodiscard]] constexpr auto Multicast() const noexcept -> bool
    {
        auto ip = this->AsUInt32();
        return (ip >> 28) == 0xE; // 224.0.0.0/4
    }

    [[nodiscard]] constexpr auto Documentation() const noexcept -> bool
    {
        auto ip = this->AsUInt32();
        return (ip >> 24) == 192 && ((ip >> 16) & 0xFF) == 0x0D; // 192.0.2.0/24
    }

    [[nodiscard]] constexpr auto AsUInt32() const noexcept -> UInt32
    {
        auto octets = this->Octets();
        return (static_cast<UInt32>(octets[0]) << 24) | (static_cast<UInt32>(octets[1]) << 16)
            | (static_cast<UInt32>(octets[2]) << 8) | (static_cast<UInt32>(octets[3]));
    }

    [[nodiscard]] constexpr auto Octets() const noexcept -> Array<UInt8, 4>
    {
        return this->n_bytes;
    }

    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const AddrV4& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

    constexpr VIOLET_EXPLICIT operator Array<UInt8, 4>() const noexcept
    {
        return this->Octets();
    }

    constexpr VIOLET_EXPLICIT operator UInt32() const noexcept
    {
        return this->AsUInt32();
    }

    constexpr friend auto operator==(const AddrV4& lhs, const AddrV4& rhs) noexcept -> bool
    {
        return lhs.AsUInt32() == rhs.AsUInt32();
    }

    constexpr friend auto operator!=(const AddrV4& lhs, const AddrV4& rhs) noexcept -> bool
    {
        return !(lhs == rhs);
    }

    constexpr friend auto operator<=>(const AddrV4& lhs, const AddrV4& rhs) noexcept -> std::strong_ordering
    {
        return lhs.AsUInt32() <=> rhs.AsUInt32();
    }

    constexpr friend auto operator==(const AddrV4& lhs, UInt32 rhs) noexcept -> bool
    {
        return lhs.AsUInt32() == rhs;
    }

    constexpr friend auto operator!=(const AddrV4& lhs, UInt32 rhs) noexcept -> bool
    {
        return !(lhs == rhs);
    }

    constexpr friend auto operator==(UInt32 lhs, const AddrV4& rhs) noexcept -> bool
    {
        return lhs == rhs.AsUInt32();
    }

    constexpr friend auto operator!=(UInt32 lhs, const AddrV4& rhs) noexcept -> bool
    {
        return !(lhs == rhs);
    }

    constexpr friend auto operator<=>(const AddrV4& lhs, UInt32 rhs) noexcept -> std::strong_ordering
    {
        return lhs.AsUInt32() <=> rhs;
    }

    constexpr friend auto operator&(const AddrV4& lhs, const AddrV4& rhs) noexcept -> AddrV4
    {
        return AddrV4::FromUInt32(lhs.AsUInt32() & rhs.AsUInt32());
    }

    constexpr friend auto operator|(const AddrV4& lhs, const AddrV4& rhs) noexcept -> AddrV4
    {
        return AddrV4::FromUInt32(lhs.AsUInt32() | rhs.AsUInt32());
    }

    constexpr friend auto operator^(const AddrV4& lhs, const AddrV4& rhs) noexcept -> AddrV4
    {
        return AddrV4::FromUInt32(lhs.AsUInt32() ^ rhs.AsUInt32());
    }

    constexpr friend auto operator~(const AddrV4& ip) noexcept -> AddrV4
    {
        return AddrV4::FromUInt32(~ip.AsUInt32());
    }

    constexpr friend auto operator&=(AddrV4& lhs, const AddrV4& rhs) noexcept -> AddrV4&
    {
        lhs = AddrV4::FromUInt32(lhs.AsUInt32() & rhs.AsUInt32());
        return lhs;
    }

    constexpr friend auto operator|=(AddrV4& lhs, const AddrV4& rhs) noexcept -> AddrV4&
    {
        lhs = AddrV4::FromUInt32(lhs.AsUInt32() | rhs.AsUInt32());
        return lhs;
    }

    constexpr friend auto operator^=(AddrV4& lhs, const AddrV4& rhs) noexcept -> AddrV4&
    {
        lhs = AddrV4::FromUInt32(lhs.AsUInt32() ^ rhs.AsUInt32());
        return lhs;
    }

private:
    Array<UInt8, 4> n_bytes;
};

static_assert(sizeof(AddrV4) == 4);
static_assert(std::is_trivially_copyable_v<AddrV4>);

/// Represents an error returned when parsing an invalid IPv4 address.
struct VIOLET_API InvalidV4AddressError final {
    /// Returns a string description of the error.
    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const InvalidV4AddressError& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

private:
    friend auto AddrV4::FromStr(Str input) noexcept -> Result<AddrV4, InvalidV4AddressError>;
    constexpr VIOLET_IMPLICIT InvalidV4AddressError() noexcept = default;

    enum struct kind_t : violet::UInt8 {
        kExceededOctetLimit = 0,
        kFailedIntegralParsing = 1,
        kMaxOctetNumber = 2,
        kNotAtleast4Octets = 3,

        kMax = std::numeric_limits<std::underlying_type_t<kind_t>>::max()
    };

    constexpr static auto exceededOctetLimit() noexcept -> InvalidV4AddressError
    {
        InvalidV4AddressError error;
        error.n_kind = kind_t::kExceededOctetLimit;

        return error;
    }

    constexpr static auto failedIntegralParsing(std::errc code) noexcept -> InvalidV4AddressError
    {
        InvalidV4AddressError error;
        error.n_kind = kind_t::kFailedIntegralParsing;
        error.n_errorCode = Some<std::error_code>(static_cast<Int32>(code), std::generic_category());

        return error;
    }

    constexpr static auto maxOctetNumber() noexcept -> InvalidV4AddressError
    {
        InvalidV4AddressError error;
        error.n_kind = kind_t::kMaxOctetNumber;

        return error;
    }

    constexpr static auto notAtleast4Octets() noexcept -> InvalidV4AddressError
    {
        InvalidV4AddressError error;
        error.n_kind = kind_t::kNotAtleast4Octets;

        return error;
    }

    Optional<std::error_code> n_errorCode;
    kind_t n_kind = kind_t::kMax;
};

} // namespace violet::net::ip

VIOLET_FORMATTER(violet::net::ip::InvalidV4AddressError);
VIOLET_FORMATTER(violet::net::ip::AddrV4);

template<>
struct std::hash<violet::net::ip::AddrV4> final {
    auto operator()(const violet::net::ip::AddrV4& addr) const noexcept -> violet::UInt
    {
        return std::hash<violet::UInt32>{}(addr.AsUInt32());
    }
};
