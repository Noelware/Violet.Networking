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
#include <violet/Iterator.h>

#include <functional>

namespace violet::net::http::header::detail {

/// Returns **true** if `input` is a valid header field-name according to [RFC9110 §5.1]
///
/// [RFC9110 §5.1]:
///
/// ```not_cpp
/// token = 1 * tchar
/// tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." /
//          "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA
/// ```
constexpr auto isValidHeaderName(violet::Str input) noexcept -> bool
{
    if (input.empty()) {
        return false;
    }

    for (char ch: input) {
        if (
            // clang-format off
            ch == '!' || ch == '#' ||
            ch == '$' || ch == '%' ||
            ch == '&' || ch == '\'' ||
            ch == '*' || ch == '+' ||
            ch == '-' || ch == '.' ||
            ch == '^' || ch == '_' ||
            ch == '`' || ch == '|' ||
            ch == '~'
            // clang-format on
        ) {
            continue;
        }

        if (ch >= 'A' && ch <= 'Z') {
            continue;
        }

        if (ch >= 'a' && ch <= 'z') {
            continue;
        }

        if (ch >= '0' && ch <= '9') {
            continue;
        }

        return false;
    }

    return true;
}

[[noreturn]] void invalidHeaderNameDetected() noexcept;

template<violet::UInt N>
struct LiteralName final {
    violet::Array<char, N> Data;
    violet::UInt Size;

    consteval VIOLET_IMPLICIT LiteralName(const char (&input)[N]) noexcept
        : Size(N - 1)
    {
        static_assert(N > 1, "header name cannot be empty");

        if (!header::detail::isValidHeaderName({ input, N - 1 })) {
            header::detail::invalidHeaderNameDetected();
        }

        for (violet::UInt i = 0; i < N - 1; i++) {
            char ch = input[i];
            Data[i] = (ch >= 'A' && ch <= 'Z') ? static_cast<char>(ch + 32) : ch;
        }

        Data[N - 1] = '\0';
    }
};

/// Returns **true** if `ch` is a valid header field-value character per [RFC9110 §5.5]:
///
/// ```not_cpp
/// field-value   = *field-content
/// field-content = field-vchar [ 1*( SP / HTAB / field-vchar ) field-vchar ]
/// field-vchar   = VCHAR / obs-text
/// VCHAR         = %x21-7E  (visible US-ASCII)
/// obs-text      = %x80-FF
/// ```
///
/// SP (0x20) and HTAB (0x09) are permitted as interior whitespace; leading and
/// trailing occurrences are stripped during construction, not validated here.
constexpr auto isValidHeaderValueCharacter(char ch) noexcept -> bool
{
    if (ch == ' ' || ch == '\t') {
        return true;
    }

    if (ch >= '\x21' && ch <= '\x7E') {
        return true;
    }

    // obs-text: 0x80–0xFF (permitted by RFC 9110 for legacy reasons)
    if (static_cast<unsigned char>(ch) >= 0x80) {
        return true;
    }

    return false;
}

/// Returns **true** if `input` is a valid header field-value according to [RFC9110 §5.5].
///
/// Validation rules:
/// - The value may be empty (empty field-value is legal).
/// - Every character must satisfy [`isValidHeaderValueChar`].
/// - CR (`\r`), LF (`\n`), and NUL (`\0`) are unconditionally rejected; this
///   also rejects deprecated obs-fold sequences (`CRLF SP/HTAB`).
constexpr auto isValidHeaderValue(violet::Str input) noexcept -> bool
{
    for (char ch: input) {
        if (ch == '\r' || ch == '\n' || ch == '\0') {
            return false;
        }

        if (!isValidHeaderValueCharacter(ch)) {
            return false;
        }
    }

    return true;
}

/// Strips leading and trailing SP/HTAB from `input` and returns the trimmed view.
///
/// Per RFC 9110 §5.5, optional whitespace (OWS) surrounding a field-value must
/// be removed before the value is stored or compared.
constexpr auto trimHeaderValue(violet::Str input) noexcept -> violet::Str
{
    violet::UInt start = 0;
    violet::UInt end = input.size();

    while (start < input.size() && (input[start] == ' ' || input[start] == '\t')) {
        ++start;
    }

    while (end > start && (input[end - 1] == ' ' || input[end - 1] == '\t')) {
        --end;
    }

    return input.substr(start, end - start);
}

template<violet::UInt N>
struct LiteralValue final {
    violet::Array<char, N> Data;
    violet::UInt Size = 0;

