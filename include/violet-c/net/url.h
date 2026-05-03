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
// # 🌺💜 `violet-c/net/url.h`

#pragma once

// NOLINTBEGIN(modernize-*,performance-enum-size)

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// A opaque, allocated value to a Rust [`url::Url`].
///
/// [`url::Url`]: https://docs.rs/url/latest/url/struct.Url.html
typedef struct violet_net_url_t violet_net_url_t;

/// Representation of a possible error that should be handled.
typedef enum violet_net_url_error_t {
    /// Success case. You can safely discard.
    URL_OK = 0,

    /// empty input provided
    URL_EMPTY = 1,

    /// input was not valid unicode
    URL_NOT_UNICODE = 2,

    /// `Url::parse_with_params()': parameter didn't have `=` sign
    URL_INVALID_PARAM_STRING = 3,

    /// `Url::parse_with_params()': parameter with pair had extra `=`
    URL_EXTRA_CHAR_IN_PARAM_STRING = 4,

    /// empty host
    URL_EMPTY_HOST = 5,

    /// invalid international domain name
    URL_IDNA_ERROR = 6,

    /// invalid port number
    URL_INVALID_PORT = 7,

    /// invalid IPv4 address
    URL_INVALID_IPV4_ADDRESS = 8,

    /// invalid IPv6 address
    URL_INVALID_IPV6_ADDRESS = 9,

    /// invalid domain character
    URL_INVALID_DOMAIN_CHARACTER = 10,

    /// relative URL without a base
    URL_RELATIVE_URL_WITHOUT_BASE = 11,

    /// relative URL with a cannot-be-a-base base
    URL_RELATIVE_URL_WITH_CANNOT_BE_A_BASE_BASE = 12,

    /// a cannot-be-a-base URL doesn’t have a host to set
    URL_SET_HOST_ON_CANNOT_BE_A_BASE_URL = 13,

    /// URLs more than 4 GB are not supported
    URL_OVERFLOW = 14,

    /// unknown, possibly-unhandled error; this should never happen
    URL_UNKNOWN = -1
} violet_net_url_error_t;

/// A non-owning view to a C string that was provided by Rust.
typedef struct violet_net_string_view {
    /// Size of the string plus null-terminated character.
    size_t len;

    /// Pointer to the data itself.
    const char* data;
} violet_net_string_view;

/// A wrapper for Rust's <code>[`Option`]\<[`&str`][`str`]\></code> in C.
///
/// [`Option`]: https://doc.rust-lang.org/stable/std/option/enum.Option.html
/// [`str`]: https://doc.rust-lang.org/stable/std/primitive.reference.html
typedef struct violet_net_optional_string_view {
    /// Returns **true** if there is something in `data`.
    bool something;

    /// The data itself.
    violet_net_string_view data;
} violet_net_optional_string_view;

/// A wrapper for Rust's <code>[`Option`]\<[`u16`]\></code> in C.
///
/// [`Option`]: https://doc.rust-lang.org/stable/std/option/enum.Option.html
/// [`u16`]: https://doc.rust-lang.org/stable/std/primitive.u16.html
typedef struct violet_net_optional_uint16 {
    /// Returns **true** if there is something in `data`.
    bool something;

    /// The data itself. If `something` is false, this returns `0xFFFF` (`65535`), which
    /// it **could** be a valid port number but don't rely on it.
    uint16_t data;
} violet_net_optional_uint16;

/// Constructs a new [`violet_net_url_t`] opaque handle.
///
/// ## Example
/// ```c
/// #include <violet-c/net/url.h>
///
/// violet_net_url_error_t error = URL_UNKNOWN;
/// violet_net_url_t* url = violet_net_url_new("https://noelware.org/");
/// if (error != URL_OK) {
///     printf("failed to parse url `https://noelware.org/': %s", violet_net_url_strerror(error));
///     return 1;
/// } else {
///     printf("%s", violet_net_url_as_str(url));
///     violet_net_url_free(url);
/// }
///
/// return 0;
/// ```
///
/// @param input the input to use
/// @param error pointer to a error, which will return the error code (if any).
violet_net_url_t* violet_net_url_new(const char* input, violet_net_url_error_t* error);

