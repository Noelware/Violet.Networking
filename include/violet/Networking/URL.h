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
//! # 🌺💜 `violet/Networking/URL.h`
//! This header file provides a high-level, safe URL parsing and manipulation facility backed
//! by Rust's [`url`] crate and accessibility for C++ programs with an addition to Violet's
//! vast Networking framework.
//!
//! This module is also backed by the C version in the `violet-c/net/url.h` header.
//!
//! This module provides:
//! - Lossless URL parsing compliant with modern URL semantics
//! - Zero-copy accessors via `Str`
//! - Fallible APIs using `Result<T, UrlError>`
//! - Explicit handling of authority, host, port, query, and fragment
//!
//! ## Example
//! ```cpp
//! #include <violet/Networking/URL.h>
//!
//! using violet::net::Url;
//!
//! auto url = Url::Parse("https://user:pass@example.com:443/path?q=1#frag");
//! if (!url) {
//!     std::println(std::cerr, "failed to parse url: {}", url.Error());
//! }
//! ```

#pragma once

#include <violet-c/net/url.h>
#include <violet/Container/Optional.h>
#include <violet/Container/Result.h>
#include <violet/Violet.h>

namespace violet::net {

/// An error produced when parsing a URL fails.
///
/// Wraps the C-level `violet_net_url_error_t` error code and provides
/// conversion to a human-readable message.
///
/// A default-constructed `UrlError` holds `URL_UNKNOWN`. The boolean conversion
/// returns `true` when the error represents an actual parse failure (i.e., is
/// neither `URL_OK` nor `URL_UNKNOWN`).
///
/// ## Example
/// ```cpp
/// #include <violet/Networking/URL.h>
/// #include <violet/Print.h>
///
/// using violet::net::Url;
///
/// if (auto result = Url::Parse("http://[::gg]"); result.Err()) {
///     violet::PrintErrln("failed to parse `http://[::gg]`: {}", result.Error());
/// }
/// ``
struct UrlError final {
    /// Constructs a default `UrlError` with the `URL_UNKNOWN` error code.
    constexpr VIOLET_IMPLICIT UrlError() noexcept = default;

    /// Constructs a `UrlError` from a C-level error code.
    constexpr VIOLET_IMPLICIT UrlError(violet_net_url_error_t error) noexcept
        : n_error(error)
    {
    }

    /// Returns the underlying C error code.
    [[nodiscard]] constexpr auto Get() const noexcept -> violet_net_url_error_t
    {
        return this->n_error;
    }

    /// Returns a human-readable description of the error.
    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const UrlError& error) noexcept -> std::ostream&
    {
        return os << error.ToString();
    }

    /// Returns `true` if this represents an actual parse failure.
    ///
    /// Returns `false` for `URL_OK` and `URL_UNKNOWN`.
    constexpr VIOLET_EXPLICIT operator bool() const noexcept
    {
        return this->n_error != URL_OK && this->n_error != URL_UNKNOWN;
    }

    /// Converts back to the underlying C error code.
    constexpr VIOLET_EXPLICIT operator violet_net_url_error_t() const noexcept
    {
        return this->n_error;
    }

private:
    violet_net_url_error_t n_error = URL_UNKNOWN;
};

/// A parsed URL conforming to the WHATWG URL Standard.
///
/// Wraps a `violet_net_url_t` handle backed by Rust's [`url::Url`]. The URL is
/// parsed and normalized during construction; dot segments are resolved,
/// default ports for special schemes are stripped, and the scheme is
/// ASCII-lowercased.
///
/// [`url::Url`]: https://docs.rs/url/latest/url/struct.Url.html
///
/// `Url` is move-only. The underlying handle is freed on destruction.
///
/// ## Example
/// ```cpp
/// #include <violet/Networking/URL.h>
/// #include <violet/Print.h>
///
/// using violet::net::Url;
///
/// auto url = Url::Parse("https://example.com/a/../b?q=1#frag").Unwrap();
///
/// violet::Println("url.Scheme()   = \"{}\"", url.Scheme());
/// violet::Println("url.Host()     = \"{}\"", url.Host());
/// violet::Println("url.Port()     = \"{}\"", url.Port());
/// violet::Println("url.Path()     = \"{}\"", url.Path());
/// violet::Println("url.Query()    = \"{}\"", url.Query());
/// violet::Println("url.Fragment() = \"{}\"", url.Fragment());
///
/// violet::CStr params[] = {"key", "hello world"};
/// auto url = Url::ParseWithParams("https://example.com/search", params);
/// violet::Println("{}", url.ToString()); // "https://example.com/search?key=hello+world"
/// ```
struct Url final {
    VIOLET_DISALLOW_CONSTRUCTOR(Url);
    VIOLET_DISALLOW_COPY(Url);

    /// Destroys the URL and frees the underlying handle.
    ~Url();

    /// Move-constructs a `Url`, taking ownership of the handle from `other`.
    ///
    /// After the move, `other` is left in a valid but empty state.
    VIOLET_IMPLICIT Url(Url&& other) noexcept;

    /// Move-assigns a `Url`, freeing any existing handle and taking ownership
    /// from `other`.
    auto operator=(Url&& other) noexcept -> Url&;

