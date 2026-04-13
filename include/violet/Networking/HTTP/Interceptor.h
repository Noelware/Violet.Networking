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
//! # 🌺💜 `violet/Networking/HTTP/Interceptor.h`

#include <violet/Networking/HTTP/Error.h>

namespace violet::net::http {

struct Request;
struct Response;

struct Chain final {
    VIOLET_DISALLOW_CONSTRUCTOR(Chain);
    VIOLET_DISALLOW_COPY(Chain);
    VIOLET_IMPLICIT_MOVE(Chain);
    ~Chain() = default;

    VIOLET_IMPLICIT Chain(std::function<Result<Response, Error>(Request&&)> next)
        : n_next(VIOLET_MOVE(next))
    {
    }

    auto Next(Request&& request) const -> Result<Response, Error>;

private:
    std::function<Result<Response, Error>(Request&&)> n_next;
};

struct Interceptor {
    virtual ~Interceptor() = default;

    [[nodiscard]] virtual auto Name() const noexcept -> String = 0;
    virtual auto Intercept(Request&& request, Chain chain) noexcept -> Result<Response, String> = 0;
};

} // namespace violet::net::http
