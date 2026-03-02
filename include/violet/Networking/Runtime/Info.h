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

#include <violet/Language/Policy.h> // IWYU: pragma export

#include <format>
#include <string>

namespace violet::net {

/// Full numeric version identifier.
///
/// The version integer is encoded into four components in decimal digits:
/// ```
/// YYMMPPBB
/// ││││││││
/// ││││││└┘└┘── build increment (only in devbuilds, always 0 on release builds)
/// ││││└┘────── patch increment
/// ││└┘──────── current month + 1 (i.e, 02 = January, 03 = Feburary, etc)
/// └┘────────── year   (e.g. 26)
/// ```
constexpr static const int VERSION = VIOLET_NET_VERSION;

/// The major year component (e.g. `26`).
constexpr static const int YEAR = VIOLET_NET_VERSION / 1'000'000;

/// The month component (`1` ~ `12`).
constexpr static const int MONTH = (VIOLET_NET_VERSION / 10000) % 100;

/// The patch component; value of `0` indicates no patch.
constexpr static const int PATCH = (VIOLET_NET_VERSION / 100) % 100;

/// The build component; only available in dev builds.
constexpr static const int BUILD = VIOLET_NET_VERSION % 100;

#ifdef VIOLET_NET_DEVBUILD
/// Returns **true** if this is a development build.
constexpr static const bool DEVBUILD = true;
#else
/// Returns **true** if this is a development build.
constexpr static const bool DEVBUILD = false;
#endif

/// Returns the library version as a human-readable string.
///
/// The format follows a calendar-versioning scheme:
///
/// ```
/// YEAR.MONTH[.PATCH][-dev[.BUILD]]
/// ```
///
/// - `PATCH` is omitted when zero.
/// - The `-dev` suffix and optional `.BUILD` are appended only when
///   `VIOLET_DEVBUILD` is defined.
///
/// # Examples
///
/// Given `VIOLET_NET_VERSION = 2026'06'03'00`:
///
/// ```cpp
/// // Release build
/// Version() == "2026.06.03"
///
/// // Dev build (VIOLET_NET_DEVBUILD defined), BUILD == 0
/// Version() == "2026.06.03-dev"
/// ```
///
/// Given `VIOLET_NET_VERSION = 2026'01'00'05` with `VIOLET_NET_DEVBUILD`:
///
/// ```cpp
/// Version() == "2026.01-dev.5"
/// ```
constexpr auto Version() noexcept -> std::string
{
    std::string version = std::format("{}.{:02}", YEAR, MONTH);
    if constexpr (PATCH > 0) {
        version.append(std::format(".{:02}", PATCH));
    }

#ifdef VIOLET_NET_DEVBUILD
    version.append("-dev");

    if constexpr (BUILD > 0) {
        version.append(".{}", BUILD);
    }
#endif

    return version;
}

} // namespace violet::net
