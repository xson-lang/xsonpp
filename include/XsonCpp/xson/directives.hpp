#pragma once
#include <array>
#include <string_view>

namespace xson {
    constexpr std::array<std::string_view, 2> directives{{ 
        "ext", 
        "extset"
    }};
}