    VIOLET_IMPLICIT consteval LiteralValue(const char (&input)[N]) noexcept
    {
        violet::Str trimmed = trimHeaderValue({ input, N - 1 });
        if (!isValidHeaderValue(trimmed)) {
            header::detail::invalidHeaderNameDetected();
        }

        for (violet::UInt i = 0; i < trimmed.size(); i++) {
            Data[i] = trimmed[i];
        }

        Size = static_cast<violet::UInt>(trimmed.size());
        if (Size < N) {
            Data[Size] = '\0';
        }
    }
};

[[noreturn]] void invalidHeaderValueDetected() noexcept;

} // namespace violet::net::http::header::detail

namespace violet::net::http {

// namespace header {
//     template<typename Alloc>
//     struct Iterator;
// }

/// Value-based error type returned when a runtime header field-name validation is invalid.
struct InvalidHeaderName final {
    [[nodiscard]] auto ToString() const noexcept -> violet::CStr
    {
        return "invalid header name";
    }

    friend auto operator<<(std::ostream& os, const InvalidHeaderName& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

private:
    friend struct HeaderName;

    VIOLET_IMPLICIT InvalidHeaderName() noexcept = default;
};

/// Value-based error type returned when a runtime header field-value validation is invalid.
struct InvalidHeaderValue final {
    [[nodiscard]] auto ToString() const noexcept -> violet::CStr
    {
        return "invalid header value";
    }

    friend auto operator<<(std::ostream& os, const InvalidHeaderValue& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

private:
    friend struct HeaderValue;

    VIOLET_IMPLICIT InvalidHeaderValue() noexcept = default;
};

/// A validated, canonicalized HTTP header field-name.
///
/// Header names are case-insensitive per [RFC9110 §5.1]; this type enforces
/// that invariant by lowercasing all input at construction time. Once constructed,
/// a `HeaderValue` is guaranteed:
///
/// * To be non-empty
/// * Contain only `tchar` characters
/// * Be stored in lowercase (canonical form)
///
/// ## Construction
/// Prefer the `consteval` variant of [`HeaderName::From`] for string literals. Validation
/// and canonicalization happen entirely at compile-time.
///
/// ```cpp
/// // comptime:
/// HeaderName name = HeaderName::From<"Content-Type">();
///
/// // runtime:
/// auto result = HeaderName::From("x-my-header");
/// ```
struct HeaderName final {
    /// Constructs a `HeaderName` from a validated compile-time string literal.
    ///
    /// Validation and lowercasing are performed entirely at `consteval` time.
    /// Passing an invalid name (empty string, or a character outside `tchar`)
    /// is a **compile error**.
    ///
    /// # Example
    ///
    /// ```cpp
    /// constexpr auto name = HeaderName::From<"Content-Type">(); // stored as "content-type"
    /// constexpr auto bad  = HeaderName::From<"bad header">();   // compile error: space is not tchar
    /// ```
    template<header::detail::LiteralName Input>
    constexpr static auto From() noexcept -> HeaderName
    {
        return HeaderName(violet::Str(Input.Data.data(), Input.Size));
    }

    /// Constructs a `HeaderName` from a runtime string, returning
    /// `InvalidHeaderName` if validation fails.
    ///
    /// The input is lowercased in-place before being stored. An empty string
    /// or any character not in `tchar` causes a failure.
    ///
    /// # Example
    ///
    /// ```cpp
    /// auto ok  = HeaderName::From("x-request-id"); // Ok
    /// auto bad = HeaderName::From("bad header");   // Err: space not allowed
    /// ```
    template<std::convertible_to<violet::Str> String>
    constexpr static auto From(String&& input) noexcept -> violet::Result<HeaderName, InvalidHeaderName>
    {
        violet::String str(VIOLET_FWD(String, input));
        if (str.empty() || !header::detail::isValidHeaderName(str)) {
            return violet::Err(InvalidHeaderName{});
        }

        for (auto& ch: str) {
            if (ch >= 'A' && ch <= 'Z') {
                ch = static_cast<char>(ch + 32);
            }
        }

        return HeaderName(VIOLET_MOVE(str));
    }

