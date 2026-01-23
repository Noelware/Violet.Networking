The `rust/` directory contains FFI bindings that is used to not reinvent stuff myself because that would be too much work.

- `violet_net_url` (`rust/url`): Ports the [`url`] Rust crate to C (`#include <violet-c/net/url.h>`) and C++ (`#include <violet/Networking/URL.h>`).

[`url`]: https://docs.rs/url