/// Creates a new URL by parsing `input` with optional query parameters appended.
///
/// Parses `input` according to the WHATWG URL Standard, backed by Rust's [`url`] crate.
/// If `params` is non-null, the key-value pairs are percent-encoded using
/// `application/x-www-form-urlencoded` encoding and appended to the query string.
///
/// Parameters are provided as a flat array of alternating key-value strings with `=`:
/// `[key0=value0, key1=value1, ...]`.
///
/// [`url`]: https://docs.rs/url
///
/// If `input` already contains a query string, the additional parameters are
/// appended with `&`.
///
/// ## Example
/// ```c
/// #include <violet-c/net/url.h>
///
/// const char* params[] = {"q=hello world", "page=2"};
///
/// violet_net_url_error_t error = URL_UNKNOWN;
/// violet_net_url_t* url = violet_net_url_new_with_params(
///     /*input=*/"https://example.com/search",
///     /*params=*/params,
///     /*len=*/2,
///     /*error=*/&error
/// );
///
/// assert(url != NULL);
/// printf("url: %s", violet_net_url_as_str(url));
///
/// violet_net_url_free(url);
/// ```
///
/// @param input  a null-terminated UTF-8 URL string to parse.
/// @param params an array of null-terminated key-value pairs, or `NULL` if no parameters are needed. Must have an even
///               number of elements.
/// @param len    the number of elements in `params` (not the number of pairs). Must be even. Ignored if `params` is
///               `NULL`.
/// @param error  out-parameter set on failure. May be `NULL` if the caller does not need error details.
violet_net_url_t* violet_net_url_new_with_params(
    const char* input, const char* params[], size_t len, violet_net_url_error_t* error);

/// Resolves `input` relative to `url`, producing a new absolute URL.
///
/// Equivalent to [`Url::join`] in the [`url`] crate. Follows the WHATWG URL Standard's
/// relative URL resolution algorithm. If `input` is already absolute, `url` is
/// effectively ignored and `input` is parsed independently.
///
/// [`url`]: https://docs.rs/url
/// [`Url::join`]: https://docs.rs/url/latest/url/struct.Url.html#method.join
///
/// ## Example
/// ```c
/// #include <violet-c/net/url.h>
///
/// violet_net_url_t* base = violet_net_url_new("https://example.com/a/b/c", NULL);
/// assert(base != NULL);
///
/// // joined is "https://example.com/a/d?x=1"
/// violet_net_url_t* joined = violet_net_url_join(base, "../d?x=1", NULL);
/// assert(joined != NULL);
///
/// // abs is "https://other.com/"
/// violet_net_url_t* abs = violet_net_url_join(base, "https://other.com/", NULL);
///
/// violet_net_url_free(abs);
/// violet_net_url_free(joined);
/// violet_net_url_free(base);
/// ```
///
/// @param url   the base URL to resolve against. Must not be `NULL`.
/// @param input a null-terminated UTF-8 string, either absolute or relative.
/// @param error out-parameter set on failure. May be `NULL`.
violet_net_url_t* violet_net_url_join(const violet_net_url_t* url, const char* input, violet_net_url_error_t* error);

/// Frees a URL previously allocated by `violet_net_url_new`, `violet_net_url_new_with_params`, or
/// `violet_net_url_join`.
///
/// If `url` is `NULL`, this function is a no-op.
///
/// @param url the URL to free, or `NULL`.
void violet_net_url_free(violet_net_url_t* url);

/// Returns the serialization of the URL as a string view.
///
/// Equivalent to [`Url::as_str`] in the `url` crate. The returned view spans the
/// full serialized URL, which is guaranteed to be valid UTF-8 and to round-trip
/// through [`violet_net_url_new_with_params`] (modulo normalization).
///
/// [`Url::as_str`]: https://docs.rs/url/latest/url/struct.Url.html#method.as_str
///
/// The returned view is valid for the lifetime of `url`.
violet_net_string_view violet_net_url_as_str(const violet_net_url_t* url);

