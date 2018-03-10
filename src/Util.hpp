//
// Created by mafn on 1/2/18.
//

#ifndef HONK_UTIL_HPP
#define HONK_UTIL_HPP

#include <string>
#include <optional>
#include <unordered_map>
#include <algorithm>

namespace Honk
{
    // (annotative) type attributes
    #define __maybe_nullptr
    #define __owning
    #define __non_owning

    namespace Util
    {
        std::string get_file_contents(const std::string& path);

        template<typename Map_K, typename Map_V>
        std::optional<Map_V> map_get_optional(const std::unordered_map<Map_K, Map_V>& map, Map_K key)
        {
            if (map.find(key) == map.cend()) {
                return std::nullopt;
            }

            return map.at(key);
        };

        template <typename Map_K, typename Map_V>
        __maybe_nullptr __non_owning Map_V* map_get_ptr(std::unordered_map<Map_K, Map_V>& map, Map_K key)
        {
            if (map.find(key) == map.cend()) {
                return nullptr;
            }

            return &map.at(key);
        };

        template <typename Container, typename Value>
        bool contains(const Container& container, const Value& val)
        {
            return std::find(container.begin(), container.end(), val) != container.end();
        };

        inline const char* bool_str(bool val)
        {
            if (val) {
                return "true";
            } else {
                return "false";
            }
        }
    }
}

#endif //HONK_UTIL_HPP