    /// Parses `input` as an absolute URL.
    ///
    /// Equivalent to `Url::parse` in the `url` crate. The input must be an
    /// absolute URL (i.e., it must have a scheme). Relative references are
    /// rejected; use `Join` to resolve relative URLs against a base.
    ///
    /// [`Url::parse`]: https://docs.rs/url/latest/url/struct.Url.html#method.parse
    ///
    /// @param input a UTF-8 URL string to parse.
    /// @see violet_net_url_new
    static auto Parse(Str input) noexcept -> Result<Url, UrlError>;

    /// Parses `input` as an absolute URL with query parameters appended.
    ///
    /// Equivalent to `Url::parse_with_params` in the `url` crate. Parameters
    /// are encoded using `application/x-www-form-urlencoded` and appended to
    /// the query string. If `input` already contains a query string, the
    /// parameters are appended with `&`.
    ///
    /// [`Url::parse_with_params`]: https://docs.rs/url/latest/url/struct.Url.html#method.parse_with_params
    ///
    /// `params` is a flat span of alternating key-value strings paired with `=`:
    /// `[key0=value0, key1=value1, ...]`. The span length must be even.
    ///
    /// @param input  a UTF-8 URL string to parse.
    /// @param params alternating key-value pairs to append as query parameters.
    /// @see violet_net_url_new_with_params
    static auto ParseWithParams(Str input, Span<CStr> params) noexcept -> Result<Url, UrlError>;

    /// Resolves `path` relative to this URL, producing a new absolute URL.
    ///
    /// Equivalent to `Url::join` in the `url` crate. Follows the WHATWG URL
    /// Standard's relative resolution algorithm. If `path` is already absolute,
    /// this URL is effectively ignored.
    ///
    /// [`Url::join`]: https://docs.rs/url/latest/url/struct.Url.html#method.join
    ///
    /// @param path a UTF-8 string, either absolute or relative.
    /// @see violet_net_url_join
    [[nodiscard]] auto Join(Str path) const noexcept -> Result<Url, UrlError>;

    /// Returns the full serialization of the URL.
    ///
    /// Equivalent to [`Url::as_str`] in the `url` crate. The returned view is
    /// valid for the lifetime of this `Url`. The serialization is guaranteed
    /// to be valid UTF-8 and round-trips through [`Url::Parse`].
    ///
    /// [`Url::as_str`]: https://docs.rs/url/latest/url/struct.Url.html#method.as_str
    ///
    /// @see violet_net_url_as_str
    [[nodiscard]] auto ToString() const noexcept -> Str;

    /// Returns the scheme, without the trailing `:`.
    ///
    /// Equivalent to [`Url::scheme`] in the `url` crate. Always ASCII-lowercase.
    ///
    /// [`Url::scheme`]: https://docs.rs/url/latest/url/struct.Url.html#method.scheme
    ///
    /// @see violet_net_url_scheme
    [[nodiscard]] auto Scheme() const noexcept -> Str;

    /// Returns whether the URL has a special scheme.
    ///
    /// Special schemes per the WHATWG URL Standard: `ftp`, `file`, `http`,
    /// `https`, `ws`, `wss`. These have default ports (except `file`) and
    /// stricter parsing rules, such as backslash normalization.
    ///
    /// @see violet_net_url_is_special
    [[nodiscard]] auto Special() const noexcept -> bool;

    /// Returns whether the URL has an authority component (`//` after scheme).
    ///
    /// Equivalent to [`Url::has_authority`] in the `url` crate. When true, the
    /// URL has a host and its path starts with `/`.
    ///
    /// [`Url::has_authority`]: https://docs.rs/url/latest/url/struct.Url.html#method.has_authority
    ///
    /// @see violet_net_url_has_authority
    [[nodiscard]] auto HasAuthority() const noexcept -> bool;

    /// Returns the authority component: `[user[:pass]@]host[:port]`.
    ///
    /// Equivalent to [`Url::authority`] in the `url` crate. Returns an empty
    /// string for URLs without an authority.
    ///
    /// [`Url::authority`]: https://docs.rs/url/latest/url/struct.Url.html#method.authority
    ///
    /// @see violet_net_url_authority
    [[nodiscard]] auto Authority() const noexcept -> Str;

    /// Returns the username, if present.
    ///
    /// Equivalent to `Url::username` in the `url` crate. Note that the `url`
    /// crate returns `""` (not [`violet::Nothing`]) for URLs without credentials; this
    /// wrapper returns [`violet::Nothing`] in that case for consistency with [`violet::Optional`].
    /// A present-but-empty `Optional` means the URL explicitly has an empty
    /// username (e.g., `http://@example.com`).
    ///
    /// [`Url::username`]: https://docs.rs/url/latest/url/struct.Url.html#method.username
    ///
    /// @see violet_net_url_username
    [[nodiscard]] auto Username() const noexcept -> Optional<Str>;

