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
//! # 🌺💜 `violet/Networking/HTTP/Error.h`

#include <violet/Experimental/OneOf.h>
#include <violet/Networking/HTTP/Headers.h>
#include <violet/Networking/URL.h>
#include <violet/Violet.h>

#if VIOLET_USE_RTTI
#include <violet/Support/Demangle.h>
#endif

#include <chrono>
#include <sstream>

namespace violet::net::http {

struct RequestError final {
    enum struct Kind : UInt8 {
        InvalidURL,
        InvalidHeaderName,
        InvalidHeaderValue
    };

    static auto InvalidURL(UrlError error) noexcept -> RequestError;
    static auto InvalidHeaderName(struct InvalidHeaderName error) noexcept -> RequestError;
    static auto InvalidHeaderValue(struct InvalidHeaderValue error) noexcept -> RequestError;

    [[nodiscard]] auto Kind() const noexcept -> enum Kind;
    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const RequestError& self) noexcept -> std::ostream&
    {
        return os << "request error: " << self.ToString();
    }

private:
    template<typename T>
        requires(std::same_as<std::decay_t<T>, UrlError> || std::same_as<std::decay_t<T>, struct InvalidHeaderName>
                    || std::same_as<std::decay_t<T>, struct InvalidHeaderValue>)
    VIOLET_EXPLICIT RequestError(enum Kind kind, T error)
        : n_kind(kind)
        , n_payload(error)
    {
    }

    enum Kind n_kind;
    union {
        struct UrlError UrlError;
        struct InvalidHeaderName HeaderName;
        struct InvalidHeaderValue InvalidHeaderValue;
    } n_payload;
};

struct ResponseError final {
    VIOLET_DISALLOW_CONSTRUCTOR(ResponseError);
    VIOLET_DISALLOW_COPY(ResponseError);
    VIOLET_IMPLICIT_MOVE(ResponseError);
    ~ResponseError() = default;

    UInt16 StatusCode;
    Vec<UInt8> Body;

    VIOLET_EXPLICIT ResponseError(UInt16 status, Vec<UInt8>&& body) noexcept
        : StatusCode(status)
        , Body(VIOLET_MOVE(body))
    {
    }

    [[nodiscard]] constexpr auto ServerError() const noexcept -> bool
    {
        return this->StatusCode >= 500;
    }

    [[nodiscard]] constexpr auto ClientError() const noexcept -> bool
    {
        return this->StatusCode >= 400 && this->StatusCode < 500;
    }

    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const ResponseError& self) noexcept -> std::ostream&
    {
        return os << "response error: " << self.ToString();
    }
};

struct TimedOutError final {
    VIOLET_DISALLOW_CONSTRUCTOR(TimedOutError);
    VIOLET_DISALLOW_COPY(TimedOutError);
    VIOLET_IMPLICIT_MOVE(TimedOutError);
    ~TimedOutError() = default;

    std::chrono::milliseconds After;

    VIOLET_EXPLICIT TimedOutError(std::chrono::milliseconds after)
        : After(after)
    {
    }

    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const TimedOutError& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }
};

struct InterceptorAbortedError final {
    VIOLET_DISALLOW_CONSTRUCTOR(InterceptorAbortedError);
    VIOLET_DISALLOW_COPY(InterceptorAbortedError);
    VIOLET_IMPLICIT_MOVE(InterceptorAbortedError);
    ~InterceptorAbortedError() = default;

    Str Interceptor;
    String Message;

    VIOLET_EXPLICIT InterceptorAbortedError(Str interceptor, Str message)
        : Interceptor(interceptor)
        , Message(message)
    {
    }

    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const InterceptorAbortedError& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }
};

struct TooManyRedirectsError final {
    VIOLET_DISALLOW_CONSTRUCTOR(TooManyRedirectsError);
    VIOLET_DISALLOW_COPY(TooManyRedirectsError);
    VIOLET_IMPLICIT_MOVE(TooManyRedirectsError);
    ~TooManyRedirectsError() = default;

    UInt32 Limit;

    VIOLET_EXPLICIT TooManyRedirectsError(UInt32 limit)
        : Limit(limit)
    {
    }

    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const TooManyRedirectsError& self) noexcept -> std::ostream&
    {
        return os << self.ToString();
    }
};

struct DriverError final {
    VIOLET_DISALLOW_CONSTRUCTOR(DriverError);
    VIOLET_DISALLOW_COPY(DriverError);
    ~DriverError();

    template<typename T, typename... Args>
        requires(std::is_constructible_v<T, Args...>)
    VIOLET_IMPLICIT DriverError(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : n_erased(new T(VIOLET_FWD(Args, args)...))
        , n_vtable(vtable_t::For<T>())
    {
    }

    VIOLET_IMPLICIT DriverError(DriverError&& other) noexcept;
    auto operator=(DriverError&& other) noexcept -> DriverError&;

    [[nodiscard]] auto ToString() const noexcept -> String;
    friend auto operator<<(std::ostream& os, const DriverError& self) noexcept -> std::ostream&
    {
        os << "driver error: " << self.ToString();
        return os;
    }

private:
    struct vtable_t final {
        template<typename T>
        constexpr static auto For() noexcept -> vtable_t;

        auto (*Message)(const void* self) noexcept -> String;
        void (*Destruct)(void* self) noexcept;
    };

    const void* n_erased = nullptr;
    vtable_t* n_vtable = nullptr;
};

template<typename T>
constexpr auto DriverError::vtable_t::For() noexcept -> DriverError::vtable_t
{
    return DriverError::vtable_t{
        // clang-format off
        .Message = [](const void* ptr) -> String {
            const auto* self = static_cast<const T*>(ptr);
            VIOLET_DEBUG_ASSERT0(self != nullptr);

            if constexpr (Stringify<T>) {
                return violet::ToString(*self);
            }

            if constexpr (requires(std::ostream& os) { os << *self; }) {
                std::ostringstream os;
                os << *self;

                return os.str();
            }

#if VIOLET_USE_RTTI
                const auto& type = typeid(T);
                return std::format("<type {}@{}>", util::DemangleCXXName(type.name()), type.hash_code());
#else
            return "<unable to stringify>";
#endif
        },

        .Destruct = [](void* ptr) -> void {
            T* self = static_cast<T*>(ptr);
            std::destroy_at(self);
            ::operator delete(self);
        }
        // clang-format on
    };
}

using Error = experimental::OneOf<DriverError, RequestError, ResponseError, TimedOutError, TooManyRedirectsError,
    InterceptorAbortedError>;

} // namespace violet::net::http

VIOLET_TO_STRING(
    violet::net::http::Error, error, { return error.Visit([](const auto& err) -> String { return err.ToString(); }); });
