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

#![allow(non_camel_case_types, non_upper_case_globals)]

// TODO(@auguwu/Noel): have a backport of `std::net::{Ipv4Addr, Ipv6Addr}` -> `violet::net::ip::{V4|V6}Addr`
// in `violet_net_url_host`.

use core::{
    ffi::{CStr, c_char},
    ptr::{null, null_mut},
};
use std::{ffi::CString, fmt::Write};
use url::{ParseError, Url};

macro_rules! map_error_ptr {
    ($ptr:ident, $variant:ident) => {
        if !($ptr).is_null() {
            unsafe {
                (*$ptr) = $crate::violet_net_url_error_t::$variant;
            }
        }
    };

    ($ptr:ident, $expr:expr) => {
        if !($ptr).is_null() {
            unsafe {
                (*$ptr) = $expr;
            }
        }
    };
}

#[repr(C)]
pub struct violet_net_url_t(Url);
impl violet_net_url_t {
    #[allow(clippy::missing_safety_doc)]
    pub unsafe fn new(input: *const c_char, error: *mut violet_net_url_error_t) -> *mut violet_net_url_t {
        if input.is_null() {
            map_error_ptr!(error, URL_EMPTY);
            return null_mut();
        }

        // Safety: the validity of `input` is checked
        let c_str = unsafe { CStr::from_ptr(input) };
        let str = match c_str.to_str() {
            Ok(str) => str,
            Err(_) => {
                map_error_ptr!(error, URL_NOT_UNICODE);
                return null_mut();
            }
        };

        match Url::parse(str) {
            Ok(url) => {
                map_error_ptr!(error, URL_OK);
                Box::into_raw(Box::new(violet_net_url_t(url)))
            }

            Err(err) => {
                map_error_ptr!(error, violet_net_url_error_t::map(err));
                null_mut()
            }
        }
    }

    #[allow(clippy::missing_safety_doc)]
    pub unsafe fn new_with_params(
        input: *const c_char,
        query: *const *const c_char,
        len: usize,
        error: *mut violet_net_url_error_t,
    ) -> *mut Self {
        if input.is_null() || query.is_null() {
            map_error_ptr!(error, URL_EMPTY);
            return null_mut();
        }

        let input = match (unsafe { CStr::from_ptr(input) }).to_str() {
            Ok(str) => str,
            Err(_) => {
                map_error_ptr!(error, URL_NOT_UNICODE);
                return null_mut();
            }
        };

        let mut pairs: Vec<(&str, &str)> = Vec::new();
        for pair in 0..len {
            let c_str = match (unsafe { CStr::from_ptr(*query.add(pair)) }).to_str() {
                Ok(str) => str,
                Err(_) => {
                    map_error_ptr!(error, URL_NOT_UNICODE);
                    return null_mut();
                }
            };

            let pair = c_str.split_once('=');
            let (key, value) = match pair {
                Some((key, value)) => {
                    if value.contains('=') {
                        map_error_ptr!(error, URL_EXTRA_CHAR_IN_PARAM_STRING);
                        return null_mut();
                    }

                    (key, value)
                }
                None => {
                    map_error_ptr!(error, URL_INVALID_PARAM_STRING);
                    return null_mut();
                }
            };

            pairs.push((key, value));
        }

        match Url::parse_with_params(input, pairs) {
            Ok(url) => {
                map_error_ptr!(error, URL_OK);
                Box::into_raw(Box::new(violet_net_url_t(url)))
            }

            Err(err) => {
                map_error_ptr!(error, violet_net_url_error_t::map(err));
                null_mut()
            }
        }
    }
}

#[derive(Clone, Copy)]
#[repr(C)]
#[non_exhaustive]
pub enum violet_net_url_error_t {
    URL_OK = 0,
    URL_EMPTY,
    URL_NOT_UNICODE,
    URL_INVALID_PARAM_STRING,
    URL_EXTRA_CHAR_IN_PARAM_STRING,
    URL_EMPTY_HOST,
    URL_IDNA_ERROR,
    URL_INVALID_PORT,
    URL_INVALID_IPV4_ADDRESS,
    URL_INVALID_IPV6_ADDRESS,
    URL_INVALID_DOMAIN_CHARACTER,
    URL_RELATIVE_URL_WITHOUT_BASE,
    URL_RELATIVE_URL_WITH_CANNOT_BE_A_BASE_BASE,
    URL_SET_HOST_ON_CANNOT_BE_A_BASE_URL,
    URL_OVERFLOW,
    URL_UNKNOWN = -1,
}