    /// Returns a string view of the canonical field-name.
    [[nodiscard]] auto Get() const noexcept -> violet::Str
    {
        return this->n_canonical;
    }

    /// Returns a stringified field-name as an owned string.
    [[nodiscard]] auto ToString() const noexcept -> violet::String
    {
        return this->n_canonical;
    }

    friend auto operator<<(std::ostream& os, const HeaderName& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

    constexpr VIOLET_EXPLICIT operator violet::Str() const noexcept
    {
        return this->n_canonical;
    }

    constexpr VIOLET_EXPLICIT operator violet::CStr() const noexcept
    {
        return this->n_canonical.c_str();
    }

    constexpr auto operator<=>(const HeaderName&) const noexcept -> std::strong_ordering = default;
    constexpr auto operator==(const HeaderName&) const noexcept -> bool = default;
    constexpr auto operator!=(const HeaderName&) const noexcept -> bool = default;

    constexpr auto operator<=>(const violet::String& str) const noexcept -> std::strong_ordering
    {
        return this->n_canonical <=> str;
    }

    constexpr auto operator==(const violet::String& str) const noexcept -> bool
    {
        return this->n_canonical == str;
    }

    constexpr auto operator!=(const violet::String& str) const noexcept -> bool
    {
        return !(*this == str);
    }

private:
    constexpr VIOLET_EXPLICIT HeaderName(violet::Str input)
        : n_canonical(input)
    {
    }

    constexpr VIOLET_EXPLICIT HeaderName(violet::String input)
        : n_canonical(VIOLET_MOVE(input))
    {
    }

    violet::String n_canonical;
};

/// A validated HTTP header field-value.
///
/// Header values are governed by [RFC9110 §5.5]. Unlike [`HeaderName`], values **are not**
/// case-folded; canonicalization is limited to stripping leading and trailing OWS (SP / HTAB). Once
/// constructed, a `HeaderValue` is guaranteed:
///
/// * To contain only permitted characters (visible ASCII, SP, HTAB, obs-text).
/// * To contain no CR (`\r`), LF (`\n`), or NUL bytes (obs-fold is rejected).
/// * To have no leading or trailing whitespace.
///
/// ## Construction
/// As with [`HeaderName`], prefer the `consteval` path for literals:
///
/// ```cpp
/// // comptime:
/// HeaderValue value = HeaderValue::From<"application/json; utf-8">();
///
/// // runtime:
/// auto result = HeaderValue::From(userInput);
/// ```
struct HeaderValue final {
    /// Constructs a `HeaderValue` from a validated compile-time string literal.
    ///
    /// Leading/trailing whitespace is stripped and validation is performed
    /// entirely at `consteval` time. An invalid character (CR, LF, NUL, or
    /// any non-field-vchar) is a **compile error**.
    ///
    /// ## Example
    /// ```cpp
    /// constexpr auto val = HeaderValue::From<"application/json">();
    /// constexpr auto bad = HeaderValue::From<"val\r\nue">();  // compile error
    /// ```
    template<header::detail::LiteralValue Input>
    constexpr static auto From() noexcept -> HeaderValue
    {
        return HeaderValue(violet::Str(Input.Data.data(), Input.Size));
    }

    /// Constructs a `HeaderValue` from a runtime string.
    ///
    /// Leading and trailing OWS is stripped before validation. Returns an
    /// `InvalidHeaderValue` sentinel if any byte is illegal.
    ///
    /// ## Example
    /// ```cpp
    /// auto ok  = HeaderValue::From("  gzip  ");  // stored as "gzip"
    /// auto bad = HeaderValue::From("val\nue");    // Err: LF not permitted
    /// ```
    template<std::convertible_to<violet::Str> String>
    constexpr static auto From(String&& input) noexcept -> violet::Result<HeaderValue, InvalidHeaderValue>
    {
        violet::Str str(VIOLET_FWD(String, input));
        violet::Str trimmed = header::detail::trimHeaderValue(str);
        if (!header::detail::isValidHeaderValue(trimmed)) {
            return Err(InvalidHeaderValue{});
        }

        return HeaderValue(trimmed);
    }

