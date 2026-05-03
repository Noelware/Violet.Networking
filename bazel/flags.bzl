# 🌺💜 Violet.Networking: C++20 library that provides networking primitives
# Copyright (c) 2026 Noelware, LLC. <team@noelware.org>, et al.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

STRING_FLAGS = {
    "http_client_transport": {
        "default": "curl",
        "doc": "Determines the backend for `violet::net::http::Client`. By default, it'll use `libcurl` as it is majorily supported on all platforms.",
        "values": ["curl", "winhttp", "none"],
    },
}

BOOL_FLAGS = {
    "asan": {
        "default": False,
        "doc": "Enables the **Address** Sanitizer on each C++ target. Usually, this is meant for Bazel workspaces that don't provide custom C++ toolchain definitions.",
    },
    "msan": {
        "default": False,
        "doc": """Enables the **Memory** Sanitizer on each C++ target. Usually, this is meant for Bazel workspaces that don't provide custom C++ toolchain definitions.

        When invoked on `cc_test`s, the C++ standard library implementation will require to be compiled with MemorySanitizer. This will always fail in libstdc++, but libc++
        has MSan support, but you will need to compile it yourself; default toolchains of libc++ don't compile with MSan by default.""",
    },
    "tsan": {
        "default": False,
        "doc": "Enables the **Thread** Sanitizer on each C++ target. Usually, this is meant for Bazel workspaces that don't provide custom C++ toolchain definitions.",
    },
    "ubsan": {
        "default": False,
        "doc": "Enables the **Undefined Behaviour** Sanitizer on each C++ target. Usually, this is meant for Bazel workspaces that don't provide custom C++ toolchain definitions.",
    },
}