/// Returns the scheme of the URL as a lowercase ASCII string.
///
/// Equivalent to `Url::scheme` in the `url` crate. The scheme is returned
/// without the trailing `:` delimiter and is always ASCII-lowercased during
/// parsing per the WHATWG URL Standard.
///
/// [`Url::scheme`]: https://docs.rs/url/latest/url/struct.Url.html#method.scheme
violet_net_string_view violet_net_url_scheme(const violet_net_url_t* url);

/// Returns whether the URL has a "special" scheme.
///
/// Special schemes as defined by the WHATWG URL Standard are: `ftp`, `file`,
/// `http`, `https`, `ws`, and `wss`. Special URLs have stricter parsing rules;
/// for instance, they always have a host (except `file:`), and backslashes are
/// normalized to forward slashes.
bool violet_net_url_is_special(const violet_net_url_t* url);

/// Returns whether the URL has an authority component.
///
/// Equivalent to [`Url::has_authority`] in the `url` crate. A URL has an authority
/// if its serialization contains `//` after the scheme. All special URLs except
/// `file:` always have an authority. Non-special URLs have an authority only if
/// they were parsed with `//`.
///
/// When true, the URL has a host and the path starts with `/`.
///
/// [`Url::has_authority`]: https://docs.rs/url/latest/url/struct.Url.html#method.has_authority
bool violet_net_url_has_authority(const violet_net_url_t* url);

/// Returns the authority component of the URL.
///
/// Equivalent to [`Url::authority`] in the `url` crate. The authority is the
/// substring between `//` and the start of the path, encompassing the
/// username, password, host, and port in the form `[user[:pass]@]host[:port]`.
///
/// Returns an empty string view for URLs without an authority.
///
/// [`Url::authority`]: https://docs.rs/url/latest/url/struct.Url.html#method.authority
violet_net_string_view violet_net_url_authority(const violet_net_url_t* url);

/// Returns the username component of the URL, if present.
///
/// Equivalent to [`Url::username`] in the `url` crate. Unlike most other optional
/// components, the `url` crate returns an empty string `""` when no username is
/// present rather than `None`. This wrapper follows the same convention: a
/// present-but-empty view means either no credentials or an empty username.
///
/// The returned username is percent-decoded.
///
/// [`Url::username`]: https://docs.rs/url/latest/url/struct.Url.html#method.username
violet_net_optional_string_view violet_net_url_username(const violet_net_url_t* url);

/// Returns the password component of the URL, if present.
///
/// Equivalent to [`Url::password`] in the `url` crate. Returns none when the URL
/// has no password, even if it has a username. A URL like `http://user@example.com`
/// has a username but no password.
///
/// The returned password is percent-decoded.
///
/// [`Url::password`]: https://docs.rs/url/latest/url/struct.Url.html#method.password
violet_net_optional_string_view violet_net_url_password(const violet_net_url_t* url);

/// Returns the port number explicitly specified in the URL.
///
/// Equivalent to [`Url::port`] in the `url` crate. Returns none when no port is
/// present in the serialization. Crucially, default ports for special schemes
/// are *stripped* during parsing; `https://example.com:443/` is normalized to
/// `https://example.com/` and this function returns none. Use
/// [`violet_net_url_port_or_known_default`] to get the effective port with
/// default fallback.
///
/// [`Url::port`]: https://docs.rs/url/latest/url/struct.Url.html#method.port
violet_net_optional_uint16 violet_net_url_port(const violet_net_url_t* url);

/// Returns the port number, falling back to the known default for the scheme.
///
/// Equivalent to [`Url::port_or_known_default`] in the `url` crate. If the URL
/// has an explicit port, that value is returned. Otherwise, returns the default
/// port for the scheme if one is known.
///
/// Known defaults:
///     * `ftp`   ~> 21
///     * `http`  ~> 80
///     * `https` ~> 443
///     * `ws`    ~> 80
///     * `wss`   ~> 443
///
/// Returns none for non-special schemes without an explicit port
/// and for `file:` URLs.
///
/// [`Url::port_or_known_default`]: https://docs.rs/url/latest/url/struct.Url.html#method.port_or_known_default
violet_net_optional_uint16 violet_net_url_port_or_known_default(const violet_net_url_t* url);

