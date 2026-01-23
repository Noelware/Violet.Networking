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

// NOLINTBEGIN(modernize-deprecated-headers)
#include <stddef.h>
#include <stdint.h>
// NOLINTEND(modernize-deprecated-headers)

#ifdef __cplusplus
extern "C" {
#endif

// NOLINTBEGIN(modernize-use-using,performance-enum-size,modernize-use-trailing-return-type)

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

/// A wrapper for Rust's <code>[`Option`]\<[`&str`]\></code> in C.
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
///     violet_net_url_free(url);
///     return 1;
/// } else {
///     printf("%s", violet_net_url_as_str(url));
///     violet_net_url_free(url);
///     return 0;
/// }
/// ```
///
/// @param input the input to use
/// @param error pointer to a error, which will return the error code (if any).
violet_net_url_t* violet_net_url_new(const char* input, violet_net_url_error_t* error);

violet_net_url_t* violet_net_url_new_with_params(
    const char* input, const char* params[], size_t len, violet_net_url_error_t* error);

violet_net_url_t* violet_net_url_join(const violet_net_url_t* url, const char* input, violet_net_url_error_t* error);
void violet_net_url_free(violet_net_url_t* url);
violet_net_string_view violet_net_url_as_str(const violet_net_url_t* url);
violet_net_string_view violet_net_url_scheme(const violet_net_url_t* url);
bool violet_net_url_is_special(const violet_net_url_t* url);
bool violet_net_url_has_authority(const violet_net_url_t* url);
violet_net_string_view violet_net_url_authority(const violet_net_url_t* url);
violet_net_optional_string_view violet_net_url_username(const violet_net_url_t* url);
violet_net_optional_string_view violet_net_url_password(const violet_net_url_t* url);
violet_net_optional_uint16 violet_net_url_port(const violet_net_url_t* url);
violet_net_optional_uint16 violet_net_url_port_or_known_default(const violet_net_url_t* url);
bool violet_net_url_has_host(const violet_net_url_t* url);
violet_net_optional_string_view violet_net_url_host(const violet_net_url_t* url);
violet_net_optional_string_view violet_net_url_domain(const violet_net_url_t* url);
violet_net_string_view violet_net_url_path(const violet_net_url_t* url);
violet_net_optional_string_view violet_net_url_query(const violet_net_url_t* url);
violet_net_optional_string_view violet_net_url_fragment(const violet_net_url_t* url);
char* violet_net_url_strerror(violet_net_url_error_t error);

// NOLINTEND(modernize-use-using,performance-enum-size,modernize-use-trailing-return-type)

#ifdef __cplusplus
}
#endif
