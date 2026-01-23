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
//
// # 🌺💜 `violet/Networking/URL.h`

#pragma once

#include <violet-c/net/url.h>
#include <violet/Container/Optional.h>
#include <violet/Container/Result.h>
#include <violet/Violet.h>

namespace violet::net {

struct UrlError final {
    constexpr VIOLET_IMPLICIT UrlError() noexcept = default;
    constexpr VIOLET_IMPLICIT UrlError(violet_net_url_error_t error) noexcept
        : n_error(error)
    {
    }

    [[nodiscard]] constexpr auto Get() const noexcept -> violet_net_url_error_t
    {
        return this->n_error;
    }

    [[nodiscard]] auto ToString() const noexcept -> String;

    friend auto operator<<(std::ostream& os, const UrlError& error) noexcept -> std::ostream&
    {
        return os << error.ToString();
    }

    constexpr VIOLET_EXPLICIT operator bool() const noexcept
    {
        return this->n_error != URL_OK && this->n_error != URL_UNKNOWN;
    }

private:
    violet_net_url_error_t n_error = URL_UNKNOWN;
};

struct Url final {
    /// @hidden
    constexpr VIOLET_IMPLICIT Url() noexcept = default;
    VIOLET_DISALLOW_COPY(Url);

    VIOLET_IMPLICIT Url(Url&& other) noexcept;
    auto operator=(Url&& other) noexcept -> Url&;

    ~Url() noexcept;

    static auto Parse(Str input) noexcept -> Result<Url, UrlError>;
    // static auto ParseWithParams(Str input, Span<CStr> params) noexcept -> Result<Url, UrlError>;

    [[nodiscard]] auto Join(Str path) const noexcept -> Result<Url, UrlError>;
    [[nodiscard]] auto ToString() const noexcept -> Str;
    [[nodiscard]] auto Scheme() const noexcept -> Str;
    [[nodiscard]] auto Special() const noexcept -> bool;
    [[nodiscard]] auto HasAuthority() const noexcept -> bool;
    [[nodiscard]] auto Authority() const noexcept -> Str;
    [[nodiscard]] auto Username() const noexcept -> Optional<Str>;
    [[nodiscard]] auto Password() const noexcept -> Optional<Str>;
    [[nodiscard]] auto Port() const noexcept -> Optional<UInt16>;
    [[nodiscard]] auto PortOrKnownDefault() const noexcept -> Optional<UInt16>;
    [[nodiscard]] auto HasHost() const noexcept -> bool;
    [[nodiscard]] auto Host() const noexcept -> Optional<Str>;
    [[nodiscard]] auto Path() const noexcept -> Str;
    [[nodiscard]] auto Query() const noexcept -> Optional<Str>;
    [[nodiscard]] auto Fragment() const noexcept -> Optional<Str>;

private:
    VIOLET_EXPLICIT Url(violet_net_url_t* handle) noexcept;

    violet_net_url_t* n_handle = nullptr;
};

} // namespace violet::net

VIOLET_FORMATTER(violet::net::UrlError);
VIOLET_FORMATTER(violet::net::Url);
