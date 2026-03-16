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

#include <violet/Networking/HTTP/Body.h>

using violet::Optional;
using violet::Str;
using violet::Vec;
using violet::net::http::Body;

auto Body::Bytes(Vec<UInt8>&& data, Str contentType) noexcept -> Body
{
    Body body;
    body.n_value = codec_t(VIOLET_MOVE(data), contentType);

    return body;
}

auto Body::ContentType() const noexcept -> Optional<Str>
{
    return this->n_value.Match(
        // clang-format off
        [](const std::monostate&) -> Optional<Str> { return Nothing; },
        [](const String&) -> Optional<Str> { return "text/plain"; },
        [](const codec_t& codec) -> Optional<Str> { return codec.ContentType; }
        // clang-format on
    );
}

auto Body::Data() const noexcept -> Span<const UInt8>
{
    return this->n_value.Match(
        // clang-format off
        [](const std::monostate&) -> Span<const UInt8> { return {}; },
        [](const String& text) -> Span<const UInt8> { return {reinterpret_cast<const UInt8*>(text.data()), text.size()}; },
        [](const codec_t& codec) -> Span<const UInt8> { return codec.Data; }
        // clang-format on
    );
}