    /// Returns a string view of the stored field-value.
    [[nodiscard]] auto Get() const noexcept -> violet::Str
    {
        return this->n_value;
    }

    /// Returns the stored field-value as an owned string.
    [[nodiscard]] auto ToString() const noexcept -> violet::String
    {
        return this->n_value;
    }

    friend auto operator<<(std::ostream& os, const HeaderValue& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }

    constexpr VIOLET_EXPLICIT operator violet::Str() const noexcept
    {
        return this->n_value;
    }

    constexpr VIOLET_EXPLICIT operator violet::CStr() const noexcept
    {
        return this->n_value.c_str();
    }

    constexpr auto operator<=>(const HeaderValue&) const noexcept -> std::strong_ordering = default;
    constexpr auto operator==(const HeaderValue&) const noexcept -> bool = default;
    constexpr auto operator!=(const HeaderValue&) const noexcept -> bool = default;

    constexpr auto operator<=>(const violet::String& str) const noexcept -> std::strong_ordering
    {
        return this->n_value <=> str;
    }

    constexpr auto operator==(const violet::String& str) const noexcept -> bool
    {
        return this->n_value == str;
    }

    constexpr auto operator!=(const violet::String& str) const noexcept -> bool
    {
        return !(*this == str);
    }

private:
    constexpr VIOLET_EXPLICIT HeaderValue(violet::Str input)
        : n_value(input)
    {
    }

    constexpr VIOLET_EXPLICIT HeaderValue(violet::String input)
        : n_value(VIOLET_MOVE(input))
    {
    }

    violet::String n_value;
};

} // namespace violet::net::http

template<>
struct std::hash<violet::net::http::HeaderName> final {
    using is_transparent = void;

    auto operator()(const violet::net::http::HeaderName& name) const noexcept -> violet::UInt
    {
        constexpr std::hash<violet::Str> hash;
        return hash(name.Get());
    }

    auto operator()(violet::Str str) const noexcept -> violet::UInt
    {
        constexpr std::hash<violet::Str> hash;
        return hash(str);
    }
};

template<>
struct std::hash<violet::net::http::HeaderValue> final {
    auto operator()(const violet::net::http::HeaderValue& value) noexcept -> violet::UInt
    {
        constexpr std::hash<violet::Str> hash;
        return hash(value.Get());
    }
};

namespace violet::net::http {

/// A map of validated HTTP header field-names to field-values.
///
/// `Headers` is a thin, ergonomic wrapper around [`std::unordered_map`] keyed by
/// [`HeaderName`] and valued by [`HeaderValue`]. Because both types enforce [RFC 9110]
/// invariants at compile and construction time, any entry that exists in this map is guaranteed
/// to be well-formed.
///
/// The underlying type also exposes a `Alloc` template parameter so callers can supply
/// a custom allocator. (e.g., an arena allocator that can be scoped to a single request)
///
/// ## Canonicalization
/// Keys are lowercased on insertion (enforced by [`HeaderName`]); lookups via the `consteval`
/// template overloads (e.g., `Get<"Content-Type">`) perform the same cononicalization at
/// compile-time, so there is no runtime cost for well-known header names.
///
/// ## Example
/// ```cpp
/// #include <violet/Networking/HTTP/Headers.h>
///
/// using namespace violet::net::http;
/// using namespace violet::net::http::header;
///
/// Headers headers;
///
/// // 1. Use both template parameters so it can be validated at compile-time.
/// //    Recommended only if the header name isn't a well-known one.
/// headers.Insert<"Content-Type", "application/json">();
///
/// // 2. Use the name template parameter so that the header name can be validated
/// //    at compile time. Recommended if the header value comes from a runtime
/// //    constructed string.
/// headers.Insert<"Content-Type">(HeaderValue::From("application/json").Unwrap());
///
/// // 3. Both name and value are runtime-constructed.
/// headers.Insert(HeaderName::From("Content-Type").Unwrap(), HeaderValue::From("application/json").Unwrap());
///
/// if (auto contentType = headers.Get<"Content-Type">(); contentType.HasValue()) {
///     violet::Println("Content-Type: {}", *contentType);
/// }
/// ```
template<typename Alloc = std::allocator<violet::Pair<const HeaderName, HeaderValue>>>
    requires(
        std::same_as<typename std::allocator_traits<Alloc>::value_type, violet::Pair<const HeaderName, HeaderValue>>)
struct Headers final {
    using Map = std::unordered_map<HeaderName, HeaderValue, std::hash<HeaderName>, std::equal_to<>, Alloc>;
    using size_type = Map::size_type;

