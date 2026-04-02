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
//! # 🌺💜 `violet/Networking/HTTP/Body.h`
//! Provides the `Body` type representing an HTTP request or response body, along
//! with the `Codec` concept for encoding arbitrary values into a serialized body.
//!
//! A `Body` holds one of three states:
//! - **Empty** — no body content (default-constructed).
//! - **Text** — a plain UTF-8 string sent as `application/text`.
//! - **Codec** — bytes produced by a `Codec` implementation, paired with the
//!   content-type that codec declares.

#pragma once

#include <violet/Container/Optional.h>
#include <violet/Experimental/OneOf.h>
#include <violet/Violet.h>

#include <concepts>
#include <variant>

namespace violet::net::http {

/// Concept satisfied by types that can encode a value of type `T` into raw bytes
/// for use as an HTTP body.
///
/// A conforming `CodecT` must expose two static members:
/// - `ContentType()` — returns a `Str` (or anything convertible to it) describing
///   the MIME type of the encoded output (e.g. `"application/json"`).
/// - `Encode(T&&)` — accepts an rvalue of `T` and returns the encoded byte
///   representation (e.g. `Vec<UInt8>`).
///
/// ## Example
/// ```cpp
/// struct JsonCodec {
///     static auto ContentType() -> violet::Str { return "application/json"; }
///     static auto Encode(MyType&& value) -> Vec<UInt8> { /* ... */ }
/// };
///
/// static_assert(violet::net::http::Codec<JsonCodec, MyType>);
/// ```
template<typename CodecT, typename T>
concept Codec = requires {
    { CodecT::ContentType() } -> std::convertible_to<violet::Str>;
    { CodecT::Encode(VIOLET_FWD(T, std::declval<T&&>())) };
};

/// Represents an HTTP request or response body.
///
/// A `Body` is move-only and holds one of three internal states:
/// - **Empty** — no payload; `Empty()` returns `true` and `Data()` yields an empty span.
/// - **Text** — a UTF-8 string payload; `ContentType()` returns `"application/text"`.
/// - **Codec** — bytes produced by a `Codec` implementation, with the content-type
///   declared by that codec.
///
/// Use the static factory methods to construct a body:
/// - `Body::Bytes` for raw byte buffers.
/// - `Body::Text` for plain-text strings.
/// - `Body::From<CodecT>` for codec-encoded values.
struct Body final {
    /// Constructs an empty body with no payload.
    VIOLET_IMPLICIT Body() noexcept = default;
    ~Body() = default;

    VIOLET_DISALLOW_COPY(Body);
    VIOLET_IMPLICIT_MOVE(Body);

    /// Constructs a body from a raw byte buffer with the given content-type.
    ///
    /// @param data        The raw bytes to use as the body payload.
    /// @param contentType The MIME type of the payload. Defaults to `"application/octet-stream"`.
    static auto Bytes(Vec<UInt8>&& data, Str contentType = "application/octet-stream") noexcept -> Body;

    /// Constructs a body from a UTF-8 string with content-type `application/text`.
    ///
    /// @param data Any value convertible to `String`.
    template<std::convertible_to<String> Str>
    static auto Text(Str&& data) noexcept -> Body
    {
        Body body;
        body.n_value = String(VIOLET_FWD(Str, data));

        return body;
    }

    /// Constructs a body by encoding `data` with the given `CodecT`.
    ///
    /// `CodecT::Encode` is called with `data` to produce the byte payload, and
    /// `CodecT::ContentType` is used to set the body's content-type header value.
    ///
    /// @tparam CodecT A type satisfying the `Codec<CodecT, T>` concept.
    /// @param  data   The value to encode.
    template<typename CodecT, typename T>
        requires(Codec<CodecT, T>)
    static auto From(T&& data) noexcept -> Body
    {
        Body body;
        body.n_value = codec_t(CodecT::Encode(VIOLET_FWD(T, data)), CodecT::ContentType());

        return body;
    }

    /// Returns the content-type of this body, or `None` if the body is empty.
    [[nodiscard]] auto ContentType() const noexcept -> Optional<Str>;

    /// Returns a read-only view over the raw bytes of this body.
    ///
    /// Returns an empty span when the body is in the empty or text state.
    [[nodiscard]] auto Data() const noexcept -> Span<const UInt8>;

    /// Returns `true` if the body carries no payload.
    [[nodiscard]] constexpr auto Empty() const noexcept -> bool
    {
        if (this->n_value.Holds<std::monostate>()) {
            return true;
        }

        if (auto codec = this->n_value.Get<codec_t>()) {
            return codec->Data.empty();
        }

        return false;
    }

private:
    struct codec_t final {
        Vec<UInt8> Data;
        Str ContentType;
    };

    // clang-format off
    experimental::OneOf<
        std::monostate,  //< empty state
        String,          //< text state ("application/text")
        codec_t          //< contextual from a codec
    > n_value = std::monostate{};
    // clang-format on
};

} // namespace violet::net::http