    /// Returns the password, if present.
    ///
    /// Equivalent to [`Url::password`] in the `url` crate. Returns [`violet::Nothing`] when
    /// the URL has no password, even if it has a username.
    ///
    /// [`Url::password`]: https://docs.rs/url/latest/url/struct.Url.html#method.password
    ///
    /// @see violet_net_url_password
    [[nodiscard]] auto Password() const noexcept -> Optional<Str>;

    /// Returns the port, if explicitly specified in the URL.
    ///
    /// Equivalent to [`Url::port`] in the `url` crate. Default ports for special
    /// schemes are stripped during parsing; `https://example.com:443/` is
    /// normalized to `https://example.com/`, so this returns [`violet::Nothing`]. Use
    /// [`Url::PortOrKnownDefault`] for fallback behavior.
    ///
    /// [`Url::port`]: https://docs.rs/url/latest/url/struct.Url.html#method.port
    ///
    /// @see violet_net_url_port
    [[nodiscard]] auto Port() const noexcept -> Optional<UInt16>;

    /// Returns the port, falling back to the known default for the scheme.
    ///
    /// Equivalent to [`Url::port_or_known_default`] in the `url` crate.
    ///
    /// Known defaults:
    ///     * `ftp`  ~> 21
    ///     * `http` ~> 80
    ///     * `https`~> 443
    ///     * `ws`   ~> 80
    ///     * `wss`  ~> 443.
    ///
    /// Returns [`violet::Nothing`] for non-special schemes without an explicit port and for `file:` URLs.
    ///
    /// [`Url::port_or_known_default`]: https://docs.rs/url/latest/url/struct.Url.html#method.port_or_known_default
    ///
    /// @see violet_net_url_port_or_known_default
    [[nodiscard]] auto PortOrKnownDefault() const noexcept -> Optional<UInt16>;

    /// Returns whether the URL has a host (including an empty host).
    ///
    /// Equivalent to [`Url::has_host`] in the `url` crate. `file:` URLs may have
    /// an empty host (e.g., `file:///path`) which still counts as having a host.
    ///
    /// [`Url::has_host`]: https://docs.rs/url/latest/url/struct.Url.html#method.has_host
    ///
    /// @see violet_net_url_has_host
    [[nodiscard]] auto HasHost() const noexcept -> bool;

    /// Returns the host as a string, if present.
    ///
    /// Equivalent to [`Url::host_str`] in the `url` crate. For IPv6 addresses,
    /// the brackets are *not* included (e.g., `"::1"` not `"[::1]"`). Returns
    /// `None` for URLs without a host.
    ///
    /// [`Url::host_str`]: https://docs.rs/url/latest/url/struct.Url.html#method.host_str
    ///
    /// @see violet_net_url_host
    [[nodiscard]] auto Host() const noexcept -> Optional<Str>;

    /// Returns the domain, if the host is a domain name.
    ///
    /// Equivalent to [`Url::domain`] in the `url` crate. Returns [`violet::Nothing`] if the URL has no host.
    /// An empty host in a `file:` URL is considered a domain.
    ///
    /// [`Url::domain`]: https://docs.rs/url/latest/url/struct.Url.html#method.domain
    ///
    /// @see violet_net_url_domain
    [[nodiscard]] auto Domain() const noexcept -> Optional<Str>;

    /// Returns the path component in percent-encoded form.
    ///
    /// Equivalent to [`Url::path`] in the `url` crate. For URLs with an authority,
    /// the path always starts with `/`. Dot segments (`.` and `..`) are resolved
    /// during parsing. For cannot-be-a-base URLs, the path is the opaque content
    /// after the scheme (e.g., `mailto:user@example.com` ~> `"user@example.com"`).
    ///
    /// [`Url::path`]: https://docs.rs/url/latest/url/struct.Url.html#method.path
    ///
    /// @see violet_net_url_path
    [[nodiscard]] auto Path() const noexcept -> Str;

    /// Returns the query string, if present, without the leading `?`.
    ///
    /// Equivalent to [`Url::query`] in the `url` crate. A present-but-empty
    /// `Optional` (from `http://example.com/?`) is distinct from [`violet::Nothing`] (no
    /// `?` at all). The query is in raw percent-encoded form.
    ///
    /// [`Url::query`]: https://docs.rs/url/latest/url/struct.Url.html#method.query
    ///
    /// @see violet_net_url_query
    [[nodiscard]] auto Query() const noexcept -> Optional<Str>;

    /// Returns the fragment, if present, without the leading `#`.
    ///
    /// Equivalent to [`Url::fragment`] in the `url` crate. A present-but-empty
    /// `Optional` (from `http://example.com/#`) is distinct from `None` (no
    /// `#` at all). Fragments are a client-side concept and are not sent in
    /// HTTP requests.
    ///
    /// [`Url::fragment`]: https://docs.rs/url/latest/url/struct.Url.html#method.fragment
    ///
    /// @see violet_net_url_fragment
    [[nodiscard]] auto Fragment() const noexcept -> Optional<Str>;

private:
    VIOLET_EXPLICIT Url(violet_net_url_t* handle) noexcept;

    violet_net_url_t* n_handle = nullptr;
};

} // namespace violet::net

VIOLET_FORMATTER(violet::net::UrlError);
VIOLET_FORMATTER(violet::net::Url);