    /// Constructs an empty `Headers` map using the default allocator.
    VIOLET_IMPLICIT Headers() noexcept(std::is_nothrow_constructible_v<Map>) = default;

    /// Constructs an empty `Headers` map with at least `size` buckets
    /// pre-allocated using the default allocator.
    VIOLET_EXPLICIT Headers(size_type size) noexcept(std::is_nothrow_constructible_v<Map, size_type>)
        : n_map(size)
    {
    }

    /// Constructs an empty `Headers` map using the selected allocator.
    template<typename A = Alloc>
    VIOLET_IMPLICIT Headers(const A& alloc) noexcept(std::is_nothrow_constructible_v<Map, const A&>)
        requires(std::same_as<A, Alloc> && std::uses_allocator_v<Map, Alloc>)
        : n_map(alloc)
    {
    }

    /// Constructs an empty `Headers` map with at least `size` buckets
    /// pre-allocated using the selected allocator.
    VIOLET_IMPLICIT Headers(size_type size, const Alloc& alloc) noexcept(
        std::is_nothrow_constructible_v<Map, size_type, const Alloc&>)
        : n_map(size, alloc)
    {
    }

    /// Constructs a `Headers` map from the iterator range `[first, last)`.
    ///
    /// Each element in the range must be a `violet::Pair<HeaderName, HeaderValue>`.
    template<typename Iterator>
    VIOLET_IMPLICIT Headers(Iterator first, Iterator last) noexcept(
        std::is_nothrow_constructible_v<Map, Iterator, Iterator>)
        : n_map(first, last)
    {
    }

    /// Constructs a `Headers` map from the iterator range `[first, last)` using
    /// the selected allocator.
    ///
    /// Each element in the range must be a `violet::Pair<HeaderName, HeaderValue>`.
    template<typename Iterator>
    VIOLET_IMPLICIT Headers(Iterator first, Iterator last, const Alloc& alloc) noexcept(
        std::is_nothrow_constructible_v<Map, Iterator, Iterator, const Alloc&>)
        : n_map(first, last, alloc)
    {
    }

    /// Constructs a `Headers` map from `[first, last)` with at least `size`
    /// buckets pre-allocated with the default allocator.
    template<typename Iterator>
    VIOLET_IMPLICIT Headers(Iterator first, Iterator last, size_type size) noexcept(
        std::is_nothrow_constructible_v<Map, Iterator, Iterator, size_type>)
        : n_map(first, last, size)
    {
    }

    /// Constructs a `Headers` map from `[first, last)` with at least `size`
    /// buckets pre-allocated with the selected allocator.
    template<typename Iterator>
    VIOLET_IMPLICIT Headers(Iterator first, Iterator last, size_type size, const Alloc& alloc) noexcept(
        std::is_nothrow_constructible_v<Map, Iterator, Iterator, size_type, const Alloc&>)
        : n_map(first, last, size, alloc)
    {
    }

    /// Constructs a `Headers` map from an initializer list.
    ///
    /// # Example
    ///
    /// ```cpp
    /// Headers h = {
    ///     { HeaderName::From<"content-type">(), HeaderValue::From<"text/plain">() },
    ///     { HeaderName::From<"accept">(),       HeaderValue::From<"*/*">()        },
    /// };
    /// ```
    VIOLET_IMPLICIT Headers(std::initializer_list<violet::Pair<const HeaderName, HeaderValue>> il) noexcept(
        std::is_nothrow_constructible_v<Map, std::initializer_list<violet::Pair<HeaderName, HeaderValue>>>)
        : n_map(il)
    {
    }

