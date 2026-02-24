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
#include <violet/Networking/IP/AddrV4.h>
#include <violet/Networking/IP/AddrV6.h>
#include <violet/Violet.h>

#include <variant>

namespace violet::net {

struct ParseIPAddressError final {
    constexpr VIOLET_IMPLICIT ParseIPAddressError() noexcept = default;

    [[nodiscard]] auto ToString() const noexcept -> CStr
    {
        return "invalid ip address";
    }

    friend auto operator<<(std::ostream& os, const ParseIPAddressError& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }
};

struct IPAddress final {
    enum struct Type : violet::UInt8 {
        V4,
        V6
    };

    constexpr static auto V4(ip::AddrV4 address) noexcept -> IPAddress
    {
        IPAddress addr;
        addr.n_value = address;

        return addr;
    }

    constexpr static auto V6(ip::AddrV6 address) noexcept -> IPAddress
    {
        IPAddress addr;
        addr.n_value = address;

        return addr;
    }

    static auto FromStr(Str input) noexcept -> Result<IPAddress, ParseIPAddressError>;

    [[nodiscard]] constexpr auto TypeOf() const noexcept -> Type
    {
        return std::holds_alternative<ip::AddrV4>(this->n_value) ? Type::V4 : Type::V6;
    }

    [[nodiscard]] constexpr auto AsV4() const noexcept -> Optional<ip::AddrV4>
    {
        if (std::holds_alternative<ip::AddrV4>(this->n_value)) {
            return std::get<ip::AddrV4>(this->n_value);
        }

        return Nothing;
    }

    [[nodiscard]] constexpr auto AsV6() const noexcept -> Optional<ip::AddrV6>
    {
        if (std::holds_alternative<ip::AddrV6>(this->n_value)) {
            return std::get<ip::AddrV6>(this->n_value);
        }

        return Nothing;
    }

    [[nodiscard]] constexpr auto AsV4Unchecked(Unsafe) const noexcept -> ip::AddrV4
    {
        return std::get<ip::AddrV4>(this->n_value);
    }

    [[nodiscard]] constexpr auto AsV6Unchecked(Unsafe) const noexcept -> ip::AddrV6
    {
        return std::get<ip::AddrV6>(this->n_value);
    }

    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const IPAddress& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

    constexpr VIOLET_EXPLICIT operator ip::AddrV4() const noexcept
    {
        VIOLET_DEBUG_ASSERT(std::holds_alternative<ip::AddrV4>(this->n_value), "current holder is a IPv6 address");
        return this->AsV4Unchecked(Unsafe("either checked in debug mode or doesn't care in release builds"));
    }

    constexpr VIOLET_EXPLICIT operator ip::AddrV6() const noexcept
    {
        VIOLET_DEBUG_ASSERT(std::holds_alternative<ip::AddrV6>(this->n_value), "current holder is a IPv4 address");
        return this->AsV6Unchecked(Unsafe("either checked in debug mode or doesn't care in release builds"));
    }

    constexpr friend auto operator==(const IPAddress& self, const IPAddress& other) noexcept -> bool
    {
        return self.n_value == other.n_value;
    }

    constexpr friend auto operator!=(const IPAddress& self, const IPAddress& other) noexcept -> bool
    {
        return !(self == other);
    }

    constexpr friend auto operator<=>(const IPAddress& self, const IPAddress& other) noexcept -> std::strong_ordering
    {
        return self.n_value <=> other.n_value;
    }

private:
    constexpr VIOLET_IMPLICIT IPAddress() noexcept = default;

    std::variant<ip::AddrV4, ip::AddrV6> n_value;
};

} // namespace violet::net

VIOLET_FORMATTER(violet::net::IPAddress);
VIOLET_TO_STRING(violet::net::IPAddress::Type, typ, {
    using T = violet::net::IPAddress::Type;
    switch (typ) {
    case T::V4:
        return "V4";

    case T::V6:
        return "V6";
    }
});
