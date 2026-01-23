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

#include <violet/Networking/URL.h>

using violet::net::Url;
using violet::net::UrlError;

auto UrlError::ToString() const noexcept -> String
{
    return ::violet_net_url_strerror(this->n_error);
}

Url::Url(violet_net_url_t* handle) noexcept
    : n_handle(handle)
{
}

Url::~Url() noexcept
{
    if (this->n_handle != nullptr) {
        ::violet_net_url_free(this->n_handle);
        this->n_handle = nullptr;
    }
}

Url::Url(Url&& other) noexcept
    : n_handle(std::exchange(other.n_handle, nullptr))
{
}

auto Url::operator=(Url&&) noexcept -> Url& = default;

auto Url::Parse(Str input) noexcept -> Result<Url, UrlError>
{
    violet_net_url_error_t error = URL_UNKNOWN;
    auto* handle = ::violet_net_url_new(input.data(), &error);
    if (error != URL_OK) {
        return Err(UrlError(error));
    }

    VIOLET_DEBUG_ASSERT(handle != nullptr, "received a nullptr in a successful case");
    return Url(handle);
}

// auto Url::ParseWithParams(Str input, Span<CStr> params) noexcept -> Result<Url, UrlError>
// {
//     violet_net_url_error_t error = URL_UNKNOWN;
//     auto* handle = ::violet_net_url_new_with_params(input.data(), params.data(), params.size(), &error);
//     if (error != URL_OK) {
//         return Err(UrlError(error));
//     }

//     VIOLET_DEBUG_ASSERT(handle != nullptr, "received a nullptr in a successful case");
//     return Url(handle);
// }

auto Url::Join(Str path) const noexcept -> Result<Url, UrlError>
{
    violet_net_url_error_t error = URL_UNKNOWN;
    auto* handle = ::violet_net_url_join(this->n_handle, path.data(), &error);
    if (error != URL_OK) {
        return Err(UrlError(error));
    }

    VIOLET_DEBUG_ASSERT(handle != nullptr, "received a nullptr in a successful case");
    return Url(handle);
}

auto Url::ToString() const noexcept -> Str
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");

    auto str = ::violet_net_url_as_str(this->n_handle);
    return { str.data, str.len };
}

auto Url::Scheme() const noexcept -> Str
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");

    auto str = ::violet_net_url_scheme(this->n_handle);
    return { str.data, str.len };
}

auto Url::Special() const noexcept -> bool
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");
    return ::violet_net_url_is_special(this->n_handle);
}

auto Url::HasAuthority() const noexcept -> bool
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");
    return ::violet_net_url_has_authority(this->n_handle);
}

auto Url::Username() const noexcept -> Optional<Str>
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");
    auto view = ::violet_net_url_username(this->n_handle);
    if (!view.something) {
        return Nothing;
    }

    return Some<Str>(view.data.data, view.data.len);
}

auto Url::Password() const noexcept -> Optional<Str>
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");
    auto view = ::violet_net_url_password(this->n_handle);
    if (!view.something) {
        return Nothing;
    }

    return Some<Str>(view.data.data, view.data.len);
}

auto Url::Port() const noexcept -> Optional<UInt16>
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");
    auto view = ::violet_net_url_port(this->n_handle);
    if (!view.something) {
        return Nothing;
    }

    return Some<UInt16>(view.data);
}

auto Url::PortOrKnownDefault() const noexcept -> Optional<UInt16>
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");
    auto view = ::violet_net_url_port_or_known_default(this->n_handle);
    if (!view.something) {
        return Nothing;
    }

    return Some<UInt16>(view.data);
}

auto Url::HasHost() const noexcept -> bool
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");
    return ::violet_net_url_has_host(this->n_handle);
}

auto Url::Host() const noexcept -> Optional<Str>
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");
    auto view = ::violet_net_url_host(this->n_handle);
    if (!view.something) {
        return Nothing;
    }

    return Some<Str>(view.data.data, view.data.len);
}

auto Url::Path() const noexcept -> Str
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");
    auto view = ::violet_net_url_path(this->n_handle);

    return { view.data, view.len };
}

auto Url::Query() const noexcept -> Optional<Str>
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");
    auto view = ::violet_net_url_query(this->n_handle);
    if (!view.something) {
        return Nothing;
    }

    return Some<Str>(view.data.data, view.data.len);
}

auto Url::Fragment() const noexcept -> Optional<Str>
{
    VIOLET_DEBUG_ASSERT(this->n_handle != nullptr, "url handle is not valid");
    auto view = ::violet_net_url_fragment(this->n_handle);
    if (!view.something) {
        return Nothing;
    }

    return Some<Str>(view.data.data, view.data.len);
}
