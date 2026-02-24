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
    # ## `--@violet.net//bazel/flags:http_client_backend=[curl|winhttp|none]`
    # Determines the backend for the `violet::net::HttpClient` C++ struct. By default,
    # it'll use `libcurl` as that is majorily supported on all platforms but there is
    # a WinHTTP backend for Windows users.
    #
    # You can set this to `none` and build your own backend as you wish, you can
    # view a reference at in the `src/http/backends/cURL.cc` file.
    "http_client_backend": ["curl", "winhttp", "none"],
}

BOOL_FLAGS = [
    # ## `--@violet.net//bazel/flags:logrin=[True|False]
    # Enables the use of Noel's `logrin` (https://github.com/auguwu/logrin) C++ library
    # to provide rich logs for the HTTP client.
    #
    # You would have to initialize the log factory (with `logrin::LogFactory::Init`) for any
    # logs to appear.
    "logrin",

    # ## `--@violet.net//bazel/flags:ubsan=[True|False]`
    # Enables the Undefined Behaviour Sanitizer that is used to catch
    # undefined behaviour mistakes. Because, you know, I am not the
    # sharpest tool in the shed.
    "ubsan",

    # ## `--@violet.net//bazel/flags:tsan=[True|False]`
    # Enables the Thread Sanitizer that is used to catch thread-like mistakes. Because, you know,
    # I am not the sharpest tool in the shed.
    "tsan",

    # ## `--@violet.net//bazel/flags:msan=[True|False]`
    # Enables the Memory Sanitizer that is used to catch memory mistakes. Because, you know,
    # I am not the sharpest tool in the shed.
    "msan",

    # ## `--@violet.net//bazel/flags:asan=[True|False]`
    # Enables the Address Sanitizer that is used to catch mistakes. Because, you know,
    # I am not the sharpest tool in the shed.
    "asan",
]