impl violet_net_url_error_t {
    pub const fn map(error: ParseError) -> violet_net_url_error_t {
        match error {
            ParseError::EmptyHost => violet_net_url_error_t::URL_EMPTY_HOST,
            ParseError::IdnaError => violet_net_url_error_t::URL_IDNA_ERROR,
            ParseError::InvalidPort => violet_net_url_error_t::URL_INVALID_PORT,
            ParseError::InvalidIpv4Address => violet_net_url_error_t::URL_INVALID_IPV4_ADDRESS,
            ParseError::InvalidIpv6Address => violet_net_url_error_t::URL_INVALID_IPV6_ADDRESS,
            ParseError::InvalidDomainCharacter => violet_net_url_error_t::URL_INVALID_DOMAIN_CHARACTER,
            ParseError::RelativeUrlWithoutBase => violet_net_url_error_t::URL_RELATIVE_URL_WITHOUT_BASE,
            ParseError::RelativeUrlWithCannotBeABaseBase => {
                violet_net_url_error_t::URL_RELATIVE_URL_WITH_CANNOT_BE_A_BASE_BASE
            }

            ParseError::SetHostOnCannotBeABaseUrl => violet_net_url_error_t::URL_SET_HOST_ON_CANNOT_BE_A_BASE_URL,
            ParseError::Overflow => violet_net_url_error_t::URL_OVERFLOW,
            _ => violet_net_url_error_t::URL_UNKNOWN,
        }
    }

    pub fn to_str(self) -> CString {
        unsafe {
            CString::new(match self {
                Self::URL_OK => "ok",
                Self::URL_EMPTY => "empty input provided",
                Self::URL_NOT_UNICODE => "input was not valid unicode",
                Self::URL_INVALID_PARAM_STRING => "`Url::parse_with_params()': parameter didn't have `=` sign",
                Self::URL_EXTRA_CHAR_IN_PARAM_STRING => {
                    "`Url::parse_with_params()': parameter with pair had extra `=`"
                }

                Self::URL_EMPTY_HOST => "empty host",
                Self::URL_IDNA_ERROR => "invalid international domain name",
                Self::URL_INVALID_PORT => "invalid port number",
                Self::URL_INVALID_IPV4_ADDRESS => "invalid IPv4 address",
                Self::URL_INVALID_IPV6_ADDRESS => "invalid IPv6 address",
                Self::URL_INVALID_DOMAIN_CHARACTER => "invalid domain character",
                Self::URL_RELATIVE_URL_WITHOUT_BASE => "relative URL without a base",
                Self::URL_RELATIVE_URL_WITH_CANNOT_BE_A_BASE_BASE => "relative URL with a cannot-be-a-base base",
                Self::URL_SET_HOST_ON_CANNOT_BE_A_BASE_URL => "a cannot-be-a-base URL doesn’t have a host to set",
                Self::URL_OVERFLOW => "URLs more than 4 GB are not supported",
                Self::URL_UNKNOWN => "unknown error: should never happen",
            })
            .unwrap_unchecked()
        }
    }
}

#[repr(C)]
pub struct violet_net_string_view {
    pub len: usize,
    pub data: *const u8,
}

impl violet_net_string_view {
    pub const empty: Self = Self { len: 0, data: null() };

    pub const fn from_str(s: &str) -> violet_net_string_view {
        violet_net_string_view {
            len: s.len(),
            data: s.as_ptr(),
        }
    }
}

#[repr(C)]
pub struct violet_net_optional_string_view {
    pub something: bool,
    pub data: violet_net_string_view,
}

impl violet_net_optional_string_view {
    pub const nothing: Self = Self {
        something: false,
        data: violet_net_string_view::empty,
    };

    pub const fn from_opt(s: Option<&str>) -> violet_net_optional_string_view {
        match s {
            Some(x) => Self::some(x),
            None => Self::nothing,
        }
    }

