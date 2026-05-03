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

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")
load("//bazel:cc/defs.bzl", "sanitizer", violet_copts = "copts", violet_defines = "defines")
load(":version.bzl", "DEVBUILD", "encode_as_int")

def violet_cc_library(
        name,
        deps = [],
        local_defines = [],
        defines = [],
        copts = [],
        linkopts = [],
        **kwargs):
    if "includes" in kwargs:
        fail("`includes` in `violet_cc_library`(%s) is not allowed" % name)

    return cc_library(
        name = name,
        deps = ["//:include_hack"] + deps,
        copts = violet_copts + sanitizer["copts"] + copts,
        linkopts = sanitizer["linkopts"] + linkopts,
        local_defines = local_defines + ["VIOLET_NET_BUILDING"],
        defines = violet_defines + defines + [
            ("VIOLET_NET_VERSION=%d" % encode_as_int()),
            ("VIOLET_NET_DEVBUILD=%d" % (1 if DEVBUILD else 0)),
        ],
        **kwargs
    )

def violet_cc_test(
        name,
        with_gtest_main = True,
        deps = [],
        copts = [],
        linkopts = [],
        env = {},
        size = "small",
        **kwargs):
    if "visibility" in kwargs:
        fail("`visibility` in `violet_cc_test`(%s) is not allowed" % name)

    return cc_test(
        name = name,
        deps = deps + ["@googletest//:gtest"] + (["@googletest//:gtest_main"] if with_gtest_main else []),
        copts = copts + sanitizer["copts"],
        linkopts = linkopts + sanitizer["linkopts"],
        env = env | sanitizer["env"],
        visibility = ["//visibility:public"],
        size = size,
        **kwargs
    )
