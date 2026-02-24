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

#include <violet/Container/Result.h>
#include <violet/Networking/IP/AddrV4.h>
#include <violet/Violet.h>

#include <variant>

namespace violet::net::socket {

struct ParseV4Error;

struct AddrV4 final {
    ip::AddrV4 Address{};
    UInt16 Port = 0;

    constexpr VIOLET_IMPLICIT AddrV4() noexcept = default;
    constexpr VIOLET_IMPLICIT AddrV4(ip::AddrV4 address, UInt16 port) noexcept
        : Address(address)
        , Port(port)
    {
    }

    constexpr void SetAddress(ip::AddrV4 address) noexcept
    {
        this->Address = address;
    }

    constexpr void SetPort(UInt16 port) noexcept
    {
        this->Port = port;
    }

    static auto FromStr(Str input) noexcept -> Result<AddrV4, ParseV4Error>;

    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const AddrV4& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

    constexpr friend auto operator==(const AddrV4& lhs, const AddrV4& rhs) noexcept -> bool
    {
        return lhs.Address == rhs.Address && lhs.Port == rhs.Port;
    }

    constexpr friend auto operator!=(const AddrV4& lhs, const AddrV4& rhs) noexcept -> bool
    {
        return !(lhs == rhs);
    }

    constexpr friend auto operator<=>(const AddrV4& lhs, const AddrV4& rhs) noexcept -> std::strong_ordering
    {
        if (auto cmp = lhs.Address <=> rhs.Address; cmp != 0) {
            return cmp;
        }

        return lhs.Port <=> rhs.Port;
    }
};

struct ParseV4Error final {
    auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const ParseV4Error& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

private:
    friend auto AddrV4::FromStr(Str) noexcept -> Result<AddrV4, ParseV4Error>;

    constexpr VIOLET_IMPLICIT ParseV4Error() noexcept = default;

    struct invalid_integral_t final {
        std::errc Code;

        [[nodiscard]] auto ToString() const noexcept -> String
        {
            std::error_code code(static_cast<Int32>(this->Code), std::generic_category());
            return code.message();
        }
    };

    constexpr static auto invalidIntegral(std::errc code) noexcept -> ParseV4Error
    {
        ParseV4Error error;
        error.n_value = invalid_integral_t{ .Code = code };

        return error;
    }

    constexpr static auto invalidAddress(ip::InvalidV4AddressError&& error) noexcept -> ParseV4Error
    {
        ParseV4Error err;
        err.n_value = VIOLET_MOVE(error);

        return err;
    }

    std::variant<invalid_integral_t, ip::InvalidV4AddressError> n_value;
};

} // namespace violet::net::socket

VIOLET_FORMATTER(violet::net::socket::AddrV4);
VIOLET_FORMATTER(violet::net::socket::ParseV4Error);