    pub const fn some(s: &str) -> violet_net_optional_string_view {
        violet_net_optional_string_view {
            something: true,
            data: violet_net_string_view::from_str(s),
        }
    }
}

#[repr(C)]
pub struct violet_net_optional_uint16 {
    pub something: bool,
    pub value: u16,
}

impl violet_net_optional_uint16 {
    pub const nothing: Self = Self {
        something: false,
        value: u16::MAX,
    };

    pub const fn from_opt(opt: Option<u16>) -> Self {
        match opt {
            Some(x) => Self::from_u16(x),
            None => Self::nothing,
        }
    }

    pub const fn from_u16(x: u16) -> Self {
        Self {
            something: true,
            value: x,
        }
    }
}

macro_rules! define_ffi {
    (
        $(#[$meta:meta])*
        $vis:vis fn $name:ident($($argname:ident: $argty:ty),*) $(-> $returnty:ty)? $code:block
    ) => {
        $(#[$meta])*
        #[unsafe(no_mangle)]
        #[allow(clippy::missing_safety_doc)]
        $vis unsafe extern "C" fn $name($($argname: $argty),*) $(-> $returnty)? $code
    };
}

macro_rules! handle_nullptr {
    ($ptr:ident => $ret:expr) => {
        if ($ptr).is_null() {
            return $ret;
        }
    };
}

define_ffi! {
    pub fn violet_net_url_new(
        input: *const c_char,
        error: *mut violet_net_url_error_t
    ) -> *mut violet_net_url_t {
        unsafe { violet_net_url_t::new(input, error) }
    }
}

define_ffi! {
    pub fn violet_net_url_new_with_params(
        input: *const c_char,
        params: *const *const c_char,
        len: usize,
        error: *mut violet_net_url_error_t
    ) -> *mut violet_net_url_t {
        unsafe { violet_net_url_t::new_with_params(input, params, len, error) }
    }
}

define_ffi! {
    pub fn violet_net_url_join(handle: *const violet_net_url_t, input: *const c_char, error: *mut violet_net_url_error_t) -> *mut violet_net_url_t {
        if input.is_null() {
            map_error_ptr!(error, URL_EMPTY);
            return null_mut();
        }

        let input = match (unsafe { CStr::from_ptr(input) }).to_str() {
            Ok(str) => str,
            Err(_) => {
                map_error_ptr!(error, URL_NOT_UNICODE);
                return null_mut();
            }
        };

        match unsafe { &(*handle).0 }.join(input) {
            Ok(url) => {
                map_error_ptr!(error, URL_OK);
                Box::into_raw(Box::new(violet_net_url_t(url)))
            }

            Err(err) => {
                map_error_ptr!(error, violet_net_url_error_t::map(err));
                null_mut()
            }
        }
    }
}

define_ffi! {
    pub fn violet_net_url_free(handle: *mut violet_net_url_t) {
        if !handle.is_null() {
            unsafe { drop(Box::from_raw(handle)) }
        }
    }
}

define_ffi! {
    pub fn violet_net_url_as_str(handle: *const violet_net_url_t) -> violet_net_string_view {
        handle_nullptr!(handle => violet_net_string_view::empty);
        violet_net_string_view::from_str(unsafe { &(*handle).0 }.as_str())
    }
}

define_ffi! {
    pub fn violet_net_url_scheme(handle: *const violet_net_url_t) -> violet_net_string_view {
        handle_nullptr!(handle => violet_net_string_view::empty);
        violet_net_string_view::from_str(unsafe { &(*handle).0 }.scheme())
    }
}

define_ffi! {
    pub fn violet_net_url_is_special(handle: *const violet_net_url_t) -> bool {
        handle_nullptr!(handle => false);
        unsafe { &(*handle).0 }.is_special()
    }
}

define_ffi! {
    pub fn violet_net_url_has_authority(handle: *const violet_net_url_t) -> bool {
        handle_nullptr!(handle => false);
        unsafe { &(*handle).0 }.has_authority()
    }
}

define_ffi! {
    pub fn violet_net_url_authority(url: *const violet_net_url_t) -> violet_net_string_view {
        handle_nullptr!(url => violet_net_string_view::empty);
        violet_net_string_view::from_str(unsafe { &(*url).0 }.authority())
    }
}

define_ffi! {
    pub fn violet_net_url_username(url: *const violet_net_url_t) -> violet_net_optional_string_view {
        handle_nullptr!(url => violet_net_optional_string_view::nothing);

        let username = unsafe { &(*url).0 }.username();
        violet_net_optional_string_view::from_opt(match username.is_empty() {
            true => None,
            false => Some(username)
        })
    }
}

define_ffi! {
    pub fn violet_net_url_password(url: *const violet_net_url_t) -> violet_net_optional_string_view {
        handle_nullptr!(url => violet_net_optional_string_view::nothing);
        violet_net_optional_string_view::from_opt(unsafe { &(*url).0 }.password())
    }
}

define_ffi! {
    pub fn violet_net_url_port(url: *const violet_net_url_t) -> violet_net_optional_uint16 {
        handle_nullptr!(url => violet_net_optional_uint16::nothing);
        violet_net_optional_uint16::from_opt(unsafe { &(*url).0 }.port())
    }
}

define_ffi! {
    pub fn violet_net_url_port_or_known_default(url: *const violet_net_url_t) -> violet_net_optional_uint16 {
        handle_nullptr!(url => violet_net_optional_uint16::nothing);
        violet_net_optional_uint16::from_opt(unsafe { &(*url).0 }.port_or_known_default())
    }
}

define_ffi! {
    pub fn violet_net_url_has_host(handle: *const violet_net_url_t) -> bool {
        handle_nullptr!(handle => false);
        unsafe { &(*handle).0 }.has_host()
    }
}

const IPV6_MAX_LEN: usize = "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff".len();

define_ffi! {
    pub fn violet_net_url_host(url: *const violet_net_url_t) -> violet_net_optional_string_view {
        handle_nullptr!(url => violet_net_optional_string_view::nothing);
        match unsafe { &(*url).0 }.host() {
            Some(url::Host::Domain(domain)) => violet_net_optional_string_view::some(domain),
            Some(url::Host::Ipv4(addr)) => {
                let c_str = unsafe { CString::new(addr.to_string()).unwrap_unchecked() };
                let ptr = c_str.into_raw();

                violet_net_optional_string_view {
                    something: true,
                    data: violet_net_string_view {
                        data: ptr as *const u8,
                        len: unsafe { libc::strlen(ptr) },
                    }
                }
            }

            Some(url::Host::Ipv6(addr)) => {
                let mut buf = [0u8; IPV6_MAX_LEN];
                let mut s = String::new();
                write!(&mut s, "{addr}").unwrap();

                let bytes = s.as_bytes();
                let len = bytes.len().min(buf.len());
                buf[..len].copy_from_slice(&bytes[..len]);

                violet_net_optional_string_view {
                    something: true,
                    data: violet_net_string_view {
                        data: buf.as_ptr(),
                        len
                    }
                }
            }

            None => violet_net_optional_string_view::nothing,
        }
    }
}

define_ffi! {
    pub fn violet_net_url_domain(url: *const violet_net_url_t) -> violet_net_optional_string_view {
        handle_nullptr!(url => violet_net_optional_string_view::nothing);
        violet_net_optional_string_view::from_opt(unsafe { &(*url).0 }.domain())
    }
}

define_ffi! {
    pub fn violet_net_url_path(url: *const violet_net_url_t) -> violet_net_string_view {
        handle_nullptr!(url => violet_net_string_view::empty);
        violet_net_string_view::from_str(unsafe { &(*url).0 }.path())
    }
}

define_ffi! {
    pub fn violet_net_url_query(url: *const violet_net_url_t) -> violet_net_optional_string_view {
        handle_nullptr!(url => violet_net_optional_string_view::nothing);
        violet_net_optional_string_view::from_opt(unsafe { &(*url).0 }.query())
    }
}

define_ffi! {
    pub fn violet_net_url_fragment(url: *const violet_net_url_t) -> violet_net_optional_string_view {
        handle_nullptr!(url => violet_net_optional_string_view::nothing);
        violet_net_optional_string_view::from_opt(unsafe { &(*url).0 }.fragment())
    }
}

define_ffi! {
    pub fn violet_net_url_strerror(error: violet_net_url_error_t) -> *mut c_char {
        let c_str = error.to_str();
        c_str.into_raw()
    }
}