/// Returns whether the URL has a host.
///
/// Equivalent to [`Url::has_host`] in the `url` crate. URLs with an authority
/// always have a host. `file:` URLs may have an empty host (which still counts
/// as having a host) or no host at all (e.g., `file:///path`). Non-special URLs
/// without an authority never have a host.
///
/// [`Url::has_host`]: https://docs.rs/url/latest/url/struct.Url.html#method.has_host
bool violet_net_url_has_host(const violet_net_url_t* url);

/// Returns the host of the URL as a string, if present.
///
/// Equivalent to [`Url::host_str`] in the `url` crate. For domain hosts, returns
/// the serialized domain. For IPv4 addresses, returns the dotted-decimal form.
/// For IPv6 addresses, returns the address *without* surrounding brackets
/// (e.g., `"::1"` not `"[::1]"`), matching the `url` crate's `host_str` behavior.
///
/// Returns none for URLs without a host. Returns a present-but-empty view for
/// URLs with an empty host (e.g., `file:///path`).
///
/// [`Url::host_str`]: https://docs.rs/url/latest/url/struct.Url.html#method.host_str
violet_net_optional_string_view violet_net_url_host(const violet_net_url_t* url);

/// Returns the domain of the URL's host, if the host is a domain name.
///
/// Equivalent to [`Url::domain`] in the `url` crate. Returns none if the URL
/// has no host. An empty host in a `file:` URL is considered a domain and returns
/// a present-but-empty view.
///
/// [`Url::domain`]: https://docs.rs/url/latest/url/struct.Url.html#method.domain
violet_net_optional_string_view violet_net_url_domain(const violet_net_url_t* url);

/// Returns the path component of the URL.
///
/// Equivalent to [`Url::path`] in the `url` crate. For URLs with an authority,
/// the path always starts with `/`. For cannot-be-a-base URLs (opaque paths),
/// the path is the opaque string following the scheme (e.g., `mailto:user@example.com`
/// has path `"user@example.com"`).
///
/// The path is returned in percent-encoded form. Path segments like `.` and `..`
/// are resolved during parsing; the returned path is already normalized.
///
/// [`Url::path`]: https://docs.rs/url/latest/url/struct.Url.html#method.path
violet_net_string_view violet_net_url_path(const violet_net_url_t* url);

/// Returns the query string of the URL, if present.
///
/// Equivalent to [`Url::query`] in the `url` crate. The query is returned
/// *without* the leading `?` delimiter. An empty query string (from a URL
/// like `http://example.com/?`) is returned as a present-but-empty view,
/// which is distinct from none (no `?` at all).
///
/// The query is returned in its raw, percent-encoded form. Use
/// `url::form_urlencoded` on the Rust side for decoded key-value iteration.
///
/// [`Url::query`]: https://docs.rs/url/latest/url/struct.Url.html#method.query
violet_net_optional_string_view violet_net_url_query(const violet_net_url_t* url);

/// Returns the fragment of the URL, if present.
///
/// Equivalent to `Url::fragment` in the `url` crate. The fragment is returned
/// *without* the leading `#` delimiter. An empty fragment (from a URL like
/// `http://example.com/#`) is returned as a present-but-empty view, which is
/// distinct from none (no `#` at all).
///
/// Note that fragments are not included in HTTP requests; they are a
/// client-side concept. The `url` crate preserves them in the serialization.
///
/// [`Url::fragment`]: https://docs.rs/url/latest/url/struct.Url.html#method.fragment
violet_net_optional_string_view violet_net_url_fragment(const violet_net_url_t* url);

/// Returns a human-readable error message for a URL parse error.
///
/// The returned string is heap-allocated and must be freed by the caller
/// with `free(3)`.
char* violet_net_url_strerror(violet_net_url_error_t error);

#ifdef __cplusplus
}
#endif

// NOLINTEND(modernize-*,performance-enum-size)
