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
#include <violet/Experimental/OneOf.h>
#include <violet/Networking/Socket/AddrV4.h>
#include <violet/Networking/Socket/AddrV6.h>

namespace violet::net {

struct ParseSocketAddressError final {
    constexpr VIOLET_IMPLICIT ParseSocketAddressError() noexcept = default;

    [[nodiscard]] auto ToString() const noexcept -> CStr
    {
        return "invalid socket address";
    }

    friend auto operator<<(std::ostream& os, const ParseSocketAddressError& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }
};

struct SocketAddress final {
    enum struct Type : violet::UInt8 {
        V4,
        V6
    };

    static auto V4(socket::AddrV4 address) noexcept -> SocketAddress
    {
        SocketAddress addr;
        addr.n_value = address;

        return addr;
    }

    static auto V6(socket::AddrV6 address) noexcept -> SocketAddress
    {
        SocketAddress addr;
        addr.n_value = address;

        return addr;
    }

    static auto FromStr(Str input) noexcept -> Result<SocketAddress, ParseSocketAddressError>;

    [[nodiscard]] constexpr auto TypeOf() const noexcept -> Type
    {
        return this->n_value.Holds<socket::AddrV4>() ? Type::V4 : Type::V6;
    }

    [[nodiscard]] constexpr auto AsV4() const noexcept -> Optional<std::reference_wrapper<const socket::AddrV4>>
    {
        return this->n_value.Get<socket::AddrV4>();
    }

    [[nodiscard]] constexpr auto AsV6() const noexcept -> Optional<std::reference_wrapper<const socket::AddrV6>>
    {
        return this->n_value.Get<socket::AddrV6>();
    }

    [[nodiscard]] constexpr auto AsV4Unchecked(Unsafe) const noexcept -> socket::AddrV4
    {
        return this->AsV4().UnwrapUnchecked(Unsafe("falls under the callee"));
    }

    [[nodiscard]] constexpr auto AsV6Unchecked(Unsafe) const noexcept -> socket::AddrV6
    {
        return this->AsV6().UnwrapUnchecked(Unsafe("falls under the callee"));
    }

    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const SocketAddress& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

    constexpr VIOLET_EXPLICIT operator socket::AddrV4() const noexcept
    {
        VIOLET_DEBUG_ASSERT(this->TypeOf() == Type::V4, "current holder is a IPv6 address");
        return this->AsV4Unchecked(Unsafe("either checked in debug mode or doesn't care in release builds"));
    }

    constexpr VIOLET_EXPLICIT operator socket::AddrV6() const noexcept
    {
        VIOLET_DEBUG_ASSERT(this->TypeOf() == Type::V6, "current holder is a IPv4 address");
        return this->AsV6Unchecked(Unsafe("either checked in debug mode or doesn't care in release builds"));
    }

    constexpr friend auto operator==(const SocketAddress& self, const SocketAddress& other) noexcept -> bool
    {
        return self.n_value == other.n_value;
    }

    constexpr friend auto operator!=(const SocketAddress& self, const SocketAddress& other) noexcept -> bool
    {
        return !(self == other);
    }

    constexpr friend auto operator<=>(const SocketAddress& self, const SocketAddress& other) -> std::strong_ordering
    {
        if (auto cmp = self.n_value.Index() <=> other.n_value.Index(); cmp != 0) {
            return cmp;
        }

        return self.n_value.Visit([&other](auto& value) -> std::strong_ordering {
            using type = std::remove_cvref_t<decltype(value)>;
            return value <=> other.n_value.template Get<type>().Unwrap();
        });
    }

private:
    VIOLET_IMPLICIT SocketAddress() noexcept = default;

    using variant_type = violet::experimental::OneOf<socket::AddrV4, socket::AddrV6>;

    variant_type n_value;
};

} // namespace violet::net

VIOLET_FORMATTER(violet::net::SocketAddress);
VIOLET_TO_STRING(violet::net::SocketAddress::Type, typ, {
    using T = violet::net::SocketAddress::Type;
    switch (typ) {
    case T::V4:
        return "V4";

    case T::V6:
        return "V6";
    }
});
