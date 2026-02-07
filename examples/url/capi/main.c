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

#include <violet-c/net/url.h>

#include <stdio.h>
#include <stdlib.h>

const char* str_view_as_cstr(violet_net_string_view data)
{
    return data.data;
}

const char* opt_str_view_as_cstr(violet_net_optional_string_view opt)
{
    if (!opt.something) {
        return "<nothing>";
    }

    return str_view_as_cstr(opt.data);
}

void print_url(const violet_net_url_t* url)
{
    violet_net_optional_uint16 port = violet_net_url_port(url);
    violet_net_optional_uint16 port_or_known = violet_net_url_port_or_known_default(url);

    printf("=== URL %s ===\n", str_view_as_cstr(violet_net_url_as_str(url)));
    printf("Scheme:        %s\n", str_view_as_cstr(violet_net_url_scheme(url)));
    printf("Special?:      %s\n", violet_net_url_is_special(url) ? "yes" : "no");
    printf("Has Authority: %s\n", violet_net_url_has_authority(url) ? "yes" : "no");
    printf("Authority:     %s\n", str_view_as_cstr(violet_net_url_authority(url)));
    printf("Username:      %s\n", opt_str_view_as_cstr(violet_net_url_username(url)));
    printf("Password:      %s\n", opt_str_view_as_cstr(violet_net_url_password(url)));
    printf("Port:          %d (present: %s)\n", port.data, port.something ? "yes" : "no");
    printf("Port Or Known: %d (present: %s)\n", port_or_known.data, port_or_known.something ? "yes" : "no");
    printf("Has Host?:     %s\n", violet_net_url_has_host(url) ? "yes" : "no");
    printf("Host:          %s\n", opt_str_view_as_cstr(violet_net_url_host(url)));
    printf("Domain:        %s\n", opt_str_view_as_cstr(violet_net_url_domain(url)));
    printf("Path:          %s\n", str_view_as_cstr(violet_net_url_path(url)));
    printf("Query:         %s\n", opt_str_view_as_cstr(violet_net_url_query(url)));
    printf("Fragment:      %s\n", opt_str_view_as_cstr(violet_net_url_fragment(url)));
}

int main(int argc, char** argv)
{
    const char* url = "https://noelware.org";
    if (argc > 1) {
        url = argv[1];
    }

    violet_net_url_error_t error = URL_UNKNOWN;
    violet_net_url_t* uri = violet_net_url_new(url, &error);
    if (error != URL_OK) {
        fprintf(stderr, "failed to parse url `%s`: %s", url, violet_net_url_strerror(error));
        return 1;
    }

    print_url(uri);

    const char* params[] = { "hello=world", "weow=fluff" };
    violet_net_url_t* url_with_params = violet_net_url_new_with_params(url, params, 2, &error);
    if (error != URL_OK) {
        fprintf(stderr, "failed to parse url `%s`: %s", url, violet_net_url_strerror(error));
        return 1;
    }

    print_url(url_with_params);

    free(uri);
    free(url_with_params);

    return 0;
}
