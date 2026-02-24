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
//! # 🌺💜 `violet/Networking/HTTP/cURL/Global.h`

#pragma once

#include <violet/Language/Macros.h>
#include <violet/Language/Policy.h>

#include <cstdint>

namespace violet::net::curl {

/// RAII guard to initialize `libcurl`
///
/// ## Example
/// ```cpp
/// #include <violet/Networking/HTTP/cURL/Global.h>
/// #include <violet/Networking/HTTP/Client.h>
///
/// using namespace violet::net;
///
/// auto main() -> int {
///     curl::Global libcurlInit;
///     http::Client client;
///
///     // ... perform
///
///     return 0;
/// }
/// ```
struct Global final {
    /// Initializes `libcurl`. This will abort the process if libcurl were to
    /// ever fail initialization.
    VIOLET_IMPLICIT Global() noexcept;

    /// Initializes `libcurl`. This will abort the process if libcurl were to
    /// ever fail initialization.
    ///
    /// @param flags list of cURL flags to pass by.
    VIOLET_IMPLICIT Global(int64_t flags) noexcept;
};

} // namespace violet::net::curl
