#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace mb
{
    struct string_hash
    {
        using is_transparent = void;

        size_t operator()(const char* txt) const
        {
            return std::hash<std::string_view>{}(txt);
        }

        size_t operator()(std::string_view txt) const
        {
            return std::hash<std::string_view>{}(txt);
        }

        size_t operator()(const std::string& txt) const
        {
            return std::hash<std::string>{}(txt);
        }
    };

    template<typename Value>
    using unordered_string_map = std::unordered_map<std::string, Value, string_hash, std::equal_to<>>;
}