    /// Constructs a `Headers` map from an initializer list using the selected allocator.
    VIOLET_IMPLICIT Headers(std::initializer_list<violet::Pair<HeaderName, HeaderValue>> il,
        const Alloc& alloc) noexcept(std::is_nothrow_constructible_v<Map,
        std::initializer_list<violet::Pair<HeaderName, HeaderValue>>, const Alloc&>)
        : n_map(il, alloc)
    {
    }

    /// Constructs a `Headers` map from an initializer list with at least
    /// `size` buckets pre-allocated with the default allocator.
    VIOLET_IMPLICIT Headers(std::initializer_list<violet::Pair<HeaderName, HeaderValue>> il, size_type size) noexcept(
        std::is_nothrow_constructible_v<Map, std::initializer_list<violet::Pair<HeaderName, HeaderValue>>, size_type>)
        : n_map(il, size)
    {
    }

    /// Constructs a `Headers` map from an initializer list with at least
    /// `size` buckets pre-allocated with the selected allocator.
    VIOLET_IMPLICIT Headers(std::initializer_list<violet::Pair<HeaderName, HeaderValue>> il, size_type size,
        const Alloc& alloc) noexcept(std::is_nothrow_constructible_v<Map,
        std::initializer_list<violet::Pair<HeaderName, HeaderValue>>, size_type, const Alloc&>)
        : n_map(il, size, alloc)
    {
    }

    /// Returns **true** if the map contains no entries.
    [[nodiscard]] auto Empty() const noexcept(noexcept(std::declval<Map>().empty())) -> bool
    {
        return this->n_map.empty();
    }

    /// Returns the number of entries in this map.
    [[nodiscard]] auto Size() const noexcept(noexcept(std::declval<Map>().size())) -> size_type
    {
        return this->n_map.size();
    }

    /// Returns the maximum number of entries the map can theoretically hold.
    [[nodiscard]] auto MaxSize() const noexcept(noexcept(std::declval<Map>().max_size())) -> size_type
    {
        return this->n_map.max_size();
    }

    /// Inserts `value` under the compile-time validated header name `Name`.
    ///
    /// If the header name is already present, the existing value is left unchanged
    /// and the old value is returned as a optional value. If the key was never present
    /// in this map, it'll return [`violet::Nothing`].
    template<header::detail::LiteralName Name>
    auto Insert(const HeaderValue& value) noexcept(
        noexcept(std::declval<Map>().insert(std::declval<violet::Pair<HeaderName, HeaderValue>>())))
        -> violet::Optional<HeaderValue>
    {
        return this->Insert(HeaderName::From<Name>(), value);
    }

    /// Inserts the compile-time validated `Value` under the compile-time validated
    /// header name `Name`.
    ///
    /// If the header name is already present, the existing value is left unchanged
    /// and the old value is returned as a optional value. If the key was never present
    /// in this map, it'll return [`violet::Nothing`].
    template<header::detail::LiteralName Name, header::detail::LiteralValue Value>
    auto Insert() noexcept(noexcept(std::declval<Map>().insert(std::declval<violet::Pair<HeaderName, HeaderValue>>())))
        -> violet::Optional<HeaderValue>
    {
        return this->Insert(HeaderName::From<Name>(), HeaderValue::From<Value>());
    }

    /// Inserts `value` under `name`.
    ///
    /// If the header name is already present, the existing value is left unchanged
    /// and the old value is returned as a optional value. If the key was never present
    /// in this map, it'll return [`violet::Nothing`].
    auto Insert(const HeaderName& name, const HeaderValue& value) noexcept(
        noexcept(std::declval<Map>().insert(std::declval<violet::Pair<HeaderName, HeaderValue>>())))
        -> violet::Optional<HeaderValue>
    {
        auto [it, inserted] = this->n_map.insert(std::make_pair(name, value));
        if (inserted) {
            return violet::Nothing;
        }

        return this->Get(it);
    }

    /// Inserts all entries from `il` into the map.
    ///
    /// Duplicate keys in `il` follow `std::unordered_map::insert` semantics:
    /// the first occurrence wins.
    void Insert(std::initializer_list<violet::Pair<const HeaderName, HeaderValue>> il) noexcept(
        noexcept(std::declval<Map>().insert(il)))
    {
        this->n_map.insert(il);
    }

