#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <source_location>
#include <string_view>

namespace corosim::detail {

[[noreturn]] inline void assert_fail(std::string_view expr,
                                     std::string_view msg,
                                     std::source_location loc = std::source_location::current()) {
    std::fprintf(stderr, "[COROSIM ASSERT] %s:%d:%d: '%s' failed",
                 loc.file_name(), loc.line(), loc.column(), expr.data());
    if (!msg.empty())
        std::fprintf(stderr, " -- %s", msg.data());
    std::fputc('\n', stderr);
    std::abort();
}

} // namespace corosim::detail

#define COROSIM_ASSERT(expr, msg)                                          \
    do {                                                                   \
        if (!(expr))                                                       \
            ::corosim::detail::assert_fail(#expr, msg);                    \
    } while (false)

#define COROSIM_CHECK(expr)                                                \
    COROSIM_ASSERT(expr, std::string_view{})

#define COROSIM_FAIL(msg)                                                  \
    do {                                                                   \
        ::corosim::detail::assert_fail("", msg);                           \
    } while (false)