    /// Returns the value associated with the compile-time validated header name `Name`,
    /// or [`violet::Nothing`] if not present otherwise.
    template<header::detail::LiteralName Name>
    [[nodiscard]] auto Get() const -> violet::Optional<HeaderValue>
    {
        return this->Get(HeaderName::From<Name>());
    }

    /// Returns the value associated with `name`, or `violet::Nothing` if not present.
    [[nodiscard]] auto Get(const HeaderName& name) const -> violet::Optional<HeaderValue>
    {
        auto it = this->n_map.find(name);
        if (it == this->n_map.end()) {
            return violet::Nothing;
        }

        return it->second;
    }

    /// Returns the value associated with that specific iterator.
    auto Get(typename Map::iterator it) -> violet::Optional<HeaderValue>
    {
        if (it == this->n_map.end()) {
            return violet::Nothing;
        }

        return it->second;
    }

    /// Returns the value associated with that specific iterator.
    auto Get(typename Map::const_iterator it) const -> violet::Optional<HeaderValue>
    {
        if (it == this->n_map.end()) {
            return violet::Nothing;
        }

        return it->second;
    }

    /// Returns the value for the compile-time validated header name `Name`, inserting
    /// `value` if the key is absent.
    ///
    /// Unlike [`Headers::Insert`], this will always return the stored value (either the existing
    /// one or the newly assigned `value`.)
    template<header::detail::LiteralName Name>
    [[nodiscard]] auto GetOrAssign(HeaderValue&& value) -> HeaderValue
    {
        return this->GetOrAssign(HeaderName::From<Name>(), VIOLET_MOVE(value));
    }

    /// Returns the value for `name`, inserting `value` if the key is absent.
    [[nodiscard]] auto GetOrAssign(const HeaderName& name, HeaderValue&& value) -> HeaderValue
    {
        auto [it, _] = this->n_map.try_emplace(name, VIOLET_MOVE(value));
        return it->second;
    }

    /// Removes the entry with the compile-time header name `Name`. Returns **true**
    /// if the entry was erased or **false** otherwise.
    template<header::detail::LiteralName Name>
    [[nodiscard]] auto Erase() -> bool
    {
        return this->Erase(HeaderName::From<Name>());
    }

    /// Removes the entry with key `name`. Returns **true** if the entry was erased
    // or **false** otherwise.
    auto Erase(const HeaderName& key) -> bool
    {
        return this->n_map.erase(key) > 0;
    }

    /// Removes all entries in this header map.
    void Clear() noexcept(noexcept(std::declval<Map>().clear()))
    {
        this->n_map.clear();
    }

    /// Sets the number of buckets to atleast `size` and rehashes the map.
    void Rehash(size_type size) noexcept(noexcept(std::declval<Map>().rehash(size)))
    {
        this->n_map.rehash(size);
    }

    /// Reserves space for atleast `size` entries without exceeding the maximum load factor.
    void Reserve(size_type size) noexcept(noexcept(std::declval<Map>().reserve(size)))
    {
        this->n_map.reserve(size);
    }

    /// Merges and moves all entries from `source` into this map. Entries whose keys already
    /// exist in this map are left in `source`.
    void Merge(Map& source)
    {
        this->n_map.merge(source);
    }

    /// Merges and moves all entries from `headers` into this map. Entries whose keys already
    /// exist in this map are left in `headers`.
    void Merge(Headers& headers)
    {
        this->n_map.merge(headers.n_map);
    }

    /// Moves all entries from the rvalue `headers` into this map.
    void Merge(Headers&& headers)
    {
        this->n_map.merge(VIOLET_MOVE(headers).n_map);
    }

    /// Returns **true** if the compile-time validated header name `Name` is present in the map.
    template<header::detail::LiteralName Name>
    [[nodiscard]] auto Contains() const -> bool
    {
        return this->Contains(HeaderName::From<Name>());
    }

    /// Returns **true** if `name` is present in the map.
    auto Contains(const HeaderName& name) const -> bool
    {
        return this->n_map.contains(name);
    }

    // /// Returns a iterator of all entries in this map.
    // ///
    // /// Iteration order is unspecified (by hash-map semantics). This is used
    // /// with the combinators from Noelware.Violet's iterator framework.
    // [[nodiscard]] auto Iter() const noexcept -> header::Iterator<Alloc>
    // {
    //     return header::Iterator(this->n_map);
    // }

    auto operator[](const HeaderName& name) -> HeaderValue&
    {
        return this->n_map[name];
    }

    VIOLET_EXPLICIT operator Map() const noexcept
    {
        return this->n_map;
    }

    /// Provides a C++-style iterator. Prefer [`Headers::Iter`] over this.
    auto begin() noexcept -> auto
    {
        return this->n_map.begin();
    }

    /// Provides a C++-style iterator. Prefer [`Headers::Iter`] over this.
    auto end() noexcept -> auto
    {
        return this->n_map.end();
    }

private:
    Map n_map;
};

Headers(typename Headers<>::size_type) -> Headers<std::allocator<violet::Pair<const HeaderName, HeaderValue>>>;

template<typename Alloc>
Headers(typename Headers<>::size_type, const Alloc& alloc) -> Headers<Alloc>;

template<typename Alloc>
Headers(typename Headers<Alloc>::size_type, const Alloc&) -> Headers<Alloc>;

template<std::input_iterator Iterator>
Headers(Iterator, Iterator) -> Headers<std::allocator<violet::Pair<const HeaderName, HeaderValue>>>;

template<std::input_iterator Iterator, typename Alloc>
    requires(std::same_as<typename Alloc::value_type, violet::Pair<const HeaderName, HeaderValue>>)
Headers(Iterator, Iterator, Alloc) -> Headers<Alloc>;

template<std::input_iterator Iterator>
Headers(Iterator, Iterator, typename Headers<std::allocator<violet::Pair<const HeaderName, HeaderValue>>>::size_type)
    -> Headers<std::allocator<violet::Pair<const HeaderName, HeaderValue>>>;

template<std::input_iterator Iterator, typename Alloc>
    requires(std::same_as<typename Alloc::value_type, violet::Pair<const HeaderName, HeaderValue>>)
Headers(Iterator, Iterator, typename Headers<Alloc>::size_type, Alloc) -> Headers<Alloc>;

Headers(std::initializer_list<violet::Pair<HeaderName, HeaderValue>>)
    -> Headers<std::allocator<violet::Pair<const HeaderName, HeaderValue>>>;

template<typename Alloc>
Headers(std::initializer_list<violet::Pair<HeaderName, HeaderValue>>, Alloc) -> Headers<Alloc>;

Headers(std::initializer_list<violet::Pair<HeaderName, HeaderValue>>,
    typename Headers<std::allocator<violet::Pair<const HeaderName, HeaderValue>>>::size_type)
    -> Headers<std::allocator<violet::Pair<const HeaderName, HeaderValue>>>;

template<typename Alloc>
Headers(std::initializer_list<violet::Pair<HeaderName, HeaderValue>>, typename Headers<Alloc>::size_type, Alloc)
    -> Headers<Alloc>;

} // namespace violet::net::http

// namespace violet::net::http::header {

// /// A forward-iterator over the entries of a [`Headers`] map.
// ///
// /// `Iterator` yields over [`violet::Pair`]\<[`HeaderName`], [`HeaderValue`]\> items in an
// /// unspecified order.
// ///
// /// ## Example
// /// ```cpp
// /// for (auto [name, value]: headers.Iter()) {
// ///     violet::Println("{}: {}", name, value);
// /// }
// /// ```
// template<typename Alloc = std::allocator<violet::Pair<const HeaderName, HeaderValue>>>
// struct Iterator final: public violet::Iterator<Iterator<Alloc>> {
//     using Item = violet::Pair<HeaderName, HeaderValue>;

//     /// Advances the iterator and returns the next entry, or [`violet::Nothing`]
//     /// when exhausted.
//     auto Next() noexcept -> Optional<Item>
//     {
//         return this->n_iter.Next();
//     }

// private:
//     friend struct Headers<Alloc>;

//     VIOLET_EXPLICIT Iterator(const typename Headers<Alloc>::Map& map) noexcept
//         : n_iter(violet::iter::detail::STLCompatibleIterator(map.begin(), map.end()))
//     {
//     }

//     decltype(violet::MkIterable(std::declval<typename Headers<Alloc>::Map>())) n_iter;
// };

// } // namespace violet::net::http::header
